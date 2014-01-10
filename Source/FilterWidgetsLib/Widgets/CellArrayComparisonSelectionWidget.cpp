#include "CellArrayComparisonSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_CellArrayComparisonSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CellArrayComparisonSelectionWidget::CellArrayComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
CellArrayComparisonSelectionWidget::~CellArrayComparisonSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CellArrayComparisonSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    CellArrayComparisonSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

