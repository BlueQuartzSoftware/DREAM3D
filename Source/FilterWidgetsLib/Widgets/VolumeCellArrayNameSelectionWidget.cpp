#include "VolumeCellArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_VolumeCellArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeCellArrayNameSelectionWidget::VolumeCellArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
VolumeCellArrayNameSelectionWidget::~VolumeCellArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeCellArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    VolumeCellArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

