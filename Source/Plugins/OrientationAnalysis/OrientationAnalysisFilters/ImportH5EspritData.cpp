/* ============================================================================
 * Copyright (c) 2019 BlueQuartz Software, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ImportH5EspritData.h"

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtCore/QString>

#include "H5Support/H5Lite.h"
#include "H5Support/H5ScopedSentinel.h"
#include "H5Support/H5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "EbsdLib/BrukerNano/EspritConstants.h"
#include "EbsdLib/BrukerNano/EspritPhase.h"
#include "EbsdLib/BrukerNano/H5EspritFields.h"
#include "EbsdLib/BrukerNano/H5EspritReader.h"

#include "SIMPLib/Geometry/ImageGeom.h"

#include "OrientationAnalysis/OrientationAnalysisVersion.h"

/**
 * @brief The ReadH5EspritDataPrivate class is a private implementation of the ImportH5EspritData class
 */
class ImportH5EspritDataPrivate
{
  Q_DISABLE_COPY(ImportH5EspritDataPrivate)
  Q_DECLARE_PUBLIC(ImportH5EspritData)
  ImportH5EspritData* const q_ptr;
  ImportH5EspritDataPrivate(ImportH5EspritData* ptr);

  Esprit_Private_Data m_FileCacheData;
  QStringList m_FileScanNames;
  QVector<int> m_PatternDims;

  QString m_InputFile_Cache;
  QDateTime m_TimeStamp_Cache;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportH5EspritDataPrivate::ImportH5EspritDataPrivate(ImportH5EspritData* ptr)
: q_ptr(ptr)
{
}

// -----------------------------------------------------------------------------
ImportH5EspritData::ImportH5EspritData()
: d_ptr(new ImportH5EspritDataPrivate(this))
{
}

// -----------------------------------------------------------------------------
ImportH5EspritData::~ImportH5EspritData() = default;

// -----------------------------------------------------------------------------
SIMPL_PIMPL_PROPERTY_DEF(ImportH5EspritData, Esprit_Private_Data, FileCacheData)

#if 0
void ImportH5EspritData::setFileCacheData(const Esprit_Private_Data& value)
{
  Q_D(ImportH5EspritData);
  d->m_FileCacheData = value;
}
Esprit_Private_Data ImportH5EspritData::getFileCacheData() const
{
  Q_D(const ImportH5EspritData);
  return d->m_FileCacheData;
}
#endif

// -----------------------------------------------------------------------------
void ImportH5EspritData::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
void ImportH5EspritData::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  H5EspritReader::Pointer reader = H5EspritReader::New();
  reader->setFileName(getInputFile());
  QVector<size_t> tDims(3, 0);
  QVector<size_t> cDims(1, 1);
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer ebsdAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());
  ebsdAttrMat->setType(AttributeMatrix::Type::Cell);

  QStringList scanNames = getSelectedScanNames();

  for(int index = 0; index < scanNames.size(); index++)
  {
    QString currentScanName = scanNames[index];

    readDataFile(reader.get(), m.get(), tDims, currentScanName, ANG_FULL_FILE);
    if(getErrorCondition() < 0)
    {
      return;
    }

    copyRawEbsdData(reader.get(), tDims, cDims, index);
    if(getErrorCondition() < 0)
    {
      return;
    }
  }

  // Set the file name and time stamp into the cache, if we are reading from the file and after all the reading has been done
  {
    QFileInfo newFi(getInputFile());
    QDateTime timeStamp(newFi.lastModified());

    if(getInputFile() == getInputFile_Cache() && getTimeStamp_Cache().isValid() && getTimeStamp_Cache() >= timeStamp)
    {
      setTimeStamp_Cache(timeStamp);
      setInputFile_Cache(getInputFile());
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportH5EspritData::newFilterInstance(bool copyFilterParameters) const
{
  ImportH5EspritData::Pointer filter = ImportH5EspritData::New();
  if(copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters());
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportH5EspritData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ImportH5EspritData::getUuid()
{
  return QUuid("{8abdea7d-f715-5a24-8165-7f946bbc2fe9}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportH5EspritData::getHumanLabel() const
{
  return "Import Bruker Nano Esprit Data (.h5)";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::OEM ImportH5EspritData::readManufacturer() const
{

  Ebsd::OEM manuf = Ebsd::OEM::Unknown;

  hid_t fid = H5Utilities::openFile(getInputFile().toStdString(), true);
  if(fid < 0)
  {
    return manuf;
  }
  H5ScopedFileSentinel sentinel(&fid, false);
  QString dsetName;
  std::list<std::string> names;
  herr_t err = H5Utilities::getGroupObjects(fid, H5Utilities::H5Support_ANY, names);
  auto findIter = std::find(names.begin(), names.end(), Ebsd::H5OIM::Manufacturer.toStdString());
  if(findIter != names.end())
  {
    dsetName = Ebsd::H5OIM::Manufacturer;
  }

  findIter = std::find(names.begin(), names.end(), Ebsd::H5Esprit::Manufacturer.toStdString());
  if(findIter != names.end())
  {
    dsetName = Ebsd::H5Esprit::Manufacturer;
  }

  QString manufacturer("Unknown");
  err = QH5Lite::readStringDataset(fid, dsetName, manufacturer);
  if(err < 0)
  {
    return manuf;
  }
  if(manufacturer == Ebsd::H5Esprit::BrukerNano)
  {
    manuf = Ebsd::OEM::Bruker;
  }

  return manuf;
}

// -----------------------------------------------------------------------------
void ImportH5EspritData::dataCheckOEM()
{
  // Read the manufacturer from the file
  Ebsd::OEM manfacturer = readManufacturer();
  setManufacturer(manfacturer);
  if(manfacturer != Ebsd::OEM::Bruker)
  {
    QString ss = QObject::tr("The manufacturer is not recognized as a valid entry.");
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);
  image->setUnits(IGeometry::LengthUnit::Micrometer);

  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
  if(getErrorCondition() < 0)
  {
    return;
  }
  tDims.resize(1);
  tDims[0] = 0;
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix(this, getCellEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble);
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataArrayPath tempPath;

  QVector<size_t> cDims(3, 0);

  H5EspritReader::Pointer reader = H5EspritReader::New();
  reader->setFileName(getInputFile());

  // We ALWAYS want to read the Scan Names from the file so that we can present that list to the user if needed.
  QStringList scanNames;
  int32_t err = reader->readScanNames(scanNames);
  if(err < 0)
  {
    setErrorCondition(reader->getErrorCode());
    notifyErrorMessage(getHumanLabel(), reader->getErrorMessage(), err);
    return;
  }
  setFileScanNames(scanNames);
  setNumberOfScans(scanNames.size());

  IDataArrayMap ebsdArrayMap;

  if(!getSelectedScanNames().empty())
  {
    readDataFile(reader.get(), m.get(), cDims, scanNames[0], ANG_HEADER_ONLY);

    // Update the size of the Cell Attribute Matrix now that the dimensions of the volume are known
    cellAttrMat->resizeAttributeArrays(cDims);
    H5EspritFields espritFeatures;
    QVector<QString> names = espritFeatures.getFilterFeatures<QVector<QString>>();
    cDims.resize(1);
    cDims[0] = 1;
    // We are doing this because we DON'T want to allocate all the data right now and by forcibly setting to InPreflight
    // the createAndAddAttributeArray array will not allocate the array
    bool areWeInPreflight = getInPreflight();
    setInPreflight(true);
    for(const auto& name : names)
    {
      if(reader->getPointerType(name) == Ebsd::Int32)
      {
        cellAttrMat->createAndAddAttributeArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, name, 0, cDims);
        ebsdArrayMap.insert(name, cellAttrMat->getAttributeArray(name));
      }
      else if(reader->getPointerType(name) == Ebsd::Float)
      {
        cellAttrMat->createAndAddAttributeArray<DataArray<float>, AbstractFilter, float>(this, name, 0, cDims);
        ebsdArrayMap.insert(name, cellAttrMat->getAttributeArray(name));
      }
    }
    setInPreflight(areWeInPreflight);
  }
  else
  {
    setErrorCondition(-996);
    QString ss = QObject::tr("At least one scan must be chosen.  Please select a scan from the list.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  cDims.resize(1);
  cDims[0] = 3;
  tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), Ebsd::Esprit::EulerAngles);
  m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims);
  if(nullptr != m_CellEulerAnglesPtr.lock())
  {
    m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
  }
  ebsdArrayMap.insert(Ebsd::Esprit::EulerAngles, getDataContainerArray()->getPrereqIDataArrayFromPath<FloatArrayType, AbstractFilter>(this, tempPath));

  cDims[0] = 1;
  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), Ebsd::Esprit::CrystalStructures);
  m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this, tempPath, Ebsd::CrystalStructure::UnknownCrystalStructure, cDims);
  if(nullptr != m_CrystalStructuresPtr.lock())
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  }
  ebsdArrayMap.insert(Ebsd::Esprit::CrystalStructures, getDataContainerArray()->getPrereqIDataArrayFromPath<UInt32ArrayType, AbstractFilter>(this, tempPath));

  cDims[0] = 6;
  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), Ebsd::Esprit::LatticeConstants);
  m_LatticeConstantsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0.0, cDims);
  if(nullptr != m_LatticeConstantsPtr.lock())
  {
    m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0);
  }
  ebsdArrayMap.insert(Ebsd::Esprit::LatticeConstants, getDataContainerArray()->getPrereqIDataArrayFromPath<FloatArrayType, AbstractFilter>(this, tempPath));

  StringDataArray::Pointer materialNames = StringDataArray::CreateArray(cellEnsembleAttrMat->getNumberOfTuples(), SIMPL::EnsembleData::MaterialName);
  cellEnsembleAttrMat->addAttributeArray(SIMPL::EnsembleData::MaterialName, materialNames);
  ebsdArrayMap.insert(SIMPL::EnsembleData::MaterialName, materialNames);

  if(getReadPatternData())
  {
    cDims.resize(2);
    cDims[0] = getPatternDims().at(1);
    cDims[1] = getPatternDims().at(0);
    if(cDims[0] != 0 && cDims[1] != 0)
    {
      // Again, needing to watch the memory allocation as these EBSD files with Patterns are getting large.
      // We will handle allocating the memory later on.
      bool areWeInPreflight = getInPreflight();
      setInPreflight(true);
      tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), Ebsd::H5Esprit::RawPatterns);
      m_CellPatternDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, cDims);
      if(nullptr != m_CellPatternDataPtr.lock())
      {
        m_CellPatternData = m_CellPatternDataPtr.lock()->getPointer(0);
      }
      ebsdArrayMap.insert(Ebsd::H5Esprit::RawPatterns, getDataContainerArray()->getPrereqIDataArrayFromPath<UInt8ArrayType, AbstractFilter>(this, tempPath));
      setInPreflight(areWeInPreflight);
    }
    else
    {
      setErrorCondition(-998);
      QString ss = QObject::tr("The filter parameter 'Read Pattern Data' has been enabled but there does not seem to be any pattern data in the file for the scan name selected");
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }

  setEbsdArrayMap(ebsdArrayMap);
}

// -----------------------------------------------------------------------------
void ImportH5EspritData::readDataFile(EbsdReader* ebsdReader, DataContainer* m, QVector<size_t>& tDims, const QString& scanName, ANG_READ_FLAG flag)
{
  auto reader = dynamic_cast<H5EspritReader*>(ebsdReader);
  QFileInfo fi(getInputFile());
  QDateTime timeStamp(fi.lastModified());
  if(flag == ANG_FULL_FILE)
  {
    setInputFile_Cache(""); // We need something to trigger the file read below
  }
  // Drop into this if statement if we need to read from a file
  if(getInputFile() != getInputFile_Cache() || !getTimeStamp_Cache().isValid() || getTimeStamp_Cache() < timeStamp)
  {
    float zStep = static_cast<float>(getZSpacing()), xOrigin = getOrigin().x, yOrigin = getOrigin().y, zOrigin = getOrigin().z;
    reader->setReadPatternData(getReadPatternData());

    // If the user has already set a Scan Name to read then we are good to go.
    reader->setHDF5Path(scanName);

    if(flag == ANG_HEADER_ONLY)
    {
      int err = reader->readHeaderOnly();
      if(err < 0)
      {
        setErrorCondition(err);
        notifyErrorMessage(getHumanLabel(), reader->getErrorMessage(), err);
        setFileWasRead(false);
        return;
      }
      setFileWasRead(true);
    }
    else
    {
      int32_t err = reader->readFile();
      if(err < 0)
      {
        setErrorCondition(err);
        notifyErrorMessage(getHumanLabel(), reader->getErrorMessage(), err);
        notifyErrorMessage(getHumanLabel(), "H5OIMReader could not read the .h5 file.", getErrorCondition());
        return;
      }
    }
    tDims[0] = reader->getXDimension();
    tDims[1] = reader->getYDimension();
    tDims[2] = getSelectedScanNames().size();

    // Set Cache with values from the file
    {
      Esprit_Private_Data data;
      data.dims = tDims;
      data.resolution.push_back(reader->getXStep());
      data.resolution.push_back(reader->getYStep());
      data.resolution.push_back(zStep);
      data.origin.push_back(xOrigin);
      data.origin.push_back(yOrigin);
      data.origin.push_back(zOrigin);
      data.phases = reader->getPhaseVector();
      setFileCacheData(data);

      std::array<int32_t, 2> patternDims = {{0, 0}};
      reader->getPatternDims(patternDims);
      QVector<int32_t> pDims(2);
      pDims[0] = patternDims[0];
      pDims[1] = patternDims[1];
      setPatternDims(pDims);

      setInputFile_Cache(getInputFile());

      QFileInfo newFi(getInputFile());
      QDateTime timeStamp(newFi.lastModified());
      setTimeStamp_Cache(timeStamp);
    }
  }
  else
  {
    setFileWasRead(false);
  }

  // Read from cache
  tDims[0] = getFileCacheData().dims[0];
  tDims[1] = getFileCacheData().dims[1];
  tDims[2] = getFileCacheData().dims[2];
  ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();
  if(image != nullptr)
  {
    image->setDimensions(tDims[0], tDims[1], tDims[2]);
    image->setResolution(getFileCacheData().resolution[0], getFileCacheData().resolution[1], getFileCacheData().resolution[2]);
    image->setOrigin(getFileCacheData().origin[0], getFileCacheData().origin[1], getFileCacheData().origin[2]);
  }

  if(flag == ANG_FULL_FILE)
  {
    loadMaterialInfo(reader);
  }
}

// -----------------------------------------------------------------------------
int32_t ImportH5EspritData::loadMaterialInfo(EbsdReader* ebsdReader)
{
  auto reader = dynamic_cast<H5EspritReader*>(ebsdReader);

  QVector<EspritPhase::Pointer> phases = getFileCacheData().phases;
  if(phases.empty())
  {
    setErrorCondition(reader->getErrorCode());
    notifyErrorMessage(getHumanLabel(), reader->getErrorMessage(), getErrorCondition());
    return getErrorCondition();
  }

  DataArray<uint32_t>::Pointer crystalStructures = DataArray<uint32_t>::CreateArray(phases.size() + 1, Ebsd::AngFile::CrystalStructures);
  StringDataArray::Pointer materialNames = StringDataArray::CreateArray(phases.size() + 1, Ebsd::AngFile::MaterialName);
  QVector<size_t> dims(1, 6);
  FloatArrayType::Pointer latticeConstants = FloatArrayType::CreateArray(phases.size() + 1, dims, Ebsd::AngFile::LatticeConstants);

  // Initialize the zero'th element to unknowns. The other elements will
  // be filled in based on values from the data file
  crystalStructures->setValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
  materialNames->setValue(0, "Invalid Phase");
  latticeConstants->setComponent(0, 0, 0.0f);
  latticeConstants->setComponent(0, 1, 0.0f);
  latticeConstants->setComponent(0, 2, 0.0f);
  latticeConstants->setComponent(0, 3, 0.0f);
  latticeConstants->setComponent(0, 4, 0.0f);
  latticeConstants->setComponent(0, 5, 0.0f);

  for(const auto& phase : phases)
  // for(size_t i = 0; i < phases.size(); i++)
  {
    int32_t phaseID = phase->getPhaseIndex();
    crystalStructures->setValue(phaseID, phase->determineLaueGroup());
    materialNames->setValue(phaseID, phase->getMaterialName());
    QVector<float> lc = phase->getLatticeConstants();

    latticeConstants->setComponent(phaseID, 0, lc[0]);
    latticeConstants->setComponent(phaseID, 1, lc[1]);
    latticeConstants->setComponent(phaseID, 2, lc[2]);
    latticeConstants->setComponent(phaseID, 3, lc[3]);
    latticeConstants->setComponent(phaseID, 4, lc[4]);
    latticeConstants->setComponent(phaseID, 5, lc[5]);
  }
  DataContainer::Pointer vdc = getDataContainerArray()->getDataContainer(getDataContainerName());
  if(nullptr == vdc)
  {
    return -1;
  }
  AttributeMatrix::Pointer attrMatrix = vdc->getAttributeMatrix(getCellEnsembleAttributeMatrixName());
  if(nullptr == attrMatrix.get())
  {
    return -2;
  }

  // Resize the AttributeMatrix based on the size of the crystal structures array
  QVector<size_t> tDims(1, crystalStructures->getNumberOfTuples());
  attrMatrix->resizeAttributeArrays(tDims);
  // Now add the attributeArray to the AttributeMatrix
  attrMatrix->addAttributeArray(SIMPL::EnsembleData::CrystalStructures, crystalStructures);
  attrMatrix->addAttributeArray(SIMPL::EnsembleData::MaterialName, materialNames);
  attrMatrix->addAttributeArray(SIMPL::EnsembleData::LatticeConstants, latticeConstants);

  // Now reset the internal ensemble array references to these new arrays
  m_CrystalStructuresPtr = crystalStructures;
  if(nullptr != m_CrystalStructuresPtr.lock())
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  }

  m_LatticeConstantsPtr = latticeConstants;
  if(nullptr != m_LatticeConstantsPtr.lock())
  {
    m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0);
  }
  return 0;
}

// -----------------------------------------------------------------------------
template <typename T, class Reader>
void copyPointerData(Reader* reader, const QString& name, const IDataArray::Pointer& dataArray, size_t offset, size_t totalPoints, AttributeMatrix::Pointer& ebsdAttrMat)
{
  using DataArrayType = DataArray<T>;
  // Copy DD from Reader into DataArray<>
  T* ptr = reinterpret_cast<T*>(reader->getPointerByName(name));
  typename DataArrayType::Pointer fArray = std::dynamic_pointer_cast<DataArrayType>(dataArray);
  typename DataArrayType::Pointer freshArray = DataArrayType::WrapPointer(ptr, totalPoints, fArray->getComponentDimensions(), fArray->getName(), true);
  reader->releaseOwnership(name);
  ebsdAttrMat->addAttributeArray(freshArray->getName(), freshArray);
}

// -----------------------------------------------------------------------------
void ImportH5EspritData::copyRawEbsdData(EbsdReader* ebsdReader, QVector<size_t>& tDims, QVector<size_t>& cDims, int index)
{
  FloatArrayType::Pointer fArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer ebsdAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());

  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();
  size_t totalPoints = imageGeom->getXPoints() * imageGeom->getYPoints();

  // Prepare the Cell Attribute Matrix with the correct number of tuples based on the total points being read from the file.
  tDims.resize(3);
  tDims[0] = imageGeom->getXPoints();
  tDims[1] = imageGeom->getYPoints();
  tDims[2] = imageGeom->getZPoints();

  // Temporarily pull out all the attribute Arrays. This will save us doubling up on memory
  QList<QString> arrayNames = ebsdAttrMat->getAttributeArrayNames();
  std::map<QString, IDataArray::Pointer> arrays;
  for(const auto& name : arrayNames)
  {
    arrays[name] = ebsdAttrMat->removeAttributeArray(name);
  }

  // Now resize the AttributeMatrix.
  ebsdAttrMat->resizeAttributeArrays(tDims);

  size_t offset = index * totalPoints;

  auto ebsdArrayMap = getEbsdArrayMap();

  auto reader = dynamic_cast<H5EspritReader*>(ebsdReader);
  // Condense the Euler Angles from 3 separate arrays into a single 1x3 array
  auto f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::H5Esprit::phi1));
  auto f2 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::H5Esprit::PHI));
  auto f3 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::H5Esprit::phi2));
  cDims[0] = 3;
  fArray = std::dynamic_pointer_cast<FloatArrayType>(ebsdArrayMap.value(SIMPL::CellData::EulerAngles));
  float* cellEulerAngles = fArray->getTuplePointer(offset);

  for(size_t i = 0; i < totalPoints; i++)
  {
    cellEulerAngles[3 * i] = f1[i];
    cellEulerAngles[3 * i + 1] = f2[i];
    cellEulerAngles[3 * i + 2] = f3[i];
  }
  ebsdAttrMat->addAttributeArray(SIMPL::CellData::EulerAngles, fArray);
  reader->deallocateArrayData<float>(f1);
  reader->deallocateArrayData<float>(f2);
  reader->deallocateArrayData<float>(f3);
  reader->releasephi1Ownership();
  reader->releasePHIOwnership();
  reader->releasephi2Ownership();

  cDims[0] = 1;

  // Copy the rest of the data from the pointer in the reader into our IDataArrayPointer
  copyPointerData<Ebsd::H5Esprit::DD_t, H5EspritReader>(reader, Ebsd::H5Esprit::DD, ebsdArrayMap.value(Ebsd::H5Esprit::DD), offset, totalPoints, ebsdAttrMat);
  copyPointerData<Ebsd::H5Esprit::MAD_t, H5EspritReader>(reader, Ebsd::H5Esprit::MAD, ebsdArrayMap.value(Ebsd::H5Esprit::MAD), offset, totalPoints, ebsdAttrMat);
  copyPointerData<Ebsd::H5Esprit::MADPhase_t, H5EspritReader>(reader, Ebsd::H5Esprit::MADPhase, ebsdArrayMap.value(Ebsd::H5Esprit::MADPhase), offset, totalPoints, ebsdAttrMat);
  copyPointerData<Ebsd::H5Esprit::NIndexedBands_t, H5EspritReader>(reader, Ebsd::H5Esprit::NIndexedBands, ebsdArrayMap.value(Ebsd::H5Esprit::NIndexedBands), offset, totalPoints, ebsdAttrMat);
  copyPointerData<Ebsd::H5Esprit::PCX_t, H5EspritReader>(reader, Ebsd::H5Esprit::PCX, ebsdArrayMap.value(Ebsd::H5Esprit::PCX), offset, totalPoints, ebsdAttrMat);
  copyPointerData<Ebsd::H5Esprit::PCY_t, H5EspritReader>(reader, Ebsd::H5Esprit::PCY, ebsdArrayMap.value(Ebsd::H5Esprit::PCY), offset, totalPoints, ebsdAttrMat);
  copyPointerData<Ebsd::H5Esprit::Phase_t, H5EspritReader>(reader, Ebsd::H5Esprit::Phase, ebsdArrayMap.value(Ebsd::H5Esprit::Phase), offset, totalPoints, ebsdAttrMat);
  copyPointerData<Ebsd::H5Esprit::RadonBandCount_t, H5EspritReader>(reader, Ebsd::H5Esprit::RadonBandCount, ebsdArrayMap.value(Ebsd::H5Esprit::RadonBandCount), offset, totalPoints, ebsdAttrMat);
  copyPointerData<Ebsd::H5Esprit::RadonQuality_t, H5EspritReader>(reader, Ebsd::H5Esprit::RadonQuality, ebsdArrayMap.value(Ebsd::H5Esprit::RadonQuality), offset, totalPoints, ebsdAttrMat);
  copyPointerData<Ebsd::H5Esprit::XBEAM_t, H5EspritReader>(reader, Ebsd::H5Esprit::XBEAM, ebsdArrayMap.value(Ebsd::H5Esprit::XBEAM), offset, totalPoints, ebsdAttrMat);
  copyPointerData<Ebsd::H5Esprit::YBEAM_t, H5EspritReader>(reader, Ebsd::H5Esprit::YBEAM, ebsdArrayMap.value(Ebsd::H5Esprit::YBEAM), offset, totalPoints, ebsdAttrMat);
  copyPointerData<Ebsd::H5Esprit::XSAMPLE_t, H5EspritReader>(reader, Ebsd::H5Esprit::XSAMPLE, ebsdArrayMap.value(Ebsd::H5Esprit::XSAMPLE), offset, totalPoints, ebsdAttrMat);
  copyPointerData<Ebsd::H5Esprit::YSAMPLE_t, H5EspritReader>(reader, Ebsd::H5Esprit::YSAMPLE, ebsdArrayMap.value(Ebsd::H5Esprit::YSAMPLE), offset, totalPoints, ebsdAttrMat);

  if(getReadPatternData()) // Get the pattern Data from the
  {
    copyPointerData<Ebsd::H5Esprit::RawPatterns_t, H5EspritReader>(reader, Ebsd::H5Esprit::RawPatterns, ebsdArrayMap.value(Ebsd::H5Esprit::RawPatterns), offset, totalPoints, ebsdAttrMat);
    //    uint8_t* ptr = reader->getPatternData();
    //    int32_t pDims[2] = {0, 0};
    //    reader->getPatternDims(pDims);

    //    if(pDims[0] != 0 && pDims[1] != 0)
    //    {
    //      QVector<size_t> pDimsV(2);
    //      pDimsV[0] = pDims[0];
    //      pDimsV[1] = pDims[1];

    //      UInt8ArrayType::Pointer patternData = std::dynamic_pointer_cast<UInt8ArrayType>(ebsdArrayMap.value(Ebsd::Ang::PatternData));
    //      ::memcpy(patternData->getPointer(offset), ptr, sizeof(uint8_t) * totalPoints);
    //      ebsdAttrMat->addAttributeArray(Ebsd::Ang::PatternData, patternData);

    //      // Remove the current PatternData array
    //      ebsdAttrMat->removeAttributeArray(Ebsd::Ang::PatternData);

    //      // Push in our own PatternData array
    //      ebsdAttrMat->addAttributeArray(patternData->getName(), patternData);
    //      // Set the readers pattern data pointer to nullptr so that reader does not "free" the memory
    //      reader->setPatternData(nullptr);
    //    }
  }
}
