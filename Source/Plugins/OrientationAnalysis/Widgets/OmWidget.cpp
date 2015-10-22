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

#include "OmWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OmWidget::OmWidget(QWidget* parent) :
  OrientationWidget(parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OmWidget::~OmWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OmWidget::setupGui()
{
  om1->setValidator(new QDoubleValidator(om1));
  om2->setValidator(new QDoubleValidator(om2));
  om3->setValidator(new QDoubleValidator(om3));
  om4->setValidator(new QDoubleValidator(om4));
  om5->setValidator(new QDoubleValidator(om5));
  om6->setValidator(new QDoubleValidator(om6));
  om7->setValidator(new QDoubleValidator(om7));
  om8->setValidator(new QDoubleValidator(om8));
  om9->setValidator(new QDoubleValidator(om9));

  connect(om1, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
  connect(om2, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
  connect(om3, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
  connect(om4, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
  connect(om5, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
  connect(om6, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
  connect(om7, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
  connect(om8, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
  connect(om9, SIGNAL(textEdited(const QString&)),
    this, SLOT(valuesUpdated(const QString&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OmWidget::updateData(OrientationUtilityCalculator* calculator)
{
  setStyleSheet("");

  if (calculator->getInputType() == OrientationConverter<double>::OrientationMatrix)
  {
    // The input type is the same as this widget, so don't update
    return;
  }
  else if (calculator->getHasErrors() == true)
  {
    om1->setText("nan");
    om2->setText("nan");
    om3->setText("nan");
    om4->setText("nan");
    om5->setText("nan");
    om6->setText("nan");
    om7->setText("nan");
    om8->setText("nan");
    om9->setText("nan");
    setStyleSheet("QLineEdit{border: 2px solid FireBrick}");
    return;
  }

  QVector<double> omValues = calculator->getValues(OrientationConverter<double>::OrientationMatrix);

  if (omValues.size() == 9)
  {
    om1->setText(QString::number(omValues[0]));
    om2->setText(QString::number(omValues[1]));
    om3->setText(QString::number(omValues[2]));
    om4->setText(QString::number(omValues[3]));
    om5->setText(QString::number(omValues[4]));
    om6->setText(QString::number(omValues[5]));
    om7->setText(QString::number(omValues[6]));
    om8->setText(QString::number(omValues[7]));
    om9->setText(QString::number(omValues[8]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OmWidget::valuesUpdated(const QString &text)
{
  QVector<double> values = getValues();
  OrientationTransforms<QVector<double>, double>::ResultType result = OrientationTransforms<QVector<double>, double>::om_check(values);
  int errorCode = result.result;
  QString errorMsg = QString::fromStdString(result.msg);

  emit clearErrorTable();

  if (errorCode >= 0)
  {
    emit valuesChanged(values, OrientationConverter<double>::OrientationMatrix, false);
  }
  else
  {
    emit valuesChanged(QVector<double>(), OrientationConverter<double>::OrientationMatrix, true);
    emit invalidValues(errorCode, errorMsg);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> OmWidget::getValues()
{
  QVector<double> values;

  if (om1->text() == "nan")
  {
    om1->setText("0");
  }
  if (om2->text() == "nan")
  {
    om2->setText("0");
  }
  if (om3->text() == "nan")
  {
    om3->setText("0");
  }
  if (om4->text() == "nan")
  {
    om4->setText("0");
  }
  if (om5->text() == "nan")
  {
    om5->setText("0");
  }
  if (om6->text() == "nan")
  {
    om6->setText("0");
  }
  if (om7->text() == "nan")
  {
    om7->setText("0");
  }
  if (om8->text() == "nan")
  {
    om8->setText("0");
  }
  if (om9->text() == "nan")
  {
    om9->setText("0");
  }

  values.push_back(om1->text().toDouble());
  values.push_back(om2->text().toDouble());
  values.push_back(om3->text().toDouble());
  values.push_back(om4->text().toDouble());
  values.push_back(om5->text().toDouble());
  values.push_back(om6->text().toDouble());
  values.push_back(om7->text().toDouble());
  values.push_back(om8->text().toDouble());
  values.push_back(om9->text().toDouble());

  return values;
}


