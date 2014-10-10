#include <FilterParameterWidget.h>


#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/FilterParameters/FilterParameter.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameterWidget::FilterParameterWidget(QWidget *parent) :
QWidget(parent),
m_Filter(NULL),
m_FilterParameter(NULL)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameterWidget::FilterParameterWidget(FilterParameter *parameter, AbstractFilter *filter, QWidget *parent) :
QWidget(parent),
m_Filter(filter),
m_FilterParameter(parameter)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameterWidget::~FilterParameterWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::setLinkedConditionalState(int state)
{
  bool boolProp = (state == Qt::Checked);
  fadeWidget(this, boolProp);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPointer<FaderWidget> FilterParameterWidget::getFaderWidget() const
{
  return m_FaderWidget;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::setFaderWidget(QPointer<FaderWidget> faderWidget)
{
  m_FaderWidget = faderWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::fadeInWidget(QWidget* widget)
{
  if (m_FaderWidget)
  {
    m_FaderWidget->close();
  }
  m_FaderWidget = new FaderWidget(widget);
  m_FaderWidget->start();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::setupGui()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::fadeWidget(QWidget* widget, bool in)
{

  if (m_FaderWidget)
  {
    m_FaderWidget->close();
  }
  m_FaderWidget = new FaderWidget(widget);
  if(in)
  {
    setVisible(true);
    m_FaderWidget->setFadeIn();
    connect(m_FaderWidget, SIGNAL(animationComplete() ),
            this, SLOT(show()));
  }
  else
  {
    m_FaderWidget->setFadeOut();
    connect(m_FaderWidget, SIGNAL(animationComplete() ),
            this, SLOT(hide()));
  }
  QColor color = DREAM3D::Defaults::BasicColor;
  if(getFilterParameter()->getAdvanced()) { color = DREAM3D::Defaults::AdvancedColor; }
  m_FaderWidget->setStartColor(color);
  m_FaderWidget->start();
}
