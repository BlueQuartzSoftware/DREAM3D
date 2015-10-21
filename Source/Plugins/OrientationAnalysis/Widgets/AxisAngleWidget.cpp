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

#include "AxisAngleWidget.h"

#include "SIMPLib/Math/SIMPLibMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleWidget::AxisAngleWidget(QWidget* parent) :
  OrientationWidget(parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleWidget::~AxisAngleWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::setupGui()
{
  aa1->setValidator(new QDoubleValidator(aa1));
  aa2->setValidator(new QDoubleValidator(aa2));
  aa3->setValidator(new QDoubleValidator(aa3));
  aa4->setValidator(new QDoubleValidator(aa4));

  connect(aa1, SIGNAL(textEdited(const QString&)),
          this, SLOT(valuesUpdated(const QString&)));
  connect(aa2, SIGNAL(textEdited(const QString&)),
          this, SLOT(valuesUpdated(const QString&)));
  connect(aa3, SIGNAL(textEdited(const QString&)),
          this, SLOT(valuesUpdated(const QString&)));
  connect(aa4, SIGNAL(textEdited(const QString&)),
          this, SLOT(valuesUpdated(const QString&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::updateData(OrientationUtilityCalculator* calculator)
{
  if (calculator->getInputType() == OrientationConverter<double>::AxisAngle)
  {
    // The input type is the same as this widget, so don't update
    return;
  }

  QVector<double> aaValues = calculator->getValues(OrientationConverter<double>::AxisAngle);

  if (m_AngleMeasurement == Degrees)
  {
    double radVal = aaValues[3];
    double degVal = radVal * SIMPLib::Constants::k_RadToDeg;
    aaValues[3] = degVal;
  }

  if (aaValues.size() == 4)
  {
    aa1->setText(QString::number(aaValues[0]));
    aa2->setText(QString::number(aaValues[1]));
    aa3->setText(QString::number(aaValues[2]));
    aa4->setText(QString::number(aaValues[3]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::valuesUpdated(const QString &text)
{
  QVector<double> values = getValues();

  if (m_AngleMeasurement == Degrees)
  {
    double degVal = values[3];
    double radVal = degVal * SIMPLib::Constants::k_DegToRad;
    values[3] = radVal;
  }

  OrientationTransforms<QVector<double>, double>::ResultType result = OrientationTransforms<QVector<double>, double>::ax_check(values);
  int errorCode = result.result;
  QString errorMsg = QString::fromStdString(result.msg);

  emit clearErrorTable();

  if (errorCode >= 0)
  {
    emit valuesChanged(values, OrientationConverter<double>::AxisAngle);
  }
  else
  {
    emit invalidValues(errorCode, errorMsg);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> AxisAngleWidget::getValues()
{
  QVector<double> values;
  values.push_back(aa1->text().toDouble());
  values.push_back(aa2->text().toDouble());
  values.push_back(aa3->text().toDouble());
  values.push_back(aa4->text().toDouble());
  return values;
}

