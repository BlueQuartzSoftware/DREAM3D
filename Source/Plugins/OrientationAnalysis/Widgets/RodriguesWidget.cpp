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

#include "RodriguesWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RodriguesWidget::RodriguesWidget(QWidget* parent) :
  OrientationWidget(parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RodriguesWidget::~RodriguesWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RodriguesWidget::setupGui()
{
  r1->setValidator(new QDoubleValidator(r1));
  r2->setValidator(new QDoubleValidator(r2));
  r3->setValidator(new QDoubleValidator(r3));
  r4->setValidator(new QDoubleValidator(r4));

  connect(r1, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
  connect(r2, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
  connect(r3, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
  connect(r4, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RodriguesWidget::updateData(OrientationUtilityCalculator* calculator)
{
  setStyleSheet("");

  if (calculator->getInputType() == OrientationConverter<double>::Rodrigues)
  {
    // The input type is the same as this widget, so don't update
    return;
  }
  else if (calculator->getHasErrors() == true)
  {
    r1->setText("nan");
    r2->setText("nan");
    r3->setText("nan");
    r4->setText("nan");
    setStyleSheet("QLineEdit{border: 2px solid FireBrick}");
    return;
  }

  QVector<double> rValues = calculator->getValues(OrientationConverter<double>::Rodrigues);

  if (rValues.size() == 4)
  {
    r1->setText(QString::number(rValues[0]));
    r2->setText(QString::number(rValues[1]));
    r3->setText(QString::number(rValues[2]));
    r4->setText(QString::number(rValues[3]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RodriguesWidget::valuesUpdated(const QString &text)
{
Q_UNUSED(text)
  QVector<double> values = getValues();

  // Sanity check the values. If they are all Zero then we just return otherwise
  // we will get a setfault in the ModifiedLambertProjection class.
  if(values[0] == 0.0 && values[1] == 0.0 && values[2] == 0)
  {
    return;
  }

// Always normalize the axis vector
  Eigen::Vector3d rod(values[0], values[1], values[2]);
  rod.normalize();
  values[0] = rod[0];
  values[1] = rod[1];
  values[2] = rod[2];


  emit clearErrorTable();
  int errorCode = 0;
  std::stringstream ss;
  ss << "Rodrigues vector was normalized. Actual values used for the calculation are: ";
  ss << values[0] << ", " << values[1] << ", " << values[2] << ", " << values[3];

  emit invalidValues(errorCode, QString::fromStdString(ss.str()));
  OrientationTransforms<QVector<double>, double>::ResultType result = OrientationTransforms<QVector<double>, double>::ro_check(values);
  errorCode = result.result;
  QString errorMsg = QString::fromStdString(result.msg);

  if (errorCode >= 0)
  {
    emit valuesChanged(values, OrientationConverter<double>::Rodrigues, false);
  }
  else
  {
    emit valuesChanged(QVector<double>(), OrientationConverter<double>::Rodrigues, true);
    emit invalidValues(errorCode, errorMsg);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> RodriguesWidget::getValues()
{
  QVector<double> values;

  if (r1->text() == "nan")
  {
    r1->setText("0");
  }
  if (r2->text() == "nan")
  {
    r2->setText("0");
  }
  if (r3->text() == "nan")
  {
    r3->setText("0");
  }
  if (r4->text() == "nan")
  {
    r4->setText("0");
  }

  values.push_back(r1->text().toDouble());
  values.push_back(r2->text().toDouble());
  values.push_back(r3->text().toDouble());
  values.push_back(r4->text().toDouble());

  return values;
}

