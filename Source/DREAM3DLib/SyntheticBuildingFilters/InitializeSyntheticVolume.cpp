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
#include "InitializeSyntheticVolume.h"


#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/HDF5ScopedFileSentinel.h"


#include "DREAM3DLib/IOFilters/DataContainerReader.h"


#define INIT_SYNTH_VOLUME_CHECK(var, errCond) \
  if (m_##var <= 0) { QString ss = QObject::tr(":%1 must be a value > 0\n").arg( #var); notifyErrorMessage(getHumanLabel(), ss, errCond);}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializeSyntheticVolume::InitializeSyntheticVolume() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_ShapeTypesArrayName(DREAM3D::EnsembleData::ShapeTypes),
  m_InputFile(""),
  m_InputDataContainerName(""),
  m_InputEnsembleAttributeMatrixName(""),
  m_InputStatsAttributeArrayName(""),
  m_XVoxels(50),
  m_YVoxels(50),
  m_ZVoxels(50),
  m_XRes(0.15f),
  m_YRes(0.15f),
  m_ZRes(0.15f),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_GoodVoxels(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializeSyntheticVolume::~InitializeSyntheticVolume()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::setupFilterParameters()
{
  FilterParameterVector parameters;

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputFile(reader->readString("InputFile", getInputFile() ) );
  setXVoxels( reader->readValue("XVoxels", getXVoxels() ) );
  setYVoxels( reader->readValue("YVoxels", getYVoxels() ) );
  setZVoxels( reader->readValue("ZVoxels", getZVoxels() ) );
  setXRes( reader->readValue("XRes", getXRes() ) );
  setYRes( reader->readValue("YRes", getYRes() ) );
  setZRes( reader->readValue("ZRes", getZRes() ) );
  setShapeTypes( reader->readArray("ShapeTypes", getShapeTypes() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int InitializeSyntheticVolume::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("XVoxels", getXVoxels() );
  writer->writeValue("YVoxels", getYVoxels() );
  writer->writeValue("ZVoxels", getZVoxels() );
  writer->writeValue("XRes", getXRes() );
  writer->writeValue("YRes", getYRes() );
  writer->writeValue("ZRes", getZRes() );
  writer->writeValue("ShapeTypes", getShapeTypes() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix* cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), DREAM3D::AttributeMatrixType::CellEnsemble);
  if(getErrorCondition() < 0) { return; }

  QVector<int> dims(1, 1);
  //Cell Data
  m_FeatureIdsPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellPhasesPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_CellPhasesArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_GoodVoxelsPtr = cellAttrMat->createNonPrereqArray<DataArray<bool>, AbstractFilter>(this, m_GoodVoxelsArrayName, true, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_InputFile.isEmpty() == true)
  {
    QString ss = QObject::tr("The intput file must be set before executing this filter.\n");
    setErrorCondition(-800);
    notifyErrorMessage(getHumanLabel(), ss, -800);
  }

  INIT_SYNTH_VOLUME_CHECK(XVoxels, -5000);
  INIT_SYNTH_VOLUME_CHECK(YVoxels, -5001);
  INIT_SYNTH_VOLUME_CHECK(ZVoxels, -5002);
  INIT_SYNTH_VOLUME_CHECK(XRes, -5003);
  INIT_SYNTH_VOLUME_CHECK(YRes, -5004);
  INIT_SYNTH_VOLUME_CHECK(ZRes, -5005);

  if (m_ShapeTypes.size() ==  0)
  {
    QString ss = QObject::tr("No ShapeTypes have been set and a shape type for each phase.\n");
    setErrorCondition(-801);
    notifyErrorMessage(getHumanLabel(), ss, -801);
  }

  m->setDimensions(m_XVoxels, m_YVoxels, m_ZVoxels);
  m->setResolution(m_XRes, m_YRes, m_ZRes);
  m->setOrigin(0.0, 0.0, 0.0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::preflight()
{
  dataCheck();

  hid_t fileId = QH5Utilities::openFile(m_InputFile, true); // Open the file Read Only
  if(fileId < 0)
  {

    QString ss = QObject::tr(": Error opening input file '%1'").arg(m_InputFile);
    setErrorCondition(-150);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  // This will make sure if we return early from this method that the HDF5 File is properly closed.
  HDF5ScopedFileSentinel scopedFileSentinel(&fileId, true);
  hid_t dcaGid = H5Gopen(fileId, DREAM3D::StringConstants::DataContainerGroupName.toLatin1().data(), 0);
  scopedFileSentinel.addGroupId(&dcaGid);
  hid_t dcGid = H5Gopen(dcaGid, "VolumeDataContainer", 0);
  scopedFileSentinel.addGroupId(&dcGid);
  hid_t amGid = H5Gopen(dcGid, "CellEnsembleData", 0);
  scopedFileSentinel.addGroupId(&amGid);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName());
  cellEnsembleAttrMat->addAttributeArrayFromHDF5Path(amGid, "Statistics", true);
  cellEnsembleAttrMat->addAttributeArrayFromHDF5Path(amGid, "CrystalStructures", true);
  cellEnsembleAttrMat->addAttributeArrayFromHDF5Path(amGid, "PhaseTypes", true);

  QVector<int> dims(1, 1);
  cellEnsembleAttrMat->createAndAddAttributeArray<DataArray<uint32_t>, uint32_t>(DREAM3D::EnsembleData::ShapeTypes, 0, dims);

  QList<QString> check = cellEnsembleAttrMat->getAttributeArrayNameList();
  for(QList<QString>::iterator it = check.begin(); it != check.end(); ++it)
  {
    QString name = *(it);
    int stop = 0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName());

  cellAttrMat->resizeAttributeArrays(m->getTotalPoints());

  UInt32ArrayType::Pointer shapeTypes = UInt32ArrayType::FromQVector(m_ShapeTypes, DREAM3D::EnsembleData::ShapeTypes);
  cellEnsembleAttrMat->resizeAttributeArrays(shapeTypes->getNumberOfTuples());
  cellEnsembleAttrMat->addAttributeArray(DREAM3D::EnsembleData::ShapeTypes, shapeTypes);

  //Now read the data out of the HDF5 file
  hid_t fileId = QH5Utilities::openFile(m_InputFile, true); // Open the file Read Only
  if(fileId < 0)
  {

    QString ss = QObject::tr(": Error opening input file '%1'").arg(m_InputFile);
    setErrorCondition(-150);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  // This will make sure if we return early from this method that the HDF5 File is properly closed.
  HDF5ScopedFileSentinel scopedFileSentinel(&fileId, true);
  hid_t dcaGid = H5Gopen(fileId, DREAM3D::StringConstants::DataContainerGroupName.toLatin1().data(), 0);
  scopedFileSentinel.addGroupId(&dcaGid);
  hid_t dcGid = H5Gopen(dcaGid, "VolumeDataContainer", 0);
  scopedFileSentinel.addGroupId(&dcGid);
  hid_t amGid = H5Gopen(dcGid, "CellEnsembleData", 0);
  scopedFileSentinel.addGroupId(&amGid);

  cellEnsembleAttrMat->addAttributeArrayFromHDF5Path(amGid, "Statistics", false);
  cellEnsembleAttrMat->addAttributeArrayFromHDF5Path(amGid, "CrystalStructures", false);
  cellEnsembleAttrMat->addAttributeArrayFromHDF5Path(amGid, "PhaseTypes", false);

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "InitializeSyntheticVolume Complete");
}


