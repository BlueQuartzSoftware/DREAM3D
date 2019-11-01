/* ============================================================================
 * Copyright (c) 2009-2019 BlueQuartz Software, LLC
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

#include "ImportH5OimData.h"

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>

#include "H5Support/H5Lite.h"
#include "H5Support/H5ScopedSentinel.h"
#include "H5Support/H5Utilities.h"
#include "H5Support/QH5Lite.h"

#include "EbsdLib/BrukerNano/EspritConstants.h"
#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/TSL/H5OIMReader.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "OrientationAnalysis/FilterParameters/OEMEbsdScanSelectionFilterParameter.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,
  AttributeMatrixID22 = 22,

  //  DataArrayID31 = 31,
  //  DataArrayID32 = 32,
  //  DataArrayID33 = 33,
  //  DataArrayID34 = 34,
  //  DataArrayID35 = 35,
  //  DataArrayID36 = 36,
  //  DataArrayID37 = 37,

  DataContainerID = 1
};

/**
 * @brief The ImportH5OimDataPrivate class is a private implementation of the ImportH5OimData class
 */
class ImportH5OimDataPrivate
{
  Q_DISABLE_COPY(ImportH5OimDataPrivate)
  Q_DECLARE_PUBLIC(ImportH5OimData)
  ImportH5OimData* const q_ptr;
  ImportH5OimDataPrivate(ImportH5OimData* ptr);

  Ang_Private_Data m_Data;
  QStringList m_FileScanNames;
  QVector<int> m_PatternDims;

  QString m_InputFile_Cache;
  QDateTime m_TimeStamp_Cache;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportH5OimDataPrivate::ImportH5OimDataPrivate(ImportH5OimData* ptr)
: q_ptr(ptr)
, m_InputFile_Cache("")
, m_TimeStamp_Cache(QDateTime())
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportH5OimData::ImportH5OimData()
: m_InputFile("")
, m_ZSpacing(1.0f)
, m_DataContainerName("EBSD Data")
, m_CellEnsembleAttributeMatrixName("Phase Data")
, m_CellAttributeMatrixName("Scan Data")
, m_PhaseNameArrayName(SIMPL::CellData::Phases)
, m_MaterialNameArrayName(SIMPL::EnsembleData::MaterialName)
, m_RefFrameZDir(SIMPL::RefFrameZDir::UnknownRefFrameZDirection)
, m_Manufacturer(Ebsd::OEM::Unknown)
, d_ptr(new ImportH5OimDataPrivate(this))
{
  FloatVec3Type value;
  value[0] = 0;
  value[1] = 0;
  value[2] = 0;
  m_Origin = value;

  setPatternDims(QVector<int32_t>(2, 0)); // initialize this with a 2 element vector
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportH5OimData::~ImportH5OimData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void ImportH5OimData::setData(const Ang_Private_Data& value)
{
  Q_D(ImportH5OimData);
  d->m_Data = value;
}

// -----------------------------------------------------------------------------
Ang_Private_Data ImportH5OimData::getData() const
{
  Q_D(const ImportH5OimData);
  return d->m_Data;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setFileScanNames(const QStringList& value)
{
  Q_D(ImportH5OimData);
  d->m_FileScanNames = value;
}

// -----------------------------------------------------------------------------
QStringList ImportH5OimData::getFileScanNames() const
{
  Q_D(const ImportH5OimData);
  return d->m_FileScanNames;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setPatternDims(const QVector<int32_t>& value)
{
  Q_D(ImportH5OimData);
  d->m_PatternDims = value;
}

// -----------------------------------------------------------------------------
QVector<int32_t> ImportH5OimData::getPatternDims() const
{
  Q_D(const ImportH5OimData);
  return d->m_PatternDims;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setInputFile_Cache(const QString& value)
{
  Q_D(ImportH5OimData);
  d->m_InputFile_Cache = value;
}

// -----------------------------------------------------------------------------
QString ImportH5OimData::getInputFile_Cache() const
{
  Q_D(const ImportH5OimData);
  return d->m_InputFile_Cache;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setTimeStamp_Cache(const QDateTime& value)
{
  Q_D(ImportH5OimData);
  d->m_TimeStamp_Cache = value;
}

// -----------------------------------------------------------------------------
QDateTime ImportH5OimData::getTimeStamp_Cache() const
{
  Q_D(const ImportH5OimData);
  return d->m_TimeStamp_Cache;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportH5OimData::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, ImportH5OimData, "*.h5 *.hdf5"));

  // parameters.push_back(DynamicChoiceFilterParameter::New("Scan Name", "ScanName", getScanName(), "FileScanNames", FilterParameter::Parameter));
  parameters.push_back(OEMEbsdScanSelectionFilterParameter::New("Scan Names", "SelectedScanNames", getSelectedScanNames(), "FileScanNames", FilterParameter::Parameter,
                                                                SIMPL_BIND_SETTER(ImportH5OimData, this, SelectedScanNames), SIMPL_BIND_GETTER(ImportH5OimData, this, SelectedScanNames)));

  parameters.push_back(SIMPL_NEW_DOUBLE_FP("Z Spacing (Microns)", ZSpacing, FilterParameter::Parameter, ImportH5OimData));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin (XYZ)", Origin, FilterParameter::Parameter, ImportH5OimData));

  parameters.push_back(SIMPL_NEW_BOOL_FP("Import Pattern Data", ReadPatternData, FilterParameter::Parameter, ImportH5OimData));
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", DataContainerName, FilterParameter::CreatedArray, ImportH5OimData));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Attribute Matrix", CellAttributeMatrixName, DataContainerName, FilterParameter::CreatedArray, ImportH5OimData));
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Ensemble Attribute Matrix", CellEnsembleAttributeMatrixName, DataContainerName, FilterParameter::CreatedArray, ImportH5OimData));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportH5OimData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readDataArrayPath("DataContainerName", getDataContainerName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setCellEnsembleAttributeMatrixName(reader->readString("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName()));
  setInputFile(reader->readString("InputFile", getInputFile()));
  setZSpacing(reader->readValue("ZSpacing", getZSpacing()));
  setOrigin(reader->readFloatVec3("Origin", getOrigin()));
  setSelectedScanNames(reader->readStringList("SelectedScanNames", getSelectedScanNames()));
  setReadPatternData(reader->readValue("ReadPatternData", getReadPatternData()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportH5OimData::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportH5OimData::dataCheck()
{
  // Clear the array map
  m_EbsdArrayMap.clear();

  // Reset FileWasRead flag
  m_FileWasRead = false;

  clearErrorCode();
  clearWarningCode();

  if(m_InputFile.isEmpty())
  {
    QString ss = QObject::tr("The input file must be set for property %1").arg("InputFile");
    setErrorCondition(-380, ss);
    return;
  }

  // Do all the file checks
  QFileInfo fi(m_InputFile);
  if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist: '%1'").arg(getInputFile());
    setErrorCondition(-381, ss);
    return;
  }

  if(fi.suffix() != Ebsd::H5OIM::H5FileExt)
  {
    QString ss = QObject::tr("The file extension '%1' was not recognized. The reader only recognizes the .h5 file extension").arg(fi.suffix());
    setErrorCondition(-997, ss);
    return;
  }

  // Check the zSpacing
  if(m_ZSpacing <= 0)
  {
    QString ss = QObject::tr("The Z Spacing field contains a value that is non-positive.  The Z Spacing field must be set to a positive value.");
    setErrorCondition(-382, ss);
    return;
  }

  dataCheckOEM();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportH5OimData::preflight()
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
void ImportH5OimData::flushCache()
{
  setInputFile_Cache("");
  setTimeStamp_Cache(QDateTime());
  setData(Ang_Private_Data());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportH5OimData::readDataFile(EbsdReader* ebsdReader, DataContainer* m, std::vector<size_t>& tDims, const QString& scanName, ANG_READ_FLAG flag)
{

  auto reader = dynamic_cast<H5OIMReader*>(ebsdReader);
  QFileInfo fi(m_InputFile);
  QDateTime timeStamp(fi.lastModified());
  if(flag == ANG_FULL_FILE)
  {
    setInputFile_Cache(""); // We need something to trigger the file read below
  }
  // Drop into this if statement if we need to read from a file
  if(m_InputFile != getInputFile_Cache() || !getTimeStamp_Cache().isValid() || getTimeStamp_Cache() < timeStamp)
  {
    float zStep = static_cast<float>(getZSpacing()), xOrigin = getOrigin()[0], yOrigin = getOrigin()[1], zOrigin = getOrigin()[2];
    reader->setReadPatternData(getReadPatternData());

    // If the user has already set a Scan Name to read then we are good to go.
    reader->setHDF5Path(scanName);

    if(flag == ANG_HEADER_ONLY)
    {
      int err = reader->readHeaderOnly();
      if(err < 0)
      {
        setErrorCondition(err, reader->getErrorMessage());
        m_FileWasRead = false;
        return;
      }

      m_FileWasRead = true;
    }
    else
    {
      int32_t err = reader->readFile();
      if(err < 0)
      {
        setErrorCondition(err, reader->getErrorMessage());
        setErrorCondition(getErrorCode(), "H5OIMReader could not read the .h5 file.");
        return;
      }
    }
    tDims[0] = reader->getXDimension();
    tDims[1] = reader->getYDimension();
    tDims[2] = getSelectedScanNames().size();

    // Set Cache with values from the file
    {
      Ang_Private_Data data;
      data.dims = SizeVec3Type(tDims[0], tDims[1], tDims[2]);
      data.resolution[0] = (reader->getXStep());
      data.resolution[1] = (reader->getYStep());
      data.resolution[2] = (zStep);
      data.origin[0] = (xOrigin);
      data.origin[1] = (yOrigin);
      data.origin[2] = (zOrigin);
      data.phases = reader->getPhaseVector();
      setData(data);

      std::array<int32_t, 2> patternDims = {{0, 0}};
      reader->getPatternDims(patternDims);
      QVector<int32_t> pDims(2);
      pDims[0] = patternDims[0];
      pDims[1] = patternDims[1];
      setPatternDims(pDims);

      setInputFile_Cache(m_InputFile);

      QFileInfo newFi(m_InputFile);
      QDateTime timeStamp(newFi.lastModified());
      setTimeStamp_Cache(timeStamp);
    }
  }
  else
  {
    m_FileWasRead = false;
  }

  // Read from cache
  {
    tDims[0] = getData().dims[0];
    tDims[1] = getData().dims[1];
    tDims[2] = getData().dims[2];
    m->getGeometryAs<ImageGeom>()->setDimensions(tDims[0], tDims[1], tDims[2]);
    m->getGeometryAs<ImageGeom>()->setSpacing(getData().resolution);
    m->getGeometryAs<ImageGeom>()->setOrigin(getData().origin);
  }

  if(flag == ANG_FULL_FILE)
  {
    loadMaterialInfo(reader);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t ImportH5OimData::loadMaterialInfo(EbsdReader* ebsdReader)
{
  auto reader = dynamic_cast<H5OIMReader*>(ebsdReader);

  QVector<AngPhase::Pointer> phases = getData().phases;
  if(phases.empty())
  {
    setErrorCondition(reader->getErrorCode(), reader->getErrorMessage());
    return getErrorCode();
  }

  DataArray<uint32_t>::Pointer crystalStructures = DataArray<uint32_t>::CreateArray(phases.size() + 1, Ebsd::AngFile::CrystalStructures, true);
  StringDataArray::Pointer materialNames = StringDataArray::CreateArray(phases.size() + 1, Ebsd::AngFile::MaterialName);
  std::vector<size_t> dims(1, 6);
  FloatArrayType::Pointer latticeConstants = FloatArrayType::CreateArray(phases.size() + 1, dims, Ebsd::AngFile::LatticeConstants, true);

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
  std::vector<size_t> tDims(1, crystalStructures->getNumberOfTuples());
  attrMatrix->resizeAttributeArrays(tDims);
  // Now add the attributeArray to the AttributeMatrix
  attrMatrix->insertOrAssign(crystalStructures);
  attrMatrix->insertOrAssign(materialNames);
  attrMatrix->insertOrAssign(latticeConstants);

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
//
// -----------------------------------------------------------------------------
void ImportH5OimData::copyRawEbsdData(EbsdReader* ebsdReader, std::vector<size_t>& tDims, std::vector<size_t>& cDims, int index)
{
  auto reader = dynamic_cast<H5OIMReader*>(ebsdReader);

  float* f1 = nullptr;
  float* f2 = nullptr;
  float* f3 = nullptr;
  int32_t* phasePtr = nullptr;

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
  ebsdAttrMat->resizeAttributeArrays(tDims);

  size_t offset = index * totalPoints;

  // Adjust the values of the 'phase' data to correct for invalid values
  phasePtr = reinterpret_cast<int32_t*>(reader->getPointerByName(Ebsd::Ang::PhaseData));
  for(size_t i = 0; i < totalPoints; i++)
  {
    if(phasePtr[i] < 1)
    {
      phasePtr[i] = 1;
    }
  }
  iArray = std::dynamic_pointer_cast<Int32ArrayType>(m_EbsdArrayMap.value(SIMPL::CellData::Phases));
  ::memcpy(iArray->getPointer(offset), phasePtr, sizeof(int32_t) * totalPoints);
  ebsdAttrMat->insertOrAssign(iArray);

  // Condense the Euler Angles from 3 separate arrays into a single 1x3 array
  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Phi1));
    f2 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Phi));
    f3 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Phi2));
    cDims[0] = 3;
    fArray = std::dynamic_pointer_cast<FloatArrayType>(m_EbsdArrayMap.value(SIMPL::CellData::EulerAngles));
    float* cellEulerAngles = fArray->getTuplePointer(offset);

    for(size_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i];
      cellEulerAngles[3 * i + 1] = f2[i];
      cellEulerAngles[3 * i + 2] = f3[i];
    }
    ebsdAttrMat->insertOrAssign(fArray);
  }

  cDims[0] = 1;
  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::ImageQuality));
    fArray = std::dynamic_pointer_cast<FloatArrayType>(m_EbsdArrayMap.value(Ebsd::Ang::ImageQuality));
    ::memcpy(fArray->getPointer(offset), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->insertOrAssign(fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::ConfidenceIndex));
    fArray = std::dynamic_pointer_cast<FloatArrayType>(m_EbsdArrayMap.value(Ebsd::Ang::ConfidenceIndex));
    ::memcpy(fArray->getPointer(offset), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->insertOrAssign(fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::SEMSignal));
    fArray = std::dynamic_pointer_cast<FloatArrayType>(m_EbsdArrayMap.value(Ebsd::Ang::SEMSignal));
    ::memcpy(fArray->getPointer(offset), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->insertOrAssign(fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Fit));
    fArray = std::dynamic_pointer_cast<FloatArrayType>(m_EbsdArrayMap.value(Ebsd::Ang::Fit));
    ::memcpy(fArray->getPointer(offset), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->insertOrAssign(fArray);
  }

  if(getReadPatternData()) // Get the pattern Data from the
  {
    uint8_t* ptr = reader->getPatternData();
    std::array<int32_t, 2> pDims = {{0, 0}};
    reader->getPatternDims(pDims);

    if(pDims[0] != 0 && pDims[1] != 0)
    {
      std::vector<size_t> pDimsV(2);
      pDimsV[0] = pDims[0];
      pDimsV[1] = pDims[1];

      UInt8ArrayType::Pointer patternData = std::dynamic_pointer_cast<UInt8ArrayType>(m_EbsdArrayMap.value(Ebsd::Ang::PatternData));
      ::memcpy(patternData->getPointer(offset), ptr, sizeof(uint8_t) * totalPoints);
      ebsdAttrMat->insertOrAssign(patternData);

      // Remove the current PatternData array
      ebsdAttrMat->removeAttributeArray(Ebsd::Ang::PatternData);

      // Push in our own PatternData array
      ebsdAttrMat->insertOrAssign(patternData);
      // Set the readers pattern data pointer to nullptr so that reader does not "free" the memory
      reader->setPatternData(nullptr);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportH5OimData::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  H5OIMReader::Pointer reader = H5OIMReader::New();
  reader->setFileName(m_InputFile);
  std::vector<size_t> tDims(3, 0);
  std::vector<size_t> cDims(1, 1);
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer ebsdAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());
  ebsdAttrMat->setType(AttributeMatrix::Type::Cell);

  QStringList scanNames = getSelectedScanNames();

  for(int index = 0; index < scanNames.size(); index++)
  {
    QString currentScanName = scanNames[index];

    readDataFile(reader.get(), m.get(), tDims, currentScanName, ANG_FULL_FILE);
    if(getErrorCode() < 0)
    {
      return;
    }

    copyRawEbsdData(reader.get(), tDims, cDims, index);
    if(getErrorCode() < 0)
    {
      return;
    }
  }

  // Set the file name and time stamp into the cache, if we are reading from the file and after all the reading has been done
  {
    QFileInfo newFi(m_InputFile);
    QDateTime timeStamp(newFi.lastModified());

    if(m_InputFile == getInputFile_Cache() && getTimeStamp_Cache().isValid() && getTimeStamp_Cache() >= timeStamp)
    {
      setTimeStamp_Cache(timeStamp);
      setInputFile_Cache(m_InputFile);
    }
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::OEM ImportH5OimData::readManufacturer() const
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
  if(manufacturer == Ebsd::H5OIM::EDAX)
  {
    manuf = Ebsd::OEM::EDAX;
  }
  return manuf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportH5OimData::newFilterInstance(bool copyFilterParameters) const
{
  ImportH5OimData::Pointer filter = ImportH5OimData::New();
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
QString ImportH5OimData::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportH5OimData::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportH5OimData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportH5OimData::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ImportH5OimData::getUuid() const
{
  return QUuid("{3ff4701b-3a0c-52e3-910a-fa927aa6584c}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportH5OimData::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportH5OimData::getHumanLabel() const
{
  return "Import EDAX OIMAnalysis Data (.h5)";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportH5OimData::dataCheckOEM()
{

  // Read the manufacturer from the file
  Ebsd::OEM manfacturer = readManufacturer();
  setManufacturer(manfacturer);
  if(manfacturer != Ebsd::OEM::EDAX)
  {
    QString ss = QObject::tr("The manufacturer is not recognized as a valid entry.");
    setErrorCondition(-384, ss);
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);
  image->setUnits(IGeometry::LengthUnit::Micrometer);

  std::vector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell, AttributeMatrixID21);
  if(getErrorCode() < 0)
  {
    return;
  }
  tDims.resize(1);
  tDims[0] = 0;
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix(this, getCellEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble, AttributeMatrixID22);
  if(getErrorCode() < 0)
  {
    return;
  }

  DataArrayPath tempPath;

  std::vector<size_t> cDims(3, 0);

  H5OIMReader::Pointer reader = H5OIMReader::New();
  reader->setFileName(getInputFile());

  // We ALWAYS want to read the Scan Names from the file so that we can present that list to the user if needed.
  QStringList scanNames;
  int32_t err = reader->readScanNames(scanNames);
  if(err < 0)
  {
    setErrorCondition(err, reader->getErrorMessage());
    return;
  }
  setFileScanNames(scanNames);
  setNumberOfScans(scanNames.size());

  if(!getSelectedScanNames().empty())
  {
    readDataFile(reader.get(), m.get(), cDims, scanNames[0], ANG_HEADER_ONLY);

    // Update the size of the Cell Attribute Matrix now that the dimensions of the volume are known
    cellAttrMat->resizeAttributeArrays(cDims);
    AngFields angfeatures;
    QVector<QString> names = angfeatures.getFilterFeatures<QVector<QString>>();
    cDims.resize(1);
    cDims[0] = 1;
    for(const auto& name : names)
    {
      if(reader->getPointerType(name) == Ebsd::Int32)
      {
        cellAttrMat->createAndAddAttributeArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, name, 0, cDims);
        m_EbsdArrayMap.insert(name, cellAttrMat->getAttributeArray(name));
      }
      else if(reader->getPointerType(name) == Ebsd::Float)
      {
        cellAttrMat->createAndAddAttributeArray<DataArray<float>, AbstractFilter, float>(this, name, 0, cDims);
        m_EbsdArrayMap.insert(name, cellAttrMat->getAttributeArray(name));
      }
    }
  }
  else
  {
    QString ss = QObject::tr("At least one scan must be chosen.  Please select a scan from the list.");
    setErrorCondition(-996, ss);
    return;
  }

  cDims.resize(1);
  cDims[0] = 3;
  tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), Ebsd::AngFile::EulerAngles);
  m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims);
  if(nullptr != m_CellEulerAnglesPtr.lock())
  {
    m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
  }
  m_EbsdArrayMap.insert(Ebsd::AngFile::EulerAngles, getDataContainerArray()->getPrereqIDataArrayFromPath<FloatArrayType, AbstractFilter>(this, tempPath));

  cDims[0] = 1;
  tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), Ebsd::AngFile::Phases);
  m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims);
  if(nullptr != m_CellPhasesPtr.lock())
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  }
  m_EbsdArrayMap.insert(Ebsd::AngFile::Phases, getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType, AbstractFilter>(this, tempPath));

  tempPath.update(getDataContainerName().getDataContainerName(), getCellEnsembleAttributeMatrixName(), Ebsd::AngFile::CrystalStructures);
  m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this, tempPath, Ebsd::CrystalStructure::UnknownCrystalStructure, cDims);
  if(nullptr != m_CrystalStructuresPtr.lock())
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  }
  m_EbsdArrayMap.insert(Ebsd::AngFile::CrystalStructures, getDataContainerArray()->getPrereqIDataArrayFromPath<UInt32ArrayType, AbstractFilter>(this, tempPath));

  cDims[0] = 6;
  tempPath.update(getDataContainerName().getDataContainerName(), getCellEnsembleAttributeMatrixName(), Ebsd::AngFile::LatticeConstants);
  m_LatticeConstantsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0.0, cDims);
  if(nullptr != m_LatticeConstantsPtr.lock())
  {
    m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0);
  }
  m_EbsdArrayMap.insert(Ebsd::AngFile::LatticeConstants, getDataContainerArray()->getPrereqIDataArrayFromPath<FloatArrayType, AbstractFilter>(this, tempPath));

  if(getReadPatternData())
  {
    cDims.resize(2);
    cDims[0] = getPatternDims().at(0);
    cDims[1] = getPatternDims().at(1);
    if(cDims[0] != 0 && cDims[1] != 0)
    {
      tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), Ebsd::Ang::PatternData);
      m_CellPatternDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, cDims);
      if(nullptr != m_CellPatternDataPtr.lock())
      {
        m_CellPatternData = m_CellPatternDataPtr.lock()->getPointer(0);
      }

      m_EbsdArrayMap.insert(Ebsd::Ang::PatternData, getDataContainerArray()->getPrereqIDataArrayFromPath<UInt8ArrayType, AbstractFilter>(this, tempPath));
    }
    else
    {
      QString ss = QObject::tr("The filter parameter 'Read Pattern Data' has been enabled but there does not seem to be any pattern data in the file for the scan name selected");
      setErrorCondition(-998, ss);
    }
  }

  StringDataArray::Pointer materialNames = StringDataArray::CreateArray(cellEnsembleAttrMat->getNumberOfTuples(), SIMPL::EnsembleData::MaterialName, true);
  cellEnsembleAttrMat->insertOrAssign(materialNames);
  m_EbsdArrayMap.insert(SIMPL::EnsembleData::MaterialName, materialNames);
}

// -----------------------------------------------------------------------------
ImportH5OimData::Pointer ImportH5OimData::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ImportH5OimData> ImportH5OimData::New()
{
  struct make_shared_enabler : public ImportH5OimData
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ImportH5OimData::getNameOfClass() const
{
  return QString("ImportH5OimData");
}

// -----------------------------------------------------------------------------
QString ImportH5OimData::ClassName()
{
  return QString("ImportH5OimData");
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString ImportH5OimData::getInputFile() const
{
  return m_InputFile;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setSelectedScanNames(const QStringList& value)
{
  m_SelectedScanNames = value;
}

// -----------------------------------------------------------------------------
QStringList ImportH5OimData::getSelectedScanNames() const
{
  return m_SelectedScanNames;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setNumberOfScans(int value)
{
  m_NumberOfScans = value;
}

// -----------------------------------------------------------------------------
int ImportH5OimData::getNumberOfScans() const
{
  return m_NumberOfScans;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setZSpacing(double value)
{
  m_ZSpacing = value;
}

// -----------------------------------------------------------------------------
double ImportH5OimData::getZSpacing() const
{
  return m_ZSpacing;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setOrigin(const FloatVec3Type& value)
{
  m_Origin = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type ImportH5OimData::getOrigin() const
{
  return m_Origin;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ImportH5OimData::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setCellEnsembleAttributeMatrixName(const QString& value)
{
  m_CellEnsembleAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ImportH5OimData::getCellEnsembleAttributeMatrixName() const
{
  return m_CellEnsembleAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ImportH5OimData::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setReadPatternData(bool value)
{
  m_ReadPatternData = value;
}

// -----------------------------------------------------------------------------
bool ImportH5OimData::getReadPatternData() const
{
  return m_ReadPatternData;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setFileWasRead(bool value)
{
  m_FileWasRead = value;
}

// -----------------------------------------------------------------------------
bool ImportH5OimData::getFileWasRead() const
{
  return m_FileWasRead;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setPhaseNameArrayName(const QString& value)
{
  m_PhaseNameArrayName = value;
}

// -----------------------------------------------------------------------------
QString ImportH5OimData::getPhaseNameArrayName() const
{
  return m_PhaseNameArrayName;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setMaterialNameArrayName(const QString& value)
{
  m_MaterialNameArrayName = value;
}

// -----------------------------------------------------------------------------
QString ImportH5OimData::getMaterialNameArrayName() const
{
  return m_MaterialNameArrayName;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setRefFrameZDir(uint32_t value)
{
  m_RefFrameZDir = value;
}

// -----------------------------------------------------------------------------
uint32_t ImportH5OimData::getRefFrameZDir() const
{
  return m_RefFrameZDir;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setManufacturer(const Ebsd::OEM& value)
{
  m_Manufacturer = value;
}

// -----------------------------------------------------------------------------
Ebsd::OEM ImportH5OimData::getManufacturer() const
{
  return m_Manufacturer;
}

// -----------------------------------------------------------------------------
void ImportH5OimData::setEbsdArrayMap(const IDataArrayMap& value)
{
  m_EbsdArrayMap = value;
}

// -----------------------------------------------------------------------------
ImportH5OimData::IDataArrayMap ImportH5OimData::getEbsdArrayMap() const
{
  return m_EbsdArrayMap;
}
