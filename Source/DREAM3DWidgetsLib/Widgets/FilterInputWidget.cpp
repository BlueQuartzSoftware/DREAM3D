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

#include "FilterInputWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QFileInfo>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QSequentialAnimationGroup>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFormLayout>
#include <QtGui/QLineEdit>

#include "DREAM3DWidgetsLib/FilterWidgetManager.h"

#include "DREAM3DWidgetsLib/Widgets/PipelineFilterWidget.h"

#include "DREAM3DWidgetsLib/moc_FilterInputWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterInputWidget::FilterInputWidget(QWidget* parent) :
  QWidget(parent),
  m_AdvFadedOut(false)
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterInputWidget::~FilterInputWidget()
{
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::setupGui()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::clearInputWidgets()
{
  // Remove any existing input widgets
  QLayoutItem* item = basicInputsGrid->itemAt(0);
  if(item) {
    QWidget* w = item->widget();
    if(w)
    {
      w->setVisible(false);
      //   w->setParent(NULL);
      basicInputsGrid->removeWidget(w);
    }
  }
  item = advInputsGrid->itemAt(0);
  if(item) {
    QWidget* w = item->widget();
    if(w)
    {
      w->setVisible(false);
      //    w->setParent(NULL);
      advInputsGrid->removeWidget(w);
    }
  }

  filterHumanLabel->setText("No Filter Selected");
  brandingLabel->clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::removeWidgetInputs(PipelineFilterWidget* w)
{

  w->getScrollWidgetContents()->setParent(w);
  w->getAdvancedScrollWidgetContents()->setParent(w);
  clearInputWidgets();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::displayFilterParameters(PipelineFilterWidget* w)
{
  clearInputWidgets();
  basicInputsGrid->addWidget(w->getScrollWidgetContents());
  advInputsGrid->addWidget(w->getAdvancedScrollWidgetContents());

  basicInputsFrame->setVisible((bool)(w->getBasicParameterCount()));

  bool showAdv = false;
  // Cases

  // First check to see if we even have advanced parameters and show/hide the buttons accordingly
  showAdv = static_cast<bool>(w->getAdvParameterCount());
  advInputsBtn->setVisible(showAdv);
  advInputsLabel->setVisible(showAdv);
  // If we do NOT have any advanced Parameters then hide the AdvFrame
  if(showAdv == false) {
    advInputsFrame->setVisible(showAdv);
  }
  else // Show or Hide the AdvFrame based on the last interaction the user did
  {
    advInputsFrame->setVisible(!m_AdvFadedOut);
  }

  w->getScrollWidgetContents()->setVisible(true);
  w->getAdvancedScrollWidgetContents()->setVisible(true);

  // Add a label at the top of the Inputs Tabs to show what filter we are working on
  filterHumanLabel->setText(w->getHumanLabel());
#if 1
  filterHumanLabel->setText(w->getHumanLabel() + "\n  [" + w->getFilterClassName() + "]" );
#endif
  AbstractFilter::Pointer filter = w->getFilter();
  brandingLabel->setText(filter->getBrandingString() );

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::fadeInWidget(QWidget* widget)
{
  if (faderWidget)
  {
    faderWidget->close();
  }
  faderWidget = new FaderWidget(widget);
  faderWidget->start();
  m_AdvFadedOut = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::fadeOutWidget(QWidget* widget)
{

  if (faderWidget)
  {
    faderWidget->close();
  }
  faderWidget = new FaderWidget(widget);
  faderWidget->setFadeOut();
  connect(faderWidget, SIGNAL(animationComplete() ),
          this, SLOT(hideButton()));
  faderWidget->start();
  m_AdvFadedOut = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::hideButton()
{
  advInputsFrame->setVisible(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::on_advInputsBtn_clicked()
{
  if(advInputsFrame->isVisible() == false)
  {
    advInputsFrame->setVisible(true);
    fadeInWidget(advInputsFrame);
  }
  else
  {
    fadeOutWidget(advInputsFrame);
  }

}
