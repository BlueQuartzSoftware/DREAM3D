#include "EdgeArrayComparisonSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_EdgeArrayComparisonSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeArrayComparisonSelectionWidget::EdgeArrayComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
EdgeArrayComparisonSelectionWidget::~EdgeArrayComparisonSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeArrayComparisonSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    EdgeArrayComparisonSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

