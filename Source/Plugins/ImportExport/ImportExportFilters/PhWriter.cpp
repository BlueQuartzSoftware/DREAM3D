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

#include <memory>

#include "PhWriter.h"
#include <fstream>

#include <QtCore/QDir>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Utilities/FileSystemPathHelper.h"

#include "ImportExport/ImportExportConstants.h"
#include "ImportExport/ImportExportVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhWriter::PhWriter()
: m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhWriter::~PhWriter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhWriter::setupFilterParameters()
{
  FileWriter::setupFilterParameters();
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output File", OutputFile, FilterParameter::Parameter, PhWriter, "*.ph", "CMU Feature Growth"));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, PhWriter, req));
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void PhWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setOutputFile(reader->readString("OutputFile", getOutputFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhWriter::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhWriter::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  FileSystemPathHelper::CheckOutputFile(this, "Output File Path", getOutputFile(), true);

  std::vector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(getErrorCode() < 0)
  {
    return;
  }

  size_t volTuples = image->getNumberOfElements();

  if(volTuples != m_FeatureIdsPtr.lock()->getNumberOfTuples())
  {
    QString ss = QObject::tr("The number of Tuples for the DataArray %1 is %2 and for the associated Image Geometry is %3. The number of tuples must match")
                     .arg(m_FeatureIdsPtr.lock()->getName())
                     .arg(m_FeatureIdsPtr.lock()->getNumberOfTuples());
    setErrorCondition(-10201, ss);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhWriter::preflight()
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
int32_t PhWriter::writeHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t PhWriter::writeFile()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return getErrorCode();
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  SizeVec3Type udims = m->getGeometryAs<ImageGeom>()->getDimensions();

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]),
      static_cast<int64_t>(udims[1]),
      static_cast<int64_t>(udims[2]),
  };
  size_t totalpoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo fi(getOutputFile());
  QDir parentPath(fi.path());
  if(!parentPath.mkpath("."))
  {

    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
    setErrorCondition(-1, ss);
    return -1;
  }

  std::ofstream outfile;
  outfile.open(getOutputFile().toLatin1().data(), std::ios_base::binary);
  if(!outfile)
  {
    QString ss = QObject::tr("Error opening output file '%1'").arg(getOutputFile());
    setErrorCondition(-100, ss);
    return getErrorCode();
  }

  // Find the unique number of features
  QMap<int32_t, bool> used;
  for(size_t i = 0; i < totalpoints; ++i)
  {
    used[m_FeatureIds[i]] = true;
  }

  int32_t features = 0;
  typedef QMap<int32_t, bool>::iterator iterator;
  for(iterator i = used.begin(); i != used.end(); i++)
  {
    if(i.value())
    {
      features++;
    }
  }

  // Buffer the output with 4096 Bytes which is typically the size of a "Block" on a
  // modern Hard Drive. This should speed up the writes considerably
  char buffer[4096];
  outfile.rdbuf()->pubsetbuf(buffer, 4096);

  outfile << "     " << dims[0] << "     " << dims[1] << "     " << dims[2] << "\n";
  outfile << "\'DREAM3\'              52.00  1.000  1.0       " << features << "\n";
  outfile << " 0.000 0.000 0.000          0        \n"; // << features << endl;

  for(size_t k = 0; k < totalpoints; k++)
  {
    outfile << m_FeatureIds[k] << '\n';
  }
  outfile.close();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Writing Ph File Complete");
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PhWriter::newFilterInstance(bool copyFilterParameters) const
{
  PhWriter::Pointer filter = PhWriter::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhWriter::getCompiledLibraryName() const
{
  return ImportExportConstants::ImportExportBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhWriter::getBrandingString() const
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhWriter::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << ImportExport::Version::Major() << "." << ImportExport::Version::Minor() << "." << ImportExport::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhWriter::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid PhWriter::getUuid() const
{
  return QUuid("{4786a02e-5fe1-58e0-a906-15556b40d5ce}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhWriter::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::OutputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhWriter::getHumanLabel() const
{
  return "Export Ph File (Feature Ids)";
}

// -----------------------------------------------------------------------------
PhWriter::Pointer PhWriter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<PhWriter> PhWriter::New()
{
  struct make_shared_enabler : public PhWriter
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString PhWriter::getNameOfClass() const
{
  return QString("PhWriter");
}

// -----------------------------------------------------------------------------
QString PhWriter::ClassName()
{
  return QString("PhWriter");
}

// -----------------------------------------------------------------------------
void PhWriter::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath PhWriter::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}
