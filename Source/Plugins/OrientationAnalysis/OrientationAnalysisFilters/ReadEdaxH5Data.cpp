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

#include "ReadEdaxH5Data.h"

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>

#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/TSL/H5OIMReader.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "OrientationAnalysis/FilterParameters/ReadEdaxH5DataFilterParameter.h"

/**
 * @brief The ReadEdaxH5DataPrivate class is a private implementation of the ReadEdaxH5Data class
 */
class ReadEdaxH5DataPrivate
{
  Q_DISABLE_COPY(ReadEdaxH5DataPrivate)
  Q_DECLARE_PUBLIC(ReadEdaxH5Data)
  ReadEdaxH5Data* const q_ptr;
  ReadEdaxH5DataPrivate(ReadEdaxH5Data* ptr);

  Ang_Private_Data m_Data;
  QStringList m_FileScanNames;
  QVector<int> m_PatternDims;

  QString m_InputFile_Cache;
  QDateTime m_TimeStamp_Cache;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadEdaxH5DataPrivate::ReadEdaxH5DataPrivate(ReadEdaxH5Data* ptr)
: q_ptr(ptr)
, m_InputFile_Cache("")
, m_TimeStamp_Cache(QDateTime())
{
}

// Include the MOC generated file for this class
#include "moc_ReadEdaxH5Data.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadEdaxH5Data::ReadEdaxH5Data()
: AbstractFilter()
, m_InputFile("")
, m_NumberOfScans(0)
, m_ZSpacing(0)
, m_DataContainerName(SIMPL::Defaults::ImageDataContainerName)
, m_CellEnsembleAttributeMatrixName(SIMPL::Defaults::CellEnsembleAttributeMatrixName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_ReadPatternData(false)
, m_FileWasRead(false)
, m_PhaseNameArrayName(SIMPL::CellData::Phases)
, m_MaterialNameArrayName(SIMPL::EnsembleData::MaterialName)
, m_RefFrameZDir(SIMPL::RefFrameZDir::UnknownRefFrameZDirection)
, m_Manufacturer(Ebsd::UnknownManufacturer)
, m_CellPhases(nullptr)
, m_CellEulerAngles(nullptr)
, m_CrystalStructures(nullptr)
, m_LatticeConstants(nullptr)
, d_ptr(new ReadEdaxH5DataPrivate(this))
{
  FloatVec3_t value;
  value.x = 0;
  value.y = 0;
  value.z = 0;
  m_Origin = value;

  setupFilterParameters();
  setPatternDims(QVector<int32_t>(2, 0)); // initialize this with a 2 element vector
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadEdaxH5Data::~ReadEdaxH5Data()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL_PIMPL_PROPERTY_DEF(ReadEdaxH5Data, Ang_Private_Data, Data)
SIMPL_PIMPL_PROPERTY_DEF(ReadEdaxH5Data, QStringList, FileScanNames)
SIMPL_PIMPL_PROPERTY_DEF(ReadEdaxH5Data, QVector<int32_t>, PatternDims)
SIMPL_PIMPL_PROPERTY_DEF(ReadEdaxH5Data, QString, InputFile_Cache)
SIMPL_PIMPL_PROPERTY_DEF(ReadEdaxH5Data, QDateTime, TimeStamp_Cache)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5Data::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, ReadEdaxH5Data, "*.h5 *.hdf5"));

  // parameters.push_back(DynamicChoiceFilterParameter::New("Scan Name", "ScanName", getScanName(), "FileScanNames", FilterParameter::Parameter));
  parameters.push_back(ReadEdaxH5DataFilterParameter::New("Scan Names", "SelectedScanNames", getSelectedScanNames(), "FileScanNames", FilterParameter::Parameter,
                                                          SIMPL_BIND_SETTER(ReadEdaxH5Data, this, SelectedScanNames), SIMPL_BIND_GETTER(ReadEdaxH5Data, this, SelectedScanNames)));

  parameters.push_back(SIMPL_NEW_DOUBLE_FP("Z Spacing (Microns)", ZSpacing, FilterParameter::Parameter, ReadEdaxH5Data));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin (XYZ)", Origin, FilterParameter::Parameter, ReadEdaxH5Data));

  parameters.push_back(SIMPL_NEW_BOOL_FP("Import Pattern Data", ReadPatternData, FilterParameter::Parameter, ReadEdaxH5Data));
  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container", DataContainerName, FilterParameter::CreatedArray, ReadEdaxH5Data));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix", CellAttributeMatrixName, FilterParameter::CreatedArray, ReadEdaxH5Data));
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Ensemble Attribute Matrix", CellEnsembleAttributeMatrixName, FilterParameter::CreatedArray, ReadEdaxH5Data));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5Data::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName()));
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
void ReadEdaxH5Data::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5Data::dataCheck()
{
  // Clear the array map
  m_EbsdArrayMap.clear();

  // Reset FileWasRead flag
  m_FileWasRead = false;

  setErrorCondition(0);
  DataArrayPath tempPath;

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
  if(getErrorCondition() < 0)
  {
    return;
  }
  tDims.resize(1);
  tDims[0] = 0;
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble);
  if(getErrorCondition() < 0)
  {
    return;
  }

  QFileInfo fi(m_InputFile);
  if(fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist: '%1'").arg(getInputFile());
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(m_InputFile.isEmpty() == true && m_Manufacturer == Ebsd::UnknownManufacturer)
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
    return;
  }

  if(m_ZSpacing <= 0)
  {
    QString ss = QObject::tr("The Z Spacing field contains a value that is non-positive.  The Z Spacing field must be set to a positive value.");
    setErrorCondition(-3);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(m_InputFile.isEmpty() == false) // User set a filename, so lets check it
  {
    QVector<size_t> cDims(3, 0);

    QString ext = fi.suffix();
    QVector<QString> names;
    int32_t comp = ext.compare(Ebsd::Ang::H5FileExt);
    if(comp == 0)
    {
      H5OIMReader::Pointer reader = H5OIMReader::New();
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

      if(getSelectedScanNames().size() > 0)
      {
        readDataFile(reader.get(), m, cDims, scanNames[0], ANG_HEADER_ONLY);

        // Update the size of the Cell Attribute Matrix now that the dimensions of the volume are known
        cellAttrMat->resizeAttributeArrays(cDims);
        AngFields angfeatures;
        names = angfeatures.getFilterFeatures<QVector<QString>>();
        cDims.resize(1);
        cDims[0] = 1;
        for(qint32 i = 0; i < names.size(); ++i)
        {
          if(reader->getPointerType(names[i]) == Ebsd::Int32)
          {
            cellAttrMat->createAndAddAttributeArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, names[i], 0, cDims);
            m_EbsdArrayMap.insert(names[i], cellAttrMat->getAttributeArray(names[i]));
          }
          else if(reader->getPointerType(names[i]) == Ebsd::Float)
          {
            cellAttrMat->createAndAddAttributeArray<DataArray<float>, AbstractFilter, float>(this, names[i], 0, cDims);
            m_EbsdArrayMap.insert(names[i], cellAttrMat->getAttributeArray(names[i]));
          }
        }
      }
      else
      {
        setErrorCondition(-996);
        QString ss = QObject::tr("At least one scan must be chosen.  Please select a scan from the list.").arg(ext);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
    }
    else
    {
      setErrorCondition(-997);
      QString ss = QObject::tr("The file extension '%1' was not recognized. The reader only recognizes the .h5 file extension").arg(ext);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    cDims.resize(1);
    cDims[0] = 3;
    tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), Ebsd::AngFile::EulerAngles);
    m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
        this, tempPath, 0, cDims);                   /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_CellEulerAnglesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    m_EbsdArrayMap.insert(Ebsd::AngFile::EulerAngles, getDataContainerArray()->getPrereqIDataArrayFromPath<FloatArrayType, AbstractFilter>(this, tempPath));

    cDims[0] = 1;
    tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), Ebsd::AngFile::Phases);
    m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
        this, tempPath, 0, cDims);              /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_CellPhasesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    m_EbsdArrayMap.insert(Ebsd::AngFile::Phases, getDataContainerArray()->getPrereqIDataArrayFromPath<Int32ArrayType, AbstractFilter>(this, tempPath));

    tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), Ebsd::AngFile::CrystalStructures);
    m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(
        this, tempPath, Ebsd::CrystalStructure::UnknownCrystalStructure, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_CrystalStructuresPtr.lock().get())                           /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    m_EbsdArrayMap.insert(Ebsd::AngFile::CrystalStructures, getDataContainerArray()->getPrereqIDataArrayFromPath<UInt32ArrayType, AbstractFilter>(this, tempPath));

    cDims[0] = 6;
    tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), Ebsd::AngFile::LatticeConstants);
    m_LatticeConstantsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
        this, tempPath, 0.0, cDims);                  /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_LatticeConstantsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    m_EbsdArrayMap.insert(Ebsd::AngFile::LatticeConstants, getDataContainerArray()->getPrereqIDataArrayFromPath<FloatArrayType, AbstractFilter>(this, tempPath));

    if(getReadPatternData())
    {
      cDims.resize(2);
      cDims[0] = getPatternDims().at(0);
      cDims[1] = getPatternDims().at(1);
      if(cDims[0] != 0 && cDims[1] != 0)
      {
        tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), Ebsd::Ang::PatternData);
        m_CellPatternDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(
            this, tempPath, 0, cDims);                   /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
        if(nullptr != m_CellPatternDataPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
        {
          m_CellPatternData = m_CellPatternDataPtr.lock()->getPointer(0);
        } /* Now assign the raw pointer to data from the DataArray<T> object */

        m_EbsdArrayMap.insert(Ebsd::Ang::PatternData, getDataContainerArray()->getPrereqIDataArrayFromPath<UInt8ArrayType, AbstractFilter>(this, tempPath));
      }
      else
      {
        setErrorCondition(-998);
        QString ss = QObject::tr("The filter parameter 'Read Pattern Data' has been enabled but there does not seem to be any pattern data in the file for the scan name selected");
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      }
    }

    StringDataArray::Pointer materialNames = StringDataArray::CreateArray(cellEnsembleAttrMat->getNumberOfTuples(), SIMPL::EnsembleData::MaterialName);
    cellEnsembleAttrMat->addAttributeArray(SIMPL::EnsembleData::MaterialName, materialNames);
    m_EbsdArrayMap.insert(SIMPL::EnsembleData::MaterialName, materialNames);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5Data::preflight()
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
void ReadEdaxH5Data::flushCache()
{
  setInputFile_Cache("");
  setTimeStamp_Cache(QDateTime());
  setData(Ang_Private_Data());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5Data::readDataFile(H5OIMReader* reader, DataContainer::Pointer m, QVector<size_t>& tDims, const QString& scanName, ANG_READ_FLAG flag)
{
  QFileInfo fi(m_InputFile);
  QDateTime timeStamp(fi.lastModified());
  if(flag == ANG_FULL_FILE)
  {
    setInputFile_Cache(""); // We need something to trigger the file read below
  }
  // Drop into this if statement if we need to read from a file
  if(m_InputFile != getInputFile_Cache() || getTimeStamp_Cache().isValid() == false || getTimeStamp_Cache() < timeStamp)
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
        m_FileWasRead = false;
        return;
      }
      else
      {
        m_FileWasRead = true;
      }
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
      Ang_Private_Data data;
      data.dims = tDims;
      data.resolution.push_back(reader->getXStep());
      data.resolution.push_back(reader->getYStep());
      data.resolution.push_back(zStep);
      data.origin.push_back(xOrigin);
      data.origin.push_back(yOrigin);
      data.origin.push_back(zOrigin);
      data.phases = reader->getPhaseVector();
      setData(data);

      int32_t patternDims[2] = {0, 0};
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
    m->getGeometryAs<ImageGeom>()->setResolution(getData().resolution[0], getData().resolution[1], getData().resolution[2]);
    m->getGeometryAs<ImageGeom>()->setOrigin(getData().origin[0], getData().origin[1], getData().origin[2]);
  }

  if(flag == ANG_FULL_FILE)
  {
    loadMaterialInfo(reader);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t ReadEdaxH5Data::loadMaterialInfo(H5OIMReader* reader)
{
  QVector<AngPhase::Pointer> phases = getData().phases;
  if(phases.size() == 0)
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

  for(size_t i = 0; i < phases.size(); i++)
  {
    int32_t phaseID = phases[i]->getPhaseIndex();
    crystalStructures->setValue(phaseID, phases[i]->determineCrystalStructure());
    materialNames->setValue(phaseID, phases[i]->getMaterialName());
    QVector<float> lc = phases[i]->getLatticeConstants();

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
  if(nullptr != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_LatticeConstantsPtr = latticeConstants;
  if(nullptr != m_LatticeConstantsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5Data::copyRawEbsdData(H5OIMReader* reader, QVector<size_t>& tDims, QVector<size_t>& cDims, int index)
{
  float* f1 = nullptr;
  float* f2 = nullptr;
  float* f3 = nullptr;
  int32_t* phasePtr = nullptr;

  FloatArrayType::Pointer fArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer ebsdAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());

  size_t totalPoints = m->getGeometryAs<ImageGeom>()->getXPoints() * m->getGeometryAs<ImageGeom>()->getYPoints();

  // Prepare the Cell Attribute Matrix with the correct number of tuples based on the total points being read from the file.
  tDims.resize(3);
  tDims[0] = m->getGeometryAs<ImageGeom>()->getXPoints();
  tDims[1] = m->getGeometryAs<ImageGeom>()->getYPoints();
  tDims[2] = m->getGeometryAs<ImageGeom>()->getZPoints();
  ebsdAttrMat->resizeAttributeArrays(tDims);

  size_t offset = index * totalPoints;

  // Adjust the values of the 'phase' data to correct for invalid values
  {
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
    ebsdAttrMat->addAttributeArray(SIMPL::CellData::Phases, iArray);
  }

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
    ebsdAttrMat->addAttributeArray(SIMPL::CellData::EulerAngles, fArray);
  }

  cDims[0] = 1;
  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::ImageQuality));
    fArray = std::dynamic_pointer_cast<FloatArrayType>(m_EbsdArrayMap.value(Ebsd::Ang::ImageQuality));
    ::memcpy(fArray->getPointer(offset), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->addAttributeArray(Ebsd::Ang::ImageQuality, fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::ConfidenceIndex));
    fArray = std::dynamic_pointer_cast<FloatArrayType>(m_EbsdArrayMap.value(Ebsd::Ang::ConfidenceIndex));
    ::memcpy(fArray->getPointer(offset), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->addAttributeArray(Ebsd::Ang::ConfidenceIndex, fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::SEMSignal));
    fArray = std::dynamic_pointer_cast<FloatArrayType>(m_EbsdArrayMap.value(Ebsd::Ang::SEMSignal));
    ::memcpy(fArray->getPointer(offset), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->addAttributeArray(Ebsd::Ang::SEMSignal, fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Fit));
    fArray = std::dynamic_pointer_cast<FloatArrayType>(m_EbsdArrayMap.value(Ebsd::Ang::Fit));
    ::memcpy(fArray->getPointer(offset), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->addAttributeArray(Ebsd::Ang::Fit, fArray);
  }

  if(getReadPatternData()) // Get the pattern Data from the
  {
    uint8_t* ptr = reader->getPatternData();
    int32_t pDims[2] = {0, 0};
    reader->getPatternDims(pDims);

    if(pDims[0] != 0 && pDims[1] != 0)
    {
      QVector<size_t> pDimsV(2);
      pDimsV[0] = pDims[0];
      pDimsV[1] = pDims[1];

      UInt8ArrayType::Pointer patternData = std::dynamic_pointer_cast<UInt8ArrayType>(m_EbsdArrayMap.value(Ebsd::Ang::PatternData));
      ::memcpy(patternData->getPointer(offset), ptr, sizeof(uint8_t) * totalPoints);
      ebsdAttrMat->addAttributeArray(Ebsd::Ang::PatternData, patternData);

      // Remove the current PatternData array
      ebsdAttrMat->removeAttributeArray(Ebsd::Ang::PatternData);

      // Push in our own PatternData array
      ebsdAttrMat->addAttributeArray(patternData->getName(), patternData);
      // Set the readers pattern data pointer to nullptr so that reader does not "free" the memory
      reader->setPatternData(nullptr);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadEdaxH5Data::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  H5OIMReader::Pointer reader = H5OIMReader::New();
  reader->setFileName(m_InputFile);
  QVector<size_t> tDims(3, 0);
  QVector<size_t> cDims(1, 1);
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer ebsdAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());
  ebsdAttrMat->setType(AttributeMatrix::Type::Cell);

  QStringList scanNames = getSelectedScanNames();

  for(int index = 0; index < scanNames.size(); index++)
  {
    QString currentScanName = scanNames[index];

    readDataFile(reader.get(), m, tDims, currentScanName, ANG_FULL_FILE);
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
    QFileInfo newFi(m_InputFile);
    QDateTime timeStamp(newFi.lastModified());

    if(m_InputFile == getInputFile_Cache() && getTimeStamp_Cache().isValid() && getTimeStamp_Cache() >= timeStamp)
    {
      setTimeStamp_Cache(timeStamp);
      setInputFile_Cache(m_InputFile);
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ReadEdaxH5Data::newFilterInstance(bool copyFilterParameters)
{
  ReadEdaxH5Data::Pointer filter = ReadEdaxH5Data::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters());
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadEdaxH5Data::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadEdaxH5Data::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadEdaxH5Data::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadEdaxH5Data::getGroupName()
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadEdaxH5Data::getSubGroupName()
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadEdaxH5Data::getHumanLabel()
{
  return "Import EDAX EBSD Data (.h5)";
}
