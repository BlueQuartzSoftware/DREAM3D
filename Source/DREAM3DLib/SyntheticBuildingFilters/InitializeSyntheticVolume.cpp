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
  if (m_##var <= 0) { QString ss = QObject::tr(":%1 must be a value > 0\n").arg( #var); addErrorMessage(getHumanLabel(), ss, errCond);}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializeSyntheticVolume::InitializeSyntheticVolume() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::HDF5::CellAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::HDF5::CellEnsembleAttributeMatrixName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_ShapeTypesArrayName(DREAM3D::EnsembleData::ShapeTypes),
  m_InputFile(""),
  m_XVoxels(50),
  m_YVoxels(50),
  m_ZVoxels(50),
  m_XRes(0.15f),
  m_YRes(0.15f),
  m_ZRes(0.15f)
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
void InitializeSyntheticVolume::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 1);
  //Cell Data
  m_FeatureIdsPtr = m->getPrereqArray<int32_t, InitializeSyntheticVolume>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellPhasesPtr = m->getPrereqArray<int32_t, InitializeSyntheticVolume>(this, m_CellAttributeMatrixName,  m_CellPhasesArrayName, -301, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_GoodVoxelsPtr = m->createNonPrereqArray<bool, InitializeSyntheticVolume>(this, m_CellAttributeMatrixName, m_GoodVoxelsArrayName, true, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_InputFile.isEmpty() == true)
  {
    QString ss = QObject::tr("The intput file must be set before executing this filter.\n");
    setErrorCondition(-800);
    addErrorMessage(getHumanLabel(), ss, -800);
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
    addErrorMessage(getHumanLabel(), ss, -801);
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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(m == NULL)
  {
    VolumeDataContainer::Pointer vdc = VolumeDataContainer::New();
    vdc->setName(getDataContainerName());
    getDataContainerArray()->pushBack(vdc);
  }

  hid_t fileId = QH5Utilities::openFile(m_InputFile, true); // Open the file Read Only
  if(fileId < 0)
  {

    QString ss = QObject::tr(": Error opening input file '%1'").arg(m_InputFile);
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  // This will make sure if we return early from this method that the HDF5 File is properly closed.
  HDF5ScopedFileSentinel scopedFileSentinel(&fileId, true);

  hid_t dcGid = H5Gopen(fileId, DREAM3D::HDF5::DataContainerName.toLatin1().data(), 0);

  scopedFileSentinel.addGroupId(&dcGid);

  DataContainerReader::Pointer read_data = DataContainerReader::New();
  read_data->setHdfFileId(fileId);
  read_data->setHdfGroupId(dcGid);
  read_data->setReadCellData(false);
  read_data->setReadCellFeatureData(false);
  read_data->setReadCellEnsembleData(true);
  read_data->setReadAllCellEnsembleArrays(true);
  read_data->setDataContainer(getDataContainerArray()->getDataContainer(getDataContainerName()).get());
  read_data->preflight();
  if (read_data->getErrorCondition() < 0)
  {
    setErrorCondition(read_data->getErrorCondition());
  }

  dataCheck(true, 1, 1, 1);

  UInt32ArrayType::Pointer shapeTypes = UInt32ArrayType::CreateArray(1, DREAM3D::EnsembleData::ShapeTypes);
  getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName())->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->addAttributeArray(DREAM3D::EnsembleData::ShapeTypes, shapeTypes);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::execute()
{

  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(m == NULL)
  {
    VolumeDataContainer::Pointer vdc = VolumeDataContainer::New();
    vdc->setName(getDataContainerName());
    getDataContainerArray()->pushBack(vdc);
    m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  }

  hid_t fileId = QH5Utilities::openFile(m_InputFile, true); // Open the file Read Only
  if(fileId < 0)
  {

    QString ss = QObject::tr(": Error opening input file '%1'").arg(m_InputFile);
    setErrorCondition(-150);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  // This will make sure if we return early from this method that the HDF5 File is properly closed.
  HDF5ScopedFileSentinel scopedFileSentinel(&fileId, true);

  hid_t dcGid = H5Gopen(fileId, DREAM3D::HDF5::DataContainerName.toLatin1().data(), 0);

  DataContainerReader::Pointer read_data = DataContainerReader::New();
  read_data->setHdfFileId(fileId);
  read_data->setHdfGroupId(dcGid);
  read_data->setReadVertexData(false);
  read_data->setReadEdgeData(false);
  read_data->setReadFaceData(false);
  read_data->setReadCellData(false);
  read_data->setReadCellFeatureData(false);
  read_data->setReadCellEnsembleData(true);
  read_data->setReadAllCellEnsembleArrays(true);
  read_data->setDataContainer(getDataContainerArray()->getDataContainer(getDataContainerName()).get());
  read_data->execute();

  m->setDimensions(m_XVoxels, m_YVoxels, m_ZVoxels);
  m->setResolution(m_XRes, m_YRes, m_ZRes);

  UInt32ArrayType::Pointer shapeTypes = UInt32ArrayType::FromQVector(m_ShapeTypes, DREAM3D::EnsembleData::ShapeTypes);
  m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName)->addAttributeArray(DREAM3D::EnsembleData::ShapeTypes, shapeTypes);
  int64_t totalPoints = m->getTotalPoints();
  int64_t totalEnsembles = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->getNumTuples();

  // Check to make sure we have all of our data arrays available or make them available.
  dataCheck(false, totalPoints, 0, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("InitializeSyntheticVolume Complete");
}

