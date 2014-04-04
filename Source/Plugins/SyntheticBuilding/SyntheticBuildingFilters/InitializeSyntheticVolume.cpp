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

#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/HDF5ScopedFileSentinel.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/IOFilters/DataContainerReader.h"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"
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
  m_InputStatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::Statistics),
  m_InputPhaseTypesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::PhaseTypes),
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

  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("Input Statistics", "InputStatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Input Phase Types", "InputPhaseTypesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
  parameters.push_back(FilterParameter::New("New DataContainer Name", "DataContainerName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  parameters.push_back(FilterParameter::New("New Cell Attribute Matrix Name", "CellAttributeMatrixName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  parameters.push_back(FilterParameter::New("Dimensions", "Dimensions", FilterParameterWidgetType::IntVec3Widget, "IntVec3_t", false, "Voxels"));
  parameters.push_back(FilterParameter::New("Resolution", "Resolution", FilterParameterWidgetType::FloatVec3Widget, "FloatVec3_t", false, "Microns"));
  parameters.push_back(FilterParameter::New("Origin", "Origin", FilterParameterWidgetType::FloatVec3Widget, "FloatVec3_t", false, "Microns"));
  parameters.push_back(FilterParameter::New("Estimated Primary Features", "EstimatedPrimaryFeatures", FilterParameterWidgetType::PreflightUpdatedValueWidget, "QString", false, ""));

  setFilterParameters(parameters);
}
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
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int InitializeSyntheticVolume::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("DataContainerName", getDataContainerName() );
  writer->writeValue("CellAttributeMatrixName", getCellAttributeMatrixName() );
  writer->writeValue("Dimensions", getDimensions() );
  writer->writeValue("Resolution", getResolution() );
  writer->writeValue("Origin", getOrigin() );
  writer->writeValue("InputStatsArrayPath", getInputStatsArrayPath() );
  writer->writeValue("InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath() );
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
  VolumeDataContainer* m = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  // Sanity Check the Dimensions and Resolution
  INIT_SYNTH_VOLUME_CHECK(Dimensions.x, -5000);
  INIT_SYNTH_VOLUME_CHECK(Dimensions.y, -5001);
  INIT_SYNTH_VOLUME_CHECK(Dimensions.z, -5002);
  INIT_SYNTH_VOLUME_CHECK(Resolution.x, -5003);
  INIT_SYNTH_VOLUME_CHECK(Resolution.y, -5004);
  INIT_SYNTH_VOLUME_CHECK(Resolution.z, -5005);
  // Set teh Dimensions, Resolution and Origin of the output data container
  m->setDimensions(m_Dimensions.x, m_Dimensions.y, m_Dimensions.z);
  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

  // Create our output Cell and Ensemble Attribute Matrix objects
  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  m_EstimatedPrimaryFeatures = estimateNumFeatures(m_Dimensions, m_Resolution);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InitializeSyntheticVolume::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
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

  // Resize the Cell AttributeMatrix to have the correct Tuple Dimensions.
  QVector<size_t> tDims(3, 0);
  tDims[0] = m->getXPoints();
  tDims[1] = m->getYPoints();
  tDims[2] = m->getZPoints();
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
    notifyErrorMessage(getHumanLabel(), ss, -80000);
    return 0;
  }

  QVector<size_t> statsDims(1, 1);
  StatsDataArray::Pointer statsPtr = dca->getPrereqArrayFromPath<StatsDataArray, AbstractFilter>(this, getInputStatsArrayPath(), statsDims);
  //  m_StatsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->getAttributeArray(DREAM3D::EnsembleData::Statistics).get());
  if(statsPtr.get() == NULL)
  {
    QString ss = QObject::tr("Stats Array Not Found when estimating the number of grains");
    notifyErrorMessage(getHumanLabel(), ss, -80001);
    return 0;
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
  for (size_t i = 0; i < primaryphasefractions.size(); i++)
  {
    primaryphasefractions[i] = primaryphasefractions[i] / totalprimaryfractions;
  }

  DREAM3D_RANDOMNG_NEW()
      // generate the Features
      int gid = 1;

  float currentvol = 0.0;
  float vol;
  float diam;
  int volgood = 0;
  for (size_t j = 0; j < primaryphases.size(); ++j)
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
    filter->setDataContainerName(getDataContainerName() );
    filter->setCellAttributeMatrixName(getCellAttributeMatrixName() );
    filter->setDimensions( getDimensions() );
    filter->setOrigin( getOrigin() );
    filter->setResolution( getResolution() );
    filter->setInputStatsArrayPath(getInputStatsArrayPath() );
    filter->setInputPhaseTypesArrayPath( getInputPhaseTypesArrayPath() );
  }
  return filter;
}
