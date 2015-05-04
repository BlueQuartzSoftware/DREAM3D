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
#include "IntWidget.h"

#include <QtCore/QMetaProperty>

#include "QtSupportLib/DREAM3DStyles.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntWidget::IntWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntWidget::~IntWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntWidget::setupGui()
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

  connect(value, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&)));

  QIntValidator* xVal = new QIntValidator(value);
  value->setValidator(xVal);

  if (getFilterParameter() != NULL)
  {
    QString units = getFilterParameter()->getUnits();
    if(units.isEmpty() == false)
    {
      label->setText(getFilterParameter()->getHumanLabel() + " (" + units + ")");
    }
    else
    {
      label->setText(getFilterParameter()->getHumanLabel() );
    }

    QString str = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
    value->setText(str);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntWidget::widgetChanged(const QString& text)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  bool ok = true;
  int defValue = getFilterParameter()->getDefaultValue().toInt();
  int i = defValue;

  // Next make sure there is something in the
  if (!value->text().isEmpty())
  {
    i = value->text().toInt(&ok);
    //  make sure we can convert the entered value to a 32 bit signed int
    if (!ok)
    {
      errorLabel->setStyleSheet(QString::fromLatin1("color: rgb(255, 0, 0);"));
      errorLabel->setText("Value entered is beyond the representable range for a 32 bit integer. The filter will use the default value of " + getFilterParameter()->getDefaultValue().toString());
      errorLabel->show();
      DREAM3DStyles::LineEditErrorStyle(value);
      i = defValue;
    }
    else
    {
      errorLabel->hide();
      DREAM3DStyles::LineEditClearStyle(value);
    }
  }
  else
  {
    DREAM3DStyles::LineEditErrorStyle(value);
    errorLabel->setStyleSheet(QString::fromLatin1("color: rgb(255, 0, 0);"));
    errorLabel->setText("No value entered. Filter will use default value of " + getFilterParameter()->getDefaultValue().toString());
    errorLabel->show();
  }

  QVariant v(i);
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, v);
  if (false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntWidget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntWidget::afterPreflight()
{

}
