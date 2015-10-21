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
  if (calculator->getInputType() == OrientationConverter<double>::Rodrigues)
  {
    // The input type is the same as this widget, so don't update
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
  QVector<double> values = getValues();
  OrientationTransforms<QVector<double>, double>::ResultType result = OrientationTransforms<QVector<double>, double>::ro_check(values);
  int errorCode = result.result;
  QString errorMsg = QString::fromStdString(result.msg);

  emit clearErrorTable();

  if (errorCode >= 0)
  {
    emit valuesChanged(values, OrientationConverter<double>::Rodrigues);
  }
  else
  {
    emit invalidValues(errorCode, errorMsg);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> RodriguesWidget::getValues()
{
  QVector<double> values;
  values.push_back(r1->text().toDouble());
  values.push_back(r2->text().toDouble());
  values.push_back(r3->text().toDouble());
  values.push_back(r4->text().toDouble());
  return values;
}

