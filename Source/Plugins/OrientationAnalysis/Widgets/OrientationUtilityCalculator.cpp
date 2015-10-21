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
#include "OrientationUtilityCalculator.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "AxisAngleWidget.h"
#include "CubochoricWidget.h"
#include "EulerWidget.h"
#include "HomochoricWidget.h"
#include "OmWidget.h"
#include "QuatWidget.h"
#include "RodriguesWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationUtilityCalculator::OrientationUtilityCalculator(QWidget* parent) :
  QWidget(parent),
  m_InputData(QVector<double>()),
  m_InputType(OrientationConverter<double>::UnknownOrientationType),
  m_HasErrors(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationUtilityCalculator::~OrientationUtilityCalculator()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationUtilityCalculator::setDataAndInputType(QVector<double> values, OrientationConverter<double>::OrientationType inputType, bool hasErrors)
{
  m_InputData = values;
  m_InputType = inputType;
  m_HasErrors = hasErrors;
  emit calculatorReady(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationConverter<double>::OrientationType OrientationUtilityCalculator::getInputType()
{
  return m_InputType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool OrientationUtilityCalculator::getHasErrors()
{
  return m_HasErrors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> OrientationUtilityCalculator::getValues(OrientationConverter<double>::OrientationType outputType)
{
  typedef OrientationConverter<double> OCType;
  QVector<OCType::Pointer> converters(7);

  converters[0] = EulerConverter<double>::New();
  converters[1] = OrientationMatrixConverter<double>::New();
  converters[2] = QuaternionConverter<double>::New();
  converters[3] = AxisAngleConverter<double>::New();
  converters[4] = RodriguesConverter<double>::New();
  converters[5] = HomochoricConverter<double>::New();
  converters[6] = CubochoricConverter<double>::New();

  QVector<OCType::OrientationType> ocTypes = OCType::GetOrientationTypes();

  QVector<size_t> cDims(1, m_InputData.size());
  DataArray<double>::Pointer inputDataArray = DataArray<double>::CreateArray(1, cDims, "Input Data");
  for (int i = 0; i < m_InputData.size(); i++)
  {
    inputDataArray->setComponent(0, i, m_InputData[i]);
  }

  converters[m_InputType]->setInputData(inputDataArray);
  converters[m_InputType]->convertRepresentationTo(ocTypes[outputType]);

  DataArray<double>::Pointer output = converters[m_InputType]->getOutputData();

  QVector<double> outputData;
  for (int i = 0; i < output->getNumberOfComponents(); i++)
  {
    outputData.push_back(output->getComponent(0, i));
  }

  return outputData;
}

