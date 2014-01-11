#include "SurfaceEnsembleArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_SurfaceEnsembleArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceEnsembleArrayNameSelectionWidget::SurfaceEnsembleArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
SurfaceEnsembleArrayNameSelectionWidget::~SurfaceEnsembleArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceEnsembleArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    SurfaceEnsembleArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

