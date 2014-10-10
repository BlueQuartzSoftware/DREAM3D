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

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/FilterParameters/FilterParameter.h"

#include "FilterParameterWidgetsDialogs.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChoiceWidget::ChoiceWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent)
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

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  connect(value, SIGNAL(currentIndexChanged(int)),
          this, SLOT(widgetChanged(int) ) );

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

    // setup the list of choices for the widget
    ChoiceFilterParameter* choice = dynamic_cast<ChoiceFilterParameter*>(getFilterParameter());
    if(choice)
    {
      QList<QString> choices = choice->getChoices().toList();
      value->blockSignals(true);
      value->addItems(choices);
      value->blockSignals(false);
    }
    // Get the Default value from the filter
    int i = getFilter()->property(PROPERTY_NAME_AS_CHAR).toInt();
    value->setCurrentIndex(i);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ChoiceWidget::getCurrentIndex()
{
  return value->currentIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChoiceWidget::widgetChanged(int index)
{

  FilterParameter* fp = getFilterParameter();

  // If the parameter type is LinkedChoicesFilterParameter then we need to emit the index that
  // was selected
  LinkedChoicesFilterParameter* lnkedChoices = dynamic_cast<LinkedChoicesFilterParameter*>(fp);
  if(lnkedChoices)
  {
    emit conditionalPropertyChanged(index);
  }


  // We emit no matter what
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChoiceWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  int index = value->currentIndex();
  QVariant v(index);
  bool ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, v);

  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChoiceWidget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ChoiceWidget::afterPreflight()
{

}
