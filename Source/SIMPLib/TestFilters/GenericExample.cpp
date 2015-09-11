/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#include "GenericExample.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/InputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/AxisAngleFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericExample::GenericExample() :
  AbstractFilter(),
  m_StlFilePrefix("Some Prefix"),
  m_ShowPrefix(false),
  m_MaxIterations(0),
  m_MisorientationTolerance(0),
  m_InputFile("/Some/Path/file.cpp"),
  m_InputPath("/Some/Path"),
  m_OutputFile("/Some/Path/Out.bin"),
  m_OutputPath("/Some/Path"),
  m_WriteAlignmentShifts(false),
  m_ConversionType(0),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, "Garbly Gook"),
  m_AttributeMatrixPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::NewCellFeatureAttributeMatrixName, ""),
  m_DataContainerName(DREAM3D::Defaults::StatsGenerator),
  m_CreatedDataArray(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::EulerColor),
  m_Bool1(false),
  m_Bool2(false),
  m_AlgorithmSelection(0),
  m_DistanceMetric(1)
{
  m_Dimensions.x = 0;
  m_Dimensions.y = 0;
  m_Dimensions.z = 0;

  m_Origin.x = 0.0;
  m_Origin.y = 0.0;
  m_Origin.z = 0.0;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericExample::~GenericExample()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::setupFilterParameters()
{
  FilterParameterVector parameters;
  /* Place all your option initialization code here */
  /* For String input use this code */
  parameters.push_back(StringFilterParameter::New("STL Output Prefix", "StlFilePrefix", getStlFilePrefix(), FilterParameter::Parameter));

  /*   For an output file use this code*/
  //parameters.push_back(OutputFileFilterParameter::New("Output File", "OutputFile", getOutputFile(), FilterParameter::Parameter));
  /*   For an output path use this code*/
  //parameters.push_back(OutputPathFilterParameter::New("Output Path", "OutputPath", getOutputPath(), FilterParameter::Parameter));
  /*   For a simple true/false boolean use this code*/
  parameters.push_back(BooleanFilterParameter::New("Write Alignment Shift File", "WriteAlignmentShifts", getWriteAlignmentShifts(), FilterParameter::Parameter));

  parameters.push_back(SeparatorFilterParameter::New("Choice Example", FilterParameter::Parameter));


  /*   For presenting a set of choices to the user use this code*/
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Conversion Type");
    parameter->setPropertyName("ConversionType");

    ////parameter->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("Degrees To Radians");
    choices.push_back("Radians To Degrees");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }


  /* Display a group of 3 text boxes to collect 3 integer values */
  parameters.push_back(IntVec3FilterParameter::New("Dimensions (XYZ)", "Dimensions", getDimensions(), FilterParameter::Parameter));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::Parameter, req));
  }


  {
    QStringList linkedProps;
    linkedProps << "Bool2" << "Double2";
    parameters.push_back(LinkedBooleanFilterParameter::New("Bool1", "Bool1", getBool1(), linkedProps, FilterParameter::Parameter));
    parameters.push_back(DoubleFilterParameter::New("Double 2", "Double2", getDouble2(), FilterParameter::Parameter));
  }

  {
    QStringList linkedProps;
    linkedProps << "AttributeMatrixPath";
    parameters.push_back(LinkedBooleanFilterParameter::New("Bool2", "Bool2", getBool2(), linkedProps, FilterParameter::Parameter));
  }
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Attribute Matrix", "AttributeMatrixPath", getAttributeMatrixPath(), FilterParameter::Parameter, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Linked Combo Box Example (1)", FilterParameter::Parameter));
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Select Distance Metric");
    parameter->setPropertyName("DistanceMetric");

    parameter->setDefaultValue(getDistanceMetric()); // Just set the first index

    QVector<QString> choices;
    choices.push_back("Alt Choice 0");
    choices.push_back("Alt Choice 1");
    choices.push_back("Alt Choice 2");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "MaxIterations" << "MisorientationTolerance" << "InputFile" << "InputPath";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);

    /*  For an Integer use this code*/
    parameters.push_back(IntFilterParameter::New("Max Iterations", "MaxIterations", getMaxIterations(), FilterParameter::Parameter, 0));
    /*  For a Floating point value use this code*/
    parameters.push_back(DoubleFilterParameter::New("Misorientation Tolerance", "MisorientationTolerance", getMisorientationTolerance(), FilterParameter::Parameter, 1));
    /*   For an input file use this code*/
    //parameters.push_back(InputFileFilterParameter::New("Input File", "InputFile", getInputFile(), FilterParameter::Parameter, "", "", 1));
    /*   For an input path use this code*/
    //parameters.push_back(InputPathFilterParameter::New("Input Path", "InputPath", getInputPath(), FilterParameter::Parameter, "", "", 2));
  }



  parameters.push_back(SeparatorFilterParameter::New("Linked Combo Box Example (2)", FilterParameter::Parameter));

  /*   For presenting a set of choices to the user use this code*/
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Select Algorithm");
    parameter->setPropertyName("AlgorithmSelection");

    parameter->setDefaultValue(getAlgorithmSelection()); // Just set the first index

    QVector<QString> choices;
    choices.push_back("Choice 0");
    choices.push_back("Choice 1");
    choices.push_back("Choice 2");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "CreatedDataArray" << "Origin" << "CrystalSymmetryRotations" << "DataContainerName";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);

    {
      DataArrayCreationFilterParameter::RequirementType req;
      parameters.push_back(DataArrayCreationFilterParameter::New("Created Data Array", "CreatedDataArray", getCreatedDataArray(), FilterParameter::Parameter, req, 0));
    }
    /* Display a group of 3 text boxes to collect 3 float values */
    parameters.push_back(FloatVec3FilterParameter::New("Origin", "Origin", getOrigin(), FilterParameter::Parameter, 1));

    /* Display the AxisAngleWidget to collect Axis-Angle pairs from the user */
    parameters.push_back(AxisAngleFilterParameter::New("Crystal Rotations", "CrystalSymmetryRotations", getCrystalSymmetryRotations(), FilterParameter::Parameter, 2));

    {
      DataContainerSelectionFilterParameter::RequirementType req;
      parameters.push_back(DataContainerSelectionFilterParameter::New("Data Container", "DataContainerName", getDataContainerName(), FilterParameter::Parameter, req, 2));
    }
  }
  QVector<DataArrayPath> paths;
  paths.push_back(DataArrayPath("StatsGeneratorDataContainer", "CellEnsembleData", "CrystalStructures"));
  paths.push_back(DataArrayPath("StatsGeneratorDataContainer", "CellEnsembleData", "Statistics"));

  {
    MultiDataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(MultiDataArraySelectionFilterParameter::New("Multi Data Array Test", "SelectedMultiArrayPaths", paths, FilterParameter::Parameter, req, 0));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{

  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setSelectedMultiArrayPaths(reader->readDataArrayPathVector("SelectedMultiArrayPaths", getSelectedMultiArrayPaths()));
  setStlFilePrefix( reader->readString("StlFilePrefix", getStlFilePrefix()) );
  setMaxIterations( reader->readValue("MaxIterations", getMaxIterations()) );
  setMisorientationTolerance( reader->readValue("MisorientationTolerance", getMisorientationTolerance()) );
  setInputFile( reader->readString("InputFile", getInputFile()) );
  setInputPath( reader->readString("InputPath", getInputPath()) );
  setOutputFile( reader->readString("OutputFile", getOutputFile()) );
  setOutputPath( reader->readString("OutputPath", getOutputPath()) );
  setWriteAlignmentShifts( reader->readValue("WriteAlignmentShifts", getWriteAlignmentShifts()) );
  setConversionType( reader->readValue("ConversionType", getConversionType()) );
  setDimensions( reader->readIntVec3("Dimensions", getDimensions()) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin()) );

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenericExample::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SelectedMultiArrayPaths)
  /* Place code that will write the inputs values into a file. reference the
       AbstractFilterParametersWriter class for the proper API to use. */
  SIMPL_FILTER_WRITE_PARAMETER(StlFilePrefix)
  SIMPL_FILTER_WRITE_PARAMETER(MaxIterations)
  SIMPL_FILTER_WRITE_PARAMETER(MisorientationTolerance)
  SIMPL_FILTER_WRITE_PARAMETER(InputFile)
  SIMPL_FILTER_WRITE_PARAMETER(InputPath)
  SIMPL_FILTER_WRITE_PARAMETER(OutputFile)
  SIMPL_FILTER_WRITE_PARAMETER(OutputPath)
  SIMPL_FILTER_WRITE_PARAMETER(WriteAlignmentShifts)
  SIMPL_FILTER_WRITE_PARAMETER(ConversionType)
  SIMPL_FILTER_WRITE_PARAMETER(Dimensions)
  SIMPL_FILTER_WRITE_PARAMETER(Origin)
  SIMPL_FILTER_WRITE_PARAMETER(CrystalSymmetryRotations)



  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::preflight()
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
void computeEulerAngle(float pq, float* eulerAngle)
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::execute()
{
  int err = 0;
  setErrorCondition(err);

  // Run the data check to get references to all of our data arrays initialized to the values stored in memory
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  qDebug() << "Feature Ids: " << getFeatureIdsArrayPath().getDataArrayName();
  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenericExample::newFilterInstance(bool copyFilterParameters)
{
  GenericExample::Pointer filter = GenericExample::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenericExample::getCompiledLibraryName()
{
  return Test::TestBaseName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenericExample::getGroupName()
{
  return DREAM3D::FilterGroups::TestFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenericExample::getSubGroupName()
{
  return "Misc";
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenericExample::getHumanLabel()
{
  return "Generic Example";
}

