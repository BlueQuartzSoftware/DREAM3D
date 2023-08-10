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

#include "StereographicWidget.h"

#include <QtGui/QRegExpValidator>

#include "SIMPLib/Math/SIMPLibMath.h"

#include "EbsdLib/Core/OrientationTransformation.hpp"

#include "OrientationUtilityCalculator.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StereographicWidget::StereographicWidget(QWidget* parent)
: OrientationWidget(parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StereographicWidget::~StereographicWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StereographicWidget::setupGui()
{
  xCoord->setValidator(new QRegExpValidator(QRegExp("([-+]?[0-9]*\\.?[0-9]+)"), xCoord));
  yCoord->setValidator(new QRegExpValidator(QRegExp("([-+]?[0-9]*\\.?[0-9]+)"), yCoord));
  zCoord->setValidator(new QRegExpValidator(QRegExp("([-+]?[0-9]*\\.?[0-9]+)"), zCoord));

  connect(xCoord, SIGNAL(textEdited(const QString&)), this, SLOT(valuesUpdated(const QString&)));
  connect(yCoord, SIGNAL(textEdited(const QString&)), this, SLOT(valuesUpdated(const QString&)));
  connect(zCoord, SIGNAL(textEdited(const QString&)), this, SLOT(valuesUpdated(const QString&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StereographicWidget::updateData(OrientationUtilityCalculator* calculator)
{
  setStyleSheet("");

  if(calculator->getInputType() == OrientationRepresentation::Type::Stereographic)
  {
    // The input type is the same as this widget, so don't update
    return;
  }
  if(calculator->getHasErrors())
  {
    xCoord->setText("nan");
    yCoord->setText("nan");
    zCoord->setText("nan");
    setStyleSheet("QLineEdit{border: 2px solid FireBrick}");
    return;
  }

  QVector<double> eValues = calculator->getValues(OrientationRepresentation::Type::Stereographic);

  if(eValues.size() == 3)
  {
    xCoord->setText(QString::number(eValues[0]));
    yCoord->setText(QString::number(eValues[1]));
    zCoord->setText(QString::number(eValues[2]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StereographicWidget::valuesUpdated(const QString& text)
{
  QVector<double> values = getValues();

  OrientationTransformation::ResultType result = OrientationTransformation::st_check(values);
  int errorCode = result.result;
  QString errorMsg = QString::fromStdString(result.msg);

  Q_EMIT clearErrorTable();

  if(errorCode >= 0)
  {
    Q_EMIT valuesChanged(values, OrientationRepresentation::Type::Stereographic, false);
  }
  else
  {
    Q_EMIT valuesChanged(QVector<double>(), OrientationRepresentation::Type::Stereographic, true);
    Q_EMIT invalidValues(errorCode, errorMsg);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StereographicWidget::convertData(bool isDegrees)
{
  QVector<double> values = getValues();

  xCoord->setText(QString::number(values[0]));
  yCoord->setText(QString::number(values[1]));
  zCoord->setText(QString::number(values[2]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> StereographicWidget::getValues()
{
  QVector<double> values;

  if(xCoord->text() == "nan")
  {
    xCoord->setText("0");
  }

  if(yCoord->text() == "nan")
  {
    yCoord->setText("0");
  }

  if(zCoord->text() == "nan")
  {
    zCoord->setText("0");
  }

  values.push_back(xCoord->text().toDouble());
  values.push_back(yCoord->text().toDouble());
  values.push_back(zCoord->text().toDouble());

  return values;
}
