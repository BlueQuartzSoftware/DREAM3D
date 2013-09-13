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


#include "DREAM3DLib/IOFilters/util/VolumeDataContainerReader.h"


#define INIT_SYNTH_VOLUME_CHECK(var, errCond) \
  if (m_##var <= 0) { QString ss = QObject::tr(":%1 must be a value > 0\n").arg( #var); addErrorMessage(getHumanLabel(), ss, errCond);}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializeSyntheticVolume::InitializeSyntheticVolume() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_ShapeTypesArrayName(DREAM3D::EnsembleData::ShapeTypes),
  m_InputFile(""),
  m_XVoxels(50),
  m_YVoxels(50),
  m_ZVoxels(50),
  m_XRes(0.15f),
  m_YRes(0.15f),
  m_ZRes(0.15f),
  m_GrainIds(NULL),
  m_CellPhases(NULL),
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
  QVector<FilterParameter::Pointer> parameters;

  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputFile( reader->readValue("InputFile", getInputFile() ) );
  setXVoxels( reader->readValue("XVoxels", getXVoxels() ) );
  setYVoxels( reader->readValue("YVoxels", getYVoxels() ) );
  setZVoxels( reader->readValue("ZVoxels", getZVoxels() ) );
  setXRes( reader->readValue("XRes", getXRes() ) );
  setYRes( reader->readValue("YRes", getYRes() ) );
  setZRes( reader->readValue("ZRes", getZRes() ) );
  setShapeTypes( reader->readValue("ShapeTypes", getShapeTypes() ) );
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
void InitializeSyntheticVolume::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  QString ss;
  VolumeDataContainer* m = getVolumeDataContainer();

  //Cell Data
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, int32_t, Int32ArrayType, -1, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, int32_t, Int32ArrayType, 0, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GoodVoxels, bool, BoolArrayType, true, voxels, 1)

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

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::preflight()
{
  QTextStream ss;
  UInt32ArrayType::Pointer shapeTypes = UInt32ArrayType::CreateArray(1, DREAM3D::EnsembleData::ShapeTypes);
  getVolumeDataContainer()->addCellEnsembleData(DREAM3D::EnsembleData::ShapeTypes, shapeTypes);

  dataCheck(true, 1, 1, 1);

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

  VolumeDataContainerReader::Pointer read_data = VolumeDataContainerReader::New();
  read_data->setHdfFileId(fileId);
  read_data->setReadCellData(false);
  read_data->setReadFieldData(false);
  read_data->setReadEnsembleData(true);
  read_data->setDataContainer(getVolumeDataContainer());
  read_data->preflight();
  if (read_data->getErrorCondition() < 0)
  {
    setErrorCondition(read_data->getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::execute()
{
  QTextStream ss;
  setErrorCondition(0);
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
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

  VolumeDataContainerReader::Pointer read_data = VolumeDataContainerReader::New();
  read_data->setHdfFileId(fileId);
  read_data->setReadCellData(false);
  read_data->setReadFieldData(false);
  read_data->setReadEnsembleData(true);
  read_data->setReadAllArrays(true);
  read_data->setDataContainer(getVolumeDataContainer());
  read_data->execute();

  m->setDimensions(m_XVoxels, m_YVoxels, m_ZVoxels);
  m->setResolution(m_XRes, m_YRes, m_ZRes);

  UInt32ArrayType::Pointer shapeTypes = UInt32ArrayType::FromStdVector(m_ShapeTypes, DREAM3D::EnsembleData::ShapeTypes);
  m->addCellEnsembleData(DREAM3D::EnsembleData::ShapeTypes, shapeTypes);

  int64_t totalPoints = m->getTotalPoints();
  int totalFields = m->getNumCellFieldTuples();
  int totalEnsembles = m_ShapeTypes.size();

  // Check to make sure we have all of our data arrays available or make them available.
  dataCheck(false, totalPoints, totalFields, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("InitializeSyntheticVolume Complete");
}

