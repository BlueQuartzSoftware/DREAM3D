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

#include "ReadH5Ebsd.h"

#include <QtCore/QFileInfo>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "EbsdLib/H5EbsdVolumeInfo.h"
#include "EbsdLib/HKL/CtfFields.h"
#include "EbsdLib/HKL/H5CtfVolumeReader.h"
#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/TSL/H5AngVolumeReader.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/FilterParameters/ReadH5EbsdFilterParameter.h"

// Include the MOC generated file for this class
#include "moc_ReadH5Ebsd.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadH5Ebsd::ReadH5Ebsd() :
  AbstractFilter(),
  m_DataContainerName(SIMPL::Defaults::ImageDataContainerName),
  m_CellEnsembleAttributeMatrixName(SIMPL::Defaults::CellEnsembleAttributeMatrixName),
  m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName),
  m_PhaseNameArrayName(""),
  m_MaterialNameArrayName(SIMPL::EnsembleData::MaterialName),
  m_InputFile(""),
  m_ZStartIndex(0),
  m_ZEndIndex(0),
  m_UseTransformations(true),
  m_AngleRepresentation(Ebsd::AngleRepresentation::Radians),
  m_RefFrameZDir(SIMPL::RefFrameZDir::UnknownRefFrameZDirection),
  m_Manufacturer(Ebsd::UnknownManufacturer),
  m_CrystalStructuresArrayName(SIMPL::EnsembleData::CrystalStructures),
  m_LatticeConstantsArrayName(SIMPL::EnsembleData::LatticeConstants),
  m_CellPhasesArrayName(SIMPL::CellData::Phases),
  m_CellEulerAnglesArrayName(SIMPL::CellData::EulerAngles),
  m_CellPhases(NULL),
  m_CellEulerAngles(NULL),
  m_CrystalStructures(NULL),
  m_LatticeConstants(NULL)
{
  m_SampleTransformation.angle = 0.0f;
  m_SampleTransformation.h = 0.0f;
  m_SampleTransformation.k = 0.0f;
  m_SampleTransformation.l = 1.0f;

  m_EulerTransformation.angle = 0.0f;
  m_EulerTransformation.h = 0.0f;
  m_EulerTransformation.k = 0.0f;
  m_EulerTransformation.l = 1.0f;

  m_MaterialNamesPtr = StringDataArray::NullPointer();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadH5Ebsd::~ReadH5Ebsd()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(ReadH5EbsdFilterParameter::New("Read H5Ebsd File", "ReadH5Ebsd", "__NULL__", FilterParameter::Parameter));
  parameters.push_back(StringFilterParameter::New("Data Container", "DataContainerName", getDataContainerName(), FilterParameter::CreatedArray));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixName", getCellAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Ensemble Attribute Matrix", "CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setCellEnsembleAttributeMatrixName(reader->readString("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName() ) );
  setInputFile(reader->readString("InputFile", getInputFile() ) );
  setRefFrameZDir( ( reader->readValue("RefFrameZDir", getRefFrameZDir() ) ) );
  setZStartIndex( reader->readValue("ZStartIndex", getZStartIndex() ) );
  setZEndIndex( reader->readValue("ZEndIndex", getZEndIndex() ) );
  setUseTransformations( reader->readValue("UseTransformations", getUseTransformations() ) );
  setSelectedArrayNames(reader->readArraySelections("SelectedArrayNames", getSelectedArrayNames() ));
  setAngleRepresentation(reader->readValue("AngleRepresentation", getAngleRepresentation() ));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ReadH5Ebsd::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(DataContainerName)
  SIMPL_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(CellEnsembleAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(InputFile)
  SIMPL_FILTER_WRITE_PARAMETER(RefFrameZDir)
  SIMPL_FILTER_WRITE_PARAMETER(ZStartIndex)
  SIMPL_FILTER_WRITE_PARAMETER(ZEndIndex)
  SIMPL_FILTER_WRITE_PARAMETER(UseTransformations)
  SIMPL_FILTER_WRITE_PARAMETER(AngleRepresentation)
  writer->writeArraySelections("SelectedArrayNames", getSelectedArrayNames() );

  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t ReadH5Ebsd::initDataContainerDimsRes(int64_t dims[3], DataContainer::Pointer m)
{
  int32_t err = 0;
  /* Sanity check what we are trying to load to make sure it can fit in our address space.
   * Note that this does not guarantee the user has enough left, just that the
   * size of the volume can fit in the address space of the program
   */
#if   (CMP_SIZEOF_SSIZE_T==4)
  int64_t max = std::numeric_limits<size_t>::max();
#else
  int64_t max = std::numeric_limits<int64_t>::max();
#endif
  if (dims[0] * dims[1] * dims[2] > max)
  {
    err = -1;
    QString ss = QObject::tr("The total number of elements '%1' is greater than this program can hold. Try the 64 bit version").arg((dims[0] * dims[1] * dims[2]));
    setErrorCondition(err);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return err;
  }

  if (dims[0] > max || dims[1] > max || dims[2] > max)
  {
    err = -1;
    QString ss = QObject::tr("One of the dimensions is greater than the max index for this sysem. Try the 64 bit version"
                             " dim[0]=%1  dim[1]=%2  dim[2]=%3").arg(dims[0]).arg(dims[1]).arg(dims[2]);
    setErrorCondition(err);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return err;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::readVolumeInfo()
{
  m_DataArrayNames.clear(); // Remove all the data arrays

  QFileInfo fi(m_InputFile);
  if (m_InputFile.isEmpty() == true )
  {
    QString ss = QObject::tr("%1: The H5Ebsd file must exist and the Manufacturer must be set correctly in the file").arg(getHumanLabel());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
    return;
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist. '%1'").arg(getInputFile());
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  else if (m_InputFile.isEmpty() == false)
  {
    H5EbsdVolumeInfo::Pointer reader = H5EbsdVolumeInfo::New();
    reader->setFileName(m_InputFile);
    int32_t err = reader->readVolumeInfo();
    if (err < 0)
    {
      QString ss = QObject::tr("Error reading VolumeInfo from H5Ebsd File");
      setErrorCondition(-1);
      notifyErrorMessage(getHumanLabel(), ss, -1);
      return;
    }

    // Get the list of data arrays in the EBSD file
    m_DataArrayNames = reader->getDataArrayNames();

    QString manufacturer = reader->getManufacturer();
    if(manufacturer.compare(Ebsd::Ang::Manufacturer) == 0)
    {
      m_Manufacturer = Ebsd::TSL;
    }
    else if(manufacturer.compare(Ebsd::Ctf::Manufacturer) == 0)
    {
      m_Manufacturer = Ebsd::HKL;
    }
    else
    {
      QString ss = QObject::tr("Original Data source could not be determined. It should be TSL, HKL or HEDM");
      setErrorCondition(-1);
      notifyErrorMessage(getHumanLabel(), ss, -1);
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
      QString ss = QObject::tr("The total number of elements '%1' is greater than this program can hold. Try the 64 bit version").arg(dims[0] * dims[1] * dims[2]);
      setErrorCondition(err);
      notifyErrorMessage(getHumanLabel(), ss, -1);
      return;
    }

    if(dims[0] > max || dims[1] > max || dims[2] > max)
    {
      err = -1;
      QString ss = QObject::tr("One of the dimensions is greater than the max index for this sysem. Try the 64 bit version"\
                               " dim[0]=%1  dim[1]=%2  dim[2]=%3").arg(dims[0]).arg(dims[1]).arg(dims[0]);
      setErrorCondition(err);
      notifyErrorMessage(getHumanLabel(), ss, -1);
      return;
    }
    /* ************ End Sanity Check *************************** */
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::initialize()
{
  m_MaterialNamesPtr = StringDataArray::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::dataCheck()
{
  setErrorCondition(0);
  initialize();

  DataArrayPath tempPath;

  m_DataArrayNames.clear(); // Remove all the data arrays

  H5EbsdVolumeInfo::Pointer volumeInfoReader = H5EbsdVolumeInfo::New();
  QFileInfo fi(getInputFile());
  if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file was not specified or was empty");
    setErrorCondition(-10);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(NULL, getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }
  tDims.resize(1);
  tDims[0] = 0;
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::CellEnsemble);
  if(getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

  volumeInfoReader->setFileName(m_InputFile);
  H5EbsdVolumeReader::Pointer reader;
  QVector<QString> names;

  int64_t dims[3] = { 0, 0, 0 };
  float res[3] = { 0.0f, 0.0f, 0.0f };
  int32_t err = volumeInfoReader->getDimsAndResolution(dims[0], dims[1], dims[2], res[0], res[1], res[2]);

  if (err < 0)
  {
    QString ss = QObject::tr("File counld not be read properly");
    setErrorCondition(-11);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  QString manufacturer = volumeInfoReader->getManufacturer();
  if (manufacturer.compare(Ebsd::Ang::Manufacturer) == 0)
  {
    m_Manufacturer = Ebsd::TSL;
  }
  else if (manufacturer.compare(Ebsd::Ctf::Manufacturer) == 0)
  {
    m_Manufacturer = Ebsd::HKL;
  }

  if(m_ZEndIndex < m_ZStartIndex)
  {
    QString ss = QObject::tr("The End Slice [%1] MUST be larger than the Start Slice [%2]. This condition was not met.").arg(m_ZEndIndex).arg(m_ZStartIndex);
    setErrorCondition(-12);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  size_t dcDims[3] = { static_cast<size_t>(dims[0]), static_cast<size_t>(dims[1]), static_cast<size_t>(dims[2]) };
  //Now Calculate our "subvolume" of slices, ie, those start and end values that the user selected from the GUI
  dcDims[2] = m_ZEndIndex - m_ZStartIndex + 1;
  m->getGeometryAs<ImageGeom>()->setDimensions(dcDims);
  m->getGeometryAs<ImageGeom>()->setResolution(res);

  //Update the size fo the Cell Attribute Matrix now that the dimensions of the volume are known
  tDims.resize(3);
  tDims[0] = dcDims[0];
  tDims[1] = dcDims[1];
  tDims[2] = dcDims[2];
  cellAttrMat->resizeAttributeArrays(tDims);

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
  else
  {
    QString ss = QObject::tr("Original Data source could not be determined. It should be TSL or HKL");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
    return;
  }


  // Get the list of data arrays in the EBSD file
  m_DataArrayNames = reader->getDataArrayNames();

  QVector<size_t> cDims(1, 1);
  for (int32_t i = 0; i < names.size(); ++i)
  {
    // First check to see if the name is in our list of names to read.
    if (m_SelectedArrayNames.find(names[i]) == m_SelectedArrayNames.end() ) { continue; }
    if (reader->getPointerType(names[i]) == Ebsd::Int32)
    {
      cellAttrMat->createAndAddAttributeArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, names[i], 0, cDims);
    }
    else if (reader->getPointerType(names[i]) == Ebsd::Float)
    {
      cellAttrMat->createAndAddAttributeArray<DataArray<float>, AbstractFilter, float>(this, names[i], 0, cDims);
    }
  }

  // Only read these arrays if the user wants them
  if (m_SelectedArrayNames.contains(SIMPL::CellData::EulerAngles) )
  {
    cDims[0] = 3;
    tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getCellEulerAnglesArrayName() );
    m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  // Only read the phases if the user wants it.
  if (m_SelectedArrayNames.contains(SIMPL::CellData::Phases) )
  {
    cDims[0] = 1;
    tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getCellPhasesArrayName() );
    m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  // Now create the Ensemble arrays for the XTal Structures, Material Names and LatticeConstants
  cDims[0] = 1;
  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getCrystalStructuresArrayName() );
  m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this,  tempPath, Ebsd::CrystalStructure::UnknownCrystalStructure, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  StringDataArray::Pointer materialNamesPtr = StringDataArray::CreateArray( cellEnsembleAttrMat->getNumTuples(), getMaterialNameArrayName());
  cellEnsembleAttrMat->addAttributeArray(materialNamesPtr->getName(), materialNamesPtr);
  m_MaterialNamesPtr = materialNamesPtr;

  cDims[0] = 6;
  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getLatticeConstantsArrayName() );
  m_LatticeConstantsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0.0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_LatticeConstantsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::preflight()
{
  setInPreflight(true);
  // Read the file to get the data arrays, size, meta data
  readVolumeInfo(); // This is specific to "readers" in general (I think), or at least those readers that need to show
  // a structure to the user to allow them to select only specific parts of the file to read
  // Now signal that any GUI widget is ready to read the information from this instance
  emit preflightAboutToExecute();
  // Let the GUI Widget (or anything else) update the parameters for this filter
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::execute()
{
  dataCheck();
  if(getErrorCondition() < 0) { return; }
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  int32_t err = 0;
  setErrorCondition(err);
  QString manufacturer;
  // Get the Size and Resolution of the Volume
  {
    H5EbsdVolumeInfo::Pointer volumeInfoReader = H5EbsdVolumeInfo::New();
    volumeInfoReader->setFileName(m_InputFile);
    err = volumeInfoReader->readVolumeInfo();
    setErrorCondition(err);
    int64_t dims[3] = { 0, 0, 0 };
    float res[3] = { 0.0f, 0.0f, 0.0f };
    volumeInfoReader->getDimsAndResolution(dims[0], dims[1], dims[2], res[0], res[1], res[2]);

    size_t dcDims[3] = { static_cast<size_t>(dims[0]), static_cast<size_t>(dims[1]), static_cast<size_t>(dims[2]) };
    m->getGeometryAs<ImageGeom>()->setDimensions(dcDims);
    m->getGeometryAs<ImageGeom>()->setResolution(res);
    // Now Calculate our "subvolume" of slices, ie, those start and end values that the user selected from the GUI
    dcDims[2] = m_ZEndIndex - m_ZStartIndex + 1;
    m->getGeometryAs<ImageGeom>()->setDimensions(dcDims);
    manufacturer = volumeInfoReader->getManufacturer();
    m_RefFrameZDir = volumeInfoReader->getStackingOrder();

    QVector<float> sampleTransAxis = volumeInfoReader->getSampleTransformationAxis();
    m_SampleTransformation.h = sampleTransAxis[0];
    m_SampleTransformation.k = sampleTransAxis[1];
    m_SampleTransformation.l = sampleTransAxis[2];
    m_SampleTransformation.angle = volumeInfoReader->getSampleTransformationAngle();

    QVector<float> eulerTransAxis = volumeInfoReader->getEulerTransformationAxis();
    m_EulerTransformation.h = eulerTransAxis[0];
    m_EulerTransformation.k = eulerTransAxis[1];
    m_EulerTransformation.l = eulerTransAxis[2];
    m_EulerTransformation.angle = volumeInfoReader->getEulerTransformationAngle();
    volumeInfoReader = H5EbsdVolumeInfo::NullPointer();
  }
  H5EbsdVolumeReader::Pointer ebsdReader;
  if (manufacturer.compare(Ebsd::Ang::Manufacturer) == 0)
  {
    ebsdReader = initTSLEbsdVolumeReader();
  }
  else if (manufacturer.compare(Ebsd::Ctf::Manufacturer) == 0)
  {
    ebsdReader = initHKLEbsdVolumeReader();
  }
  else
  {
    setErrorCondition(-1);

    QString ss = QObject::tr("Could not determine or match a supported manufacturer from the data file. Supported manufacturer codes are: %1, %2 and %3")\
                 .arg(Ebsd::Ctf::Manufacturer).arg(Ebsd::Ang::Manufacturer);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // Sanity Check the Error Condition or the state of the EBSD Reader Object.
  if (getErrorCondition() < 0 || NULL == ebsdReader.get())
  {
    return;
  }

  // Initialize all the arrays with some default values
  {
    QString ss = QObject::tr("Reading Ebsd Data from file %1").arg(getInputFile());
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  }
  ebsdReader->setSliceStart(m_ZStartIndex);
  ebsdReader->setSliceEnd(m_ZEndIndex);
  ebsdReader->readAllArrays(false);
  ebsdReader->setArraysToRead(m_SelectedArrayNames);
  err = ebsdReader->loadData(m->getGeometryAs<ImageGeom>()->getXPoints(), m->getGeometryAs<ImageGeom>()->getYPoints(), m->getGeometryAs<ImageGeom>()->getZPoints(), m_RefFrameZDir);
  if (err < 0)
  {
    setErrorCondition(err);
    notifyErrorMessage(ebsdReader->getNameOfClass(), ebsdReader->getErrorMessage(), getErrorCondition());
    notifyErrorMessage(getHumanLabel(), "Error Loading Data from Ebsd Data file.", -1);
    return;
  }

  // Copy the data from the pointers embedded in the reader object into our data container (Cell array).
  if (manufacturer.compare(Ebsd::Ang::Manufacturer) == 0)
  {
    copyTSLArrays(ebsdReader.get());
  }
  else if (manufacturer.compare(Ebsd::Ctf::Manufacturer) == 0)
  {
    copyHKLArrays(ebsdReader.get());
  }

  else
  {
    QString ss = QObject::tr("Could not determine or match a supported manufacturer from the data file. Supported manufacturer codes are: %1 and %2")\
                 .arg(Ebsd::Ctf::Manufacturer).arg(Ebsd::Ang::Manufacturer);
    setErrorCondition(-109875);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_UseTransformations == true)
  {

    if (m_SampleTransformation.angle > 0)
    {
      FloatVec3_t sampleAxis;
      sampleAxis.x = m_SampleTransformation.h;
      sampleAxis.y = m_SampleTransformation.k;
      sampleAxis.z = m_SampleTransformation.l;
      QString filtName = "RotateSampleRefFrame";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer rotSampleFactory = fm->getFactoryForFilter(filtName);
      if (NULL != rotSampleFactory.get() )
      {
        // If we get this far, the Factory is good so creating the filter should not fail unless something has
        // horribly gone wrong in which case the system is going to come down quickly after this.
        AbstractFilter::Pointer rot_Sample = rotSampleFactory->create();

        // Connect up the Error/Warning/Progress object so the filter can report those things
        connect(rot_Sample.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
                this, SLOT(broadcastPipelineMessage(const PipelineMessage&)));
        rot_Sample->setDataContainerArray(getDataContainerArray()); // AbstractFilter implements this so no problem
        // Now set the filter parameters for the filter using QProperty System since we can not directly
        // instantiate the filter since it resides in a plugin. These calls are SLOW. DO NOT EVER do this in a
        // tight loop. Your filter will slow down by 10X.
        bool propWasSet = rot_Sample->setProperty("RotationAngle", m_SampleTransformation.angle);
        if (false == propWasSet)
        {
          QString ss = QObject::tr("Error Setting Property '%1' into filter '%2' which is a subfilter called by ReadH5Ebsd. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers").arg("RotationAngle").arg(filtName);
          setErrorCondition(-109874);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        }
        QVariant v;
        v.setValue(sampleAxis);
        propWasSet = rot_Sample->setProperty("RotationAxis", v);
        if (false == propWasSet)
        {
          QString ss = QObject::tr("Error Setting Property '%1' into filter '%2' which is a subfilter called by ReadH5Ebsd. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers").arg("RotationAxis").arg(filtName);
          setErrorCondition(-109873);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        }
        v.setValue(true);
        propWasSet = rot_Sample->setProperty("SliceBySlice", v);
        if (false == propWasSet)
        {
          QString ss = QObject::tr("Error Setting Property '%1' into filter '%2' which is a subfilter called by ReadH5Ebsd. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers").arg("SliceBySlice").arg(filtName);
          setErrorCondition(-109872);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        }
        DataArrayPath tempPath;
        tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), "");
        v.setValue(tempPath);
        propWasSet = rot_Sample->setProperty("CellAttributeMatrixPath", v);
        if (false == propWasSet)
        {
          QString ss = QObject::tr("Error Setting Property '%1' into filter '%2' which is a subfilter called by ReadH5Ebsd. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers").arg("CellAttributeMatrixName").arg(filtName);
          setErrorCondition(-109871);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        }
        rot_Sample->execute();
      }
      else
      {
        QString ss = QObject::tr("Error creating filter '%1' which is a subfilter called by ReadH5Ebsd. Filter was not created/executed. Please notify the developers").arg(filtName);
        setErrorCondition(-109870);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      }
    }

    if (m_EulerTransformation.angle > 0)
    {
      FloatVec3_t eulerAxis;
      eulerAxis.x = m_EulerTransformation.h;
      eulerAxis.y = m_EulerTransformation.k;
      eulerAxis.z = m_EulerTransformation.l;

      QString filtName = "RotateEulerRefFrame";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer rotEulerFactory = fm->getFactoryForFilter(filtName);
      if (NULL != rotEulerFactory.get() )
      {
        // If we get this far, the Factory is good so creating the filter should not fail unless something has
        // horribly gone wrong in which case the system is going to come down quickly after this.
        AbstractFilter::Pointer rot_Euler = rotEulerFactory->create();

        // Connect up the Error/Warning/Progress object so the filter can report those things
        connect(rot_Euler.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
                this, SLOT(broadcastPipelineMessage(const PipelineMessage&)));
        rot_Euler->setDataContainerArray(getDataContainerArray()); // AbstractFilter implements this so no problem
        // Now set the filter parameters for the filter using QProperty System since we can not directly
        // instantiate the filter since it resides in a plugin. These calls are SLOW. DO NOT EVER do this in a
        // tight loop. Your filter will slow down by 10X.
        bool propWasSet = rot_Euler->setProperty("RotationAngle", m_EulerTransformation.angle);
        if (false == propWasSet)
        {
          QString ss = QObject::tr("ReadH5Ebsd Error Setting Property '%1' into filter '%2' which is a subfilter called by ReadH5Ebsd. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers").arg("RotationAngle").arg(filtName);
          setErrorCondition(-109874);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        }
        QVariant v;
        v.setValue(eulerAxis);
        propWasSet = rot_Euler->setProperty("RotationAxis", v);
        if (false == propWasSet)
        {
          QString ss = QObject::tr("ReadH5Ebsd Error Setting Property '%1' into filter '%2' which is a subfilter called by ReadH5Ebsd. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers").arg("RotationAxis").arg(filtName);
          setErrorCondition(-109873);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        }
        DataArrayPath tempPath;
        tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getCellEulerAnglesArrayName() );
        v.setValue(tempPath);
        propWasSet = rot_Euler->setProperty("CellEulerAnglesArrayPath", v);
        if (false == propWasSet)
        {
          QString ss = QObject::tr("ReadH5Ebsd Error Setting Property '%1' into filter '%2' which is a subfilter called by ReadH5Ebsd. The property was not set which could mean the property was not exposed with a Q_PROPERTY macro. Please notify the developers").arg("CellEulerAnglesArrayPath").arg(filtName);
          setErrorCondition(-109872);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        }
        rot_Euler->execute();
      }
      else
      {
        QString ss = QObject::tr("ReadH5Ebsd Error creating filter '%1' which is a subfilter called by ReadH5Ebsd. Filter was not created/executed. Please notify the developers").arg(filtName);
        setErrorCondition(-109870);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      }
    }

  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5EbsdVolumeReader::Pointer ReadH5Ebsd::initTSLEbsdVolumeReader()
{
  int32_t err = 0;
  H5EbsdVolumeReader::Pointer ebsdReader = H5AngVolumeReader::New();
  if (NULL == ebsdReader)
  {
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), "Could not Create H5AngVolumeReader object.", -1);
    return H5EbsdVolumeReader::NullPointer();
  }
  H5AngVolumeReader* angReader = dynamic_cast<H5AngVolumeReader*>(ebsdReader.get());
  err = loadInfo<H5AngVolumeReader, AngPhase>(angReader);
  if (err < 0)
  {
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), "Could not read information about the Ebsd Volume.", -1);
    return H5EbsdVolumeReader::NullPointer();
  }
  if (m_SelectedArrayNames.find(m_CellEulerAnglesArrayName) != m_SelectedArrayNames.end())
  {
    m_SelectedArrayNames.insert(Ebsd::Ang::Phi1);
    m_SelectedArrayNames.insert(Ebsd::Ang::Phi);
    m_SelectedArrayNames.insert(Ebsd::Ang::Phi2);
  }
  if (m_SelectedArrayNames.find(m_CellPhasesArrayName) != m_SelectedArrayNames.end())
  {
    m_SelectedArrayNames.insert(Ebsd::Ang::PhaseData);
  }
  return ebsdReader;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5EbsdVolumeReader::Pointer ReadH5Ebsd::initHKLEbsdVolumeReader()
{
  int32_t err = 0;
  H5EbsdVolumeReader::Pointer ebsdReader = H5CtfVolumeReader::New();
  if (NULL == ebsdReader)
  {
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), "Could not Create H5CtfVolumeReader object.", -1);
    return H5EbsdVolumeReader::NullPointer();
  }
  H5CtfVolumeReader* ctfReader = dynamic_cast<H5CtfVolumeReader*>(ebsdReader.get());
  err = loadInfo<H5CtfVolumeReader, CtfPhase>(ctfReader);
  if (err < 0)
  {
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), "Could not read information about the Ebsd Volume.", -1);
    return H5EbsdVolumeReader::NullPointer();
  }
  if (m_SelectedArrayNames.find(m_CellEulerAnglesArrayName) != m_SelectedArrayNames.end())
  {
    m_SelectedArrayNames.insert(Ebsd::Ctf::Euler1);
    m_SelectedArrayNames.insert(Ebsd::Ctf::Euler2);
    m_SelectedArrayNames.insert(Ebsd::Ctf::Euler3);
  }
  if (m_SelectedArrayNames.find(m_CellPhasesArrayName) != m_SelectedArrayNames.end())
  {
    m_SelectedArrayNames.insert(Ebsd::Ctf::Phase);
  }
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
  int32_t* phasePtr = NULL;

  FloatArrayType::Pointer fArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  AttributeMatrix::Pointer cellAttrMatrix = m->getAttributeMatrix(getCellAttributeMatrixName());
  QVector<size_t> tDims(3, 0);
  tDims[0] = m->getGeometryAs<ImageGeom>()->getXPoints();
  tDims[1] = m->getGeometryAs<ImageGeom>()->getYPoints();
  tDims[2] = m->getGeometryAs<ImageGeom>()->getZPoints();
  cellAttrMatrix->resizeAttributeArrays(tDims); // Resize the attribute Matrix to the proper dimensions

  size_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();
  QVector<size_t> cDims(1, 1);
  if (m_SelectedArrayNames.find(m_CellPhasesArrayName) != m_SelectedArrayNames.end() )
  {
    phasePtr = reinterpret_cast<int32_t*>(ebsdReader->getPointerByName(Ebsd::Ang::PhaseData));
    iArray = Int32ArrayType::CreateArray(tDims, cDims, SIMPL::CellData::Phases);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    cellAttrMatrix->addAttributeArray(SIMPL::CellData::Phases, iArray);
  }

  if (m_SelectedArrayNames.find(m_CellEulerAnglesArrayName) != m_SelectedArrayNames.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::Phi1));
    f2 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::Phi));
    f3 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::Phi2));
    cDims[0] = 3;
    fArray = FloatArrayType::CreateArray(tDims, cDims, SIMPL::CellData::EulerAngles);
    float* cellEulerAngles = fArray->getPointer(0);
    float degToRad = 1.0f;
    if(m_AngleRepresentation != Ebsd::AngleRepresentation::Radians && m_UseTransformations == true)
    {
      degToRad = SIMPLib::Constants::k_PiOver180;
    }
    for (size_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i] * degToRad;
      cellEulerAngles[3 * i + 1] = f2[i] * degToRad;
      cellEulerAngles[3 * i + 2] = f3[i] * degToRad;
    }
    cellAttrMatrix->addAttributeArray(SIMPL::CellData::EulerAngles, fArray);
  }

  // Reset this back to 1 for the rest of the arrays
  cDims[0] = 1;

  if (m_SelectedArrayNames.find(Ebsd::Ang::ImageQuality) != m_SelectedArrayNames.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::ImageQuality));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::ImageQuality);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->addAttributeArray(Ebsd::Ang::ImageQuality, fArray);
  }

  if (m_SelectedArrayNames.find(Ebsd::Ang::ConfidenceIndex) != m_SelectedArrayNames.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::ConfidenceIndex));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::ConfidenceIndex);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->addAttributeArray(Ebsd::Ang::ConfidenceIndex, fArray);
  }

  if (m_SelectedArrayNames.find(Ebsd::Ang::SEMSignal) != m_SelectedArrayNames.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::SEMSignal));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::SEMSignal);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->addAttributeArray(Ebsd::Ang::SEMSignal, fArray);
  }

  if (m_SelectedArrayNames.find(Ebsd::Ang::Fit) != m_SelectedArrayNames.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::Fit));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::Fit);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->addAttributeArray(Ebsd::Ang::Fit, fArray);
  }

  if (m_SelectedArrayNames.find(Ebsd::Ang::XPosition) != m_SelectedArrayNames.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::XPosition));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::XPosition);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->addAttributeArray(Ebsd::Ang::XPosition, fArray);
  }

  if (m_SelectedArrayNames.find(Ebsd::Ang::YPosition) != m_SelectedArrayNames.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ang::YPosition));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ang::YPosition);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->addAttributeArray(Ebsd::Ang::YPosition, fArray);
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
  int32_t* phasePtr = NULL;

  FloatArrayType::Pointer fArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer cellAttrMatrix = m->getAttributeMatrix(getCellAttributeMatrixName());
  QVector<size_t> tDims(3, 0);
  tDims[0] = m->getGeometryAs<ImageGeom>()->getXPoints();
  tDims[1] = m->getGeometryAs<ImageGeom>()->getYPoints();
  tDims[2] = m->getGeometryAs<ImageGeom>()->getZPoints();
  cellAttrMatrix->resizeAttributeArrays(tDims); // Resize the attribute Matrix to the proper dimensions

  size_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();
  QVector<size_t> cDims(1, 1);
  phasePtr = reinterpret_cast<int32_t*>(ebsdReader->getPointerByName(Ebsd::Ctf::Phase));
  iArray = Int32ArrayType::CreateArray(tDims, cDims, SIMPL::CellData::Phases);
  ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
  cellAttrMatrix->addAttributeArray(SIMPL::CellData::Phases, iArray);

  if (m_SelectedArrayNames.find(m_CellEulerAnglesArrayName) != m_SelectedArrayNames.end() )
  {
    //  radianconversion = M_PI / 180.0;
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ctf::Euler1));
    f2 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ctf::Euler2));
    f3 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ctf::Euler3));
    cDims[0] = 3;
    fArray = FloatArrayType::CreateArray(tDims, cDims, SIMPL::CellData::EulerAngles);
    float* cellEulerAngles = fArray->getPointer(0);
    int32_t* cellPhases = iArray->getPointer(0);
    float degToRad = 1.0f;
    if(m_AngleRepresentation != Ebsd::AngleRepresentation::Radians && m_UseTransformations == true)
    {
      degToRad = SIMPLib::Constants::k_PiOver180;
    }
    for (size_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i] * degToRad;
      cellEulerAngles[3 * i + 1] = f2[i] * degToRad;
      cellEulerAngles[3 * i + 2] = f3[i] * degToRad;
      if(m_CrystalStructures[cellPhases[i]] == Ebsd::CrystalStructure::Hexagonal_High)
      {cellEulerAngles[3 * i + 2] = cellEulerAngles[3 * i + 2] + (30.0 * degToRad);}
    }
    cellAttrMatrix->addAttributeArray(SIMPL::CellData::EulerAngles, fArray);
  }

  cDims[0] = 1;
  if (m_SelectedArrayNames.find(Ebsd::Ctf::Bands) != m_SelectedArrayNames.end() )
  {
    phasePtr = reinterpret_cast<int32_t*>(ebsdReader->getPointerByName(Ebsd::Ctf::Bands));
    iArray = Int32ArrayType::CreateArray(tDims, cDims, Ebsd::Ctf::Bands);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    cellAttrMatrix->addAttributeArray(Ebsd::Ctf::Bands, iArray);
  }

  if (m_SelectedArrayNames.find(Ebsd::Ctf::Error) != m_SelectedArrayNames.end() )
  {
    phasePtr = reinterpret_cast<int32_t*>(ebsdReader->getPointerByName(Ebsd::Ctf::Error));
    iArray = Int32ArrayType::CreateArray(tDims, cDims, Ebsd::Ctf::Error);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    cellAttrMatrix->addAttributeArray(Ebsd::Ctf::Error, iArray);
  }

  if (m_SelectedArrayNames.find(Ebsd::Ctf::MAD) != m_SelectedArrayNames.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ctf::MAD));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ctf::MAD);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->addAttributeArray(Ebsd::Ctf::MAD, fArray);
  }

  if (m_SelectedArrayNames.find(Ebsd::Ctf::BC) != m_SelectedArrayNames.end() )
  {
    phasePtr = reinterpret_cast<int32_t*>(ebsdReader->getPointerByName(Ebsd::Ctf::BC));
    iArray = Int32ArrayType::CreateArray(tDims, cDims, Ebsd::Ctf::BC);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    cellAttrMatrix->addAttributeArray(Ebsd::Ctf::BC, iArray);
  }

  if (m_SelectedArrayNames.find(Ebsd::Ctf::BS) != m_SelectedArrayNames.end() )
  {
    phasePtr = reinterpret_cast<int32_t*>(ebsdReader->getPointerByName(Ebsd::Ctf::BS));
    iArray = Int32ArrayType::CreateArray(tDims, cDims, Ebsd::Ctf::BS);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    cellAttrMatrix->addAttributeArray(Ebsd::Ctf::BS, iArray);
  }
  if (m_SelectedArrayNames.find(Ebsd::Ctf::X) != m_SelectedArrayNames.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ctf::X));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ctf::X);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->addAttributeArray(Ebsd::Ctf::X, fArray);
  }
  if (m_SelectedArrayNames.find(Ebsd::Ctf::Y) != m_SelectedArrayNames.end() )
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(Ebsd::Ctf::Y));
    fArray = FloatArrayType::CreateArray(tDims, cDims, Ebsd::Ctf::Y);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->addAttributeArray(Ebsd::Ctf::Y, fArray);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ReadH5Ebsd::newFilterInstance(bool copyFilterParameters)
{
  ReadH5Ebsd::Pointer filter = ReadH5Ebsd::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters() );
    // We are going to hand copy all of the parameters because the other way of copying the parameters are going to
    // miss some of them because we are not enumerating all of them.
    filter->setFilterParameters(getFilterParameters());
    filter->setDataContainerName(getDataContainerName());
    filter->setCellEnsembleAttributeMatrixName(getCellEnsembleAttributeMatrixName());
    filter->setCellAttributeMatrixName(getCellAttributeMatrixName());
    filter->setPhaseNameArrayName(getPhaseNameArrayName());
    filter->setMaterialNameArrayName(getMaterialNameArrayName());
    filter->setInputFile(getInputFile());
    filter->setZStartIndex(getZStartIndex());
    filter->setZEndIndex(getZEndIndex());
    filter->setUseTransformations(getUseTransformations());
    filter->setSelectedArrayNames(getSelectedArrayNames());
    filter->setDataArrayNames(getDataArrayNames());
    filter->setAngleRepresentation(getAngleRepresentation());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadH5Ebsd::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadH5Ebsd::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadH5Ebsd::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadH5Ebsd::getGroupName()
{ return SIMPL::FilterGroups::IOFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadH5Ebsd::getSubGroupName()
{ return SIMPL::FilterSubGroups::InputFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadH5Ebsd::getHumanLabel()
{ return "Read H5EBSD File"; }
