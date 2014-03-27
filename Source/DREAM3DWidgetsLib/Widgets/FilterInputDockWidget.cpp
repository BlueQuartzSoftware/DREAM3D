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

#include "FilterInputDockWidget.h"

#include <QtCore/QFileInfo>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFormLayout>
#include <QtGui/QLineEdit>

#include "DREAM3DWidgetsLib/FilterWidgetManager.h"

#include "DREAM3DWidgetsLib/Widgets/PipelineFilterWidget.h"

#include "DREAM3DWidgetsLib/moc_FilterInputDockWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterInputDockWidget::FilterInputDockWidget(QWidget* parent) :
  QWidget(parent)
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterInputDockWidget::~FilterInputDockWidget()
{
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputDockWidget::setupGui()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputDockWidget::clearInputWidgets()
{
  QWidget* widget = scrollArea->takeWidget();
  scrollArea->setWidget(new QFrame);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputDockWidget::removeWidgetInputs(PipelineFilterWidget* w)
{
  // Check to see if the filter that just got removed from the pipeline is the one we are displaying
  // the inputs for.
  if (scrollArea->widget() == w->getScrollWidgetContents() )
  {
    clearInputWidgets();
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputDockWidget::displayFilterParameters(PipelineFilterWidget* w)
{

  QWidget* widget = scrollArea->takeWidget();
  scrollArea->setWidget(w->getScrollWidgetContents() );

  QWidget* advWidget = advScrollArea->takeWidget();
  advScrollArea->setWidget(w->getAdvancedScrollWidgetContents() );

#if 0
  // Remove all existing QWidgets from this Widget
  QObjectList objs = basicInputsScrollWidget->children();
  for(int i = 0; i < objs.size(); i++)
  {
    objs[i]->setParent(NULL);
  }


  // Remove the Layout itself
  QLayoutItem* wItem = basicInputsScrollWidget->layout()->takeAt(0);
  while (wItem != NULL) {
    //delete wItem;
    wItem = basicInputsScrollWidget->layout()->takeAt(0);
  }

  delete verticalLayout;
  delete basicInputsScrollWidget;
  basicInputsScrollWidget = new QWidget();
  basicInputsScrollWidget->setObjectName(QString::fromUtf8("basicInputsScrollWidget"));
  basicInputsScrollWidget->setGeometry(QRect(0, 0, 250, 267));
  verticalLayout = new QVBoxLayout(basicInputsScrollWidget);
  verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
  scrollArea->setWidget(basicInputsScrollWidget);
#endif

  // Add a label at the top of the Inputs Tabs to show what filter we are working on
  filterHumanLabel->setText(w->getHumanLabel());
  AbstractFilter::Pointer filter = w->getFilter();
  brandingLabel->setText(filter->getBrandingString() );
#if 0
  QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Maximum);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  setSizePolicy(sizePolicy2);


  FilterWidgetManager::Pointer fwm = FilterWidgetManager::Instance();
  QVector<QWidget*>& filterParameterWidgets = w->getFilterParameterWidgets();
  for (int i = 0; i < filterParameterWidgets.size(); i++ )
  {
    QWidget* fpw = filterParameterWidgets[i];
    qDebug() << "    Pointer: " << fpw << "\n";
    // Make sure we do not have a NULL widget
    if (NULL == filterParameterWidgets.at(i)) continue;
    // Set the parent for the widget
    filterParameterWidgets[i]->setParent(basicInputsScrollWidget);
    // Add the FilterWidget to the layout
    verticalLayout->addWidget(fpw);
  }
  QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

  verticalLayout->addItem(verticalSpacer);
#endif
}
