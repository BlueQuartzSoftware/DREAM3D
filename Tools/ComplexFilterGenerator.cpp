
#include <stdlib.h>

#include <iostream>
#include <set>
#include <string>

using namespace std::string_literals;


#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QMetaProperty>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <sys/_types/_int32_t.h>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"

#include "DREAM3DToolsConfiguration.h"

const std::set<std::string> k_BlackList = { "ConditionalSetValue", "CreateImageGeometry"};

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

#if 1
const QString k_OutputDir("/tmp");
const QString k_PluginsOutputDir("/Users/mjackson/Workspace1/complex_plugins");
#else
const QString k_OutputDir("/tmp/");
const QString k_PluginsOutputDir("/tmp/complex_plugins");
#endif
static QString s_CurrentPlugin = "";
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
const QString k_UUID("@UUID@");
const QString k_PARAMETER_KEYS("@PARAMETER_KEYS@");
const QString k_PARAMETER_DEFS("@PARAMETER_DEFS@");
const QString k_FILTER_HUMAN_NAME("@FILTER_HUMAN_NAME@");
const QString k_PARAMETER_INCLUDES("@PARAMETER_INCLUDES@");
const QString k_PREFLIGHT_DEFS("@PREFLIGHT_DEFS@");
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


void InitParameterCount ()
{
  s_ParameterCount["BooleanFilterParameter"] = 0;
  s_ParameterCount["DataArrayCreationFilterParameter"] = 0;
  s_ParameterCount["DataArraySelectionFilterParameter"] = 0;
  s_ParameterCount["ChoiceFilterParameter"] =0;
  s_ParameterCount["AttributeMatrixCreationFilterParameter"] = 0;
  s_ParameterCount["AttributeMatrixSelectionFilterParameter"] = 0;
  s_ParameterCount["DataContainerCreationFilterParameter"] =0;
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
  s_ParameterCount["MultiDataArraySelectionFilterParameter"] =0;
  s_ParameterCount["LinkedBooleanFilterParameter"] = 0;
  s_ParameterCount["LinkedChoicesFilterParameter"] = 0;
  s_ParameterCount["PreflightUpdatedValueFilterParameter"] = 0;

  // No COMPLEX Type Yet
  s_ParameterCount["CalculatorFilterParameter"] = 0;
  s_ParameterCount["ComparisonSelectionAdvancedFilterParameter"] =0;
  s_ParameterCount["ComparisonSelectionFilterParameter"] = 0;
  s_ParameterCount["ConvertHexGridToSquareGridFilterParameter"] =0;
  s_ParameterCount["DataContainerArrayProxyFilterParameter"] = 0;
  s_ParameterCount["DataContainerReaderFilterParameter"] = 0;
  s_ParameterCount["DynamicChoiceFilterParameter"] = 0;
  s_ParameterCount["DynamicTableFilterParameter"] = 0;
  s_ParameterCount["EMMPMFilterParameter"] = 0;
  s_ParameterCount["EbsdMontageImportFilterParameter"] = 0;
  s_ParameterCount["EbsdToH5EbsdFilterParameter"] = 0;
  s_ParameterCount["EbsdWarpPolynomialFilterParameter"] = 0;
  s_ParameterCount["EnsembleInfoFilterParameter"] =0;
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

void InitParameterTypeMapping ()
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


void InitDefaultParameterConstructor ()
{
  s_DefaultConstructorMapping["BooleanFilterParameter"] = "false";
  s_DefaultConstructorMapping["DataArrayCreationFilterParameter"] = "DataPath{}";
  s_DefaultConstructorMapping["DataArraySelectionFilterParameter"] = "DataPath{}";
  s_DefaultConstructorMapping["ChoiceFilterParameter"] = "0, ChoicesParameter::Choices{\"Option 1\", \"Option 2\", \"Option 3\"}";
  s_DefaultConstructorMapping["AttributeMatrixCreationFilterParameter"] = "DataPath{}";
  s_DefaultConstructorMapping["AttributeMatrixSelectionFilterParameter"] = "DataPath{}";
  s_DefaultConstructorMapping["DataContainerCreationFilterParameter"] = "DataPath{}";
  s_DefaultConstructorMapping["DataContainerSelectionFilterParameter"] = "DataPath{}";
  s_DefaultConstructorMapping["InputFileFilterParameter"] = "fs::path(\"<default file to read goes here>\"), FileSystemPathParameter::PathType::InputFile";
  s_DefaultConstructorMapping["InputPathFilterParameter"] = "fs::path(\"<default file to read goes here>\"), FileSystemPathParameter::PathType::InputDir";
  s_DefaultConstructorMapping["OutputFileFilterParameter"] = "fs::path(\"<default file to read goes here>\"), FileSystemPathParameter::PathType::OutputFile";
  s_DefaultConstructorMapping["OutputPathFilterParameter"] = "fs::path(\"<default file to read goes here>\"), FileSystemPathParameter::PathType::OutputDir";
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


void InitUnitTestParameterConstruction ()
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
    if (parameter->getPropertyName() == property &&
        (origParamClassName == "PreflightUpdatedValueFilterParameter" || origParamClassName == "SeparatorFilterParameter"))
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
void writeTopLevelOutput(const QString& pluginName, const QString& outLines, const QString& fileName)
{
  QString filtersDir = "/Filters/";
  if(!s_HasAllParameters)
  {
    filtersDir = "/Filters-Disabled/";
  }

  // Assume we are Core first...
  QString outputPath;
  if(s_CurrentPlugin == "AFRLDistributionC" 
  || s_CurrentPlugin == "CRADA3DSystems" 
  || s_CurrentPlugin == "ECAccess" 
  || s_CurrentPlugin == "BrukerIntegration" 
  || s_CurrentPlugin == "CAxisByPolarizedLight" 
  || s_CurrentPlugin == "NETLIntegration" 
  )
  {
   outputPath = k_PluginsOutputDir + "_private/" + s_CurrentPlugin;
  }
  else
  {
    outputPath = k_PluginsOutputDir + "/" + s_CurrentPlugin;
  }

  // Generate the Output File Path
  QString finalOutPath = outputPath + "/" + fileName;
  QFileInfo fi(finalOutPath);
  outputPath = fi.absolutePath();

  // Make sure the output directory structure is created.
  QDir outputDir(outputPath);
  outputDir.mkpath(outputPath);

  // Write the output
  QFile hOut(finalOutPath);
  hOut.open(QFile::WriteOnly);
  QTextStream stream(&hOut);
  stream << outLines;
  hOut.close();
  // qDebug() << "Saved File " << fi.absoluteFilePath();
}

// -----------------------------------------------------------------------------
void writeOutput( AbstractFilter* filter, const QString& outLines, const QString& ext, const QString& mdSummary)
{
  QString filtersDir = "/Filters/";
  if(!s_HasAllParameters)
  {
    filtersDir = "/Filters-Disabled/";
  }
 
  // Assume we are Core first...
  QString outputPath;
  QString mdOutputPath;
 if(s_CurrentPlugin == "AFRLDistributionC" 
  || s_CurrentPlugin == "CRADA3DSystems" 
  || s_CurrentPlugin == "ECAccess" 
  || s_CurrentPlugin == "BrukerIntegration" 
  || s_CurrentPlugin == "CAxisByPolarizedLight" 
  || s_CurrentPlugin == "NETLIntegration" 
  )
  {
    outputPath = k_PluginsOutputDir + "_private/" + s_CurrentPlugin + "/src/" + s_CurrentPlugin + "/" + filtersDir;
    mdOutputPath = k_PluginsOutputDir + "_private/" + s_CurrentPlugin + "/docs/" + s_CurrentPlugin + "/";
  }
  else
  {
    outputPath = k_PluginsOutputDir + "/" + s_CurrentPlugin + "/src/" + s_CurrentPlugin + "/" + filtersDir;
    mdOutputPath = k_PluginsOutputDir + "/" + s_CurrentPlugin + "/docs/" + s_CurrentPlugin + "/";
  }

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
  if(!mdSummary.isEmpty())
  {
    QDir outputDir(mdOutputPath);
    outputDir.mkpath(mdOutputPath);
    // Generate the Output File Path
    QString finalOutPath = mdOutputPath + filter->getNameOfClass() + ".md";
    // std::cout << "Writing MD File: " << finalOutPath.toStdString() << std::endl;
    QFileInfo fi(finalOutPath);

    // Write the output
    QFile hOut(finalOutPath);
    hOut.open(QFile::WriteOnly);
    QTextStream stream(&hOut);
    stream << mdSummary;
    hOut.close();
  }
}

QString GenerateUuid(const QString& libName, const QString& className)
{
  uint l = 100;
  ushort w1 = 200;
  ushort w2 = 300;
  uchar b[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  int32_t i = 0;
  while(i < 8 && i < libName.size())
  {
    b[i] = static_cast<uint8_t>(libName.at(i).toLatin1());
    i++;
  }
  QUuid uuid = QUuid(l, w1, w2, b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7]);
  QString nameSpace;
  QTextStream out(&nameSpace);
  out << className << " " << libName;
  QUuid p1 = QUuid::createUuidV5(uuid, nameSpace);

  //qDebug() << p1.toString();
  return p1.toString().replace("{", "").replace("}", "");
}

//std::vector<QString> sepKeys;
// -----------------------------------------------------------------------------
void GenerateHeaderFile(AbstractFilter* filter, const QString& pluginName)
{
  QString headerTemplate = ReadTemplateFile(k_HeaderFile);
  QString filterName = filter->getNameOfClass();
  QString humanName = filter->getHumanLabel();
  QString uuid = filter->getUuid().toString();//GenerateUuid(filter->getCompiledLibraryName(), filter->getNameOfClass());
  QString prevUuid = filter->getUuid().toString();
  uuid = uuid.remove("{");
  uuid = uuid.remove("}");
  s_FilterUuidMapping[prevUuid] = uuid;
  QTextStream ptonew(&s_PrevToNewUuidMapping);

  ptonew << "{Uuid::FromString(\"" << prevUuid << "\").value(), Uuid::FromString(\"" << uuid << "\").value()}, ";
  ptonew << "/* " << filterName << " */\n";

  headerTemplate = headerTemplate.replace(k_FILTER_NAME, filterName);
  headerTemplate = headerTemplate.replace(k_UUID, uuid);
  headerTemplate = headerTemplate.replace(k_PLUGIN_NAME, pluginName);
  QString pluginNameUpper = pluginName.toUpper();
  headerTemplate = headerTemplate.replace("@PLUGIN_NAME_UPPER@", pluginNameUpper);

  // Generate the Parameter keys
  FilterParameterVectorType parameters = filter->getFilterParameters();
  QString pString;
  QTextStream out(&pString);
  for(const auto& parameter : parameters)
  {
    QString origParamClassName = parameter->getNameOfClass();
    QString propName = parameter->getPropertyName();
    if(origParamClassName != "SeparatorFilterParameter" && origParamClassName != "PreflightUpdatedValueFilterParameter")
    {
      out << "  static inline constexpr StringLiteral k_" << propName << "_Key = \"" << propName << "\";\n";
    }
  }
  headerTemplate = headerTemplate.replace(k_PARAMETER_KEYS, pString);

  writeOutput(filter, headerTemplate, ".hpp", "");
}

// -----------------------------------------------------------------------------
void GenerateAlgorithmFile(AbstractFilter* filter)
{

  QString filterName = filter->getNameOfClass();
  QString humanName = filter->getHumanLabel();
  QString pluginName = filter->getCompiledLibraryName();
  QString pluginNameUpper = pluginName.toUpper();

  QString sourceTemplate = ReadTemplateFile(k_AlgorithmSourceFile);
  QString headerTemplate = ReadTemplateFile(k_AlgorithmHeaderFile);

  sourceTemplate = sourceTemplate.replace(k_FILTER_NAME, filterName);
  sourceTemplate = sourceTemplate.replace(k_PLUGIN_NAME, pluginName);

  headerTemplate = headerTemplate.replace(k_FILTER_NAME, filterName);
  headerTemplate = headerTemplate.replace(k_PLUGIN_NAME, pluginName);
  headerTemplate = headerTemplate.replace("@PLUGIN_NAME_UPPER@", pluginNameUpper);

  // Generate the Parameter Section
  FilterParameterVectorType parameters = filter->getFilterParameters();
  QString pString;
  QTextStream parameterOut(&pString);

  QString executeString;
  QTextStream executeOut(&executeString);

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
    bool endsWithValue = propName.endsWith("Value");

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
      //  includeOut << "#include \"complex/Parameters/" << propInclude << ".hpp\"\n";
    }

    if(propInclude == "FileSystemPathParameter")
    {
      //  needsFs = true;
    }
    //  includeSet.insert(incString);
    //  incString.clear();
  }

  executeOut << "\n  return " << filterName << "(dataStructure, messageHandler, shouldCancel, &inputValues)();\n";

  headerTemplate = headerTemplate.replace("@INPUT_VALUE_STRUCT_DEF@", pString);
  headerTemplate = headerTemplate.replace("@EXECUTE_EXAMPLE_CODE@", executeString);

  writeTopLevelOutput(pluginName, sourceTemplate, "src/" + pluginName + "/Filters_Private/Algorithms/" + filterName + ".cpp");
  writeTopLevelOutput(pluginName, headerTemplate, "src/" + pluginName + "/Filters_Private/Algorithms/" + filterName + ".hpp");
}

// -----------------------------------------------------------------------------
void GenerateSourceFile(AbstractFilter* filter)
{
  QString sourceTemplate = ReadTemplateFile(k_SourceFile);
  QString filterName = filter->getNameOfClass();
  QString humanName = filter->getHumanLabel();
  QString pluginName = filter->getCompiledLibraryName();

  QString defaultTags = QString("\"#%1\", \"#%2\"").arg(filter->getGroupName(), filter->getSubGroupName());
  if(filter->getSubGroupName() == SIMPL::FilterSubGroups::InputFilters)
  {
    defaultTags = defaultTags + ", \"#Read\", \"#Import\"";
  }
  if(filter->getSubGroupName() == SIMPL::FilterSubGroups::OutputFilters)
  {
    defaultTags = defaultTags + ", \"#Write\", \"#Export\"";
  }

  sourceTemplate = sourceTemplate.replace(k_FILTER_NAME, filterName);
  sourceTemplate = sourceTemplate.replace(k_FILTER_HUMAN_NAME, humanName);
  sourceTemplate = sourceTemplate.replace(k_DEFAULT_TAGS, defaultTags);


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
  QMap<QString, int> propToGroupMap;
  for(const auto& parameter : parameters)
  {
    QString propName = parameter->getPropertyName();
    int32_t groupIndex = parameter->getGroupIndex();
    propToGroupMap[propName] = groupIndex;
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

    s_ParameterCount[origParamClassName]++;

    if(!hasParameter)
    {
      s_HasAllParameters = false;
    }

    if(origParamClassName != "SeparatorFilterParameter" && origParamClassName != "PreflightUpdatedValueFilterParameter")
    {
      preFlightOut << "  auto p" << propName << (endsWithValue ? "":"Value") <<  " = filterArgs.value<" << paramType << ">(k_" << propName << "_Key);\n";
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
          linkedOut << "  params.linkParameters(k_" << propName << "_Key, k_" << linkedProp << "_Key, " << propToGroupMap[linkedProp] << ");\n";
          i++;
        }
      }
      includeOut << "#include \"complex/Parameters/" << propInclude << ".hpp\"\n";
    }
    else if(origParamClassName == "PreflightUpdatedValueFilterParameter")
    {
      PreflightUpdatedValueFilterParameter::Pointer fp = std::dynamic_pointer_cast<PreflightUpdatedValueFilterParameter>(parameter);
      QString lower = fp->getPropertyName();
      lower[0] = lower[0].toLower();
      pfud << "  std::string "<< lower << ";\n";
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
      actionsOut << "  {\n    // auto createArrayAction = std::make_unique<CreateArrayAction>(complex::NumericType::FILL_ME_IN, std::vector<usize>{NUM_TUPLES_VALUE}, NUM_COMPONENTS, p" << propName << (endsWithValue ? "":"Value") <<  ");\n";
      actionsOut << "    // resultOutputActions.value().actions.push_back(std::move(createArrayAction));\n  }\n";
      includeSet.insert("#include \"complex/Filter/Actions/CreateArrayAction.hpp\"\n");
      dataArrayCreation++;
    }
    else if(origParamClassName == "AttributeMatrixCreationFilterParameter")
    {
      actionsOut << "  {\n    auto createDataGroupAction = std::make_unique<CreateDataGroupAction>(p" << propName << (endsWithValue ? "":"Value") <<  ");\n";
      actionsOut << "    resultOutputActions.value().actions.push_back(std::move(createDataGroupAction));\n  }\n";
      includeSet.insert("#include \"complex/Filter/Actions/CreateDataGroupAction.hpp\"\n");
      dataArrayCreation++;
    }
    else if(origParamClassName == "DataContainerCreationFilterParameter")
    {
      actionsOut << "  {\n    auto createDataGroupAction = std::make_unique<CreateDataGroupAction>(p" << propName << (endsWithValue ? "":"Value") <<  ");\n";
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
    preflightUpdatedDefs = QString("  // These variables should be updated with the latest data generated for each variable during preflight. \n")
                         + QString("  // These will be returned through the preflightResult variable to the\n")
                         + QString("  // user interface. You could make these member variables instead if needed.\n") + preflightUpdatedDefs;
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
    std::cout << filter->getNameOfClass().toStdString() << std::endl;
  }
  else {
    s_TotalBadFilters++;
  }
  s_FilterHasAllParameters[filter->getNameOfClass().toStdString()] = s_HasAllParameters;
  writeOutput(filter, sourceTemplate, ".cpp", mdParamString);

 // writeTopLevelOutput(pluginName, sourceTemplate, "src/" + pluginName + "/Filters/Algorithms/" + filterName + ".cpp");
}

// -----------------------------------------------------------------------------
void GenerateUnitTestSourceFile(const AbstractFilter::Pointer& filter)
{
  QString sourceTemplate = ReadTemplateFile(k_UnitTestSourceFile);
  QString filterName = filter->getNameOfClass();
  QString humanName = filter->getHumanLabel();
  QString DEFAULT_VALUE = "{0}";
  // Generate the Parameter Section
  FilterParameterVectorType parameters = filter->getFilterParameters();

  sourceTemplate = sourceTemplate.replace(k_FILTER_NAME, filterName);
  sourceTemplate = sourceTemplate.replace(k_FILTER_HUMAN_NAME, humanName);
  sourceTemplate = sourceTemplate.replace(k_PLUGIN_NAME, s_CurrentPlugin);

  QString pString;
  QTextStream parameterOut(&pString);

  QString linkString;
  QTextStream linkedOut(&linkString);

  QString incString;
  QTextStream includeOut(&incString);
  QSet<QString> includeSet;

  QString preflightString;
  QTextStream preFlightOut(&preflightString);

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

      mdParamOut << "| " << (hasParameter ? "YES" : "NO") << " | " << propName << " | " << propHuman << " | " << paramType << " | " << propClass << " |\n";
    }

    if(origParamClassName == "SeparatorFilterParameter")
    {
      //parameterOut << "  params.insertSeparator(Parameters::Separator{\"" << propHuman << "\"});\n";
    }
    else if(origParamClassName == "LinkedBooleanFilterParameter")
    {
      parameterOut << "  args.insertOrAssign(" << filterName << "::k_" << propName << "_Key, std::make_any<" << paramType << ">(" << unitTestDefaultValue << "));\n";
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
      parameterOut << "  args.insertOrAssign(" << filterName << "::k_" << propName << "_Key, std::make_any<" << paramType << ">(" << unitTestDefaultValue << "));\n";
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

  QString outputFileName = QString("test/%1Test.cpp").arg(filterName);
  writeTopLevelOutput(s_CurrentPlugin, sourceTemplate, outputFileName);
}

// -----------------------------------------------------------------------------
void GenerateUnitTestCMakeFile(const QString& pluginName, std::vector<AbstractFilter::Pointer>& filters)
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
      GenerateUnitTestSourceFile(filter);
    }
  }

  cmakeTemplate = cmakeTemplate.replace("@FILTER_LIST@", filterList);
  writeTopLevelOutput(pluginName, cmakeTemplate, "test/CMakeLists.txt");


  // Write the 'Catch2' main file
  QString catch2Main;
  QTextStream c2mOut(&catch2Main);

  c2mOut <<"// Catch2 recommends placing these lines by themselves in a translation unit\n";
  c2mOut <<"// which will help reduce unnecessary recompilations of the expensive Catch main\n";
  c2mOut <<"#define CATCH_CONFIG_MAIN\n";
  c2mOut <<"#include <catch2/catch.hpp>\n";

  QString outputFileName = QString("test/%1_test_main.cpp").arg(s_CurrentPlugin);

  writeTopLevelOutput(pluginName, catch2Main, outputFileName);


  // Write the cmake configured header template file
  QString testDirsTemplate = ReadTemplateFile(k_TestDirsHeaderFile);
  outputFileName = QString("test/test_dirs.hpp.in");
  writeTopLevelOutput(pluginName, testDirsTemplate, outputFileName);

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

  writeTopLevelOutput(pluginName, cmakeTemplate, "CMakeLists.txt");
}

// -----------------------------------------------------------------------------
void GeneratePluginHeader(const QString& pluginName, std::vector<AbstractFilter::Pointer>& filters)
{
  QString cmakeTemplate = ReadTemplateFile(k_PluginHeaderFile);
  cmakeTemplate = cmakeTemplate.replace(k_PLUGIN_NAME, pluginName);
  QString pluginNameUpper = pluginName.toUpper();
  cmakeTemplate = cmakeTemplate.replace("@PLUGIN_NAME_UPPER@", pluginNameUpper);

  writeTopLevelOutput(pluginName, cmakeTemplate, "src/" + pluginName + "/" + pluginName + "Plugin.hpp");
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

  QString uuid = GenerateUuid(pluginName, "SomeFilterThing");
  hOut << "// Plugin Uuid\n";
  hOut << "  " << "constexpr AbstractPlugin::IdType k_ID = *Uuid::FromString(\"" << uuid << "\");\n";
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
    hOut << prefix <<  "addFilter([]() -> IFilter::UniquePointer { return std::make_unique<" << i.key() << ">(); });\n";
  }
  cmakeTemplate = cmakeTemplate.replace("@FILTER_REGISTER_LIST@", filterHeaderList);

  writeTopLevelOutput(pluginName, cmakeTemplate, "src/" + pluginName + "/" + pluginName + "Plugin.cpp");
}


void GenerateMarkdownSummary(const QString& pluginName, std::vector<AbstractFilter::Pointer>& filters )
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

  writeTopLevelOutput(pluginName, md, "FilterGuide.md");
}
// -----------------------------------------------------------------------------
void GenerateComplexFilters()
{
  qDebug() << "-------------- PrintFilterInfo ------------------------------";
  FilterManager* fm = FilterManager::Instance();
  QSet<QString> pluginNames = fm->getPluginNames();
  for(const auto& pluginName : pluginNames)
  {
    s_CurrentPlugin = pluginName;
    // if(pluginName != "EMMPM")
    // {
    //   continue;
    // }
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
          // GenerateSourceFile(filter.get());
          // GenerateHeaderFile(filter.get(), s_CurrentPlugin);
          GenerateAlgorithmFile(filter.get());

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

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
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

  
  return EXIT_SUCCESS;
}
