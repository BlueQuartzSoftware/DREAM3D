/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "CubochoricWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CubochoricWidget::CubochoricWidget(QWidget* parent) :
  OrientationWidget(parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CubochoricWidget::~CubochoricWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CubochoricWidget::setupGui()
{
  c1->setValidator(new QDoubleValidator(c1));
  c2->setValidator(new QDoubleValidator(c2));
  c3->setValidator(new QDoubleValidator(c3));

  connect(c1, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
  connect(c2, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
  connect(c3, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CubochoricWidget::updateData(OrientationUtilityCalculator* calculator)
{
  setStyleSheet("");

  if (calculator->getInputType() == OrientationConverter<double>::Cubochoric)
  {
    // The input type is the same as this widget, so don't update
    return;
  }
  else if (calculator->getHasErrors() == true)
  {
    c1->setText("nan");
    c2->setText("nan");
    c3->setText("nan");
    setStyleSheet("QLineEdit{border: 2px solid FireBrick}");
    return;
  }

  QVector<double> cValues = calculator->getValues(OrientationConverter<double>::Cubochoric);

  if (cValues.size() == 3)
  {
    c1->setText(QString::number(cValues[0]));
    c2->setText(QString::number(cValues[1]));
    c3->setText(QString::number(cValues[2]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CubochoricWidget::valuesUpdated(const QString &text)
{
  QVector<double> values = getValues();
  OrientationTransforms<QVector<double>, double>::ResultType result = OrientationTransforms<QVector<double>, double>::cu_check(values);
  int errorCode = result.result;
  QString errorMsg = QString::fromStdString(result.msg);

  emit clearErrorTable();

  if (errorCode >= 0)
  {
    emit valuesChanged(values, OrientationConverter<double>::Cubochoric, false);
  }
  else
  {
    emit valuesChanged(QVector<double>(), OrientationConverter<double>::Cubochoric, true);
    emit invalidValues(errorCode, errorMsg);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> CubochoricWidget::getValues()
{
  QVector<double> values;

  if (c1->text() == "nan")
  {
    c1->setText("0");
  }
  if (c2->text() == "nan")
  {
    c2->setText("0");
  }
  if (c3->text() == "nan")
  {
    c3->setText("0");
  }

  values.push_back(c1->text().toDouble());
  values.push_back(c2->text().toDouble());
  values.push_back(c3->text().toDouble());

  return values;
}

