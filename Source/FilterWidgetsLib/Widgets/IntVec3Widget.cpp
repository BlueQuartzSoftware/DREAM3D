#include "IntVec3Widget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_IntVec3Widget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntVec3Widget::IntVec3Widget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
IntVec3Widget::~IntVec3Widget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntVec3Widget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    IntVec3WidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

