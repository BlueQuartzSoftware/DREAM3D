#include "SurfaceEdgeArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_SurfaceEdgeArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceEdgeArrayNameSelectionWidget::SurfaceEdgeArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
SurfaceEdgeArrayNameSelectionWidget::~SurfaceEdgeArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceEdgeArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    SurfaceEdgeArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

