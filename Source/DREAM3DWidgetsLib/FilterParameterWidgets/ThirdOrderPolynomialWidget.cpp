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

#include "ThirdOrderPolynomialWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QSignalMapper>

#include "DREAM3DLib/FilterParameters/ThirdOrderPolynomialFilterParameter.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ThirdOrderPolynomialWidget::ThirdOrderPolynomialWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<ThirdOrderPolynomialFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "ThirdOrderPolynomialWidget can ONLY be used with a ThirdOrderPolynomialFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ThirdOrderPolynomialWidget::~ThirdOrderPolynomialWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThirdOrderPolynomialWidget::setupGui()
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


  connect(c30, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(c03, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(c21, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(c12, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(c20, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(c02, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(c11, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(c10, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(c01, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(c00, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );


  QDoubleValidator* c30Val = new QDoubleValidator(c30);
  c30->setValidator(c30Val);
  QDoubleValidator* c03Val = new QDoubleValidator(c03);
  c03->setValidator(c03Val);
  QDoubleValidator* c21Val = new QDoubleValidator(c21);
  c21->setValidator(c21Val);
  QDoubleValidator* c12Val = new QDoubleValidator(c12);
  c12->setValidator(c12Val);
  QDoubleValidator* c20Val = new QDoubleValidator(c20);
  c20->setValidator(c20Val);
  QDoubleValidator* c02Val = new QDoubleValidator(c02);
  c02->setValidator(c02Val);
  QDoubleValidator* c11Val = new QDoubleValidator(c11);
  c11->setValidator(c11Val);
  QDoubleValidator* c10Val = new QDoubleValidator(c10);
  c10->setValidator(c10Val);
  QDoubleValidator* c01Val = new QDoubleValidator(c01);
  c01->setValidator(c01Val);
  QDoubleValidator* c00Val = new QDoubleValidator(c00);
  c00->setValidator(c00Val);

  if (getFilterParameter() != NULL)
  {
    label->setText(getFilterParameter()->getHumanLabel() );

    Float3rdOrderPoly_t data = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<Float3rdOrderPoly_t>();
    c30->setText( QString::number(data.c30)  );
    c03->setText( QString::number(data.c03)  );
    c21->setText( QString::number(data.c21)  );
    c12->setText( QString::number(data.c12) );
    c20->setText( QString::number(data.c20)  );
    c02->setText( QString::number(data.c02)  );
    c11->setText( QString::number(data.c11)  );
    c10->setText( QString::number(data.c10) );
    c01->setText( QString::number(data.c01) );
    c00->setText( QString::number(data.c00) );
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThirdOrderPolynomialWidget::widgetChanged(const QString& text)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThirdOrderPolynomialWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  bool ok = false;
  Float3rdOrderPoly_t data;
  data.c30 = c30->text().toDouble(&ok);
  data.c03 = c03->text().toDouble(&ok);
  data.c21 = c21->text().toDouble(&ok);
  data.c12 = c12->text().toDouble(&ok);
  data.c20 = c20->text().toDouble(&ok);
  data.c02 = c02->text().toDouble(&ok);
  data.c11 = c11->text().toDouble(&ok);
  data.c10 = c10->text().toDouble(&ok);
  data.c01 = c01->text().toDouble(&ok);
  data.c00 = c00->text().toDouble(&ok);

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
void ThirdOrderPolynomialWidget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThirdOrderPolynomialWidget::afterPreflight()
{

}
