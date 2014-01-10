#include "ChoiceWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_ChoiceWidget.cxx"


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
  if (m_FilterParameter != NULL)
  {
    ChoiceWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

