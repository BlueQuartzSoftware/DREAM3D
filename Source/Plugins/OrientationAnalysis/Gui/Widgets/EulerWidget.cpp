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

#include "EulerWidget.h"

#include <QtGui/QRegExpValidator>

#include "SIMPLib/Math/SIMPLibMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EulerWidget::EulerWidget(QWidget* parent)
: OrientationWidget(parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EulerWidget::~EulerWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EulerWidget::setupGui()
{
  e1->setValidator(new QRegExpValidator(QRegExp("([-+]?[0-9]*\\.?[0-9]+)|pi"), e1));
  e2->setValidator(new QRegExpValidator(QRegExp("([-+]?[0-9]*\\.?[0-9]+)|pi"), e2));
  e3->setValidator(new QRegExpValidator(QRegExp("([-+]?[0-9]*\\.?[0-9]+)|pi"), e3));

  connect(e1, SIGNAL(textEdited(const QString&)), this, SLOT(valuesUpdated(const QString&)));
  connect(e2, SIGNAL(textEdited(const QString&)), this, SLOT(valuesUpdated(const QString&)));
  connect(e3, SIGNAL(textEdited(const QString&)), this, SLOT(valuesUpdated(const QString&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EulerWidget::updateData(OrientationUtilityCalculator* calculator)
{
  setStyleSheet("");

  if(calculator->getInputType() == OrientationConverter<double>::Euler)
  {
    // The input type is the same as this widget, so don't update
    return;
  }
  else if(calculator->getHasErrors() == true)
  {
    e1->setText("nan");
    e2->setText("nan");
    e3->setText("nan");
    setStyleSheet("QLineEdit{border: 2px solid FireBrick}");
    return;
  }

  QVector<double> eValues = calculator->getValues(OrientationConverter<double>::Euler);

  if(m_AngleMeasurement == Degrees)
  {
    eValues = toDegrees(eValues);
  }

  if(eValues.size() == 3)
  {
    e1->setText(QString::number(eValues[0]));
    e2->setText(QString::number(eValues[1]));
    e3->setText(QString::number(eValues[2]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EulerWidget::valuesUpdated(const QString& text)
{
  QVector<double> values = getValues();

  if(m_AngleMeasurement == Degrees)
  {
    values = toRadians(values);
  }

  OrientationTransforms<QVector<double>, double>::ResultType result = OrientationTransforms<QVector<double>, double>::eu_check(values);
  int errorCode = result.result;
  QString errorMsg = QString::fromStdString(result.msg);

  emit clearErrorTable();

  if(errorCode >= 0)
  {
    emit valuesChanged(values, OrientationConverter<double>::Euler, false);
  }
  else
  {
    emit valuesChanged(QVector<double>(), OrientationConverter<double>::Euler, true);
    emit invalidValues(errorCode, errorMsg);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EulerWidget::convertData(bool isDegrees)
{
  QVector<double> values = getValues();

  if(isDegrees == true)
  {
    values = toDegrees(values);
  }
  else
  {
    values = toRadians(values);
  }

  e1->setText(QString::number(values[0]));
  e2->setText(QString::number(values[1]));
  e3->setText(QString::number(values[2]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> EulerWidget::getValues()
{
  QVector<double> values;

  if(e1->text() == "p" || e1->text() == "pi")
  {
    if(m_AngleMeasurement == Degrees)
    {
      e1->setText("180");
    }
    else
    {
      e1->setText(QString::number(SIMPLib::Constants::k_Pi));
    }
  }
  else if(e1->text() == "nan")
  {
    e1->setText("0");
  }

  if(e2->text() == "p" || e2->text() == "pi")
  {
    if(m_AngleMeasurement == Degrees)
    {
      e2->setText("180");
    }
    else
    {
      e2->setText(QString::number(SIMPLib::Constants::k_Pi));
    }
  }
  else if(e2->text() == "nan")
  {
    e2->setText("0");
  }

  if(e3->text() == "p" || e3->text() == "pi")
  {
    if(m_AngleMeasurement == Degrees)
    {
      e3->setText("180");
    }
    else
    {
      e3->setText(QString::number(SIMPLib::Constants::k_Pi));
    }
  }
  else if(e3->text() == "nan")
  {
    e3->setText("0");
  }

  values.push_back(e1->text().toDouble());
  values.push_back(e2->text().toDouble());
  values.push_back(e3->text().toDouble());

  return values;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> EulerWidget::toDegrees(QVector<double> data)
{
  for(int i = 0; i < data.size(); i++)
  {
    double radVal = data[i];
    double degVal = radVal * SIMPLib::Constants::k_RadToDeg;
    data[i] = degVal;
  }

  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> EulerWidget::toRadians(QVector<double> data)
{
  for(int i = 0; i < data.size(); i++)
  {
    double degVal = data[i];
    double radVal = degVal * SIMPLib::Constants::k_DegToRad;
    data[i] = radVal;
  }

  return data;
}
