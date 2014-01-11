#include "VolumeVertexArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_VolumeVertexArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeVertexArrayNameSelectionWidget::VolumeVertexArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
VolumeVertexArrayNameSelectionWidget::~VolumeVertexArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeVertexArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    VolumeVertexArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

