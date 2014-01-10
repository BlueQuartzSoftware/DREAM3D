#include "FloatVec3Widget.h"

#include <QtCore/QMetaProperty>


#include "FilterWidgetsLib/Widgets/moc_FloatVec3Widget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FloatVec3Widget::FloatVec3Widget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
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
FloatVec3Widget::~FloatVec3Widget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FloatVec3Widget::setupGui()
{
  if (m_FilterParameter != NULL)
  {
    FloatVec3WidgetLabel->setText(m_FilterParameter->getHumanLabel() );
  }
}

