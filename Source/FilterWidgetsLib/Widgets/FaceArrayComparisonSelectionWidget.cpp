#include "FaceArrayComparisonSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_FaceArrayComparisonSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FaceArrayComparisonSelectionWidget::FaceArrayComparisonSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
FaceArrayComparisonSelectionWidget::~FaceArrayComparisonSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FaceArrayComparisonSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    FaceArrayComparisonSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

