#include "VertexVertexArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_VertexVertexArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexVertexArrayNameSelectionWidget::VertexVertexArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
VertexVertexArrayNameSelectionWidget::~VertexVertexArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VertexVertexArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    VertexVertexArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

