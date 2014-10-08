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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "DynamicChoiceWidget.h"

#include <QtCore/QMetaProperty>

#include "DREAM3DLib/FilterParameters/FilterParameter.h"

#include "FilterParameterWidgetsDialogs.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicChoiceWidget::DynamicChoiceWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  QWidget(parent),
  m_Filter(filter),
  m_FilterParameter(NULL)
{

  m_FilterParameter = dynamic_cast<DynamicChoiceFilterParameter*>(parameter);
  Q_ASSERT_X(NULL != m_FilterParameter, "DynamicChoiceWidget can ONLY be used with Dynamic Choice Filter Parameters", __FILE__);

  setupUi(this);
  setupGui();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicChoiceWidget::~DynamicChoiceWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicChoiceWidget::setupGui()
{

  // Catch when the filter is about to execute the preflight
  connect(m_Filter, SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(m_Filter, SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(m_Filter, SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  connect(value, SIGNAL(currentIndexChanged(int)),
          this, SLOT(widgetChanged(int) ) );

  if (m_FilterParameter != NULL)
  {
    QString units = m_FilterParameter->getUnits();
    if(units.isEmpty() == false)
    {
      label->setText(m_FilterParameter->getHumanLabel() + " (" + units + ")");
    }
    else
    {
      label->setText(m_FilterParameter->getHumanLabel() );
    }
    updateComboBox();
  }
}


#define LIST_PROPERTY_NAME_AS_CHAR m_FilterParameter->getListProperty().toLatin1().constData()

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicChoiceWidget::updateComboBox()
{
  // setup the list of choices for the widget

  if(m_Filter)
  {
  //  QString currentText = value->currentText();

    // Get the list of choices from the filter
    QString listProp =  m_FilterParameter->getListProperty();
    //qDebug() << listProp;

    QVariant var = m_Filter->property(listProp.toLatin1().constData());
    if(var.isValid() == false)
    {
      qDebug() << "Error getting Property " << m_FilterParameter->getListProperty() << " from Filter";
    }
    QStringList choices = var.toStringList();

    value->blockSignals(true);
    value->clear(); // Remove everything


    if(choices.size() == 0)
    {
      value->addItem(m_FilterParameter->getDefaultValue().toString());
    }
    else
    {
      value->addItems(choices);
    }
    // Get the Default value from the filter
    QString i = m_Filter->property(PROPERTY_NAME_AS_CHAR).toString();
    int index = value->findText(i);
    value->setCurrentIndex(index);
    value->blockSignals(false);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicChoiceWidget::widgetChanged(int index)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicChoiceWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  QString index = value->currentText();
  QVariant v(index);
  bool ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, v);

  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(m_Filter, m_FilterParameter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicChoiceWidget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicChoiceWidget::afterPreflight()
{
  updateComboBox();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicChoiceWidget::setLinkedConditionalState(int state)
{
  bool boolProp = (state == Qt::Checked);
  fadeWidget(this, boolProp);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicChoiceWidget::fadeWidget(QWidget* widget, bool in)
{

  if (faderWidget)
  {
    faderWidget->close();
  }
  faderWidget = new FaderWidget(widget);
  if(in)
  {
    setVisible(true);
    faderWidget->setFadeIn();
    connect(faderWidget, SIGNAL(animationComplete() ),
            this, SLOT(show()));
  }
  else
  {
    faderWidget->setFadeOut();
    connect(faderWidget, SIGNAL(animationComplete() ),
            this, SLOT(hide()));
  }
  QColor color = DREAM3D::Defaults::BasicColor;
  if(m_FilterParameter->getAdvanced()) { color = DREAM3D::Defaults::AdvancedColor; }
  faderWidget->setStartColor(color);
  faderWidget->start();
}
