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

#include "QPrebuiltPipelinesDockWidget.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>

#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>


#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "QFilterWidget.h"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPrebuiltPipelinesDockWidget::QPrebuiltPipelinesDockWidget(QWidget* parent) :
    QDockWidget(parent)
{
    setupUi(this);
    setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPrebuiltPipelinesDockWidget::~QPrebuiltPipelinesDockWidget()
{
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QPrebuiltPipelinesDockWidget::setupGui()
{
    // This is just here to have the code compile. This is a QListWidget where the filters that make up
  // a prebuilt pipeline are displayed.
    //filterList = new QListWidget(this);

    // Clear out the default stuff
    filterLibraryTree->clear();


#if 0
    QTreeWidgetItem* prebuiltTreeWidgetItem = new QTreeWidgetItem(filterLibraryTree, ItemType::Prebuilt_Category_Item_Type);
    prebuiltTreeWidgetItem->setText(0, DREAM3D::Settings::PrebuiltPipelines);
    prebuiltTreeWidgetItem->setIcon(0, QIcon(":/flag_blue_scroll.png"));
    prebuiltTreeWidgetItem->setExpanded(true);
#endif

    readPrebuiltPipelines(filterLibraryTree->invisibleRootItem());
    //
    filterList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(filterList, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(onFilterListCustomContextMenuRequested(const QPoint&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QPrebuiltPipelinesDockWidget::readPrebuiltPipelines(QTreeWidgetItem* prebuiltTreeWidgetItem)
{
    QString appPath = qApp->applicationDirPath();
    QDir prebuiltDir = QDir(appPath);
#if defined(Q_OS_WIN)

#elif defined(Q_OS_MAC)
    if (prebuiltDir.dirName() == "MacOS")
    {
        prebuiltDir.cdUp();
        prebuiltDir.cdUp();
        prebuiltDir.cdUp();
    }
#else
    // We are on Linux - I think
    prebuiltDir.cdUp();
#endif

#if defined(Q_OS_WIN)
    QFileInfo fi( prebuiltDir.absolutePath() + QDir::separator() + "PrebuiltPipelines");
    if (fi.exists() == false)
    {
        // The help file does not exist at the default location because we are probably running from visual studio.
        // Try up one more directory
        prebuiltDir.cdUp();
    }
#endif
    prebuiltDir = prebuiltDir.absolutePath() + QDir::separator() + "PrebuiltPipelines";

    // So Now we have the top level Directory for the Prebuilts
    addFiltersRecursively(prebuiltDir, prebuiltTreeWidgetItem);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QPrebuiltPipelinesDockWidget::addFiltersRecursively(QDir currentDir, QTreeWidgetItem* currentDirItem)
{
    QTreeWidgetItem* nextDirItem;

    // Get a list of all the directories
    QFileInfoList dirList = currentDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    if ( dirList.size() > 0 )
    {
        foreach(QFileInfo fi, dirList)
        {
            // At this point we have the first level of directories and we want to do 2 things:
            // 1.Create an entry in the tree widget with this name
            // 2.drop into the directory and look for all the .txt files and add entries for those items.
            //qDebug() << fi.absoluteFilePath() << "\n";
            // Add a tree widget item for this Prebuilt Group
            nextDirItem = new QTreeWidgetItem(currentDirItem);
            nextDirItem->setText(0, fi.baseName());
            addFiltersRecursively( QDir( fi.absoluteFilePath() ), nextDirItem );   // Recursive call
        }
    }

    QStringList filters;
    filters << "*.txt";
    QFileInfoList pblist = currentDir.entryInfoList(filters);
    foreach(QFileInfo pbinfo, pblist)
    {
        QString pbFilePath = pbinfo.absoluteFilePath();
        QSettings pbPref(pbFilePath, QSettings::IniFormat);
        pbPref.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
        QString pbName = pbPref.value("Name").toString();
        pbPref.endGroup();
        //qDebug() << pbinfo.absoluteFilePath() << "\n";
        // Add tree widget for this Prebuilt Pipeline
        QTreeWidgetItem* prebuiltItem = new QTreeWidgetItem(currentDirItem, ItemType::Prebuilt_Item_Type);
        prebuiltItem->setText(0, pbName);
        prebuiltItem->setIcon(0, QIcon(":/bullet_ball_blue.png"));
        prebuiltItem->setData(0, Qt::UserRole, QVariant(pbinfo.absoluteFilePath()));
    }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QPrebuiltPipelinesDockWidget::on_filterLibraryTree_itemClicked( QTreeWidgetItem* item, int column )
{
  //QString prebuiltName = item->text(0);
  QString prebuiltPath = item->data(0, Qt::UserRole).toString();
  QStringList filterList = generateFilterListFromPipelineFile(prebuiltPath);
  populateFilterList(filterList);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList QPrebuiltPipelinesDockWidget::generateFilterListFromPipelineFile(QString path)
{

    QStringList filterNames;
    QSettings prefs(path, QSettings::IniFormat);

    prefs.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
    bool ok = false;
    int filterCount = prefs.value("Number_Filters").toInt(&ok);
    prefs.endGroup();
    if (false == ok) {filterCount = 0;}
    for (int i = 0; i < filterCount; ++i)
    {
        QString gName = QString::number(i);
        prefs.beginGroup(gName);
        QString filterName = prefs.value("Filter_Name", "").toString();
        filterNames.push_back(filterName);
        prefs.endGroup();
    }
    return filterNames;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QPrebuiltPipelinesDockWidget::populateFilterList(QStringList filterNames)
{
    FilterManager::Pointer fm = FilterManager::Instance();

    // Clear all the current items from the list
    filterList->clear();
    filterList->setSortingEnabled(false);

    for(int i = 0; i < filterNames.size(); ++i)
    {
        QString filterName = filterNames[i];
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
void QPrebuiltPipelinesDockWidget::on_filterLibraryTree_itemChanged( QTreeWidgetItem* item, int column )
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QPrebuiltPipelinesDockWidget::on_filterLibraryTree_currentItemChanged(QTreeWidgetItem* item, QTreeWidgetItem* previous )
{
    on_filterLibraryTree_itemClicked(item, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QPrebuiltPipelinesDockWidget::on_filterLibraryTree_itemDoubleClicked( QTreeWidgetItem* item, int column )
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

