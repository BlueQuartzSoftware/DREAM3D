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

#include "ConvertOrientations.h"

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"


#include "OrientationLib/OrientationMath/OrientationConverter.hpp"
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"
#include "OrientationAnalysis/OrientationAnalysisConstants.h"

// Include the MOC generated file for this class
#include "moc_ConvertOrientations.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertOrientations::ConvertOrientations() :
  AbstractFilter(),
  m_InputType(0),
  m_OutputType(1)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertOrientations::~ConvertOrientations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientations::setupFilterParameters()
{
  FilterParameterVector parameters;

  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Input Orientation Type");
    parameter->setPropertyName("InputType");

    QVector<QString> inputChoices = OrientationConverter<float>::GetOrientationTypeStrings();
    parameter->setChoices(inputChoices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Output Orientation Type");
    parameter->setPropertyName("OutputType");

    QVector<QString> inputChoices = OrientationConverter<float>::GetOrientationTypeStrings();
    parameter->setChoices(inputChoices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  {
    DataArraySelectionFilterParameter::RequirementType req;
    req.daTypes = QVector<QString>(2, DREAM3D::TypeNames::Double);
    req.daTypes[1] = DREAM3D::TypeNames::Float;
    parameters.push_back(DataArraySelectionFilterParameter::New("Input Orientations", "InputOrientationArrayPath", getInputOrientationArrayPath(), FilterParameter::RequiredArray, req, 0));
  }

  parameters.push_back(StringFilterParameter::New("Output Orientations", "OutputOrientationArrayName", getOutputOrientationArrayName(), FilterParameter::CreatedArray, 0));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputType(reader->readValue("InputType", getInputType() ) );
  setOutputType(reader->readValue("OutputType", getOutputType() ) );
  setInputOrientationArrayPath(reader->readDataArrayPath("InputOrientationArrayPath", getInputOrientationArrayPath()));
  setOutputOrientationArrayName(reader->readString("OutputOrientationArrayName", getOutputOrientationArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ConvertOrientations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(InputType)
  SIMPL_FILTER_WRITE_PARAMETER(OutputType)
  SIMPL_FILTER_WRITE_PARAMETER(InputOrientationArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(OutputOrientationArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientations::dataCheck()
{
  setErrorCondition(0);

  if(getInputType() == getOutputType())
  {
    QString ss = QObject::tr("Input and output orientation representation types must be different");
    setErrorCondition(-1000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if( getInputType() < OrientationConverter<float>::GetMinIndex() || getInputType() > OrientationConverter<float>::GetMaxIndex() )
  {
    QString ss = QObject::tr("There was an error with teh selection of the input orientation type. The valid values range from 0 to %1").arg(OrientationConverter<float>::GetMaxIndex());
    setErrorCondition(-1001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if( getOutputType() < OrientationConverter<float>::GetMinIndex() || getOutputType() > OrientationConverter<float>::GetMaxIndex() )
  {
    QString ss = QObject::tr("There was an error with the selection of the output orientation type. The valid values range from 0 to %1").arg(OrientationConverter<float>::GetMaxIndex());
    setErrorCondition(-1002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  // We need to return NOW because the next lines assume we have and index that is within
  // the valid bounds
  if(getErrorCondition() < 0) { return; }

  // Figure out what kind of Array the user selected
  // Get the input data and create the output Data appropriately
  IDataArray::Pointer iDataArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getInputOrientationArrayPath());

  DataArrayPath outputArrayPath = getInputOrientationArrayPath();
  outputArrayPath.setDataArrayName(getOutputOrientationArrayName());

  FloatArrayType::Pointer fArray = std::dynamic_pointer_cast<FloatArrayType>(iDataArrayPtr);
  if(NULL != fArray.get())
  {
    QVector<int32_t> componentCounts = OrientationConverter<float>::GetComponentCounts();
    QVector<size_t> outputCDims(1, componentCounts[getOutputType()]);
    getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, outputArrayPath, 0, outputCDims);
  }

  DoubleArrayType::Pointer dArray = std::dynamic_pointer_cast<DoubleArrayType>(iDataArrayPtr);
  if(NULL != dArray.get())
  {
    QVector<int32_t> componentCounts = OrientationConverter<double>::GetComponentCounts();
    QVector<size_t> outputCDims(1, componentCounts[getOutputType()]);
    getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, outputArrayPath, 0, outputCDims);
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientations::preflight()
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
template<typename T>
void generateRepresentation(ConvertOrientations* filter, typename DataArray<T>::Pointer inputOrientations,
                            typename DataArray<T>::Pointer outputOrientations)
{
  typedef typename DataArray<T>::Pointer ArrayType;
  typedef OrientationConverter<T> OCType;
  QVector<typename OCType::Pointer> converters(7);

  converters[0] = EulerConverter<T>::New();
  converters[1] = OrientationMatrixConverter<T>::New();
  converters[2] = QuaternionConverter<T>::New();
  converters[3] = AxisAngleConverter<T>::New();
  converters[4] = RodriguesConverter<T>::New();
  converters[5] = HomochoricConverter<T>::New();
  converters[6] = CubochoricConverter<T>::New();

  QVector<typename OCType::OrientationType> ocTypes = OCType::GetOrientationTypes();

  converters[filter->getInputType()]->setInputData(inputOrientations);
  converters[filter->getInputType()]->convertRepresentationTo(ocTypes[filter->getOutputType()]);

  ArrayType output = converters[filter->getInputType()]->getOutputData();
  if(NULL == output.get())
  {
    QString ss = QObject::tr("There was an error converting the input data using convertor %1").arg(converters[filter->getInputType()]->getNameOfClass());
    filter->setErrorCondition(-1004);
    filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
    return;
  }

  if(!output->copyIntoArray(outputOrientations) )
  {
    QString ss = QObject::tr("There was an error copying the final results into the output array.");
    filter->setErrorCondition(-1003);
    filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientations::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  IDataArray::Pointer iDataArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getInputOrientationArrayPath());

  DataArrayPath outputArrayPath = getInputOrientationArrayPath();
  outputArrayPath.setDataArrayName(getOutputOrientationArrayName());

  FloatArrayType::Pointer fArray = std::dynamic_pointer_cast<FloatArrayType>(iDataArrayPtr);
  if(NULL != fArray.get())
  {
    QVector<int32_t> componentCounts = OrientationConverter<float>::GetComponentCounts();
    QVector<size_t> outputCDims(1, componentCounts[getOutputType()]);
    FloatArrayType::Pointer outData = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, outputArrayPath, outputCDims);
    generateRepresentation<float>(this, fArray, outData);
  }

  DoubleArrayType::Pointer dArray = std::dynamic_pointer_cast<DoubleArrayType>(iDataArrayPtr);
  if(NULL != dArray.get())
  {
    QVector<int32_t> componentCounts = OrientationConverter<double>::GetComponentCounts();
    QVector<size_t> outputCDims(1, componentCounts[getOutputType()]);
    DoubleArrayType::Pointer outData = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, outputArrayPath, outputCDims);
    generateRepresentation<double>(this, dArray, outData);
  }


  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ConvertOrientations::newFilterInstance(bool copyFilterParameters)
{
  ConvertOrientations::Pointer filter = ConvertOrientations::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertOrientations::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertOrientations::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertOrientations::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertOrientations::getGroupName()
{ return DREAM3D::FilterGroups::OrientationAnalysisFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertOrientations::getSubGroupName()
{ return DREAM3D::FilterSubGroups::ConversionFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertOrientations::getHumanLabel()
{ return "Convert Orientation Representation"; }
