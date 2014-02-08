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
#include "IntVec3Widget.h"

#include <QtCore/QMetaProperty>


#include "DREAM3DWidgetsLib/FilterParameterWidgets/moc_IntVec3Widget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3Widget::IntVec3Widget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
IntVec3Widget::~IntVec3Widget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntVec3Widget::setupGui()
{

  connect(xData, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(yData, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(zData, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );

  QIntValidator* xVal = new QIntValidator(xData);
  xData->setValidator(xVal);
  QIntValidator* yVal = new QIntValidator(yData);
  yData->setValidator(yVal);
  QIntValidator* zVal = new QIntValidator(zData);
  zData->setValidator(zVal);

  if (m_FilterParameter != NULL)
  {
    IntVec3WidgetLabel->setText(m_FilterParameter->getHumanLabel() );
    IntVec3_t data = m_Filter->property(PROPERTY_NAME_AS_CHAR).value<IntVec3_t>();
    xData->setText(QString::number(data.x) );
    yData->setText(QString::number(data.y) );
    zData->setText(QString::number(data.z) );
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntVec3Widget::widgetChanged(const QString &text)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntVec3Widget::filterNeedsInputParameters(AbstractFilter* filter)
{
  bool ok = false;
  IntVec3_t data;
  data.x = xData->text().toInt(&ok);
  data.y = yData->text().toInt(&ok);
  data.z = zData->text().toInt(&ok);

  QVariant v;
  v.setValue(data);
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, v);
  if(false == ok)
  {
    QString ss = QObject::tr("Error occurred setting Filter Parameter %1").arg(m_FilterParameter->getPropertyName());
    emit errorSettingFilterParameter(ss);
  }
}

