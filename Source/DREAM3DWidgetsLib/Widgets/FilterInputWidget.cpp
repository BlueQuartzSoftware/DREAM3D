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


#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"
#include "DREAM3DWidgetsLib/FilterWidgetManager.h"
#include "DREAM3DWidgetsLib/Widgets/PipelineFilterWidget.h"
#include "DREAM3DWidgetsLib/Widgets/DataContainerArrayWidget.h"

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

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterInputWidget::getLabelStyleSheet()
{
  QString styleSheet;
  QTextStream ss(&styleSheet);

  ss << "QLabel#label {";
#if defined(Q_OS_WIN)
  ss << "font: 10 pt \"Times New Roman\";";
#elif defined(Q_OS_MAC)
  ss << "font: 13 pt \"Times New Roman\";";
#else
  ss << "font: 10 pt \"Times New Roman\"";
#endif
  ss << "  font-weight: bold;  }";
  return styleSheet;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterInputWidget::eventFilter(QObject *o, QEvent *e)
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
  QFont humanLabelFont;
  humanLabelFont.setBold(true);
  humanLabelFont.setItalic(false);
  humanLabelFont.setWeight(75);
  humanLabelFont.setStyleStrategy(QFont::PreferAntialias);

  QFont brandingFont;
  brandingFont.setBold(true);
  brandingFont.setItalic(true);
  brandingFont.setWeight(75);
  brandingFont.setStyleStrategy(QFont::PreferAntialias);
  brandingFont.setFamily(QString::fromUtf8("Arial"));

  QFont categoryFont;
  categoryFont.setBold(true);
  categoryFont.setWeight(75);
  categoryFont.setStyleStrategy(QFont::PreferAntialias);

#if defined (Q_OS_MAC)
  humanLabelFont.setPointSize(16);
  brandingFont.setPointSize(11);
  categoryFont.setPointSize(14);
#elif defined (Q_OS_WIN)
  humanLabelFont.setPointSize(12);
  brandingFont.setPointSize(8);
  categoryFont.setPointSize(10);
#endif


  filterHumanLabel->setFont(humanLabelFont);
  brandingLabel->setFont(brandingFont);
  basicInputsLabel->setFont(categoryFont);
  advInputsLabel->setFont(categoryFont);

  brandingLabel->installEventFilter(this);
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

  basicInputsFrame->setVisible((bool)(w->getBasicParameterCount()));

  bool showAdv = false;
  // Cases

  // First check to see if we even have advanced parameters and show/hide the buttons accordingly
  showAdv = static_cast<bool>(w->getAdvParameterCount());
  advInputsBtn->setVisible(showAdv);
  advInputsLabel->setVisible(showAdv);
  // If we do NOT have any advanced Parameters then hide the AdvFrame
  if(showAdv == false)
  {
    advInputsFrame->setVisible(showAdv);
  }
  else // Show or Hide the AdvFrame based on the last interaction the user did
  {
    advInputsFrame->setVisible(!m_AdvFadedOut);
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterInputWidget::on_currentStructureBtn_clicked()
{
  if(currentStructureFrame->isVisible() == false)
  {
    currentStructureFrame->setVisible(true);
    fadeInWidget(currentStructureFrame);
  }
  else
  {
    fadeOutWidget(currentStructureFrame);
  }

}
