#include "SurfaceFeatureArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_SurfaceFeatureArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceFeatureArrayNameSelectionWidget::SurfaceFeatureArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
SurfaceFeatureArrayNameSelectionWidget::~SurfaceFeatureArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceFeatureArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    SurfaceFeatureArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

