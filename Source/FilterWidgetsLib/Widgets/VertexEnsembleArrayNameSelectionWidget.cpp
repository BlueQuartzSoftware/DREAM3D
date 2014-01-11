#include "VertexEnsembleArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_VertexEnsembleArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexEnsembleArrayNameSelectionWidget::VertexEnsembleArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
VertexEnsembleArrayNameSelectionWidget::~VertexEnsembleArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexEnsembleArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    VertexEnsembleArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

