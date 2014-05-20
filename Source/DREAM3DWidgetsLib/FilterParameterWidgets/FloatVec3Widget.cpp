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
#include "FloatVec3Widget.h"

#include <QtCore/QMetaProperty>


#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3Widget::FloatVec3Widget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
FloatVec3Widget::~FloatVec3Widget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec3Widget::setupGui()
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


  connect(xData, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(yData, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(zData, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );

  QDoubleValidator* xVal = new QDoubleValidator(xData);
  xData->setValidator(xVal);
  QDoubleValidator* yVal = new QDoubleValidator(yData);
  yData->setValidator(yVal);
  QDoubleValidator* zVal = new QDoubleValidator(zData);
  zData->setValidator(zVal);
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

    FloatVec3_t data = m_Filter->property(PROPERTY_NAME_AS_CHAR).value<FloatVec3_t>();
    xData->setText(QString::number(data.x) );
    yData->setText(QString::number(data.y) );
    zData->setText(QString::number(data.z) );
  }


  // is the filter parameter tied to a boolean property of the Filter Instance, if it is then we need to make the check box visible
  if(m_FilterParameter->isConditional() == true)
  {
    bool boolProp = m_Filter->property(m_FilterParameter->getConditionalProperty().toLatin1().constData() ).toBool();
    conditionalCB->setChecked(boolProp);
    conditionalCB->setText(m_FilterParameter->getConditionalLabel());
    xData->setEnabled(boolProp);
    yData->setEnabled(boolProp);
    zData->setEnabled(boolProp);
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
void FloatVec3Widget::on_conditionalCB_stateChanged(int state)
{
  bool boolProp = conditionalCB->isChecked();
  xData->setEnabled(boolProp);
  xData->setVisible(boolProp);
  yData->setEnabled(boolProp);
  yData->setVisible(boolProp);
  zData->setEnabled(boolProp);
  zData->setVisible(boolProp);

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
void FloatVec3Widget::widgetChanged(const QString& text)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec3Widget::filterNeedsInputParameters(AbstractFilter* filter)
{
  bool ok = false;
  FloatVec3_t data;
  data.x = xData->text().toDouble(&ok);
  data.y = yData->text().toDouble(&ok);
  data.z = zData->text().toDouble(&ok);

  QVariant v;
  v.setValue(data);
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, v);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(m_Filter, m_FilterParameter);
  }

  if(m_FilterParameter->isConditional() )
  {
    QVariant var(conditionalCB->isChecked());
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
void FloatVec3Widget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec3Widget::afterPreflight()
{

}
