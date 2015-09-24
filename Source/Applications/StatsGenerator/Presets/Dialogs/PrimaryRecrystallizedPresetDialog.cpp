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

#include "PrimaryRecrystallizedPresetDialog.h"
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDialogButtonBox>
#include <QtGui/QDoubleValidator>
#include <QtWidgets/QApplication>

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_PrimaryRecrystallizedPresetDialog.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrimaryRecrystallizedPresetDialog::PrimaryRecrystallizedPresetDialog(QWidget* parent) :
  QDialog(parent)
{
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrimaryRecrystallizedPresetDialog::~PrimaryRecrystallizedPresetDialog()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryRecrystallizedPresetDialog::setupGui()
{

  QVBoxLayout* verticalLayout_2 = new QVBoxLayout(this);

  QFormLayout*         formLayout = new QFormLayout();
  formLayout->setObjectName(QString::fromUtf8("eulerLayout"));

  QLabel* percentRecystallizedLabel = new QLabel(this);
  percentRecystallizedLabel->setObjectName(QString::fromUtf8("Percent Recrystallized Label"));
  formLayout->setWidget(0, QFormLayout::LabelRole, percentRecystallizedLabel);

  percentRecystallized = new QLineEdit(this);
  percentRecystallized->setText(QString("1.0"));
  percentRecystallized->setObjectName(QString::fromUtf8("percentRecystallized"));
  formLayout->setWidget(0, QFormLayout::FieldRole, percentRecystallized);
  QDoubleValidator* aspectRatioValidator = new QDoubleValidator(percentRecystallized);
  aspectRatioValidator->setDecimals(4);


  verticalLayout_2->addLayout(formLayout);

  QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
  buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
  buttonBox->setOrientation(Qt::Horizontal);
  buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
  verticalLayout_2->addWidget(buttonBox);

  percentRecystallizedLabel->setText(QApplication::translate("Percent Recrystallized Preset Dialog", "Percent Recrystallized:", 0));


  QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QMetaObject::connectSlotsByName(this);

  setWindowTitle("Recrystallized Preset Options");
  setSizeGripEnabled(false);
  QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  //sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
  this->setSizePolicy(sizePolicy);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float PrimaryRecrystallizedPresetDialog::getPercentRecrystallized()
{
  bool ok = false;
  double d = percentRecystallized->text().toFloat(&ok);
  //FIXME: Should check the 'ok' variable here
  return d;
}


