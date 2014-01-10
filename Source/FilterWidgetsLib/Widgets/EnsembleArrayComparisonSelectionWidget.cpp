#include "EnsembleArrayComparisonSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_EnsembleArrayComparisonSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnsembleArrayComparisonSelectionWidget::EnsembleArrayComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
EnsembleArrayComparisonSelectionWidget::~EnsembleArrayComparisonSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnsembleArrayComparisonSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    EnsembleArrayComparisonSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

