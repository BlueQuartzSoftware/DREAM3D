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

#include "QuatWidget.h"

#include "SIMPLib/Math/QuaternionMath.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuatWidget::QuatWidget(QWidget* parent)
: OrientationWidget(parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuatWidget::~QuatWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuatWidget::setupGui()
{
  q1->setValidator(new QDoubleValidator(q1));
  q2->setValidator(new QDoubleValidator(q2));
  q3->setValidator(new QDoubleValidator(q3));
  q4->setValidator(new QDoubleValidator(q4));

  connect(q1, SIGNAL(textEdited(const QString&)), this, SLOT(valuesUpdated(const QString&)));
  connect(q2, SIGNAL(textEdited(const QString&)), this, SLOT(valuesUpdated(const QString&)));
  connect(q3, SIGNAL(textEdited(const QString&)), this, SLOT(valuesUpdated(const QString&)));
  connect(q4, SIGNAL(textEdited(const QString&)), this, SLOT(valuesUpdated(const QString&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuatWidget::updateData(OrientationUtilityCalculator* calculator)
{
  setStyleSheet("");

  if(calculator->getInputType() == OrientationConverter<double>::Quaternion)
  {
    // The input type is the same as this widget, so don't update
    return;
  }
  else if(calculator->getHasErrors() == true)
  {
    q1->setText("nan");
    q2->setText("nan");
    q3->setText("nan");
    q4->setText("nan");
    setStyleSheet("QLineEdit{border: 2px solid FireBrick}");
    return;
  }

  QVector<double> qValues = calculator->getValues(OrientationConverter<double>::Quaternion);

  if(qValues.size() == 4)
  {
    q1->setText(QString::number(qValues[0]));
    q2->setText(QString::number(qValues[1]));
    q3->setText(QString::number(qValues[2]));
    q4->setText(QString::number(qValues[3]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuatWidget::valuesUpdated(const QString& text)
{
  QVector<double> values = getValues();

  QuatD quat = QuaternionMathD::New(values[0], values[1], values[2], values[3]);
  QuaternionMathD::UnitQuaternion(quat);
  values[0] = quat.x;
  values[1] = quat.y;
  values[2] = quat.z;
  values[3] = quat.w;

  OrientationTransforms<QVector<double>, double>::ResultType result = OrientationTransforms<QVector<double>, double>::qu_check(values);
  int errorCode = result.result;
  QString errorMsg = QString::fromStdString(result.msg);

  emit clearErrorTable();

  if(errorCode >= 0)
  {
    emit valuesChanged(values, OrientationConverter<double>::Quaternion, false);
  }
  else
  {
    emit valuesChanged(QVector<double>(), OrientationConverter<double>::Quaternion, true);
    emit invalidValues(errorCode, errorMsg);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> QuatWidget::getValues()
{
  QVector<double> values;

  if(q1->text() == "nan")
  {
    q1->setText("0");
  }
  if(q2->text() == "nan")
  {
    q2->setText("0");
  }
  if(q3->text() == "nan")
  {
    q3->setText("0");
  }
  if(q4->text() == "nan")
  {
    q4->setText("0");
  }

  values.push_back(q1->text().toDouble());
  values.push_back(q2->text().toDouble());
  values.push_back(q3->text().toDouble());
  values.push_back(q4->text().toDouble());

  return values;
}
