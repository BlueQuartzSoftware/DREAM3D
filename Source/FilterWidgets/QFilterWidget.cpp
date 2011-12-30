/*
 * QFilterWidget.cpp
 *
 *  Created on: Dec 30, 2011
 *      Author: mjackson
 */

#include "QFilterWidget.h"


#include "DREAM3DLib/Common/FilterOption.h"
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QSpinBox>
#include <QtGui/QLabel>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget::QFilterWidget(QWidget* parent) :
      QFrame(parent)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget::~QFilterWidget(){}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setupGui()
{
  delete layout();

  QVBoxLayout* vLayout = new QVBoxLayout(this);
  vLayout->setObjectName("QFilterWidget Vertical Layout");

  std::vector<FilterOption::Pointer> options = getFilter()->getFilterOptions();

  for (std::vector<FilterOption::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {
    FilterOption* option = (*iter).get();

    if (option->getWidgetType() == FilterOption::IntWidget)
    {
      QHBoxLayout* hLayout = new QHBoxLayout;
      hLayout->addWidget(new QLabel(QString::fromStdString(option->getLabel()), this));
      hLayout->addWidget(new QSpinBox(this));

      vLayout->addLayout(hLayout);
    }




  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QFilterWidget::getFilter()
{
  return AbstractFilter::NullPointer();
}


