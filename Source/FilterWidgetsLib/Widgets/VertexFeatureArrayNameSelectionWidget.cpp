#include "VertexFeatureArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_VertexFeatureArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexFeatureArrayNameSelectionWidget::VertexFeatureArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
VertexFeatureArrayNameSelectionWidget::~VertexFeatureArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexFeatureArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    VertexFeatureArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

