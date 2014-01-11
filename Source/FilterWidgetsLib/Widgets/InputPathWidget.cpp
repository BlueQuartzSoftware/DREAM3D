#include "InputPathWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_InputPathWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InputPathWidget::InputPathWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
InputPathWidget::~InputPathWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputPathWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    InputPathWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

