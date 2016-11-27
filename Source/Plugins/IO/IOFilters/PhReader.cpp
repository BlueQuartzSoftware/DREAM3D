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

#include "PhReader.h"

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "IO/IOConstants.h"
#include "IO/IOVersion.h"

#define BUF_SIZE 1024

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
  QVector<size_t> m_Dims;
  QString m_InputFile_Cache;
  QDateTime m_LastRead;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhReaderPrivate::PhReaderPrivate(PhReader* ptr)
: q_ptr(ptr)
, m_Dims()
, m_InputFile_Cache("")
, m_LastRead()
{
}

// Include the MOC generated file for this class
#include "moc_PhReader.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhReader::PhReader()
: FileReader()
, m_VolumeDataContainerName(SIMPL::Defaults::ImageDataContainerName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_InputFile("")
, m_FileWasRead(false)
, m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds)
, d_ptr(new PhReaderPrivate(this))
, m_FeatureIds(nullptr)
{
  m_Origin.x = 0.0f;
  m_Origin.y = 0.0f;
  m_Origin.z = 0.0f;

  m_Resolution.x = 1.0f;
  m_Resolution.y = 1.0f;
  m_Resolution.z = 1.0f;

  m_Dims[0] = 0;
  m_Dims[1] = 0;
  m_Dims[2] = 0;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhReader::~PhReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL_PIMPL_PROPERTY_DEF(PhReader, QVector<size_t>, Dims)
SIMPL_PIMPL_PROPERTY_DEF(PhReader, QString, InputFile_Cache)
SIMPL_PIMPL_PROPERTY_DEF(PhReader, QDateTime, LastRead)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, PhReader, "*.ph", "CMU Grain Growth"));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Parameter, PhReader));

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Resolution", Resolution, FilterParameter::Parameter, PhReader));

  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container", VolumeDataContainerName, FilterParameter::CreatedArray, PhReader));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix", CellAttributeMatrixName, FilterParameter::CreatedArray, PhReader));
  parameters.push_back(SIMPL_NEW_STRING_FP("Feature Ids", FeatureIdsArrayName, FilterParameter::CreatedArray, PhReader));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setVolumeDataContainerName(reader->readString("VolumeDataContainerName", getVolumeDataContainerName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName()));
  setInputFile(reader->readString("InputFile", getInputFile()));
  setOrigin(reader->readFloatVec3("Origin", getOrigin()));
  setResolution(reader->readFloatVec3("Resolution", getResolution()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::updateCellInstancePointers()
{
  setErrorCondition(0);

  if(nullptr != m_FeatureIdsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
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
  QVector<size_t> v;
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
  setErrorCondition(0);
  DataArrayPath tempPath;


  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getVolumeDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

  QFileInfo fi(getInputFile());
  if(getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if(fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getInputFile().isEmpty() == false && fi.exists() == true)
  {
    QDateTime lastModified(fi.lastModified());

    if(getInputFile() == getInputFile_Cache() && getLastRead().isValid() && lastModified.msecsTo(getLastRead()) >= 0)
    {
      // We are reading from the cache, so set the FileWasRead flag to false
      m_FileWasRead = false;

      QVector<size_t> v = getDims();
      ImageGeom::Pointer imageGeom =  m->getGeometryAs<ImageGeom>();
      if(nullptr != imageGeom.get())
      {
        imageGeom->setDimensions(v[0], v[1], v[2]);
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
        setErrorCondition(-48802);
        QString ss = QObject::tr("Error opening input file '%1'").arg(getInputFile());
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
      int32_t error = readHeader();
      fclose(m_InStream);
      m_InStream = nullptr;
      if(error < 0)
      {
        setErrorCondition(error);
        QString ss = QObject::tr("Error occurred trying to parse the dimensions from the input file");
        notifyErrorMessage(getHumanLabel(), ss, -48010);
      }

      // Set the file path and time stamp into the cache
      setLastRead(QDateTime::currentDateTime());
      setInputFile_Cache(getInputFile());
    }
  }

  QVector<size_t> tDims = getDims();
  m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
  if(getErrorCondition() < 0)
  {
    return;
  }

  QVector<size_t> cDims(1, 1);
  tempPath.update(getVolumeDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName());
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims);
  if(nullptr != m_FeatureIdsPtr.lock().get())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }

  m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->getGeometryAs<ImageGeom>()->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
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
  int32_t err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  m_InStream = fopen(getInputFile().toLatin1().data(), "r");
  if(m_InStream == nullptr)
  {
    setErrorCondition(-48030);
    QString ss = QObject::tr("Error opening input file '%1'").arg(getInputFile());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  err = readHeader();
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

  size_t nx = 0;
  size_t ny = 0;
  size_t nz = 0;

  // Read Line #1 which has the dimensions
  fscanf(m_InStream, "%ld %ld %ld\n", &nx, &ny, &nz);

  // Set the values into the cache, so that they can be used later
  QVector<size_t> v;
  v.push_back(nx);
  v.push_back(ny);
  v.push_back(nz);
  setDims(v);

  if(nullptr != m.get())
  {
    ImageGeom::Pointer imageGeom =  m->getGeometryAs<ImageGeom>();
    if(nullptr != imageGeom.get())
    {
      imageGeom->setDimensions(nx, ny, nz);
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

  QVector<size_t> tDims(3, 0);
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
      setErrorCondition(-48040);
      notifyErrorMessage(getHumanLabel(), "Error reading Ph data", getErrorCondition());
      return getErrorCondition();
    }
  }

  // Now set the Resolution and Origin that the user provided on the GUI or as parameters
  m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->getGeometryAs<ImageGeom>()->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

  notifyStatusMessage(getHumanLabel(), "Complete");
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PhReader::newFilterInstance(bool copyFilterParameters)
{
  PhReader::Pointer filter = PhReader::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PhReader::getCompiledLibraryName()
{
  return IOConstants::IOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PhReader::getBrandingString()
{
  return "IO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PhReader::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << IO::Version::Major() << "." << IO::Version::Minor() << "." << IO::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PhReader::getGroupName()
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PhReader::getSubGroupName()
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PhReader::getHumanLabel()
{
  return "Read Ph File (Feature Ids)";
}
