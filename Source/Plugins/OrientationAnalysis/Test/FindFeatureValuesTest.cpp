// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------

#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/EnsembleInfo.h"
#include "SIMPLib/Common/Observable.h"
#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/CoreFilters/CombineAttributeArrays.h"
#include "SIMPLib/CoreFilters/CreateDataContainer.h"
#include "SIMPLib/CoreFilters/ReadASCIIData.h"
#include "SIMPLib/CoreFilters/util/ASCIIWizardData.hpp"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Messages/FilterStatusMessage.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "UnitTestSupport.hpp"

#include "OrientationAnalysis/OrientationAnalysisFilters/CreateEnsembleInfo.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/FindAvgOrientations.h"
#include "OrientationAnalysisTestFileLocations.h"

#define CREATE_FILTER_STATUS(var, index) FilterStatusMessage::New(var->getNameOfClass(), var->getHumanLabel(), index, var->getHumanLabel() + " Complete")

class FindFeatureValuesTest
{
private:
  const QString k_CellDataCSVFile = UnitTest::OrientationAnalysisSourceDir + "/Test/TestFiles/fw-ar-IF1-aptr12-corr_CellData.csv";
  const QString k_FeatureDataCSVFile = UnitTest::OrientationAnalysisSourceDir + "/Test/TestFiles/fw-ar-IF1-aptr12-corr_FeatureData.csv";
  const QString k_EnsembleDataCSVFile = UnitTest::OrientationAnalysisSourceDir + "/Test/TestFiles/fw-ar-IF1-aptr12-corr_EnsembleData.csv";
  const QString k_IPFImageFile = UnitTest::OrientationAnalysisSourceDir + "/Test/TestFiles/fw-ar-IF1-aptr12-corr.png";

public:
  FindFeatureValuesTest() = default;
  virtual ~FindFeatureValuesTest() = default;

  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
//    QFile::remove(UnitTest::FindFeatureValuesTest::TestFile1);
//    QFile::remove(UnitTest::FindFeatureValuesTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  void Test()
  {
    Observer obs;
    int32_t index = 0;

    DataContainerArray::Pointer dca = DataContainerArray::New();

    CreateDataContainer::Pointer createDataContainer = CreateDataContainer::New();
    createDataContainer->connect(createDataContainer.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), &obs, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
    createDataContainer->setDataContainerArray(dca);
    createDataContainer->setDataContainerName({"DataContainer", "", ""});
    createDataContainer->execute();
    createDataContainer->messageGenerated(CREATE_FILTER_STATUS(createDataContainer, ++index));
    int32_t err = createDataContainer->getErrorCode();
    DREAM3D_REQUIRED(err, ==, 0)

    ReadASCIIData::Pointer readAsciCell = ReadASCIIData::New();
    readAsciCell->connect(readAsciCell.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), &obs, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
    ASCIIWizardData wizardData;
    wizardData.attrMatType = 3;
    wizardData.automaticAM = true;
    wizardData.beginIndex = 2;
    wizardData.consecutiveDelimiters = 0;
    wizardData.dataHeaders = QStringList({"FeatureIds", "Phases", "FZQuaternions_0", "FZQuaternions_1", "FZQuaternions_2", "FZQuaternions_3"});
    wizardData.dataTypes = QStringList({"int32_t", "int32_t", "float", "float", "float", "float"});
    wizardData.delimiters = QList<char>{','};
    wizardData.headerIsCustom = true;
    wizardData.headerLine = -1;
    wizardData.headerUsesDefaults = false;
    wizardData.inputFilePath = k_CellDataCSVFile;
    wizardData.numberOfLines = 22951;
    wizardData.selectedPath = {"DataContainer", "CellData", ""};
    wizardData.tupleDims = {306, 75, 1};
    readAsciCell->setDataContainerArray(dca);
    readAsciCell->setWizardData(wizardData);
    readAsciCell->execute();
    createDataContainer->messageGenerated(CREATE_FILTER_STATUS(readAsciCell, ++index));
    err = readAsciCell->getErrorCode();
    DREAM3D_REQUIRED(err, ==, 0)
    {
      CombineAttributeArrays::Pointer combineAttrArrays = CombineAttributeArrays::New();
      combineAttrArrays->connect(combineAttrArrays.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), &obs, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
      combineAttrArrays->setDataContainerArray(dca);
      combineAttrArrays->setMoveValues(true);
      combineAttrArrays->setNormalizeData(false);
      combineAttrArrays->setStackedDataArrayName("Quats");
      combineAttrArrays->setSelectedDataArrayPaths({{"DataContainer", "CellData", "FZQuaternions_0"},
                                                    {"DataContainer", "CellData", "FZQuaternions_1"},
                                                    {"DataContainer", "CellData", "FZQuaternions_2"},
                                                    {"DataContainer", "CellData", "FZQuaternions_3"}});
      combineAttrArrays->execute();
      createDataContainer->messageGenerated(CREATE_FILTER_STATUS(combineAttrArrays, ++index));
      err = combineAttrArrays->getErrorCode();
      DREAM3D_REQUIRED(err, ==, 0)
    }
    ReadASCIIData::Pointer readAsciFeature = ReadASCIIData::New();
    readAsciFeature->connect(readAsciFeature.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), &obs, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
    wizardData.attrMatType = 7;
    wizardData.automaticAM = true;
    wizardData.beginIndex = 2;
    wizardData.consecutiveDelimiters = 0;
    wizardData.dataHeaders = QStringList({"AvgQuats_0", "AvgQuats_1", "AvgQuats_2", "AvgQuats_3"});
    wizardData.dataTypes = QStringList({"float", "float", "float", "float"});
    wizardData.delimiters = QList<char>{','};
    wizardData.headerIsCustom = true;
    wizardData.headerLine = -1;
    wizardData.headerUsesDefaults = false;
    wizardData.inputFilePath = k_FeatureDataCSVFile;
    wizardData.numberOfLines = 50;
    wizardData.selectedPath = {"DataContainer", "FeatureData", ""};
    wizardData.tupleDims = {49};
    readAsciFeature->setDataContainerArray(dca);
    readAsciFeature->setWizardData(wizardData);
    readAsciFeature->execute();
    createDataContainer->messageGenerated(CREATE_FILTER_STATUS(readAsciFeature, ++index));
    err = readAsciFeature->getErrorCode();
    DREAM3D_REQUIRED(err, ==, 0)
    {
      CombineAttributeArrays::Pointer combineAttrArrays = CombineAttributeArrays::New();
      combineAttrArrays->connect(combineAttrArrays.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), &obs, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
      combineAttrArrays->setDataContainerArray(dca);
      combineAttrArrays->setMoveValues(true);
      combineAttrArrays->setNormalizeData(false);
      combineAttrArrays->setStackedDataArrayName("AvgQuats_GroundTruth");
      combineAttrArrays->setSelectedDataArrayPaths({{"DataContainer", "FeatureData", "AvgQuats_0"},
                                                    {"DataContainer", "FeatureData", "AvgQuats_1"},
                                                    {"DataContainer", "FeatureData", "AvgQuats_2"},
                                                    {"DataContainer", "FeatureData", "AvgQuats_3"}});
      combineAttrArrays->execute();
      err = combineAttrArrays->getErrorCode();
      DREAM3D_REQUIRED(err, ==, 0)
    }
    CreateEnsembleInfo::Pointer createEnsembleInfo = CreateEnsembleInfo::New();
    createEnsembleInfo->connect(createEnsembleInfo.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), &obs, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
    createEnsembleInfo->setDataContainerArray(dca);
    createEnsembleInfo->setCellEnsembleAttributeMatrixName("PhaseData");
    createEnsembleInfo->setCrystalStructuresArrayName("CrystalStructures");
    createEnsembleInfo->setDataContainerName({"DataContainer", "", ""});
    EnsembleInfo ensembleInfo;
    ensembleInfo.addValues(EnsembleInfo::CrystalStructure::Cubic_High, PhaseType::Type::Primary, "Iron bcc");
    createEnsembleInfo->setEnsemble(ensembleInfo);
    createEnsembleInfo->setPhaseNamesArrayName("PhaseNames");
    createEnsembleInfo->setPhaseTypesArrayName("PhaseTypes");
    createEnsembleInfo->execute();
    createDataContainer->messageGenerated(CREATE_FILTER_STATUS(createEnsembleInfo, ++index));
    err = createEnsembleInfo->getErrorCode();
    DREAM3D_REQUIRED(err, ==, 0)

    FindAvgOrientations::Pointer findAvgOrient = FindAvgOrientations::New();
    findAvgOrient->connect(findAvgOrient.get(), SIGNAL(messageGenerated(const AbstractMessage::Pointer&)), &obs, SLOT(processPipelineMessage(const AbstractMessage::Pointer&)));
    findAvgOrient->setDataContainerArray(dca);
    findAvgOrient->setAvgEulerAnglesArrayPath({"DataContainer", "FeatureData", "EulerAngles"});
    findAvgOrient->setAvgQuatsArrayPath({"DataContainer", "FeatureData", "AvgQuats"});
    findAvgOrient->setCellPhasesArrayPath({"DataContainer", "CellData", "Phases"});
    findAvgOrient->setCrystalStructuresArrayPath({"DataContainer", "PhaseData", "CrystalStructures"});
    findAvgOrient->setFeatureIdsArrayPath({"DataContainer", "CellData", "FeatureIds"});
    findAvgOrient->setQuatsArrayPath({"DataContainer", "CellData", "Quats"});
    findAvgOrient->execute();
    createDataContainer->messageGenerated(CREATE_FILTER_STATUS(findAvgOrient, ++index));
    err = findAvgOrient->getErrorCode();
    DREAM3D_REQUIRED(err, ==, 0)

    FloatArrayType& groundTruthAvgQuats = *(dca->getPrereqArrayFromPath<FloatArrayType>(nullptr, {"DataContainer", "FeatureData", "AvgQuats_GroundTruth"}, {4}));
    FloatArrayType& avgQuats = *(dca->getPrereqArrayFromPath<FloatArrayType>(nullptr, {"DataContainer", "FeatureData", "AvgQuats"}, {4}));
    size_t size = groundTruthAvgQuats.size();
    for(size_t i = 0; i < size; i++)
    {
      DREAM3D_COMPARE_FLOATS(groundTruthAvgQuats.getPointer(i), avgQuats.getPointer(i), 10)
    }
  }

  // -----------------------------------------------------------------------------
  void operator()()
  {
    std::cout << "      Starting Unit Test FindFeatureValuesTest     " << std::endl;
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(Test());

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

private:
  FindFeatureValuesTest(const FindFeatureValuesTest&); // Copy Constructor Not Implemented
  void operator=(const FindFeatureValuesTest&);        // Move assignment Not Implemented
};
