#include "EdgeEnsembleArrayNameSelectionWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_EdgeEnsembleArrayNameSelectionWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeEnsembleArrayNameSelectionWidget::EdgeEnsembleArrayNameSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
EdgeEnsembleArrayNameSelectionWidget::~EdgeEnsembleArrayNameSelectionWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EdgeEnsembleArrayNameSelectionWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    EdgeEnsembleArrayNameSelectionWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

