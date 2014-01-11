#include "OutputFileWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_OutputFileWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OutputFileWidget::OutputFileWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
OutputFileWidget::~OutputFileWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OutputFileWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    OutputFileWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

