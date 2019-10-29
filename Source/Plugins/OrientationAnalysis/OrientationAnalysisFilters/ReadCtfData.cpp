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

#include "ReadCtfData.h"

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>

#include "EbsdLib/HKL/CtfFields.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"
#include "ChangeAngleRepresentation.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,
  AttributeMatrixID22 = 22,

  DataContainerID = 1
};

/**
 * @brief The ReadCtfDataPrivate class is a private implementation of the ReadCtfData class
 */
class ReadCtfDataPrivate
{
  Q_DISABLE_COPY(ReadCtfDataPrivate)
  Q_DECLARE_PUBLIC(ReadCtfData)
  ReadCtfData* const q_ptr;
  ReadCtfDataPrivate(ReadCtfData* ptr);

  Ctf_Private_Data m_Data;

  QString m_InputFile_Cache;
  QDateTime m_TimeStamp_Cache;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadCtfDataPrivate::ReadCtfDataPrivate(ReadCtfData* ptr)
: q_ptr(ptr)
, m_InputFile_Cache("")
, m_TimeStamp_Cache(QDateTime())
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadCtfData::ReadCtfData()
: m_DegreesToRadians(true)
, m_EdaxHexagonalAlignment(true)
, m_DataContainerName(SIMPL::Defaults::ImageDataContainerName)
, m_CellEnsembleAttributeMatrixName(SIMPL::Defaults::CellEnsembleAttributeMatrixName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_FileWasRead(false)
, m_PhaseNameArrayName("")
, m_MaterialNameArrayName(SIMPL::EnsembleData::MaterialName)
, m_InputFile("")
, m_RefFrameZDir(SIMPL::RefFrameZDir::UnknownRefFrameZDirection)
, m_Manufacturer(Ebsd::OEM::Unknown)
, d_ptr(new ReadCtfDataPrivate(this))
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadCtfData::~ReadCtfData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void ReadCtfData::setData(const Ctf_Private_Data& value)
{
  Q_D(ReadCtfData);
  d->m_Data = value;
}

// -----------------------------------------------------------------------------
Ctf_Private_Data ReadCtfData::getData() const
{
  Q_D(const ReadCtfData);
  return d->m_Data;
}

// -----------------------------------------------------------------------------
void ReadCtfData::setInputFile_Cache(const QString& value)
{
  Q_D(ReadCtfData);
  d->m_InputFile_Cache = value;
}

// -----------------------------------------------------------------------------
QString ReadCtfData::getInputFile_Cache() const
{
  Q_D(const ReadCtfData);
  return d->m_InputFile_Cache;
}

// -----------------------------------------------------------------------------
void ReadCtfData::setTimeStamp_Cache(const QDateTime& value)
{
  Q_D(ReadCtfData);
  d->m_TimeStamp_Cache = value;
}

// -----------------------------------------------------------------------------
QDateTime ReadCtfData::getTimeStamp_Cache() const
{
  Q_D(const ReadCtfData);
  return d->m_TimeStamp_Cache;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadCtfData::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, ReadCtfData, "*.ctf"));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Convert Eulers to Radians", DegreesToRadians, FilterParameter::Parameter, ReadCtfData));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Convert Hexagonal X-Axis to Edax Standard", EdaxHexagonalAlignment, FilterParameter::Parameter, ReadCtfData));
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", DataContainerName, FilterParameter::CreatedArray, ReadCtfData));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Attribute Matrix", CellAttributeMatrixName, DataContainerName, FilterParameter::CreatedArray, ReadCtfData));
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Ensemble Attribute Matrix", CellEnsembleAttributeMatrixName, DataContainerName, FilterParameter::CreatedArray, ReadCtfData));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadCtfData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDegreesToRadians(reader->readValue("DegreesToRadians", getDegreesToRadians()));
  setEdaxHexagonalAlignment(reader->readValue("EdaxHexagonalAlignment", getEdaxHexagonalAlignment()));
  setDataContainerName(reader->readDataArrayPath("DataContainerName", getDataContainerName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setCellEnsembleAttributeMatrixName(reader->readString("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName()));
  setInputFile(reader->readString("InputFile", getInputFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadCtfData::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadCtfData::dataCheck()
{
  // Reset FileWasRead flag
  m_FileWasRead = false;

  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

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

  QFileInfo fi(m_InputFile);
  if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist: '%1'").arg(getInputFile());
    setErrorCondition(-388, ss);
  }

  if(m_InputFile.isEmpty() && m_Manufacturer == Ebsd::OEM::Unknown)
  {
    QString ss = QObject::tr("The input file must be set");
    setErrorCondition(-1, ss);
  }

  if(!m_InputFile.isEmpty()) // User set a filename, so lets check it
  {
    std::vector<size_t> tDims(3, 0);

    QString ext = fi.suffix().toLower();
    QVector<QString> names;
    if(ext == Ebsd::Ctf::FileExt)
    {
      std::shared_ptr<CtfReader> reader(new CtfReader());
      readDataFile(reader.get(), m.get(), tDims, CTF_HEADER_ONLY);

      // Update the size of the Cell Attribute Matrix now that the dimensions of the volume are known
      cellAttrMat->resizeAttributeArrays(tDims);
      CtfFields ctffeatures;
      names = ctffeatures.getFilterFeatures<QVector<QString>>();
      std::vector<size_t> cDims(1, 1);
      for(const auto& name : names)
      {
        if(reader->getPointerType(name) == Ebsd::Int32)
        {
          cellAttrMat->createAndAddAttributeArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, name, 0, cDims);
        }
        else if(reader->getPointerType(name) == Ebsd::Float)
        {
          cellAttrMat->createAndAddAttributeArray<DataArray<float>, AbstractFilter, float>(this, name, 0, cDims);
        }
      }
    }
    else
    {
      QString ss = QObject::tr("The file extension '%1' was not recognized. The reader only recognizes the .ctf file extension").arg(ext);
      setErrorCondition(-997, ss);
      return;
    }

    std::vector<size_t> cDims(1, 3);
    tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), Ebsd::CtfFile::EulerAngles);
    m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
        this, tempPath, 0, cDims);             /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_CellEulerAnglesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    cDims[0] = 1;
    tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), Ebsd::CtfFile::Phases);
    m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
        this, tempPath, 0, cDims);        /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_CellPhasesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    tempPath.update(getDataContainerName().getDataContainerName(), getCellEnsembleAttributeMatrixName(), Ebsd::CtfFile::CrystalStructures);
    m_CrystalStructuresPtr =
        getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this, tempPath, Ebsd::CrystalStructure::UnknownCrystalStructure, cDims);
    if(nullptr != m_CrystalStructuresPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    cDims[0] = 6;
    tempPath.update(getDataContainerName().getDataContainerName(), getCellEnsembleAttributeMatrixName(), Ebsd::CtfFile::LatticeConstants);
    m_LatticeConstantsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
        this, tempPath, 0.0, cDims);            /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_LatticeConstantsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    StringDataArray::Pointer materialNames = StringDataArray::CreateArray(cellEnsembleAttrMat->getNumberOfTuples(), Ebsd::CtfFile::MaterialName, true);
    cellEnsembleAttrMat->insertOrAssign(materialNames);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadCtfData::preflight()
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
void ReadCtfData::flushCache()
{
  setInputFile_Cache("");
  setTimeStamp_Cache(QDateTime());
  setData(Ctf_Private_Data());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadCtfData::readDataFile(CtfReader* reader, DataContainer* m, std::vector<size_t>& tDims, CTF_READ_FLAG flag)
{
  QFileInfo fi(m_InputFile);
  QDateTime timeStamp(fi.lastModified());
  if(flag == CTF_FULL_FILE)
  {
    setInputFile_Cache(""); // We need something to trigger the file read below
  }

  // Drop into this if statement if we need to read from a file
  if(m_InputFile != getInputFile_Cache() || !getTimeStamp_Cache().isValid() || getTimeStamp_Cache() < timeStamp)
  {
    float xOrigin = 0.0f, yOrigin = 0.0f, zOrigin = 0.0f, zStep = 1.0f;
    reader->setFileName(m_InputFile);

    if(flag == CTF_HEADER_ONLY)
    {
      int32_t err = reader->readHeaderOnly();
      if(err < 0)
      {
        setErrorCondition(err, "CtfReader could not read the .ctf file header.");
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
        setErrorCondition(getErrorCode(), "CtfReader could not read the .ctf file.");
        return;
      }
    }

    tDims[0] = reader->getXCells();
    tDims[1] = reader->getYCells();
    tDims[2] = reader->getZCells(); // With CTF files there can be more than a single slice

    // Set Cache with values from the file
    {
      Ctf_Private_Data data;
      data.dims = SizeVec3Type(tDims[0], tDims[1], tDims[2]);
      data.resolution[0] = (reader->getXStep());
      data.resolution[1] = (reader->getYStep());
      if(reader->getZStep() != 0.0f)
      {
        data.resolution[2] = (reader->getZStep());
      }
      else
      {
        data.resolution[2] = (zStep);
      }
      data.origin[0] = (xOrigin);
      data.origin[1] = (yOrigin);
      data.origin[2] = (zOrigin);
      data.phases = reader->getPhaseVector();
      setData(data);

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
    m->getGeometryAs<ImageGeom>()->setDimensions(SizeVec3Type(tDims[0], tDims[1], tDims[2]));
    m->getGeometryAs<ImageGeom>()->setSpacing(getData().resolution);
    m->getGeometryAs<ImageGeom>()->setOrigin(getData().origin);
  }

  if(flag == CTF_FULL_FILE)
  {
    loadMaterialInfo(reader);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t ReadCtfData::loadMaterialInfo(CtfReader* reader)
{
  QVector<CtfPhase::Pointer> phases = getData().phases;
  if(phases.empty())
  {
    setErrorCondition(reader->getErrorCode(), reader->getErrorMessage());
    return getErrorCode();
  }

  DataArray<uint32_t>::Pointer crystalStructures = DataArray<uint32_t>::CreateArray(phases.size() + 1, Ebsd::CtfFile::CrystalStructures, true);
  StringDataArray::Pointer materialNames = StringDataArray::CreateArray(phases.size() + 1, getMaterialNameArrayName());
  std::vector<size_t> cDims(1, 6);
  FloatArrayType::Pointer latticeConstants = FloatArrayType::CreateArray(phases.size() + 1, cDims, Ebsd::CtfFile::LatticeConstants, true);

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

  // for(int32_t i = 0; i < phases.size(); i++)
  for(const auto& phase : phases)
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
  if(nullptr != m_CrystalStructuresPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_LatticeConstantsPtr = latticeConstants;
  if(nullptr != m_LatticeConstantsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadCtfData::copyRawEbsdData(CtfReader* reader, std::vector<size_t>& tDims, std::vector<size_t>& cDims)
{
  float* f1 = nullptr;
  float* f2 = nullptr;
  float* f3 = nullptr;
  int* phasePtr = nullptr;

  FloatArrayType::Pointer fArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer ebsdAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());

  size_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();

  // Prepare the Cell Attribute Matrix with the correct number of tuples based on the total points being read from the file.
  tDims.resize(3);
  tDims[0] = m->getGeometryAs<ImageGeom>()->getXPoints();
  tDims[1] = m->getGeometryAs<ImageGeom>()->getYPoints();
  tDims[2] = m->getGeometryAs<ImageGeom>()->getZPoints();
  ebsdAttrMat->resizeAttributeArrays(tDims);
  {
    /* Take from H5CtfVolumeReader.cpp
     * For HKL OIM Files if there is a single phase then the value of the phase
     * data is one (1). If there are 2 or more phases then the lowest value
     * of phase is also one (1). However, if there are "zero solutions" in the data
     * then those points are assigned a phase of zero.  Since those points can be identified
     * by other methods, the phase of these points should be changed to one since in the rest
     * of the reconstruction code we follow the convention that the lowest value is One (1)
     * even if there is only a single phase. The next if statement converts all zeros to ones
     * if there is a single phase in the OIM data.
     */
    phasePtr = reinterpret_cast<int32_t*>(reader->getPointerByName(Ebsd::Ctf::Phase));
    for(size_t i = 0; i < totalPoints; i++)
    {
      if(phasePtr[i] < 1)
      {
        phasePtr[i] = 1;
      }
    }
    iArray = Int32ArrayType::CreateArray(totalPoints, SIMPL::CellData::Phases, true);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    ebsdAttrMat->insertOrAssign(iArray);
  }
  {
    //  radianconversion = M_PI / 180.0;
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ctf::Euler1));
    f2 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ctf::Euler2));
    f3 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ctf::Euler3));
    std::vector<size_t> dims(1, 3);
    fArray = FloatArrayType::CreateArray(totalPoints, dims, SIMPL::CellData::EulerAngles, true);
    float* cellEulerAngles = fArray->getPointer(0);
    int32_t* cellPhases = iArray->getPointer(0);

    for(size_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i];
      cellEulerAngles[3 * i + 1] = f2[i];
      cellEulerAngles[3 * i + 2] = f3[i];
      if(m_CrystalStructures[cellPhases[i]] == Ebsd::CrystalStructure::Hexagonal_High && m_EdaxHexagonalAlignment)
      {
        cellEulerAngles[3 * i + 2] = cellEulerAngles[3 * i + 2] + (30.0); // See the documentation for this correction factor
      }
      // Now convert to radians if requested by the user
      if(m_DegreesToRadians)
      {
        cellEulerAngles[3 * i] = cellEulerAngles[3 * i] * SIMPLib::Constants::k_PiOver180;
        cellEulerAngles[3 * i + 1] = cellEulerAngles[3 * i + 1] * SIMPLib::Constants::k_PiOver180;
        cellEulerAngles[3 * i + 2] = cellEulerAngles[3 * i + 2] * SIMPLib::Constants::k_PiOver180;
      }
    }
    ebsdAttrMat->insertOrAssign(fArray);
  }

  {
    phasePtr = reinterpret_cast<int32_t*>(reader->getPointerByName(Ebsd::Ctf::Bands));
    iArray = Int32ArrayType::CreateArray(totalPoints, Ebsd::Ctf::Bands, true);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    ebsdAttrMat->insertOrAssign(iArray);
  }

  {
    phasePtr = reinterpret_cast<int32_t*>(reader->getPointerByName(Ebsd::Ctf::Error));
    iArray = Int32ArrayType::CreateArray(totalPoints, Ebsd::Ctf::Error, true);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    ebsdAttrMat->insertOrAssign(iArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ctf::MAD));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Ctf::MAD, true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->insertOrAssign(fArray);
  }

  {
    phasePtr = reinterpret_cast<int32_t*>(reader->getPointerByName(Ebsd::Ctf::BC));
    iArray = Int32ArrayType::CreateArray(totalPoints, Ebsd::Ctf::BC, true);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    ebsdAttrMat->insertOrAssign(iArray);
  }

  {
    phasePtr = reinterpret_cast<int32_t*>(reader->getPointerByName(Ebsd::Ctf::BS));
    iArray = Int32ArrayType::CreateArray(totalPoints, Ebsd::Ctf::BS, true);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    ebsdAttrMat->insertOrAssign(iArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ctf::X));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ctf::X, true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->insertOrAssign(fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ctf::Y));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ctf::Y, true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->insertOrAssign(fArray);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadCtfData::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  std::shared_ptr<CtfReader> reader(new CtfReader());
  std::vector<size_t> tDims(3, 0);
  std::vector<size_t> cDims(1, 1);
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer ebsdAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());
  ebsdAttrMat->setType(AttributeMatrix::Type::Cell);

  readDataFile(reader.get(), m.get(), tDims, CTF_FULL_FILE);
  if(getErrorCode() < 0)
  {
    return;
  }

  copyRawEbsdData(reader.get(), tDims, cDims);

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
AbstractFilter::Pointer ReadCtfData::newFilterInstance(bool copyFilterParameters) const
{
  ReadCtfData::Pointer filter = ReadCtfData::New();
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
QString ReadCtfData::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadCtfData::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadCtfData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadCtfData::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ReadCtfData::getUuid() const
{
  return QUuid("{d1df969c-0428-53c3-b61d-99ea2bb6da28}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadCtfData::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadCtfData::getHumanLabel() const
{
  return "Import Oxford Instr. EBSD Data (.ctf)";
}

// -----------------------------------------------------------------------------
ReadCtfData::Pointer ReadCtfData::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ReadCtfData> ReadCtfData::New()
{
  struct make_shared_enabler : public ReadCtfData
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ReadCtfData::getNameOfClass() const
{
  return QString("ReadCtfData");
}

// -----------------------------------------------------------------------------
QString ReadCtfData::ClassName()
{
  return QString("ReadCtfData");
}

// -----------------------------------------------------------------------------
void ReadCtfData::setDegreesToRadians(bool value)
{
  m_DegreesToRadians = value;
}

// -----------------------------------------------------------------------------
bool ReadCtfData::getDegreesToRadians() const
{
  return m_DegreesToRadians;
}

// -----------------------------------------------------------------------------
void ReadCtfData::setEdaxHexagonalAlignment(bool value)
{
  m_EdaxHexagonalAlignment = value;
}

// -----------------------------------------------------------------------------
bool ReadCtfData::getEdaxHexagonalAlignment() const
{
  return m_EdaxHexagonalAlignment;
}

// -----------------------------------------------------------------------------
void ReadCtfData::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ReadCtfData::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void ReadCtfData::setCellEnsembleAttributeMatrixName(const QString& value)
{
  m_CellEnsembleAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ReadCtfData::getCellEnsembleAttributeMatrixName() const
{
  return m_CellEnsembleAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ReadCtfData::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ReadCtfData::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ReadCtfData::setFileWasRead(bool value)
{
  m_FileWasRead = value;
}

// -----------------------------------------------------------------------------
bool ReadCtfData::getFileWasRead() const
{
  return m_FileWasRead;
}

// -----------------------------------------------------------------------------
void ReadCtfData::setPhaseNameArrayName(const QString& value)
{
  m_PhaseNameArrayName = value;
}

// -----------------------------------------------------------------------------
QString ReadCtfData::getPhaseNameArrayName() const
{
  return m_PhaseNameArrayName;
}

// -----------------------------------------------------------------------------
void ReadCtfData::setMaterialNameArrayName(const QString& value)
{
  m_MaterialNameArrayName = value;
}

// -----------------------------------------------------------------------------
QString ReadCtfData::getMaterialNameArrayName() const
{
  return m_MaterialNameArrayName;
}

// -----------------------------------------------------------------------------
void ReadCtfData::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString ReadCtfData::getInputFile() const
{
  return m_InputFile;
}

// -----------------------------------------------------------------------------
void ReadCtfData::setRefFrameZDir(uint32_t value)
{
  m_RefFrameZDir = value;
}

// -----------------------------------------------------------------------------
uint32_t ReadCtfData::getRefFrameZDir() const
{
  return m_RefFrameZDir;
}

// -----------------------------------------------------------------------------
void ReadCtfData::setManufacturer(const Ebsd::OEM& value)
{
  m_Manufacturer = value;
}

// -----------------------------------------------------------------------------
Ebsd::OEM ReadCtfData::getManufacturer() const
{
  return m_Manufacturer;
}
