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

  QLabel* aspectRatio1Label = new QLabel(this);
  aspectRatio1Label->setObjectName(QString::fromUtf8("Aspect Ratio 1 Label"));
  formLayout->setWidget(0, QFormLayout::LabelRole, aspectRatio1Label);

  aspectRatio1 = new QLineEdit(this);
  aspectRatio1->setText(QString("1.0"));
  aspectRatio1->setObjectName(QString::fromUtf8("aspectRatio1"));
  formLayout->setWidget(0, QFormLayout::FieldRole, aspectRatio1);
  QDoubleValidator* aspectRatio1Validator = new QDoubleValidator(aspectRatio1);
  aspectRatio1Validator->setDecimals(4);

  QLabel* aspectRatio2Label = new QLabel(this);
  aspectRatio2Label->setObjectName(QString::fromUtf8("Aspect Ratio 2 Label"));
  formLayout->setWidget(1, QFormLayout::LabelRole, aspectRatio2Label);

  aspectRatio2 = new QLineEdit(this);
  aspectRatio2->setText(QString("1.0"));
  aspectRatio2->setObjectName(QString::fromUtf8("aspectRatio2"));
  formLayout->setWidget(1, QFormLayout::FieldRole, aspectRatio2);
  QDoubleValidator* aspectRatio2Validator = new QDoubleValidator(aspectRatio2);
  aspectRatio2Validator->setDecimals(4);

  verticalLayout_2->addLayout(formLayout);

  QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
  buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
  buttonBox->setOrientation(Qt::Horizontal);
  buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
  verticalLayout_2->addWidget(buttonBox);

  aspectRatio1Label->setText(QApplication::translate("Rolled Preset Dialog", "Aspect Ratio 1 (b/a):", 0, QApplication::UnicodeUTF8));
  aspectRatio2Label->setText(QApplication::translate("Rolled Preset Dialog", "Aspect Ratio 2 (c/a):", 0, QApplication::UnicodeUTF8));


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
float RolledPresetDialog::getAspectRatio1()
{
  bool ok = false;
  double d = aspectRatio1->text().toFloat(&ok);
  //FIXME: Should check the 'ok' variable here
  return d;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float RolledPresetDialog::getAspectRatio2()
{
  bool ok = false;
  double d = aspectRatio2->text().toFloat(&ok);
  //FIXME: Should check the 'ok' variable here
  return d;
}
