/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "FilterListDockWidget.h"

#include <QtCore/QFileInfo>
#include <QtGui/QTreeWidgetItem>

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "DREAM3DWidgetsLib/moc_FilterListDockWidget.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListDockWidget::FilterListDockWidget(QWidget* parent) :
  QDockWidget(parent),
  m_SearchFilterClassName(false),
  m_SearchFilterHumanName(true),
  m_SearchParameterPropertyName(false),
  m_SearchParameterHumanName(true)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListDockWidget::~FilterListDockWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::setupGui()
{

  {
    m_ActionSearchFilterHumanName = new QAction(&m_Menu);
    m_ActionSearchFilterHumanName->setObjectName(QString::fromUtf8("actionFilterHumanLabel"));
    m_ActionSearchFilterHumanName->setText(QApplication::translate("DREAM3D_UI", "Filter Human Name", 0, QApplication::UnicodeUTF8));
    m_ActionSearchFilterHumanName->setCheckable(true);
    m_ActionSearchFilterHumanName->setChecked(m_SearchFilterHumanName);
    m_Menu.addAction(m_ActionSearchFilterHumanName);
    connect(m_ActionSearchFilterHumanName, SIGNAL(triggered()),
            this, SLOT( searchFieldsChanged() ) );
  }
  {
    m_ActionSearchFilterClassName = new QAction(&m_Menu);
    m_ActionSearchFilterClassName->setObjectName(QString::fromUtf8("actionFilterClassName"));
    m_ActionSearchFilterClassName->setText(QApplication::translate("DREAM3D_UI", "Filter Class Name", 0, QApplication::UnicodeUTF8));
    m_ActionSearchFilterClassName->setCheckable(true);
    m_ActionSearchFilterClassName->setChecked(m_SearchFilterClassName);
    m_Menu.addAction(m_ActionSearchFilterClassName);
    connect(m_ActionSearchFilterClassName, SIGNAL(triggered()),
            this, SLOT( searchFieldsChanged() ) );
  }

  {
    m_ActionSearchParameterName = new QAction(&m_Menu);
    m_ActionSearchParameterName->setObjectName(QString::fromUtf8("actionFilterParameterName"));
    m_ActionSearchParameterName->setText(QApplication::translate("DREAM3D_UI", "Parameter Name", 0, QApplication::UnicodeUTF8));
    m_ActionSearchParameterName->setCheckable(true);
    m_ActionSearchParameterName->setChecked(m_SearchParameterHumanName);
    m_Menu.addAction(m_ActionSearchParameterName);
    connect(m_ActionSearchParameterName, SIGNAL(triggered()),
            this, SLOT( searchFieldsChanged() ) );
  }
  {
    m_ActionSearchParameterPropertyName = new QAction(&m_Menu);
    m_ActionSearchParameterPropertyName->setObjectName(QString::fromUtf8("actionFilterParameterPropertyName"));
    m_ActionSearchParameterPropertyName->setText(QApplication::translate("DREAM3D_UI", "Parameter Property Name", 0, QApplication::UnicodeUTF8));
    m_ActionSearchParameterPropertyName->setCheckable(true);
    m_ActionSearchParameterPropertyName->setChecked(m_SearchParameterPropertyName);
    m_Menu.addAction(m_ActionSearchParameterPropertyName);
    connect(m_ActionSearchParameterPropertyName, SIGNAL(triggered()),
            this, SLOT( searchFieldsChanged() ) );
  }

  searchFieldsBtn->setText("");
  searchFieldsBtn->setMenu(&m_Menu);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::updateFilterList(const QStringList& list, bool sortItems)
{
  filterList->clear();

  FilterManager::Pointer fm = FilterManager::Instance();


  for (int i = 0; i < list.size(); i++)
  {
    QString filterName = list.at(i);
    IFilterFactory::Pointer wigFactory = fm->getFactoryForFilter(filterName);
    if (NULL == wigFactory.get() )
    {
      continue;
    }
    QString humanName = (wigFactory->getFilterHumanLabel());
    QString iconName(":/");
    iconName.append( (wigFactory->getFilterGroup()));
    iconName.append("_Icon.png");

    // Validate the icon is in the resource system
    QFileInfo iconInfo(iconName);
    if (iconInfo.exists() == false)
    {
      iconName = ":/Plugin_Icon.png"; // Switch to our generic icon for Plugins that do not provide their own
    }

    QIcon icon(iconName);
    // Create the QListWidgetItem and add it to the filterList
    QListWidgetItem* filterItem = new QListWidgetItem(icon, humanName, filterList);
    // Set an "internal" QString that is the name of the filter. We need this value
    // when the item is clicked in order to retreive the Filter Widget from the
    // filter widget manager.
    filterItem->setData( Qt::UserRole, filterName);
  }
  if (sortItems)
  {
    filterList->sortItems(Qt::AscendingOrder);
  }
  filterSearch->clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::on_filterSearch_textChanged (const QString& text)
{

  int listWidgetSize = filterList->count();
  if(text.isEmpty() )
  {
    for (int k1 = 0; k1 < listWidgetSize; k1++)
    {
      filterList->item(k1)->setHidden(false);
    }
    return;
  }

  // The user is typing something in the search box so lets search the filter class name and human label

  FilterManager::Pointer fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories(); // Get all the Factories
  //qDebug() << "Search Text: " << text;
  bool match = false;
  for (int k1 = 0; k1 < listWidgetSize; k1++)
  {
    match = false;
    QString filterHumanLabel = filterList->item(k1)->text();

    if (m_ActionSearchFilterHumanName->isChecked() && filterHumanLabel.contains(text, Qt::CaseInsensitive) == true)
    {
      //  qDebug() << "   " << filterHumanLabel;
      match = true;
    }

    QString filterClassName = filterList->item(k1)->data(Qt::UserRole).toString();
    if (m_ActionSearchFilterClassName->isChecked() && filterClassName.contains(text, Qt::CaseInsensitive) == true)
    {
      //   qDebug() << "   " << filterClassName;
      match = true;
    }

    IFilterFactory::Pointer factory = fm->getFactoryForFilter(filterClassName);
    if(NULL != factory.get() )
    {
      AbstractFilter::Pointer filter = factory->create();
      // Get a list of all the filterParameters from the filter.
      QVector<FilterParameter::Pointer> filterParameters = filter->getFilterParameters();
      // Create all the FilterParameterWidget objects that can be displayed where ever the developer needs
      for (QVector<FilterParameter::Pointer>::iterator iter = filterParameters.begin(); iter != filterParameters.end(); ++iter )
      {
        FilterParameter* option = (*iter).get();
        if (m_ActionSearchParameterName->isChecked() && option->getHumanLabel().contains(text, Qt::CaseInsensitive) == true)
        {
          //  qDebug() << "       " << option->getHumanLabel();
          match = true;
        }
        if (m_ActionSearchParameterPropertyName->isChecked() && option->getPropertyName().contains(text, Qt::CaseInsensitive) == true)
        {
          //   qDebug() << "       " << option->getPropertyName();
          match = true;
        }
      }
    }
    filterList->item(k1)->setHidden(!match);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::searchFieldsChanged()
{

  on_filterSearch_textChanged(filterSearch->text());

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::on_filterList_itemDoubleClicked( QListWidgetItem* item )
{
  emit filterItemDoubleClicked(item->data(Qt::UserRole).toString());
}
