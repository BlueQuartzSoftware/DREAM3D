/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "QCheckboxDialog.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QCheckboxDialog::QCheckboxDialog(QVector<QString> list, QWidget* parent) :
  QDialog(parent)
{
  m_List = list;
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QCheckboxDialog::~QCheckboxDialog()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCheckboxDialog::setupGui()
{

  QVBoxLayout* verticalLayout_2 = new QVBoxLayout(this);

  foreach(QString str, m_List)
  {
    QCheckBox* cb = new QCheckBox(this);
    cb->setText(str);
    cb->setObjectName(str);
    cb->setChecked(true);
    verticalLayout_2->addWidget(cb);
    m_WidgetMap.insert(str, cb);
  }

  QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
  buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
  buttonBox->setOrientation(Qt::Horizontal);
  buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
  verticalLayout_2->addWidget(buttonBox);


  QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QMetaObject::connectSlotsByName(this);

  setWindowTitle("Output Options");
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
bool QCheckboxDialog::getValue(QString key)
{
  QCheckBox* cb = m_WidgetMap[key];
  if (NULL != cb)
  {
    return cb->isChecked();
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCheckboxDialog::setValue(QString key, bool value)
{
  QCheckBox* cb = m_WidgetMap[key];
  if (NULL != cb)
  {
    cb->setChecked(value);
  }
}


