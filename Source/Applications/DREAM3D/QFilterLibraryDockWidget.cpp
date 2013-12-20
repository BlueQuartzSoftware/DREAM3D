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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "QFilterLibraryDockWidget.h"

#include <QtCore/QFileInfo>


#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "QFilterWidget.h"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterLibraryDockWidget::QFilterLibraryDockWidget(QWidget* parent) :
QDockWidget(parent)
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterLibraryDockWidget::~QFilterLibraryDockWidget()
{
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterLibraryDockWidget::setupGui()
{
    FilterManager::Pointer fm = FilterManager::Instance();
    QSet<QString> groupNames = fm->getGroupNames();

    // Clear out the default stuff
    filterLibraryTree->clear();
    QTreeWidgetItem* library = new QTreeWidgetItem(filterLibraryTree);
    library->setText(0, DREAM3D::Settings::Library);
    library->setIcon(0, QIcon(":/cubes.png"));

    for(QSet<QString>::iterator iter = groupNames.begin(); iter != groupNames.end(); ++iter)
    {
      //   qDebug() << *iter << "\n";
      QString iconName(":/");
      iconName.append( (*iter));
      iconName.append("_Icon.png");
      // Validate the icon is in the resource system
      QFileInfo iconInfo(iconName);
      if (iconInfo.exists() == false)
      {
        iconName = ":/Plugin_Icon.png"; // Switch to our generic icon for Plugins that do not provide their own
      }

      QIcon icon(iconName);
      QTreeWidgetItem* filterGroup = new QTreeWidgetItem(library);
      filterGroup->setText(0, (*iter));
      filterGroup->setIcon(0, icon);
      QSet<QString> subGroupNames = fm->getSubGroupNames(*iter);
      for(QSet<QString>::iterator iter2 = subGroupNames.begin(); iter2 != subGroupNames.end(); ++iter2)
      {
        QTreeWidgetItem* filterSubGroup = new QTreeWidgetItem(filterGroup);
        filterSubGroup->setText(0, (*iter2));
      }
    }
    library->setExpanded(true);


}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterLibraryDockWidget::on_filterLibraryTree_itemClicked( QTreeWidgetItem* item, int column )
{
  // Get the QFilterWidget Manager Instance
  FilterManager::Pointer fm = FilterManager::Instance();
  FilterManager::Collection factories;
  QTreeWidgetItem* parent = item->parent();
  while(NULL != parent)
  {
    if (NULL == parent->parent() )
    {
      break;
    }
    parent = parent->parent();
  }
  if (parent == NULL)
  {
    return;
  }

  QString itemText = parent->text(0);

#if 0
  if (itemText.compare(Detail::Library) == 0)
  {
    factories = fm->getFactories();
    updateFilterGroupList(factories);
  }
#endif

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterLibraryDockWidget::on_filterLibraryTree_itemChanged( QTreeWidgetItem* item, int column )
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterLibraryDockWidget::on_filterLibraryTree_currentItemChanged(QTreeWidgetItem* item, QTreeWidgetItem* previous )
{
  // Get the QFilterWidget Manager Instance
  FilterManager::Pointer fm = FilterManager::Instance();
  FilterManager::Collection factories;

  //If the user clicks on Detail::Library, display all
  if ( item->text(0).compare(DREAM3D::Settings::Library) == 0)
  {
    factories = fm->getFactories();
    updateFilterGroupList(factories);
  }
  else if (NULL != item->parent() && item->parent()->text(0).compare(DREAM3D::Settings::Library) == 0)
  {
    factories = fm->getFactories(item->text(0));
    updateFilterGroupList(factories);
  }
  else if (NULL != item->parent() && NULL != item->parent()->parent() && item->parent()->parent()->text(0).compare(DREAM3D::Settings::Library) == 0)
  {
    factories = fm->getFactories(item->parent()->text(0), item->text(0));
    updateFilterGroupList(factories);
  }
  else
  {
    // Update filter list with preview of current item
    on_filterLibraryTree_itemClicked(item, 0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterLibraryDockWidget::on_filterLibraryTree_itemDoubleClicked( QTreeWidgetItem* item, int column )
{
  QTreeWidgetItem* parent = item->parent();

  while(NULL != parent)
  {
    if (NULL == parent->parent() )
    {
      break;
    }
    parent = parent->parent();
  }
  if (parent == NULL)
  {
    return;
  }

  QString itemText = parent->text(0);

}




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterLibraryDockWidget::updateFilterGroupList(FilterManager::Collection& factories)
{
  // Clear all the current items from the list
  filterList->clear();
  filterList->setSortingEnabled(true);

  for (FilterManager::Collection::iterator factory = factories.begin(); factory != factories.end(); ++factory)
  {
    IFilterFactory::Pointer wigFactory = factory.value();
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
    QString filterName = (factory.key());
    filterItem->setData( Qt::UserRole, filterName);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterLibraryDockWidget::on_filterSearch_textChanged (const QString& text)
{

  FilterManager::Pointer fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories(); // Get all the Factories
  QTreeWidgetItem* item = filterLibraryTree->currentItem();
  if (item->parent() == NULL && item->text(0).compare(DREAM3D::Settings::Library) == 0)
  {
    factories = fm->getFactories();
  }
  else if (item->parent() != NULL && item->parent()->text(0).compare(DREAM3D::Settings::Library) == 0)
  {
    factories = fm->getFactories(item->text(0));
  }
  else if (item->parent()->parent() != NULL && item->parent()->parent()->text(0).compare(DREAM3D::Settings::Library) == 0)
  {
    factories = fm->getFactories(item->parent()->text(0), item->text(0));
  }

  // Nothing was in the search Feature so just reset to what was listed before
  if (text.isEmpty() == true)
  {
    updateFilterGroupList(factories);
    return;
  }

  // The user is typing something in the search box so lets search the filter class name and human label
  filterList->clear();
  for (FilterManager::Collection::iterator factory = factories.begin(); factory != factories.end(); ++factory)
  {
    IFilterFactory::Pointer wigFactory = factory.value();
    if (NULL == wigFactory.get() )
    {
      continue;
    }
    QString humanName = (wigFactory->getFilterHumanLabel());
    bool match = false;
    if (humanName.contains(text, Qt::CaseInsensitive) == true)
    {
      match = true;
    }
    QString filterName = (factory.key());
    if (filterName.contains(text, Qt::CaseInsensitive) == true)
    {
      match = true;
    }
    // Nothing matched the string
    if (false == match) { continue; }

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

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterLibraryDockWidget::on_filterList_currentItemChanged ( QListWidgetItem* item, QListWidgetItem* previous )
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterLibraryDockWidget::on_filterList_itemDoubleClicked( QListWidgetItem* item )
{
  emit filterItemDoubleClicked(item->data(Qt::UserRole).toString());
}
