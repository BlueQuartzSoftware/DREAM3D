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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "AxisAngleWidget.h"

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigen>

#include <QtGui/QDoubleValidator>

#include "SIMPLib/Math/SIMPLibMath.h"

#include "EbsdLib/Core/OrientationRepresentation.h"
#include "EbsdLib/Core/OrientationTransformation.hpp"
#include "OrientationUtilityCalculator.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleWidget::AxisAngleWidget(QWidget* parent)
: OrientationWidget(parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleWidget::~AxisAngleWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::setupGui()
{
  aa1->setValidator(new QDoubleValidator(aa1));
  aa2->setValidator(new QDoubleValidator(aa2));
  aa3->setValidator(new QDoubleValidator(aa3));
  aa4->setValidator(new QRegExpValidator(QRegExp("([-+]?[0-9]*\\.?[0-9]+)|pi"), aa4));

  connect(aa1, SIGNAL(textEdited(const QString&)), this, SLOT(valuesUpdated(const QString&)));
  connect(aa2, SIGNAL(textEdited(const QString&)), this, SLOT(valuesUpdated(const QString&)));
  connect(aa3, SIGNAL(textEdited(const QString&)), this, SLOT(valuesUpdated(const QString&)));
  connect(aa4, SIGNAL(textEdited(const QString&)), this, SLOT(valuesUpdated(const QString&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::updateData(OrientationUtilityCalculator* calculator)
{
  setStyleSheet("");

  if(calculator->getInputType() == OrientationRepresentation::Type::AxisAngle)
  {
    // The input type is the same as this widget, so don't update
    return;
  }
  if(calculator->getHasErrors())
  {
    aa1->setText("nan");
    aa2->setText("nan");
    aa3->setText("nan");
    aa4->setText("nan");
    setStyleSheet("QLineEdit{border: 2px solid FireBrick}");
    return;
  }

  QVector<double> aaValues = calculator->getValues(OrientationRepresentation::Type::AxisAngle);

  if(m_AngleMeasurement == Degrees)
  {
    double radVal = aaValues[3];
    double degVal = radVal * SIMPLib::Constants::k_RadToDegD;
    aaValues[3] = degVal;
  }

  aa1->setText(QString::number(aaValues[0]));
  aa2->setText(QString::number(aaValues[1]));
  aa3->setText(QString::number(aaValues[2]));
  aa4->setText(QString::number(aaValues[3]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::valuesUpdated(const QString& text)
{
  QVector<double> values = getValues();

  if(values[0] == 0.0 && values[1] == 0.0 && values[2] == 0)
  {
    return;
  }

  if(m_AngleMeasurement == Degrees)
  {
    double degVal = values[3];
    double radVal = degVal * SIMPLib::Constants::k_DegToRadD;
    values[3] = radVal;
  }

  // Always normalize the axis vector
  Eigen::Vector3d axis(values[0], values[1], values[2]);
  axis.normalize();
  values[0] = axis[0];
  values[1] = axis[1];
  values[2] = axis[2];

  Q_EMIT clearErrorTable();
  int errorCode = 0;
  std::stringstream ss;
  ss << "Axis Angle values were normalized. Actual values used for the calculation are: ";
  ss << "<" << values[0] << ", " << values[1] << ", " << values[2] << "> " << values[3];

  Q_EMIT invalidValues(errorCode, QString::fromStdString(ss.str()));

  OrientationTransformation::ResultType result = OrientationTransformation::ax_check(values);
  errorCode = result.result;
  QString errorMsg = QString::fromStdString(result.msg);

  if(errorCode >= 0)
  {
    Q_EMIT valuesChanged(values, OrientationRepresentation::Type::AxisAngle, false);
  }
  else
  {
    Q_EMIT valuesChanged(QVector<double>(), OrientationRepresentation::Type::AxisAngle, true);
    Q_EMIT invalidValues(errorCode, errorMsg);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::convertData(bool isDegrees)
{
  QVector<double> values = getValues();
  double value = values[3];

  if(isDegrees)
  {
    value = value * SIMPLib::Constants::k_RadToDegD;
  }
  else
  {
    value = value * SIMPLib::Constants::k_DegToRadD;
  }

  aa4->setText(QString::number(value));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> AxisAngleWidget::getValues()
{
  QVector<double> values;

  if(aa1->text() == "nan")
  {
    aa1->setText("0");
  }
  if(aa2->text() == "nan")
  {
    aa2->setText("0");
  }
  if(aa3->text() == "nan")
  {
    aa3->setText("0");
  }

  if(aa4->text() == "p" || aa4->text() == "pi")
  {
    if(m_AngleMeasurement == Degrees)
    {
      aa4->setText("180");
    }
    else
    {
      aa4->setText(QString::number(SIMPLib::Constants::k_PiD));
    }
  }
  else if(aa4->text() == "nan")
  {
    aa4->setText("0");
  }

  values.push_back(aa1->text().toDouble());
  values.push_back(aa2->text().toDouble());
  values.push_back(aa3->text().toDouble());
  values.push_back(aa4->text().toDouble());

  return values;
}
