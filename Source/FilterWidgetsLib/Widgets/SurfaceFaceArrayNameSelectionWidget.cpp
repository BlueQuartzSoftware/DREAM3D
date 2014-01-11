#include "SurfaceFaceArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_SurfaceFaceArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceFaceArrayNameSelectionWidget::SurfaceFaceArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
SurfaceFaceArrayNameSelectionWidget::~SurfaceFaceArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceFaceArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    SurfaceFaceArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

