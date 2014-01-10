#include "OutputPathWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_OutputPathWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OutputPathWidget::OutputPathWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
OutputPathWidget::~OutputPathWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputPathWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    OutputPathWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

