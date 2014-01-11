#include "VolumeFeatureArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_VolumeFeatureArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeFeatureArrayNameSelectionWidget::VolumeFeatureArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
VolumeFeatureArrayNameSelectionWidget::~VolumeFeatureArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeFeatureArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    VolumeFeatureArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

