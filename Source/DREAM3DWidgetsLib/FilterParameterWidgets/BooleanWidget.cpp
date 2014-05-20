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
#include "BooleanWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterParameterWidgetsDialogs.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BooleanWidget::BooleanWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  QWidget(parent),
  m_Filter(filter),
  m_FilterParameter(parameter)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BooleanWidget::~BooleanWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BooleanWidget::setupGui()
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

  connect(value, SIGNAL(stateChanged(int)),
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

    QVariant objValue = m_Filter->property(PROPERTY_NAME_AS_CHAR);
    if (objValue.isValid() == true)
    {
      value->setChecked(objValue.toBool());
    }
    else
    {
      QString ss = QObject::tr("Error occurred getting Filter Parameter %1 for filter %2").arg(m_FilterParameter->getPropertyName()).arg(m_Filter->getNameOfClass());
      emit errorSettingFilterParameter(ss);
      qDebug() << ss;
    }

  }

  // is the filter parameter tied to a boolean property of the Filter Instance, if it is then we need to make the check box visible
  if(m_FilterParameter->isConditional() == true)
  {
    bool boolProp = m_Filter->property(m_FilterParameter->getConditionalProperty().toLatin1().constData() ).toBool();
    conditionalCB->setChecked(boolProp);
    conditionalCB->setText(m_FilterParameter->getConditionalLabel());
    value->setEnabled(boolProp);
    on_conditionalCB_stateChanged(conditionalCB->checkState());
  }
  else
  {
    widgetLayout->removeWidget(conditionalCB);
    conditionalCB->deleteLater();
    widgetLayout->removeWidget(linkLeft);
    linkLeft->deleteLater();
    widgetLayout->removeWidget(linkRight);
    linkRight->deleteLater();
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BooleanWidget::on_conditionalCB_stateChanged(int state)
{
  bool boolProp = conditionalCB->isChecked();
  value->setEnabled(boolProp);
  value->setVisible(boolProp);

  label->setVisible(boolProp);
  linkLeft->setVisible(boolProp);
  linkRight->setVisible(boolProp);
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BooleanWidget::widgetChanged(int state)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BooleanWidget::filterNeedsInputParameters(AbstractFilter* filter)
{

  QVariant var(value->isChecked() );
  bool ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(m_Filter, m_FilterParameter);
  }

  if(m_FilterParameter->isConditional() )
  {
    var.setValue(conditionalCB->isChecked());
    ok = filter->setProperty(m_FilterParameter->getConditionalProperty().toLatin1().constData(), var);
    if(false == ok)
    {
      FilterParameterWidgetsDialogs::ShowCouldNotSetConditionalFilterParameter(m_Filter, m_FilterParameter);
    }
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BooleanWidget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BooleanWidget::afterPreflight()
{

}
