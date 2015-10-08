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
#include "PrimaryRolledPresetDialog.h"

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QApplication>

#include <QtGui/QDoubleValidator>


#include "QtSupportLib/DREAM3DStyles.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_PrimaryRolledPresetDialog.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrimaryRolledPresetDialog::PrimaryRolledPresetDialog(QWidget* parent) :
  QDialog(parent)
{
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrimaryRolledPresetDialog::~PrimaryRolledPresetDialog()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryRolledPresetDialog::setupGui()
{

  QVBoxLayout* verticalLayout_2 = new QVBoxLayout(this);

  QFormLayout*         formLayout = new QFormLayout();
  formLayout->setObjectName(QString::fromUtf8("eulerLayout"));

  QLabel* ALabel = new QLabel(this);
  ALabel->setObjectName(QString::fromUtf8("A Label"));
  formLayout->setWidget(0, QFormLayout::LabelRole, ALabel);

  A = new QLineEdit(this);
  A->setText(QString("1.0"));
  A->setObjectName(QString::fromUtf8("A"));
  formLayout->setWidget(0, QFormLayout::FieldRole, A);
  QDoubleValidator* AValidator = new QDoubleValidator(A);
  AValidator->setDecimals(4);

  QLabel* BLabel = new QLabel(this);
  BLabel->setObjectName(QString::fromUtf8("B Label"));
  formLayout->setWidget(1, QFormLayout::LabelRole, BLabel);

  B = new QLineEdit(this);
  B->setText(QString("1.0"));
  B->setObjectName(QString::fromUtf8("B"));
  formLayout->setWidget(1, QFormLayout::FieldRole, B);
  QDoubleValidator* BValidator = new QDoubleValidator(B);
  BValidator->setDecimals(4);

  QLabel* CLabel = new QLabel(this);
  CLabel->setObjectName(QString::fromUtf8("C Label"));
  formLayout->setWidget(2, QFormLayout::LabelRole, CLabel);

  C = new QLineEdit(this);
  C->setText(QString("1.0"));
  C->setObjectName(QString::fromUtf8("C"));
  formLayout->setWidget(2, QFormLayout::FieldRole, C);
  QDoubleValidator* CValidator = new QDoubleValidator(C);
  CValidator->setDecimals(4);

  QLabel* infoLabel = new QLabel(this);
  infoLabel->setObjectName(QString::fromUtf8("infoLabel"));
  infoLabel->setText("The following must be true: A >= B >= C");
  formLayout->setWidget(3, QFormLayout::SpanningRole, infoLabel);

  verticalLayout_2->addLayout(formLayout);

  QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
  buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
  buttonBox->setOrientation(Qt::Horizontal);
  buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
  verticalLayout_2->addWidget(buttonBox);

  ALabel->setText(QApplication::translate("Rolled Preset Dialog", "A Axis Length:", 0));
  BLabel->setText(QApplication::translate("Rolled Preset Dialog", "B Axis Length:", 0));
  CLabel->setText(QApplication::translate("Rolled Preset Dialog", "C Axis Length:", 0));

  QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(A, SIGNAL(textEdited(const QString&)),
          this, SLOT(checkInputs(const QString&)));
  connect(B, SIGNAL(textEdited(const QString&)),
          this, SLOT(checkInputs(const QString&)));
  connect(C, SIGNAL(textEdited(const QString&)),
          this, SLOT(checkInputs(const QString&)));

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
void PrimaryRolledPresetDialog::checkInputs(const QString& text)
{
  float a = getA();
  float b = getB();
  float c = getC();
  if (a >= b && b >= c)
  {
    DREAM3DStyles::LineEditClearStyle(A);
    DREAM3DStyles::LineEditClearStyle(B);
    DREAM3DStyles::LineEditClearStyle(C);
  }
  else
  {
    DREAM3DStyles::LineEditRedErrorStyle(A);
    DREAM3DStyles::LineEditRedErrorStyle(B);
    DREAM3DStyles::LineEditRedErrorStyle(C);
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float PrimaryRolledPresetDialog::getA()
{
  bool ok = false;
  double d = A->text().toFloat(&ok);
  //FIXME: Should check the 'ok' variable here
  return d;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float PrimaryRolledPresetDialog::getB()
{
  bool ok = false;
  double d = B->text().toFloat(&ok);
  //FIXME: Should check the 'ok' variable here
  return d;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float PrimaryRolledPresetDialog::getC()
{
  bool ok = false;
  double d = C->text().toFloat(&ok);
  //FIXME: Should check the 'ok' variable here
  return d;
}
