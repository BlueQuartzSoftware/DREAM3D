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


#include "FilterInputWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QFileInfo>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QSequentialAnimationGroup>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>

#include "QtSupportLib/DREAM3DHelpUrlGenerator.h"
#include "QtSupportLib/DREAM3DStyles.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"
#include "DREAM3DWidgetsLib/FilterWidgetManager.h"
#include "DREAM3DWidgetsLib/Widgets/DREAM3DUserManualDialog.h"
#include "DREAM3DWidgetsLib/Widgets/PipelineFilterWidget.h"
#include "DREAM3DWidgetsLib/Widgets/DataContainerArrayWidget.h"

enum TabIndices
{
  BASIC_TAB,
  ADVANCED_TAB,
  CURRENT_STRUCTURE_TAB,
  HELP_TAB
};

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
bool FilterInputWidget::eventFilter(QObject* o, QEvent* e)
{
  if ( e->type() == QEvent::Resize && qobject_cast<QLabel*>(o) && brandingLabel == o)
  {
    QFontMetrics metrics(brandingLabel->font());
    QString elidedText = metrics.elidedText(m_BrandingLabel, Qt::ElideMiddle, brandingLabel->width());
    brandingLabel->setText(elidedText);
    return true;
  }
  return QWidget::eventFilter( o, e );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::setupGui()
{

  QFont humanLabelFont = DREAM3DStyles::GetHumanLabelFont();
  QFont brandingFont =DREAM3DStyles::GetBrandingLabelFont();
  QFont categoryFont = DREAM3DStyles::GetCategoryFont();

  filterHumanLabel->setFont(humanLabelFont);
  brandingLabel->setFont(brandingFont);
  brandingLabel->installEventFilter(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::on_filterHelpBtn_pressed()
{
  DREAM3DUserManualDialog::LaunchHelpDialog(filterHumanLabel->text());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::clearInputWidgets()
{
  // Remove any existing input widgets
  QLayoutItem* item = basicInputsGrid->itemAt(0);
  if(item)
  {
    QWidget* w = item->widget();
    if(w)
    {
      w->setVisible(false);
      basicInputsGrid->removeWidget(w);
    }
  }
  item = advInputsGrid->itemAt(0);
  if(item)
  {
    QWidget* w = item->widget();
    if(w)
    {
      w->setVisible(false);
      advInputsGrid->removeWidget(w);
    }
  }

  item = currentStructureGrid->itemAt(0);
  if(item)
  {
    QWidget* w = item->widget();
    if(w)
    {
      w->setVisible(false);
      currentStructureGrid->removeWidget(w);
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
  w->getBasicInputsWidget()->setParent(w);
  w->getAdvancedInputsWidget()->setParent(w);
  clearInputWidgets();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::displayFilterParameters(PipelineFilterWidget* w)
{
  clearInputWidgets();
  basicInputsGrid->addWidget(w->getBasicInputsWidget());
  advInputsGrid->addWidget(w->getAdvancedInputsWidget());
  currentStructureGrid->addWidget(w->getCurrentStructureWidget());

  // Set the current index to the basic tab by default
  tabWidget->setCurrentIndex(BASIC_TAB);

  // First check to see if we even have advanced parameters and show/hide the buttons accordingly
  bool showAdv = static_cast<bool>(w->getAdvParameterCount());
  bool showBasic = static_cast<bool>(w->getBasicParameterCount());
  // If we do NOT have any basic Parameters then disable the basic tab
  if (showBasic == false)
  {
    tabWidget->setTabEnabled(BASIC_TAB, false);
    tabWidget->setCurrentIndex(ADVANCED_TAB);
  }

  // If we do NOT have any advanced Parameters then disable the advanced tab
  if (showAdv == false)
  {
    tabWidget->setTabEnabled(ADVANCED_TAB, false);

    if (tabWidget->isTabEnabled(BASIC_TAB) == false)
    {
      tabWidget->setCurrentIndex(CURRENT_STRUCTURE_TAB);
    }
  }

  w->getBasicInputsWidget()->setVisible(true);
  w->getAdvancedInputsWidget()->setVisible(true);
  w->getCurrentStructureWidget()->setVisible(true);

  // Add a label at the top of the Inputs Tabs to show what filter we are working on
  filterHumanLabel->setText(w->getHumanLabel());

  AbstractFilter::Pointer filter = w->getFilter();
  m_BrandingLabel = filter->getBrandingString()  + "  [" + w->getCompiledLibraryName() + "/" + w->getFilterGroup() + "/" + w->getFilterClassName() + "]";

  brandingLabel->setText(m_BrandingLabel);
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
  faderWidget->setStartColor(DREAM3D::Defaults::AdvancedColor);
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
  faderWidget->setStartColor(DREAM3D::Defaults::AdvancedColor);
  connect(faderWidget, SIGNAL(animationComplete() ),
          widget, SLOT(hide()));
  faderWidget->start();
  m_AdvFadedOut = true;
}



