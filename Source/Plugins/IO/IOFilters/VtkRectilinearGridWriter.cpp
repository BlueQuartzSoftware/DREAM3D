/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "VtkRectilinearGridWriter.h"


#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/HKL/CtfFields.h"

#include "IO/IOConstants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkRectilinearGridWriter::VtkRectilinearGridWriter() :
  AbstractFilter(),
  m_OutputFile(""),
  m_WriteBinaryFile(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkRectilinearGridWriter::~VtkRectilinearGridWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FileSystemFilterParameter::New("Output File", "OutputFile", FilterParameterWidgetType::OutputFileWidget, getOutputFile(), false, "", "*.vtk", "VTK Rectilinear Grid"));
  parameters.push_back(FilterParameter::New("Cell AttributeMatrix to Write", "SelectedAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getSelectedAttributeMatrixPath(), false));
  parameters.push_back(FilterParameter::New("Write Binary File", "WriteBinaryFile", FilterParameterWidgetType::BooleanWidget, getWriteBinaryFile(), false));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputFile( reader->readString( "OutputFile", getOutputFile() ) );
  setSelectedAttributeMatrixPath( reader->readDataArrayPath("SelectedAttributeMatrixPath", getSelectedAttributeMatrixPath()) );
  setWriteBinaryFile( reader->readValue("WriteBinaryFile", false) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkRectilinearGridWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputFile)
  DREAM3D_FILTER_WRITE_PARAMETER(WriteBinaryFile)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VtkRectilinearGridWriter::dataCheck()
{
  setErrorCondition(0);

  if(m_OutputFile.isEmpty() == true)
  {
    QString ss = QObject::tr("The output file must be set before executing this filter.");
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
  }

  // Make sure what we are checking is an actual file name and not a directory
  QFileInfo fi(m_OutputFile);
  if (fi.isDir() == false)
  {
    QDir parentPath = fi.path();
    if (parentPath.exists() == false)
    {
      QString ss = QObject::tr("The directory path for the output file does not exist.");
      notifyWarningMessage(getHumanLabel(), ss, -1);
    }
  }
  else
  {
    QString ss = QObject::tr("The output file path is a path to an existing directory. Please change the path to point to a file");
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
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
  dataCheck();
  if(getErrorCondition() < 0) { return; }

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

  int err = write(m_OutputFile);

  if (err < 0)
  {
    QString ss = QObject::tr("Error writing output vtk file '%1'\n ").arg(m_OutputFile);
    setErrorCondition(-2031001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VtkRectilinearGridWriter::write(const QString& file)
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_SelectedAttributeMatrixPath.getDataContainerName());

  int err = 0;
  FILE* f = NULL;
  f = fopen(file.toLatin1().data(), "wb");
  if(NULL == f)
  {
    return -1;
  }
  // Write the correct header
  if(m_WriteBinaryFile == true)
  {
    WRITE_RECTILINEAR_GRID_HEADER("BINARY", m, m->getXPoints() + 1, m->getYPoints() + 1, m->getZPoints() + 1)
  }
  else
  {
    WRITE_RECTILINEAR_GRID_HEADER("ASCII", m, m->getXPoints() + 1, m->getYPoints() + 1, m->getZPoints() + 1)
  }

  // Write the Coordinate Points
  VtkRectilinearGridWriter::WriteCoords(f, "X_COORDINATES", "float", m->getXPoints() + 1, 0.0f - m->getXRes() * 0.5f, (float)(m->getXPoints() + 1 * m->getXRes()), m->getXRes(), m_WriteBinaryFile);
  VtkRectilinearGridWriter::WriteCoords(f, "Y_COORDINATES", "float", m->getYPoints() + 1, 0.0f - m->getYRes() * 0.5f, (float)(m->getYPoints() + 1 * m->getYRes()), m->getYRes(), m_WriteBinaryFile);
  VtkRectilinearGridWriter::WriteCoords(f, "Z_COORDINATES", "float", m->getZPoints() + 1, 0.0f - m->getZRes() * 0.5f, (float)(m->getZPoints() + 1 * m->getZRes()), m->getZRes(), m_WriteBinaryFile);

  int64_t totalPoints = m->getXPoints() * m->getYPoints() * m->getZPoints();
  fprintf(f, "CELL_DATA %d\n", (int)totalPoints);

  AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(m_SelectedAttributeMatrixPath);
  QList<QString> arrayNames = am->getAttributeArrayNameList();
  foreach(const QString arrayName, arrayNames)
  {
    IDataArray::Pointer iDataPtr = am->getAttributeArray(arrayName);
    QString className = iDataPtr->getNameOfClass();
    if (className.startsWith("DataArray"))
    {
      QString ss = QObject::tr("Writing Cell Data %1").arg(iDataPtr->getName());
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      //qDebug() << "Writing DataArray " << iDataPtr->getName() << " To a VTK File";
      VTK_WRITE_RECTILINEAR_DATA(UInt8ArrayType, iDataPtr, "unsigned_char", uint8_t, "%d ");
      VTK_WRITE_RECTILINEAR_DATA(Int8ArrayType, iDataPtr, "char", int8_t, "%d ");
      VTK_WRITE_RECTILINEAR_DATA(UInt16ArrayType, iDataPtr, "unsigned_short", uint16_t, "%d ");
      VTK_WRITE_RECTILINEAR_DATA(Int16ArrayType, iDataPtr, "short", int16_t, "%d ");
      VTK_WRITE_RECTILINEAR_DATA(UInt32ArrayType, iDataPtr, "unsigned_int", uint32_t, "%d ");
      VTK_WRITE_RECTILINEAR_DATA(Int32ArrayType, iDataPtr, "int", int32_t, "%d ");
      #if CMP_SIZEOF_INT64_T == 0
      VTK_WRITE_RECTILINEAR_DATA(UInt64ArrayType, iDataPtr, "unsigned_long", uint64_t, "%lu ");
      #else
      VTK_WRITE_RECTILINEAR_DATA(UInt64ArrayType, iDataPtr, "unsigned_long", uint64_t, "%llu ");
      #endif
      VTK_WRITE_RECTILINEAR_DATA(Int64ArrayType, iDataPtr, "long", int64_t, "%lld ");
      VTK_WRITE_RECTILINEAR_DATA(FloatArrayType, iDataPtr, "float", float, "%f ");
      VTK_WRITE_RECTILINEAR_DATA(DoubleArrayType, iDataPtr, "double", double, "%f ");
    }
  }

  if(err < 0)
  {
    QString ss = QObject::tr("Error writing vtk file '%1'\n ").arg(file);
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  // Close the file
  fclose(f);
  return err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VtkRectilinearGridWriter::newFilterInstance(bool copyFilterParameters)
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
const QString VtkRectilinearGridWriter::getCompiledLibraryName()
{ return IO::IOBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VtkRectilinearGridWriter::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VtkRectilinearGridWriter::getSubGroupName()
{ return DREAM3D::FilterSubGroups::OutputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VtkRectilinearGridWriter::getHumanLabel()
{ return "Vtk Rectilinear Grid Writer"; }

