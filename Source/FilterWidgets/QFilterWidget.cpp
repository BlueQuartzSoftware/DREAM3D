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
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QIntValidator>
#include <QtGui/QDoubleValidator>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QFileDialog>
#include <QtGui/QMouseEvent>


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

#if 1
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::changeStyle(bool selected)
{
  QString style("QGroupBox\n");
  if (selected) {
    style.append("{\n");
    style.append("font: 75 italic 12pt \"Arial\";");
    style.append("font-weight: bold;");
  //  style.append("font-size: 11px;");
    style.append("border-radius: 10px;");
    style.append("border: 3px solid purple;\n");
    style.append("padding: 6px;\n");
    style.append("}\n");

  }
  else
  {
    style.append("{\n");
    style.append("font: 75 italic 12pt \"Arial\";");
    style.append("font-weight: bold;");
   // style.append("font-size: 11px;");
    style.append("border-radius: 10px;");
    style.append("border: 1px solid gray;");
    style.append("padding: 8px;");
    style.append("}\n");
  }
  setStyleSheet(style);
}

#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setupGui()
{


  delete layout();

  //setTitle(getFilter()->getNameOfClass());

  QFormLayout* frmLayout = new QFormLayout(this);
  frmLayout->setObjectName("QFilterWidget QFormLayout Layout");

  changeStyle(false);

  std::vector<FilterOption::Pointer> options = getFilter()->getFilterOptions();
  int optIndex = 0;
  for (std::vector<FilterOption::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {
    FilterOption* option = (*iter).get();
    FilterOption::WidgetType wType = option->getWidgetType();

    if (wType == FilterOption::StringWidget)
    {

    }
    else if (wType == FilterOption::IntWidget)
    {
    //  QHBoxLayout* hLayout = new QHBoxLayout;
    //  hLayout->addWidget(new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      QIntValidator* ival = new QIntValidator;
      le->setValidator(ival);
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      //hLayout->addWidget(le);
      //QSpacerItem* horzSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
      //hLayout->addSpacerItem(horzSpacer);
      //vLayout->addLayout(hLayout);
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditIntValue()));
    }
    else if (wType == FilterOption::DoubleWidget)
    {
//      QHBoxLayout* hLayout = new QHBoxLayout;
//      hLayout->addWidget(new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      QDoubleValidator* ival = new QDoubleValidator;
      le->setValidator(ival);
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, new QLabel(QString::fromStdString(option->getHumanLabel()), this));
//      hLayout->addWidget(le);
//      QSpacerItem* horzSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
//      hLayout->addSpacerItem(horzSpacer);
//      vLayout->addLayout(hLayout);
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditDoubleValue()));
    }
    else if (wType == FilterOption::InputFileWidget)
    {

    //  hLayout->addWidget(new QLabel(QString::fromStdString(option->getHumanLabel()), this));

      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, new QLabel(QString::fromStdString(option->getHumanLabel()), this));

      QFrame* frame = new QFrame(this);
      frame->setContentsMargins(0,0,0,0);
      QGridLayout* frameLayout = new QGridLayout(frame);
      frameLayout->setContentsMargins(0,0,0,0);

      QPushButton* btn = new QPushButton("Select", frame);
      btn->setObjectName(QString::fromStdString(option->getPropertyName()));
      frameLayout->addWidget(btn, 0, 0, 1, 1);
      QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
      frameLayout->addItem(spacer, 0, 1, 1, 1);
      QLabel* fp = new QLabel("Not Set", this);
      fp->setObjectName(QString::fromStdString(option->getHumanLabel()));
      fp->setWordWrap(true);
      fp->setStyleSheet("QLabel {\nfont-weight: bold;\nfont-size: 10px;\n}");
      frameLayout->addWidget(fp, 1, 0, 1, 2);
//      hLayout->addWidget(btn);
//      frmLayout->addLayout(hLayout);
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, frame);
      connect(btn, SIGNAL(clicked()), this, SLOT(selectInputFile()));
    }
    else if (wType == FilterOption::OutputFileWidget)
    {

    }
    else if (wType == FilterOption::BooleanWidget)
    {

    }
    else if (wType == FilterOption::IntConstrainedWidget)
    {
//      QHBoxLayout* hLayout = new QHBoxLayout;
//      hLayout->addWidget(new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      QSpinBox* le = new QSpinBox(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      ConstrainedFilterOption<int>* filtOpt = dynamic_cast<ConstrainedFilterOption<int>* >(option);
      if (filtOpt)
      {
        le->setRange(filtOpt->getMinimum(), filtOpt->getMaximum());
        le->setValue(0);
      }
   //   hLayout->addWidget(le);
  //    QSpacerItem* horzSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
//      hLayout->addSpacerItem(horzSpacer);
//      frmLayout->addLayout(hLayout);
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(valueChanged(int)), this, SLOT(updateQSpinBoxValue(int)));
    }
    else if (wType == FilterOption::DoubleContrainedWidget)
    {
//      QHBoxLayout* hLayout = new QHBoxLayout;
//      hLayout->addWidget(new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      QDoubleSpinBox* le = new QDoubleSpinBox(this);
      le->setObjectName(QString::fromStdString(option->getPropertyName()));
      ConstrainedFilterOption<float>* filtOpt = dynamic_cast<ConstrainedFilterOption<float>* >(option);
      if (filtOpt)
      {
        le->setRange(filtOpt->getMinimum(), filtOpt->getMaximum());
        le->setValue(0);
      }
   //   hLayout->addWidget(le);
  //    QSpacerItem* horzSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
//      hLayout->addSpacerItem(horzSpacer);
//      frmLayout->addLayout(hLayout);
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      connect(le, SIGNAL(valueChanged(double)), this, SLOT(updateQDoubleSpinBoxValue(double)));
    }
    else if (wType == FilterOption::ChoiceWidget)
    {
      ChoiceFilterOption* choiceFilterOption = ChoiceFilterOption::SafeObjectDownCast<FilterOption*, ChoiceFilterOption*>(option);
      if (NULL == choiceFilterOption) { return; }
      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, new QLabel(QString::fromStdString(option->getHumanLabel()), this));

//      QHBoxLayout* hLayout = new QHBoxLayout;
//      hLayout->addWidget(new QLabel(QString::fromStdString(option->getHumanLabel()), this));
      QComboBox* cb = new QComboBox(this);
      cb->setObjectName(QString::fromStdString(option->getPropertyName()));
      std::vector<std::string> choices = choiceFilterOption->getChoices();
      for(unsigned int i = 0; i < choices.size(); ++i)
      {
        cb->addItem(QString::fromStdString(choices[i]));
      }
//      hLayout->addWidget(cb);
//      QSpacerItem* horzSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
//      hLayout->addSpacerItem(horzSpacer);
//      frmLayout->addLayout(hLayout);
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, cb);
      connect(cb, SIGNAL( currentIndexChanged(int)), this, SLOT(updateComboBoxValue(int)));

    }


    ++optIndex;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::selectInputFile()
{
  QObject* whoSent = sender();

  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"),
                                                 "",
                                                 tr("ALL Files (*.*)") );
  if ( true == file.isEmpty() ){ return; }
  bool ok = false;
  ok = setProperty(whoSent->objectName().toStdString().c_str(), file);
  if (false == ok)
  {
    std::cout << "QPushButton '" << title().toStdString() <<  "'Property: '" << whoSent->objectName().toStdString() << "' was NOT set."<< std::endl;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateComboBoxValue(int v)
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQSpinBoxValue(int v)
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateQDoubleSpinBoxValue(double v)
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  QFilterWidget::mouseReleaseEvent ( QMouseEvent* event )
{

  std::cout << title().toStdString() << " mouse released" << std::endl;
  event->setAccepted(true);
  changeStyle(true);
}


