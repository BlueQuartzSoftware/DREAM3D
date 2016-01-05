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

#include "FloatVec3Widget.h"

#include <QtCore/QMetaProperty>

#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"

#include "QtSupportLib/DREAM3DStyles.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// Include the MOC generated file for this class
#include "moc_FloatVec3Widget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3Widget::FloatVec3Widget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent)
{
  m_FilterParameter = dynamic_cast<FloatVec3FilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "FloatVec3Widget can ONLY be used with a FloatVec3FilterParameter object");

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
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));


  connect(xData, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(yData, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(zData, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );

  QLocale loc = QLocale::system();

  QDoubleValidator* xVal = new QDoubleValidator(xData);
  xData->setValidator(xVal);
  xVal->setLocale(loc);

  QDoubleValidator* yVal = new QDoubleValidator(yData);
  yData->setValidator(yVal);
  yVal->setLocale(loc);

  QDoubleValidator* zVal = new QDoubleValidator(zData);
  zData->setValidator(zVal);
  zVal->setLocale(loc);
  if (getFilterParameter() != NULL)
  {
    label->setText(getFilterParameter()->getHumanLabel() );

    FloatVec3_t data = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<FloatVec3_t>();

    xData->setText(loc.toString(data.x));
    yData->setText(loc.toString(data.y));
    zData->setText(loc.toString(data.z));
  }

  errorLabel->hide();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec3Widget::widgetChanged(const QString& text)
{
  Q_UNUSED(text);

  QLineEdit* le = NULL;
  (sender() == xData) ? le = xData: le = NULL;
  (sender() == yData) ? le = yData: le = NULL;
  (sender() == zData) ? le = zData: le = NULL;

  errorLabel->hide();

  if(le)
  {
    if(le->text().isEmpty())
    {
      QString objName = le->objectName();
      DREAM3DStyles::LineEditErrorStyle(le);
      errorLabel->setStyleSheet(QString::fromLatin1("color: rgb(255, 0, 0);"));
      errorLabel->setText("No value entered. Filter will use default value of " + getFilterParameter()->getDefaultValue().toString());
      errorLabel->show();
    }
  }

  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec3Widget::filterNeedsInputParameters(AbstractFilter* filter)
{
  bool ok = false;
  FloatVec3_t defValue = m_FilterParameter->getDefaultValue().value<FloatVec3_t>();
  FloatVec3_t data;

  QLocale loc;


  data.x = loc.toFloat(xData->text(), &ok);
  if(!ok)
  {
    errorLabel->setStyleSheet(QString::fromLatin1("color: rgb(255, 0, 0);"));
    errorLabel->setText("X Value entered is beyond the representable range for a double.\nThe filter will use the default value of " + getFilterParameter()->getDefaultValue().toString());
    errorLabel->show();
    DREAM3DStyles::LineEditErrorStyle(xData);
    data.x = defValue.x;
  }

  data.y = loc.toFloat(yData->text(), &ok);
  if(!ok)
  {
    errorLabel->setStyleSheet(QString::fromLatin1("color: rgb(255, 0, 0);"));
    errorLabel->setText("Y Value entered is beyond the representable range for a double.\nThe filter will use the default value of " + getFilterParameter()->getDefaultValue().toString());
    errorLabel->show();
    DREAM3DStyles::LineEditErrorStyle(xData);
    data.y = defValue.y;
  }

  data.z = loc.toFloat(zData->text(), &ok);
  if(!ok)
  {
    errorLabel->setStyleSheet(QString::fromLatin1("color: rgb(255, 0, 0);"));
    errorLabel->setText("Z Value entered is beyond the representable range for a double.\nThe filter will use the default value of " + getFilterParameter()->getDefaultValue().toString());
    errorLabel->show();
    DREAM3DStyles::LineEditErrorStyle(zData);
    data.z = defValue.z;
  }


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
void FloatVec3Widget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec3Widget::afterPreflight()
{

}
