
#include <stdlib.h>

#include <iostream>
#include <map>
#include <set>
#include <string>

using namespace std::string_literals;

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMetaProperty>
#include <QtCore/QRegExp>
#include <QtCore/QRegularExpression>
#include <QtCore/QString>
#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"

#include "DREAM3DToolsConfiguration.h"

const std::set<std::string> k_BlackList = {"ConditionalSetValue", "CreateImageGeometry"};

const QString k_HeaderFile(D3DTools::GetDREAM3DToolsDir() + "/complex_filter.hpp.in");
const QString k_SourceFile(D3DTools::GetDREAM3DToolsDir() + "/complex_filter.cpp.in");

const QString k_AlgorithmHeaderFile(D3DTools::GetDREAM3DToolsDir() + "/complex_algorithm.hpp.in");
const QString k_AlgorithmSourceFile(D3DTools::GetDREAM3DToolsDir() + "/complex_algorithm.cpp.in");

const QString k_CMakeListsFile(D3DTools::GetDREAM3DToolsDir() + "/complex_CMakeLists.txt.in");
const QString k_PluginHeaderFile(D3DTools::GetDREAM3DToolsDir() + "/complex_plugin.hpp.in");
const QString k_PluginSourceFile(D3DTools::GetDREAM3DToolsDir() + "/complex_plugin.cpp.in");
const QString k_TestCMakeListsFile(D3DTools::GetDREAM3DToolsDir() + "/test_complex_CMakeLists.txt.in");
const QString k_UnitTestSourceFile(D3DTools::GetDREAM3DToolsDir() + "/unit_test_filter.cpp.in");
const QString k_TestDirsHeaderFile(D3DTools::GetDREAM3DToolsDir() + "/test_dirs.hpp.in");

#if 0
#if 0
const QString k_OutputDir("/tmp");
const QString k_PluginsOutputDir("/Users/mjackson/Workspace1/complex_plugins");
#else
const QString k_OutputDir("/tmp/");
const QString k_PluginsOutputDir("/tmp/complex_plugins");
#endif
#endif

//  static QString s_CurrentPlugin = "";
static bool s_HasAllParameters = false;
static int32_t s_TotalGoodFilters = 0;
static int32_t s_TotalBadFilters = 0;
static QMap<QString, QString> s_FilterUuidMapping;

static QString s_PrevToNewUuidMapping;
static std::map<std::string, bool> s_FilterHasAllParameters;

const QString k_PLUGIN_NAME("@PLUGIN_NAME@");
const QString k_PLUGIN_DESCRIPTION("@PLUGIN_DESCRIPTION@");
const QString k_PLUGIN_VENDOR("@PLUGIN_VENDOR@");
const QString k_FILTER_NAME("@FILTER_NAME@");
const QString k_ALGORITHM_NAME("@ALGORITHM_NAME@");
const QString k_UUID("@UUID@");
const QString k_OLD_UUID("@OLD_UUID@");
const QString k_PARAMETER_KEYS("@PARAMETER_KEYS@");
const QString k_PARAMETER_DEFS("@PARAMETER_DEFS@");
const QString k_FILTER_HUMAN_NAME("@FILTER_HUMAN_NAME@");
const QString k_PARAMETER_INCLUDES("@PARAMETER_INCLUDES@");
const QString k_PREFLIGHT_DEFS("@PREFLIGHT_DEFS@");
const QString k_INPUT_VALUES_DEF("@INPUT_VALUES_DEF@");
const QString k_DEFAULT_TAGS("@DEFAULT_TAGS@");
const QString k_PREFLIGHT_UPDATED_VALUES("@PREFLIGHT_UPDATED_VALUES@");
const QString k_PREFLIGHT_UPDATED_DEFS("@PREFLIGHT_UPDATED_DEFS@");
const QString k_PROPOSED_ACTIONS("@PROPOSED_ACTIONS@");

static QMap<QString, QString> s_ParameterMapping;
static QMap<QString, QString> s_InlcudeMapping;
static QMap<QString, QString> s_DefaultConstructorMapping;
static QMap<QString, QString> s_ParameterTypeMapping;
static QMap<QString, bool> s_ParameterAvailable;
static QMap<QString, int32_t> s_ParameterCount;
static QMap<QString, QString> s_UnitTestDefaultValue;

std::map<std::string, std::string> SIMPL_to_Complex{
    // syntax std::make_pair {Dream3d UUID , Dream3dnx UUID}, // dream3d-class-name
    {"0559aa37-c5ad-549a-82d4-bff4bfcb6cc6", "4bc81b8c-1594-409a-89eb-3ea8d8f061b0"}, // AbaqusHexahedronWriter
    {"abbe2e1e-6fb2-5511-91f3-0637252f0705", "155db7a2-7b12-4c52-8000-9f4421bd32d4"}, // AbaqusSurfaceMeshWriter
    {"28977b7a-5b88-5145-abcd-d1c933f7d975", "a44834c7-59c7-4aad-b1c2-bb88c79f60a0"}, // AdaptiveAlignmentFeature
    {"8ef88380-ece9-5f8e-a12d-d149d0856752", "9429f639-6be7-4de1-965c-8a4d5c00d00b"}, // AdaptiveAlignmentMisorientation
    {"738c8da9-45d0-53dd-aa54-3f3a337b70d7", "1a5dd69b-4af9-4575-8c26-b2237ddce13e"}, // AdaptiveAlignmentMutualInformation
    {"ac99b706-d1e0-5f78-9246-fbbe1efd93d2", "f44c66d1-a095-4e33-871c-c7699d89a011"}, // AddBadData
    {"9f392fba-1520-5f8f-988b-0032d7c51811", "d6f01afe-2fe7-458a-98e1-a4fd3ec6d48b"}, // AddOrientationNoise
    {"2bb76fa9-934a-51df-bff1-b0c866971706", "2bcaf030-2689-4da9-8456-35c1d9efad9d"}, // AlignSectionsFeature
    {"accf8f6c-0551-5da3-9a3d-e4be41c3985c", "6c0e9fcf-bea2-4939-999e-e26379ed7aad"}, // AlignSectionsList
    {"61c5519b-5561-58b8-a522-2ce1324e244d", "3cf33ad9-8322-4d40-96de-14bbe40969cc"}, // AlignSectionsMutualInformation
    {"52b2918a-4fb5-57aa-97d4-ccc084b89572", "c62c5c89-5ea8-4948-99ca-51cbc5b54b05"}, // AppendImageGeometryZSlice
    {"7ff0ebb3-7b0d-5ff7-b9d8-5147031aca10", "af95480b-ffc2-4cda-9d60-bb3bb53156f7"}, // ArrayCalculator
    {"9df4c18a-f51b-5698-8067-530d37f57c61", "2826d4e2-d42d-4158-9cb1-ab991f436732"}, // AverageEdgeFaceCellArrayToVertexArray
    {"caed6d53-6846-523f-a837-0692b3a81570", "8d752942-1fff-4e48-9940-013ecbe1e324"}, // AverageVertexArrayToEdgeFaceCellArray
    {"2861f4b4-8d50-5e69-9575-68c9d35f1256", "58661ea8-0322-44af-a48e-1dc80e999376"}, // AvizoRectilinearCoordinateWriter
    {"339f1349-9236-5023-9a56-c82fb8eafd12", "0ed4ae48-3295-4a5d-8505-d5d2a6f2a212"}, // AvizoUniformCoordinateWriter
    {"bff6be19-1219-5876-8838-1574ad29d965", "9fe07e17-aef1-4bf1-834c-d3a73dafc27d"}, // CAxisSegmentFeatures
    {"39df0167-1ef8-5e01-8702-c08c8afba1c0", "59d46e93-412c-4e18-9a41-bb424e57cd4b"}, // ClearDataMask
    {"7fc3ba91-781e-5627-b375-b4d4c402e57f", "c26c57cd-f9f8-4941-bc12-c58cc9413294"}, // CleaveTetVolumeMesh
    {"334034e9-405f-51a3-9c3c-8d9c955835d9", "a0152af1-6d22-4c21-b549-41b53ea0616d"}, // CombineAttributeMatrices
    {"71d46128-1d2d-58fd-9924-1714695768c3", "76b56f80-fcbe-4d48-a34d-a73d0fc6e5ae"}, // CombineStlFiles
    {"879e1eb8-40dc-5a5b-abe5-7e0baa77ed73", "94c5365c-41d8-42cf-a2a1-923a37818b6f"}, // ComputeFeatureEigenstrains
    {"6eda8dbf-dbd8-562a-ae1a-f2904157c189", "bf947c45-8610-44c9-917a-405e88f73fe7"}, // ComputeFeatureRect
    {"27a132b2-a592-519a-8cb7-38599a7f28ec", "650aa454-130c-406e-a9df-80b99f89e03c"}, // ComputeMomentInvariants2D
    {"3192d494-d1ec-5ee7-a345-e9963f02aaab", "83ed86d9-1d8a-4871-87c2-ce26e4bbeaf6"}, // ComputeUmeyamaTransform
    {"06d9ebfd-4c69-566a-8f4c-30e133a523e7", "8ffbd67d-19a5-41fe-9943-847415cd6d33"}, // CopyAttributeArray
    {"9bc962eb-f363-5caf-9f82-23a26be8ae2f", "752f3b11-98da-4762-91d6-a8e7c13e2ded"}, // CopyAttributeMatrix
    {"088ef69b-ca98-51a9-97ac-369862015d71", "e7f7284b-a06e-4247-9591-5ec1e4de6a2d"}, // CopyObject
    {"d702beff-eb02-5ee1-a76a-79d5b56ec730", "957ffcc4-e240-4e2c-afd8-5a3b69235d66"}, // CreateAbaqusFile
    {"542c2417-1685-54e5-9322-f60792e9176c", "407b4976-ac06-4827-9627-7af7124b447f"}, // CreateBsamFile
    {"c5bb92df-c1bb-5e57-a2bf-280303a81935", "82ff5ee4-334c-447f-b5cb-b279b202d31f"}, // CreateGridMontage
    {"e6b9a566-c5eb-5e3a-87de-7fe65d1d12b6", "85b8fc02-55a6-42df-a8d8-6489fe461428"}, // CreateStringArray
    {"c2d4f1e8-2b04-5d82-b90f-2191e8f4262e", "6afe99b8-fec6-4588-a083-2ba711c3a76e"}, // DBSCAN
    {"10b319ca-6b2f-5118-9f4e-d19ed481cd1f", "ae091368-35ff-48e2-a929-09daf90ec068"}, // DelaunayTriangulation
    {"14a39c35-8558-58c1-9d00-952396e6632b", "033ea842-43c9-424c-9480-fa3042d45ecd"}, // DetectEllipsoids
    {"ebdfe707-0c9c-5552-89f6-6ee4a1e0891b", "de7f7882-410f-40f9-9a46-a97dfdbc21b0"}, // DiscretizeDDDomain
    {"676cca06-40ab-56fb-bb15-8f31c6fa8c3e", "558575bb-62b1-4e68-bb5c-113fb4c71ffb"}, // DownsampleVertexGeometry
    {"ba2238f8-a20f-5f2f-ac9f-43ed458460f7", "1889dd1d-f699-464c-a00e-0958271843a0"}, // DxReader
    {"9072e51c-632f-5ee5-bf6b-9a90bfac2fcf", "78f0b066-bcc0-4e11-a4fe-a641a26fdf03"}, // DxWriter
    {"4cd8f98b-75d0-5293-bf8e-d0f9f6211f58", "3b8b661a-bb72-4edd-b941-7fb30f9705b0"}, // EMMPMFilter
    {"d26e85ff-7e52-53ae-b095-b1d969c9e73c", "e4553e82-ab82-49d7-993c-8b55b15c7724"}, // ErodeDilateCoordinationNumber
    {"4fff1aa6-4f62-56c4-8ee9-8e28ec2fcbba", "cab66cd1-f64c-42b4-8f94-18f0835a967f"}, // ErodeDilateMask
    {"14ad5792-a186-5e96-8e9e-4d623919dabc", "bd76bba1-00d4-4acd-88e0-05578525a8cb"}, // EstablishFoamMorphology
    {"28910d1c-4309-500a-9508-e3ef1612e1f8", "451be41e-354f-41ac-b73e-ae7d9a7e9504"}, // EstablishMatrixPhase
    {"4edbbd35-a96b-5ff1-984a-153d733e2abb", "e19a554d-227c-4091-bc46-f3a6aa541f7a"}, // EstablishShapeTypes
    {"8a2308ec-86cd-5636-9a0a-6c7d383e9e7f", "fb511a70-2175-4595-8c11-d1b5b6794221"}, // ExecuteProcess
    {"fcff3b03-bff6-5511-bc65-5e558d12f0a6", "7adbdb16-fe3c-4f51-89fc-884d1769eafd"}, // Export3dSolidMesh
    {"351c38c9-330d-599c-8632-19d74868be86", "207630e3-7372-46e4-93be-096f249dc7b5"}, // ExportCLIFile
    {"7c58e612-d7d6-5ec7-806b-cce0c1c211a3", "ab4b77ef-973c-48dc-977e-2771e3163095"}, // ExportDAMASKFiles
    {"33c10889-4cdc-5992-ae00-1795e9bee022", "8ae1680d-1216-43ac-9a6d-c8a8924ad0b3"}, // ExportLAMMPSFile
    {"d5873836-f150-5fc9-9bc8-0bc837bd8dd4", "cca4bc69-6edb-4c33-99ce-86d0b8c5e456"}, // ExportMultiOnScaleTableFile
    {"8efc447d-1c92-5ec5-885c-60b4a597835c", "12054925-afbc-436f-9070-13b179f42514"}, // ExportOnScaleTableFile
    {"2060a933-b6f5-50fd-9382-a008a5cef17f", "a25a02a9-e463-415d-b14f-0e1157219736"}, // ExtractAttributeArraysFromGeometry
    {"79d59b85-01e8-5c4a-a6e1-3fd3e2ceffb4", "fcc1c1cc-c37a-40fc-97fa-ce40201d34e3"}, // ExtractComponentAsArray
    {"e0555de5-bdc6-5bea-ba2f-aacfbec0a022", "5b9d0ab9-eee8-467a-b904-3f2894cf2764"}, // ExtractFlaggedFeatures
    {"1422a4d7-ecd3-5e84-8883-8d2c8c551675", "bb1ca09e-d445-4cad-914e-9998132807d8"}, // ExtractTripleLinesFromTriangleGeometry
    {"bc8a91ca-0cee-59c6-b5cb-acc1aab8617f", "621a71ca-124b-4471-ad1a-02f05ffba099"}, // ExtractVertexGeometry
    {"b6b1ba7c-14aa-5c6f-9436-8c46face6053", "4e210206-7872-4bd0-ad0c-b85fdc83ad73"}, // FFTHDFWriterFilter
    {"64d1df13-17a2-56a2-90a5-4dfda442b144", "cc07a095-16c8-4174-b7e1-8613aef487ce"}, // FeatureCountDecision
    {"a69d8d43-5be9-59a0-b997-81773a635673", "dac93adf-0aa0-43d5-abb3-f00228918643"}, // FeatureFaceCurvatureFilter
    {"38f04ea5-d6cd-5baa-8450-ac963570821b", "ee740d81-ecb4-4472-9589-cd1028813888"}, // FeatureInfoReader
    {"bf35f515-294b-55ed-8c69-211b7e69cb56", "645ecae2-cb30-4b53-8165-c9857dfa754f"}, // FindArrayStatistics
    {"cd91b8fd-0383-5803-ad26-9a47d6c309b0", "fc0c716d-7fb9-4b25-ada4-12663115996a"}, // FindAvgScalarValueForFeatures
    {"0cdb2c7f-55cb-5fc7-9108-b0c6826bd803", "7d9ead78-65d0-400f-aeb8-8f85bb941bc9"}, // FindBoundaryAreas
    {"8a1106d4-c67f-5e09-a02a-b2e9b99d031e", "a1dd1c29-9152-4648-836c-3b6967e32600"}, // FindBoundaryCells
    {"6357243e-41a6-52c4-be2d-2f6894c39fcc", "f268dab6-9a31-498e-8502-338cd7a549b4"}, // FindBoundaryElementFractions
    {"450c2f00-9ddf-56e1-b4c1-0e74e7ad2349", "d46f2fd7-dc68-4b57-bca3-693016512b2f"}, // FindBoundingBoxFeatures
    {"d87ff37c-120d-577d-a955-571c8280fa8e", "3b2365ec-3eed-44d3-8838-baaddff8155a"}, // FindCSLBoundaries
    {"8ec1fc8e-6484-5412-a898-8079986c0a26", "e550d811-a1ba-4304-a468-a9313fb9cdb7"}, // FindDerivatives
    {"ce4e8767-d74e-52a1-b34c-7fe0d1efa3b9", "63c1cdaa-7ec9-4ea3-8f04-6249e52d0886"}, // FindDirectionalModuli
    {"ef37f78b-bc9a-5884-b372-d882df6abe74", "bb480357-b33d-45b0-9bd7-c6c252d56081"}, // FindElementCentroids
    {"a1e9cf6d-2d1b-573e-98b8-0314c993d2b6", "d6e01678-3a03-433f-89ad-4e9adf1f9a45"}, // FindFeatureClustering
    {"64d20c7b-697c-5ff1-9d1d-8a27b071f363", "16010080-a913-443a-b5b3-bd43391fe3c0"}, // FindFeaturePhasesBinary
    {"4cf2cb82-76ee-504b-8d31-061d52ae6d30", "e42cb38e-b23f-4bdd-b761-5dd880c32280"}, // FindFeatureSignedDistanceFields
    {"9f77b4a9-6416-5220-a688-115f4e14c90d", "18ba2f7a-4e3a-4547-bd8c-b0955d542a9f"}, // FindLargestCrossSections
    {"c85277c7-9f02-5891-8141-04523658737d", "4d02d8db-7686-4acd-b468-e5f05805ce76"}, // FindLayerStatistics
    {"6cc3148c-0bad-53b4-9568-ee1971cadb00", "40288632-6c22-4c81-a201-3f0205a4ffb5"}, // FindMinkowskiBouligandDimension
    {"a0b4c16f-bfb1-57cf-aba1-eb08b5486abb", "70d7cff9-1dcb-405e-bdc9-7124b5a2126f"}, // FindModulusMismatch
    {"5d0cd577-3e3e-57b8-a36d-b215b834251f", "fd3223fa-e065-4b41-9b6f-06ba3f972c6d"}, // FindNorm
    {"529743cf-d5d5-5d5a-a79f-95c84a5ddbb5", "ec3388de-d7a7-4cf6-94f4-81061b0863c8"}, // FindNumFeatures
    {"577dfdf6-02f8-5284-b45b-e31f5392a191", "4ebd7734-0b1f-44eb-a2e4-a1d233af0d88"}, // FindProjectedImageStatistics
    {"801008ce-1dcb-5604-8f16-a86526e28cf9", "dd26b68a-2e64-4e58-a110-764646418478"}, // FindRelativeMotionBetweenSlices
    {"4178c7f9-5f90-5e95-8cf1-a67ca2a98a60", "b595b0b8-c848-4504-868f-7271b42a45a5"}, // FindSurfaceRoughness
    {"a5cff82b-9fe4-5a8c-90c9-6db74b6dcd50", "074c0523-ab7a-4097-b0c3-c4dcbfb9a05e"}, // FindTriangleGeomCentroids
    {"749dc8ae-a402-5ee7-bbca-28d5734c60df", "2d93bed7-6472-4b9e-a904-ab5c48983994"}, // FindTriangleGeomNeighbors
    {"26765457-89fb-5686-87f6-878ca549f0df", "e8f0fed3-d0d8-456e-b5a1-7961cc17b739"}, // FindTriangleGeomShapes
    {"9157ef1c-7cbc-5840-b6e7-26089c0b0f88", "a979bd9b-834e-4497-84b0-ab7a8add341a"}, // FindTriangleGeomSizes
    {"fcdde553-36b4-5731-bc88-fc499806cb4e", "af60518e-5fdb-45f2-9028-cd7787016830"}, // FindVertexToTriangleDistances
    {"68246a67-7f32-5c80-815a-bec82008d7bc", "340b3433-193a-4287-a7d2-794e1bce061b"}, // FindVolFractions
    {"6c255fc4-1692-57cf-be55-71dc4e05ec83", "1e789e0f-aee1-410b-9bf8-3c751c13aaae"}, // FitFeatureData
    {"433976f0-710a-5dcc-938e-fcde49cd842f", "6e0da3a1-dc42-4b01-80aa-3af9e8209ee3"}, // GBCDTriangleDumper
    {"0d0a6535-6565-51c5-a3fc-fbc00008606d", "d1731177-4d70-41c0-9334-566a0b482796"}, // GenerateColorTable
    {"19a1cb76-6b46-528d-b629-1af5f1d6344c", "643e29d3-5af4-4d51-be57-060ae9f0ba37"}, // GenerateEnsembleStatistics
    {"8b2fa51e-3bad-58ec-9fbf-b03b065e67fc", "435eb53a-2582-4e17-8b63-e75a4a1a81a7"}, // GenerateFeatureIDsbyBoundingBoxes
    {"ea565056-784e-52c5-b705-92f6799714c7", "36848349-68b6-4230-a8fb-a5b08ca9c82e"}, // GenerateGeometryConnectivity
    {"e4d6fda0-1143-56cc-9d00-c09a94e2f501", "5be12e6f-6afd-45fb-bddb-eb724d7ccdf3"}, // GenerateMaskFromSimpleShapes
    {"7152790d-26a4-571a-8fef-493120eced6d", "9f92ca90-48ea-47d7-9615-3dd0a90129e3"}, // GenerateMisorientationColors
    {"16659766-5c53-5ada-a7b7-8a95c29ea674", "8e1a4183-73b5-44b0-a355-0f06176881aa"}, // GeneratePrecipitateStatsData
    {"383f0e2a-c82e-5f7e-a904-156828b42314", "feca9156-280f-4a10-a9da-64cd4558893d"}, // GeneratePrimaryStatsData
    {"829da805-6d7c-5106-8209-aae1c207de15", "4337e42c-9f50-4e57-a835-cd01b3a0fad2"}, // GenerateTiltSeries
    {"ef28de7e-5bdd-57c2-9318-60ba0dfaf7bc", "1a35650d-a9f5-4ea2-af70-5b9ce894e45f"}, // GenerateVectorColors
    {"657a79de-7d60-5d9d-9f41-80abb62a3158", "b6c685f1-28e5-4975-8b00-996876678159"}, // GenerateVertexCoordinates
    {"073798a1-1fb4-5e3c-81f6-e426f60e347a", "520d60cb-1b58-4d55-857b-368f9fd4241e"}, // IdentifyDislocationSegments
    {"76ec2a58-a0cd-5548-b248-5a5eb08a1581", "189594d1-963f-4398-af63-1198fb9d1514"}, // ImportCLIFile
    {"f3def19a-b932-5ce7-9796-9e9476a29e1a", "de73a933-bd3e-4650-b375-81d966f58689"}, // ImportDelamData
    {"248fdae8-a623-511b-8d09-5368c793c04d", "1489ef78-b05f-4bbd-a35d-892ce7521dc2"}, // ImportFEAData
    {"85654f78-04a8-50ed-9ae1-25dfbd0539b3", "0544f31f-87f0-4f68-9e27-3c7af77fe349"}, // ImportMASSIFData
    {"06dd6e66-84fb-5170-a923-d925dc39bb94", "fc1de1a0-1fb4-462f-a2c7-8ae1eda5098e"}, // ImportOnScaleTableFile
    {"ab8f6892-2b57-5ec6-88b7-01610d80c32c", "48f97a46-37b9-4e64-80f5-5802f39e41ce"}, // ImportPrintRiteHDF5File
    {"38fde424-0292-5c42-b3b4-18d80c95524d", "60a8c8f1-c877-48cb-891f-df4e4a3a3535"}, // ImportPrintRiteTDMSFiles
    {"14f85d76-2400-57b8-9650-438563a8b8eb", "1686230c-c32f-466b-98f5-d9f1b36f6b5f"}, // ImportQMMeltpoolH5File
    {"60b75e1c-4c65-5d86-8cb0-8b8086193d23", "10a12123-d4f5-4c6a-8636-73c9d9181bfd"}, // ImportQMMeltpoolTDMSFile
    {"5fa10d81-94b4-582b-833f-8eabe659069e", "dfd93665-b455-499a-abed-36ae7dbcdf57"}, // ImportVolumeGraphicsFile
    {"c2ae366b-251f-5dbd-9d70-d790376c0c0d", "9ffb1c6e-26cd-4f3b-bdba-34a42a0652b7"}, // InitializeSyntheticVolume
    {"436eab43-0531-5e56-9309-5931109a85ca", "0d160df4-b3db-4a3b-9514-c721e37f7f31"}, // InputCrystalCompliances
    {"4ee65edd-8d7f-5b0b-a7dd-c4b96e272a87", "64149fe7-bf6d-4ecb-ab83-16f6233edfa7"}, // InsertAtoms
    {"1e552e0c-53bb-5ae1-bd1c-c7a6590f9328", "ba3c79c0-0e4c-40d2-8a98-bbff04066a3e"}, // InsertPrecipitatePhases
    {"fd6da27d-0f2c-5c35-802f-7d6ce1ad0ca1", "23682d16-3f6c-4a03-8a94-f2a29cfc4b8d"}, // InsertTransformationPhases
    {"b1c401e4-cb40-574d-a663-2170b5a7cdaa", "668be69d-2496-449b-bd03-64dd1a91dc53"}, // ItkAutoThreshold
    {"76fd1b13-5feb-5338-8d7f-b3b935ff3f22", "e5072b2f-e108-4a26-8a70-f2866b5b55c9"}, // ItkBinaryWatershedLabeled
    {"52f9e4c4-4e1c-55a3-a316-30c9e99c1216", "edc208c7-e9ce-4d8a-9dfc-6fba100d0e79"}, // ItkConvertArrayTo8BitImage
    {"cd075a60-93a9-52b4-ace6-84342b742c0a", "67aab419-677a-4bfc-bc5c-67d149eb95b0"}, // ItkConvertArrayTo8BitImageAttributeMatrix
    {"73ad993e-a36e-5978-bda2-ccee69d36186", "59aa4ee5-be66-4f48-89ec-da2b0d1a04fc"}, // ItkDiscreteGaussianBlur
    {"d2ebf8df-1469-5b77-bfcd-e9e99344749e", "000be116-d9c8-4a5a-9ec9-60ed12e7ea22"}, // ItkFindMaxima
    {"b09afbe3-8483-59ef-b0cd-3dcdaf313f37", "704f6145-aa17-40fb-9596-747e2f230671"}, // ItkGaussianBlur
    {"d9b598d3-ef06-5776-8f68-362931fa5092", "733f5db7-a851-47b5-a634-d0a25f2e08b9"}, // ItkGrayToRGB
    {"98721549-0070-5f80-80d6-0d8d31ade5d7", "26f7ea14-4d42-40b1-9d7e-b470023082d1"}, // ItkHoughCircles
    {"d0916970-0294-5970-aa76-efbee321b56f", "b1502eac-1772-46ed-be36-003351dc9ecd"}, // ItkImageCalculator
    {"57b7367b-c4f1-56e0-b47f-e61418479b03", "fe72b880-79af-4ecd-a541-0fda729d1bb7"}, // ItkImageMath
    {"f398f1e8-2e9a-5b5b-b784-f0b8ce7e0abf", "b8000dd0-351b-42c8-829a-560065f477ca"}, // ItkKMeans
    {"68d2b4e5-7325-5c9b-b3e0-26c726e8fd6f", "2a4b3db3-8335-44d2-83b1-8e84515f0a42"}, // ItkKdTreeKMeans
    {"8a8d3481-34d4-5845-b41f-f89e1d7448b7", "d1180c91-c4e9-4264-9297-1cb797c005a5"}, // ItkManualThreshold
    {"9f6b76ba-cf04-5da1-8e99-783ff481ed85", "3d452981-3be3-4116-b5d3-8b80aca63614"}, // ItkMeanKernel
    {"303cc321-df55-5bb9-b3a4-22d490b728e7", "e13e6898-739e-436c-ada2-c7a3d4738024"}, // ItkMedianKernel
    {"fa281497-2f98-5fc7-bfd7-305dcea866ed", "0a6088e8-19f2-43cb-9178-2aff90c6009f"}, // ItkMultiOtsuThreshold
    {"73917acf-23aa-59ac-b4fd-ab59a8ce1060", "1e4503ef-bf0e-4490-9618-28e99ddb31a7"}, // ItkSobelEdge
    {"aed7a137-bf2f-5bbc-b5e6-bf5db18e46c2", "9d7d0bb9-8e37-481f-988f-b02a5d96bbe8"}, // ItkStitchImages
    {"994eb3c3-dafa-5611-8afe-efdac8c2f7da", "7b4778c0-5a64-4fec-b043-68944d9a1d39"}, // ItkWatershed
    {"b7301dbf-27d5-5335-b86e-563d573f002b", "85b7e460-f80f-4b14-9fa4-8ce28bf412f3"}, // JumbleOrientations
    {"247ea39f-cb50-5dbb-bb9d-ecde12377fed", "8a990207-fa87-40b0-af4b-8cf5353b67ed"}, // KDistanceGraph
    {"b56a04de-0ca0-509d-809f-52219fca9c98", "b8682b04-ccc0-49a2-9ced-32d5f5c512f3"}, // KMeans
    {"f7486aa6-3049-5be7-8511-ae772b70c90b", "7643cb64-bcb9-4867-b85a-bf158cd0f54a"}, // KMedoids
    {"a250a228-3b6b-5b37-a6e4-8687484f04c4", "7a7a2c6f-3b03-444d-8b8c-5976b0e5c82e"}, // LabelTriangleGeometry
    {"1cf52f08-a11a-5870-a38c-ea8958071bd8", "4fa2d9ed-dcfd-44bb-90c1-d561f1f24541"}, // LaplacianSmoothPointCloud
    {"52ce1c70-74b1-581a-af32-0212c8396739", "2f838629-5b13-43ee-b439-837f92090aa7"}, // Lesson1
    {"f5295d39-4563-5a71-927d-3c0f5e36702a", "6e707a6a-eab0-4caa-b7f1-9330441a7d1c"}, // Lesson2
    {"c712cd5a-5223-58a8-a0bd-14c06009b2ed", "795caa2f-b02d-41c8-aaca-297ea263c56b"}, // Lesson3
    {"43bceb39-849d-5c9a-a75c-5ef958908007", "7e5f1fc2-2e78-4ca4-aaec-98c16e83516f"}, // Lesson4
    {"169f34c2-62b2-57cf-ac79-9067dbac0d73", "e64d4f6b-33d2-41ab-b0c5-511d86829dcf"}, // Lesson5
    {"b2eb2376-9c34-5744-bbb3-05c8874a3a31", "d160f282-25bf-4709-b34a-c308c662fe94"}, // Lesson6
    {"a4b86d5f-d812-50d9-8d95-3f3f5fcb0597", "6bce0de9-e108-44d0-a1f4-ee6759ca3406"}, // Lesson7
    {"0e1c45f6-ed7a-5279-8a5c-a2d5cc6bfead", "e76a23c9-89d4-4afa-bc4c-115aabd140a9"}, // LinkFeatureMapToElementArray
    {"620a3022-0f92-5d07-b725-b22604874bbf", "2d0e57b4-288b-4eb6-b667-a69184a3088f"}, // LocalDislocationDensityCalculator
    {"158ebe9e-f772-57e2-ac1b-71ff213cf890", "9d5ffeba-4339-49d2-a519-5743fbd5e7a9"}, // LosAlamosFFTWriter
    {"34a19028-c50b-5dea-af0e-e06c798d3686", "49f3fe99-72c9-4458-8bb1-18a4c27a0504"}, // MaskCountDecision
    {"7bfb6e4a-6075-56da-8006-b262d99dff30", "47de904e-6108-40cb-9c1b-a885e2e2ca8d"}, // MatchCrystallography
    {"2c4a6d83-6a1b-56d8-9f65-9453b28845b9", "7e3dbc15-51a3-482c-97c2-f82f7af685bf"}, // MergeColonies
    {"e3702900-a6c1-59e1-9180-b57557a7b193", "5fac94ab-1b8e-403b-af7e-01e034cd5f43"}, // MoveMultiData
    {"b2847755-dd39-5989-b624-98b1fdc9db5b", "82be6e42-e174-480c-8e9b-e798bbb72f07"}, // MultiEmmpmFilter
    {"cbaf9e68-5ded-560c-9440-509289100ea8", "c8746bf8-fa5f-48b0-8a47-11023402e543"}, // NearestPointFuseRegularGrids
    {"8c584519-15c3-5010-b5ed-a2ac626591a1", "b8097cc3-2841-4708-90b9-7001c7c8d719"}, // NormalizeArrays
    {"84305312-0d10-50ca-b89a-fda17a353cc9", "9b786e15-e01a-458d-82cd-d6483b22d13c"}, // PackPrimaryPhases
    {"39652621-3cc4-5a72-92f3-e56c516d2b18", "7824d032-a370-4a6f-9e2d-b6bfe5a32467"}, // ParaDisReader
    {"c923176f-39c9-5521-9786-624f88d2b2c0", "628615b7-ebfc-4e52-9ff8-3a926a17d4f7"}, // PhReader
    {"4786a02e-5fe1-58e0-a906-15556b40d5ce", "b824e9bb-0676-4581-b919-c70eb51f0551"}, // PhWriter
    {"8cc2198b-6a9d-5bf4-b8c0-b0878bb57f10", "daeb2404-338e-4355-868c-9ca3fe648141"}, // PipelineAnnotation
    {"0ca83462-8564-54ea-9f4e-e5141974f30b", "0625f382-02b4-4ef4-b9d2-dbab66d5a78a"}, // PostSlackMessage
    {"e15ec84b-1e02-53a6-a830-59e0813775a1", "236a6372-cce2-4ddf-80de-c98269588469"}, // PottsModel
    {"ec163736-39c8-5c69-9a56-61940a337c07", "48d7200b-26ec-4ed9-8d1c-c8111118508a"}, // PrincipalComponentAnalysis
    {"f2259481-5011-5f22-9fcb-c92fb6f8be10", "5469af5c-368a-465b-87b7-7c0dfdf73666"}, // ReadBinaryCTNorthStar
    {"5a2b714e-bae9-5213-8171-d1e190609e2d", "09887e07-c1e5-4631-91fb-1e28be5b7054"}, // ReadMicData
    {"0df3da89-9106-538e-b1a9-6bbf1cf0aa92", "7d061fe1-7c88-45ef-a7ee-f8ab027c8e85"}, // RegularGridSampleSurfaceMesh
    {"bc4952fa-34ca-50bf-a1e9-2b9f7e5d47ce", "d33bdfec-3fac-4f54-84a9-131eb79b8c25"}, // RegularizeZSpacing
    {"1b4b9941-62e4-52f2-9918-15d48147ab88", "c29d1d17-c2e8-4c85-8272-d16be01adbf5"}, // RemoveComponentFromArray
    {"53a5f731-2858-5e3e-bd43-8f2cf45d90ec", "d87a756a-2182-4748-b149-bd0de22e87f8"}, // RenameAttributeArray
    {"ee29e6d6-1f59-551b-9350-a696523261d5", "b4158c59-eb6d-4479-8eaa-228f2cc48be2"}, // RenameAttributeMatrix
    {"a37f2e24-7400-5005-b9a7-b2224570cbe9", "d0632fb9-fd32-4c3c-812f-991f8788b337"}, // ReplaceValueInArray
    {"1fe19578-6856-55f2-adc8-2236fac22c25", "2ef70939-2c17-4509-afa0-c1f3a1bf205d"}, // RequiredZThickness
    {"77befd69-4536-5856-9f81-02996d038f73", "28ed3258-41b2-45a0-8f37-6574264650f9"}, // ResampleRectGridToImageGeom
    {"9b9fb9e1-074d-54b6-a6ce-0be21ab4496d", "f11a45e9-c169-4227-85b0-0fb0108e47de"}, // ReverseTriangleWinding
    {"48db4da6-19c3-52a4-894f-776f3a57362e", "fcb2f2a4-75ee-4a44-8bc6-2a8b419b657b"}, // SPParksDumpReader
    {"bcf2f246-610f-5575-a434-241d04114b9f", "dec4ded3-13d7-4da9-a7af-fb2579cb9c9f"}, // SPParksSitesWriter
    {"0f44da6f-5272-5d69-8378-9bf0bc4ae4f9", "0373dae8-8354-46fd-a186-9c6b6c6887c4"}, // SampleSurfaceMeshSpecifiedPoints
    {"3cf44c27-9149-5548-945a-deef1dc994a8", "1121a7db-845d-47ca-b5b7-658bd46c7aeb"}, // ScaleVolume
    {"15c743db-5936-53a2-92cf-edf00526486d", "aaf7a258-fc92-48d7-9d06-ba317a3769e8"}, // SharedFeatureFaceFilter
    {"f84d4d69-9ea5-54b6-a71c-df76d76d50cf", "520de667-5ff0-4e8d-bb70-a3b6122cf4ba"}, // Silhouette
    {"222307a4-67fd-5cb5-a12e-d80f9fb970ae", "754a47c0-149a-4109-a810-2b2968743895"}, // SliceTriangleGeometry
    {"f642e217-4722-5dd8-9df9-cee71e7b26ba", "fd7861f9-9972-4f86-afb7-d9c589e3a636"}, // StatsGeneratorFilter
    {"07b1048e-d6d4-56d0-8cc5-132ac79bdf60", "addb4284-2048-40bd-ac2e-b1d4c4589913"}, // SteinerCompact
    {"6f1abe57-ca7b-57ce-b03a-8c6f06fdc633", "e30e9cee-fb85-4414-af46-41ae2df329f4"}, // TesselateFarFieldGrains
    {"81e94b15-efb6-5bae-9ab1-c74099136174", "98fb00fe-de43-439b-a875-bfdbe3d93412"}, // TiDwellFatigueCrystallographicAnalysis
    {"7aa33007-4186-5d7f-ba9d-d0a561b3351d", "5d7156ae-3e0a-4db0-a792-552f492e8911"}, // TriangleCentroidFilter
    {"75cfeb9b-cd4b-5a20-a344-4170b39bbfaf", "c594cc3b-3f45-43b1-9030-1aa56c755fc8"}, // UncertainRegularGridSampleSurfaceMesh
    {"317afa50-defb-541c-8d42-ea659953bde2", "548b1e1f-7c4e-4fa9-a851-2f66a2a7c87f"}, // VMFillLevelSetWithTetrahedra
    {"e2b44fbc-f216-5009-a258-578bbe56b587", "b90d6860-4921-4586-b424-66658f281d8b"}, // VMFindDistanceFieldFromTriangleGeometry
    {"f62065b4-54e9-53b1-bed7-2178a57d3c7a", "31710abf-b8a6-423f-a03f-4b8885bd3633"}, // VisualizeGBCDGMT
    {"85900eba-3da9-5985-ac71-1d9d290a5d31", "eed5183e-5a9c-485b-9e64-579a81f0d301"}, // VisualizeGBCDPoleFigure
    {"a043bd66-2681-5126-82e1-5fdc46694bf4", "58a4d758-c5ff-43d3-a394-1508ad00a0d2"}, // VtkRectilinearGridWriter
    {"f2fecbf9-636b-5ef9-89db-5cb57e4c5676", "60d55662-ca7a-4be7-b0a0-ed6e785eb51b"}, // VtkStructuredPointsReader
    {"520fc4c4-9c22-5520-9e75-a64b81a5a38d", "5c7f39d5-a12e-4712-a92b-238b1067076b"}, // WarpRegularGrid
    {"5d4c38fe-af0a-5cb3-b7fa-fb9e57b2097f", "459db803-64f1-4447-84f0-fe1a2f340d8c"}, // WaveFrontObjectFileWriter
    {"b9134758-d5e5-59dd-9907-28d23e0e0143", "54a293f4-1366-46ca-b284-fe5965545dd2"}, // WriteStlFile
    {"5e523ec1-49ac-541e-a4ba-6fa725798b91", "a11c1306-f89e-4a1d-9133-0efeda3b0bba"}, // WriteTriangleGeometry
    {"4fb2b9de-3124-534b-b914-dbbbdbc14604", "8df2135c-7079-49f4-9756-4f3c028a5ced"}, // AlignSectionsMisorientation
    {"f4a7c2df-e9b0-5da9-b745-a862666d6c99", "3f342977-aea1-49e1-a9c2-f73760eba0d3"}, // BadDataNeighborOrientationCheck
    {"e5629880-98c4-5656-82b8-c9fe2b9744de", "501e54e6-a66f-4eeb-ae37-00e649c00d4b"}, // ConvertOrientations
    {"7861c691-b821-537b-bd25-dc195578e0ea", "1810c2c7-63e3-41db-b204-a5821e6271c0"}, // EBSDSegmentFeatures
    {"bf7036d8-25bd-540e-b6de-3a5ab0e42c5f", "086ddb9a-928f-46ab-bad6-b1498270d71e"}, // FindAvgOrientations
    {"428e1f5b-e6d8-5e8b-ad68-56ff14ee0e8c", "24b54daf-3bf5-4331-93f6-03a49f719bf1"}, // FindFeatureReferenceMisorientations
    {"88d332c1-cf6c-52d3-a38d-22f6eae19fa6", "61cfc9c1-aa0e-452b-b9ef-d3b9e6268035"}, // FindKernelAvgMisorientations
    {"286dd493-4fea-54f4-b59e-459dd13bbe57", "0b68fe25-b5ef-4805-ae32-20acb8d4e823"}, // FindMisorientations
    {"e67ca06a-176f-58fc-a676-d6ee5553511a", "b4681855-0a3d-4237-97f2-5aec509115c4"}, // FindSchmids
    {"9a6677a6-b9e5-5fee-afa2-27e868cab8ca", "8b651407-08a9-4c25-967a-d86444eca87f"}, // GenerateFZQuaternions
    {"a50e6532-8075-5de5-ab63-945feb0de7f7", "64cb4f27-6e5e-4dd2-8a03-0c448cb8f5e6"}, // GenerateIPFColors
    {"c9af506e-9ea1-5ff5-a882-fa561def5f52", "f173786a-50cd-4c3c-9518-48ef6fc2bac9"}, // MergeTwins
    {"6427cd5e-0ad2-5a24-8847-29f8e0720f4f", "4625c192-7e46-4333-a294-67a2eb64cb37"}, // NeighborOrientationCorrelation
    {"b8e128a8-c2a3-5e6c-a7ad-e4fb864e5d40", "5b062816-79ac-47ce-93cb-e7966896bcbd"}, // ReadAngData
    {"d1df969c-0428-53c3-b61d-99ea2bb6da28", "7751923c-afb9-4032-8372-8078325c69a4"}, // ReadCtfData
    {"4ef7f56b-616e-5a80-9e68-1da8f35ad235", "db291b6c-9aaf-432d-9547-430c865a539c"}, // ReadH5Ebsd
    {"ef9420b2-8c46-55f3-8ae4-f53790639de4", "0458edcd-3655-4465-adc8-b036d76138b5"}, // RotateEulerRefFrame
    {"e1343abe-e5ad-5eb1-a89d-c209e620e4de", "fcdb311f-a70c-4bf8-9e83-9275b36bc57f"}, // ConvertHexGridToSquareGrid
    {"439e31b7-3198-5d0d-aef6-65a9e9c1a016", "d6b51fb2-279d-41cd-ad1a-c7a5a3f4b1cc"}, // ConvertQuaternion
    {"2a0bfcd3-2517-5117-b164-964dfac8fe22", "8ce3d70c-49fe-4812-a1eb-7ce4c962a59d"}, // CreateEnsembleInfo
    {"c4398303-db7d-506e-81ea-08f253895ccb", "916e2881-4c1c-47bd-99b0-6fb183ecdcea"}, // CreateLambertSphere
    {"6e332fca-0475-5fec-821e-e01f668ec1d3", "2d05ca72-0a1b-4aec-b9b0-bc470845c448"}, // EbsdToH5Ebsd
    {"b78d8825-d3ac-5351-be20-172f07fd2aec", "74478e86-ce29-40b8-8c17-d20009195f91"}, // EMsoftSO3Sampler
    {"33a37a47-d002-5c18-b270-86025881fe1e", "ecf1ec45-bc27-4ddb-b2c0-3d465b8c182a"}, // EnsembleInfoReader
    {"c5a9a96c-7570-5279-b383-cc25ebae0046", "453cdb58-7bbb-4576-ad5e-f75a1c54d348"}, // FindAvgCAxes
    {"8071facb-8905-5699-b345-105ae4ac33ff", "f39707f9-0411-4736-a591-1bfbd099bb71"}, // FindBoundaryStrengths
    {"68ae7b7e-b9f7-5799-9f82-ce21d0ccd55e", "a51c257a-ddc1-499a-9b21-f2d25a19d098"}, // FindCAxisLocations
    {"94f986fc-1295-5e32-9808-752855fa658a", "dbd14c09-f447-4086-b270-90919fce3f2e"}, // FindDistsToCharactGBs
    {"cdd50b83-ea09-5499-b008-4b253cf4c246", "636ee030-9f07-4f16-a4f3-592eff8ef1ee"}, // FindFeatureNeighborCAxisMisalignments
    {"1a0848da-2edd-52c0-b111-62a4dc6d2886", "16c487d2-8f99-4fb5-a4df-d3f70a8e6b25"}, // FindFeatureReferenceCAxisMisorientations
    {"6e97ff50-48bf-5403-a049-1d271bd72df9", "0cc592b7-a3d3-4c6a-8894-1533670a31b9"}, // FindGBCD
    {"d67e9f28-2fe5-5188-b0f8-323a7e603de6", "fc0d695a-e381-4f11-a8fd-26d5b5cda30a"}, // FindGBCDMetricBased
    {"00d20627-5b88-56ba-ac7a-fc2a4b337903", "8267878b-d23a-4666-83a0-f1a7711861ce"}, // FindGBPDMetricBased
    {"97523038-5fb2-5e82-9177-ed3e8b24b4bd", "7569d075-d05a-4e07-8660-5cca3e78ee49"}, // FindSlipTransmissionMetrics
    {"a10124f3-05d0-5f49-93a0-e93926f5b48b", "7e6e8432-0fe0-42aa-a103-a47ece44cf66"}, // FindTwinBoundaries
    {"b0e30e6d-912d-5a7e-aeed-750134aba86b", "19ad8363-40b5-45aa-829a-4406f35b00ce"}, // FindTwinBoundarySchmidFactors
    {"0a121e03-3922-5c29-962d-40d88653f4b6", "30759600-7c02-4650-b5ca-e7036d6b568e"}, // GenerateFaceIPFColoring
    {"7cd30864-7bcf-5c10-aea7-d107373e2d40", "f3473af9-db77-43db-bd25-60df7230ea73"}, // GenerateFaceMisorientationColoring
    {"ec58f4fe-8e51-527e-9536-8b6f185684be", "bf7cc5a9-42b6-4147-8c56-de234cd6b949"}, // GenerateOrientationMatrixTranspose
    {"630d7486-75ea-5e04-874c-894460cd7c4d", "e1653c62-3894-4b95-8449-605400038110"}, // GenerateQuaternionConjugate
    {"179b0c7a-4e62-5070-ba49-ae58d5ccbfe8", "53b4d0b2-1aa3-4725-93df-f2857e491931"}, // ImportEbsdMontage
    {"8abdea7d-f715-5a24-8165-7f946bbc2fe9", "694a0d54-b492-41bc-993b-ce0f5006e927"}, // ImportH5EspritData
    {"3ff4701b-3a0c-52e3-910a-fa927aa6584c", "4ad3d47c-b1e1-4429-bc65-5e021be62ba0"}, // ImportH5OimData
    {"27c724cc-8b69-5ebe-b90e-29d33858a032", "a5a37eb3-c143-4b38-a89b-e0641a20dc7e"}, // INLWriter
    {"5af9c1e6-ed6f-5672-9ae0-2b931344d729", "cf06dcb7-fa92-4606-9716-28ede2cd1c62"}, // OrientationUtility
    {"17410178-4e5f-58b9-900e-8194c69200ab", "65128c53-d3be-4a69-a559-32a48d603884"}, // ReplaceElementAttributesWithNeighborValues
    {"a2b62395-1a7d-5058-a840-752d8f8e2430", "3704a3c6-dd09-4b91-9eda-2fbb4cb9bf99"}, // RodriguesConvertor
    {"3630623e-724b-5154-a060-a5fca4ecfff5", "42ddcd69-dde3-4b08-b90c-6002079540eb"}, // Stereographic3D
    {"a10bb78e-fcff-553d-97d6-830a43c85385", "00cbb97e-a5c2-43e6-9a35-17a0f9ce26ed"}, // WritePoleFigure
    {"a4952f40-22dd-54ec-8c38-69c3fcd0e6f7", "aa6d399b-715e-44f1-9902-f1bd18faf1c5"}, // WriteStatsGenOdfAngleFile
    {"09f45c29-1cfb-566c-b3ae-d832b4f95905", "e9dd12bc-f7fa-4ba2-98b0-fec3326bf620"}, // ITKAbsImage
    {"b09ec654-87a5-5dfa-9949-aa69f1fbfdd1", "e7411c44-95ab-4623-8bf4-59b63d2d08c5"}, // ITKAcosImage
    {"2d5a7599-5e01-5489-a107-23b704d2b5eb", "ea3e7439-8327-4190-8ff7-49ecc321718f"}, // ITKAdaptiveHistogramEqualizationImage
    {"79509ab1-24e1-50e4-9351-c5ce7cd87a72", "1b463492-041f-4680-abb1-0b94a3019063"}, // ITKAsinImage
    {"e938569d-3644-5d00-a4e0-ab327937457d", "39933f50-088c-46ac-a421-d238f1b178fd"}, // ITKAtanImage
    {"3c451ac9-bfef-5e41-bae9-3957a0fc26a1", "ed214e76-6954-49b4-817b-13f92315e722"}, // ITKBinaryContourImage
    {"ba8a3f2e-3963-57c0-a8da-239e25de0526", "ba2494b0-c4f0-43ff-9d08-900395900e0c"}, // ITKBinaryThresholdImage
    {"99a7aa3c-f945-5e77-875a-23b5231ab3f4", "b5ff32a8-e799-4f72-8d13-e2581f748562"}, // ITKClosingByReconstructionImage
    {"2c2d7bf6-1e78-52e6-80aa-58b504ce0912", "6fe37f77-ceae-4839-9cf6-3ca7a70e14d0"}, // ITKCosImage
    {"3aa99151-e722-51a0-90ba-71e93347ab09", "719df7b2-8db2-43eb-a40c-a015982eec08"}, // ITKGradientMagnitudeImage
    {"fd688b32-d90e-5945-905b-2b7187b46265", "0ac5ed47-963e-4c54-b33d-967e0aa6a621"}, // ITKGradientMagnitudeRecursiveGaussianImage
    {"54c8dd45-88c4-5d4b-8a39-e3cc595e1cf8", "1f1dd9e4-d361-432b-a22b-5535664ee545"}, // ITKGrayscaleFillholeImage
    {"653b7b5c-03cb-5b32-8c3e-3637745e5ff6", "d72eaf98-9b1d-44c9-88f2-a5c3cf57b4f2"}, // ITKImageReader
    {"11473711-f94d-5d96-b749-ec36a81ad338", "a181ee3e-1678-4133-b9c5-a9dd7bfec62f"}, // ITKImageWriter
    {"cf7d7497-9573-5102-bedd-38f86a6cdfd4", "dcf980b7-ecca-46d1-af31-ac65f6e3b6bb"}, // ITKImportImageStack
    {"c6e10fa5-5462-546b-b34b-0f0ea75a7e43", "9958d587-5698-4ea5-b8ea-fb71428b5d02"}, // ITKInvertIntensityImage
    {"dbfd1a57-2a17-572d-93a7-8fd2f8e92eb0", "900ca377-e79d-4b54-b298-33d518238099"}, // ITKLog10Image
    {"69aba77c-9a35-5251-a18a-e3728ddd2963", "4b6655ad-4e6c-4e68-a771-55ca0ae40915"}, // ITKLogImage
    {"97102d65-9c32-576a-9177-c59d958bad10", "d3138266-3f34-4d6e-8e21-904c94351293"}, // ITKMaskImage
    {"cc27ee9a-9946-56ad-afd4-6e98b71f417d", "a60ca165-59ac-486b-b4b4-0f0c24d80af8"}, // ITKMedianImage
    {"b2248340-a371-5899-90a2-86047950f0a2", "f70337e5-4435-41f7-aecc-d79b4b1faccd"}, // ITKMorphologicalWatershedImage
    {"5b905619-c46b-5690-b6fa-8e97cf4537b8", "9d8ce30e-c75e-4ca8-b6be-0b11baa7e6ce"}, // ITKNormalizeImage
    {"ca04004f-fb11-588d-9f77-d00b3ee9ad2a", "c4225a23-0b23-4782-b509-296fb39a672b"}, // ITKOpeningByReconstructionImage
    {"6e66563a-edcf-5e11-bc1d-ceed36d8493f", "30f37bcd-701f-4e64-aa9d-1181469d3fb5"}, // ITKOtsuMultipleThresholdsImage
    {"bb15d42a-3077-582a-be1a-76b2bae172e9", "e81f72d3-e806-4afe-ab4c-795c6a3f526f"}, // ITKSignedMaurerDistanceMapImage
    {"1eb4b4f7-1704-58e4-9f78-8726a5c8c302", "06c76c7a-c384-44be-bd01-6fd58070cd65"}, // ITKSinImage
    {"8087dcad-68f2-598b-9670-d0f57647a445", "05c7c812-4e33-4e9a-bf27-d4c17f5dff68"}, // ITKSqrtImage
    {"672810d9-5ec0-59c1-a209-8fb56c7a018a", "7cf3c08e-1af1-4540-aa08-4488a74923fc"}, // ITKTanImage
    {"7c2a7c4e-4582-52a6-92de-16b626d43fbf", "f6bcb859-3cb1-4fab-a520-8fcc21be9a0a"}, // ApplyDewarpParameters
    {"30687f44-9c10-5617-bcb8-4109cbd6e55e", "367f8491-1189-48fa-b297-db170fb87259"}, // AxioVisionV4ToTileConfiguration
    {"5394f60c-b0f0-5f98-85da-3b5730109060", "bd12a81d-730c-41d6-8dad-05d782d2e2d7"}, // CalcDewarpParameters
    {"a48f7a51-0ca9-584f-a0ca-4bfebdc41d7c", "95866cba-a92e-4d87-b7f6-8f03928ee633"}, // IlluminationCorrection
    {"411b008c-006f-51b2-ba05-99e51a01af3c", "697f306e-9acd-4aba-b2c9-486acda269d4"}, // ImportAxioVisionV4Montage
    {"c5474cd1-bea9-5a33-a0df-516e5735bab4", "290bb407-f685-4b93-858f-7bcfcdfb679b"}, // ImportVectorImageStack
    {"774ec947-eed6-5eb2-a01b-ee6470e61521", "e11e4cb3-5e21-4130-873b-3f14d0c2c4a3"}, // ImportZenInfoMontage
    {"5878723b-cc16-5486-ac5f-ff0107107e74", "b3a528aa-3029-4f9f-956b-b1d647e620a8"}, // ITKImportFijiMontage
    {"cdb130af-3616-57b1-be59-fe18113b2621", "56b6e862-13f4-4c90-87e9-ec76db1c584b"}, // ITKImportRoboMetMontage
    {"49b5feb1-ec05-5a26-af25-00053151d944", "6cf74e33-0a8d-402d-b3ee-b3b2f3cda07e"}, // ITKRefineTileCoordinates
    {"fa4efd40-f4a6-5524-9fc6-e1f8bbb2c42f", "0417e934-1294-4387-9ef5-53c19d7d40b0"}, // ITKStitchMontage
    {"066712e3-0378-566e-8236-1796c88d5e02", "6ccb887c-79d6-4e20-a09b-c79b0bb6a875"}, // ITKApproximateSignedDistanceMapImage
    {"18ab754c-3219-59c8-928e-5fb4a09174e0", "2c2351bc-f39f-43d7-ae02-2ec14be1dada"}, // ITKBilateralImage
    {"0cf0698d-65eb-58bb-9f89-51e875432197", "ba8535b3-3762-44a3-b147-45642c5634fc"}, // ITKBinaryClosingByReconstructionImage
    {"f86167ad-a1a1-557b-97ea-92a3618baa8f", "83b8f253-dfa8-4195-9349-6fb89539ea1a"}, // ITKBinaryDilateImage
    {"522c5249-c048-579a-98dd-f7aadafc5578", "fcb447ba-c875-4c74-8341-89ba9657a772"}, // ITKBinaryErodeImage
    {"1d8deea7-c6d0-5fa1-95cb-b14f19df97e8", "abb27e0c-b049-4f60-8355-178d86bb1de4"}, // ITKBinaryMorphologicalClosingImage
    {"704c801a-7549-54c4-9def-c4bb58d07fd1", "61e849cd-6781-4949-9edf-893145404130"}, // ITKBinaryMorphologicalOpeningImage
    {"bd1c2353-0a39-52c0-902b-ee64721994c7", "b288d716-5763-4461-b020-22c1f6dbf49a"}, // ITKBinaryOpeningByReconstructionImage
    {"606c3700-f793-5852-9a0f-3123bd212447", "f0568385-071f-4667-9ce1-0b7e497836d7"}, // ITKBinaryProjectionImage
    {"dcceeb50-5924-5eae-88ea-34793cf545a9", "8c9d384f-2900-45b8-85a4-4b2af64a79f4"}, // ITKBinaryThinningImage
    {"4f51765f-ee36-52d0-96b6-f2aca3c24e7c", "cb5c5a3c-7cf8-48e4-8d50-bbd2566bd055"}, // ITKBinomialBlurImage
    {"e26e7359-f72c-5924-b42e-dd5dd454a794", "79427abf-1d77-423b-8985-85d3be32926a"}, // ITKBlackTopHatImage
    {"17f60a64-de93-59aa-a5e2-063e0aadafb7", "5da5d0a4-35da-42e9-8604-7f318e2713fa"}, // ITKBoundedReciprocalImage
    {"bf554dd5-a927-5969-b651-1c47d386afce", "57f68c9b-d2c4-4241-8b10-9b2c46b37797"}, // ITKConnectedComponentImage
    {"009fb2d0-6f65-5406-bb2a-4a883d0bc18c", "ae839936-b2a1-4911-911b-54d2e18d873f"}, // ITKCurvatureAnisotropicDiffusionImage
    {"653f26dd-a5ef-5c75-b6f6-bc096268f25e", "3f47b7a4-dfc0-4f3c-b696-ce5dc50f536f"}, // ITKCurvatureFlowImage
    {"53d5b289-a716-559b-89d9-5ebb34f714ca", "4e761f2f-c14c-4309-9610-cb140f352cd1"}, // ITKDanielssonDistanceMapImage
    {"dbf29c6d-461c-55e7-a6c4-56477d9da55b", "559a01c2-7202-43db-9a5c-5b82c4592985"}, // ITKDilateObjectMorphologyImage
    {"7fcfa231-519e-58da-bf8f-ad0f728bf0fe", "3b0dcf6b-6e81-44ff-876d-2430461809cd"}, // ITKDoubleThresholdImage
    {"caea0698-4253-518b-ab3f-8ebc140d92ea", "b8fd081c-e3ff-4559-b5fa-75d25f713d26"}, // ITKErodeObjectMorphologyImage
    {"a6fb3f3a-6c7a-5dfc-a4f1-75ff1d62c32f", "a07e0cd3-1601-40df-9d2c-55011ba2b618"}, // ITKExpImage
    {"634c2306-c1ee-5a45-a55c-f8286e36999a", "d928e692-3cb8-4026-a576-ec5724d42ac3"}, // ITKExpNegativeImage
    {"98d0b50b-9639-53a0-9e30-2fae6f7ab869", "faf31fd6-952b-4846-aa5c-78a599e6bda4"}, // ITKGradientAnisotropicDiffusionImage
    {"66cec151-2950-51f8-8a02-47d3516d8721", "798dc1cf-fe06-4702-a87c-29ea45481d7d"}, // ITKGrayscaleDilateImage
    {"aef4e804-3f7a-5dc0-911c-b1f16a393a69", "fdf31421-7081-48fc-8cc0-3b2020428fad"}, // ITKGrayscaleErodeImage
    {"d910551f-4eec-55c9-b0ce-69c2277e61bd", "d4cccf34-ad72-4b5e-a805-e5d6f0a6b2e6"}, // ITKGrayscaleGrindPeakImage
    {"849a1903-5595-5029-bbde-6f4b68b2a25c", "4c95856b-5d79-47d7-b8d7-9e5cad00a98d"}, // ITKGrayscaleMorphologicalClosingImage
    {"c88ac42b-9477-5088-9ec0-862af1e0bb56", "fdcb0dd8-9103-4f9d-920e-9f8059d22fa0"}, // ITKGrayscaleMorphologicalOpeningImage
    {"8bc34707-04c0-5e83-8583-48ee19306a1d", "691d1953-f161-417c-9c4e-207ddf63d6aa"}, // ITKHConvexImage
    {"932a6df4-212e-53a1-a2ab-c29bd376bb7b", "fc8c9fb6-b297-4b30-907f-14b5d5c2b336"}, // ITKHMaximaImage
    {"f1d7cf59-9b7c-53cb-b71a-76cf91c86e8f", "2aed53ed-3875-4230-aeb8-448cc924c080"}, // ITKHMinimaImage
    {"4faf4c59-6f29-53af-bc78-5aecffce0e37", "c291a322-f9be-4675-a24d-dfcc20f41eba"}, // ITKIntensityWindowingImage
    {"e9952e0e-97e4-53aa-8c6c-115fd18f0b89", "fc68ef8d-ab37-4b1d-b336-0a9b12fb81e3"}, // ITKIsoContourDistanceImage
    {"668f0b90-b504-5fba-b648-7c9677e1f452", "c681e527-0e80-4ec6-af1f-fea84cb6e454"}, // ITKLabelContourImage
    {"9677659d-b08c-58a4-ac4d-fba7d9093454", "05718743-53e8-4992-afda-6ac521ab687a"}, // ITKLaplacianRecursiveGaussianImage
    {"c4963181-c788-5efc-8560-d005a5e01eea", "25e0f394-0a80-4401-9560-9eae84b00c1b"}, // ITKLaplacianSharpeningImage
    {"b2cb7ad7-6f62-51c4-943d-54d19c64e7be", "26f6de2f-2bac-4b82-a2cd-318e0198b620"}, // ITKMaximumProjectionImage
    {"85c061bc-3ad7-5abc-8fc7-140678311706", "ffc5fe81-f046-4bab-ba35-48db20b9e0e4"}, // ITKMeanProjectionImage
    {"1a7e8483-5838-585c-8d71-e79f836133c4", "dadf3a63-73ca-44f2-bb1b-6de5680e42ae"}, // ITKMedianProjectionImage
    {"bd9bdf46-a229-544a-b158-151920261a63", "cc1f2f12-dfd4-4d25-b269-0c08a088b290"}, // ITKMinMaxCurvatureFlowImage
    {"6394a737-4a31-5593-9bb5-28492129ccf7", "dbb9aa27-1f2b-451b-9874-934c8146dd8f"}, // ITKMinimumProjectionImage
    {"12c83608-c4c5-5c72-b22f-a7696e3f5448", "bf889097-f4ad-47f1-b297-432fb818cf15"}, // ITKMorphologicalGradientImage
    {"81308863-094b-511d-9aa9-865e02e2b8d5", "8082ca87-790e-417f-92dd-7df187765d4d"}, // ITKMorphologicalWatershedFromMarkersImage
    {"001dd629-7032-56a9-99ec-ffaf2caf2ab0", "f8e0029c-886b-4c51-9b13-2937602e37d3"}, // ITKNormalizeToConstantImage
    {"c8362fb9-d3ab-55c0-902b-274cc27d9bb8", "6ec430f4-cadf-40db-a1db-a6272e6b4ad7"}, // ITKNotImage
    {"9af89118-2d15-54ca-9590-75df8be33317", "7471c31b-cbab-45ed-8173-5a229d6e035d"}, // ITKRegionalMaximaImage
    {"f8ed09ae-1f84-5668-a4ad-d770e264f01e", "23611690-605e-4d8a-8090-f13d1879fa94"}, // ITKRegionalMinimaImage
    {"4398d76d-c9aa-5161-bb48-92dd9daaa352", "3f971837-a967-40b6-b3dd-f5c2bea5fe19"}, // ITKRelabelComponentImage
    {"602c270d-ec55-580c-9108-785ba8530964", "90e69ba1-5480-49af-835d-ae2082a70247"}, // ITKSaltAndPepperNoiseImage
    {"31d325fa-e605-5415-85ab-ab93e8cbf32f", "80acb013-503c-4e9d-a8c0-98f9f1b5ede2"}, // ITKShiftScaleImage
    {"97f20f54-276b-54f3-87c9-5eaf16e6c4df", "db92bb50-1e38-4c7c-ad8f-15654f224eb2"}, // ITKShotNoiseImage
    {"e6675be7-e98d-5e0f-a088-ba15cc301038", "78e8203e-e46e-4600-89d8-1546a7b075b2"}, // ITKSigmoidImage
    {"fc192fa8-b6b7-539c-9763-f683724da626", "35bc9242-6594-4dee-8527-06ef3571f243"}, // ITKSignedDanielssonDistanceMapImage
    {"0fd06492-06b1-5044-964c-e0555c556327", "0ba7776e-0ba6-4baa-8b2b-5368083db51a"}, // ITKSmoothingRecursiveGaussianImage
    {"764085a4-6ecb-5fb7-891d-2fda208ba5d8", "bf5e8e22-2caa-4ef5-9f2c-a45b7cc279c1"}, // ITKSpeckleNoiseImage
    {"f092420e-14a0-5dc0-91f8-de0082103aef", "a212bd72-b688-4550-9064-2c1af5e74d74"}, // ITKSquareImage
    {"89b327a7-c6a0-5965-b8aa-9d8bfcedcc76", "32e157de-c864-49c9-adec-5dc1d9496690"}, // ITKStandardDeviationProjectionImage
    {"40714670-b3bd-554c-badb-787d8bab7568", "1649703a-0e21-4dc8-96f6-a683114fbc7d"}, // ITKSumProjectionImage
    {"5845ee06-5c8a-5a74-80fb-c820bd8dfb75", "1d73ff42-d952-451f-9394-28304db2ae46"}, // ITKThresholdImage
    {"2a531add-79fd-5c07-98a6-f875d2ecef4e", "97008e55-96cc-49ea-98ff-768c0a964145"}, // ITKThresholdMaximumConnectedComponentsImage
    {"10aff542-81c5-5f09-9797-c7171c40b6a0", "5afeb9d9-9d6f-46e6-a6a5-ad3efa5e21a5"}, // ITKValuedRegionalMaximaImage
    {"739a0908-cb60-50f7-a484-b2157d023093", "8aa6b919-f403-416a-8eea-894d71ea8b64"}, // ITKValuedRegionalMinimaImage
    {"02e059f7-8055-52b4-9d48-915b67d1e39a", "d9768249-608c-4e68-acf7-697db263a362"}, // ITKWhiteTopHatImage
    {"0259fa1a-4706-5df1-8418-95ffc7b932dd", "a182975d-5371-4360-bb94-a7686f1f0f4c"}, // ITKZeroCrossingImage
    {"886f8b46-51b6-5682-a289-6febd10b7ef0", "b83f9bae-9ccf-4932-96c3-7f2fdb091452"}, // AlignSectionsFeatureCentroid
    {"289f0d8c-29ab-5fbc-91bd-08aac01e37c5", "c6b6d9e5-301d-4767-abf7-530f5ef5007d"}, // CalculateArrayHistogram
    {"eb5a89c4-4e71-59b1-9719-d10a652d961e", "d938a2aa-fee2-4db9-aa2f-2c34a9736580"}, // ConvertColorToGrayScale
    {"f4ba5fa4-bb5c-5dd1-9429-0dd86d0ecb37", "6dc586cc-59fb-4ee8-90ff-2d3587da12f5"}, // ConvertData
    {"9ac220b9-14f9-581a-9bac-5714467589cc", "24768170-5b90-4a9d-82ac-9aeecd9f892e"}, // CreateGeometry
    {"3adfe077-c3c9-5cd0-ad74-cf5f8ff3d254", "7f2f7378-580e-4337-8c04-a29e7883db0b"}, // ErodeDilateBadData
    {"737b8d5a-8622-50f9-9a8a-bfdb57608891", "d734293f-3017-4699-b458-c05aca078b96"}, // FeatureDataCSVWriter
    {"30ae0a1e-3d94-5dab-b279-c5727ab5d7ff", "a59eb864-9e6b-40bb-9292-e5281b0b4f3e"}, // FillBadData
    {"933e4b2d-dd61-51c3-98be-00548ba783a3", "ba9ae8f6-443e-41d3-bb45-a08a139325c1"}, // FindEuclideanDistMap
    {"6f8ca36f-2995-5bd3-8672-6b0b80d5b2ca", "c6875ac7-8bdd-4f69-b6ce-82ac09bd3421"}, // FindFeatureCentroids
    {"697ed3de-db33-5dd1-a64b-04fb71e7d63e", "924c10e3-2f39-4c08-9d7a-7fe029f74f6d"}, // FindNeighborhoods
    {"3b0ababf-9c8d-538d-96af-e40775c4f0ab", "036b17d5-23bb-4a24-9187-c4a8dd918792"}, // FindShapes
    {"5d586366-6b59-566e-8de1-57aa9ae8a91c", "94e83e4f-797d-4594-b130-3819b7676f01"}, // FindSurfaceAreaToVolume
    {"a8463056-3fa7-530b-847f-7f4cb78b8602", "6e8cc6ec-8b9b-402e-9deb-85bd1cdba743"}, // RemoveFlaggedFeatures
    {"1966e540-759c-5798-ae26-0c6a3abc65c0", "9783ea2c-4cf7-46de-ab21-b40d91a48c5b"}, // ResampleImageGeom
    {"e25d9b4c-2b37-578c-b1de-cf7032b5ef19", "d2451dc1-a5a1-4ac2-a64d-7991669dcffc"}, // RotateSampleRefFrame
    {"5ecf77f4-a38a-52ab-b4f6-0fb8a9c5cb9c", "55da791f-4d1c-4413-8673-742f27d2b22b"}, // SplitAttributeArray
    {"5fbf9204-2c6c-597b-856a-f4612adbac38", "06c8bfe8-2b42-4956-aca3-580bc0620716"}, // WriteASCIIData
    {"ce1ee404-0336-536c-8aad-f9641c9458be", "87fa9e07-6c66-45b0-80a0-cf80cc0def5d"}, // AlignGeometries
    {"c681caf4-22f2-5885-bbc9-a0476abc72eb", "f5bbc16b-3426-4ae0-b27b-ba7862dc40fe"}, // ApplyTransformationToGeometry
    {"fab669ad-66c6-5a39-bdb7-fc47b94311ed", "c19203b7-2217-4e52-bff4-7f611695421a"}, // ApproximatePointCloudHull
    {"656f144c-a120-5c3b-bee5-06deab438588", "c666ee17-ca58-4969-80d0-819986c72485"}, // FindSizes
    {"a9900cc3-169e-5a1b-bcf4-7569e1950d41", "b149addd-c0c8-4010-a264-596005eaf2a5"}, // TriangleAreaFilter
    {"f7bc0e1e-0f50-5fe0-a9e7-510b6ed83792", "565e06e2-6fd0-4232-89c4-ee672926d565"}, // ChangeAngleRepresentation
    {"a6b50fb0-eb7c-5d9b-9691-825d6a4fe772", "2436b614-e96d-47f0-9f6f-41d6fe97acd4"}, // CombineAttributeArrays
    {"47cafe63-83cc-5826-9521-4fb5bea684ef", "bad9b7bd-1dc9-4f21-a889-6520e7a41881"}, // ConditionalSetValue
    {"99836b75-144b-5126-b261-b411133b5e8a", "4c8c976a-993d-438b-bd8e-99f71114b9a1"}, // CopyFeatureArrayToElementArray
    {"93375ef0-7367-5372-addc-baa019b1b341", "a6a28355-ee69-4874-bcac-76ed427423ed"}, // CreateAttributeMatrix
    {"77f392fb-c1eb-57da-a1b1-e7acf9239fb8", "67041f9b-bdc6-4122-acc6-c9fe9280e90d"}, // CreateDataArray
    {"816fbe6b-7c38-581b-b149-3f839fb65b93", "e7d2f9b8-4131-4b28-a843-ea3c6950f101"}, // CreateDataContainer
    {"94438019-21bb-5b61-a7c3-66974b9a34dc", "50e1be47-b027-4f40-8f70-1283682ee3e7"}, // CreateFeatureArrayFromElementArray
    {"f2132744-3abb-5d66-9cd9-c9a233b5c4aa", "c4320659-1a84-461d-939e-c7c10229a504"}, // CreateImageGeometry
    {"baa4b7fe-31e5-5e63-a2cb-0bb9d844cfaf", "e6476737-4aa7-48ba-a702-3dfab82c96e2"}, // CropImageGeometry
    {"f28cbf07-f15a-53ca-8c7f-b41a11dae6cc", "8b16452f-f75e-4918-9460-d3914fdc0d08"}, // CropVertexGeometry
    {"7b1c8f46-90dd-584a-b3ba-34e16958a7d0", "bf286740-e987-49fe-a7c8-6e566e3a0606"}, // RemoveArrays
    {"3fcd4c43-9d75-5b86-aad4-4441bc914f37", "b3a95784-2ced-41ec-8d3d-0242ac130003"}, // DataContainerWriter
    {"52a069b4-6a46-5810-b0ec-e0693c636034", "e020f76f-a77f-4999-8bf1-9b7529f06d0a"}, // ExtractInternalSurfacesFromTriangleGeometry
    {"bf35f515-294b-55ed-8c69-211b7e69cb56", "645ecae2-cb30-4b53-8165-c9857dfa754f"}, // FindArrayStatistics
    {"29086169-20ce-52dc-b13e-824694d759aa", "5a0ee5b5-c135-4535-85d0-9c2058943099"}, // FindDifferenceMap
    {"6334ce16-cea5-5643-83b5-9573805873fa", "da5bb20e-4a8e-49d9-9434-fbab7bc434fc"}, // FindFeaturePhases
    {"73ee33b6-7622-5004-8b88-4d145514fb6a", "270a824e-414b-455e-bb7e-b38a0848990d"}, // FindNeighborListStatistics
    {"97cf66f8-7a9b-5ec2-83eb-f8c4c8a17bac", "7177e88c-c3ab-4169-abe9-1fdaff20e598"}, // FindNeighbors
    {"d2b0ae3d-686a-5dc0-a844-66bc0dc8f3cb", "0893e490-5d24-4c21-95e7-e8372baa8948"}, // FindSurfaceFeatures
    {"0e8c0818-a3fb-57d4-a5c8-7cb8ae54a40a", "94d47495-5a89-4c7f-a0ee-5ff20e6bd273"}, // IdentifySample
    {"bdb978bc-96bf-5498-972c-b509c38b8d50", "373be1f8-31cf-49f6-aa5d-e356f4f3f261"}, // ReadASCIIData
    {"043cbde5-3878-5718-958f-ae75714df0df", "0dbd31c7-19e0-4077-83ef-f4a6459a0e2d"}, // DataContainerReader
    {"9e98c3b0-5707-5a3b-b8b5-23ef83b02896", "8027f145-c7d5-4589-900e-b909fb3a059c"}, // ImportHDF5Dataset
    {"a7007472-29e5-5d0a-89a6-1aed11b603f8", "25f7df3e-ca3e-4634-adda-732c0e56efd4"}, // ImportAsciDataArray
    {"dfab9921-fea3-521c-99ba-48db98e43ff8", "447b8909-661f-446a-8c1f-72e0cb568fcf"}, // InitializeData
    {"4b551c15-418d-5081-be3f-d3aeb62408e5", "996c4464-08f0-4268-a8a6-f9796c88cf58"}, // InterpolatePointCloudToRegularGrid
    {"6c8fb24b-5b12-551c-ba6d-ae2fa7724764", "3a206668-fa44-418d-b78e-9fd803b8fb98"}, // IterativeClosestPoint
    {"601c4885-c218-5da6-9fc7-519d85d241ad", "0dd0916e-9305-4a7b-98cf-a6cfb97cb501"}, // LaplacianSmoothing
    {"9fe34deb-99e1-5f3a-a9cc-e90c655b47ee", "af53ac60-092f-4e4a-9e13-57f0034ce2c7"}, // MapPointCloudToRegularGrid
    {"dab5de3c-5f81-5bb5-8490-73521e1183ea", "4ab5153f-6014-4e6d-bbd6-194068620389"}, // MinNeighbors
    {"fe2cbe09-8ae1-5bea-9397-fd5741091fdb", "651e5894-ab7c-4176-b7f0-ea466c521753"}, // MoveData
    {"014b7300-cf36-5ede-a751-5faf9b119dae", "4246245e-1011-4add-8436-0af6bed19228"}, // MultiThresholdObjects
    {"686d5393-2b02-5c86-b887-dd81a8ae80f2", "4246245e-1011-4add-8436-0af6bed19228"}, // MultiThresholdObjects2
    {"119861c5-e303-537e-b210-2e62936222e9", "ee34ef95-aa04-4ad3-8232-5783a880d279"}, // PointSampleTriangleGeometry
    {"07b49e30-3900-5c34-862a-f1fb48bad568", "13dd00bd-ad49-4e04-95eb-3267952fd6e5"}, // QuickSurfaceMesh
    {"0791f556-3d73-5b1e-b275-db3f7bb6850d", "dd159366-5f12-42db-af6d-a33592ae8a89"}, // RawBinaryReader
    {"379ccc67-16dd-530a-984f-177db2314bac", "46099b4c-ef90-4fb3-b5e9-6c8c543c5be1"}, // RemoveFlaggedVertices
    {"53ac1638-8934-57b8-b8e5-4b91cdda23ec", "074472d3-ba8d-4a1a-99f2-2d56a0d082a0"}, // MinSize
    {"53a5f731-2858-5e3e-bd43-8f2cf45d90ec", "911a3aa9-d3c2-4f66-9451-8861c4b726d5"}, // RenameAttributeArray
    {"ee29e6d6-1f59-551b-9350-a696523261d5", "911a3aa9-d3c2-4f66-9451-8861c4b726d5"}, // RenameAttributeMatrix
    {"d53c808f-004d-5fac-b125-0fffc8cc78d6", "911a3aa9-d3c2-4f66-9451-8861c4b726d5"}, // RenameDataContainer
    {"3062fc2c-76b2-5c50-92b7-edbbb424c41d", "ade392e6-f0da-4cf3-bf11-dfe69e200b49"}, // RobustAutomaticThreshold
    {"2c5edebf-95d8-511f-b787-90ee2adf485c", "e067cd97-9bbf-4c92-89a6-3cb4fdb76c93"}, // ScalarSegmentFeatures
    {"6d3a3852-6251-5d2e-b749-6257fd0d8951", "057bc7fd-c84a-4902-9397-87e51b1b1fe0"}, // SetOriginResolutionImageGeom
    {"980c7bfd-20b2-5711-bc3b-0190b9096c34", "2f64bd45-9d28-4254-9e07-6aa7c6d3d015"}, // ReadStlFile
    {"0541c5eb-1976-5797-9468-be50a93d44e2", "dd42c521-4ae5-485d-ad35-d1276547d2f1"}, // TriangleDihedralAngleFilter
    {"8133d419-1919-4dbf-a5bf-1c97282ba63f", "928154f6-e4bc-5a10-a9dd-1abb6a6c0f6b"}, // TriangleNormalFilter
};

void InitParameterCount()
{
  s_ParameterCount["BooleanFilterParameter"] = 0;
  s_ParameterCount["DataArrayCreationFilterParameter"] = 0;
  s_ParameterCount["DataArraySelectionFilterParameter"] = 0;
  s_ParameterCount["ChoiceFilterParameter"] = 0;
  s_ParameterCount["AttributeMatrixCreationFilterParameter"] = 0;
  s_ParameterCount["AttributeMatrixSelectionFilterParameter"] = 0;
  s_ParameterCount["DataContainerCreationFilterParameter"] = 0;
  s_ParameterCount["DataContainerSelectionFilterParameter"] = 0;
  s_ParameterCount["InputFileFilterParameter"] = 0;
  s_ParameterCount["InputPathFilterParameter"] = 0;
  s_ParameterCount["OutputFileFilterParameter"] = 0;
  s_ParameterCount["OutputPathFilterParameter"] = 0;
  s_ParameterCount["FileListInfoFilterParameter"] = 0;
  s_ParameterCount["FloatFilterParameter"] = 0;
  s_ParameterCount["IntFilterParameter"] = 0;
  s_ParameterCount["DoubleFilterParameter"] = 0;
  s_ParameterCount["UInt64FilterParameter"] = 0;
  s_ParameterCount["AxisAngleFilterParameter"] = 0;
  s_ParameterCount["IntVec2FilterParameter"] = 0;
  s_ParameterCount["IntVec3FilterParameter"] = 0;
  s_ParameterCount["FloatVec2FilterParameter"] = 0;
  s_ParameterCount["FloatVec3FilterParameter"] = 0;
  s_ParameterCount["NumericTypeFilterParameter"] = 0;
  s_ParameterCount["StringFilterParameter"] = 0;
  s_ParameterCount["SeparatorFilterParameter"] = 0;
  s_ParameterCount["LinkedDataContainerSelectionFilterParameter"] = 0;
  s_ParameterCount["LinkedPathCreationFilterParameter"] = 0;
  s_ParameterCount["MultiDataArraySelectionFilterParameter"] = 0;
  s_ParameterCount["LinkedBooleanFilterParameter"] = 0;
  s_ParameterCount["LinkedChoicesFilterParameter"] = 0;
  s_ParameterCount["PreflightUpdatedValueFilterParameter"] = 0;

  // No COMPLEX Type Yet
  s_ParameterCount["CalculatorFilterParameter"] = 0;
  s_ParameterCount["ComparisonSelectionAdvancedFilterParameter"] = 0;
  s_ParameterCount["ComparisonSelectionFilterParameter"] = 0;
  s_ParameterCount["ConvertHexGridToSquareGridFilterParameter"] = 0;
  s_ParameterCount["DataContainerArrayProxyFilterParameter"] = 0;
  s_ParameterCount["DataContainerReaderFilterParameter"] = 0;
  s_ParameterCount["DynamicChoiceFilterParameter"] = 0;
  s_ParameterCount["DynamicTableFilterParameter"] = 0;
  s_ParameterCount["EMMPMFilterParameter"] = 0;
  s_ParameterCount["EbsdMontageImportFilterParameter"] = 0;
  s_ParameterCount["EbsdToH5EbsdFilterParameter"] = 0;
  s_ParameterCount["EbsdWarpPolynomialFilterParameter"] = 0;
  s_ParameterCount["EnsembleInfoFilterParameter"] = 0;
  s_ParameterCount["SecondOrderPolynomialFilterParameter"] = 0;
  s_ParameterCount["ThirdOrderPolynomialFilterParameter"] = 0;
  s_ParameterCount["FourthOrderPolynomialFilterParameter"] = 0;
  s_ParameterCount["GenerateColorTableFilterParameter"] = 0;
  s_ParameterCount["ImportHDF5DatasetFilterParameter"] = 0;
  s_ParameterCount["ImportVectorImageStackFilterParameter"] = 0;
  s_ParameterCount["KbrRecisConfigFilterParameter"] = 0;
  s_ParameterCount["MontageSelectionFilterParameter"] = 0;
  s_ParameterCount["MontageStructureSelectionFilterParameter"] = 0;
  s_ParameterCount["MultiAttributeMatrixSelectionFilterParameter"] = 0;
  s_ParameterCount["MultiDataContainerSelectionFilterParameter"] = 0;
  s_ParameterCount["MultiInputFileFilterParameter"] = 0;
  s_ParameterCount["OEMEbsdScanSelectionFilterParameter"] = 0;
  s_ParameterCount["OrientationUtilityFilterParameter"] = 0;
  s_ParameterCount["ParagraphFilterParameter"] = 0;
  s_ParameterCount["PhaseTypeSelectionFilterParameter"] = 0;
  s_ParameterCount["RangeFilterParameter"] = 0;
  s_ParameterCount["ReadASCIIDataFilterParameter"] = 0;
  s_ParameterCount["ReadH5EbsdFilterParameter"] = 0;
  s_ParameterCount["ScalarTypeFilterParameter"] = 0;
  s_ParameterCount["ShapeTypeSelectionFilterParameter"] = 0;
  s_ParameterCount["StatsGeneratorFilterParameter"] = 0;
  s_ParameterCount["Symmetric6x6FilterParameter"] = 0;
}

void InitParameterTypeMapping()
{
  s_ParameterTypeMapping["BooleanFilterParameter"] = "bool";
  s_ParameterTypeMapping["DataArrayCreationFilterParameter"] = "DataPath";
  s_ParameterTypeMapping["DataArraySelectionFilterParameter"] = "DataPath";
  s_ParameterTypeMapping["ChoiceFilterParameter"] = "ChoicesParameter::ValueType";
  s_ParameterTypeMapping["AttributeMatrixCreationFilterParameter"] = "DataPath";
  s_ParameterTypeMapping["AttributeMatrixSelectionFilterParameter"] = "DataPath";
  s_ParameterTypeMapping["DataContainerCreationFilterParameter"] = "DataPath";
  s_ParameterTypeMapping["DataContainerSelectionFilterParameter"] = "DataPath";
  s_ParameterTypeMapping["InputFileFilterParameter"] = "FileSystemPathParameter::ValueType";
  s_ParameterTypeMapping["InputPathFilterParameter"] = "FileSystemPathParameter::ValueType";
  s_ParameterTypeMapping["OutputFileFilterParameter"] = "FileSystemPathParameter::ValueType";
  s_ParameterTypeMapping["OutputPathFilterParameter"] = "FileSystemPathParameter::ValueType";
  s_ParameterTypeMapping["FileListInfoFilterParameter"] = "GeneratedFileListParameter::ValueType";
  s_ParameterTypeMapping["FloatFilterParameter"] = "float32";
  s_ParameterTypeMapping["IntFilterParameter"] = "int32";
  s_ParameterTypeMapping["DoubleFilterParameter"] = "float64";
  s_ParameterTypeMapping["UInt64FilterParameter"] = "uint64";
  s_ParameterTypeMapping["AxisAngleFilterParameter"] = "VectorFloat32Parameter::ValueType";
  s_ParameterTypeMapping["IntVec2FilterParameter"] = "VectorInt32Parameter::ValueType";
  s_ParameterTypeMapping["IntVec3FilterParameter"] = "VectorInt32Parameter::ValueType";
  s_ParameterTypeMapping["FloatVec2FilterParameter"] = "VectorFloat32Parameter::ValueType";
  s_ParameterTypeMapping["FloatVec3FilterParameter"] = "VectorFloat32Parameter::ValueType";
  s_ParameterTypeMapping["NumericTypeFilterParameter"] = "NumericType";
  s_ParameterTypeMapping["StringFilterParameter"] = "StringParameter::ValueType";
  s_ParameterTypeMapping["SeparatorFilterParameter"] = "{}";
  s_ParameterTypeMapping["LinkedDataContainerSelectionFilterParameter"] = "DataPath";
  s_ParameterTypeMapping["LinkedPathCreationFilterParameter"] = "DataPath";
  s_ParameterTypeMapping["MultiDataArraySelectionFilterParameter"] = "MultiArraySelectionParameter::ValueType";
  s_ParameterTypeMapping["LinkedBooleanFilterParameter"] = "bool";
  s_ParameterTypeMapping["LinkedChoicesFilterParameter"] = "ChoicesParameter::ValueType";
  s_ParameterTypeMapping["PreflightUpdatedValueFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";

  // No COMPLEX Type Yet
  s_ParameterTypeMapping["CalculatorFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["ComparisonSelectionAdvancedFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["ComparisonSelectionFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["ConvertHexGridToSquareGridFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["DataContainerArrayProxyFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["DataContainerReaderFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["DynamicChoiceFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["DynamicTableFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["EMMPMFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["EbsdMontageImportFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["EbsdToH5EbsdFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["EbsdWarpPolynomialFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["EnsembleInfoFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["SecondOrderPolynomialFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["ThirdOrderPolynomialFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["FourthOrderPolynomialFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["GenerateColorTableFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["ImportHDF5DatasetFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["ImportVectorImageStackFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["KbrRecisConfigFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["MontageSelectionFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["MontageStructureSelectionFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["MultiAttributeMatrixSelectionFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["MultiDataContainerSelectionFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["MultiInputFileFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["OEMEbsdScanSelectionFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["OrientationUtilityFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["ParagraphFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["PhaseTypeSelectionFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["RangeFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["ReadASCIIDataFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["ReadH5EbsdFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["ScalarTypeFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["ShapeTypeSelectionFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["StatsGeneratorFilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
  s_ParameterTypeMapping["Symmetric6x6FilterParameter"] = "<<<NOT_IMPLEMENTED>>>";
}

void InitDefaultParameterConstructor()
{
  s_DefaultConstructorMapping["BooleanFilterParameter"] = "false";
  s_DefaultConstructorMapping["DataArrayCreationFilterParameter"] = "DataPath{}";
  s_DefaultConstructorMapping["DataArraySelectionFilterParameter"] = "DataPath{}, complex::GetAllDataTypes() /* This will allow ANY data type. Adjust as necessary for your filter*/";
  s_DefaultConstructorMapping["ChoiceFilterParameter"] = "0, ChoicesParameter::Choices{\"Option 1\", \"Option 2\", \"Option 3\"}/* Change this to the proper choices */";
  s_DefaultConstructorMapping["AttributeMatrixCreationFilterParameter"] = "DataPath{}";
  s_DefaultConstructorMapping["AttributeMatrixSelectionFilterParameter"] = "DataPath{}";
  s_DefaultConstructorMapping["DataContainerCreationFilterParameter"] = "DataPath{}";
  s_DefaultConstructorMapping["DataContainerSelectionFilterParameter"] = "DataPath{}";
  s_DefaultConstructorMapping["InputFileFilterParameter"] = "fs::path(\"DefaultInputFileName\"), FileSystemPathParameter::ExtensionsType{\"*.*\"}, FileSystemPathParameter::PathType::InputFile";
  s_DefaultConstructorMapping["InputPathFilterParameter"] = "fs::path(\"<default file to read goes here>\"), FileSystemPathParameter::ExtensionsType{}, FileSystemPathParameter::PathType::InputDir";
  s_DefaultConstructorMapping["OutputFileFilterParameter"] = "fs::path(\"<default file to read goes here>\"), FileSystemPathParameter::ExtensionsType{}, FileSystemPathParameter::PathType::OutputFile";
  s_DefaultConstructorMapping["OutputPathFilterParameter"] = "fs::path(\"<default file to read goes here>\"), FileSystemPathParameter::ExtensionsType{}, FileSystemPathParameter::PathType::OutputDir";
  s_DefaultConstructorMapping["FileListInfoFilterParameter"] = "GeneratedFileListParameter::ValueType{}";
  s_DefaultConstructorMapping["FloatFilterParameter"] = "1.23345f";
  s_DefaultConstructorMapping["IntFilterParameter"] = "1234356";
  s_DefaultConstructorMapping["DoubleFilterParameter"] = "2.3456789";
  s_DefaultConstructorMapping["UInt64FilterParameter"] = "13412341234212";
  s_DefaultConstructorMapping["AxisAngleFilterParameter"] = "std::vector<float32>(4), std::vector<std::string>(4)";
  s_DefaultConstructorMapping["IntVec2FilterParameter"] = "std::vector<int32>(2), std::vector<std::string>(2)";
  s_DefaultConstructorMapping["IntVec3FilterParameter"] = "std::vector<int32>(3), std::vector<std::string>(3)";
  s_DefaultConstructorMapping["FloatVec2FilterParameter"] = "std::vector<float32>(2), std::vector<std::string>(2)";
  s_DefaultConstructorMapping["FloatVec3FilterParameter"] = "std::vector<float32>(3), std::vector<std::string>(3)";
  s_DefaultConstructorMapping["NumericTypeFilterParameter"] = "NumericType::int8";
  s_DefaultConstructorMapping["StringFilterParameter"] = "\"SomeString\"";
  s_DefaultConstructorMapping["SeparatorFilterParameter"] = "\"Separator\"";
  s_DefaultConstructorMapping["LinkedDataContainerSelectionFilterParameter"] = "DataPath{}";
  s_DefaultConstructorMapping["LinkedPathCreationFilterParameter"] = "DataPath{}";
  s_DefaultConstructorMapping["MultiDataArraySelectionFilterParameter"] = "MultiArraySelectionParameter::ValueType {DataPath(), DataPath(), DataPath()}";
  s_DefaultConstructorMapping["LinkedBooleanFilterParameter"] = "false";
  s_DefaultConstructorMapping["LinkedChoicesFilterParameter"] = "0, ChoicesParameter::Choices{\"Option 1\", \"Option 2\", \"Option 3\"}";
  s_DefaultConstructorMapping["PreflightUpdatedValueFilterParameter"] = "{}";

  // No COMPLEX Type Yet
  s_DefaultConstructorMapping["CalculatorFilterParameter"] = "{}";
  s_DefaultConstructorMapping["ComparisonSelectionAdvancedFilterParameter"] = "{}";
  s_DefaultConstructorMapping["ComparisonSelectionFilterParameter"] = "{}";
  s_DefaultConstructorMapping["ConvertHexGridToSquareGridFilterParameter"] = "{}";
  s_DefaultConstructorMapping["DataContainerArrayProxyFilterParameter"] = "{}";
  s_DefaultConstructorMapping["DataContainerReaderFilterParameter"] = "{}";
  s_DefaultConstructorMapping["DynamicChoiceFilterParameter"] = "{}";
  s_DefaultConstructorMapping["DynamicTableFilterParameter"] = "{}";
  s_DefaultConstructorMapping["EMMPMFilterParameter"] = "{}";
  s_DefaultConstructorMapping["EbsdMontageImportFilterParameter"] = "{}";
  s_DefaultConstructorMapping["EbsdToH5EbsdFilterParameter"] = "{}";
  s_DefaultConstructorMapping["EbsdWarpPolynomialFilterParameter"] = "{}";
  s_DefaultConstructorMapping["EnsembleInfoFilterParameter"] = "{}";
  s_DefaultConstructorMapping["SecondOrderPolynomialFilterParameter"] = "{}";
  s_DefaultConstructorMapping["ThirdOrderPolynomialFilterParameter"] = "{}";
  s_DefaultConstructorMapping["FourthOrderPolynomialFilterParameter"] = "{}";
  s_DefaultConstructorMapping["GenerateColorTableFilterParameter"] = "{}";
  s_DefaultConstructorMapping["ImportHDF5DatasetFilterParameter"] = "{}";
  s_DefaultConstructorMapping["ImportVectorImageStackFilterParameter"] = "{}";
  s_DefaultConstructorMapping["KbrRecisConfigFilterParameter"] = "{}";
  s_DefaultConstructorMapping["MontageSelectionFilterParameter"] = "{}";
  s_DefaultConstructorMapping["MontageStructureSelectionFilterParameter"] = "{}";
  s_DefaultConstructorMapping["MultiAttributeMatrixSelectionFilterParameter"] = "{}";
  s_DefaultConstructorMapping["MultiDataContainerSelectionFilterParameter"] = "{}";
  s_DefaultConstructorMapping["MultiInputFileFilterParameter"] = "{}";
  s_DefaultConstructorMapping["OEMEbsdScanSelectionFilterParameter"] = "{}";
  s_DefaultConstructorMapping["OrientationUtilityFilterParameter"] = "{}";
  s_DefaultConstructorMapping["ParagraphFilterParameter"] = "{}";
  s_DefaultConstructorMapping["PhaseTypeSelectionFilterParameter"] = "{}";
  s_DefaultConstructorMapping["RangeFilterParameter"] = "{}";
  s_DefaultConstructorMapping["ReadASCIIDataFilterParameter"] = "{}";
  s_DefaultConstructorMapping["ReadH5EbsdFilterParameter"] = "{}";
  s_DefaultConstructorMapping["ScalarTypeFilterParameter"] = "{}";
  s_DefaultConstructorMapping["ShapeTypeSelectionFilterParameter"] = "{}";
  s_DefaultConstructorMapping["StatsGeneratorFilterParameter"] = "{}";
  s_DefaultConstructorMapping["Symmetric6x6FilterParameter"] = "{}";
}

void InitUnitTestParameterConstruction()
{
  s_UnitTestDefaultValue["BooleanFilterParameter"] = "false";
  s_UnitTestDefaultValue["DataArrayCreationFilterParameter"] = "DataPath{}";
  s_UnitTestDefaultValue["DataArraySelectionFilterParameter"] = "DataPath{}";
  s_UnitTestDefaultValue["ChoiceFilterParameter"] = "0";
  s_UnitTestDefaultValue["AttributeMatrixCreationFilterParameter"] = "DataPath{}";
  s_UnitTestDefaultValue["AttributeMatrixSelectionFilterParameter"] = "DataPath{}";
  s_UnitTestDefaultValue["DataContainerCreationFilterParameter"] = "DataPath{}";
  s_UnitTestDefaultValue["DataContainerSelectionFilterParameter"] = "DataPath{}";
  s_UnitTestDefaultValue["InputFileFilterParameter"] = "fs::path(\"/Path/To/Input/File/To/Read.data\")";
  s_UnitTestDefaultValue["InputPathFilterParameter"] = "fs::path(\"/Path/To/Input/Directory/To/Read\")";
  s_UnitTestDefaultValue["OutputFileFilterParameter"] = "fs::path(\"/Path/To/Output/File/To/Write.data\")";
  s_UnitTestDefaultValue["OutputPathFilterParameter"] = "fs::path(\"/Path/To/Output/Directory/To/Read\")";
  s_UnitTestDefaultValue["FileListInfoFilterParameter"] = "GeneratedFileListParameter::ValueType{}";
  s_UnitTestDefaultValue["FloatFilterParameter"] = "1.23345f";
  s_UnitTestDefaultValue["IntFilterParameter"] = "1234356";
  s_UnitTestDefaultValue["DoubleFilterParameter"] = "2.3456789";
  s_UnitTestDefaultValue["UInt64FilterParameter"] = "13412341234212";
  s_UnitTestDefaultValue["AxisAngleFilterParameter"] = "std::vector<float32>(4)";
  s_UnitTestDefaultValue["IntVec2FilterParameter"] = "std::vector<int32>(2)";
  s_UnitTestDefaultValue["IntVec3FilterParameter"] = "std::vector<int32>(3)";
  s_UnitTestDefaultValue["FloatVec2FilterParameter"] = "std::vector<float32>(2)";
  s_UnitTestDefaultValue["FloatVec3FilterParameter"] = "std::vector<float32>(3)";
  s_UnitTestDefaultValue["NumericTypeFilterParameter"] = "NumericType::int8";
  s_UnitTestDefaultValue["StringFilterParameter"] = "\"SomeString\"";
  s_UnitTestDefaultValue["SeparatorFilterParameter"] = "\"Separator\"";
  s_UnitTestDefaultValue["LinkedDataContainerSelectionFilterParameter"] = "DataPath{}";
  s_UnitTestDefaultValue["LinkedPathCreationFilterParameter"] = "DataPath{}";
  s_UnitTestDefaultValue["MultiDataArraySelectionFilterParameter"] = "MultiArraySelectionParameter::ValueType {DataPath(), DataPath(), DataPath()}";
  s_UnitTestDefaultValue["LinkedBooleanFilterParameter"] = "false";
  s_UnitTestDefaultValue["LinkedChoicesFilterParameter"] = "0";
  s_UnitTestDefaultValue["PreflightUpdatedValueFilterParameter"] = "{{\"key\", \"Value\"}}";

  // No COMPLEX Type Yet
  s_UnitTestDefaultValue["CalculatorFilterParameter"] = "{}";
  s_UnitTestDefaultValue["ComparisonSelectionAdvancedFilterParameter"] = "{}";
  s_UnitTestDefaultValue["ComparisonSelectionFilterParameter"] = "{}";
  s_UnitTestDefaultValue["ConvertHexGridToSquareGridFilterParameter"] = "{}";
  s_UnitTestDefaultValue["DataContainerArrayProxyFilterParameter"] = "{}";
  s_UnitTestDefaultValue["DataContainerReaderFilterParameter"] = "{}";
  s_UnitTestDefaultValue["DynamicChoiceFilterParameter"] = "{}";
  s_UnitTestDefaultValue["DynamicTableFilterParameter"] = "{}";
  s_UnitTestDefaultValue["EMMPMFilterParameter"] = "{}";
  s_UnitTestDefaultValue["EbsdMontageImportFilterParameter"] = "{}";
  s_UnitTestDefaultValue["EbsdToH5EbsdFilterParameter"] = "{}";
  s_UnitTestDefaultValue["EbsdWarpPolynomialFilterParameter"] = "{}";
  s_UnitTestDefaultValue["EnsembleInfoFilterParameter"] = "{}";
  s_UnitTestDefaultValue["SecondOrderPolynomialFilterParameter"] = "{}";
  s_UnitTestDefaultValue["ThirdOrderPolynomialFilterParameter"] = "{}";
  s_UnitTestDefaultValue["FourthOrderPolynomialFilterParameter"] = "{}";
  s_UnitTestDefaultValue["GenerateColorTableFilterParameter"] = "{}";
  s_UnitTestDefaultValue["ImportHDF5DatasetFilterParameter"] = "{}";
  s_UnitTestDefaultValue["ImportVectorImageStackFilterParameter"] = "{}";
  s_UnitTestDefaultValue["KbrRecisConfigFilterParameter"] = "{}";
  s_UnitTestDefaultValue["MontageSelectionFilterParameter"] = "{}";
  s_UnitTestDefaultValue["MontageStructureSelectionFilterParameter"] = "{}";
  s_UnitTestDefaultValue["MultiAttributeMatrixSelectionFilterParameter"] = "{}";
  s_UnitTestDefaultValue["MultiDataContainerSelectionFilterParameter"] = "{}";
  s_UnitTestDefaultValue["MultiInputFileFilterParameter"] = "{}";
  s_UnitTestDefaultValue["OEMEbsdScanSelectionFilterParameter"] = "{}";
  s_UnitTestDefaultValue["OrientationUtilityFilterParameter"] = "{}";
  s_UnitTestDefaultValue["ParagraphFilterParameter"] = "{}";
  s_UnitTestDefaultValue["PhaseTypeSelectionFilterParameter"] = "{}";

  s_UnitTestDefaultValue["RangeFilterParameter"] = "{}";
  s_UnitTestDefaultValue["ReadASCIIDataFilterParameter"] = "{}";
  s_UnitTestDefaultValue["ReadH5EbsdFilterParameter"] = "{}";
  s_UnitTestDefaultValue["ScalarTypeFilterParameter"] = "{}";
  s_UnitTestDefaultValue["ShapeTypeSelectionFilterParameter"] = "{}";
  s_UnitTestDefaultValue["StatsGeneratorFilterParameter"] = "{}";
  s_UnitTestDefaultValue["Symmetric6x6FilterParameter"] = "{}";
}

void InitIncludeMapping()
{
  s_InlcudeMapping["BooleanFilterParameter"] = "BoolParameter";
  s_InlcudeMapping["DataArrayCreationFilterParameter"] = "ArrayCreationParameter";
  s_InlcudeMapping["DataArraySelectionFilterParameter"] = "ArraySelectionParameter";
  s_InlcudeMapping["ChoiceFilterParameter"] = "ChoicesParameter";
  s_InlcudeMapping["AttributeMatrixCreationFilterParameter"] = "DataGroupCreationParameter";
  s_InlcudeMapping["AttributeMatrixSelectionFilterParameter"] = "DataGroupSelectionParameter";
  s_InlcudeMapping["DataContainerCreationFilterParameter"] = "DataGroupCreationParameter";
  s_InlcudeMapping["DataContainerSelectionFilterParameter"] = "DataGroupSelectionParameter";
  s_InlcudeMapping["InputFileFilterParameter"] = "FileSystemPathParameter";
  s_InlcudeMapping["InputPathFilterParameter"] = "FileSystemPathParameter";
  s_InlcudeMapping["OutputFileFilterParameter"] = "FileSystemPathParameter";
  s_InlcudeMapping["OutputPathFilterParameter"] = "FileSystemPathParameter";
  s_InlcudeMapping["FileListInfoFilterParameter"] = "GeneratedFileListParameter";
  s_InlcudeMapping["FloatFilterParameter"] = "NumberParameter";
  s_InlcudeMapping["IntFilterParameter"] = "NumberParameter";
  s_InlcudeMapping["DoubleFilterParameter"] = "NumberParameter";
  s_InlcudeMapping["UInt64FilterParameter"] = "NumberParameter";
  s_InlcudeMapping["AxisAngleFilterParameter"] = "VectorParameter";
  s_InlcudeMapping["IntVec2FilterParameter"] = "VectorParameter";
  s_InlcudeMapping["IntVec3FilterParameter"] = "VectorParameter";
  s_InlcudeMapping["FloatVec2FilterParameter"] = "VectorParameter";
  s_InlcudeMapping["FloatVec3FilterParameter"] = "VectorParameter";
  s_InlcudeMapping["NumericTypeFilterParameter"] = "NumericTypeParameter";
  s_InlcudeMapping["StringFilterParameter"] = "StringParameter";
  s_InlcudeMapping["SeparatorFilterParameter"] = "";
  s_InlcudeMapping["LinkedDataContainerSelectionFilterParameter"] = "DataGroupSelectionParameter";
  s_InlcudeMapping["LinkedPathCreationFilterParameter"] = "ArrayCreationParameter";
  s_InlcudeMapping["MultiDataArraySelectionFilterParameter"] = "MultiArraySelectionParameter";
  s_InlcudeMapping["LinkedBooleanFilterParameter"] = "BoolParameter";
  s_InlcudeMapping["LinkedChoicesFilterParameter"] = "ChoicesParameter";
  s_InlcudeMapping["PreflightUpdatedValueFilterParameter"] = "// no includes for PreflightUpdatedValueFilterParameter";

  // No COMPLEX Type Yet
  s_InlcudeMapping["CalculatorFilterParameter"] = "CalculatorFilterParameter";
  s_InlcudeMapping["ComparisonSelectionAdvancedFilterParameter"] = "ComparisonSelectionAdvancedFilterParameter";
  s_InlcudeMapping["ComparisonSelectionFilterParameter"] = "ComparisonSelectionFilterParameter";
  s_InlcudeMapping["ConvertHexGridToSquareGridFilterParameter"] = "ConvertHexGridToSquareGridFilterParameter";
  s_InlcudeMapping["DataContainerArrayProxyFilterParameter"] = "DataContainerArrayProxyFilterParameter";
  s_InlcudeMapping["DataContainerReaderFilterParameter"] = "DataContainerReaderFilterParameter";
  s_InlcudeMapping["DynamicChoiceFilterParameter"] = "DynamicChoiceFilterParameter";
  s_InlcudeMapping["DynamicTableFilterParameter"] = "DynamicTableFilterParameter";
  s_InlcudeMapping["EMMPMFilterParameter"] = "EMMPMFilterParameter";
  s_InlcudeMapping["EbsdMontageImportFilterParameter"] = "EbsdMontageImportFilterParameter";
  s_InlcudeMapping["EbsdToH5EbsdFilterParameter"] = "EbsdToH5EbsdFilterParameter";
  s_InlcudeMapping["EbsdWarpPolynomialFilterParameter"] = "EbsdWarpPolynomialFilterParameter";
  s_InlcudeMapping["EnsembleInfoFilterParameter"] = "EnsembleInfoFilterParameter";
  s_InlcudeMapping["SecondOrderPolynomialFilterParameter"] = "SecondOrderPolynomialFilterParameter";
  s_InlcudeMapping["ThirdOrderPolynomialFilterParameter"] = "ThirdOrderPolynomialFilterParameter";
  s_InlcudeMapping["FourthOrderPolynomialFilterParameter"] = "FourthOrderPolynomialFilterParameter";
  s_InlcudeMapping["GenerateColorTableFilterParameter"] = "GenerateColorTableFilterParameter";
  s_InlcudeMapping["ImportHDF5DatasetFilterParameter"] = "ImportHDF5DatasetFilterParameter";
  s_InlcudeMapping["ImportVectorImageStackFilterParameter"] = "ImportVectorImageStackFilterParameter";
  s_InlcudeMapping["KbrRecisConfigFilterParameter"] = "KbrRecisConfigFilterParameter";
  s_InlcudeMapping["MontageSelectionFilterParameter"] = "MontageSelectionFilterParameter";
  s_InlcudeMapping["MontageStructureSelectionFilterParameter"] = "MontageStructureSelectionFilterParameter";
  s_InlcudeMapping["MultiAttributeMatrixSelectionFilterParameter"] = "MultiAttributeMatrixSelectionFilterParameter";
  s_InlcudeMapping["MultiDataContainerSelectionFilterParameter"] = "MultiDataContainerSelectionFilterParameter";
  s_InlcudeMapping["MultiInputFileFilterParameter"] = "MultiInputFileFilterParameter";
  s_InlcudeMapping["OEMEbsdScanSelectionFilterParameter"] = "OEMEbsdScanSelectionFilterParameter";
  s_InlcudeMapping["OrientationUtilityFilterParameter"] = "OrientationUtilityFilterParameter";
  s_InlcudeMapping["ParagraphFilterParameter"] = "ParagraphFilterParameter";
  s_InlcudeMapping["PhaseTypeSelectionFilterParameter"] = "PhaseTypeSelectionFilterParameter";
  s_InlcudeMapping["RangeFilterParameter"] = "RangeFilterParameter";
  s_InlcudeMapping["ReadASCIIDataFilterParameter"] = "ReadASCIIDataFilterParameter";
  s_InlcudeMapping["ReadH5EbsdFilterParameter"] = "ReadH5EbsdFilterParameter";
  s_InlcudeMapping["ScalarTypeFilterParameter"] = "ScalarTypeFilterParameter";
  s_InlcudeMapping["ShapeTypeSelectionFilterParameter"] = "ShapeTypeSelectionFilterParameter";
  s_InlcudeMapping["StatsGeneratorFilterParameter"] = "StatsGeneratorFilterParameter";
  s_InlcudeMapping["Symmetric6x6FilterParameter"] = "Symmetric6x6FilterParameter";
}

void InitParameterMapping()
{
  s_ParameterMapping["BooleanFilterParameter"] = "BoolParameter";
  s_ParameterMapping["DataArrayCreationFilterParameter"] = "ArrayCreationParameter";
  s_ParameterMapping["DataArraySelectionFilterParameter"] = "ArraySelectionParameter";
  s_ParameterMapping["ChoiceFilterParameter"] = "ChoicesParameter";
  s_ParameterMapping["AttributeMatrixCreationFilterParameter"] = "DataGroupCreationParameter";
  s_ParameterMapping["AttributeMatrixSelectionFilterParameter"] = "DataGroupSelectionParameter";
  s_ParameterMapping["DataContainerCreationFilterParameter"] = "DataGroupCreationParameter";
  s_ParameterMapping["DataContainerSelectionFilterParameter"] = "DataGroupSelectionParameter";
  s_ParameterMapping["InputFileFilterParameter"] = "FileSystemPathParameter";
  s_ParameterMapping["InputPathFilterParameter"] = "FileSystemPathParameter";
  s_ParameterMapping["OutputFileFilterParameter"] = "FileSystemPathParameter";
  s_ParameterMapping["OutputPathFilterParameter"] = "FileSystemPathParameter";
  s_ParameterMapping["FileListInfoFilterParameter"] = "GeneratedFileListParameter";
  s_ParameterMapping["FloatFilterParameter"] = "Float32Parameter";
  s_ParameterMapping["IntFilterParameter"] = "Int32Parameter";
  s_ParameterMapping["DoubleFilterParameter"] = "Float64Parameter";
  s_ParameterMapping["UInt64FilterParameter"] = "UInt64Parameter";
  s_ParameterMapping["AxisAngleFilterParameter"] = "VectorFloat32Parameter";
  s_ParameterMapping["IntVec2FilterParameter"] = "VectorInt32Parameter";
  s_ParameterMapping["IntVec3FilterParameter"] = "VectorInt32Parameter";
  s_ParameterMapping["FloatVec2FilterParameter"] = "VectorFloat32Parameter";
  s_ParameterMapping["FloatVec3FilterParameter"] = "VectorFloat32Parameter";
  s_ParameterMapping["NumericTypeFilterParameter"] = "NumericTypeParameter";
  s_ParameterMapping["StringFilterParameter"] = "StringParameter";
  s_ParameterMapping["SeparatorFilterParameter"] = "SeparatorParameter";
  s_ParameterMapping["LinkedDataContainerSelectionFilterParameter"] = "DataGroupSelectionParameter";
  s_ParameterMapping["LinkedPathCreationFilterParameter"] = "ArrayCreationParameter";
  s_ParameterMapping["MultiDataArraySelectionFilterParameter"] = "MultiArraySelectionParameter";
  s_ParameterMapping["LinkedBooleanFilterParameter"] = "BoolParameter";
  s_ParameterMapping["LinkedChoicesFilterParameter"] = "ChoicesParameter";
  s_ParameterMapping["PreflightUpdatedValueFilterParameter"] = "NO PARAMETER TYPE";

  // No COMPLEX Type Yet
  s_ParameterMapping["CalculatorFilterParameter"] = "CalculatorFilterParameter";
  s_ParameterMapping["ComparisonSelectionAdvancedFilterParameter"] = "ComparisonSelectionAdvancedFilterParameter";
  s_ParameterMapping["ComparisonSelectionFilterParameter"] = "ComparisonSelectionFilterParameter";
  s_ParameterMapping["ConvertHexGridToSquareGridFilterParameter"] = "ConvertHexGridToSquareGridFilterParameter";
  s_ParameterMapping["DataContainerArrayProxyFilterParameter"] = "DataContainerArrayProxyFilterParameter";
  s_ParameterMapping["DataContainerReaderFilterParameter"] = "DataContainerReaderFilterParameter";
  s_ParameterMapping["DynamicChoiceFilterParameter"] = "DynamicChoiceFilterParameter";
  s_ParameterMapping["DynamicTableFilterParameter"] = "DynamicTableFilterParameter";
  s_ParameterMapping["EMMPMFilterParameter"] = "EMMPMFilterParameter";
  s_ParameterMapping["EbsdMontageImportFilterParameter"] = "EbsdMontageImportFilterParameter";
  s_ParameterMapping["EbsdToH5EbsdFilterParameter"] = "EbsdToH5EbsdFilterParameter";
  s_ParameterMapping["EbsdWarpPolynomialFilterParameter"] = "EbsdWarpPolynomialFilterParameter";
  s_ParameterMapping["EnsembleInfoFilterParameter"] = "EnsembleInfoFilterParameter";
  s_ParameterMapping["SecondOrderPolynomialFilterParameter"] = "SecondOrderPolynomialFilterParameter";
  s_ParameterMapping["ThirdOrderPolynomialFilterParameter"] = "ThirdOrderPolynomialFilterParameter";
  s_ParameterMapping["FourthOrderPolynomialFilterParameter"] = "FourthOrderPolynomialFilterParameter";
  s_ParameterMapping["GenerateColorTableFilterParameter"] = "GenerateColorTableFilterParameter";
  s_ParameterMapping["ImportHDF5DatasetFilterParameter"] = "ImportHDF5DatasetFilterParameter";
  s_ParameterMapping["ImportVectorImageStackFilterParameter"] = "ImportVectorImageStackFilterParameter";
  s_ParameterMapping["KbrRecisConfigFilterParameter"] = "KbrRecisConfigFilterParameter";
  s_ParameterMapping["MontageSelectionFilterParameter"] = "MontageSelectionFilterParameter";
  s_ParameterMapping["MontageStructureSelectionFilterParameter"] = "MontageStructureSelectionFilterParameter";
  s_ParameterMapping["MultiAttributeMatrixSelectionFilterParameter"] = "MultiAttributeMatrixSelectionFilterParameter";
  s_ParameterMapping["MultiDataContainerSelectionFilterParameter"] = "MultiDataContainerSelectionFilterParameter";
  s_ParameterMapping["MultiInputFileFilterParameter"] = "MultiInputFileFilterParameter";
  s_ParameterMapping["OEMEbsdScanSelectionFilterParameter"] = "OEMEbsdScanSelectionFilterParameter";
  s_ParameterMapping["OrientationUtilityFilterParameter"] = "OrientationUtilityFilterParameter";
  s_ParameterMapping["ParagraphFilterParameter"] = "ParagraphFilterParameter";
  s_ParameterMapping["PhaseTypeSelectionFilterParameter"] = "PhaseTypeSelectionFilterParameter";
  s_ParameterMapping["RangeFilterParameter"] = "RangeFilterParameter";
  s_ParameterMapping["ReadASCIIDataFilterParameter"] = "ReadASCIIDataFilterParameter";
  s_ParameterMapping["ReadH5EbsdFilterParameter"] = "ReadH5EbsdFilterParameter";
  s_ParameterMapping["ScalarTypeFilterParameter"] = "ScalarTypeFilterParameter";
  s_ParameterMapping["ShapeTypeSelectionFilterParameter"] = "ShapeTypeSelectionFilterParameter";
  s_ParameterMapping["StatsGeneratorFilterParameter"] = "StatsGeneratorFilterParameter";
  s_ParameterMapping["Symmetric6x6FilterParameter"] = "Symmetric6x6FilterParameter";
}

void InitParameterAvailableMapping()
{
  s_ParameterAvailable["BooleanFilterParameter"] = true;
  s_ParameterAvailable["DataArrayCreationFilterParameter"] = true;
  s_ParameterAvailable["DataArraySelectionFilterParameter"] = true;
  s_ParameterAvailable["ChoiceFilterParameter"] = true;
  s_ParameterAvailable["AttributeMatrixCreationFilterParameter"] = true;
  s_ParameterAvailable["AttributeMatrixSelectionFilterParameter"] = true;
  s_ParameterAvailable["DataContainerCreationFilterParameter"] = true;
  s_ParameterAvailable["DataContainerSelectionFilterParameter"] = true;
  s_ParameterAvailable["InputFileFilterParameter"] = true;
  s_ParameterAvailable["InputPathFilterParameter"] = true;
  s_ParameterAvailable["OutputFileFilterParameter"] = true;
  s_ParameterAvailable["OutputPathFilterParameter"] = true;
  s_ParameterAvailable["FileListInfoFilterParameter"] = true;
  s_ParameterAvailable["FloatFilterParameter"] = true;
  s_ParameterAvailable["IntFilterParameter"] = true;
  s_ParameterAvailable["DoubleFilterParameter"] = true;
  s_ParameterAvailable["UInt64FilterParameter"] = true;
  s_ParameterAvailable["AxisAngleFilterParameter"] = true;
  s_ParameterAvailable["IntVec2FilterParameter"] = true;
  s_ParameterAvailable["IntVec3FilterParameter"] = true;
  s_ParameterAvailable["FloatVec2FilterParameter"] = true;
  s_ParameterAvailable["FloatVec3FilterParameter"] = true;
  s_ParameterAvailable["NumericTypeFilterParameter"] = true;
  s_ParameterAvailable["StringFilterParameter"] = true;
  s_ParameterAvailable["SeparatorFilterParameter"] = true;
  s_ParameterAvailable["LinkedDataContainerSelectionFilterParameter"] = true;
  s_ParameterAvailable["LinkedPathCreationFilterParameter"] = true;
  s_ParameterAvailable["MultiDataArraySelectionFilterParameter"] = true;
  s_ParameterAvailable["LinkedBooleanFilterParameter"] = true;
  s_ParameterAvailable["LinkedChoicesFilterParameter"] = true;
  s_ParameterAvailable["PreflightUpdatedValueFilterParameter"] = true;

  // No COMPLEX Type Yet
  s_ParameterAvailable["CalculatorFilterParameter"] = false;
  s_ParameterAvailable["ComparisonSelectionAdvancedFilterParameter"] = false;
  s_ParameterAvailable["ComparisonSelectionFilterParameter"] = false;
  s_ParameterAvailable["ConvertHexGridToSquareGridFilterParameter"] = false;
  s_ParameterAvailable["DataContainerArrayProxyFilterParameter"] = false;
  s_ParameterAvailable["DataContainerReaderFilterParameter"] = false;
  s_ParameterAvailable["DynamicChoiceFilterParameter"] = false;
  s_ParameterAvailable["DynamicTableFilterParameter"] = false;
  s_ParameterAvailable["EMMPMFilterParameter"] = false;
  s_ParameterAvailable["EbsdMontageImportFilterParameter"] = false;
  s_ParameterAvailable["EbsdToH5EbsdFilterParameter"] = false;
  s_ParameterAvailable["EbsdWarpPolynomialFilterParameter"] = false;
  s_ParameterAvailable["EnsembleInfoFilterParameter"] = false;
  s_ParameterAvailable["SecondOrderPolynomialFilterParameter"] = false;
  s_ParameterAvailable["ThirdOrderPolynomialFilterParameter"] = false;
  s_ParameterAvailable["FourthOrderPolynomialFilterParameter"] = false;
  s_ParameterAvailable["GenerateColorTableFilterParameter"] = false;
  s_ParameterAvailable["ImportHDF5DatasetFilterParameter"] = false;
  s_ParameterAvailable["ImportVectorImageStackFilterParameter"] = false;
  s_ParameterAvailable["KbrRecisConfigFilterParameter"] = false;
  s_ParameterAvailable["MontageSelectionFilterParameter"] = false;
  s_ParameterAvailable["MontageStructureSelectionFilterParameter"] = false;
  s_ParameterAvailable["MultiAttributeMatrixSelectionFilterParameter"] = false;
  s_ParameterAvailable["MultiDataContainerSelectionFilterParameter"] = false;
  s_ParameterAvailable["MultiInputFileFilterParameter"] = false;
  s_ParameterAvailable["OEMEbsdScanSelectionFilterParameter"] = false;
  s_ParameterAvailable["OrientationUtilityFilterParameter"] = false;
  s_ParameterAvailable["ParagraphFilterParameter"] = false;
  s_ParameterAvailable["PhaseTypeSelectionFilterParameter"] = false;
  s_ParameterAvailable["RangeFilterParameter"] = false;
  s_ParameterAvailable["ReadASCIIDataFilterParameter"] = false;
  s_ParameterAvailable["ReadH5EbsdFilterParameter"] = false;
  s_ParameterAvailable["ScalarTypeFilterParameter"] = false;
  s_ParameterAvailable["ShapeTypeSelectionFilterParameter"] = false;
  s_ParameterAvailable["StatsGeneratorFilterParameter"] = false;
  s_ParameterAvailable["Symmetric6x6FilterParameter"] = false;
}

bool isPropertySepOrPreflight(const QString& property, FilterParameterVectorType parameters)
{
  for(const auto& parameter : parameters)
  {
    QString origParamClassName = parameter->getNameOfClass();
    if(parameter->getPropertyName() == property && (origParamClassName == "PreflightUpdatedValueFilterParameter" || origParamClassName == "SeparatorFilterParameter"))
    {
      return true;
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
QString ReadTemplateFile(const QString& file)
{
  QString contents;
  QFileInfo fi(file);
  QFile source(file);
  source.open(QFile::ReadOnly);
  contents = source.readAll();
  source.close();
  return contents;
}

// -----------------------------------------------------------------------------
void writeTopLevelOutput(const QString& outputPath, const QString& pluginName, const QString& outLines, const QString& fileName)
{
  QString filtersDir = "/Filters/";
  if(!s_HasAllParameters)
  {
    filtersDir = "/Filters-Disabled/";
  }

  //  // Assume we are Core first...
  //  QString outputPath;
  //  if(s_CurrentPlugin == "AFRLDistributionC"
  //  || s_CurrentPlugin == "CRADA3DSystems"
  //  || s_CurrentPlugin == "ECAccess"
  //  || s_CurrentPlugin == "BrukerIntegration"
  //  || s_CurrentPlugin == "CAxisByPolarizedLight"
  //  || s_CurrentPlugin == "NETLIntegration"
  //  )
  //  {
  //   outputPath = k_PluginsOutputDir + "_private/" + s_CurrentPlugin;
  //  }
  //  else
  //  {
  //    outputPath = k_PluginsOutputDir + "/" + s_CurrentPlugin;
  //  }

  // Generate the Output File Path
  QString finalOutPath = outputPath + "/" + fileName;
  QFileInfo fi(finalOutPath);

  // Make sure the output directory structure is created.
  QDir outputDir(fi.absolutePath());
  outputDir.mkpath(fi.absolutePath());

  // Write the output
  QFile hOut(finalOutPath);
  hOut.open(QFile::WriteOnly);
  QTextStream stream(&hOut);
  stream << outLines;
  hOut.close();
  // qDebug() << "Saved File " << fi.absoluteFilePath();
}

// -----------------------------------------------------------------------------
void writeOutput(AbstractFilter* filter, const QString& outputPath, const QString& outLines, const QString& ext, const QString& mdSummary)
{
  QString filtersDir = "/Filters/";
  //  if(!s_HasAllParameters)
  //  {
  //    filtersDir = "/Filters-Disabled/";
  //  }

  // Assume we are Core first...
  //  QString outputPath;
  //  QString mdOutputPath;
  // if(s_CurrentPlugin == "AFRLDistributionC"
  //  || s_CurrentPlugin == "CRADA3DSystems"
  //  || s_CurrentPlugin == "ECAccess"
  //  || s_CurrentPlugin == "BrukerIntegration"
  //  || s_CurrentPlugin == "CAxisByPolarizedLight"
  //  || s_CurrentPlugin == "NETLIntegration"
  //  )
  //  {
  //    outputPath = k_PluginsOutputDir + "_private/" + s_CurrentPlugin + "/src/" + s_CurrentPlugin + "/" + filtersDir;
  //    mdOutputPath = k_PluginsOutputDir + "_private/" + s_CurrentPlugin + "/docs/" + s_CurrentPlugin + "/";
  //  }
  //  else
  //  {
  //    outputPath = k_PluginsOutputDir + "/" + s_CurrentPlugin + "/src/" + s_CurrentPlugin + "/" + filtersDir;
  //    mdOutputPath = k_PluginsOutputDir + "/" + s_CurrentPlugin + "/docs/" + s_CurrentPlugin + "/";
  //  }

  // Make sure the output directory structure is created.
  {
    QDir outputDir(outputPath);
    outputDir.mkpath(outputPath);

    // Generate the Output File Path
    QString finalOutPath = outputPath + filter->getNameOfClass() + ext;
    QFileInfo fi(finalOutPath);

    // Write the output
    QFile hOut(finalOutPath);
    hOut.open(QFile::WriteOnly);
    QTextStream stream(&hOut);
    stream << outLines;
    hOut.close();
  }
  // write the markdown file
  //  if(!mdSummary.isEmpty())
  //  {
  //    QDir outputDir(mdOutputPath);
  //    outputDir.mkpath(mdOutputPath);
  //    // Generate the Output File Path
  //    QString finalOutPath = mdOutputPath + filter->getNameOfClass() + ".md";
  //    // std::cout << "Writing MD File: " << finalOutPath.toStdString() << std::endl;
  //    QFileInfo fi(finalOutPath);

  //    // Write the output
  //    QFile hOut(finalOutPath);
  //    hOut.open(QFile::WriteOnly);
  //    QTextStream stream(&hOut);
  //    stream << mdSummary;
  //    hOut.close();
  //  }
}

// check for duplicate uuids
template <typename Type>
bool SearchMapKeysAndValue(Type search, std::map<Type, Type> mapToQuery)
{
  for(auto const& [key, val] : mapToQuery)
  {
    if(key == search)
    {
      return true;
    }

    if(val == search)
    {
      return true;
    }
  }
  return false;
}

template <typename Type>
QString GenerateUuid(std::map<Type, Type> mapToCheck)
{
  QString uuid;
  std::string search;
  do
  {
    uuid = QUuid::createUuid().toString();
    search = uuid.replace("{", "").replace("}", "").toStdString();
  } while(SearchMapKeysAndValue(search, SIMPL_to_Complex));
  return uuid;
}

// std::vector<QString> sepKeys;
//  -----------------------------------------------------------------------------
void GenerateHeaderFile(AbstractFilter* filter, const QString& outputDir, const QString& pluginName)
{
  static QRegularExpression regExp1{"(.)([A-Z][a-z]+)"};
  static QRegularExpression regExp2{"([a-z0-9])([A-Z])"};
  const QString k_UUIDMapFileName("src/" + pluginName + "/" + QString::fromStdString(pluginName.toStdString() + "LegacyUUIDMapping.hpp").replace("/", ""));
  const QString k_NewIncludeToken("// @@__HEADER__TOKEN__DO__NOT__DELETE__@@");
  const QString k_SIMPLToComplexToken("// @@__MAP__UPDATE__TOKEN__DO__NOT__DELETE__@@");

  QString headerTemplate = ReadTemplateFile(k_HeaderFile);
  QString sourceTemplate = ReadTemplateFile(outputDir + "/" + k_UUIDMapFileName);

  QString filterName = filter->getNameOfClass();
  if(filterName.endsWith("Filter"))
  {
    filterName.replace("Filter", "");
  }
  QString humanName = filter->getHumanLabel();
  QString uuid;
  QString prevUuid = filter->getUuid().toString();
  auto temp = prevUuid.replace("{", "").replace("}", "");
  std::string searchUUID = temp.toStdString();
  if(SIMPL_to_Complex.count(searchUUID) == 0)
  {
    uuid = GenerateUuid(SIMPL_to_Complex);
  }
  else
  {
    uuid = QString::fromStdString(SIMPL_to_Complex[searchUUID]);
  }
  uuid = uuid.remove("{");
  uuid = uuid.remove("}");
  s_FilterUuidMapping[prevUuid] = uuid;
  QTextStream ptonew(&s_PrevToNewUuidMapping);

  ptonew << "{Uuid::FromString(\"" << prevUuid << "\").value(), Uuid::FromString(\"" << uuid << "\").value()}, ";
  ptonew << "/* " << filterName << " */\n";

  sourceTemplate = sourceTemplate.replace(k_NewIncludeToken, "#include \"" + pluginName + "/Filters/" + filterName + "Filter.hpp\"\n" + k_NewIncludeToken);
  sourceTemplate = sourceTemplate.replace(k_SIMPLToComplexToken, "{complex::Uuid::FromString(\"" + prevUuid + "\").value(), complex::FilterTraits<" + filterName + "Filter>::uuid}, // " + filterName +
                                                                     "\n    " + k_SIMPLToComplexToken);

  headerTemplate = headerTemplate.replace(k_FILTER_NAME, filterName + "Filter");
  headerTemplate = headerTemplate.replace(k_UUID, uuid);
  headerTemplate = headerTemplate.replace(k_OLD_UUID, prevUuid);
  QString plugName = pluginName;
  plugName.replace("/", "");
  headerTemplate = headerTemplate.replace(k_PLUGIN_NAME, plugName);
  QString pluginNameUpper = pluginName.toUpper().replace("/", "");
  headerTemplate = headerTemplate.replace("@PLUGIN_NAME_UPPER@", pluginNameUpper);

  // Generate the Parameter keys
  FilterParameterVectorType parameters = filter->getFilterParameters();
  QString pString;
  QTextStream out(&pString);
  for(const auto& parameter : parameters)
  {
    QString origParamClassName = parameter->getNameOfClass();
    QString propName = parameter->getPropertyName();

    QString snake_case_prop = propName;
    snake_case_prop.replace(regExp1, "\\1_\\2");
    snake_case_prop = snake_case_prop.replace(regExp2, "\\1_\\2").toLower();

    if(origParamClassName != "SeparatorFilterParameter" && origParamClassName != "PreflightUpdatedValueFilterParameter")
    {
      out << "  static inline constexpr StringLiteral k_" << propName << "_Key = \"" << snake_case_prop << "\";\n";
    }
  }
  headerTemplate = headerTemplate.replace(k_PARAMETER_KEYS, pString);

  writeTopLevelOutput(outputDir, pluginName, headerTemplate, "src/" + pluginName + "/Filters/" + filterName + "Filter.hpp"); // actual file
  writeTopLevelOutput(outputDir, pluginName, sourceTemplate, k_UUIDMapFileName);                                             // UUID Map
}

// -----------------------------------------------------------------------------
void GenerateAlgorithmFile(AbstractFilter* filter, const QString& outputDir, const QString& pluginName)
{

  QString filterName = filter->getNameOfClass();
  if(filterName.endsWith("Filter"))
  {
    filterName.replace("Filter", "");
  }
  QString humanName = filter->getHumanLabel();
  // QString pluginName = filter->getCompiledLibraryName();
  QString pluginNameUpper = pluginName.toUpper().replace("/", "");

  QString sourceTemplate = ReadTemplateFile(k_AlgorithmSourceFile);
  QString headerTemplate = ReadTemplateFile(k_AlgorithmHeaderFile);

  sourceTemplate = sourceTemplate.replace(k_FILTER_NAME, filterName);
  sourceTemplate = sourceTemplate.replace(k_PLUGIN_NAME, pluginName);

  headerTemplate = headerTemplate.replace(k_FILTER_NAME, filterName);
  QString plugName = pluginName;
  plugName.replace("/", "");
  headerTemplate = headerTemplate.replace(k_PLUGIN_NAME, plugName);
  headerTemplate = headerTemplate.replace("@PLUGIN_NAME_UPPER@", pluginNameUpper);

  // Generate the Parameter Section
  FilterParameterVectorType parameters = filter->getFilterParameters();
  QString pString;
  QTextStream parameterOut(&pString);

  QString executeString;
  QTextStream executeOut(&executeString);

  QString incString;
  QTextStream includeOut(&incString);
  QSet<QString> includeSet;

  executeOut << "  " << filterName << "InputValues inputValues;\n\n";

  for(const auto& parameter : parameters)
  {
    QString origParamClassName = parameter->getNameOfClass();
    QString propName = parameter->getPropertyName();
    QString propClass = s_ParameterMapping[origParamClassName];
    QString propHuman = parameter->getHumanLabel();
    QString propInclude = s_InlcudeMapping[origParamClassName];
    QString defaultValue = s_DefaultConstructorMapping[origParamClassName];
    QString unitTestDefaultValue = s_UnitTestDefaultValue[origParamClassName];
    QString paramType = s_ParameterTypeMapping[origParamClassName];
    bool hasParameter = s_ParameterAvailable[origParamClassName];
    s_ParameterCount[origParamClassName]++;
    //bool endsWithValue = propName.endsWith("Value");

    if(!hasParameter)
    {
      s_HasAllParameters = false;
    }

    if(!propName.isEmpty())
    {
      parameterOut << "  " << paramType << " " << propName << ";\n";
      executeOut << "  inputValues." << propName << " = filterArgs.value<" << paramType << ">(k_" << propName << "_Key);\n";
    }

    if(origParamClassName == "SeparatorFilterParameter")
    {
      // parameterOut << "  params.insertSeparator(Parameters::Separator{\"" << propHuman << "\"});\n";
    }
    else if(origParamClassName == "LinkedBooleanFilterParameter")
    {
      // parameterOut << "  args.insertOrAssign(" << filterName << "::k_" << propName << "_Key, std::make_any<" << paramType << ">(" << unitTestDefaultValue << "));\n";
      //      includeOut << "#include \"complex/Parameters/" << propInclude << ".hpp\"\n";
    }
    else if(origParamClassName == "LinkedChoicesFilterParameter")
    {
    }
    else if(origParamClassName == "PreflightUpdatedValueFilterParameter")
    {
    }
    else
    {
      if(!hasParameter)
      {
        parameterOut << "/*[x]*/";
      }
      // parameterOut << "  args.insertOrAssign(" << filterName << "::k_" << propName << "_Key, std::make_any<" << paramType << ">(" << unitTestDefaultValue << "));\n";
      includeOut << "#include \"complex/Parameters/" << propInclude << ".hpp\"\n";
    }

    if(propInclude == "FileSystemPathParameter")
    {
      //  needsFs = true;
    }
    //  includeSet.insert(incString);
    //  incString.clear();
  }

  executeOut << "\n  return " << filterName << "(dataStructure, messageHandler, shouldCancel, &inputValues)();\n";

  headerTemplate = headerTemplate.replace(k_PARAMETER_INCLUDES, incString);
  headerTemplate = headerTemplate.replace("@INPUT_VALUE_STRUCT_DEF@", pString);
  headerTemplate = headerTemplate.replace("@EXECUTE_EXAMPLE_CODE@", executeString);

  writeTopLevelOutput(outputDir, pluginName, sourceTemplate, "src/" + pluginName + "/Filters/Algorithms/" + filterName + ".cpp");
  writeTopLevelOutput(outputDir, pluginName, headerTemplate, "src/" + pluginName + "/Filters/Algorithms/" + filterName + ".hpp");
}

// -----------------------------------------------------------------------------
void GenerateSourceFile(AbstractFilter* filter, const QString& outputDir, const QString& pluginName)
{
  QString sourceTemplate = ReadTemplateFile(k_SourceFile);
  QString filterName = filter->getNameOfClass();
  if(filterName.endsWith("Filter"))
  {
    filterName.replace("Filter", "");
  }
  QString humanName = filter->getHumanLabel();
  // QString pluginName = filter->getCompiledLibraryName();

  QString defaultTags = QString("\"%1\", \"%2\"").arg(filter->getGroupName(), filter->getSubGroupName());
  if(filter->getSubGroupName() == SIMPL::FilterSubGroups::InputFilters)
  {
    defaultTags = defaultTags + ", \"Read\", \"Import\"";
  }
  if(filter->getSubGroupName() == SIMPL::FilterSubGroups::OutputFilters)
  {
    defaultTags = defaultTags + ", \"Write\", \"Export\"";
  }

  sourceTemplate = sourceTemplate.replace(k_ALGORITHM_NAME, filterName);

  sourceTemplate = sourceTemplate.replace(k_FILTER_NAME, filterName + "Filter");
  sourceTemplate = sourceTemplate.replace(k_FILTER_HUMAN_NAME, humanName);
  sourceTemplate = sourceTemplate.replace(k_DEFAULT_TAGS, defaultTags);
  QString plugName = pluginName;
  plugName.replace("/", "");
  sourceTemplate = sourceTemplate.replace(k_PLUGIN_NAME, plugName);

  // Generate the Parameter Section
  FilterParameterVectorType parameters = filter->getFilterParameters();
  QString pString;
  QTextStream parameterOut(&pString);

  QString linkString;
  QTextStream linkedOut(&linkString);

  QString incString;
  QTextStream includeOut(&incString);
  QSet<QString> includeSet;

  QString preflightString;
  QTextStream preFlightOut(&preflightString);

  QString inputValuesString;
  QTextStream inputValOut(&inputValuesString);

  QString mdParamString;
  QTextStream mdParamOut(&mdParamString);

  QString preflightUpdatedValues;
  QTextStream pfuv(&preflightUpdatedValues);
  QString preflightUpdatedDefs;
  QTextStream pfud(&preflightUpdatedDefs);

  QString proposedActions;
  QTextStream actionsOut(&proposedActions);
  int dataArrayCreation = 0;

  mdParamOut << "# " << filterName << " #\n\n";
  mdParamOut << "| Ready | Parameter Key | Human Name | Parameter Type | Parameter Class |\n";
  mdParamOut << "|-------|---------------|------------|-----------------|----------------|\n";

  s_HasAllParameters = true;
  bool needsFs = false;

  // Loop through all the parameters once to map out any linkable parameters
  QMap<QString, std::vector<int>> propToGroupMap;
  for(const auto& parameter : parameters)
  {
    QString propName = parameter->getPropertyName();
    auto groupIndices = parameter->getGroupIndices();
    propToGroupMap[propName] = groupIndices;
  }

  for(const auto& parameter : parameters)
  {
    QString origParamClassName = parameter->getNameOfClass();
    QString propName = parameter->getPropertyName();
    QString propClass = s_ParameterMapping[origParamClassName];
    QString propHuman = parameter->getHumanLabel();
    QString propInclude = s_InlcudeMapping[origParamClassName];
    QString defaultValue = s_DefaultConstructorMapping[origParamClassName];
    QString paramType = s_ParameterTypeMapping[origParamClassName];
    bool hasParameter = s_ParameterAvailable[origParamClassName];
    bool endsWithValue = propName.endsWith("Value");
    //auto category = parameter->getCategory();

    s_ParameterCount[origParamClassName]++;

    if(!hasParameter)
    {
      s_HasAllParameters = false;
    }

    if(origParamClassName != "SeparatorFilterParameter" && origParamClassName != "PreflightUpdatedValueFilterParameter")
    {
      preFlightOut << "  auto p" << propName << (endsWithValue ? "" : "Value") << " = filterArgs.value<" << paramType << ">(k_" << propName << "_Key);\n";
      inputValOut << "  inputValues." << propName << " = filterArgs.value<" << paramType << ">(k_" << propName << "_Key);\n";

      mdParamOut << "| " << (hasParameter ? "YES" : "NO") << " | " << propName << " | " << propHuman << " | " << paramType << " | " << propClass << " |\n";
    }

    if(origParamClassName == "SeparatorFilterParameter")
    {
      parameterOut << "  params.insertSeparator(Parameters::Separator{\"" << propHuman << "\"});\n";
    }
    else if(origParamClassName == "LinkedBooleanFilterParameter")
    {
      parameterOut << "  params.insertLinkableParameter(std::make_unique<" << propClass << ">(k_" << propName << "_Key, \"" << propHuman << "\", \"\", " << defaultValue << "));\n";
      LinkedBooleanFilterParameter::Pointer fp = std::dynamic_pointer_cast<LinkedBooleanFilterParameter>(parameter);
      std::vector<QString> linkedProps = fp->getConditionalProperties();
      for(const auto& linkedProp : linkedProps)
      {
        if(!isPropertySepOrPreflight(linkedProp, parameters))
        {
          linkedOut << "  params.linkParameters(k_" << propName << "_Key, k_" << linkedProp << "_Key, true);\n";
        }
      }
      includeOut << "#include \"complex/Parameters/" << propInclude << ".hpp\"\n";
    }
    else if(origParamClassName == "LinkedChoicesFilterParameter")
    {
      LinkedChoicesFilterParameter::Pointer fp = std::dynamic_pointer_cast<LinkedChoicesFilterParameter>(parameter);
      std::vector<QString> linkedChoices = fp->getChoices();
      size_t i = 0;
      defaultValue = "0,  ChoicesParameter::Choices{";
      for(const auto& choice : linkedChoices)
      {
        defaultValue = defaultValue + "\"" + choice + "\"";
        i++;
        if(i < linkedChoices.size())
        {
          defaultValue += ", ";
        }
      }
      defaultValue += "}";
      parameterOut << "  params.insertLinkableParameter(std::make_unique<" << propClass << ">(k_" << propName << "_Key, \"" << propHuman << "\", \"\", " << defaultValue << "));\n";
      std::vector<QString> linkedProps = fp->getLinkedProperties();
      i = 0;
      for(const auto& linkedProp : linkedProps)
      {
        if(!isPropertySepOrPreflight(linkedProp, parameters))
        {
          // linkedOut << "  params.linkParameters(k_" << propName << "_Key, k_" << linkedProp << "_Key, " << propToGroupMap[linkedProp] << ");\n";
          linkedOut << "//TODO: THIS NEEDS TO BE FIXED\n";
        }
      }
      includeOut << "#include \"complex/Parameters/" << propInclude << ".hpp\"\n";
    }
    else if(origParamClassName == "PreflightUpdatedValueFilterParameter")
    {
      PreflightUpdatedValueFilterParameter::Pointer fp = std::dynamic_pointer_cast<PreflightUpdatedValueFilterParameter>(parameter);
      QString lower = fp->getPropertyName();
      lower[0] = lower[0].toLower();
      pfud << "  std::string " << lower << ";\n";
      pfuv << "  preflightUpdatedValues.push_back({\"" << fp->getPropertyName() << "\", " << lower << "});\n";
    }
    else
    {
      if(!hasParameter)
      {
        parameterOut << "/*[x]*/";
      }
      parameterOut << "  params.insert(std::make_unique<" << propClass << ">(k_" << propName << "_Key, \"" << propHuman << "\", \"\", " << defaultValue << "));\n";
      includeOut << "#include \"complex/Parameters/" << propInclude << ".hpp\"\n";
    }

    if(origParamClassName == "DataArrayCreationFilterParameter")
    {
      actionsOut << "  // This block is commented out because it needs some variables to be filled in.\n";
      actionsOut << "  {\n    // auto createArrayAction = std::make_unique<CreateArrayAction>(complex::NumericType::FILL_ME_IN, std::vector<usize>{NUM_TUPLES_VALUE}, NUM_COMPONENTS, p" << propName
                 << (endsWithValue ? "" : "Value") << ");\n";
      actionsOut << "    // resultOutputActions.value().actions.push_back(std::move(createArrayAction));\n  }\n";
      includeSet.insert("#include \"complex/Filter/Actions/CreateArrayAction.hpp\"\n");
      dataArrayCreation++;
    }
    else if(origParamClassName == "AttributeMatrixCreationFilterParameter")
    {
      actionsOut << "  {\n    auto createDataGroupAction = std::make_unique<CreateDataGroupAction>(p" << propName << (endsWithValue ? "" : "Value") << ");\n";
      actionsOut << "    resultOutputActions.value().actions.push_back(std::move(createDataGroupAction));\n  }\n";
      includeSet.insert("#include \"complex/Filter/Actions/CreateDataGroupAction.hpp\"\n");
      dataArrayCreation++;
    }
    else if(origParamClassName == "DataContainerCreationFilterParameter")
    {
      actionsOut << "  {\n    auto createDataGroupAction = std::make_unique<CreateDataGroupAction>(p" << propName << (endsWithValue ? "" : "Value") << ");\n";
      actionsOut << "    resultOutputActions.value().actions.push_back(std::move(createDataGroupAction));\n  }\n";
      includeSet.insert("#include \"complex/Filter/Actions/CreateDataGroupAction.hpp\"\n");
      dataArrayCreation++;
    }

    if(propInclude == "FileSystemPathParameter")
    {
      needsFs = true;
    }
    includeSet.insert(incString);
    incString.clear();

  } // End loop over all Parameters

  if(dataArrayCreation > 0)
  {
    proposedActions = "  // These are some proposed Actions based on the FilterParameters used. Please check them for correctness.\n" + proposedActions;
  }
  sourceTemplate = sourceTemplate.replace(k_PROPOSED_ACTIONS, proposedActions);
  if(!preflightUpdatedValues.isEmpty())
  {
    preflightUpdatedValues = "  // These values should have been updated during the preflightImpl(...) method\n" + preflightUpdatedValues;
  }
  if(!preflightUpdatedDefs.isEmpty())
  {
    preflightUpdatedDefs = QString("  // These variables should be updated with the latest data generated for each variable during preflight. \n") +
                           QString("  // These will be returned through the preflightResult variable to the\n") +
                           QString("  // user interface. You could make these member variables instead if needed.\n") + preflightUpdatedDefs;
  }

  for(const auto& incStr : includeSet)
  {
    includeOut << incStr;
  }
  if(needsFs)
  {
    includeOut << "\n#include <filesystem>\nnamespace fs = std::filesystem;\n";
  }

  sourceTemplate = sourceTemplate.replace(k_PARAMETER_INCLUDES, incString);

  // Concatenate the parameter string with the linkable params string.
  if(!linkString.isEmpty())
  {
    linkString = "  // Associate the Linkable Parameter(s) to the children parameters that they control\n" + linkString;
    pString = pString + linkString;
  }

  sourceTemplate = sourceTemplate.replace(k_PARAMETER_DEFS, pString);
  sourceTemplate = sourceTemplate.replace(k_PREFLIGHT_DEFS, preflightString);
  sourceTemplate = sourceTemplate.replace(k_INPUT_VALUES_DEF, inputValuesString);

  if(preflightUpdatedValues.isEmpty())
  {
    preflightUpdatedValues = "  // None found based on the filter parameters\n";
  }
  sourceTemplate = sourceTemplate.replace(k_PREFLIGHT_UPDATED_VALUES, preflightUpdatedValues);
  if(preflightUpdatedDefs.isEmpty())
  {
    preflightUpdatedDefs = "  // None found in this filter based on the filter parameters\n";
  }
  sourceTemplate = sourceTemplate.replace(k_PREFLIGHT_UPDATED_DEFS, preflightUpdatedDefs);

  if(s_HasAllParameters)
  {
    s_TotalGoodFilters++;
  }
  else
  {
    s_TotalBadFilters++;
  }
  s_FilterHasAllParameters[filter->getNameOfClass().toStdString()] = s_HasAllParameters;
  // writeOutput(filter, outputDir, sourceTemplate, ".cpp", mdParamString);

  writeTopLevelOutput(outputDir, pluginName, sourceTemplate, "src/" + pluginName + "/Filters/" + filterName + "Filter.cpp");
}

// -----------------------------------------------------------------------------
void GenerateUnitTestSourceFile(const AbstractFilter::Pointer& filter, const QString& outputDir, const QString& pluginName)
{
  QString sourceTemplate = ReadTemplateFile(k_UnitTestSourceFile);
  QString filterName = filter->getNameOfClass();
  if(filterName.endsWith("Filter"))
  {
    filterName.replace("Filter", "");
  }
  QString humanName = filter->getHumanLabel();
  QString DEFAULT_VALUE = "{0}";
  // Generate the Parameter Section
  FilterParameterVectorType parameters = filter->getFilterParameters();

  sourceTemplate = sourceTemplate.replace(k_FILTER_NAME, filterName + "Filter");
  sourceTemplate = sourceTemplate.replace(k_FILTER_HUMAN_NAME, humanName);

  QString plugName = pluginName;
  plugName.replace("/", "");
  sourceTemplate = sourceTemplate.replace(k_PLUGIN_NAME, plugName);

  QString pString;
  QTextStream parameterOut(&pString);

  QString linkString;
  QTextStream linkedOut(&linkString);

  QString incString;
  QTextStream includeOut(&incString);
  QSet<QString> includeSet;

  QString preflightString;
  QTextStream preFlightOut(&preflightString);

  QString inputValuesString;
  QTextStream inputValOut(&inputValuesString);

  QString mdParamString;
  QTextStream mdParamOut(&mdParamString);

  bool needsFs = false;
  for(const auto& parameter : parameters)
  {
    QString origParamClassName = parameter->getNameOfClass();
    QString propName = parameter->getPropertyName();
    QString propClass = s_ParameterMapping[origParamClassName];
    QString propHuman = parameter->getHumanLabel();
    QString propInclude = s_InlcudeMapping[origParamClassName];
    QString defaultValue = s_DefaultConstructorMapping[origParamClassName];
    QString unitTestDefaultValue = s_UnitTestDefaultValue[origParamClassName];
    QString paramType = s_ParameterTypeMapping[origParamClassName];
    bool hasParameter = s_ParameterAvailable[origParamClassName];
    s_ParameterCount[origParamClassName]++;
    bool endsWithValue = propName.endsWith("Value");

    if(!hasParameter)
    {
      s_HasAllParameters = false;
    }

    if(!propName.isEmpty())
    {
      preFlightOut << "  auto p" << propName << (endsWithValue ? "" : "Value") << " = filterArgs.value<" << paramType << ">(k_" << propName << "_Key);\n";

      inputValOut << "  inputValues." << propName << " = filterArgs.value<" << paramType << ">(k_" << propName << "_Key);\n";

      mdParamOut << "| " << (hasParameter ? "YES" : "NO") << " | " << propName << " | " << propHuman << " | " << paramType << " | " << propClass << " |\n";
    }

    if(origParamClassName == "SeparatorFilterParameter")
    {
      // parameterOut << "  params.insertSeparator(Parameters::Separator{\"" << propHuman << "\"});\n";
    }
    else if(origParamClassName == "LinkedBooleanFilterParameter")
    {
      parameterOut << "  args.insertOrAssign(" << filterName << "Filter::k_" << propName << "_Key, std::make_any<" << paramType << ">(" << unitTestDefaultValue << "));\n";
      includeOut << "#include \"complex/Parameters/" << propInclude << ".hpp\"\n";
    }
    else if(origParamClassName == "LinkedChoicesFilterParameter")
    {
    }
    else if(origParamClassName == "PreflightUpdatedValueFilterParameter")
    {
    }
    else
    {
      if(!hasParameter)
      {
        parameterOut << "/*[x]*/";
      }
      parameterOut << "  args.insertOrAssign(" << filterName << "Filter::k_" << propName << "_Key, std::make_any<" << paramType << ">(" << unitTestDefaultValue << "));\n";
      includeOut << "#include \"complex/Parameters/" << propInclude << ".hpp\"\n";
    }

    if(propInclude == "FileSystemPathParameter")
    {
      needsFs = true;
    }
    includeSet.insert(incString);
    incString.clear();
  }

  for(const auto& incStr : includeSet)
  {
    includeOut << incStr;
  }
  if(needsFs)
  {
    includeOut << "\n#include <filesystem>\nnamespace fs = std::filesystem;\n";
  }

  sourceTemplate = sourceTemplate.replace(k_PARAMETER_INCLUDES, incString);

  // Concatenate the parameter string with the linkable params string.
  if(!linkString.isEmpty())
  {
    linkString = "  // Associate the Linkable Parameter(s) to the children parameters that they control\n" + linkString;
    pString = pString + linkString;
  }

  sourceTemplate = sourceTemplate.replace(k_PARAMETER_DEFS, pString);
  sourceTemplate = sourceTemplate.replace(k_PREFLIGHT_DEFS, preflightString);
  sourceTemplate = sourceTemplate.replace(k_INPUT_VALUES_DEF, inputValuesString);

  QString outputFileName = QString("test/%1Test.cpp").arg(filterName);
  writeTopLevelOutput(outputDir, pluginName, sourceTemplate, "test/" + filterName + "Test.cpp");
}

// -----------------------------------------------------------------------------
void GenerateUnitTestCMakeFile(std::vector<AbstractFilter::Pointer>& filters, const QString& outputDir, const QString& pluginName)
{
  QString cmakeTemplate = ReadTemplateFile(k_TestCMakeListsFile);
  cmakeTemplate = cmakeTemplate.replace(k_PLUGIN_NAME, pluginName);

  QString filterList;
  QTextStream hOut(&filterList);

  for(const auto& filter : filters)
  {
    if(k_BlackList.find(filter->getNameOfClass().toStdString()) == k_BlackList.end())
    {
      QString prefix = "    #";
      QString suffix = "    # MISSING 1 or more Parameter Implementations";
      if(s_FilterHasAllParameters[filter->getNameOfClass().toStdString()])
      {
        prefix = "    ";
        suffix = "";
      }
      hOut << prefix << filter->getNameOfClass() << "Test.cpp" << suffix << "\n";
      GenerateUnitTestSourceFile(filter, outputDir, pluginName);
    }
  }

  cmakeTemplate = cmakeTemplate.replace("@FILTER_LIST@", filterList);
  writeTopLevelOutput(pluginName, "/tmp", cmakeTemplate, "test/CMakeLists.txt");

  // Write the 'Catch2' main file
  QString catch2Main;
  QTextStream c2mOut(&catch2Main);

  c2mOut << "// Catch2 recommends placing these lines by themselves in a translation unit\n";
  c2mOut << "// which will help reduce unnecessary recompilations of the expensive Catch main\n";
  c2mOut << "#define CATCH_CONFIG_MAIN\n";
  c2mOut << "#include <catch2/catch.hpp>\n";

  QString outputFileName = QString("test/%1_test_main.cpp").arg(pluginName);

  writeTopLevelOutput(pluginName, "/tmp", catch2Main, outputFileName);

  // Write the cmake configured header template file
  QString testDirsTemplate = ReadTemplateFile(k_TestDirsHeaderFile);
  outputFileName = QString("test/test_dirs.hpp.in");
  writeTopLevelOutput(pluginName, "/tmp", testDirsTemplate, outputFileName);
}

// -----------------------------------------------------------------------------
void GenerateCMakeFile(const QString& pluginName, std::vector<AbstractFilter::Pointer>& filters)
{
  QString cmakeTemplate = ReadTemplateFile(k_CMakeListsFile);
  cmakeTemplate = cmakeTemplate.replace(k_PLUGIN_NAME, pluginName);

  QString filterList;
  QTextStream hOut(&filterList);

  for(const auto& filter : filters)
  {
    if(k_BlackList.find(filter->getNameOfClass().toStdString()) == k_BlackList.end())
    {
      QString prefix = "  #";
      QString suffix = " # MISSING 1 or more Parameter Implementations";
      if(s_FilterHasAllParameters[filter->getNameOfClass().toStdString()])
      {
        prefix = "  ";
        suffix = "";
      }
      hOut << prefix << filter->getNameOfClass() << suffix << "\n";
    }
  }

  cmakeTemplate = cmakeTemplate.replace("@FILTER_LIST@", filterList);

  writeTopLevelOutput(pluginName, "/tmp", cmakeTemplate, "CMakeLists.txt");
}

// -----------------------------------------------------------------------------
void GeneratePluginHeader(const QString& pluginName, std::vector<AbstractFilter::Pointer>& filters)
{
  QString cmakeTemplate = ReadTemplateFile(k_PluginHeaderFile);
  cmakeTemplate = cmakeTemplate.replace(k_PLUGIN_NAME, pluginName);
  QString pluginNameUpper = pluginName.toUpper();
  cmakeTemplate = cmakeTemplate.replace("@PLUGIN_NAME_UPPER@", pluginNameUpper);

  writeTopLevelOutput(pluginName, "/tmp", cmakeTemplate, "src/" + pluginName + "/" + pluginName + "Plugin.hpp");
}

// -----------------------------------------------------------------------------
void GeneratePluginSource(const QString& pluginName, std::vector<AbstractFilter::Pointer>& filters)
{
  QString cmakeTemplate = ReadTemplateFile(k_PluginSourceFile);
  cmakeTemplate = cmakeTemplate.replace(k_PLUGIN_NAME, pluginName);

  PluginManager* pluginManager = PluginManager::Instance();
  QVector<ISIMPLibPlugin*> plugins = pluginManager->getPluginsVector();
  ISIMPLibPlugin* currentPlugin = nullptr;
  for(const auto& p : plugins)
  {
    if(p->getPluginBaseName() == pluginName)
    {
      currentPlugin = p;
    }
  }
  QString plugDescription = "Description";
  QString plugOrg = "BlueQuartz Software";
  if(currentPlugin != nullptr)
  {
    plugDescription = currentPlugin->getDescription();
    plugOrg = currentPlugin->getVendor();
  }

  cmakeTemplate = cmakeTemplate.replace(k_PLUGIN_DESCRIPTION, plugDescription);
  cmakeTemplate = cmakeTemplate.replace(k_PLUGIN_VENDOR, plugOrg);

  QString filterHeaderList;
  QTextStream hOut(&filterHeaderList);
  for(const auto& filter : filters)
  {
    if(k_BlackList.find(filter->getNameOfClass().toStdString()) == k_BlackList.end())
    {
      QString prefix = "//";
      if(s_FilterHasAllParameters[filter->getNameOfClass().toStdString()])
      {
        prefix = "";
      }
      hOut << prefix << "#include \"" << pluginName << "/Filters/" << filter->getNameOfClass() << ".hpp\"\n";
    }
  }

  cmakeTemplate = cmakeTemplate.replace("@FILTER_INCLUDE_LIST@", filterHeaderList);

  filterHeaderList.clear();
  QMapIterator<QString, QString> i(s_FilterUuidMapping);
  hOut << "// This maps previous filters from DREAM.3D Version 6.x to DREAM.3D Version 7.x\n";
  hOut << "std::map<complex::Uuid, complex::Uuid> k_SimplToComplexFilterMapping = {\n";
  hOut << s_PrevToNewUuidMapping;
  //  while(i.hasNext())
  //  {
  //    i.next();
  //    hOut << "{Uuid::FromString(\"" << i.key() << "\").value(), Uuid::FromString(\"" << i.value() << "\").value()}, \n";
  //  }
  hOut << "};\n";

  QString uuid = GenerateUuid(SIMPL_to_Complex);
  hOut << "// Plugin Uuid\n";
  hOut << "  "
       << "constexpr AbstractPlugin::IdType k_ID = *Uuid::FromString(\"" << uuid << "\");\n";
  cmakeTemplate = cmakeTemplate.replace("@FILTER_UUID_LIST@", filterHeaderList);

  filterHeaderList.clear();
  i.toFront();
  while(i.hasNext())
  {
    i.next();
    QString prefix = "//";
    if(s_FilterHasAllParameters[i.key().toStdString()])
    {
      prefix = "  ";
    }
    hOut << prefix << "addFilter([]() -> IFilter::UniquePointer { return std::make_unique<" << i.key() << ">(); });\n";
  }
  cmakeTemplate = cmakeTemplate.replace("@FILTER_REGISTER_LIST@", filterHeaderList);

  writeTopLevelOutput(pluginName, "/tmp", cmakeTemplate, "src/" + pluginName + "/" + pluginName + "Plugin.cpp");
}

void GenerateMarkdownSummary(const QString& pluginName, std::vector<AbstractFilter::Pointer>& filters)
{
  QString md;
  QTextStream mdOut(&md);

  mdOut << "# " << pluginName << " #\n\n";
  mdOut << "|  Filter Name | All Parameters Working | DataCheck Implemented | Execute Implemented | Documentation Implemented |\n";
  mdOut << "|--------------|------------------------|-----------------------|---------------------|---------------------------|\n";
  int32_t goodCount = 0;
  int32_t badCount = 0;
  for(const auto& filter : filters)
  {
    if(k_BlackList.find(filter->getNameOfClass().toStdString()) == k_BlackList.end())
    {
      QString prefix = "|   ";
      mdOut << "| " << filter->getNameOfClass() << " |";
      if(s_FilterHasAllParameters[filter->getNameOfClass().toStdString()])
      {
        mdOut << " TRUE ";
        goodCount++;
      }
      else
      {
        mdOut << " FALSE ";
        badCount++;
      }
      mdOut << " | | | |\n";
    }
  }

  mdOut << "\n\n";
  mdOut << "## Summary ##\n\n";
  mdOut << "+ Filters Ready to be Ported: " << goodCount << "\n";
  mdOut << "+ Filters Needing more Parameters Implemented: " << badCount << "\n";

  writeTopLevelOutput(pluginName, "/tmp", md, "FilterGuide.md");
}
// -----------------------------------------------------------------------------
void GenerateComplexFilters()
{
  qDebug() << "-------------- PrintFilterInfo ------------------------------";
  QString outputDir = "/tmp";

  FilterManager* fm = FilterManager::Instance();
  QSet<QString> pluginNames = fm->getPluginNames();
  for(const auto& pluginName : pluginNames)
  {
    // s_CurrentPlugin = pluginName;
    //  if(pluginName != "EMMPM")
    //  {
    //    continue;
    //  }
    std::cout << "<================================" << pluginName.toStdString() << "================================>" << std::endl;
    std::vector<AbstractFilter::Pointer> filters;
    s_FilterUuidMapping.clear();
    s_PrevToNewUuidMapping.clear();

    s_FilterHasAllParameters.clear();

    FilterManager::Collection factories = fm->getFactoriesForPluginName(pluginName);
    FilterManager::Collection::const_iterator factoryMapIter = factories.constBegin();
    while(factoryMapIter != factories.constEnd())
    {
      IFilterFactory::Pointer factory = fm->getFactoryFromClassName(factoryMapIter.key());
      if(factory.get() != nullptr)
      {
        AbstractFilter::Pointer filter = factory->create();

        if(k_BlackList.find(filter->getNameOfClass().toStdString()) == k_BlackList.end())
        {
          GenerateSourceFile(filter.get(), outputDir, pluginName);
          GenerateHeaderFile(filter.get(), outputDir, pluginName);
          GenerateAlgorithmFile(filter.get(), outputDir, pluginName);

          filters.push_back(filter);
        }
      }
      factoryMapIter++;
    }

    //    GenerateCMakeFile(s_CurrentPlugin, filters);
    //    GeneratePluginHeader(s_CurrentPlugin, filters);
    //    GeneratePluginSource(s_CurrentPlugin, filters);
    //    GenerateMarkdownSummary(s_CurrentPlugin, filters);
    //    GenerateUnitTestCMakeFile(s_CurrentPlugin, filters);
  }
  //  for(const auto& pName : paramNames)
  //  {
  //    std::cout << "s_ParameterMapping[\"" << pName.toStdString() << "\"] = \"" << pName.toStdString() << "\";"<< std::endl;
  //  }
}

void GenerateComplexFilter(const QString& outputDir, const QString& pluginName, const QString& inputClassName)
{
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(inputClassName);
  if(nullptr == factory)
  {
    std::cout << "Input ClassName: " << inputClassName.toStdString() << " is not a known C++ Class name for a filter." << std::endl;
    return;
  }
  AbstractFilter::Pointer filter = factory->create();
  GenerateSourceFile(filter.get(), outputDir, pluginName);
  GenerateHeaderFile(filter.get(), outputDir, pluginName);
  GenerateAlgorithmFile(filter.get(), outputDir, pluginName);
  GenerateUnitTestSourceFile(filter, outputDir, pluginName);

  QString docFile = D3DTools::GetDREAM3DBinaryRefManualDir() + "/" + filter->getPluginInstance()->getPluginBaseName() + "_Filters/" + filter->getNameOfClass() + ".md";
  QFileInfo docFileInfo(docFile);
  if(docFileInfo.exists())
  {
    std::cout << "Documentation Exists.. " << std::endl;
    QString sourceTemplate = ReadTemplateFile(docFile);
    QString filterName = filter->getNameOfClass();
    if(filterName.endsWith("Filter"))
    {
      filterName.replace("Filter", "");
    }
    writeTopLevelOutput(outputDir, pluginName, sourceTemplate, "docs/" + filterName + "Filter.md");
  }
}

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  const size_t k_InputClassName = 0;
  const size_t k_OutputDir = 1;
  const size_t k_HelpIndex = 2;

  using ArgEntry = std::vector<std::string>;
  using ArgEntries = std::vector<ArgEntry>;

  ArgEntries args;

  args.push_back({"-c", "--classname", "The Class Name of the filter to port"});
  args.push_back({"-o", "--outputdir", "The output plugin directory to write the new files"});

  args.push_back({"-h", "--help", "Show help for this program"});

  QString inputClassName;
  QString outputDir;

  //bool header = false;

  for(int32_t i = 0; i < argc; i++)
  {
    if(argv[i] == args[k_InputClassName][0] || argv[i] == args[k_InputClassName][1])
    {
      inputClassName = argv[++i];
    }
    if(argv[i] == args[k_OutputDir][0] || argv[i] == args[k_OutputDir][1])
    {
      outputDir = argv[++i];
    }

    if(argv[i] == args[k_HelpIndex][0] || argv[i] == args[k_HelpIndex][1])
    {
      std::cout << "This program has the following arguments:" << std::endl;
      for(const auto& input : args)
      {
        std::cout << input[0] << ", " << input[1] << ": " << input[2] << std::endl;
      }
      return 0;
    }
  }

  if(argc != 5)
  {
    std::cout << "4 Arguments are required. Use --help for more information." << std::endl;
    return EXIT_FAILURE;
  }

  InitParameterMapping();
  InitIncludeMapping();
  InitDefaultParameterConstructor();
  InitParameterTypeMapping();
  InitParameterAvailableMapping();
  InitParameterCount();
  InitUnitTestParameterConstruction();

  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("ComplexCodeGen");

  // Load all the plugins and
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);
  // THIS IS A VERY IMPORTANT LINE: It will register all the known filters in the dream3d library. This
  // will NOT however get filters from plugins. We are going to have to figure out how to compile filters
  // into their own plugin and load the plugins from a command line.
  fm->RegisterKnownFilters(fm);

  QMetaObjectUtilities::RegisterMetaTypes();

  QFileInfo fi(outputDir);
  QString pluginName = outputDir.split("/").last();
  GenerateComplexFilter(outputDir, pluginName, inputClassName);

  std::cout << "You will need to update the following files:" << std::endl;
  std::cout << outputDir.toStdString() << "/CMakeLists.txt to include the filter and the algorithm. Look for the line\n"
            << "  'set(FilterList' and add the name of the filter on a new line AFTER that line.\n"
            << "    The entry in that list should be '" << inputClassName.toStdString() << "Filter'\n"
            << "  'set(filter_algorithms' and add the name of the filter there.\n"
            << "    The entry in that list should be '" << inputClassName.toStdString() << "'\n"
            << outputDir.toStdString() << "/test/CMakeLists.txt\n"
            << "  'set(${PLUGIN_NAME}UnitTest_SRCS' and add the name of the filter on a new line AFTER that line.\n"
            << "    The entry in that list should be '" << inputClassName.toStdString() << "Test.cpp'\n";

  return EXIT_SUCCESS;
}

#if 0
  GenerateComplexFilters();
  std::cout << "#########################################################" << std::endl;
  std::cout << "Total Good Filters: " << s_TotalGoodFilters << std::endl;
  std::cout << "Total Bad Filters:  " << s_TotalBadFilters << std::endl;

  QMapIterator<QString, int> i(s_ParameterCount);
  while(i.hasNext())
  {
    i.next();
    if(s_ParameterAvailable[i.key()])
    {
          std::cout << i.key().toStdString() << "\t" << i.value() << std::endl;
      }
  }

  std::cout << "########################################################" << std::endl;
  i.toFront();
  while(i.hasNext())
  {
    i.next();
    if(!s_ParameterAvailable[i.key()])
    {
      std::cout << i.key().toStdString() << "\t" << i.value() << std::endl;
    }
  }

#endif
