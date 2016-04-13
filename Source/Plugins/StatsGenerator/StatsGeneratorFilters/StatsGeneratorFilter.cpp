/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Softwae, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "StatsGeneratorFilter.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/TransformationStatsData.h"

#include "StatsGenerator/StatsGeneratorConstants.h"
#include "StatsGenerator/StatsGeneratorVersion.h"
#include "StatsGenerator/FilterParameters/StatsGeneratorFilterParameter.h"
#include "StatsGenerator/StatsGeneratorFilters/StatsGeneratorUtilities.h"

#include "EbsdLib/EbsdConstants.h"

// Include the MOC generated file for this class
#include "moc_StatsGeneratorFilter.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGeneratorFilter::StatsGeneratorFilter() :
  AbstractFilter()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGeneratorFilter::~StatsGeneratorFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorFilter::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(StatsGeneratorFilterParameter::New("StatsGenerator", "StatsGenerator", "", FilterParameter::Parameter));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);

  JsonFilterParametersReader* jsonReader = dynamic_cast<JsonFilterParametersReader*>(reader);
  QJsonObject& jsonRoot = jsonReader->getCurrentGroupObject();

  // Clear the array as we are going to populate the entire array with new objects
  if(nullptr != m_StatsDataArray.get() ) {
    m_StatsDataArray = StatsDataArray::NullPointer();
  }

  QString filterName = jsonRoot["Filter_Name"].toString();
  m_StatsDataArray = StatsDataArray::CreateArray(0, "THIS SHOULD BE RESET");
  m_StatsDataArray->readFromJson(jsonRoot);
  size_t numTuples = m_StatsDataArray->getNumberOfTuples();

  readArray(jsonRoot, numTuples);

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorFilter::readArray(const QJsonObject &jsonRoot, size_t numTuples)
{
  QJsonObject statsObject = jsonRoot["StatsDataArray"].toObject();

  int phaseCount = statsObject["Phase Count"].toInt();

  m_CrystalStructures = UInt32ArrayType::CreateArray(numTuples, SIMPL::EnsembleData::CrystalStructures, true);
  m_CrystalStructures->initializeWithValue(Ebsd::CrystalStructure::UnknownCrystalStructure);

  m_PhaseTypes = UInt32ArrayType::CreateArray(numTuples, SIMPL::EnsembleData::PhaseTypes, true);
  m_PhaseTypes->initializeWithValue(SIMPL::PhaseType::UnknownPhaseType);

  // Start from index 1. Index 0 is always junk.
  for(int index = 1; index < phaseCount; index++)
  {
    QString phaseAsString = QString::number(index);
    QJsonObject phaseObject = statsObject[phaseAsString].toObject();
    QString statsType = phaseObject[SIMPL::StringConstants::PhaseType].toString();
    int crystalSymmetry = phaseObject[SIMPL::EnsembleData::CrystalSymmetry].toInt(999);
    m_CrystalStructures->setValue(index, crystalSymmetry);

    unsigned int pt = m_StatsDataArray->getStatsData(index)->getPhaseType();
    m_PhaseTypes->setValue(index, pt);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGeneratorFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);

  JsonFilterParametersWriter* jsonWriter = dynamic_cast<JsonFilterParametersWriter*>(writer);
  if(nullptr == jsonWriter)
  {
    writer->closeFilterGroup();
    return ++index; // we want to return the next index that was just written to
  }
  QJsonObject& jsonRoot = jsonWriter->getCurrentGroupObject();


  m_StatsDataArray->writeToJson(jsonRoot, m_CrystalStructures);

  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorFilter::dataCheck()
{
  setErrorCondition(0);

  if (nullptr != m_StatsDataArray.get())
  {
    DataContainer::Pointer m = DataContainer::New(SIMPL::Defaults::StatsGenerator);
    getDataContainerArray()->addDataContainer(m);

    QVector<size_t> tDims(1, m_StatsDataArray->getNumberOfTuples());
    AttributeMatrix::Pointer cellEnsembleAttrMat = AttributeMatrix::New(tDims, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::AttributeMatrixType::CellEnsemble);
    m->addAttributeMatrix(SIMPL::Defaults::CellEnsembleAttributeMatrixName, cellEnsembleAttrMat);

    cellEnsembleAttrMat->addAttributeArray(SIMPL::EnsembleData::Statistics, m_StatsDataArray);


    if(nullptr != m_CrystalStructures.get()) {
      cellEnsembleAttrMat->addAttributeArray(SIMPL::EnsembleData::CrystalStructures, m_CrystalStructures);
    }
    if(nullptr != m_PhaseTypes.get()) {
      cellEnsembleAttrMat->addAttributeArray(SIMPL::EnsembleData::PhaseTypes, m_PhaseTypes);
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorFilter::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorFilter::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (getCancel() == true) { return; }


  // Create the ODF and MDF Arrays, RDF (if needed) & Axis ODF for EACH PHASE
  size_t count = m_StatsDataArray->getNumberOfTuples();
  for(size_t c = 0; c < count; c++)
  {
    StatsData::Pointer statsData = m_StatsDataArray->getStatsData(c);
    if(nullptr != statsData.get())
    {
      u_int32_t phaseType = m_PhaseTypes->getValue(c);
      u_int32_t crystalStruct = m_CrystalStructures->getValue(c);

      // ODF Data ************************************************************************
      VectorOfFloatArray odfWeights;
      VectorOfFloatArray mdfWeights;
      if(phaseType == SIMPL::PhaseType::PrimaryPhase)
      {
        PrimaryStatsData::Pointer pp = std::dynamic_pointer_cast<PrimaryStatsData>(statsData);
        odfWeights = pp->getODF_Weights();
        mdfWeights = pp->getMDF_Weights();
      }
      else if(phaseType == SIMPL::PhaseType::PrecipitatePhase)
      {
        PrecipitateStatsData::Pointer pp = std::dynamic_pointer_cast<PrecipitateStatsData>(statsData);
        odfWeights = pp->getODF_Weights();
        mdfWeights = pp->getMDF_Weights();
      }
      else if(phaseType == SIMPL::PhaseType::TransformationPhase)
      {
        TransformationStatsData::Pointer tp = std::dynamic_pointer_cast<TransformationStatsData>(statsData);
        odfWeights = tp->getODF_Weights();
        mdfWeights = tp->getMDF_Weights();
      }

      QVector<float> e1s;
      QVector<float> e2s;
      QVector<float> e3s;
      QVector<float> weights;
      QVector<float> sigmas;

      if(odfWeights.size() == 5)
      {
        size_t odfCount = odfWeights[0]->getNumberOfTuples();
        for(size_t i = 0; i < odfCount; i++)
        {
          e1s.push_back(odfWeights[0]->getValue(i));
          e2s.push_back(odfWeights[1]->getValue(i));
          e3s.push_back(odfWeights[2]->getValue(i));
          weights.push_back(odfWeights[3]->getValue(i));
          sigmas.push_back(odfWeights[4]->getValue(i));
        }
      }
      StatsGeneratorUtilities::GenerateODFBinData(statsData.get(), phaseType, crystalStruct, e1s, e2s, e3s, weights, sigmas);


      // MDF Data ************************************************************************
      QVector<float> mdf_angles;
      QVector<float> mdf_axes;
      QVector<float> mdf_weights;

      if(mdfWeights.size() == 3)
      {
        size_t mdfCount = mdfWeights[0]->getNumberOfTuples();
        for(size_t i = 0; i < mdfCount; i++)
        {
          mdf_angles.push_back(mdfWeights[0]->getValue(i));
          mdf_axes.push_back(mdfWeights[1]->getComponent(i, 0));
          mdf_axes.push_back(mdfWeights[1]->getComponent(i, 1));
          mdf_axes.push_back(mdfWeights[1]->getComponent(i, 2));
          mdf_weights.push_back(mdfWeights[2]->getValue(i));
        }


      }

      QVector<float> odf = StatsGeneratorUtilities::GenerateODFData(crystalStruct, e1s, e2s, e3s, weights, sigmas);

      StatsGeneratorUtilities::GenerateMisorientationBinData(statsData.get(), phaseType, crystalStruct, odf, mdf_angles, mdf_axes, mdf_weights);

      // Primary
      //m_MDFWidget->getMisorientationData(statsData, phaseType);
      // Precipitate

      // Transformation



      // Axis ODF ************************************************************************
      //err = m_AxisODFWidget->getOrientationData(primaryStatsData, SIMPL::PhaseType::PrimaryPhase);

      // RDF Data ************************************************************************
      if(phaseType == SIMPL::PhaseType::PrecipitatePhase)
      {

      }
    }

  }
  // Loop Each Phase
  // StatsGenODFWidget->getOrientationData()
  // Factor out common code so both the widget and this code can call it



  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer StatsGeneratorFilter::newFilterInstance(bool copyFilterParameters)
{
  StatsGeneratorFilter::Pointer filter = StatsGeneratorFilter::New();
  if(true == copyFilterParameters)
  {
    //copyFilterParameterInstanceVariables(filter.get());
    filter->setCrystalStructures(std::dynamic_pointer_cast<UInt32ArrayType>(m_CrystalStructures->deepCopy(getInPreflight())));
    filter->setPhaseTypes(std::dynamic_pointer_cast<UInt32ArrayType>(m_PhaseTypes->deepCopy(getInPreflight())));

    filter->setStatsDataArray(std::dynamic_pointer_cast<StatsDataArray>(m_StatsDataArray->deepCopy(getInPreflight())));

  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString StatsGeneratorFilter::getCompiledLibraryName()
{ return StatsGeneratorConstants::StatsGeneratorBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString StatsGeneratorFilter::getBrandingString()
{
  return "StatsGenerator";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString StatsGeneratorFilter::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  StatsGenerator::Version::Major() << "." << StatsGenerator::Version::Minor() << "." << StatsGenerator::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString StatsGeneratorFilter::getGroupName()
{ return SIMPL::FilterGroups::SyntheticBuildingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString StatsGeneratorFilter::getSubGroupName()
{ return "StatsGenerator"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString StatsGeneratorFilter::getHumanLabel()
{ return "StatsGenerator Filter"; }

