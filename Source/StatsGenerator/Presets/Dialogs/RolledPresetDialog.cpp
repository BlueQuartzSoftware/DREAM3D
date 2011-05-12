/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "RolledPresetDialog.h"
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleValidator>
#include <QtGui/QApplication>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RolledPresetDialog::RolledPresetDialog(QWidget* parent) :
QDialog(parent)
{
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RolledPresetDialog::~RolledPresetDialog()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RolledPresetDialog::setupGui()
{

  QVBoxLayout* verticalLayout_2 = new QVBoxLayout(this);

  QFormLayout*         formLayout = new QFormLayout();
  formLayout->setObjectName(QString::fromUtf8("eulerLayout"));

  QLabel* aspectRatioLabel = new QLabel(this);
  aspectRatioLabel->setObjectName(QString::fromUtf8("Aspect Ratio Label"));
  formLayout->setWidget(0, QFormLayout::LabelRole, aspectRatioLabel);

  aspectRatio = new QLineEdit(this);
  aspectRatio->setText(QString("1.0"));
  aspectRatio->setObjectName(QString::fromUtf8("aspectRatio"));
  formLayout->setWidget(0, QFormLayout::FieldRole, aspectRatio);
  QDoubleValidator* aspectRatioValidator = new QDoubleValidator(aspectRatio);
  aspectRatioValidator->setDecimals(4);

  QLabel* rollingFractionLabel = new QLabel(this);
  rollingFractionLabel->setObjectName(QString::fromUtf8("Rolling Fraction Label"));
  formLayout->setWidget(1, QFormLayout::LabelRole, rollingFractionLabel);

  rollingFraction = new QLineEdit(this);
  rollingFraction->setText(QString("0.5"));
  rollingFraction->setObjectName(QString::fromUtf8("rollingFraction"));
  formLayout->setWidget(1, QFormLayout::FieldRole, rollingFraction);
  QDoubleValidator* rollingFractionValidator = new QDoubleValidator(rollingFraction);
  rollingFractionValidator->setDecimals(4);

  verticalLayout_2->addLayout(formLayout);

  QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
  buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
  buttonBox->setOrientation(Qt::Horizontal);
  buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
  verticalLayout_2->addWidget(buttonBox);

  aspectRatioLabel->setText(QApplication::translate("Rolled Preset Dialog", "Aspect Ratio:", 0, QApplication::UnicodeUTF8));
  rollingFractionLabel->setText(QApplication::translate("Rolled Preset Dialog", "Rolling Fraction:", 0, QApplication::UnicodeUTF8));


  QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QMetaObject::connectSlotsByName(this);

  setWindowTitle("Rolled Preset Options");
  setSizeGripEnabled(false);

//  TextureDialog->resize(690, 406);
  QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  //sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
  this->setSizePolicy(sizePolicy);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float RolledPresetDialog::getAspectRatio()
{
  bool ok = false;
  double d = aspectRatio->text().toDouble(&ok);
  //FIXME: Should check the 'ok' variable here
  return d;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float RolledPresetDialog::getRollingFraction()
{
  bool ok = false;
  double d = rollingFraction->text().toDouble(&ok);
  //FIXME: Should check the 'ok' variable here
  return d;
}
