#include "EdgeVertexArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_EdgeVertexArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeVertexArrayNameSelectionWidget::EdgeVertexArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
EdgeVertexArrayNameSelectionWidget::~EdgeVertexArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeVertexArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    EdgeVertexArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

