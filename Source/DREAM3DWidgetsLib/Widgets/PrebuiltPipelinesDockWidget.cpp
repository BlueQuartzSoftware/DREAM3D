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

#include "PrebuiltPipelinesDockWidget.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

#include <QtGui/QMessageBox>
#include <QtGui/QDesktopServices>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QMenu>


#include "FilterListDockWidget.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrebuiltPipelinesDockWidget::PrebuiltPipelinesDockWidget(QWidget* parent) :
  QDockWidget(parent)
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrebuiltPipelinesDockWidget::~PrebuiltPipelinesDockWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrebuiltPipelinesDockWidget::connectFilterList(FilterListDockWidget* filterListWidget)
{
  connect(this, SIGNAL(filterListGenerated(const QStringList&, bool)),
          filterListWidget, SLOT(updateFilterList(const QStringList&, bool) ) );

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrebuiltPipelinesDockWidget::setupGui()
{
  // Clear out the default stuff
  filterLibraryTree->clear();

  readPipelines();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterLibraryTreeWidget* PrebuiltPipelinesDockWidget::getFilterLibraryTreeWidget()
{
  return filterLibraryTree;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDir PrebuiltPipelinesDockWidget::findPipelinesDirectory()
{
  QString dirName("PrebuiltPipelines");

  QString appPath = qApp->applicationDirPath();
  QDir pipelinesDir = QDir(appPath);
#if defined(Q_OS_WIN)
  QFileInfo fi(pipelinesDir.absolutePath() + QDir::separator() + dirName);
  if (fi.exists() == false)
  {
    // The help file does not exist at the default location because we are probably running from visual studio.
    // Try up one more directory
    pipelinesDir.cdUp();
  }
#elif defined(Q_OS_MAC)
  if (pipelinesDir.dirName() == "MacOS")
  {
    pipelinesDir.cdUp();
    pipelinesDir.cdUp();
    pipelinesDir.cdUp();
  }
#else
  // We are on Linux - I think
  QFileInfo fi(pipelinesDir.absolutePath() + QDir::separator() + dirName);
// qDebug() << fi.absolutePath();
  // Look for the "PrebuiltPipelines" directory in the current app directory
  if (fi.exists() == false)
  {
    // Try up one more directory
    pipelinesDir.cdUp();
  }

#endif

  pipelinesDir = pipelinesDir.absolutePath() + QDir::separator() + dirName;
  return pipelinesDir;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrebuiltPipelinesDockWidget::readPipelines()
{
  QDir pipelinesDir = findPipelinesDirectory();

  FilterLibraryTreeWidget::ItemType itemType = FilterLibraryTreeWidget::Leaf_Item_Type;
  QString iconFileName(":/text.png");
  bool allowEditing = false;
  QStringList fileExtension;
  fileExtension.append("*.txt");
  fileExtension.append("*.ini");
  // Need to add the path to the prebuilts directory to the root item since we may use this later on.
  filterLibraryTree->invisibleRootItem()->setData(0, Qt::UserRole, QVariant(pipelinesDir.absolutePath()));

  // Now block signals and load up all the pipelines in the folder
  filterLibraryTree->blockSignals(true);
  addPipelinesRecursively(pipelinesDir, filterLibraryTree->invisibleRootItem(), iconFileName, allowEditing, fileExtension, itemType);
  filterLibraryTree->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrebuiltPipelinesDockWidget::addPipelinesRecursively(QDir currentDir, QTreeWidgetItem* currentDirItem, QString iconFileName,
                                                          bool allowEditing, QStringList filters, FilterLibraryTreeWidget::ItemType itemType)
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
      // Add a tree widget item for this  Group
      //qDebug() << fi.absoluteFilePath();
      nextDirItem = new QTreeWidgetItem(currentDirItem, FilterLibraryTreeWidget::Node_Item_Type);
      nextDirItem->setText(0, fi.baseName());
      nextDirItem->setIcon(0, QIcon(":/folder_blue.png"));
      nextDirItem->setData(0, Qt::UserRole, QVariant(fi.absoluteFilePath() ) );
      addPipelinesRecursively( QDir( fi.absoluteFilePath() ), nextDirItem, iconFileName, allowEditing, filters, itemType );   // Recursive call
    }
  }


  QFileInfoList itemList = currentDir.entryInfoList(filters);
  foreach(QFileInfo itemInfo, itemList)
  {
    QString itemFilePath = itemInfo.absoluteFilePath();
    QSettings itemPref(itemFilePath, QSettings::IniFormat);
    itemPref.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
    QString itemName = itemPref.value("Name").toString();
    //itemPref.setValue("DREAM3D_Version", QString::fromStdString(DREAM3DLib::Version::Package()));
    itemPref.endGroup();
    //qDebug() << itemInfo.absoluteFilePath() << "\n";
    // Add tree widget for this Prebuilt Pipeline
    QTreeWidgetItem* itemWidget = new QTreeWidgetItem(currentDirItem, itemType);
    itemWidget->setText(0, itemName);
    itemWidget->setIcon(0, QIcon(iconFileName));
    itemWidget->setData(0, Qt::UserRole, QVariant(itemInfo.absoluteFilePath()));
    if(allowEditing == true)
    {
      itemWidget->setFlags(itemWidget->flags() | Qt::ItemIsEditable);
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrebuiltPipelinesDockWidget::on_filterLibraryTree_itemClicked( QTreeWidgetItem* item, int column )
{
  QString favoritePath = item->data(0, Qt::UserRole).toString();
  QStringList filterList = generateFilterListFromPipelineFile(favoritePath);
  emit filterListGenerated(filterList, false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList PrebuiltPipelinesDockWidget::generateFilterListFromPipelineFile(QString path)
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
void PrebuiltPipelinesDockWidget::on_filterLibraryTree_itemDoubleClicked( QTreeWidgetItem* item, int column )
{
  QString pipelinePath = item->data(0, Qt::UserRole).toString();
  if (item->type() == FilterLibraryTreeWidget::Node_Item_Type)
  {
    return; // The user double clicked a folder, so don't do anything
  }
  if (pipelinePath.isEmpty() == false)
  {
    emit pipelineFileActivated(pipelinePath, QSettings::IniFormat, false);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrebuiltPipelinesDockWidget::actionAppendPipeline_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();

  QString pipelinePath = item->data(0, Qt::UserRole).toString();
  if (pipelinePath.isEmpty() == false)
  {
    QFileInfo fi(pipelinePath);
    if (fi.exists() == false) { return; }
    emit pipelineFileActivated(fi.absoluteFilePath(), QSettings::IniFormat, true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrebuiltPipelinesDockWidget::actionShowInFileSystem_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();
  QString pipelinePath = item->data(0, Qt::UserRole).toString();

  QFileInfo pipelinePathInfo(pipelinePath);
  QString pipelinePathDir = pipelinePathInfo.path();

  QString s("file://");
#if defined(Q_OS_WIN)
  s = s + "/"; // Need the third slash on windows because file paths start with a drive letter
#elif defined(Q_OS_MAC)

#else
  // We are on Linux - I think

#endif
  s = s + pipelinePathDir;
  QDesktopServices::openUrl(s);
}


