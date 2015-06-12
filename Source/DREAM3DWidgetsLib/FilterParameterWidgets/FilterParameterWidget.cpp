#include <FilterParameterWidget.h>

#include <QtWidgets/QApplication>

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/FilterParameters/FilterParameter.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameterWidget::FilterParameterWidget(QWidget* parent) :
  QWidget(parent),
  m_Filter(NULL),
  m_FilterParameter(NULL),
  m_Timer(NULL)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameterWidget::FilterParameterWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  QWidget(parent),
  m_Filter(filter),
  m_FilterParameter(parameter),
  m_Timer(NULL)
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
void FilterParameterWidget::setLinkedComboBoxState(int groupId)
{
  if(m_FilterParameter)
  {
    int groupIndex = m_FilterParameter->getGroupIndex();
    int state = (groupIndex == groupId) ? Qt::Checked : Qt::Unchecked;
    {
      setLinkedConditionalState(state);
    }
  }
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
    connect(m_FaderWidget, SIGNAL(animationComplete()),
      this, SLOT(showBorder()));

    widget->setStyleSheet("border: 2px solid MediumBlue;");
  }
  else
  {
    m_FaderWidget->setFadeOut();
    connect(m_FaderWidget, SIGNAL(animationComplete() ),
            this, SLOT(hide()));
  }

  m_FaderWidget->setStartColor(palette().color(QWidget::backgroundRole()));
  m_FaderWidget->start();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::showBorder()
{
  if (NULL != m_Timer)
  {
    delete m_Timer;
    m_Timer = NULL;
  }
  m_Timer = new QTimer(this);
  connect(m_Timer, SIGNAL(timeout()), this, SLOT(hideBorder()));
  m_Timer->start(4000);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidget::hideBorder()
{
  setStyleSheet("");
}
