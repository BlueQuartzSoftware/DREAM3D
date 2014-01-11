#include "EdgeEdgeArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_EdgeEdgeArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeEdgeArrayNameSelectionWidget::EdgeEdgeArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
EdgeEdgeArrayNameSelectionWidget::~EdgeEdgeArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeEdgeArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    EdgeEdgeArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

