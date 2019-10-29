/* ============================================================================
 * Copyright (c) 2016 BlueQuartz Software, LLC
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

#include <memory>

#include "StatsGeneratorFilter.h"

#include <QtCore/QJsonDocument>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/FilterParameters/H5FilterParametersWriter.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/TransformationStatsData.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "SyntheticBuilding/FilterParameters/StatsGeneratorFilterParameter.h"
#include "SyntheticBuilding/SyntheticBuildingConstants.h"
#include "SyntheticBuilding/SyntheticBuildingFilters/StatsGeneratorUtilities.h"
#include "SyntheticBuilding/SyntheticBuildingVersion.h"

#include "EbsdLib/EbsdConstants.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,

  DataContainerID = 1,

  DataArrayID30 = 30,
  DataArrayID31 = 31,
  DataArrayID32 = 32,
  DataArrayID33 = 33,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGeneratorFilter::StatsGeneratorFilter()
: m_StatsGeneratorDataContainerName(SIMPL::Defaults::StatsGenerator)
, m_CellEnsembleAttributeMatrixName(SIMPL::Defaults::CellEnsembleAttributeMatrixName)
, m_StatsDataArrayName(SIMPL::EnsembleData::Statistics)
, m_CrystalStructuresArrayName(SIMPL::EnsembleData::CrystalStructures)
, m_PhaseTypesArrayName(SIMPL::EnsembleData::PhaseTypes)
, m_PhaseNamesArrayName(SIMPL::EnsembleData::PhaseName)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGeneratorFilter::~StatsGeneratorFilter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorFilter::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(StatsGeneratorFilterParameter::New("StatsGenerator", "StatsGenerator", "", FilterParameter::Parameter));

  parameters.push_back(SeparatorFilterParameter::New("Created Data Container", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Statistics Data Container Name", StatsGeneratorDataContainerName, FilterParameter::CreatedArray, StatsGeneratorFilter));

  parameters.push_back(SeparatorFilterParameter::New("Created Ensemble AttributeMatrix", FilterParameter::CreatedArray));

  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Cell Ensemble Attribute Matrix Name", CellEnsembleAttributeMatrixName, StatsGeneratorDataContainerName, FilterParameter::CreatedArray, StatsGeneratorFilter));

  parameters.push_back(SeparatorFilterParameter::New("Created Ensemble Arrays", FilterParameter::CreatedArray));

  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Statistics Array Name", StatsDataArrayName, StatsGeneratorDataContainerName, CellEnsembleAttributeMatrixName, FilterParameter::CreatedArray, StatsGeneratorFilter));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Crystal Structures Array Name", CrystalStructuresArrayName, StatsGeneratorDataContainerName, CellEnsembleAttributeMatrixName, FilterParameter::CreatedArray, StatsGeneratorFilter));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Phase Types Array Name", PhaseTypesArrayName, StatsGeneratorDataContainerName, CellEnsembleAttributeMatrixName, FilterParameter::CreatedArray, StatsGeneratorFilter));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Phase Names Array Name", PhaseNamesArrayName, StatsGeneratorDataContainerName, CellEnsembleAttributeMatrixName, FilterParameter::CreatedArray, StatsGeneratorFilter));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);

  // Clear the array as we are going to populate the entire array with new objects
  if(nullptr != m_StatsDataArray)
  {
    m_StatsDataArray = StatsDataArray::NullPointer();
  }

  m_StatsDataArray = StatsDataArray::CreateArray(0, "THIS SHOULD BE RESET", true);

  if(dynamic_cast<H5FilterParametersReader*>(reader) != nullptr)
  {
    QString jsonString = reader->readString("StatsDataArray", "");
    QJsonDocument jDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    m_StatsDataArray->readFromJson(jDoc.object());
    size_t numTuples = m_StatsDataArray->getNumberOfTuples();
    readArray(jDoc.object(), numTuples);
  }
  else if(dynamic_cast<JsonFilterParametersReader*>(reader) != nullptr)
  {
    JsonFilterParametersReader* jsonReader = dynamic_cast<JsonFilterParametersReader*>(reader);
    QJsonObject& jsonRoot = jsonReader->getCurrentGroupObject();
    m_StatsDataArray->readFromJson(jsonRoot);
    size_t numTuples = m_StatsDataArray->getNumberOfTuples();
    readArray(jsonRoot, numTuples);
  }

  setStatsGeneratorDataContainerName(reader->readDataArrayPath("StatsGeneratorDataContainerName", getStatsGeneratorDataContainerName()));
  setCellEnsembleAttributeMatrixName(reader->readString("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName()));
  setStatsDataArrayName(reader->readString("StatsDataArrayName", getStatsDataArrayName()));
  setCrystalStructuresArrayName(reader->readString("CrystalStructuresArrayName", getCrystalStructuresArrayName()));
  setPhaseTypesArrayName(reader->readString("PhaseTypesArrayName", getPhaseTypesArrayName()));
  setPhaseNamesArrayName(reader->readString("PhaseNamesArrayName", getPhaseNamesArrayName()));

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorFilter::readFilterParameters(QJsonObject& obj)
{

  // Clear the array as we are going to populate the entire array with new objects
  if(nullptr != m_StatsDataArray)
  {
    m_StatsDataArray = StatsDataArray::NullPointer();
  }

  m_StatsDataArray = StatsDataArray::CreateArray(0, "THIS SHOULD BE RESET", true);

  m_StatsDataArray->readFromJson(obj);
  size_t numTuples = m_StatsDataArray->getNumberOfTuples();
  readArray(obj, numTuples);

  FilterParameterVectorType filterParameters = getFilterParameters();
  for(int i = 0; i < filterParameters.size(); i++)
  {
    FilterParameter::Pointer fp = filterParameters[i];
    fp->readJson(obj);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorFilter::writeFilterParameters(QJsonObject& obj) const
{
  FilterParameterVectorType filterParameters = getFilterParameters();
  for(int i = 0; i < filterParameters.size(); i++)
  {
    FilterParameter::Pointer fp = filterParameters[i];
    fp->writeJson(obj);
  }

  if(nullptr != m_StatsDataArray.get())
  {
    m_StatsDataArray->writeToJson(obj, m_CrystalStructures);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorFilter::readArray(const QJsonObject& jsonRoot, size_t numTuples)
{
  QJsonObject statsObject = jsonRoot["StatsDataArray"].toObject();

  int phaseCount = statsObject["Phase Count"].toInt();

  m_CrystalStructures = UInt32ArrayType::CreateArray(numTuples, SIMPL::EnsembleData::CrystalStructures, true);
  m_CrystalStructures->initializeWithValue(Ebsd::CrystalStructure::UnknownCrystalStructure);

  m_PhaseTypes = UInt32ArrayType::CreateArray(numTuples, SIMPL::EnsembleData::PhaseTypes, true);
  m_PhaseTypes->initializeWithValue(static_cast<PhaseType::EnumType>(PhaseType::Type::Unknown));

  m_PhaseNames = StringDataArray::CreateArray(numTuples, SIMPL::EnsembleData::PhaseName, true);
  m_PhaseNames->initializeWithValue(QString("Unknown Phase"));

  // Start from index 1. Index 0 is always junk.
  for(int index = 1; index < phaseCount; index++)
  {
    QString phaseAsString = QString::number(index);
    QJsonObject phaseObject = statsObject[phaseAsString].toObject();
    QString statsType = phaseObject[SIMPL::StringConstants::PhaseType].toString();
    int crystalSymmetry = phaseObject[SIMPL::EnsembleData::CrystalSymmetry].toInt(999);
    m_CrystalStructures->setValue(index, crystalSymmetry);

    PhaseType::Type pt = m_StatsDataArray->getStatsData(index)->getPhaseType();
    m_PhaseTypes->setValue(index, static_cast<PhaseType::EnumType>(pt));

    QString phaseName = phaseObject[SIMPL::StringConstants::Name].toString();
    m_PhaseNames->setValue(index, phaseName);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorFilter::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorFilter::dataCheck()
{
  if(nullptr != m_StatsDataArray)
  {
    getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getStatsGeneratorDataContainerName(), DataContainerID);

    if(getErrorCode() < 0)
    {
      return;
    }

    DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getStatsGeneratorDataContainerName());

    std::vector<size_t> tDims(1, m_StatsDataArray->getNumberOfTuples());
    AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix(this, getCellEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble, AttributeMatrixID21);

    m_StatsDataArray->setName(getStatsDataArrayName());
    cellEnsembleAttrMat->insertOrAssign(m_StatsDataArray);

    if(nullptr != m_CrystalStructures)
    {
      m_CrystalStructures->setName(getCrystalStructuresArrayName());
      cellEnsembleAttrMat->insertOrAssign(m_CrystalStructures);
    }

    if(nullptr != m_PhaseTypes)
    {
      m_PhaseTypes->setName(getPhaseTypesArrayName());
      cellEnsembleAttrMat->insertOrAssign(m_PhaseTypes);
    }

    if(nullptr != m_PhaseNames)
    {
      m_PhaseNames->setName(getPhaseNamesArrayName());
      cellEnsembleAttrMat->insertOrAssign(m_PhaseNames);
    }
  }
  else
  {
    QString ss = QObject::tr("Unable to retrieve a valid pointer for statistics data");
    setErrorCondition(-1, ss);
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorFilter::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorFilter::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  // Create the ODF, MDF, Axis ODF, bin numbers and RDF (precipitate ONLY) for EACH PHASE
  // These arrays are re-created here because they MAY be junk having come from the GUI,
  // since we avoid re-computing these during preflights and the user may not have
  // decided to click the update buttons
  size_t count = m_StatsDataArray->getNumberOfTuples();
  for(size_t c = 0; c < count; c++)
  {
    if(getCancel())
    {
      return;
    }
    StatsData::Pointer statsData = m_StatsDataArray->getStatsData(c);
    if(nullptr != statsData)
    {
      // Pull the ODF, MDF and AxisODF weights from the StatsDataArray
      PhaseType::Type phaseType = static_cast<PhaseType::Type>(m_PhaseTypes->getValue(c));
      uint32_t crystalStruct = m_CrystalStructures->getValue(c);
      VectorOfFloatArray odfWeights;
      VectorOfFloatArray mdfWeights;
      VectorOfFloatArray aodfWeights;
      if(phaseType == PhaseType::Type::Primary)
      {
        PrimaryStatsData::Pointer pp = std::dynamic_pointer_cast<PrimaryStatsData>(statsData);
        pp->generateBinNumbers();
        odfWeights = pp->getODF_Weights();
        mdfWeights = pp->getMDF_Weights();
        aodfWeights = pp->getAxisODF_Weights();
      }
      else if(phaseType == PhaseType::Type::Precipitate)
      {
        PrecipitateStatsData::Pointer pp = std::dynamic_pointer_cast<PrecipitateStatsData>(statsData);
        pp->generateBinNumbers();
        odfWeights = pp->getODF_Weights();
        mdfWeights = pp->getMDF_Weights();
        aodfWeights = pp->getAxisODF_Weights();

        // RDF Data ************************************************************************
        RdfData::Pointer rdf = pp->getRadialDistFunction();
        std::array<float, 3> boxSize = rdf->getBoxSize();
        std::array<float, 3> boxRes = rdf->getBoxResolution();

        std::vector<float> freqs = RadialDistributionFunction::GenerateRandomDistribution(rdf->getMinDistance(), rdf->getMaxDistance(), rdf->getNumberOfBins(), boxSize, boxRes);
        RdfData::Pointer cleanRDF = RdfData::New();
        cleanRDF->setFrequencies(freqs);
        cleanRDF->setMinDistance(rdf->getMinDistance());
        cleanRDF->setMaxDistance(rdf->getMaxDistance());
        cleanRDF->setDistributionType(rdf->getDistributionType());
        cleanRDF->setBoxSize(boxSize);
        cleanRDF->setBoxResolution(boxRes);
        pp->setRadialDistFunction(cleanRDF);
      }
      else if(phaseType == PhaseType::Type::Transformation)
      {
        TransformationStatsData::Pointer tp = std::dynamic_pointer_cast<TransformationStatsData>(statsData);
        tp->generateBinNumbers();
        odfWeights = tp->getODF_Weights();
        mdfWeights = tp->getMDF_Weights();
        aodfWeights = tp->getAxisODF_Weights();
      }

      // ODF Data ************************************************************************
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
          weights.push_back(odfWeights[4]->getValue(i));
          sigmas.push_back(odfWeights[3]->getValue(i));
        }
      }

      // Compute the binned ODF and set it into the StatsDataArray
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

      // Compute the ODF representation that will be used to determine the MDF
      QVector<float> odf = StatsGeneratorUtilities::GenerateODFData(crystalStruct, e1s, e2s, e3s, weights, sigmas);

      // Compute the binned MDF and set it into the StatsDataArray
      StatsGeneratorUtilities::GenerateMisorientationBinData(statsData.get(), phaseType, crystalStruct, odf, mdf_angles, mdf_axes, mdf_weights);

      // AxisODF Data ********************************************************************
      QVector<float> axis_e1s;
      QVector<float> axis_e2s;
      QVector<float> axis_e3s;
      QVector<float> axis_weights;
      QVector<float> axis_sigmas;

      if(aodfWeights.size() == 5)
      {
        size_t aodfCount = aodfWeights[0]->getNumberOfTuples();
        for(size_t i = 0; i < aodfCount; i++)
        {
          axis_e1s.push_back(aodfWeights[0]->getValue(i));
          axis_e2s.push_back(aodfWeights[1]->getValue(i));
          axis_e3s.push_back(aodfWeights[2]->getValue(i));
          axis_weights.push_back(aodfWeights[4]->getValue(i));
          axis_sigmas.push_back(aodfWeights[3]->getValue(i));
        }
      }

      // Compute the binned axis ODF and set it into the StatsDataArray
      StatsGeneratorUtilities::GenerateAxisODFBinData(statsData.get(), phaseType, axis_e1s, axis_e2s, axis_e3s, axis_weights, axis_sigmas);
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer StatsGeneratorFilter::newFilterInstance(bool copyFilterParameters) const
{
  StatsGeneratorFilter::Pointer filter = StatsGeneratorFilter::New();
  if(copyFilterParameters)
  {
    filter->setStatsGeneratorDataContainerName(getStatsGeneratorDataContainerName());
    filter->setCellEnsembleAttributeMatrixName(getCellEnsembleAttributeMatrixName());
    filter->setStatsDataArrayName(getStatsDataArrayName());
    filter->setCrystalStructuresArrayName(getCrystalStructuresArrayName());
    filter->setPhaseTypesArrayName(getPhaseTypesArrayName());
    if(m_CrystalStructures)
    {
      filter->setCrystalStructures(std::dynamic_pointer_cast<UInt32ArrayType>(m_CrystalStructures->deepCopy(getInPreflight())));
    }
    if(m_PhaseTypes)
    {
      filter->setPhaseTypes(std::dynamic_pointer_cast<UInt32ArrayType>(m_PhaseTypes->deepCopy(getInPreflight())));
    }
    if(m_PhaseNames)
    {
      filter->setPhaseNames(std::dynamic_pointer_cast<StringDataArray>(m_PhaseNames->deepCopy(getInPreflight())));
    }
    if(m_StatsDataArray)
    {
      filter->setStatsDataArray(std::dynamic_pointer_cast<StatsDataArray>(m_StatsDataArray->deepCopy(getInPreflight())));
    }
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsGeneratorFilter::getCompiledLibraryName() const
{
  return SyntheticBuildingConstants::SyntheticBuildingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsGeneratorFilter::getBrandingString() const
{
  return "StatsGenerator";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsGeneratorFilter::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SyntheticBuilding::Version::Major() << "." << SyntheticBuilding::Version::Minor() << "." << SyntheticBuilding::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsGeneratorFilter::getGroupName() const
{
  return SIMPL::FilterGroups::SyntheticBuildingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid StatsGeneratorFilter::getUuid() const
{
  return QUuid("{f642e217-4722-5dd8-9df9-cee71e7b26ba}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsGeneratorFilter::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsGeneratorFilter::getHumanLabel() const
{
  return "StatsGenerator";
}

// -----------------------------------------------------------------------------
StatsGeneratorFilter::Pointer StatsGeneratorFilter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<StatsGeneratorFilter> StatsGeneratorFilter::New()
{
  struct make_shared_enabler : public StatsGeneratorFilter
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString StatsGeneratorFilter::getNameOfClass() const
{
  return QString("StatsGeneratorFilter");
}

// -----------------------------------------------------------------------------
QString StatsGeneratorFilter::ClassName()
{
  return QString("StatsGeneratorFilter");
}

// -----------------------------------------------------------------------------
void StatsGeneratorFilter::setStatsGeneratorDataContainerName(const DataArrayPath& value)
{
  m_StatsGeneratorDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath StatsGeneratorFilter::getStatsGeneratorDataContainerName() const
{
  return m_StatsGeneratorDataContainerName;
}

// -----------------------------------------------------------------------------
void StatsGeneratorFilter::setCellEnsembleAttributeMatrixName(const QString& value)
{
  m_CellEnsembleAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString StatsGeneratorFilter::getCellEnsembleAttributeMatrixName() const
{
  return m_CellEnsembleAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void StatsGeneratorFilter::setStatsDataArrayName(const QString& value)
{
  m_StatsDataArrayName = value;
}

// -----------------------------------------------------------------------------
QString StatsGeneratorFilter::getStatsDataArrayName() const
{
  return m_StatsDataArrayName;
}

// -----------------------------------------------------------------------------
void StatsGeneratorFilter::setCrystalStructuresArrayName(const QString& value)
{
  m_CrystalStructuresArrayName = value;
}

// -----------------------------------------------------------------------------
QString StatsGeneratorFilter::getCrystalStructuresArrayName() const
{
  return m_CrystalStructuresArrayName;
}

// -----------------------------------------------------------------------------
void StatsGeneratorFilter::setPhaseTypesArrayName(const QString& value)
{
  m_PhaseTypesArrayName = value;
}

// -----------------------------------------------------------------------------
QString StatsGeneratorFilter::getPhaseTypesArrayName() const
{
  return m_PhaseTypesArrayName;
}

// -----------------------------------------------------------------------------
void StatsGeneratorFilter::setPhaseNamesArrayName(const QString& value)
{
  m_PhaseNamesArrayName = value;
}

// -----------------------------------------------------------------------------
QString StatsGeneratorFilter::getPhaseNamesArrayName() const
{
  return m_PhaseNamesArrayName;
}

// -----------------------------------------------------------------------------
void StatsGeneratorFilter::setStatsDataArray(const StatsDataArray::Pointer& value)
{
  m_StatsDataArray = value;
}

// -----------------------------------------------------------------------------
StatsDataArray::Pointer StatsGeneratorFilter::getStatsDataArray() const
{
  return m_StatsDataArray;
}

// -----------------------------------------------------------------------------
void StatsGeneratorFilter::setCrystalStructures(const UInt32ArrayType::Pointer& value)
{
  m_CrystalStructures = value;
}

// -----------------------------------------------------------------------------
UInt32ArrayType::Pointer StatsGeneratorFilter::getCrystalStructures() const
{
  return m_CrystalStructures;
}

// -----------------------------------------------------------------------------
void StatsGeneratorFilter::setPhaseTypes(const UInt32ArrayType::Pointer& value)
{
  m_PhaseTypes = value;
}

// -----------------------------------------------------------------------------
UInt32ArrayType::Pointer StatsGeneratorFilter::getPhaseTypes() const
{
  return m_PhaseTypes;
}

// -----------------------------------------------------------------------------
void StatsGeneratorFilter::setPhaseNames(const StringDataArray::Pointer& value)
{
  m_PhaseNames = value;
}

// -----------------------------------------------------------------------------
StringDataArray::Pointer StatsGeneratorFilter::getPhaseNames() const
{
  return m_PhaseNames;
}

// -----------------------------------------------------------------------------
void StatsGeneratorFilter::setPath(const DataArrayPath& value)
{
  m_Path = value;
}

// -----------------------------------------------------------------------------
DataArrayPath StatsGeneratorFilter::getPath() const
{
  return m_Path;
}
