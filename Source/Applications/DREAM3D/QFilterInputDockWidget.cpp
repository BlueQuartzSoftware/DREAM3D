/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "QFilterInputDockWidget.h"

#include <QtCore/QFileInfo>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFormLayout>
#include <QtGui/QLineEdit>



#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "QFilterWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterInputDockWidget::QFilterInputDockWidget(QWidget* parent) :
  QDockWidget(parent)
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterInputDockWidget::~QFilterInputDockWidget()
{
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterInputDockWidget::setupGui()
{


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterInputDockWidget::setSelectedFilterWidget(AbstractFilter* filter)
{
  QObjectList objs = basicInputs->children();
  for(int i = 0; i < objs.size(); i++)
  {
    objs[i]->deleteLater();
  }



  QLayoutItem* wItem = basicInputs->layout()->takeAt(0);
  while (wItem != NULL) {
    delete wItem;
    wItem = basicInputs->layout()->takeAt(0);
  }

  delete basicInputs->layout();


  QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Maximum);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  setSizePolicy(sizePolicy2);

  QVBoxLayout* vertLayout = new QVBoxLayout(basicInputs);

  QFormLayout* frmLayout = new QFormLayout();
  vertLayout->addLayout(frmLayout);
  frmLayout->setObjectName("QFilterWidget QFormLayout Layout");

  QVector<FilterParameter::Pointer> options = filter->getFilterParameters();
  int optIndex = 0;
  for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {

    FilterParameter* option = (*iter).get();
    FilterParameter::WidgetType wType = option->getWidgetType();

    QString labelName = (option->getHumanLabel());
    if (option->getUnits().isEmpty() == false)
    {
      labelName.append(" (").append((option->getUnits())).append(")");
    }
    QLabel* label = new QLabel(labelName, this);

    // if (wType == FilterParameter::StringWidget)
    {
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName(option->getPropertyName());

      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      //connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditStringValue(const QString &)));
      //QVariant v = property(option->getPropertyName());
      le->setText(option->getPropertyName());
    }

    ++optIndex;
  }

}
