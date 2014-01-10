#include "IntWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_IntWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntWidget::IntWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
IntWidget::~IntWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    IntWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

