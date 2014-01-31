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

#include "DREAM3DWidgetsLib/Widgets/moc_FilterListDockWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterListDockWidget::FilterListDockWidget(QWidget* parent) :
  QDockWidget(parent)
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

  //
  filterList->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(filterList, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(onFilterListCustomContextMenuRequested(const QPoint&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::updateFilterList(const QStringList& list)
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::on_filterSearch_textChanged (const QString& text)
{

#if 0
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
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::on_filterList_currentItemChanged ( QListWidgetItem* item, QListWidgetItem* previous )
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterListDockWidget::on_filterList_itemDoubleClicked( QListWidgetItem* item )
{
  emit filterItemDoubleClicked(item->data(Qt::UserRole).toString());
}
