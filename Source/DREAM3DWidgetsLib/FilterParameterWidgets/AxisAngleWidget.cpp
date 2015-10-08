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

#include "AxisAngleWidget.h"

#include <QtCore/QMetaProperty>

#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"


#include "FilterParameterWidgetsDialogs.h"


// Include the MOC generated file for this class
#include "moc_AxisAngleWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleWidget::AxisAngleWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<AxisAngleFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "AxisAngleWidget can ONLY be used with an AxisAngleFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleWidget::~AxisAngleWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::setupGui()
{



  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  connect(angle, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(axis_i, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(axis_j, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(axis_k, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );

  QDoubleValidator* xVal = new QDoubleValidator(axis_i);
  axis_i->setValidator(xVal);
  QDoubleValidator* yVal = new QDoubleValidator(axis_j);
  axis_j->setValidator(yVal);
  QDoubleValidator* zVal = new QDoubleValidator(axis_k);
  axis_k->setValidator(zVal);
  QDoubleValidator* aVal = new QDoubleValidator(angle);
  angle->setValidator(aVal);

  if (getFilterParameter() != NULL)
  {
    label->setText(getFilterParameter()->getHumanLabel() );


    AxisAngleInput_t data = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<AxisAngleInput_t>();
    axis_i->setText(QString::number(data.h) );
    axis_j->setText(QString::number(data.k) );
    axis_k->setText(QString::number(data.l) );
    angle->setText(QString::number(data.angle) );
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::widgetChanged(const QString& text)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  bool ok = false;
  AxisAngleInput_t data;
  data.angle = angle->text().toDouble(&ok);
  data.h = axis_i->text().toDouble(&ok);
  data.k = axis_j->text().toDouble(&ok);
  data.l = axis_k->text().toDouble(&ok);

  QVariant v;
  v.setValue(data);
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, v);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::afterPreflight()
{

}
