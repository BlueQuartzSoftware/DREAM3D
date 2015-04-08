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

#include <QtCore/QFileInfo>

#include "H5Support/HDF5ScopedFileSentinel.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"

#include "DREAM3DLib/CoreFilters/DataContainerReader.h"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "SyntheticBuilding/SyntheticBuildingConstants.h"


#define INIT_SYNTH_VOLUME_CHECK(var, errCond) \
  if (m_##var <= 0) { QString ss = QObject::tr(":%1 must be a value > 0\n").arg( #var); notifyErrorMessage(getHumanLabel(), ss, errCond);}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InitializeSyntheticVolume::InitializeSyntheticVolume() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::SyntheticVolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_InputStatsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::Statistics),
  m_InputPhaseTypesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::PhaseTypes),
  m_EstimateNumberOfFeatures(false),
  m_EstimatedPrimaryFeatures(0)
{
  m_Dimensions.x = 128;
  m_Dimensions.y = 128;
  m_Dimensions.z = 128;

  m_Resolution.x = 0.25;
  m_Resolution.y = 0.25;
  m_Resolution.z = 0.25;

  m_Origin.x = 0.0f;
  m_Origin.y = 0.0f;
  m_Origin.z = 0.0f;

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

  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Input Statistics", "InputStatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getInputStatsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Input Phase Types", "InputPhaseTypesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getInputPhaseTypesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("New DataContainer Name", "DataContainerName", FilterParameterWidgetType::StringWidget, getDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("New Cell Attribute Matrix Name", "CellAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Dimensions", "Dimensions", FilterParameterWidgetType::IntVec3Widget, getDimensions(), false, "Voxels"));
  parameters.push_back(FilterParameter::New("Resolution", "Resolution", FilterParameterWidgetType::FloatVec3Widget, getResolution(), false, "Microns"));
  parameters.push_back(FilterParameter::New("Origin", "Origin", FilterParameterWidgetType::FloatVec3Widget, getOrigin(), false, "Microns"));

  QStringList linkedProps("EstimatedPrimaryFeatures");
  linkedProps << "InputStatsFile";
  parameters.push_back(FilterParameter::New("Optional Information", "", FilterParameterWidgetType::SeparatorWidget, "", false));

  parameters.push_back(LinkedBooleanFilterParameter::New("Estimate Number of Features", "EstimateNumberOfFeatures", getEstimateNumberOfFeatures(), linkedProps, false));
  parameters.push_back(FileSystemFilterParameter::New("Input Stats File", "InputStatsFile", FilterParameterWidgetType::InputFileWidget, getInputStatsFile(), false, "", "*.dream3d"));
  parameters.push_back(FilterParameter::New("Estimated Primary Features", "EstimatedPrimaryFeatures", FilterParameterWidgetType::PreflightUpdatedValueWidget, getEstimatedPrimaryFeatures(), false, ""));
  parameters.back()->setReadOnly(true);
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setDimensions( reader->readIntVec3("Dimensions", getDimensions() ) );
  setResolution( reader->readFloatVec3("Resolution", getResolution() ) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin() ) );
  setInputStatsArrayPath(reader->readDataArrayPath("InputStatsArrayPath", getInputStatsArrayPath() ) );
  setInputPhaseTypesArrayPath(reader->readDataArrayPath("InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath() ) );
  setInputStatsFile(reader->readString("InputStatsFile", getInputStatsFile() ) );
  setEstimateNumberOfFeatures(reader->readValue("EstimateNumberOfFeatures", getEstimateNumberOfFeatures() ) );

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int InitializeSyntheticVolume::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(DataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(Dimensions)
  DREAM3D_FILTER_WRITE_PARAMETER(Resolution)
  DREAM3D_FILTER_WRITE_PARAMETER(Origin)
  DREAM3D_FILTER_WRITE_PARAMETER(InputStatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(InputPhaseTypesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(InputStatsFile);
  DREAM3D_FILTER_WRITE_PARAMETER(EstimateNumberOfFeatures);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::dataCheck()
{
  setErrorCondition(0);

  // Create the output Data Container
  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
  m->setGeometry(image);

  // Sanity Check the Dimensions and Resolution
  INIT_SYNTH_VOLUME_CHECK(Dimensions.x, -5000);
  INIT_SYNTH_VOLUME_CHECK(Dimensions.y, -5001);
  INIT_SYNTH_VOLUME_CHECK(Dimensions.z, -5002);
  INIT_SYNTH_VOLUME_CHECK(Resolution.x, -5003);
  INIT_SYNTH_VOLUME_CHECK(Resolution.y, -5004);
  INIT_SYNTH_VOLUME_CHECK(Resolution.z, -5005);
  // Set teh Dimensions, Resolution and Origin of the output data container
  m->getGeometryAs<ImageGeom>()->setDimensions(m_Dimensions.x, m_Dimensions.y, m_Dimensions.z);
  m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->getGeometryAs<ImageGeom>()->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

  // Create our output Cell and Ensemble Attribute Matrix objects
  QVector<size_t> tDims(3, 0);
  tDims[0] = m_Dimensions.x;
  tDims[1] = m_Dimensions.y;
  tDims[2] = m_Dimensions.z;
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0 && cellAttrMat.get() == NULL) { return; }

  QVector<size_t> compDims(1, 1); // This states that we are looking for an array with a single component
  UInt32ArrayType::Pointer phaseType = getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType, AbstractFilter>(NULL, getInputPhaseTypesArrayPath(), compDims);
  if(getErrorCondition() < 0 && phaseType.get() == NULL) { return; }

  QVector<size_t> statsDims(1, 1);
  StatsDataArray::Pointer statsPtr = getDataContainerArray()->getPrereqArrayFromPath<StatsDataArray, AbstractFilter>(this, getInputStatsArrayPath(), statsDims);
  if(getErrorCondition() < 0 && statsPtr.get() == NULL) { return; }

  if(m_EstimateNumberOfFeatures)
  {
    m_EstimatedPrimaryFeatures = estimateNumFeatures(m_Dimensions, m_Resolution);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::preflight()
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
void InitializeSyntheticVolume::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());

  // Resize the Cell AttributeMatrix to have the correct Tuple Dimensions.
  QVector<size_t> tDims(3, 0);
  tDims[0] = m->getGeometryAs<ImageGeom>()->getXPoints();
  tDims[1] = m->getGeometryAs<ImageGeom>()->getYPoints();
  tDims[2] = m->getGeometryAs<ImageGeom>()->getZPoints();
  cellAttrMat->resizeAttributeArrays(tDims);

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int InitializeSyntheticVolume::estimateNumFeatures(IntVec3_t dims, FloatVec3_t res)
{
  float totalvol;
  int phase;

  totalvol = (dims.x * res.x) * (dims.y * res.y) * (dims.z * res.z);
  if (totalvol == 0.0)
  {
    return -1;
  }

  DataContainerArray::Pointer dca = getDataContainerArray();

  // Get the PhaseTypes - Remember there is a Dummy PhaseType in the first slot of the array
  QVector<size_t> compDims(1, 1); // This states that we are looking for an array with a single component
  UInt32ArrayType::Pointer phaseType = dca->getPrereqArrayFromPath<UInt32ArrayType, AbstractFilter>(NULL, getInputPhaseTypesArrayPath(), compDims);
  if(phaseType.get() == NULL)
  {
    QString ss = QObject::tr("PhaseTypes Array Not Found when estimating the number of grains");
    setErrorCondition(-80000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return 0;
  }


  QVector<size_t> statsDims(1, 1);
  StatsDataArray::Pointer statsPtr = dca->getPrereqArrayFromPath<StatsDataArray, AbstractFilter>(this, getInputStatsArrayPath(), statsDims);
  if(statsPtr.get() == NULL)
  {
    QString ss = QObject::tr("Stats Array Not Found when estimating the number of grains");
    notifyErrorMessage(getHumanLabel(), ss, -80001);
    return 0;
  }


  if( !phaseType->isAllocated())
  {
    if(getInputStatsFile().isEmpty())
    {
      QString ss = QObject::tr("PhaseTypes Array has not been allocated and the Input Stats file is Empty.");
      setErrorCondition(-1000);
      notifyWarningMessage(getHumanLabel(), ss, getErrorCondition());
      return -1;
    }
    QFileInfo fi(getInputStatsFile());
    if(fi.exists() == false)
    {
      QString ss = QObject::tr("PhaseTypes Array has not been allocated and the Input Stats file does not exist at '%1'").arg(fi.absoluteFilePath());
      setErrorCondition(-1001);
      notifyWarningMessage(getHumanLabel(), ss, getErrorCondition());
      return -1;
    }


    hid_t fileId = -1;
    herr_t err = 0;
    // open the file
    fileId = H5Utilities::openFile(getInputStatsFile().toLatin1().constData(), true);
    // This will make sure if we return early from this method that the HDF5 File is properly closed.
    HDF5ScopedFileSentinel scopedFileSentinel(&fileId, true);

    DataArrayPath dap = getInputPhaseTypesArrayPath();
    // Generate the path to the AttributeMatrix
    QString hPath = DREAM3D::StringConstants::DataContainerGroupName + "/" + dap.getDataContainerName() + "/" + dap.getAttributeMatrixName();
    // Open the AttributeMatrix Group
    hid_t amGid = H5Gopen(fileId, hPath.toLatin1().data(), H5P_DEFAULT );
    scopedFileSentinel.addGroupId(&amGid);
    err = phaseType->readH5Data(amGid);
    if(err < 0)
    {
      QString ss = QObject::tr("Error reading phase type data.");
      setErrorCondition(-1003);
      notifyWarningMessage(getHumanLabel(), ss, getErrorCondition());
      return -1;
    }
    if( !phaseType->isAllocated())
    {
      QString ss = QObject::tr("PhaseTypes Array was not allocated due to an error reading the data from the stats file %1").arg(fi.absoluteFilePath());
      setErrorCondition(-1002);
      notifyWarningMessage(getHumanLabel(), ss, getErrorCondition());
      return -1;
    }
  }

  // Create a Reference Variable so we can use the [] syntax
  StatsDataArray& statsDataArray = *(statsPtr.get());

  QVector<int> primaryphases;
  QVector<double> primaryphasefractions;
  double totalprimaryfractions = 0.0;
  //StatsData::Pointer statsData = StatsData::NullPointer();
  // find which phases are primary phases

  for (size_t i = 1; i < phaseType->getNumberOfTuples(); ++i)
  {
    if(phaseType->getValue(i) == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      primaryphases.push_back(i);
      primaryphasefractions.push_back(pp->getPhaseFraction());
      totalprimaryfractions = totalprimaryfractions + pp->getPhaseFraction();
    }
  }

  // scale the primary phase fractions to total to 1
  for (qint32 i = 0; i < primaryphasefractions.size(); i++)
  {
    primaryphasefractions[i] = primaryphasefractions[i] / totalprimaryfractions;
  }

  DREAM3D_RANDOMNG_NEW()
  // generate the Features
  int gid = 1;

  float currentvol = 0.0f;
  float vol = 0.0f;
  float diam = 0.0f;
  int volgood = 0;
  for (qint32 j = 0; j < primaryphases.size(); ++j)
  {
    float curphasetotalvol = totalvol * primaryphasefractions[j];
    while (currentvol < (curphasetotalvol))
    {
      volgood = 0;
      phase = primaryphases[j];
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
      while (volgood == 0)
      {
        volgood = 1;
        // u = rg.genrand_res53();
        if(pp->getFeatureSize_DistType() == DREAM3D::DistributionType::LogNormal)
        {
          float avgdiam = pp->getFeatureSizeDistribution().at(0)->getValue(0);
          float sddiam = pp->getFeatureSizeDistribution().at(1)->getValue(0);
          diam = rg.genrand_norm(avgdiam, sddiam);
          diam = exp(diam);
          if(diam >= pp->getMaxFeatureDiameter()) { volgood = 0; }
          if(diam < pp->getMinFeatureDiameter()) { volgood = 0; }
          vol = (4.0f / 3.0f) * (M_PI) * ((diam * 0.5f) * (diam * 0.5f) * (diam * 0.5f));
        }
      }
      currentvol = currentvol + vol;
      gid++;
    }
  }
  return gid;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int InitializeSyntheticVolume::getEstimatedPrimaryFeatures()
{
  return m_EstimatedPrimaryFeatures;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer InitializeSyntheticVolume::newFilterInstance(bool copyFilterParameters)
{
  InitializeSyntheticVolume::Pointer filter = InitializeSyntheticVolume::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeSyntheticVolume::getCompiledLibraryName()
{ return SyntheticBuilding::SyntheticBuildingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeSyntheticVolume::getGroupName()
{return DREAM3D::FilterGroups::SyntheticBuildingFilters;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeSyntheticVolume::getSubGroupName()
{ return DREAM3D::FilterSubGroups::PackingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString InitializeSyntheticVolume::getHumanLabel()
{return "Initialize Synthetic Volume";}

