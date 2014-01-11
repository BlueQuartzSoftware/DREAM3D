#include "BooleanWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_BooleanWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BooleanWidget::BooleanWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
BooleanWidget::~BooleanWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BooleanWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    BooleanWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

