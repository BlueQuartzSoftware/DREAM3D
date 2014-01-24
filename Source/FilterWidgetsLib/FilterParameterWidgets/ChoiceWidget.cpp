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
#include "ChoiceWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/FilterParameterWidgets/moc_ChoiceWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChoiceWidget::ChoiceWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
ChoiceWidget::~ChoiceWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChoiceWidget::setupGui()
{
  connect(value, SIGNAL(currentIndexChanged(int)),
          this, SLOT(parametersChanged(int) ) );

  if (m_FilterParameter != NULL)
  {
    ChoiceWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
    // setup the list of choices for the widget
    ChoiceFilterParameter* choice = dynamic_cast<ChoiceFilterParameter*>(m_FilterParameter);
    QList<QString> choices = choice->getChoices().toList();
    value->blockSignals(true);
    value->addItems(choices);
    value->blockSignals(false);
    // Get the Default value from the filter
    int i = m_Filter->property(PROPERTY_NAME_AS_CHAR).toInt();
    value->setCurrentIndex(i);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChoiceWidget::parametersChanged(int index)
{

  QVariant v(index);
  bool ok = m_Filter->setProperty(PROPERTY_NAME_AS_CHAR, v);

  if(false == ok)
  {
    QString ss = QObject::tr("Error occurred setting Filter Parameter %1").arg(m_FilterParameter->getPropertyName());
    emit errorSettingFilterParameter(ss);
  }
}

