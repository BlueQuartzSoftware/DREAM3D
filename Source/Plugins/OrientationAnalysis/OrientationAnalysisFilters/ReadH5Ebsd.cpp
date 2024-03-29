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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ReadH5Ebsd.h"

#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/CoreFilters/RotateSampleRefFrame.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"

#include "EbsdLib/Core/EbsdMacros.h"
#include "EbsdLib/IO/H5EbsdVolumeInfo.h"
#include "EbsdLib/IO/HKL/CtfFields.h"
#include "EbsdLib/IO/HKL/H5CtfVolumeReader.h"
#include "EbsdLib/IO/TSL/AngFields.h"
#include "EbsdLib/IO/TSL/H5AngVolumeReader.h"

#include "OrientationAnalysis/FilterParameters/ReadH5EbsdFilterParameter.h"
#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,
  AttributeMatrixID22 = 22,

  DataContainerID = 1
};

namespace
{

std::set<std::string> convertToStl(const QSet<QString>& in)
{
  std::set<std::string> out;
  for(const auto& v : in)
  {
    out.insert(v.toStdString());
  }
  return out;
}

#if 0
QSet<QString> convertToQt(std::set<std::string>& in)
{
  QSet<QString> out;
  for(const auto& v : in)
  {
    out.insert(QString::fromStdString(v));
  }
  return out;
}
#endif
} // namespace

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadH5Ebsd::ReadH5Ebsd()
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadH5Ebsd::~ReadH5Ebsd() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(ReadH5EbsdFilterParameter::Create("Import H5Ebsd File", "ReadH5Ebsd", "__NULL__", FilterParameter::Category::Parameter, this, "h5ebsd", "H5Ebsd"));
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Data Container", DataContainerName, FilterParameter::Category::CreatedArray, ReadH5Ebsd));
  parameters.push_back(SeparatorFilterParameter::Create("Cell Data", FilterParameter::Category::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Attribute Matrix", CellAttributeMatrixName, DataContainerName, FilterParameter::Category::CreatedArray, ReadH5Ebsd));
  parameters.push_back(SeparatorFilterParameter::Create("Cell Ensemble Data", FilterParameter::Category::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Ensemble Attribute Matrix", CellEnsembleAttributeMatrixName, DataContainerName, FilterParameter::Category::CreatedArray, ReadH5Ebsd));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readDataArrayPath("DataContainerName", getDataContainerName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setCellEnsembleAttributeMatrixName(reader->readString("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName()));
  setInputFile(reader->readString("InputFile", getInputFile()));
  setRefFrameZDir((reader->readValue("RefFrameZDir", getRefFrameZDir())));
  setZStartIndex(reader->readValue("ZStartIndex", getZStartIndex()));
  setZEndIndex(reader->readValue("ZEndIndex", getZEndIndex()));
  setUseTransformations(reader->readValue("UseTransformations", getUseTransformations()));
  setSelectedArrayNames(reader->readArraySelections("SelectedArrayNames", getSelectedArrayNames()));
  setAngleRepresentation(reader->readValue("AngleRepresentation", getAngleRepresentation()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t ReadH5Ebsd::initDataContainerDimsRes(int64_t dims[3], const DataContainer::Pointer& m)
{
  int32_t err = 0;
/* Sanity check what we are trying to load to make sure it can fit in our address space.
 * Note that this does not guarantee the user has enough left, just that the
 * size of the volume can fit in the address space of the program
 */
#if(CMP_SIZEOF_SSIZE_T == 4)
  int64_t max = std::numeric_limits<size_t>::max();
#else
  int64_t max = std::numeric_limits<int64_t>::max();
#endif
  if(dims[0] * dims[1] * dims[2] > max)
  {
    err = -1;
    QString ss = QObject::tr("The total number of elements '%1' is greater than this program can hold. Try the 64 bit version").arg((dims[0] * dims[1] * dims[2]));
    setErrorCondition(err, ss);
    return err;
  }

  if(dims[0] > max || dims[1] > max || dims[2] > max)
  {
    err = -1;
    QString ss = QObject::tr("One of the dimensions is greater than the max index for this sysem. Try the 64 bit version"
                             " dim[0]=%1  dim[1]=%2  dim[2]=%3")
                     .arg(dims[0])
                     .arg(dims[1])
                     .arg(dims[2]);
    setErrorCondition(err, ss);
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
  if(m_InputFile.isEmpty())
  {
    QString ss = QObject::tr("The input file must be set for property %1").arg("InputFile");
    setErrorCondition(-1, ss);
    return;
  }
  if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist. '%1'").arg(getInputFile());
    setErrorCondition(-388, ss);
    return;
  }
  if(!m_InputFile.isEmpty())
  {
    H5EbsdVolumeInfo::Pointer reader = H5EbsdVolumeInfo::New();
    reader->setFileName(m_InputFile.toStdString());
    int32_t err = reader->readVolumeInfo();
    if(err < 0)
    {
      QString ss = QObject::tr("Error reading VolumeInfo from H5Ebsd File");
      setErrorCondition(-1, ss);
      return;
    }

    // Get the list of data arrays in the EBSD file
    m_DataArrayNames.clear();
    std::set<std::string> daNames = reader->getDataArrayNames();
    for(const auto& name : daNames)
    {
      m_DataArrayNames.insert(QString::fromStdString(name));
    }

    std::string manufacturer = reader->getManufacturer();
    if(manufacturer == EbsdLib::Ang::Manufacturer)
    {
      m_Manufacturer = EbsdLib::OEM::EDAX;
    }
    else if(manufacturer == EbsdLib::Ctf::Manufacturer)
    {
      m_Manufacturer = EbsdLib::OEM::Oxford;
    }
    else
    {
      QString ss = QObject::tr("Original Data source could not be determined. It should be TSL, HKL or HEDM");
      setErrorCondition(-1, ss);
      return;
    }

    int64_t dims[3];
    float res[3];
    reader->getDimsAndResolution(dims[0], dims[1], dims[2], res[0], res[1], res[2]);
    /* Sanity check what we are trying to load to make sure it can fit in our address space.
     * Note that this does not guarantee the user has enough left, just that the
     * size of the volume can fit in the address space of the program
     */
    int64_t max = std::numeric_limits<int64_t>::max();

    if(dims[0] * dims[1] * dims[2] > max)
    {
      err = -1;
      QString ss = QObject::tr("The total number of elements '%1' is greater than this program can hold. Try the 64 bit version").arg(dims[0] * dims[1] * dims[2]);
      setErrorCondition(err, ss);
      return;
    }

    if(dims[0] > max || dims[1] > max || dims[2] > max)
    {
      err = -1;
      QString ss = QObject::tr("One of the dimensions is greater than the max index for this sysem. Try the 64 bit version"
                               " dim[0]=%1  dim[1]=%2  dim[2]=%3")
                       .arg(dims[0])
                       .arg(dims[1])
                       .arg(dims[0]);
      setErrorCondition(err, ss);
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
  clearErrorCode();
  clearWarningCode();
  initialize();

  DataArrayPath tempPath;

  m_DataArrayNames.clear(); // Remove all the data arrays

  H5EbsdVolumeInfo::Pointer volumeInfoReader = H5EbsdVolumeInfo::New();
  QFileInfo fi(getInputFile());
  if(!fi.exists())
  {
    QString ss = QObject::tr("The input file was not specified or was empty");
    setErrorCondition(-10, ss);
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer(this, getDataContainerName(), DataContainerID);
  if(getErrorCode() < 0)
  {
    return;
  }

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

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

  volumeInfoReader->setFileName(m_InputFile.toStdString());
  H5EbsdVolumeReader::Pointer reader;
  std::vector<std::string> names;

  int64_t dims[3] = {0, 0, 0};
  FloatVec3Type res = {0.0f, 0.0f, 0.0f};
  int32_t err = volumeInfoReader->getDimsAndResolution(dims[0], dims[1], dims[2], res[0], res[1], res[2]);

  if(err < 0)
  {
    QString ss = QObject::tr("File counld not be read properly");
    setErrorCondition(-11, ss);
    return;
  }
  std::string manufacturer = volumeInfoReader->getManufacturer();
  if(manufacturer == EbsdLib::Ang::Manufacturer)
  {
    m_Manufacturer = EbsdLib::OEM::EDAX;
  }
  else if(manufacturer == EbsdLib::Ctf::Manufacturer)
  {
    m_Manufacturer = EbsdLib::OEM::Oxford;
  }

  if(m_ZEndIndex < m_ZStartIndex)
  {
    QString ss = QObject::tr("The End Slice [%1] MUST be larger than the Start Slice [%2]. This condition was not met.").arg(m_ZEndIndex).arg(m_ZStartIndex);
    setErrorCondition(-12, ss);
    return;
  }

  size_t dcDims[3] = {static_cast<size_t>(dims[0]), static_cast<size_t>(dims[1]), static_cast<size_t>(dims[2])};
  // Now Calculate our "subvolume" of slices, ie, those start and end values that the user selected from the GUI
  dcDims[2] = m_ZEndIndex - m_ZStartIndex + 1;
  m->getGeometryAs<ImageGeom>()->setDimensions(dcDims);
  m->getGeometryAs<ImageGeom>()->setSpacing(res);

  // Update the size fo the Cell Attribute Matrix now that the dimensions of the volume are known
  tDims.resize(3);
  tDims[0] = dcDims[0];
  tDims[1] = dcDims[1];
  tDims[2] = dcDims[2];
  cellAttrMat->resizeAttributeArrays(tDims);

  if(m_Manufacturer == EbsdLib::OEM::EDAX)
  {
    AngFields angFeatures;
    reader = H5AngVolumeReader::New();
    names = angFeatures.getFilterFeatures<std::vector<std::string>>();
  }
  else if(m_Manufacturer == EbsdLib::OEM::Oxford)
  {
    CtfFields cfeatures;
    reader = H5CtfVolumeReader::New();
    names = cfeatures.getFilterFeatures<std::vector<std::string>>();
  }
  else
  {
    QString ss = QObject::tr("Original Data source could not be determined. It should be TSL or HKL");
    setErrorCondition(-1, ss);
    return;
  }

  // Get the list of data arrays in the EBSD file
  m_DataArrayNames.clear();
  std::set<std::string> daNames = reader->getDataArrayNames();
  for(const auto& name : daNames)
  {
    m_DataArrayNames.insert(QString::fromStdString(name));
  }

  std::vector<size_t> cDims(1, 1);
  for(int32_t i = 0; i < names.size(); ++i)
  {
    // First check to see if the name is in our list of names to read.
    if(m_SelectedArrayNames.find(S2Q(names[i])) == m_SelectedArrayNames.end())
    {
      continue;
    }
    if(reader->getPointerType(names[i]) == EbsdLib::NumericTypes::Type::Int32)
    {
      cellAttrMat->createAndAddAttributeArray<DataArray<int32_t>>(this, S2Q(names[i]), 0, cDims);
    }
    else if(reader->getPointerType(names[i]) == EbsdLib::NumericTypes::Type::Float)
    {
      cellAttrMat->createAndAddAttributeArray<DataArray<float>>(this, S2Q(names[i]), 0, cDims);
    }
  }

  // Only read these arrays if the user wants them
  if(m_SelectedArrayNames.contains(SIMPL::CellData::EulerAngles))
  {
    cDims[0] = 3;
    tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), getCellEulerAnglesArrayName());
    m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, tempPath, 0, cDims);
    if(nullptr != m_CellEulerAnglesPtr.lock())
    {
      m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  // Only read the phases if the user wants it.
  if(m_SelectedArrayNames.contains(SIMPL::CellData::Phases))
  {
    cDims[0] = 1;
    tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), getCellPhasesArrayName());
    m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>>(this, tempPath, 0, cDims);
    if(nullptr != m_CellPhasesPtr.lock())
    {
      m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  // Now create the Ensemble arrays for the XTal Structures, Material Names and LatticeConstants
  cDims[0] = 1;
  tempPath.update(getDataContainerName().getDataContainerName(), getCellEnsembleAttributeMatrixName(), getCrystalStructuresArrayName());
  m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>>(this, tempPath, EbsdLib::CrystalStructure::UnknownCrystalStructure, cDims);
  if(nullptr != m_CrystalStructuresPtr.lock())
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  StringDataArray::Pointer materialNamesPtr = StringDataArray::CreateArray(cellEnsembleAttrMat->getNumberOfTuples(), getMaterialNameArrayName(), true);
  cellEnsembleAttrMat->insertOrAssign(materialNamesPtr);
  m_MaterialNamesPtr = materialNamesPtr;

  cDims[0] = 6;
  tempPath.update(getDataContainerName().getDataContainerName(), getCellEnsembleAttributeMatrixName(), getLatticeConstantsArrayName());
  m_LatticeConstantsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, tempPath, 0.0, cDims);
  if(nullptr != m_LatticeConstantsPtr.lock())
  {
    m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
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
  Q_EMIT preflightAboutToExecute();
  // Let the GUI Widget (or anything else) update the parameters for this filter
  Q_EMIT updateFilterParameters(this);
  dataCheck();
  Q_EMIT preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::execute()
{
  clearErrorCode();
  clearWarningCode();

  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  std::string manufacturer;
  // Get the Size and Spacing of the Volume
  {
    H5EbsdVolumeInfo::Pointer volumeInfoReader = H5EbsdVolumeInfo::New();
    volumeInfoReader->setFileName(m_InputFile.toStdString());
    int err = volumeInfoReader->readVolumeInfo();
    if(err < 0)
    {
      setErrorCondition(err, tr("Unable to read volume information for file '%1'").arg(m_InputFile));
      return;
    }
    int64_t dims[3] = {0, 0, 0};
    FloatVec3Type res = {0.0f, 0.0f, 0.0f};
    volumeInfoReader->getDimsAndResolution(dims[0], dims[1], dims[2], res[0], res[1], res[2]);

    size_t dcDims[3] = {static_cast<size_t>(dims[0]), static_cast<size_t>(dims[1]), static_cast<size_t>(dims[2])};
    m->getGeometryAs<ImageGeom>()->setDimensions(dcDims);
    m->getGeometryAs<ImageGeom>()->setSpacing(res);
    // Now Calculate our "subvolume" of slices, ie, those start and end values that the user selected from the GUI
    dcDims[2] = m_ZEndIndex - m_ZStartIndex + 1;
    m->getGeometryAs<ImageGeom>()->setDimensions(dcDims);
    manufacturer = volumeInfoReader->getManufacturer();
    m_RefFrameZDir = volumeInfoReader->getStackingOrder();

    std::array<float, 3> sampleTransAxis = volumeInfoReader->getSampleTransformationAxis();
    m_SampleTransformation.h = sampleTransAxis[0];
    m_SampleTransformation.k = sampleTransAxis[1];
    m_SampleTransformation.l = sampleTransAxis[2];
    m_SampleTransformation.angle = volumeInfoReader->getSampleTransformationAngle();

    std::array<float, 3> eulerTransAxis = volumeInfoReader->getEulerTransformationAxis();
    m_EulerTransformation.h = eulerTransAxis[0];
    m_EulerTransformation.k = eulerTransAxis[1];
    m_EulerTransformation.l = eulerTransAxis[2];
    m_EulerTransformation.angle = volumeInfoReader->getEulerTransformationAngle();
    volumeInfoReader = H5EbsdVolumeInfo::NullPointer();
  }
  H5EbsdVolumeReader::Pointer ebsdReader;
  if(manufacturer == EbsdLib::Ang::Manufacturer)
  {
    ebsdReader = initTSLEbsdVolumeReader();
  }
  else if(manufacturer == EbsdLib::Ctf::Manufacturer)
  {
    ebsdReader = initHKLEbsdVolumeReader();
  }
  else
  {

    QString ss = QObject::tr("Could not determine or match a supported manufacturer from the data file. Supported manufacturer codes are: %1, %2 and %3")
                     .arg(S2Q(EbsdLib::Ctf::Manufacturer), S2Q(EbsdLib::Ang::Manufacturer));
    setErrorCondition(-1, ss);
    return;
  }

  // Sanity Check the Error Condition or the state of the EBSD Reader Object.
  if(getErrorCode() < 0 || nullptr == ebsdReader.get())
  {
    return;
  }

  // Initialize all the arrays with some default values
  {
    QString ss = QObject::tr("Reading Ebsd Data from file %1").arg(getInputFile());
    notifyStatusMessage(ss);
  }
  ebsdReader->setSliceStart(m_ZStartIndex);
  ebsdReader->setSliceEnd(m_ZEndIndex);
  ebsdReader->readAllArrays(false);
  ebsdReader->setArraysToRead(::convertToStl(m_SelectedArrayNames));
  int err = ebsdReader->loadData(m->getGeometryAs<ImageGeom>()->getXPoints(), m->getGeometryAs<ImageGeom>()->getYPoints(), m->getGeometryAs<ImageGeom>()->getZPoints(), m_RefFrameZDir);
  if(err < 0)
  {
    setErrorCondition(err, S2Q(ebsdReader->getErrorMessage()));
    setErrorCondition(-1, "Error Loading Data from Ebsd Data file.");
    return;
  }

  // Copy the data from the pointers embedded in the reader object into our data container (Cell array).
  if(manufacturer == EbsdLib::Ang::Manufacturer)
  {
    copyTSLArrays(ebsdReader.get());
  }
  else if(manufacturer == EbsdLib::Ctf::Manufacturer)
  {
    copyHKLArrays(ebsdReader.get());
  }

  else
  {
    QString ss = QObject::tr("Could not determine or match a supported manufacturer from the data file. Supported manufacturer codes are: %1 and %2")
                     .arg(S2Q(EbsdLib::Ctf::Manufacturer))
                     .arg(S2Q(EbsdLib::Ang::Manufacturer));
    setErrorCondition(-109875, ss);
    return;
  }

  if(m_UseTransformations)
  {

    if(m_SampleTransformation.angle > 0)
    {
      FloatVec3Type sampleAxis;
      sampleAxis[0] = m_SampleTransformation.h;
      sampleAxis[1] = m_SampleTransformation.k;
      sampleAxis[2] = m_SampleTransformation.l;

      RotateSampleRefFrame::Pointer rot_Sample = RotateSampleRefFrame::New();

      // Connect up the Error/Warning/Progress object so the filter can report those things
      connect(rot_Sample.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), this, SIGNAL(messageGenerated(const AbstractMessage::Pointer&)));
      rot_Sample->setDataContainerArray(getDataContainerArray()); // AbstractFilter implements this so no problem
      // Now set the filter parameters for the filter using QProperty System since we can not directly
      // instantiate the filter since it resides in a plugin. These calls are SLOW. DO NOT EVER do this in a
      // tight loop. Your filter will slow down by 10X.
      rot_Sample->setRotationAngle(m_SampleTransformation.angle);
      rot_Sample->setRotationAxis(sampleAxis);
      rot_Sample->setSliceBySlice(true);

      DataArrayPath tempPath;
      tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), "");
      rot_Sample->setCellAttributeMatrixPath(tempPath);

      rot_Sample->execute();
    }

    if(m_EulerTransformation.angle > 0)
    {
      FloatVec3Type eulerAxis;
      eulerAxis[0] = m_EulerTransformation.h;
      eulerAxis[1] = m_EulerTransformation.k;
      eulerAxis[2] = m_EulerTransformation.l;

      QString filtName = "RotateEulerRefFrame";
      FilterManager* fm = FilterManager::Instance();
      IFilterFactory::Pointer rotEulerFactory = fm->getFactoryFromClassName(filtName);
      if(nullptr != rotEulerFactory.get())
      {
        // If we get this far, the Factory is good so creating the filter should not fail unless something has
        // horribly gone wrong in which case the system is going to come down quickly after this.
        AbstractFilter::Pointer rot_Euler = rotEulerFactory->create();

        // Connect up the Error/Warning/Progress object so the filter can report those things
        connect(rot_Euler.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), this, SIGNAL(messageGenerated(const AbstractMessage::Pointer&)));
        rot_Euler->setDataContainerArray(getDataContainerArray()); // AbstractFilter implements this so no problem
        // Now set the filter parameters for the filter using QProperty System since we can not directly
        // instantiate the filter since it resides in a plugin. These calls are SLOW. DO NOT EVER do this in a
        // tight loop. Your filter will slow down by 10X.
        bool propWasSet = rot_Euler->setProperty("RotationAngle", m_EulerTransformation.angle);
        if(!propWasSet)
        {
          QString ss = QObject::tr("ReadH5Ebsd Error Setting Property '%1' into filter '%2' which is a subfilter called by ReadH5Ebsd. The property was not set which could mean the property was not "
                                   "exposed with a Q_PROPERTY macro. Please notify the developers")
                           .arg("RotationAngle")
                           .arg(filtName);
          setErrorCondition(-109884, ss);
        }
        QVariant v;
        v.setValue(eulerAxis);
        propWasSet = rot_Euler->setProperty("RotationAxis", v);
        if(!propWasSet)
        {
          QString ss = QObject::tr("ReadH5Ebsd Error Setting Property '%1' into filter '%2' which is a subfilter called by ReadH5Ebsd. The property was not set which could mean the property was not "
                                   "exposed with a Q_PROPERTY macro. Please notify the developers")
                           .arg("RotationAxis")
                           .arg(filtName);
          setErrorCondition(-109873, ss);
        }
        DataArrayPath tempPath;
        tempPath.update(getDataContainerName().getDataContainerName(), getCellAttributeMatrixName(), getCellEulerAnglesArrayName());
        v.setValue(tempPath);
        propWasSet = rot_Euler->setProperty("CellEulerAnglesArrayPath", v);
        if(!propWasSet)
        {
          QString ss = QObject::tr("ReadH5Ebsd Error Setting Property '%1' into filter '%2' which is a subfilter called by ReadH5Ebsd. The property was not set which could mean the property was not "
                                   "exposed with a Q_PROPERTY macro. Please notify the developers")
                           .arg("CellEulerAnglesArrayPath")
                           .arg(filtName);
          setErrorCondition(-109872, ss);
        }
        rot_Euler->execute();
      }
      else
      {
        QString ss = QObject::tr("ReadH5Ebsd Error creating filter '%1' which is a subfilter called by ReadH5Ebsd. Filter was not created/executed. Please notify the developers").arg(filtName);
        setErrorCondition(-109881, ss);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
H5EbsdVolumeReader::Pointer ReadH5Ebsd::initTSLEbsdVolumeReader()
{
  int32_t err = 0;
  H5EbsdVolumeReader::Pointer ebsdReader = H5AngVolumeReader::New();
  if(nullptr == ebsdReader)
  {
    setErrorCondition(-1, "Could not Create H5AngVolumeReader object.");
    return H5EbsdVolumeReader::NullPointer();
  }
  H5AngVolumeReader* angReader = dynamic_cast<H5AngVolumeReader*>(ebsdReader.get());
  err = loadInfo<H5AngVolumeReader, AngPhase>(angReader);
  if(err < 0)
  {
    setErrorCondition(-1, "Could not read information about the Ebsd Volume.");
    return H5EbsdVolumeReader::NullPointer();
  }
  if(m_SelectedArrayNames.find(m_CellEulerAnglesArrayName) != m_SelectedArrayNames.end())
  {
    m_SelectedArrayNames.insert(S2Q(EbsdLib::Ang::Phi1));
    m_SelectedArrayNames.insert(S2Q(EbsdLib::Ang::Phi));
    m_SelectedArrayNames.insert(S2Q(EbsdLib::Ang::Phi2));
  }
  if(m_SelectedArrayNames.find(m_CellPhasesArrayName) != m_SelectedArrayNames.end())
  {
    m_SelectedArrayNames.insert(S2Q(EbsdLib::Ang::PhaseData));
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
  if(nullptr == ebsdReader)
  {
    setErrorCondition(-1, "Could not Create H5CtfVolumeReader object.");
    return H5EbsdVolumeReader::NullPointer();
  }
  H5CtfVolumeReader* ctfReader = dynamic_cast<H5CtfVolumeReader*>(ebsdReader.get());
  err = loadInfo<H5CtfVolumeReader, CtfPhase>(ctfReader);
  if(err < 0)
  {
    setErrorCondition(-1, "Could not read information about the Ebsd Volume.");
    return H5EbsdVolumeReader::NullPointer();
  }
  if(m_SelectedArrayNames.find(m_CellEulerAnglesArrayName) != m_SelectedArrayNames.end())
  {
    m_SelectedArrayNames.insert(S2Q(EbsdLib::Ctf::Euler1));
    m_SelectedArrayNames.insert(S2Q(EbsdLib::Ctf::Euler2));
    m_SelectedArrayNames.insert(S2Q(EbsdLib::Ctf::Euler3));
  }
  if(m_SelectedArrayNames.find(m_CellPhasesArrayName) != m_SelectedArrayNames.end())
  {
    m_SelectedArrayNames.insert(S2Q(EbsdLib::Ctf::Phase));
  }
  return ebsdReader;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::copyTSLArrays(H5EbsdVolumeReader* ebsdReader)
{
  float* f1 = nullptr;
  float* f2 = nullptr;
  float* f3 = nullptr;
  int32_t* phasePtr = nullptr;

  FloatArrayType::Pointer fArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  AttributeMatrix::Pointer cellAttrMatrix = m->getAttributeMatrix(getCellAttributeMatrixName());
  std::vector<size_t> tDims(3, 0);
  tDims[0] = m->getGeometryAs<ImageGeom>()->getXPoints();
  tDims[1] = m->getGeometryAs<ImageGeom>()->getYPoints();
  tDims[2] = m->getGeometryAs<ImageGeom>()->getZPoints();
  cellAttrMatrix->resizeAttributeArrays(tDims); // Resize the attribute Matrix to the proper dimensions

  size_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();
  std::vector<size_t> cDims(1, 1);
  if(m_SelectedArrayNames.find(m_CellPhasesArrayName) != m_SelectedArrayNames.end())
  {
    phasePtr = reinterpret_cast<int32_t*>(ebsdReader->getPointerByName(EbsdLib::Ang::PhaseData));
    iArray = Int32ArrayType::CreateArray(tDims, cDims, SIMPL::CellData::Phases, true);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    cellAttrMatrix->insertOrAssign(iArray);
  }

  if(m_SelectedArrayNames.find(m_CellEulerAnglesArrayName) != m_SelectedArrayNames.end())
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(EbsdLib::Ang::Phi1));
    f2 = reinterpret_cast<float*>(ebsdReader->getPointerByName(EbsdLib::Ang::Phi));
    f3 = reinterpret_cast<float*>(ebsdReader->getPointerByName(EbsdLib::Ang::Phi2));
    cDims[0] = 3;
    fArray = FloatArrayType::CreateArray(tDims, cDims, SIMPL::CellData::EulerAngles, true);
    float* cellEulerAngles = fArray->getPointer(0);
    float degToRad = 1.0f;
    if(m_AngleRepresentation != EbsdLib::AngleRepresentation::Radians && m_UseTransformations)
    {
      degToRad = SIMPLib::Constants::k_PiOver180D;
    }
    for(size_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i] * degToRad;
      cellEulerAngles[3 * i + 1] = f2[i] * degToRad;
      cellEulerAngles[3 * i + 2] = f3[i] * degToRad;
    }
    cellAttrMatrix->insertOrAssign(fArray);
  }

  // Reset this back to 1 for the rest of the arrays
  cDims[0] = 1;

  if(m_SelectedArrayNames.find(S2Q(EbsdLib::Ang::ImageQuality)) != m_SelectedArrayNames.end())
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(EbsdLib::Ang::ImageQuality));
    fArray = FloatArrayType::CreateArray(tDims, cDims, S2Q(EbsdLib::Ang::ImageQuality), true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->insertOrAssign(fArray);
  }

  if(m_SelectedArrayNames.find(S2Q(EbsdLib::Ang::ConfidenceIndex)) != m_SelectedArrayNames.end())
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(EbsdLib::Ang::ConfidenceIndex));
    fArray = FloatArrayType::CreateArray(tDims, cDims, S2Q(EbsdLib::Ang::ConfidenceIndex), true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->insertOrAssign(fArray);
  }

  if(m_SelectedArrayNames.find(S2Q(EbsdLib::Ang::SEMSignal)) != m_SelectedArrayNames.end())
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(EbsdLib::Ang::SEMSignal));
    fArray = FloatArrayType::CreateArray(tDims, cDims, S2Q(EbsdLib::Ang::SEMSignal), true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->insertOrAssign(fArray);
  }

  if(m_SelectedArrayNames.find(S2Q(EbsdLib::Ang::Fit)) != m_SelectedArrayNames.end())
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(EbsdLib::Ang::Fit));
    fArray = FloatArrayType::CreateArray(tDims, cDims, S2Q(EbsdLib::Ang::Fit), true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->insertOrAssign(fArray);
  }

  if(m_SelectedArrayNames.find(S2Q(EbsdLib::Ang::XPosition)) != m_SelectedArrayNames.end())
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(EbsdLib::Ang::XPosition));
    fArray = FloatArrayType::CreateArray(tDims, cDims, S2Q(EbsdLib::Ang::XPosition), true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->insertOrAssign(fArray);
  }

  if(m_SelectedArrayNames.find(S2Q(EbsdLib::Ang::YPosition)) != m_SelectedArrayNames.end())
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(EbsdLib::Ang::YPosition));
    fArray = FloatArrayType::CreateArray(tDims, cDims, S2Q(EbsdLib::Ang::YPosition), true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->insertOrAssign(fArray);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadH5Ebsd::copyHKLArrays(H5EbsdVolumeReader* ebsdReader)
{
  float* f1 = nullptr;
  float* f2 = nullptr;
  float* f3 = nullptr;
  int32_t* phasePtr = nullptr;

  FloatArrayType::Pointer fArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer cellAttrMatrix = m->getAttributeMatrix(getCellAttributeMatrixName());
  std::vector<size_t> tDims(3, 0);
  tDims[0] = m->getGeometryAs<ImageGeom>()->getXPoints();
  tDims[1] = m->getGeometryAs<ImageGeom>()->getYPoints();
  tDims[2] = m->getGeometryAs<ImageGeom>()->getZPoints();
  cellAttrMatrix->resizeAttributeArrays(tDims); // Resize the attribute Matrix to the proper dimensions

  size_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();
  std::vector<size_t> cDims(1, 1);
  phasePtr = reinterpret_cast<int32_t*>(ebsdReader->getPointerByName(EbsdLib::Ctf::Phase));
  iArray = Int32ArrayType::CreateArray(tDims, cDims, SIMPL::CellData::Phases, true);
  ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
  cellAttrMatrix->insertOrAssign(iArray);

  if(m_SelectedArrayNames.find(m_CellEulerAnglesArrayName) != m_SelectedArrayNames.end())
  {
    //  radianconversion = M_PI / 180.0;
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(EbsdLib::Ctf::Euler1));
    f2 = reinterpret_cast<float*>(ebsdReader->getPointerByName(EbsdLib::Ctf::Euler2));
    f3 = reinterpret_cast<float*>(ebsdReader->getPointerByName(EbsdLib::Ctf::Euler3));
    cDims[0] = 3;
    fArray = FloatArrayType::CreateArray(tDims, cDims, SIMPL::CellData::EulerAngles, true);
    float* cellEulerAngles = fArray->getPointer(0);
    int32_t* cellPhases = iArray->getPointer(0);
    float degToRad = 1.0f;
    if(m_AngleRepresentation != EbsdLib::AngleRepresentation::Radians && m_UseTransformations)
    {
      degToRad = SIMPLib::Constants::k_PiOver180D;
    }
    for(size_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i] * degToRad;
      cellEulerAngles[3 * i + 1] = f2[i] * degToRad;
      cellEulerAngles[3 * i + 2] = f3[i] * degToRad;
      if(m_CrystalStructures[cellPhases[i]] == EbsdLib::CrystalStructure::Hexagonal_High)
      {
        cellEulerAngles[3 * i + 2] = cellEulerAngles[3 * i + 2] + (30.0 * degToRad);
      }
    }
    cellAttrMatrix->insertOrAssign(fArray);
  }

  cDims[0] = 1;
  if(m_SelectedArrayNames.find(S2Q(EbsdLib::Ctf::Bands)) != m_SelectedArrayNames.end())
  {
    phasePtr = reinterpret_cast<int32_t*>(ebsdReader->getPointerByName(EbsdLib::Ctf::Bands));
    iArray = Int32ArrayType::CreateArray(tDims, cDims, S2Q(EbsdLib::Ctf::Bands), true);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    cellAttrMatrix->insertOrAssign(iArray);
  }

  if(m_SelectedArrayNames.find(S2Q(EbsdLib::Ctf::Error)) != m_SelectedArrayNames.end())
  {
    phasePtr = reinterpret_cast<int32_t*>(ebsdReader->getPointerByName(EbsdLib::Ctf::Error));
    iArray = Int32ArrayType::CreateArray(tDims, cDims, S2Q(EbsdLib::Ctf::Error), true);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    cellAttrMatrix->insertOrAssign(iArray);
  }

  if(m_SelectedArrayNames.find(S2Q(EbsdLib::Ctf::MAD)) != m_SelectedArrayNames.end())
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(EbsdLib::Ctf::MAD));
    fArray = FloatArrayType::CreateArray(tDims, cDims, S2Q(EbsdLib::Ctf::MAD), true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->insertOrAssign(fArray);
  }

  if(m_SelectedArrayNames.find(S2Q(EbsdLib::Ctf::BC)) != m_SelectedArrayNames.end())
  {
    phasePtr = reinterpret_cast<int32_t*>(ebsdReader->getPointerByName(EbsdLib::Ctf::BC));
    iArray = Int32ArrayType::CreateArray(tDims, cDims, S2Q(EbsdLib::Ctf::BC), true);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    cellAttrMatrix->insertOrAssign(iArray);
  }

  if(m_SelectedArrayNames.find(S2Q(EbsdLib::Ctf::BS)) != m_SelectedArrayNames.end())
  {
    phasePtr = reinterpret_cast<int32_t*>(ebsdReader->getPointerByName(EbsdLib::Ctf::BS));
    iArray = Int32ArrayType::CreateArray(tDims, cDims, S2Q(EbsdLib::Ctf::BS), true);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    cellAttrMatrix->insertOrAssign(iArray);
  }
  if(m_SelectedArrayNames.find(S2Q(EbsdLib::Ctf::X)) != m_SelectedArrayNames.end())
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(EbsdLib::Ctf::X));
    fArray = FloatArrayType::CreateArray(tDims, cDims, S2Q(EbsdLib::Ctf::X), true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->insertOrAssign(fArray);
  }
  if(m_SelectedArrayNames.find(S2Q(EbsdLib::Ctf::Y)) != m_SelectedArrayNames.end())
  {
    f1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(EbsdLib::Ctf::Y));
    fArray = FloatArrayType::CreateArray(tDims, cDims, S2Q(EbsdLib::Ctf::Y), true);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMatrix->insertOrAssign(fArray);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ReadH5Ebsd::newFilterInstance(bool copyFilterParameters) const
{
  ReadH5Ebsd::Pointer filter = ReadH5Ebsd::New();
  if(copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters());
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
QString ReadH5Ebsd::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadH5Ebsd::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadH5Ebsd::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadH5Ebsd::getGroupName() const
{
  return SIMPL::FilterGroups::IOFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid ReadH5Ebsd::getUuid() const
{
  return QUuid("{4ef7f56b-616e-5a80-9e68-1da8f35ad235}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadH5Ebsd::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ReadH5Ebsd::getHumanLabel() const
{
  return "Import H5EBSD File";
}

// -----------------------------------------------------------------------------
ReadH5Ebsd::Pointer ReadH5Ebsd::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<ReadH5Ebsd> ReadH5Ebsd::New()
{
  struct make_shared_enabler : public ReadH5Ebsd
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString ReadH5Ebsd::getNameOfClass() const
{
  return QString("ReadH5Ebsd");
}

// -----------------------------------------------------------------------------
QString ReadH5Ebsd::ClassName()
{
  return QString("ReadH5Ebsd");
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setDataContainerName(const DataArrayPath& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath ReadH5Ebsd::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setCellEnsembleAttributeMatrixName(const QString& value)
{
  m_CellEnsembleAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ReadH5Ebsd::getCellEnsembleAttributeMatrixName() const
{
  return m_CellEnsembleAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString ReadH5Ebsd::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setPhaseNameArrayName(const QString& value)
{
  m_PhaseNameArrayName = value;
}

// -----------------------------------------------------------------------------
QString ReadH5Ebsd::getPhaseNameArrayName() const
{
  return m_PhaseNameArrayName;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setMaterialNameArrayName(const QString& value)
{
  m_MaterialNameArrayName = value;
}

// -----------------------------------------------------------------------------
QString ReadH5Ebsd::getMaterialNameArrayName() const
{
  return m_MaterialNameArrayName;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setInputFile(const QString& value)
{
  m_InputFile = value;
}

// -----------------------------------------------------------------------------
QString ReadH5Ebsd::getInputFile() const
{
  return m_InputFile;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setZStartIndex(int value)
{
  m_ZStartIndex = value;
}

// -----------------------------------------------------------------------------
int ReadH5Ebsd::getZStartIndex() const
{
  return m_ZStartIndex;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setZEndIndex(int value)
{
  m_ZEndIndex = value;
}

// -----------------------------------------------------------------------------
int ReadH5Ebsd::getZEndIndex() const
{
  return m_ZEndIndex;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setUseTransformations(bool value)
{
  m_UseTransformations = value;
}

// -----------------------------------------------------------------------------
bool ReadH5Ebsd::getUseTransformations() const
{
  return m_UseTransformations;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setSelectedArrayNames(const QSet<QString>& value)
{
  m_SelectedArrayNames = value;
}

// -----------------------------------------------------------------------------
QSet<QString> ReadH5Ebsd::getSelectedArrayNames() const
{
  return m_SelectedArrayNames;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setDataArrayNames(const QSet<QString>& value)
{
  m_DataArrayNames = value;
}

// -----------------------------------------------------------------------------
QSet<QString> ReadH5Ebsd::getDataArrayNames() const
{
  return m_DataArrayNames;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setAngleRepresentation(int value)
{
  m_AngleRepresentation = value;
}

// -----------------------------------------------------------------------------
int ReadH5Ebsd::getAngleRepresentation() const
{
  return m_AngleRepresentation;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setRefFrameZDir(uint32_t value)
{
  m_RefFrameZDir = value;
}

// -----------------------------------------------------------------------------
uint32_t ReadH5Ebsd::getRefFrameZDir() const
{
  return m_RefFrameZDir;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setManufacturer(const EbsdLib::OEM& value)
{
  m_Manufacturer = value;
}

// -----------------------------------------------------------------------------
EbsdLib::OEM ReadH5Ebsd::getManufacturer() const
{
  return m_Manufacturer;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setSampleTransformation(const AxisAngleInput& value)
{
  m_SampleTransformation = value;
}

// -----------------------------------------------------------------------------
AxisAngleInput ReadH5Ebsd::getSampleTransformation() const
{
  return m_SampleTransformation;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setEulerTransformation(const AxisAngleInput& value)
{
  m_EulerTransformation = value;
}

// -----------------------------------------------------------------------------
AxisAngleInput ReadH5Ebsd::getEulerTransformation() const
{
  return m_EulerTransformation;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setCrystalStructuresArrayName(const QString& value)
{
  m_CrystalStructuresArrayName = value;
}

// -----------------------------------------------------------------------------
QString ReadH5Ebsd::getCrystalStructuresArrayName() const
{
  return m_CrystalStructuresArrayName;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setLatticeConstantsArrayName(const QString& value)
{
  m_LatticeConstantsArrayName = value;
}

// -----------------------------------------------------------------------------
QString ReadH5Ebsd::getLatticeConstantsArrayName() const
{
  return m_LatticeConstantsArrayName;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setCellPhasesArrayName(const QString& value)
{
  m_CellPhasesArrayName = value;
}

// -----------------------------------------------------------------------------
QString ReadH5Ebsd::getCellPhasesArrayName() const
{
  return m_CellPhasesArrayName;
}

// -----------------------------------------------------------------------------
void ReadH5Ebsd::setCellEulerAnglesArrayName(const QString& value)
{
  m_CellEulerAnglesArrayName = value;
}

// -----------------------------------------------------------------------------
QString ReadH5Ebsd::getCellEulerAnglesArrayName() const
{
  return m_CellEulerAnglesArrayName;
}
