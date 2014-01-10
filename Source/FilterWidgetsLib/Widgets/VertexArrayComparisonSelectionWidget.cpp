#include "VertexArrayComparisonSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_VertexArrayComparisonSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexArrayComparisonSelectionWidget::VertexArrayComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
VertexArrayComparisonSelectionWidget::~VertexArrayComparisonSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexArrayComparisonSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    VertexArrayComparisonSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

