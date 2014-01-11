#include "VolumeFaceArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_VolumeFaceArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeFaceArrayNameSelectionWidget::VolumeFaceArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
VolumeFaceArrayNameSelectionWidget::~VolumeFaceArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeFaceArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    VolumeFaceArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

