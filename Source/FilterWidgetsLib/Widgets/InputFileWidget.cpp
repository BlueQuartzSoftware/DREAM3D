#include "InputFileWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_InputFileWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
InputFileWidget::InputFileWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
InputFileWidget::~InputFileWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void InputFileWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    InputFileWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

