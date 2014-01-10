#include "VolumeEnsembleArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_VolumeEnsembleArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeEnsembleArrayNameSelectionWidget::VolumeEnsembleArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
VolumeEnsembleArrayNameSelectionWidget::~VolumeEnsembleArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeEnsembleArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    VolumeEnsembleArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

