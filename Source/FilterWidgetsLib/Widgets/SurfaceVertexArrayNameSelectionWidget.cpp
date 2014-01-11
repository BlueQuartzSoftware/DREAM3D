#include "SurfaceVertexArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_SurfaceVertexArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceVertexArrayNameSelectionWidget::SurfaceVertexArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
SurfaceVertexArrayNameSelectionWidget::~SurfaceVertexArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceVertexArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    SurfaceVertexArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

