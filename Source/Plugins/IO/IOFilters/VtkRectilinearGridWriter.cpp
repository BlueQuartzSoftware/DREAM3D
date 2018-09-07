/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "VtkRectilinearGridWriter.h"

#include <sstream>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "SIMPLib/Common/ScopedFileMonitor.hpp"
#include "SIMPLib/Common/TemplateHelpers.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/VTKUtils/VTKUtil.hpp"

#include "IO/IOConstants.h"
#include "IO/IOVersion.h"

#define LD_CAST(arg) static_cast<long int>(arg)
namespace Detail
{

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <class Geometry> void WriteVTKHeader(FILE* f, DataContainer::Pointer m, bool isBinary)
{

  size_t xpoints = m->getGeometryAs<Geometry>()->getXPoints() + 1;
  size_t ypoints = m->getGeometryAs<Geometry>()->getYPoints() + 1;
  size_t zpoints = m->getGeometryAs<Geometry>()->getZPoints() + 1;

  fprintf(f, "# vtk DataFile Version 2.0\n");
  fprintf(f, "Data set from %s\n", IO::Version::PackageComplete().toLatin1().constData());
  if(isBinary)
  {
    fprintf(f, "BINARY\n");
  }
  else
  {
    fprintf(f, "ASCII\n");
  }
  fprintf(f, "\n");
  fprintf(f, "DATASET RECTILINEAR_GRID\n");
  fprintf(f, "DIMENSIONS %ld %ld %ld\n", LD_CAST(xpoints), LD_CAST(ypoints), LD_CAST(zpoints) );
}

/**
      * @brief This function writes a set of Axis coordinates to that are needed
      * for a Rectilinear Grid based data set.
      * @param f The "C" FILE* pointer to the file being written to.
      * @param axis The name of the Axis that is being written
      * @param type The type of primitive being written (float, int, ...)
      * @param npoints The total number of points in the array
      * @param min The minimum value of the axis
      * @param max The maximum value of the axis
      * @param step The step value between each point on the axis.
      */
template <typename T> int WriteCoords(FILE* f, const char* axis, const char* type, int64_t npoints, T min, T max, T step, bool binary)
{
  int err = 0;
#if CMP_SIZEOF_LONG == 8 && !defined(__APPLE__)
  fprintf(f, "%s %ld %s\n", axis, npoints, type);
#else
  fprintf(f, "%s %lld %s\n", axis, npoints, type);
#endif
  if(binary == true)
  {
    T* data = new T[npoints];
    T d;
    for(int idx = 0; idx < npoints; ++idx)
    {
      d = idx * step + min;
      SIMPLib::Endian::FromSystemToBig::convert(d);
      data[idx] = d;
    }
    size_t totalWritten = fwrite(static_cast<void*>(data), sizeof(T), static_cast<size_t>(npoints), f);
    fprintf(f, "\n"); // Write a newline character at the end of the coordinates
    delete[] data;
    if(totalWritten != static_cast<size_t>(npoints))
    {
      qDebug() << "Error Writing Binary VTK Data into file ";
      fclose(f);
      return -1;
    }
  }
  else
  {
    T d;
    for(int idx = 0; idx < npoints; ++idx)
    {
      d = idx * step + min;
      fprintf(f, "%f ", d);
      if(idx % 20 == 0 && idx != 0)
      {
        fprintf(f, "\n");
      }
    }
    fprintf(f, "\n");
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> void WriteDataArray(AbstractFilter* filter, FILE* f, IDataArray::Pointer iDataPtr, bool writeBinary)
{
  QString ss = QObject::tr("Writing Cell Data %1").arg(iDataPtr->getName());
  filter->notifyStatusMessage(filter->getMessagePrefix(), filter->getHumanLabel(), ss);
  // qDebug() << "Writing DataArray " << iDataPtr->getName() << " To a VTK File";

  typedef DataArray<T> ArrayType;

  typename ArrayType::Pointer array = std::dynamic_pointer_cast<ArrayType>(iDataPtr);

  if(nullptr != array.get())
  {
    size_t totalElements = array->getSize();
    T* val = array->getPointer(0);
    int numComps = array->getNumberOfComponents();
    QString dName = array->getName();
    dName = dName.replace(" ", "_");

    QString vtkTypeString = VTKUtil::TypeForPrimitive<T>(val[0]);
    bool useIntCast = false;
    if(vtkTypeString.compare("unsigned_char") == 0 || vtkTypeString.compare("char") == 0)
    {
      useIntCast = true;
    }

    fprintf(f, "SCALARS %s %s %d\n", dName.toLatin1().data(), vtkTypeString.toLatin1().data(), numComps);
    fprintf(f, "LOOKUP_TABLE default\n");
    if(writeBinary)
    {
      if(BIGENDIAN == 0)
      {
        array->byteSwapElements();
      }
      size_t totalWritten = fwrite(val, array->getTypeSize(), totalElements, f);
      if(totalWritten != totalElements)
      {
      }
      fprintf(f, "\n");
      if(BIGENDIAN == 0)
      {
        array->byteSwapElements();
      }
    }
    else
    {
      std::stringstream ss;
      for(size_t i = 0; i < totalElements; i++)
      {
        if(i % 20 == 0 && i > 0)
        {
          ss << "\n";
        }
        if(useIntCast)
        {
          ss << " " << static_cast<int>(val[i]);
        }
        else
        {
          ss << " " << val[i];
        }
      }
      ss << "\n";
      fprintf(f, "%s", ss.str().c_str());
    }
  }
}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkRectilinearGridWriter::VtkRectilinearGridWriter()
: m_OutputFile("")
, m_WriteBinaryFile(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkRectilinearGridWriter::~VtkRectilinearGridWriter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output File", OutputFile, FilterParameter::Parameter, VtkRectilinearGridWriter, "*.vtk", "VTK Rectilinear Grid"));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Write Binary File", WriteBinaryFile, FilterParameter::Parameter, VtkRectilinearGridWriter));

  {
    MultiDataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Attribute Arrays to Write", SelectedDataArrayPaths, FilterParameter::RequiredArray, VtkRectilinearGridWriter, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputFile(reader->readString("OutputFile", getOutputFile()));
  setSelectedDataArrayPaths(reader->readDataArrayPathVector("SelectedDataArrayPaths", getSelectedDataArrayPaths()));
  setWriteBinaryFile(reader->readValue("WriteBinaryFile", false));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::initialize()
{
  m_SelectedWeakPtrVector.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  initialize();
  if(m_OutputFile.isEmpty() == true)
  {
    QString ss = QObject::tr("The output file must be set before executing this filter.");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  // Make sure what we are checking is an actual file name and not a directory
  QFileInfo fi(m_OutputFile);
  if(fi.isDir() == false)
  {
    QDir parentPath = fi.path();
    if(parentPath.exists() == false)
    {
      setWarningCondition(-11000);
      QString ss = QObject::tr("The directory path for the output file does not exist.");
      notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
    }
  }
  else
  {
    QString ss = QObject::tr("The output file path is a path to an existing directory. Please change the path to point to a file");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(m_SelectedDataArrayPaths.isEmpty() == true)
  {
    setErrorCondition(-11001);
    QString ss = QObject::tr("At least one Attribute Array must be selected");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<DataArrayPath> paths = getSelectedDataArrayPaths();

  if(DataArrayPath::ValidateVector(paths) == false)
  {
    setErrorCondition(-11004);
    QString ss = QObject::tr("There are Attribute Arrays selected that are not contained in the same Attribute Matrix. All selected Attribute Arrays must belong to the same Attribute Matrix");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  for(int32_t i = 0; i < paths.count(); i++)
  {
    DataArrayPath path = paths.at(i);
    IDataArray::WeakPointer ptr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, path);
    m_SelectedWeakPtrVector.push_back(ptr);
  }

  QString dcName = DataArrayPath::GetAttributeMatrixPath(getSelectedDataArrayPaths()).getDataContainerName();

  ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(dcName)->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || nullptr == image.get())
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(m_OutputFile);
  QString parentPath = fi.path();
  QDir dir;
  if(!dir.mkpath(parentPath))
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    setErrorCondition(-2031000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QString dcName = DataArrayPath::GetAttributeMatrixPath(getSelectedDataArrayPaths()).getDataContainerName();

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(dcName);

  ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
  size_t dims[3] = {0, 0, 0};
  std::tie(dims[0], dims[1], dims[2]) = image->getDimensions();
  float res[3] = {0.0f, 0.0f, 0.0f};
  image->getResolution(res);
  float origin[3] = {0.0f, 0.0f, 0.0f};
  image->getOrigin(origin);

  int err = 0;
  FILE* f = nullptr;
  f = fopen(getOutputFile().toLatin1().data(), "wb");
  ScopedFileMonitor fMon(f);
  if(nullptr == f)
  {
    QString ss = QObject::tr("Error opening output vtk file '%1'\n ").arg(m_OutputFile);
    setErrorCondition(-2031001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // write the header
  Detail::WriteVTKHeader<ImageGeom>(f, m, getWriteBinaryFile());

  // Write the Coordinate Points
  err = Detail::WriteCoords<float>(f, "X_COORDINATES", "float", dims[0] + 1, origin[0] - res[0] * 0.5f, (float)(dims[0] + 1 * res[0]), res[0], m_WriteBinaryFile);
  if(err < 0)
  {
    QString ss = QObject::tr("Error writing X Coordinates in vtk file %s'\n ").arg(m_OutputFile);
    setErrorCondition(-2031002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  err = Detail::WriteCoords<float>(f, "Y_COORDINATES", "float", dims[1] + 1, origin[1] - res[1] * 0.5f, (float)(dims[1] + 1 * res[1]), res[1], m_WriteBinaryFile);
  if(err < 0)
  {
    QString ss = QObject::tr("Error writing Y Coordinates in vtk file %s'\n ").arg(m_OutputFile);
    setErrorCondition(-2031002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  err = Detail::WriteCoords<float>(f, "Z_COORDINATES", "float", dims[2] + 1, origin[0] - res[2] * 0.5f, (float)(dims[2] + 1 * res[2]), res[2], m_WriteBinaryFile);
  if(err < 0)
  {
    QString ss = QObject::tr("Error writing Z Coordinates in vtk file %s'\n ").arg(m_OutputFile);
    setErrorCondition(-2031002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  int64_t totalCells = image->getXPoints() * image->getYPoints() * image->getZPoints();
  fprintf(f, "CELL_DATA %d\n", (int)totalCells);

  QVector<DataArrayPath> dataPaths = getSelectedDataArrayPaths();
  foreach(const DataArrayPath arrayPath, dataPaths)
  {
    IDataArray::Pointer iDataPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, arrayPath);

    EXECUTE_FUNCTION_TEMPLATE(this, Detail::WriteDataArray, iDataPtr, this, f, iDataPtr, m_WriteBinaryFile);

#if 0
    QString className = iDataPtr->getNameOfClass();
    if (className.startsWith("DataArray"))
    {
      QString ss = QObject::tr("Writing Cell Data %1").arg(iDataPtr->getName());
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      //qDebug() << "Writing DataArray " << iDataPtr->getName() << " To a VTK File";
      VTK_WRITE_RECTILINEAR_DATA(UInt8ArrayType, iDataPtr, "unsigned_char", quint8, "%d ");
      VTK_WRITE_RECTILINEAR_DATA(Int8ArrayType, iDataPtr, "char", int8_t, "%d ");
      VTK_WRITE_RECTILINEAR_DATA(UInt16ArrayType, iDataPtr, "unsigned_short", quint16, "%d ");
      VTK_WRITE_RECTILINEAR_DATA(Int16ArrayType, iDataPtr, "short", int16_t, "%d ");
      VTK_WRITE_RECTILINEAR_DATA(UInt32ArrayType, iDataPtr, "unsigned_int", quint32, "%d ");
      VTK_WRITE_RECTILINEAR_DATA(Int32ArrayType, iDataPtr, "int", int32_t, "%d ");
#if CMP_SIZEOF_LONG == 8 && !defined(__APPLE__)
      VTK_WRITE_RECTILINEAR_DATA(UInt64ArrayType, iDataPtr, "unsigned_long", uint64_t, "%lu ");
      VTK_WRITE_RECTILINEAR_DATA(Int64ArrayType, iDataPtr, "long", int64_t, "%ld ");
#else
      VTK_WRITE_RECTILINEAR_DATA(UInt64ArrayType, iDataPtr, "unsigned_long", quint64, "%llu ");
      VTK_WRITE_RECTILINEAR_DATA(Int64ArrayType, iDataPtr, "long", int64_t, "%lld ");
#endif

      VTK_WRITE_RECTILINEAR_DATA(FloatArrayType, iDataPtr, "float", float, "%f ");
      VTK_WRITE_RECTILINEAR_DATA(DoubleArrayType, iDataPtr, "double", double, "%f ");
    }
#endif
  }

  // The scopedFileMonitor will fclose() the FILE pointer for us

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VtkRectilinearGridWriter::newFilterInstance(bool copyFilterParameters) const
{
  VtkRectilinearGridWriter::Pointer filter = VtkRectilinearGridWriter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VtkRectilinearGridWriter::getCompiledLibraryName() const
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VtkRectilinearGridWriter::getBrandingString() const
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VtkRectilinearGridWriter::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << IO::Version::Major() << "." << IO::Version::Minor() << "." << IO::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VtkRectilinearGridWriter::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid VtkRectilinearGridWriter::getUuid()
{
  return QUuid("{a043bd66-2681-5126-82e1-5fdc46694bf4}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VtkRectilinearGridWriter::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VtkRectilinearGridWriter::getHumanLabel() const
{
  return "Vtk Rectilinear Grid Exporter";
}
