/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "ReadH5Ebsd.h"

#include <limits>
#include <vector>
#include <sstream>

#include <QtCore/QFileInfo>

#include "EbsdLib/H5EbsdVolumeInfo.h"
#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/HKL/CtfFields.h"
#include "EbsdLib/HEDM/MicFields.h"

#include "EbsdLib/TSL/H5AngVolumeReader.h"
#include "EbsdLib/HKL/H5CtfVolumeReader.h"
#include "EbsdLib/HEDM/H5MicVolumeReader.h"

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/ProcessingFilters/RotateEulerRefFrame.h"
#include "DREAM3DLib/SamplingFilters/RotateSampleRefFrame.h"


#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1





#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadH5Ebsd::ReadH5Ebsd() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_LatticeConstantsArrayName(DREAM3D::EnsembleData::LatticeConstants),
  m_MaterialNameArrayName(DREAM3D::EnsembleData::MaterialName),
  m_InputFile(""),
  m_RefFrameZDir(Ebsd::UnknownRefFrameZDirection),
  m_ZStartIndex(0),
  m_ZEndIndex(0),
  m_UseTransformations(true),
  m_Manufacturer(Ebsd::UnknownManufacturer),
  m_SampleTransformationAngle(0.0),
  m_EulerTransformationAngle(0.0),
  m_CellPhases(NULL),
  m_CellEulerAngles(NULL),
  m_CrystalStructures(NULL),
  tempxpoints(0),
  tempypoints(0),
  totaltemppoints(0)
{
  m_SampleTransformationAxis.resize(3);
  m_SampleTransformationAxis[0] = 0.0;
  m_SampleTransformationAxis[1] = 0.0;
  m_SampleTransformationAxis[2] = 1.0;

  m_EulerTransformationAxis.resize(3);
  m_EulerTransformationAxis[0] = 0.0;
  m_EulerTransformationAxis[1] = 0.0;
  m_EulerTransformationAxis[2] = 1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadH5Ebsd::~ReadH5Ebsd()
{
}



// -----------------------------------------------------------------------------
void ReadH5Ebsd::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputFile(reader->readString("InputFile", getInputFile() ) );
  setZStartIndex( reader->readValue("ZStartIndex", getZStartIndex() ) );
  setZEndIndex( reader->readValue("ZEndIndex", getZEndIndex() ) );
  setUseTransformations( reader->readValue("UseTransformations", getUseTransformations() ) );
  setRefFrameZDir( static_cast<Ebsd::RefFrameZDir>( reader->readValue("RefFrameZDir", getRefFrameZDir() ) ) );


  // we need to support H5Ebsd Files from previous versions of DREAM3D where there was a "Voxel" data container so
  // try reading these values. If they are there then it is an older file and it will set the appropriate values
  setSelectedVolumeCellArrays( reader->readArraySelections("VoxelCell", getSelectedVolumeCellArrays() ) );
  setSelectedVolumeCellFeatureArrays( reader->readArraySelections("VoxelFeature", getSelectedVolumeCellFeatureArrays() ) );
  setSelectedVolumeCellEnsembleArrays( reader->readArraySelections("VoxelEnsemble", getSelectedVolumeCellEnsembleArrays() ) );

  // If we are reading a Version 4.3 file and above then the data container names have changed and the last set of
  // reads would have failed to read anything so those arrays are still empty. These functions will now read those
  // arrays from the file.
  READ_ARRAY_SELECTION_PARAMETER(VolumeVertex)
  READ_ARRAY_SELECTION_PARAMETER(VolumeEdge)
  READ_ARRAY_SELECTION_PARAMETER(VolumeFace)
  READ_ARRAY_SELECTION_PARAMETER(VolumeCell)
  READ_ARRAY_SELECTION_PARAMETER(VolumeCellFeature)
  READ_ARRAY_SELECTION_PARAMETER(VolumeCellEnsemble)

  READ_ARRAY_SELECTION_PARAMETER(SurfaceVertex)
  READ_ARRAY_SELECTION_PARAMETER(SurfaceEdge)
  READ_ARRAY_SELECTION_PARAMETER(SurfaceFace)
  READ_ARRAY_SELECTION_PARAMETER(SurfaceFaceFeature)
  READ_ARRAY_SELECTION_PARAMETER(SurfaceFaceEnsemble)

  READ_ARRAY_SELECTION_PARAMETER(EdgeVertex)
  READ_ARRAY_SELECTION_PARAMETER(EdgeEdge)
  READ_ARRAY_SELECTION_PARAMETER(EdgeEdgeFeature)
  READ_ARRAY_SELECTION_PARAMETER(EdgeEdgeEnsemble)

  READ_ARRAY_SELECTION_PARAMETER(VertexVertex)
  READ_ARRAY_SELECTION_PARAMETER(VertexVertexFeature)
  READ_ARRAY_SELECTION_PARAMETER(VertexVertexEnsemble)

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ReadH5Ebsd::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("ZStartIndex", getZStartIndex() );
  writer->writeValue("ZEndIndex", getZEndIndex() );
  writer->writeValue("UseTransformations", getUseTransformations() );
  writer->writeValue("RefFrameZDir", getRefFrameZDir());

  WRITE_ARRAY_SELECTION_PARAMETER(VolumeVertex)
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeEdge)
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeFace)
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeCell)
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeCellFeature)
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeCellEnsemble)

  WRITE_ARRAY_SELECTION_PARAMETER(SurfaceVertex)
  WRITE_ARRAY_SELECTION_PARAMETER(SurfaceEdge)
  WRITE_ARRAY_SELECTION_PARAMETER(SurfaceFace)
  WRITE_ARRAY_SELECTION_PARAMETER(SurfaceFaceFeature)
  WRITE_ARRAY_SELECTION_PARAMETER(SurfaceFaceEnsemble)

  WRITE_ARRAY_SELECTION_PARAMETER(EdgeVertex)
  WRITE_ARRAY_SELECTION_PARAMETER(EdgeEdge)
  WRITE_ARRAY_SELECTION_PARAMETER(EdgeEdgeFeature)
  WRITE_ARRAY_SELECTION_PARAMETER(EdgeEdgeEnsemble)

  WRITE_ARRAY_SELECTION_PARAMETER(VertexVertex)
  WRITE_ARRAY_SELECTION_PARAMETER(VertexVertexFeature)
  WRITE_ARRAY_SELECTION_PARAMETER(VertexVertexEnsemble)

  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ReadH5Ebsd::initDataContainerDimsRes(int64_t dims[3], VolumeDataContainer* m)
{
  int err = 0;
  /* Sanity check what we are trying to load to make sure it can fit in our address space.
   * Note that this does not guarantee the user has enough left, just that the
   * size of the volume can fit in the address space of the program
   */
#if   (CMP_SIZEOF_SSIZE_T==4)
  int64_t max = std::numeric_limits<size_t>::max();
#else
  int64_t max = std::numeric_limits<int64_t>::max();
#endif
  if(dims[0] * dims[1] * dims[2] > max)
  {
    err = -1;
    QString ss = QObject::tr("The total number of elements '%1' is greater than this program can hold. Try the 64 bit version.").arg((dims[0] * dims[1] * dims[2]));
    setErrorCondition(err);
    addErrorMessage(getHumanLabel(), ss, err);
    return err;
  }

  if(dims[0] > max || dims[1] > max || dims[2] > max)
  {
    err = -1;
    QString ss = QObject::tr("One of the dimensions is greater than the max index for this sysem. Try the 64 bit version."
                             " dim[0]=%1  dim[1]=%2  dim[2]=%3").arg(dims[0]).arg(dims[1]).arg(dims[2]);
    setErrorCondition(err);
    addErrorMessage(getHumanLabel(), ss, err);
    return err;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QFileInfo fi(m_InputFile);
  if (m_InputFile.isEmpty() == true && m_Manufacturer == Ebsd::UnknownManufacturer)
  {
    QString ss = QObject::tr("%1: The H5Ebsd file must exist and the Manufacturer must be set correctly in the file").arg(getHumanLabel());
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss, -1);
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist. '%1'").arg(getInputFile());
    setErrorCondition(-388);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (m_InputFile.isEmpty() == false)
  {
    H5EbsdVolumeInfo::Pointer reader = H5EbsdVolumeInfo::New();
    reader->setFileName(m_InputFile);
    int err = reader->readVolumeInfo();
    if (err < 0)
    {
      QString ss = QObject::tr("%1: Error reading VolumeInfo from H5Ebsd File").arg(getHumanLabel());
      setErrorCondition(-1);
      addErrorMessage(getHumanLabel(), ss, -1);
      return;
    }

    QString manufacturer = reader->getManufacturer();
    if(manufacturer.compare(Ebsd::Ang::Manufacturer) == 0)
    {
      m_Manufacturer = Ebsd::TSL;
    }
    else if(manufacturer.compare(Ebsd::Ctf::Manufacturer) == 0)
    {
      m_Manufacturer = Ebsd::HKL;
    }
    else if(manufacturer.compare(Ebsd::Mic::Manufacturer) == 0)
    {
      m_Manufacturer = Ebsd::HEDM;
    }
    else
    {
      QString ss = QObject::tr("%1:  Original Data source could not be determined. It should be TSL, HKL or HEDM").arg(getHumanLabel());
      setErrorCondition(-1);
      addErrorMessage(getHumanLabel(), ss, -1);
      return;
    }

    int64_t dims[3];
    float res[3];
    reader->getDimsAndResolution(dims[0], dims[1], dims[2], res[0], res[1], res[2]);
    /* Sanity check what we are trying to load to make sure it can fit in our address space.
     * Note that this does not guarantee the user has enough left, just that the
     * size of the volume can fit in the address space of the program
     */
#if   (CMP_SIZEOF_SSIZE_T==4)
    int64_t max = std::numeric_limits<size_t>::max();
#else
    int64_t max = std::numeric_limits<int64_t>::max();
#endif
    if(dims[0] * dims[1] * dims[2] > max)
    {
      err = -1;
      QString ss = QObject::tr("The total number of elements '%1' is greater than this program can hold. Try the 64 bit version.").arg(dims[0] * dims[1] * dims[2]);
      setErrorCondition(err);
      addErrorMessage(getHumanLabel(), ss, -1);
      return;
    }

    if(dims[0] > max || dims[1] > max || dims[2] > max)
    {
      err = -1;
      QString ss = QObject::tr("One of the dimensions is greater than the max index for this sysem. Try the 64 bit version."\
                               " dim[0]=%1  dim[1]=%2  dim[2]=%3").arg(dims[0]).arg(dims[1]).arg(dims[0]);
      setErrorCondition(err);
      addErrorMessage(getHumanLabel(), ss, -1);
      return;
    }
    /* ************ End Sanity Check *************************** */
    size_t dcDims[3] =
    { dims[0], dims[1], dims[2] };
    m->setDimensions(dcDims);
    m->setResolution(res);
    m->setOrigin(0.0f, 0.0f, 0.0f);
  }

  H5EbsdVolumeReader::Pointer reader;
  QVector<QString> names;

  if (m_Manufacturer == Ebsd::TSL)
  {
    AngFields angFeatures;
    reader = H5AngVolumeReader::New();
    names = angFeatures.getFilterFeatures<QVector<QString> > ();
  }
  else if (m_Manufacturer == Ebsd::HKL)
  {
    CtfFields cfeatures;
    reader = H5CtfVolumeReader::New();
    names = cfeatures.getFilterFeatures<QVector<QString> > ();
  }
  else if (m_Manufacturer == Ebsd::HEDM)
  {
    MicFields mfeatures;
    reader = H5MicVolumeReader::New();
    names = mfeatures.getFilterFeatures<QVector<QString> > ();
  }
  else
  {
    QString ss = QObject::tr("%1:  Original Data source could not be determined. It should be TSL or HKL").arg(getHumanLabel());
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss, -1);
    return;
  }

  for (size_t i = 0; i < names.size(); ++i)
  {
    if (reader->getPointerType(names[i]) == Ebsd::Int32)
    {
      Int32ArrayType::Pointer array = Int32ArrayType::CreateArray(voxels, names[i]);
      m->addCellData(names[i], array);
    }
    else if (reader->getPointerType(names[i]) == Ebsd::Float)
    {
      FloatArrayType::Pointer array = FloatArrayType::CreateArray(voxels, names[i]);
      m->addCellData(names[i], array);
    }
  }

  QVector<int> dim(1, 3);
  m_CellEulerAnglesPtr = m->createNonPrereqArray<float, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellEulerAnglesArrayName, 0, voxels, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_CellEulerAngles */
  dim[0] = 1;
  m_CellPhasesPtr = m->createNonPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_CellPhasesArrayName, 0, voxels, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_CellPhases */

  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = m->createNonPrereqArray<unsigned int, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_CrystalStructuresArrayName, Ebsd::CrystalStructure::UnknownCrystalStructure, ensembles, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_CrystalStructures */
  dim[0] = 6;
  m_LatticeConstantsPtr = m->createNonPrereqArray<float, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_LatticeConstantsArrayName, 0.0, ensembles, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_LatticeConstants */

  StringDataArray::Pointer materialNames = StringDataArray::CreateArray(1, DREAM3D::EnsembleData::MaterialName);
  m->addCellEnsembleData( DREAM3D::EnsembleData::MaterialName, materialNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    VolumeDataContainer::Pointer vdc = VolumeDataContainer::New();
    vdc->setName(getDataContainerName());
    getDataContainerArray()->pushBack(vdc);
    m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  }

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    VolumeDataContainer::Pointer vdc = VolumeDataContainer::New();
    vdc->setName(getDataContainerName());
    getDataContainerArray()->pushBack(vdc);
    m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  }

  int err = 0;
  setErrorCondition(err);
  QString manufacturer;
  // Get the Size and Resolution of the Volume
  {
    H5EbsdVolumeInfo::Pointer volumeInfoReader = H5EbsdVolumeInfo::New();
    volumeInfoReader->setFileName(m_InputFile);
    err = volumeInfoReader->readVolumeInfo();
    setErrorCondition(err);
    int64_t dims[3];
    float res[3];
    volumeInfoReader->getDimsAndResolution(dims[0], dims[1], dims[2], res[0], res[1], res[2]);
    /* Sanity check what we are trying to load to make sure it can fit in our address space.
     * Note that this does not guarantee the user has enough left, just that the
     * size of the volume can fit in the address space of the program
     */
#if   (CMP_SIZEOF_SSIZE_T==4)
    int64_t max = std::numeric_limits<size_t>::max();
#else
    int64_t max = std::numeric_limits<int64_t>::max();
#endif
    if(dims[0] * dims[1] * dims[2] > max)
    {
      err = -1;
      QString s = QObject::tr("The total number of elements '%1' is greater than this program can hold. Try the 64 bit version.").arg(dims[0] * dims[1] * dims[2]);
      setErrorCondition(err);
      addErrorMessage(getHumanLabel(), s, -1);
      return;
    }

    if(dims[0] > max || dims[1] > max || dims[2] > max)
    {
      err = -1;
      QString ss = QObject::tr("One of the dimensions is greater than the max index for this sysem. Try the 64 bit version."\
                               " dim[0]=%1  dim[1]=%2  dim[2]=%3").arg(dims[0]).arg(dims[1]).arg(dims[0]);
      setErrorCondition(err);
      addErrorMessage(getHumanLabel(), ss, -1);
      return;
    }
    /* ************ End Sanity Check *************************** */
    size_t dcDims[3] =
    { dims[0], dims[1], dims[2] };
    m->setDimensions(dcDims);
    m->setResolution(res);
    //Now Calculate our "subvolume" of slices, ie, those start and end values that the user selected from the GUI
    dcDims[2] = m_ZEndIndex - m_ZStartIndex + 1;
    m->setDimensions(dcDims);
    manufacturer = volumeInfoReader->getManufacturer();
    m_RefFrameZDir = volumeInfoReader->getStackingOrder();
    m_SampleTransformationAngle = volumeInfoReader->getSampleTransformationAngle();
    m_SampleTransformationAxis = volumeInfoReader->getSampleTransformationAxis();
    m_EulerTransformationAngle = volumeInfoReader->getEulerTransformationAngle();
    m_EulerTransformationAxis = volumeInfoReader->getEulerTransformationAxis();
    volumeInfoReader = H5EbsdVolumeInfo::NullPointer();
  }
  H5EbsdVolumeReader::Pointer ebsdReader;
  if(manufacturer.compare(Ebsd::Ang::Manufacturer) == 0)
  {
    ebsdReader = initTSLEbsdVolumeReader();
  }
  else if(manufacturer.compare(Ebsd::Ctf::Manufacturer) == 0)
  {
    ebsdReader = initHKLEbsdVolumeReader();
  }
  else if(manufacturer.compare(Ebsd::Mic::Manufacturer) == 0)
  {
    ebsdReader = initHEDMEbsdVolumeReader();
  }
  else
  {
    setErrorCondition(-1);

    QString msg = QObject::tr("Could not determine or match a supported manufacturer from the data file. Supported manufacturer codes are: %1, %2 and %3")\
                  .arg(Ebsd::Ctf::Manufacturer).arg(Ebsd::Ang::Manufacturer).arg(Ebsd::Mic::Manufacturer);
    addErrorMessage(getHumanLabel(), msg, -1);
    return;
  }

  // Sanity Check the Error Condition or the state of the EBSD Reader Object.
  if(getErrorCondition() < 0 || NULL == ebsdReader.get())
  {
    return;
  }

  // Initialize all the arrays with some default values

  int64_t totalPoints = m->getTotalPoints();
  {
    QString ss = QObject::tr("Initializing %1 voxels").arg(totalPoints);
    notifyStatusMessage(ss);
  }
  {
    QString ss = QObject::tr("Reading Ebsd Data from file %1").arg(getInputFile());
    notifyStatusMessage(ss);
  }
  ebsdReader->setSliceStart(m_ZStartIndex);
  ebsdReader->setSliceEnd(m_ZEndIndex);
  ebsdReader->readAllArrays(false);
  ebsdReader->setArraysToRead(m_SelectedVolumeCellArrays);
  err = ebsdReader->loadData(m->getXPoints(), m->getYPoints(), m->getZPoints(), m_RefFrameZDir);
  if(err < 0)
  {
    setErrorCondition(err);
    PipelineMessage em (getHumanLabel(), "Error Loading Data from Ebsd Data file.", -1);
    addErrorMessage(em);
    return;
  }


  QVector<int> dim(1, 1);
  typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = m->getPrereqArray<unsigned int, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_CrystalStructuresArrayName, -304, m->getNumCellEnsembleTuples(), dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_CrystalStructures */
  dim[0] = 6;
  m_LatticeConstantsPtr = m->getPrereqArray<float, AbstractFilter>(this, m_CellEnsembleAttributeMatrixName,  m_LatticeConstantsArrayName, -305, m->getNumCellEnsembleTuples(), dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0); /* Assigns the actual data pointer to our instance variable m_LatticeConstants */

  // Copy the data from the pointers embedded in the reader object into our data container (Cell array).
  if(manufacturer.compare(Ebsd::Ang::Manufacturer) == 0)
  {
    copyTSLArrays(ebsdReader.get());
  }
  else if(manufacturer.compare(Ebsd::Ctf::Manufacturer) == 0)
  {
    copyHKLArrays(ebsdReader.get());
  }
  else if(manufacturer.compare(Ebsd::Mic::Manufacturer) == 0)
  {
    copyHEDMArrays(ebsdReader.get());
  }
  else
  {
    QString msg = QObject::tr("Could not determine or match a supported manufacturer from the data file. Supported manufacturer codes are: %1, %2 and %3")\
                  .arg(Ebsd::Ctf::Manufacturer).arg(Ebsd::Ang::Manufacturer).arg(Ebsd::Mic::Manufacturer);
    addErrorMessage(getHumanLabel(), msg, -10001);
    return;
  }

  if(m_UseTransformations == true)
  {

    if(m_SampleTransformationAngle > 0)
    {
      FloatVec3Widget_t sampleAxis;
      sampleAxis.x = m_SampleTransformationAxis[0];
      sampleAxis.y = m_SampleTransformationAxis[1];
      sampleAxis.z = m_SampleTransformationAxis[2];

      RotateSampleRefFrame::Pointer rot_Sample = RotateSampleRefFrame::New();
      rot_Sample->setObservers(this->getObservers());
      rot_Sample->setDataContainerArray(getDataContainerArray());
      rot_Sample->setRotationAngle(m_SampleTransformationAngle);
      rot_Sample->setRotationAxis(sampleAxis);
      rot_Sample->setsliceBySlice(true);
      rot_Sample->execute();
    }

    if(m_EulerTransformationAngle > 0)
    {
      FloatVec3Widget_t eulerAxis;
      eulerAxis.x = m_EulerTransformationAxis[0];
      eulerAxis.y = m_EulerTransformationAxis[1];
      eulerAxis.z = m_EulerTransformationAxis[2];

      RotateEulerRefFrame::Pointer rot_Euler = RotateEulerRefFrame::New();
      rot_Euler->setObservers(this->getObservers());
      rot_Euler->setDataContainerArray(getDataContainerArray());
      rot_Euler->setRotationAngle(m_EulerTransformationAngle);
      rot_Euler->setRotationAxis(eulerAxis);
      rot_Euler->execute();
    }

  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::setVolumeSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                             QSet<QString> selectedEdgeArrays,
                                             QSet<QString> selectedFaceArrays,
                                             QSet<QString> selectedCellArrays,
                                             QSet<QString> selectedFeatureArrays,
                                             QSet<QString> selectedEnsembleArrays)
{
  m_SelectedVolumeVertexArrays = selectedVertexArrays;
  m_SelectedVolumeEdgeArrays = selectedEdgeArrays;
  m_SelectedVolumeFaceArrays = selectedFaceArrays;
  m_SelectedVolumeCellArrays = selectedCellArrays;
  m_SelectedVolumeCellFeatureArrays = selectedFeatureArrays;
  m_SelectedVolumeCellEnsembleArrays = selectedEnsembleArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::setSurfaceSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                              QSet<QString> selectedEdgeArrays,
                                              QSet<QString> selectedFaceArrays,
                                              QSet<QString> selectedFeatureArrays,
                                              QSet<QString> selectedEnsembleArrays)
{
  // Empty because there is no Surface data in an H5Ebsd file
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::setEdgeSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                           QSet<QString> selectedEdgeArrays,
                                           QSet<QString> selectedFeatureArrays,
                                           QSet<QString> selectedEnsembleArrays)
{
  // Empty because there is no Edge data in an H5Ebsd file
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::setVertexSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                             QSet<QString> selectedFeatureArrays,
                                             QSet<QString> selectedEnsembleArrays)
{
  // Empty because there is no Vertex data in an H5Ebsd file
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5EbsdVolumeReader::Pointer ReadH5Ebsd::initTSLEbsdVolumeReader()
{
  int err = 0;
  H5EbsdVolumeReader::Pointer ebsdReader = H5AngVolumeReader::New();
  if(NULL == ebsdReader)
  {
    setErrorCondition(-1);
    PipelineMessage em (getHumanLabel(), "Could not Create H5AngVolumeReader object.", -1);
    addErrorMessage(em);
    return H5EbsdVolumeReader::NullPointer();
  }
  H5AngVolumeReader* angReader = dynamic_cast<H5AngVolumeReader*>(ebsdReader.get());
  err = loadInfo<H5AngVolumeReader, AngPhase>(angReader);
  if(err < 0)
  {
    setErrorCondition(-1);
    PipelineMessage em (getHumanLabel(), "Could not read information about the Ebsd Volume.", -1);
    addErrorMessage(em);
    return H5EbsdVolumeReader::NullPointer();
  }
  if (m_SelectedVolumeCellArrays.find(m_CellEulerAnglesArrayName) != m_SelectedVolumeCellArrays.end())
  {
    m_SelectedVolumeCellArrays.insert(Ebsd::Ang::Phi1);
    m_SelectedVolumeCellArrays.insert(Ebsd::Ang::Phi);
    m_SelectedVolumeCellArrays.insert(Ebsd::Ang::Phi2);
  }
  if (m_SelectedVolumeCellArrays.find(m_CellPhasesArrayName) != m_SelectedVolumeCellArrays.end())
  {
    m_SelectedVolumeCellArrays.insert(Ebsd::Ang::PhaseData);
  }
  return ebsdReader;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5EbsdVolumeReader::Pointer ReadH5Ebsd::initHKLEbsdVolumeReader()
{
  int err = 0;
  H5EbsdVolumeReader::Pointer ebsdReader = H5CtfVolumeReader::New();
  if(NULL == ebsdReader)
  {
    setErrorCondition(-1);
    PipelineMessage em (getHumanLabel(), "Could not Create H5CtfVolumeReader object.", -1);
    addErrorMessage(em);
    return H5EbsdVolumeReader::NullPointer();
  }
  H5CtfVolumeReader* ctfReader = dynamic_cast<H5CtfVolumeReader*>(ebsdReader.get());
  err = loadInfo<H5CtfVolumeReader, CtfPhase>(ctfReader);
  if(err < 0)
  {
    setErrorCondition(-1);
    PipelineMessage em (getHumanLabel(), "Could not read information about the Ebsd Volume.", -1);
    addErrorMessage(em);
    return H5EbsdVolumeReader::NullPointer();
  }
  if (m_SelectedVolumeCellArrays.find(m_CellEulerAnglesArrayName) != m_SelectedVolumeCellArrays.end())
  {
    m_SelectedVolumeCellArrays.insert(Ebsd::Ctf::Euler1);
    m_SelectedVolumeCellArrays.insert(Ebsd::Ctf::Euler2);
    m_SelectedVolumeCellArrays.insert(Ebsd::Ctf::Euler3);
  }
  if (m_SelectedVolumeCellArrays.find(m_CellPhasesArrayName) != m_SelectedVolumeCellArrays.end())
  {
    m_SelectedVolumeCellArrays.insert(Ebsd::Ctf::Phase);
  }
  return ebsdReader;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5EbsdVolumeReader::Pointer ReadH5Ebsd::initHEDMEbsdVolumeReader()
{
  int err = 0;
  H5EbsdVolumeReader::Pointer ebsdReader = H5MicVolumeReader::New();
  if(NULL == ebsdReader)
  {
    setErrorCondition(-1);
    PipelineMessage em (getHumanLabel(), "Could not Create H5MicVolumeReader object.", -1);
    addErrorMessage(em);
    return H5EbsdVolumeReader::NullPointer();
  }
  H5MicVolumeReader* micReader = dynamic_cast<H5MicVolumeReader*>(ebsdReader.get());
  err = loadInfo<H5MicVolumeReader, MicPhase>(micReader);
  if(err < 0)
  {
    setErrorCondition(-1);
    PipelineMessage em (getHumanLabel(), "Could not read information about the Ebsd Volume.", -1);
    addErrorMessage(em);
    return H5EbsdVolumeReader::NullPointer();
  }
  if (m_SelectedVolumeCellArrays.find(m_CellEulerAnglesArrayName) != m_SelectedVolumeCellArrays.end())
  {
    m_SelectedVolumeCellArrays.insert(Ebsd::Mic::Euler1);
    m_SelectedVolumeCellArrays.insert(Ebsd::Mic::Euler2);
    m_SelectedVolumeCellArrays.insert(Ebsd::Mic::Euler3);
  }
  if (m_SelectedVolumeCellArrays.find(m_CellPhasesArrayName) != m_SelectedVolumeCellArrays.end())
  {
    m_SelectedVolumeCellArrays.insert(Ebsd::Mic::Phase);
  }
  m_SelectedVolumeCellArrays.insert(Ebsd::Mic::X);
  m_SelectedVolumeCellArrays.insert(Ebsd::Mic::Y);
  return ebsdReader;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::copyTSLArrays(H5EbsdVolumeReader* ebsdReader)
{
  float* f1 = NULL;
  float* f2 = NULL;
  float* f3 = NULL;
  int* phasePtr = NULL;

  FloatArrayType::Pointer fArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getTotalPoints();
  QVector<int> dims(1, 1);
  if (m_SelectedVolumeCellArrays.find(m_CellPhasesArrayName) != m_SelectedVolumeCellArrays.end() )
  {
    phasePtr = reinterpret_cast<int*>(ebsdReader->getPointerByName(Ebsd::Ang::PhaseData));
    iArray = Int32ArrayType::CreateArray(totalPoints, dims, DREAM3D::CellData::Phases);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->addCellData(DREAM3D::CellData::Phases, iArray);
  }

  if (m_SelectedVolumeCellArrays.find(m_CellEulerAnglesArrayName) != m_SelectedVolumeCellArrays.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::Phi1));
    f2 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::Phi));
    f3 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::Phi2));
    dims[0] = 3;
    fArray = FloatArrayType::CreateArray(totalPoints, dims, DREAM3D::CellData::EulerAngles);
    float* cellEulerAngles = fArray->getPointer(0);

    for (int64_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i];
      cellEulerAngles[3 * i + 1] = f2[i];
      cellEulerAngles[3 * i + 2] = f3[i];
    }
    m->addCellData(DREAM3D::CellData::EulerAngles, fArray);
  }

  if (m_SelectedVolumeCellArrays.find(Ebsd::Ang::ImageQuality) != m_SelectedVolumeCellArrays.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::ImageQuality));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Ang::ImageQuality);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    m->addCellData(Ebsd::Ang::ImageQuality, fArray);
  }

  if (m_SelectedVolumeCellArrays.find(Ebsd::Ang::ConfidenceIndex) != m_SelectedVolumeCellArrays.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::ConfidenceIndex));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Ang::ConfidenceIndex);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    m->addCellData(Ebsd::Ang::ConfidenceIndex, fArray);
  }

  if (m_SelectedVolumeCellArrays.find(Ebsd::Ang::SEMSignal) != m_SelectedVolumeCellArrays.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::SEMSignal));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Ang::SEMSignal);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    m->addCellData(Ebsd::Ang::SEMSignal, fArray);
  }

  if (m_SelectedVolumeCellArrays.find(Ebsd::Ang::Fit) != m_SelectedVolumeCellArrays.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::Fit));
    fArray = FloatArrayType::CreateArray(totalPoints, Ebsd::Ang::Fit);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    m->addCellData(Ebsd::Ang::Fit, fArray);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::copyHKLArrays(H5EbsdVolumeReader* ebsdReader)
{
  float* f1 = NULL;
  float* f2 = NULL;
  float* f3 = NULL;
  int* phasePtr = NULL;

  FloatArrayType::Pointer fArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getTotalPoints();
  QVector<int> dims(1,1);
  phasePtr = reinterpret_cast<int*>(ebsdReader->getPointerByName(Ebsd::Ctf::Phase));
  iArray = Int32ArrayType::CreateArray(totalPoints, dims, DREAM3D::CellData::Phases);
  ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
  m->addCellData(DREAM3D::CellData::Phases, iArray);

  if (m_SelectedVolumeCellArrays.find(m_CellEulerAnglesArrayName) != m_SelectedVolumeCellArrays.end() )
  {
    //  radianconversion = M_PI / 180.0;
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ctf::Euler1));
    f2 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ctf::Euler2));
    f3 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ctf::Euler3));
    dims[0] = 3;
    fArray = FloatArrayType::CreateArray(totalPoints, dims, DREAM3D::CellData::EulerAngles);
    float* cellEulerAngles = fArray->getPointer(0);
    int* cellPhases = iArray->getPointer(0);

    for (int64_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i];
      cellEulerAngles[3 * i + 1] = f2[i];
      cellEulerAngles[3 * i + 2] = f3[i];
      if(m_CrystalStructures[cellPhases[i]] == Ebsd::CrystalStructure::Hexagonal_High)
      {cellEulerAngles[3 * i + 2] = cellEulerAngles[3 * i + 2] + (30.0);}
    }
    m->addCellData(DREAM3D::CellData::EulerAngles, fArray);
  }

  dims[0] = 1;
  if (m_SelectedVolumeCellArrays.find(Ebsd::Ctf::Bands) != m_SelectedVolumeCellArrays.end() )
  {
    phasePtr = reinterpret_cast<int*>(ebsdReader->getPointerByName(Ebsd::Ctf::Bands));
    iArray = Int32ArrayType::CreateArray(totalPoints, dims, Ebsd::Ctf::Bands);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->addCellData(Ebsd::Ctf::Bands, iArray);
  }

  if (m_SelectedVolumeCellArrays.find(Ebsd::Ctf::Error) != m_SelectedVolumeCellArrays.end() )
  {
    phasePtr = reinterpret_cast<int*>(ebsdReader->getPointerByName(Ebsd::Ctf::Error));
    iArray = Int32ArrayType::CreateArray(totalPoints, dims, Ebsd::Ctf::Error);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->addCellData(Ebsd::Ctf::Error, iArray);
  }

  if (m_SelectedVolumeCellArrays.find(Ebsd::Ctf::MAD) != m_SelectedVolumeCellArrays.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ctf::MAD));
    fArray = FloatArrayType::CreateArray(totalPoints, dims, Ebsd::Ctf::MAD);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    m->addCellData(Ebsd::Ctf::MAD, fArray);
  }

  if (m_SelectedVolumeCellArrays.find(Ebsd::Ctf::BC) != m_SelectedVolumeCellArrays.end() )
  {
    phasePtr = reinterpret_cast<int*>(ebsdReader->getPointerByName(Ebsd::Ctf::BC));
    iArray = Int32ArrayType::CreateArray(totalPoints, dims, Ebsd::Ctf::BC);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->addCellData(Ebsd::Ctf::BC, iArray);
  }

  if (m_SelectedVolumeCellArrays.find(Ebsd::Ctf::BS) != m_SelectedVolumeCellArrays.end() )
  {
    phasePtr = reinterpret_cast<int*>(ebsdReader->getPointerByName(Ebsd::Ctf::BS));
    iArray = Int32ArrayType::CreateArray(totalPoints, dims, Ebsd::Ctf::BS);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->addCellData(Ebsd::Ctf::BS, iArray);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::copyHEDMArrays(H5EbsdVolumeReader* ebsdReader)
{
  float* f1 = NULL;
  float* f2 = NULL;
  float* f3 = NULL;
  int* phasePtr = NULL;

  FloatArrayType::Pointer fArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getTotalPoints();

  float x, y;
  float xMin = 10000000;
  float yMin = 10000000;
  f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Mic::X));
  f2 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Mic::Y));
  for (int64_t i = 0; i < totalPoints; i++)
  {
    x = f1[i];
    y = f2[i];
    if(x < xMin) { xMin = x; }
    if(y < yMin) { yMin = y; }
  }
  m->setOrigin(xMin, yMin, 0.0);
  QVector<int> dims(1, 3);
  if (m_SelectedVolumeCellArrays.find(m_CellEulerAnglesArrayName) != m_SelectedVolumeCellArrays.end() )
  {
    //  radianconversion = M_PI / 180.0;
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Mic::Euler1));
    f2 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Mic::Euler2));
    f3 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Mic::Euler3));
    fArray = FloatArrayType::CreateArray(totalPoints, dims, DREAM3D::CellData::EulerAngles);
    float* cellEulerAngles = fArray->getPointer(0);
    for (int64_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i];
      cellEulerAngles[3 * i + 1] = f2[i];
      cellEulerAngles[3 * i + 2] = f3[i];
    }
    m->addCellData(DREAM3D::CellData::EulerAngles, fArray);
  }
  dims[0] = 1;
  if (m_SelectedVolumeCellArrays.find(m_CellPhasesArrayName) != m_SelectedVolumeCellArrays.end() )
  {
    phasePtr = reinterpret_cast<int*>(ebsdReader->getPointerByName(Ebsd::Mic::Phase));
    iArray = Int32ArrayType::CreateArray(totalPoints, dims, DREAM3D::CellData::Phases);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    m->addCellData(DREAM3D::CellData::Phases, iArray);
  }

  if (m_SelectedVolumeCellArrays.find(Ebsd::Mic::Confidence) != m_SelectedVolumeCellArrays.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Mic::Confidence));
    fArray = FloatArrayType::CreateArray(totalPoints, dims, Ebsd::Mic::Confidence);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    m->addCellData(Ebsd::Mic::Confidence, fArray);
  }
}
