#include "EdgeFeatureArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_EdgeFeatureArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeFeatureArrayNameSelectionWidget::EdgeFeatureArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
EdgeFeatureArrayNameSelectionWidget::~EdgeFeatureArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeFeatureArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    EdgeFeatureArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

