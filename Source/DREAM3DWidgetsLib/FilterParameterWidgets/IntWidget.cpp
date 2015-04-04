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

#include "QtSupport/DREAM3DStyles.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"


#define FS_STANDARD_STYLE 0
#define FS_DRAGGING_STYLE 1
#define FS_DOESNOTEXIST_STYLE 2
#define FS_WARNING_STYLE 3
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
  int i = getFilterParameter()->getDefaultValue().toInt();
  if (!value->text().isEmpty())
  {
    i = value->text().toInt(&ok);
    errorLabel->hide();
  }
  else
  {
    errorLabel->setStyleSheet(QString::fromLatin1("color: rgb(255, 0, 0);"));
    errorLabel->setText("Filter will use default value of " + getFilterParameter()->getDefaultValue().toString());
    errorLabel->show();
  }

  DREAM3DStyles::LineEditErrorStyle(value);

  if (ok)
  {
    QVariant v(i);
    ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, v);
    if (false == ok)
    {
      FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
    }
  }
  else 	// Check for Empty String. If empty, show error dialog
  {
    // Some error message "Could not convert string to an integer"
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString IntWidget::generateStyleSheet(int style)
{
  QString styleSheet;
  QTextStream ss(&styleSheet);

  ss << "QLineEdit {";
#if defined(Q_OS_WIN)
  ss << "font: italic 9 pt \"Arial\";";
#elif defined(Q_OS_MAC)
  ss << "font: italic 12 pt \"Arial\";";
#else
  ss << "font: italic 10 pt \"Arial\";";
#endif

  if (style == FS_STANDARD_STYLE)
  {

  }
  else if (style == FS_DRAGGING_STYLE)
  {
    ss << "border: 2px solid rgb(34, 170, 46);";
    ss << "border-radius: 5px;";
  }
  else if (style == FS_DOESNOTEXIST_STYLE)
  {
    ss << "color: rgb(200, 50, 50); font: bold;";
  }
  else if (style == FS_WARNING_STYLE)
  {
    ss << "color: rgb(255, 140, 0); font: bold;";
  }


  ss << "}";

  return styleSheet;
}
