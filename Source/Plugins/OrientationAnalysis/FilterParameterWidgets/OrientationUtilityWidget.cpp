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

#include "OrientationUtilityWidget.h"

#include "OrientationUtilityCalculator.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationUtilityWidget::OrientationUtilityWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_Calculator(new OrientationUtilityCalculator(this))
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationUtilityWidget::~OrientationUtilityWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationUtilityWidget::setupGui()
{
  m_OrientationWidgets.push_back(axisAngleWidget);
  m_OrientationWidgets.push_back(cubochoricWidget);
  m_OrientationWidgets.push_back(eulerWidget);
  m_OrientationWidgets.push_back(homochoricWidget);
  m_OrientationWidgets.push_back(omWidget);
  m_OrientationWidgets.push_back(quatWidget);
  m_OrientationWidgets.push_back(rodriguesWidget);

  // Connect signals and slots
  connect(degreesBtn, SIGNAL(toggled(bool)), axisAngleWidget, SLOT(convertData(bool)));
  connect(degreesBtn, SIGNAL(toggled(bool)), eulerWidget, SLOT(convertData(bool)));

  for (int i = 0; i < m_OrientationWidgets.size(); i++)
  {
    OrientationWidget* widget = m_OrientationWidgets[i];

    connect(widget, SIGNAL(valuesChanged(QVector<double>, OrientationConverter<double>::OrientationType, bool)), m_Calculator, SLOT(setDataAndInputType(QVector<double>, OrientationConverter<double>::OrientationType, bool)));
    connect(widget, SIGNAL(invalidValues(int, QString)), this, SLOT(setErrorEntry(int, QString)));
    connect(widget, SIGNAL(clearErrorTable()), this, SLOT(clearErrorTable()));
    connect(m_Calculator, SIGNAL(calculatorReady(OrientationUtilityCalculator*)), widget, SLOT(updateData(OrientationUtilityCalculator*)));
    connect(degreesBtn, SIGNAL(toggled(bool)), widget, SLOT(updateAngleMeasurement(bool)));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationUtilityWidget::setErrorEntry(int errorCode, QString errorMsg)
{
  errorTextEdit->clear();
  errorMsg = errorMsg.toHtmlEscaped();
  if(errorCode == 0)
  {
    errorTextEdit->setText("<b>Information: </b>" + errorMsg);
  }
  else
  {
    errorTextEdit->setText("<b>Error: </b>" + errorMsg);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationUtilityWidget::clearErrorTable()
{
  errorTextEdit->clear();
}

