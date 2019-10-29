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

#include "PhReader.h"

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "ImportExport/ImportExportConstants.h"
#include "ImportExport/ImportExportVersion.h"

#define BUF_SIZE 1024

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataArrayID31 = 31,

  DataContainerID = 1
};

/* ############## Start Private Implementation ############################### */
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class PhReaderPrivate
{
  Q_DISABLE_COPY(PhReaderPrivate)
  Q_DECLARE_PUBLIC(PhReader)
  PhReader* const q_ptr;
  PhReaderPrivate(PhReader* ptr);
  std::vector<size_t> m_Dims;
  QString m_InputFile_Cache;
  QDateTime m_LastRead;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhReaderPrivate::PhReaderPrivate(PhReader* ptr)
: q_ptr(ptr)
, m_InputFile_Cache("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhReader::PhReader()
: m_VolumeDataContainerName(SIMPL::Defaults::ImageDataContainerName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_InputFile("")
, m_FileWasRead(false)
, m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds)
, d_ptr(new PhReaderPrivate(this))
{
  m_Origin[0] = 0.0f;
  m_Origin[1] = 0.0f;
  m_Origin[2] = 0.0f;

  m_Spacing[0] = 1.0f;
  m_Spacing[1] = 1.0f;
  m_Spacing[2] = 1.0f;

  m_Dims[0] = 0;
  m_Dims[1] = 0;
  m_Dims[2] = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhReader::~PhReader() = default;

// -----------------------------------------------------------------------------
void PhReader::setDims(const std::vector<size_t>& value)
{
  Q_D(PhReader);
  d->m_Dims = value;
}

// -----------------------------------------------------------------------------
std::vector<size_t> PhReader::getDims() const
{
  Q_D(const PhReader);
  return d->m_Dims;
}

// -----------------------------------------------------------------------------
void PhReader::setInputFile_Cache(const QString& value)
{
  Q_D(PhReader);
  d->m_InputFile_Cache = value;
}

// -----------------------------------------------------------------------------
QString PhReader::getInputFile_Cache() const
{
  Q_D(const PhReader);
  return d->m_InputFile_Cache;
}

// -----------------------------------------------------------------------------
void PhReader::setLastRead(const QDateTime& value)
{
  Q_D(PhReader);
  d->m_LastRead = value;
}

// -----------------------------------------------------------------------------
QDateTime PhReader::getLastRead() const
{
  Q_D(const PhReader);
  return d->m_LastRead;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::setupFilterParameters()
{
  FileReader::setupFilterParameters();
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, PhReader, "*.ph", "CMU Grain Growth"));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, PhReader));

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Spacing", Spacing, FilterParameter::Parameter, PhReader));

  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", VolumeDataContainerName, FilterParameter::CreatedArray, PhReader));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Attribute Matrix", CellAttributeMatrixName, VolumeDataContainerName, FilterParameter::CreatedArray, PhReader));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Feature Ids", FeatureIdsArrayName, VolumeDataContainerName, CellAttributeMatrixName, FilterParameter::CreatedArray, PhReader));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setVolumeDataContainerName(reader->readDataArrayPath("VolumeDataContainerName", getVolumeDataContainerName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName()));
  setInputFile(reader->readString("InputFile", getInputFile()));
  setOrigin(reader->readFloatVec3("Origin", getOrigin()));
  setSpacing(reader->readFloatVec3("Spacing", getSpacing()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::updateCellInstancePointers()
{
  clearErrorCode();
  clearWarningCode();

  if(nullptr != m_FeatureIdsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::flushCache()
{
  setInputFile_Cache("");
  std::vector<size_t> v;
  v.push_back(0);
  v.push_back(0);
  v.push_back(0);
  setDims(v);
  setLastRead(QDateTime());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getVolumeDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

  QFileInfo fi(getInputFile());
  if(getInputFile().isEmpty())
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-387, ss);
  }
  else if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388, ss);
  }

  if(!getInputFile().isEmpty() && fi.exists())
  {
    QDateTime lastModified(fi.lastModified());

    if(getInputFile() == getInputFile_Cache() && getLastRead().isValid() && lastModified.msecsTo(getLastRead()) >= 0)
    {
      // We are reading from the cache, so set the FileWasRead flag to false
      m_FileWasRead = false;

      std::vector<size_t> v = getDims();
      ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();
      if(nullptr != imageGeom.get())
      {
        imageGeom->setDimensions(v.data());
      }
    }
    else
    {
      // We are reading from the file, so set the FileWasRead flag to true
      m_FileWasRead = true;

      // We need to read the header of the input file to get the dimensions
      m_InStream = fopen(getInputFile().toLatin1().data(), "r");
      if(m_InStream == nullptr)
      {
        QString ss = QObject::tr("Error opening input file '%1'").arg(getInputFile());
        setErrorCondition(-48802, ss);
        return;
      }
      int32_t error = readHeader();
      fclose(m_InStream);
      m_InStream = nullptr;
      if(error < 0)
      {
        QString ss = QObject::tr("Error occurred trying to parse the dimensions from the input file");
        setErrorCondition(error, ss);
      }

      // Set the file path and time stamp into the cache
      setLastRead(QDateTime::currentDateTime());
      setInputFile_Cache(getInputFile());
    }
  }

  std::vector<size_t> tDims = getDims();
  m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell, AttributeMatrixID21);
  if(getErrorCode() < 0)
  {
    return;
  }

  std::vector<size_t> cDims(1, 1);
  tempPath.update(getVolumeDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName());
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims, "", DataArrayID31);
  if(nullptr != m_FeatureIdsPtr.lock())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }

  m->getGeometryAs<ImageGeom>()->setSpacing(std::make_tuple(m_Spacing[0], m_Spacing[1], m_Spacing[2]));
  m->getGeometryAs<ImageGeom>()->setOrigin(std::make_tuple(m_Origin[0], m_Origin[1], m_Origin[2]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::preflight()
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
void PhReader::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  m_InStream = fopen(getInputFile().toLatin1().data(), "r");
  if(m_InStream == nullptr)
  {
    QString ss = QObject::tr("Error opening input file '%1'").arg(getInputFile());
    setErrorCondition(-48030, ss);
    return;
  }

  int32_t err = readHeader();
  if(err < 0)
  {
    fclose(m_InStream);
    m_InStream = nullptr;
    return;
  }
  err = readFile();
  fclose(m_InStream);
  m_InStream = nullptr;
  if(err < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t PhReader::readHeader()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVolumeDataContainerName());

  int nx = 0;
  int ny = 0;
  int nz = 0;

  // Read Line #1 which has the dimensions
  fscanf(m_InStream, "%d %d %d\n", &nx, &ny, &nz);

  // Set the values into the cache, so that they can be used later
  std::vector<size_t> v;
  v.push_back(static_cast<size_t>(nx));
  v.push_back(static_cast<size_t>(ny));
  v.push_back(static_cast<size_t>(nz));
  setDims(v);

  if(nullptr != m.get())
  {
    ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();
    if(nullptr != imageGeom.get())
    {
      imageGeom->setDimensions(v.data());
    }
  }

  char buf[BUF_SIZE];
  // Read Line #2 and dump it
  ::memset(buf, 0, BUF_SIZE);
  fgets(buf, BUF_SIZE, m_InStream);
  // Read Line #3 and dump it
  ::memset(buf, 0, BUF_SIZE);
  fgets(buf, BUF_SIZE, m_InStream);
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t PhReader::readFile()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getVolumeDataContainerName());

  size_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();

  std::vector<size_t> tDims(3, 0);
  tDims[0] = m->getGeometryAs<ImageGeom>()->getXPoints();
  tDims[1] = m->getGeometryAs<ImageGeom>()->getYPoints();
  tDims[2] = m->getGeometryAs<ImageGeom>()->getZPoints();
  m->getAttributeMatrix(getCellAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateCellInstancePointers();

  for(size_t n = 0; n < totalPoints; ++n)
  {
    if(fscanf(m_InStream, "%d", m_FeatureIds + n) == 0)
    {
      fclose(m_InStream);
      m_InStream = nullptr;
      setErrorCondition(-48040, "Error reading Ph data");
      return getErrorCode();
    }
  }

  // Now set the Spacing and Origin that the user provided on the GUI or as parameters
  m->getGeometryAs<ImageGeom>()->setSpacing(std::make_tuple(m_Spacing[0], m_Spacing[1], m_Spacing[2]));
  m->getGeometryAs<ImageGeom>()->setOrigin(std::make_tuple(m_Origin[0], m_Origin[1], m_Origin[2]));

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PhReader::newFilterInstance(bool copyFilterParameters) const
{
  PhReader::Pointer filter = PhReader::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhReader::getCompiledLibraryName() const
{
  return ImportExportConstants::ImportExportBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhReader::getBrandingString() const
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhReader::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << ImportExport::Version::Major() << "." << ImportExport::Version::Minor() << "." << ImportExport::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhReader::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid PhReader::getUuid() const
{
  return QUuid("{c923176f-39c9-5521-9786-624f88d2b2c0}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhReader::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhReader::getHumanLabel() const
{
  return "Import Ph File (Feature Ids)";
}

// -----------------------------------------------------------------------------
PhReader::Pointer PhReader::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<PhReader> PhReader::New()
{
  struct make_shared_enabler : public PhReader
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString PhReader::getNameOfClass() const
{
  return QString("PhReader");
}

// -----------------------------------------------------------------------------
QString PhReader::ClassName()
{
  return QString("PhReader");
}

// -----------------------------------------------------------------------------
void PhReader::setVolumeDataContainerName(const DataArrayPath& value)
{
  m_VolumeDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath PhReader::getVolumeDataContainerName() const
{
  return m_VolumeDataContainerName;
}

// -----------------------------------------------------------------------------
void PhReader::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString PhReader::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void PhReader::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString PhReader::getInputFile() const
{
  return m_InputFile;
}

// -----------------------------------------------------------------------------
void PhReader::setOrigin(const FloatVec3Type& value)
{
  m_Origin = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type PhReader::getOrigin() const
{
  return m_Origin;
}

// -----------------------------------------------------------------------------
void PhReader::setSpacing(const FloatVec3Type& value)
{
  m_Spacing = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type PhReader::getSpacing() const
{
  return m_Spacing;
}

// -----------------------------------------------------------------------------
void PhReader::setFileWasRead(bool value)
{
  m_FileWasRead = value;
}

// -----------------------------------------------------------------------------
bool PhReader::getFileWasRead() const
{
  return m_FileWasRead;
}

// -----------------------------------------------------------------------------
void PhReader::setFeatureIdsArrayName(const QString& value)
{
  m_FeatureIdsArrayName = value;
}

// -----------------------------------------------------------------------------
QString PhReader::getFeatureIdsArrayName() const
{
  return m_FeatureIdsArrayName;
}
