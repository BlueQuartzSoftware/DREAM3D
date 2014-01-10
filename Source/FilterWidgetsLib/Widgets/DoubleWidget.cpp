#include "DoubleWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_DoubleWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DoubleWidget::DoubleWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
DoubleWidget::~DoubleWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DoubleWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    DoubleWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

