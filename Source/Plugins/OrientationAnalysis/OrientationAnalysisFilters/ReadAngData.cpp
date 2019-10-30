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

#include "ReadAngData.h"

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "EbsdLib/TSL/AngFields.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,
  AttributeMatrixID22 = 22,

  DataContainerID = 1
};

/**
 * @brief The ReadAngDataPrivate class is a private implementation of the ReadAngData class
 */
class ReadAngDataPrivate
{
  Q_DISABLE_COPY(ReadAngDataPrivate)
  Q_DECLARE_PUBLIC(ReadAngData)
  ReadAngData* const q_ptr;
  ReadAngDataPrivate(ReadAngData* ptr);

  Ang_Private_Data m_Data;

  QString m_InputFile_Cache;
  QDateTime m_TimeStamp_Cache;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadAngDataPrivate::ReadAngDataPrivate(ReadAngData* ptr)
: q_ptr(ptr)
, m_InputFile_Cache("")
, m_TimeStamp_Cache(QDateTime())
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadAngData::ReadAngData()
: m_DataContainerName(SIMPL::Defaults::ImageDataContainerName)
, m_CellEnsembleAttributeMatrixName(SIMPL::Defaults::CellEnsembleAttributeMatrixName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_FileWasRead(false)
, m_MaterialNameArrayName(SIMPL::EnsembleData::MaterialName)
, m_InputFile("")
, m_RefFrameZDir(SIMPL::RefFrameZDir::UnknownRefFrameZDirection)
, m_Manufacturer(Ebsd::OEM::Unknown)
, d_ptr(new ReadAngDataPrivate(this))
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadAngData::~ReadAngData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void ReadAngData::setData(const Ang_Private_Data& value)
{
  Q_D(ReadAngData);
  d->m_Data = value;
}

// -----------------------------------------------------------------------------
Ang_Private_Data ReadAngData::getData() const
{
  Q_D(const ReadAngData);
  return d->m_Data;
}

// -----------------------------------------------------------------------------
void ReadAngData::setInputFile_Cache(const QString& value)
{
  Q_D(ReadAngData);
  d->m_InputFile_Cache = value;
}

// -----------------------------------------------------------------------------
QString ReadAngData::getInputFile_Cache() const
{
  Q_D(const ReadAngData);
  return d->m_InputFile_Cache;
}

// -----------------------------------------------------------------------------
void ReadAngData::setTimeStamp_Cache(const QDateTime& value)
{
  Q_D(ReadAngData);
  d->m_TimeStamp_Cache = value;
}

// -----------------------------------------------------------------------------
QDateTime ReadAngData::getTimeStamp_Cache() const
{
  Q_D(const ReadAngData);
  return d->m_TimeStamp_Cache;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadAngData::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, ReadAngData, "*.ang"));
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", DataContainerName, FilterParameter::CreatedArray, ReadAngData));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Attribute Matrix", CellAttributeMatrixName, DataContainerName, FilterParameter::CreatedArray, ReadAngData));
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Ensemble Attribute Matrix", CellEnsembleAttributeMatrixName, DataContainerName, FilterParameter::CreatedArray, ReadAngData));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadAngData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readDataArrayPath("DataContainerName", getDataContainerName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setCellEnsembleAttributeMatrixName(reader->readString("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName()));
  setInputFile(reader->readString("InputFile", getInputFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadAngData::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadAngData::dataCheck()
{
  // Reset FileWasRead flag
  m_FileWasRead = false;

  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  QFileInfo fi(m_InputFile);

  if(m_InputFile.isEmpty() && m_Manufacturer == Ebsd::OEM::Unknown)
  {
    QString ss = QObject::tr("The input file must be set for property %1").arg("InputFile");
    setErrorCondition(-1, ss);
    return;
  }

  QString ext = fi.suffix().toLower();
  if(ext != Ebsd::Ang::FileExtLower)
  {
    QString ss = QObject::tr("The file extension '%1' was not recognized. The reader only recognizes the .ang file extension").arg(ext);
    setErrorCondition(-997, ss);
    return;
  }

  if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist: '%1'").arg(getInputFile());
    setErrorCondition(-388, ss);
    return;
  }

  // Reading the header worked, now start setting up our DataContainer
  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);
  std::vector<size_t> tDims(3, 0);

  // File is at least on the system with the proper extension, now try to read it.
  std::shared_ptr<AngReader> reader(new AngReader());
  readDataFile(reader.get(), m.get(), tDims, ANG_HEADER_ONLY);
  if(getErrorCode() < 0)
  {
    return;
  }

  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell, AttributeMatrixID21);
  if(getErrorCode() < 0)
  {
    return;
  }

  // Update the size of the Cell Attribute Matrix now that the dimensions of the volume are known
  cellAttrMat->resizeAttributeArrays(tDims);

  tDims.resize(1);
  tDims[0] = 1;
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix(this, getCellEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble, AttributeMatrixID22);
  if(getErrorCode() < 0)
  {
    return;
  }

  AngFields angfeatures;
  QVector<QString> names = angfeatures.getFilterFeatures<QVector<QString>>();

  for(const QString& name : names)
  {
    if(reader->getPointerType(name) == Ebsd::Int32)
    {
      cellAttrMat->createAndAddAttributeArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, name, 0, tDims);
    }
    else if(reader->getPointerType(name) == Ebsd::Float)
    {
      cellAttrMat->createAndAddAttributeArray<DataArray<float>, AbstractFilter, float>(this, name, 0, tDims);
    }
  }

  std::vector<size_t> cDims(1, 3);
  tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), Ebsd::AngFile::EulerAngles);
  m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims, "");
  if(nullptr != m_CellEulerAnglesPtr.lock())
  {
    m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
  }

  cDims[0] = 1;
  tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), Ebsd::AngFile::Phases);
  m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims, "");
  if(nullptr != m_CellPhasesPtr.lock())
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  }

  tempPath.update(getDataContainerName().getDataContainerName(), getCellEnsembleAttributeMatrixName(), Ebsd::AngFile::CrystalStructures);
  m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this, tempPath, Ebsd::CrystalStructure::UnknownCrystalStructure, cDims);
  if(nullptr != m_CrystalStructuresPtr.lock())
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  }

  cDims[0] = 6;
  tempPath.update(getDataContainerName().getDataContainerName(), getCellEnsembleAttributeMatrixName(), Ebsd::AngFile::LatticeConstants);
  m_LatticeConstantsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0.0, cDims);
  if(nullptr != m_LatticeConstantsPtr.lock())
  {
    m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0);
  }

  StringDataArray::Pointer materialNames = StringDataArray::CreateArray(cellEnsembleAttrMat->getNumberOfTuples(), SIMPL::EnsembleData::MaterialName, true);
  cellEnsembleAttrMat->insertOrAssign(materialNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadAngData::preflight()
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
void ReadAngData::flushCache()
{
  setInputFile_Cache("");
  setTimeStamp_Cache(QDateTime());
  setData(Ang_Private_Data());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadAngData::readDataFile(AngReader* reader, DataContainer* m, std::vector<size_t>& tDims, ANG_READ_FLAG flag)
{
  QFileInfo fi(m_InputFile);
  QDateTime timeStamp(fi.lastModified());
  if(flag == ANG_FULL_FILE)
  {
    setInputFile_Cache(""); // We need something to trigger the file read below
  }

  // Drop into this if statement if we need to read from a file
  if(m_InputFile != getInputFile_Cache() || !getTimeStamp_Cache().isValid() || getTimeStamp_Cache() < timeStamp)
  {
    float zStep = 1.0, xOrigin = 0.0f, yOrigin = 0.0f, zOrigin = 0.0f;
    size_t zDim = 1;

    reader->setFileName(m_InputFile);

    if(flag == ANG_HEADER_ONLY)
    {
      int32_t err = reader->readHeaderOnly();
      if(err < 0)
      {
        setErrorCondition(err, reader->getErrorMessage());
        setErrorCondition(getErrorCode(), "AngReader could not read the .ang file header.");
        m_FileWasRead = false;
        return;
      }

        m_FileWasRead = true;

      if(reader->getGrid().compare(Ebsd::Ang::HexGrid) == 0)
      {
        setErrorCondition(-1000, "DREAM.3D does not directly read HEX grid .ang files. Please use the 'Convert Hexagonal "
                                  "Grid Data to Square Grid Data (TSL - .ang)' filter first to batch convert the Hex grid files.");
        m_FileWasRead = false;
        return;
      }
    }
    else
    {
      int32_t err = reader->readFile();
      if(err < 0)
      {
        setErrorCondition(err, reader->getErrorMessage());
        setErrorCondition(getErrorCode(), "AngReader could not read the .ang file.");
        return;
      }
    }
    tDims[0] = reader->getXDimension();
    tDims[1] = reader->getYDimension();
    tDims[2] = zDim; // We are reading a single slice

      Ang_Private_Data data;
      data.dims = SizeVec3Type(tDims[0], tDims[1], tDims[2]);
      data.resolution[0] = (reader->getXStep());
      data.resolution[1] = (reader->getYStep());
      data.resolution[2] = (zStep);
      data.origin[0] = (xOrigin);
      data.origin[1] = (yOrigin);
      data.origin[2] = (zOrigin);
      data.phases = reader->getPhaseVector();

      QString header = reader->getOriginalHeader();
      if(header.contains("# TEM data") || header.contains("# File Created from ACOM RES results"))
      {
        data.units = 0;
      }
      else
      {
        data.units = 1;
      }

      setData(data);

      setInputFile_Cache(m_InputFile);

      QFileInfo newFi(m_InputFile);
      QDateTime timeStamp(newFi.lastModified());
      setTimeStamp_Cache(timeStamp);
  }
  else
  {
    m_FileWasRead = false;
  }

  // Read from cache

  Ang_Private_Data data = getData();
  tDims[0] = data.dims[0];
  tDims[1] = data.dims[1];
  tDims[2] = data.dims[2];
  ImageGeom::Pointer image = m->getGeometryAs<ImageGeom>();

  image->setDimensions(tDims[0], tDims[1], tDims[2]);
  image->setSpacing(data.resolution);
  image->setOrigin(data.origin);
  if(data.units == 0)
  {
    image->setUnits(IGeometry::LengthUnit::Nanometer);
  }
  else
  {
    image->setUnits(IGeometry::LengthUnit::Micrometer);
  }

  if(flag == ANG_FULL_FILE)
  {
    loadMaterialInfo(reader);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t ReadAngData::loadMaterialInfo(AngReader* reader)
{
  QVector<AngPhase::Pointer> phases = getData().phases;
  if(phases.empty())
  {
    setErrorCondition(reader->getErrorCode(), reader->getErrorMessage());
    return getErrorCode();
  }

  DataArray<uint32_t>::Pointer crystalStructures = DataArray<uint32_t>::CreateArray(phases.size() + 1, Ebsd::AngFile::CrystalStructures, true);
  StringDataArray::Pointer materialNames = StringDataArray::CreateArray(phases.size() + 1, Ebsd::AngFile::MaterialName);
  std::vector<size_t> cDims(1, 6);
  FloatArrayType::Pointer latticeConstants = FloatArrayType::CreateArray(phases.size() + 1, cDims, Ebsd::AngFile::LatticeConstants, true);

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

  for(const AngPhase::Pointer& phase : phases)
  // for(int i = 0; i < phases.size(); i++)
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
void ReadAngData::copyRawEbsdData(AngReader* reader, std::vector<size_t>& tDims, std::vector<size_t>& cDims)
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
    iArray = Int32ArrayType::CreateArray(tDims, cDims, SIMPL::CellData::Phases, true);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    ebsdAttrMat->insertOrAssign(iArray);
  }

  // Condense the Euler Angles from 3 separate arrays into a single 1x3 array
  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Phi1));
    f2 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Phi));
    f3 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Phi2));
    cDims[0] = 3;
    fArray = FloatArrayType::CreateArray(tDims, cDims, SIMPL::CellData::EulerAngles, true);
    float* cellEulerAngles = fArray->getPointer(0);

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
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::ImageQuality, true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->insertOrAssign(fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::ConfidenceIndex));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::ConfidenceIndex, true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->insertOrAssign(fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::SEMSignal));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::SEMSignal, true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->insertOrAssign(fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::Fit));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::Fit, true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->insertOrAssign(fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::XPosition));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::XPosition, true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->insertOrAssign(fArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Ebsd::Ang::YPosition));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::YPosition, true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    ebsdAttrMat->insertOrAssign(fArray);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadAngData::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  std::shared_ptr<AngReader> reader(new AngReader());
  std::vector<size_t> tDims(3, 0);
  std::vector<size_t> cDims(1, 1);
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer ebsdAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());
  ebsdAttrMat->setType(AttributeMatrix::Type::Cell);

  readDataFile(reader.get(), m.get(), tDims, ANG_FULL_FILE);
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
AbstractFilter::Pointer ReadAngData::newFilterInstance(bool copyFilterParameters) const
{
  ReadAngData::Pointer filter = ReadAngData::New();
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
QString ReadAngData::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadAngData::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadAngData::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadAngData::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ReadAngData::getUuid() const
{
  return QUuid("{b8e128a8-c2a3-5e6c-a7ad-e4fb864e5d40}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadAngData::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadAngData::getHumanLabel() const
{
  return "Import EDAX EBSD Data (.ang)";
}

// -----------------------------------------------------------------------------
ReadAngData::Pointer ReadAngData::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ReadAngData> ReadAngData::New()
{
  struct make_shared_enabler : public ReadAngData
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ReadAngData::getNameOfClass() const
{
  return QString("ReadAngData");
}

// -----------------------------------------------------------------------------
QString ReadAngData::ClassName()
{
  return QString("ReadAngData");
}

// -----------------------------------------------------------------------------
void ReadAngData::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ReadAngData::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void ReadAngData::setCellEnsembleAttributeMatrixName(const QString& value)
{
  m_CellEnsembleAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ReadAngData::getCellEnsembleAttributeMatrixName() const
{
  return m_CellEnsembleAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ReadAngData::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ReadAngData::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ReadAngData::setFileWasRead(bool value)
{
  m_FileWasRead = value;
}

// -----------------------------------------------------------------------------
bool ReadAngData::getFileWasRead() const
{
  return m_FileWasRead;
}

// -----------------------------------------------------------------------------
void ReadAngData::setMaterialNameArrayName(const QString& value)
{
  m_MaterialNameArrayName = value;
}

// -----------------------------------------------------------------------------
QString ReadAngData::getMaterialNameArrayName() const
{
  return m_MaterialNameArrayName;
}

// -----------------------------------------------------------------------------
void ReadAngData::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString ReadAngData::getInputFile() const
{
  return m_InputFile;
}

// -----------------------------------------------------------------------------
void ReadAngData::setRefFrameZDir(uint32_t value)
{
  m_RefFrameZDir = value;
}

// -----------------------------------------------------------------------------
uint32_t ReadAngData::getRefFrameZDir() const
{
  return m_RefFrameZDir;
}

// -----------------------------------------------------------------------------
void ReadAngData::setManufacturer(const Ebsd::OEM& value)
{
  m_Manufacturer = value;
}

// -----------------------------------------------------------------------------
Ebsd::OEM ReadAngData::getManufacturer() const
{
  return m_Manufacturer;
}
