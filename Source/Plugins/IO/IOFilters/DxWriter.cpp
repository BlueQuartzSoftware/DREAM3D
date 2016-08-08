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

#include "DxWriter.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "IO/IOConstants.h"
#include "IO/IOVersion.h"

// Include the MOC generated file for this class
#include "moc_DxWriter.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxWriter::DxWriter() :
  FileWriter(),
  m_AddSurfaceLayer(false),
  m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds),
  m_FeatureIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxWriter::~DxWriter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxWriter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(OutputFileFilterParameter::New("Output File", "OutputFile", getOutputFile(), FilterParameter::Parameter, SIMPL_BIND_SETTER(DxWriter, this, OutputFile), SIMPL_BIND_GETTER(DxWriter, this, OutputFile), "*.dx", "Open DX Visualization"));
  parameters.push_back(BooleanFilterParameter::New("Add Surface Layer", "AddSurfaceLayer", getAddSurfaceLayer(), FilterParameter::Parameter, SIMPL_BIND_SETTER(DxWriter, this, AddSurfaceLayer), SIMPL_BIND_GETTER(DxWriter, this, AddSurfaceLayer)));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(DxWriter, this, FeatureIdsArrayPath), SIMPL_BIND_GETTER(DxWriter, this, FeatureIdsArrayPath)));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setOutputFile( reader->readString( "OutputFile", getOutputFile() ) );
  setAddSurfaceLayer( reader->readValue("AddSurfaceLayer", getAddSurfaceLayer()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxWriter::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxWriter::dataCheck()
{
  setErrorCondition(0);

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  if (getOutputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("The output file must be set");
    setErrorCondition(-1000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QFileInfo fi(getOutputFile());

  QDir parentPath = fi.path();
  if (parentPath.exists() == false)
  {
    QString ss = QObject::tr("The directory path for the output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    notifyWarningMessage(getHumanLabel(), ss, -1);
  }

  if (fi.suffix().compare("") == 0)
  {
    setOutputFile(getOutputFile().append(".dx"));
  }

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(getErrorCondition() < 0) { return; }

  size_t volTuples = image->getNumberOfElements();

  if (volTuples != m_FeatureIdsPtr.lock()->getNumberOfTuples() )
  {
    setErrorCondition(-10200);
    QString ss = QObject::tr("The number of Tuples for the DataArray %1 is %2 and for the associated Image Geometry is %3. The number of tuples must match").arg(m_FeatureIdsPtr.lock()->getName()).arg(m_FeatureIdsPtr.lock()->getNumberOfTuples());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxWriter::preflight()
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
int32_t DxWriter::writeHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t DxWriter::writeFile()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return getErrorCondition(); }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  int32_t err = 0;
  size_t udims[3] =
  { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);

  int64_t dims[3] =
  { static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]), };

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getOutputFile());

  QDir dir(fi.path());
  if(!dir.mkpath("."))
  {
    QString ss;
    ss = QObject::tr("Error creating parent path '%1'").arg(dir.path());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }

  QFile file(getOutputFile());
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Error opening output file '%1'").arg(getOutputFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }

  QTextStream out(&file);
  int64_t fileXDim = dims[0];
  int64_t fileYDim = dims[1];
  int64_t fileZDim = dims[2];

  int64_t posXDim = fileXDim;
  int64_t posYDim = fileYDim;
  int64_t posZDim = fileZDim;

  if (m_AddSurfaceLayer)
  {
    fileXDim = dims[0] + 2;
    fileYDim = dims[1] + 2;
    fileZDim = dims[2] + 2;

    posXDim = fileXDim;
    posYDim = fileYDim;
    posZDim = fileZDim;
  }

  // Write the header
  out << "# object 1 are the regular positions. The grid is " << posZDim << " " << posYDim << " " << posXDim << ". The origin is\n";
  out << "# at [0 0 0], and the deltas are 1 in the first and third dimensions, and\n";
  out << "# 2 in the second dimension\n";
  out << "#\n";
  out << "object 1 class gridpositions counts " << posZDim << " " << posYDim << " " << posXDim << "\n";
  out << "origin 0 0 0" << "\n";
  out << "delta  1 0 0" << "\n";
  out << "delta  0 1 0" << "\n";
  out << "delta  0 0 1" << "\n";
  out << "#\n";
  out << "# object 2 are the regular connections\n";
  out << "#\n";
  out << "object 2 class gridconnections counts " << posZDim << " " << posYDim << " " << posXDim << "\n";
  out << "#\n";
  out << "# object 3 are the data, which are in a one-to-one correspondence with\n";
  out << "# the positions (\"dep\" on positions). The positions increment in the order\n";
  out << "# \"last index varies fastest\", i.e. (x0, y0, z0), (x0, y0, z1), (x0, y0, z2),\n";
  out << "# (x0, y1, z0), etc.\n";
  out << "#\n";
  out << "object 3 class array type int rank 0 items " << fileXDim* fileYDim* fileZDim << " data follows" << "\n";

  // Add a complete layer of surface voxels
  size_t rnIndex = 1;
  if (m_AddSurfaceLayer)
  {
    for (int64_t i = 0; i < (fileXDim * fileYDim); ++i)
    {
      out << "-3 ";
      if (rnIndex == 20)
      {
        rnIndex = 0;
        out << "\n";
      }
      rnIndex++;
    }
  }

  int64_t index = 0;
  for (int64_t x = 0; x < dims[0]; ++x)
  {
    // Add a leading surface Row for this plane if needed
    if (m_AddSurfaceLayer)
    {
      for (int64_t i = 0; i < fileXDim; ++i)
      {
        out << "-4 ";
      }
      out << "\n";
    }
    for (int64_t y = 0; y < dims[1]; ++y)
    {
      // write leading surface voxel for this row
      if (m_AddSurfaceLayer)
      {
        out << "-5 ";
      }
      // Write the actual voxel data
      for (int64_t z = 0; z < dims[2]; ++z)
      {
        index = (z * dims[0] * dims[1]) + (dims[0] * y) + x;
        if (m_FeatureIds[index] == 0)
        {
          out << "0" << " ";
        }
        else
        {
          out << m_FeatureIds[index] << " ";
        }
      }
      // write trailing surface voxel for this row
      if (m_AddSurfaceLayer)
      {
        out << "-6 ";
      }
      out << "\n";
    }
    // Add a trailing surface Row for this plane if needed
    if (m_AddSurfaceLayer)
    {
      for (int64_t i = 0; i < fileXDim; ++i)
      {
        out << "-7 ";
      }
      out << "\n";
    }
  }

  // Add a complete layer of surface voxels
  if (m_AddSurfaceLayer)
  {
    rnIndex = 1;
    for (int64_t i = 0; i < (fileXDim * fileYDim); ++i)
    {
      out << "-8 ";
      if (rnIndex == 20)
      {
        out << "\n";
        rnIndex = 0;
      }
      rnIndex++;
    }
  }

  out << "attribute \"dep\" string \"positions\"\n";
  out << "#\n";
  out << "# A field is created with three components: \"positions\", \"connections\",\n";
  out << "# and \"data\"\n";
  out << "object \"regular positions regular connections\" class field" << "\n";
  out << "component  \"positions\"    value 1" << "\n";
  out << "component  \"connections\"  value 2" << "\n";
  out << "component  \"data\"         value 3" << "\n";
  out << "#" << "\n";
  out << "end" << "\n";

  file.close();
#if 0
  out.open("/tmp/m3cmesh.raw", std::ios_base::binary);
  out.write((const char*)(&dims[0]), 4);
  out.write((const char*)(&dims[1]), 4);
  out.write((const char*)(&dims[2]), 4);
  getTotalPoints = dims[0] * dims[1] * dims[2];
  int32_t d = 0;
  for(int index = 0; index < getTotalPoints; ++index)
  {
    d = featureIds[index];
    if (d == 0)
    { d = -3;}
    out.write((const char*)(&d), sizeof(d));
  }

  out.close();
#endif

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer DxWriter::newFilterInstance(bool copyFilterParameters)
{
  DxWriter::Pointer filter = DxWriter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DxWriter::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DxWriter::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DxWriter::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  IO::Version::Major() << "." << IO::Version::Minor() << "." << IO::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DxWriter::getGroupName()
{ return SIMPL::FilterGroups::IOFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DxWriter::getSubGroupName()
{ return SIMPL::FilterSubGroups::OutputFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DxWriter::getHumanLabel()
{ return "Write Dx File (Feature Ids)"; }
