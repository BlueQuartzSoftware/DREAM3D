#include "CustomWidget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_CustomWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CustomWidget::CustomWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
CustomWidget::~CustomWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CustomWidget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    CustomWidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

