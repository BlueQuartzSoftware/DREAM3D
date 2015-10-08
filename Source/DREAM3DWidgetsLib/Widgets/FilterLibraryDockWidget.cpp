/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "FilterLibraryDockWidget.h"

#include <QtCore/QFileInfo>

#include <QtWidgets/QMainWindow>

#include "SIMPLib/Common/IFilterFactory.hpp"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/DocRequestManager.h"

#include "FilterListDockWidget.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_FilterLibraryDockWidget.cpp"


#define LIBRARY_NODE_TYPE 0
#define PLUGIN_NODE_TYPE 1
#define SUBGROUP_NODE_TYPE 2
#define FILTER_NODE_TYPE 3

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterLibraryDockWidget::FilterLibraryDockWidget(QWidget* parent) :
  QDockWidget(parent),
  m_ContextMenu(new QMenu(this)),
  m_Mapper(NULL)
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterLibraryDockWidget::~FilterLibraryDockWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryDockWidget::connectFilterList(FilterListDockWidget* filterListWidget)
{
  connect(this, SIGNAL(filterListGenerated(const QStringList&, bool)),
          filterListWidget, SLOT(updateFilterList(const QStringList&, bool) ) );

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryDockWidget::refreshFilterGroups()
{
  FilterManager* fm = FilterManager::Instance();
  QSet<QString> grpNames = fm->getGroupNames();

  QList<QString> grpNameSorted = grpNames.toList();
  qSort(grpNameSorted);

  // Clear out the default stuff
  bookmarksTreeView->clear();

#if 1
  QTreeWidgetItem* library = new QTreeWidgetItem(bookmarksTreeView);
  library->setText(0, DREAM3D::Settings::Library);
  library->setIcon(0, QIcon(":/Groups/cubes.png"));
  library->setData(0, Qt::UserRole, QVariant(LIBRARY_NODE_TYPE) );
#else
  QTreeWidgetItem* library = bookmarksTreeView->invisibleRootItem();
  library->setText(0, DREAM3D::Settings::Library);
  library->setIcon(0, QIcon(":/cubes.png"));
#endif
  for(QList<QString>::iterator iter = grpNameSorted.begin(); iter != grpNameSorted.end(); ++iter)
  {
    //   qDebug() << *iter << "\n";
    QString groupName = *iter;
    QString iconName(":/Groups/");
    iconName.append(groupName);
    iconName.append("_Icon.png");
    // Validate the icon is in the resource system
    QFileInfo iconInfo(iconName);
    if (iconInfo.exists() == false)
    {
      iconName = ":/Groups/Plugin_Icon.png"; // Switch to our generic icon for Plugins that do not provide their own
    }
    QIcon icon(iconName);
    QTreeWidgetItem* filterGroup = new QTreeWidgetItem(library);
    filterGroup->setText(0, groupName);
    filterGroup->setIcon(0, icon);
    filterGroup->setData(0, Qt::UserRole, QVariant(PLUGIN_NODE_TYPE));
    QSet<QString> subGroupNames = fm->getSubGroupNames(groupName);
    for(QSet<QString>::iterator iter2 = subGroupNames.begin(); iter2 != subGroupNames.end(); ++iter2)
    {
      QTreeWidgetItem* filterSubGroup = new QTreeWidgetItem(filterGroup);
      QString subGroupName = *iter2;
      filterSubGroup->setText(0, subGroupName);
      filterSubGroup->setIcon(0, QIcon(":/folder_blue.png"));
      filterSubGroup->setData(0, Qt::UserRole, QVariant(SUBGROUP_NODE_TYPE));

      // Now find all the filters that are in this Group/SubGroup
      FilterManager::Collection filters = fm->getFactories(groupName, subGroupName);
      FilterManager::CollectionIterator iter(filters);
      while(iter.hasNext())
      {
        iter.next();
        IFilterFactory::Pointer factory = iter.value();
        AbstractFilter::Pointer filter = factory->create();
        QTreeWidgetItem* filterTreeItem = new QTreeWidgetItem(filterSubGroup);
        filterTreeItem->setText(0, filter->getHumanLabel());
        if(groupName.compare(DREAM3D::FilterGroups::Unsupported) == 0)
        {
          filterTreeItem->setIcon(0, QIcon(":/cube_red.png"));
        }
        else
        {
          filterTreeItem->setIcon(0, QIcon(":/cube_gray.png"));
        }
        filterTreeItem->setData(0, Qt::UserRole, QVariant(FILTER_NODE_TYPE));
        filterTreeItem->setData(0, Qt::UserRole + 1, QVariant(filter->getNameOfClass()));
        filterTreeItem->setToolTip(0, filter->generateHtmlSummary());
      }
    }
  }
  library->setExpanded(true);
  on_bookmarksTreeView_currentItemChanged(library, NULL);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryDockWidget::setupGui()
{
  refreshFilterGroups();
  QString css(" QToolTip {\
              border: 2px solid #434343;\
              padding: 2px;\
              border-radius: 3px;\
              opacity: 255;\
              background-color: #FFFFFF;\
              }");
  bookmarksTreeView->setStyleSheet(css);

  bookmarksTreeView->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(bookmarksTreeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenuForWidget(const QPoint&)));
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryDockWidget::on_bookmarksTreeView_itemClicked( QTreeWidgetItem* item, int column )
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryDockWidget::on_bookmarksTreeView_itemChanged( QTreeWidgetItem* item, int column )
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryDockWidget::showContextMenuForWidget(const QPoint& pos)
{
  QTreeWidgetItem* item = bookmarksTreeView->itemAt(pos);

  if (NULL != item && item->childCount() == 0)
  {
    // Clear the existing context menu
    m_ContextMenu->clear();

    QString itemName = item->text(0);

    m_Mapper = new QSignalMapper(this);

    QAction* actionLaunchHelp = new QAction(m_ContextMenu);
    actionLaunchHelp->setObjectName(QString::fromUtf8("actionLaunchHelp"));
    actionLaunchHelp->setText(QApplication::translate("DREAM3D_UI", "Filter Help", 0));
    connect(actionLaunchHelp, SIGNAL(triggered()),
            m_Mapper, SLOT(map()));
    m_Mapper->setMapping(actionLaunchHelp, itemName);
    connect(m_Mapper, SIGNAL(mapped(QString)),
            this, SLOT(launchHelpForItem(QString)));

    m_ContextMenu->addAction(actionLaunchHelp);
    m_ContextMenu->exec(QCursor::pos());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryDockWidget::launchHelpForItem(QString humanLabel)
{
  FilterManager* fm = FilterManager::Instance();
  if (NULL == fm)
  {
    return;
  }
  IFilterFactory::Pointer factory = fm->getFactoryForFilterHumanName(humanLabel);
  if (NULL == factory.get())
  {
    return;
  }
  AbstractFilter::Pointer filter = factory->create();
  if (NULL == filter.get())
  {
    return;
  }
  QString className = filter->getNameOfClass();

  DocRequestManager* docRequester = DocRequestManager::Instance();
  docRequester->requestFilterDocs(className);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryDockWidget::on_bookmarksTreeView_currentItemChanged(QTreeWidgetItem* item, QTreeWidgetItem* previous )
{
  // Get the PipelineFilterWidget Manager Instance
  FilterManager* fm = FilterManager::Instance();
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
    on_bookmarksTreeView_itemClicked(item, 0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryDockWidget::on_bookmarksTreeView_itemDoubleClicked( QTreeWidgetItem* item, int column )
{
  QVariant nodeType = item->data(0, Qt::UserRole);
  if(nodeType.toInt() == FILTER_NODE_TYPE)
  {
    QString filterClassName = item->data(0, Qt::UserRole + 1).toString();
    emit filterItemDoubleClicked(filterClassName);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryDockWidget::updateFilterGroupList(FilterManager::Collection& factories)
{
  QStringList filterNames;
  for (FilterManager::Collection::iterator factory = factories.begin(); factory != factories.end(); ++factory)
  {
    filterNames << factory.key();
  }
  emit filterListGenerated(filterNames, true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryDockWidget::writeSettings(DREAM3DSettings& prefs)
{
  prefs.setValue(objectName(), isHidden());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterLibraryDockWidget::readSettings(QMainWindow* main, DREAM3DSettings& prefs)
{
  main->restoreDockWidget(this);

  bool b = prefs.value(objectName(), false).toBool();
  setHidden(b);
}
