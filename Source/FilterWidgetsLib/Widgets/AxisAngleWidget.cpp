#include "AxisAngleWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_AxisAngleWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AxisAngleWidget::AxisAngleWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
AxisAngleWidget::~AxisAngleWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AxisAngleWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    AxisAngleWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

