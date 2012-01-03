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
#include <QtGui/QLineEdit>
#include <QtGui/QIntValidator>
#include <QtGui/QDoubleValidator>
#include <QtGui/QComboBox>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget::QFilterWidget(QWidget* parent) :
      QGroupBox(parent)
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

  QString style("QGroupBox {\n");
  style.append("margin: 10px;\n");
  style.append("border: 2px solid green;\n");
  style.append("padding: 6px;\n");

  style.append("background-color: gray;\n");
  style.append("background-origin: content;\n");
  style.append("background-repeat: none;\n");
  style.append("}\n");
 // setStyleSheet(style);

  //setTitle(getFilter()->getNameOfClass());

  QVBoxLayout* vLayout = new QVBoxLayout(this);
  vLayout->setObjectName("QFilterWidget Vertical Layout");

  std::vector<FilterOption::Pointer> options = getFilter()->getFilterOptions();

  for (std::vector<FilterOption::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {
    FilterOption* option = (*iter).get();
    FilterOption::WidgetType wType = option->getWidgetType();

    if (wType == FilterOption::StringWidget)
    {

    }
    else if (wType == FilterOption::IntWidget)
    {
      QHBoxLayout* hLayout = new QHBoxLayout;
      hLayout->addWidget(new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      QIntValidator* ival = new QIntValidator;
      le->setValidator(ival);
      hLayout->addWidget(le);
      vLayout->addLayout(hLayout);
      connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditIntValue()));
    }
    else if (wType == FilterOption::DoubleWidget)
    {
      QHBoxLayout* hLayout = new QHBoxLayout;
      hLayout->addWidget(new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      QDoubleValidator* ival = new QDoubleValidator;
      le->setValidator(ival);
      hLayout->addWidget(le);
      vLayout->addLayout(hLayout);
      connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditDoubleValue()));
    }
    else if (wType == FilterOption::InputFileWidget)
    {

    }
    else if (wType == FilterOption::OutputFileWidget)
    {

    }
    else if (wType == FilterOption::BooleanWidget)
    {

    }
    else if (wType == FilterOption::IntConstrainedWidget)
    {

    }
    else if (wType == FilterOption::DoubleContrainedWidget)
    {

    }
    else if (wType == FilterOption::ChoiceWidget)
    {
      ChoiceFilterOption* choiceFilterOption = ChoiceFilterOption::SafeObjectDownCast<FilterOption*, ChoiceFilterOption*>(option);
      if (NULL == choiceFilterOption) { return; }
      QHBoxLayout* hLayout = new QHBoxLayout;
      hLayout->addWidget(new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      QComboBox* cb = new QComboBox(this);
      cb->setObjectName(QString::fromStdString(option->getPropertyName()));
      std::vector<std::string> choices = choiceFilterOption->getChoices();
      for(unsigned int i = 0; i < choices.size(); ++i)
      {
        cb->addItem(QString::fromStdString(choices[i]));
      }
      hLayout->addWidget(cb);
      vLayout->addLayout(hLayout);
    }
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateFilterValues()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QFilterWidget::getFilter()
{
  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQLineEditIntValue()
{
  QObject* whoSent = sender();
  std::cout << "Filter: " << title().toStdString() << " Getting updated from whoSent Name: "
      << whoSent->objectName().toStdString() << std::endl;
  QLineEdit* le = qobject_cast<QLineEdit*>(whoSent);
  if(le) {
    bool ok = false;
    setProperty(whoSent->objectName().toStdString().c_str(), le->text().toInt(&ok));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQLineEditDoubleValue()
{
  QObject* whoSent = sender();
  std::cout << "Filter: " << title().toStdString() << " Getting updated from whoSent Name: "
      << whoSent->objectName().toStdString() << std::endl;
  QLineEdit* le = qobject_cast<QLineEdit*>(whoSent);
  if(le) {
    bool ok = false;
    ok = setProperty(whoSent->objectName().toStdString().c_str(), le->text().toDouble(&ok));
    if (false == ok)
    {
      std::cout << "QLineEdit '" << title().toStdString() <<  "'Property: '" << whoSent->objectName().toStdString() << "' was NOT set."<< std::endl;
    }
  }
}


