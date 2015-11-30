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


#include "PrebuiltPipelinesDockWidget.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

#include <QtWidgets/QMessageBox>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QMenu>

#include "Applications/DREAM3D/DREAM3DApplication.h"

#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/QFilterParametersReader.h"

#include "FilterListDockWidget.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_PrebuiltPipelinesDockWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrebuiltPipelinesDockWidget::PrebuiltPipelinesDockWidget(QWidget* parent) :
  QDockWidget(parent)
{
  setupUi(this);
  setupGui();

  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), dream3dApp, SLOT(on_prebuiltsDockContextMenuRequested(const QPoint&)));
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
  prebuiltsLibraryTree->clear();

  readPipelines();

  QString css(" QToolTip {\
              border: 2px solid #434343;\
              padding: 2px;\
              border-radius: 3px;\
              opacity: 255;\
              background-color: #FFFFFF;\
              }");
  prebuiltsLibraryTree->setStyleSheet(css);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterLibraryTreeWidget* PrebuiltPipelinesDockWidget::getFilterLibraryTreeWidget()
{
  return prebuiltsLibraryTree;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDir PrebuiltPipelinesDockWidget::findPipelinesDirectory()
{
  QString dirName("PrebuiltPipelines");

  QString appPath = dream3dApp->applicationDirPath();
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
  fileExtension.append("*.json");
  // Need to add the path to the prebuilts directory to the root item since we may use this later on.
  prebuiltsLibraryTree->invisibleRootItem()->setData(0, Qt::UserRole, QVariant(pipelinesDir.absolutePath()));

  // Now block signals and load up all the pipelines in the folder
  prebuiltsLibraryTree->blockSignals(true);
  addPipelinesRecursively(pipelinesDir, prebuiltsLibraryTree->invisibleRootItem(), iconFileName, allowEditing, fileExtension, itemType);
  prebuiltsLibraryTree->blockSignals(false);
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
    QString itemName;
    if (itemInfo.suffix().compare("ini") == 0 || itemInfo.suffix().compare("txt") == 0)
    {
      QSettings itemPref(itemFilePath, QSettings::IniFormat);
      itemPref.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
      itemName = itemPref.value(DREAM3D::Settings::PipelineName).toString();
      itemPref.endGroup();
    }
    else if (itemInfo.suffix().compare("json") == 0)
    {
      QString dVers;
      JsonFilterParametersReader::ReadNameOfPipelineFromFile(itemFilePath, itemName, dVers, NULL);
    }
    // Add tree widget for this Prebuilt Pipeline
    QTreeWidgetItem* itemWidget = new QTreeWidgetItem(currentDirItem, itemType);
    itemWidget->setText(0, itemName);
    itemWidget->setIcon(0, QIcon(iconFileName));
    itemWidget->setData(0, Qt::UserRole, itemName);
    itemWidget->setData(1, Qt::UserRole, QVariant(itemInfo.absoluteFilePath()));
    if(allowEditing == true)
    {
      itemWidget->setFlags(itemWidget->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);
    }
    QString htmlFormattedString = generateHtmlFilterListFromPipelineFile(itemInfo.absoluteFilePath());
    itemWidget->setToolTip(0, htmlFormattedString);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrebuiltPipelinesDockWidget::on_prebuiltsLibraryTree_itemClicked( QTreeWidgetItem* item, int column )
{
#if 0
  QString favoritePath = item->data(1, Qt::UserRole).toString();
  QStringList filterList = generateFilterListFromPipelineFile(favoritePath);
  if(filterList.size() > 0)
  {
    emit filterListGenerated(filterList, false);
  }
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrebuiltPipelinesDockWidget::on_prebuiltsLibraryTree_itemDoubleClicked( QTreeWidgetItem* item, int column )
{
  QString pipelinePath = item->data(1, Qt::UserRole).toString();
  if (item->type() == FilterLibraryTreeWidget::Node_Item_Type)
  {
    return; // The user double clicked a folder, so don't do anything
  }
  if (pipelinePath.isEmpty() == false)
  {
    emit pipelineFileActivated(pipelinePath, false, false);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrebuiltPipelinesDockWidget::on_prebuiltsLibraryTree_currentItemChanged(QTreeWidgetItem* item, QTreeWidgetItem* previous )
{
//  on_prebuiltsLibraryTree_itemClicked(item, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PrebuiltPipelinesDockWidget::generateHtmlFilterListFromPipelineFile(QString path)
{
  int filterCount = 0;
  QString name;
  QString dVers;
  FilterPipeline::Pointer pipeline;
  
  QString html;
  QTextStream ss(&html);
  ss << "<html><head></head>\n";
  ss << "<body>\n";

  QFileInfo fi(path);
  if(fi.suffix().compare("ini") == 0)
  {
    QSettings prefs(path, QSettings::IniFormat);
    prefs.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
    bool ok = false;
    name = prefs.value("Name").toString();
    dVers = prefs.value("DREAM3D_Version").toString();
    if(dVers.isEmpty() == true)
    {
      dVers = prefs.value("Version").toString();
    }
    prefs.endGroup();
    if (false == ok) {filterCount = 0;}

    pipeline = QFilterParametersReader::ReadPipelineFromFile(path, QSettings::IniFormat, NULL);
    filterCount = pipeline->getFilterContainer().size();
  }
  else if (fi.suffix().compare("json") == 0)
  {
    pipeline = JsonFilterParametersReader::ReadPipelineFromFile(path, NULL);
    if (pipeline.get() == NULL)
    {
      ss << "<b>Unkonwn Pipeline File format for file " << path << "</b>";
      ss << "</tbody></table>\n";
      ss << "</body></html>";
      return html;
    }
    JsonFilterParametersReader::ReadNameOfPipelineFromFile(path, name, dVers, NULL);
    filterCount = pipeline->getFilterContainer().size();
  }

  // A table for the summary items
  ss << "<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\">\n";
  ss << "<tbody>\n";
  ss << "	<tr><th align=\"right\">Pipeline Name:</th><td>" << name << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">Filter Count:</th><td>" << filterCount << "</td></tr>\n";
  ss << "	<tr><th align=\"right\">DREAM3D Version:</th><td>" << dVers << "</td></tr>\n";
  ss << "</tbody>\n";
  ss << "</table>\n";
  ss << "<p></p>\n";
  if(NULL == pipeline.get())
  {
    ss << "<b>Unkonwn Pipeline File format for file " << path << "</b>";
    ss << "</tbody></table>\n";
    ss << "</body></html>";
    return html;
  }
  // Start the table of the Pipeline
  ss << "<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\" width=\"300px\">\n";
  ss << "<tbody>\n";
  ss << "<tr bgcolor=\"#A2E99C\"><th>Index</th><th>Filter Group</th><th>Filter Name</th></tr>\n";

//  FilterManager* filtManager = FilterManager::Instance();
  char rowColor = 0;
  QString red("#FFAAAA");
  QString odd("#FFFFFF");
  QString even("#B0E4FF");
  QString color = odd;
  bool unknownFilters = false;

  FilterPipeline::FilterContainerType filters = pipeline->getFilterContainer();
  filterCount = filters.size();
  for (int i = 0; i < filterCount; ++i)
  {
    if (rowColor == 0) { rowColor = 1; color = odd; }
    else { rowColor = 0; color = even; }

    AbstractFilter::Pointer filter = filters.at(i);

    if(NULL != filter.get())
    {
      ss << "<tr bgcolor=\"" << color << "\"><td>" << i << "</td><td>" << filter->getGroupName() << "</td><td>" << filter->getHumanLabel() << "</td></tr>\n";
    }
  }


  if(unknownFilters)
  {
    color = red;
    ss << "<tr bgcolor=\"" << color << "\"><th colspan=\"3\">There are filters in the pipeline that the currently running version of DREAM3D does not know about. This ";
    ss << "can happen if you are missing plugins that contain the filters or if the pipeline was created in a prior version ";
    ss << "of DREAM3D in which case those filters may have been renamed. Please consult the DREAM3D documentation for more details ";
    ss << "or ask the individual who gave you the pipeline file for more details.</th></tr>\n";
  }
  ss << "</tbody></table>\n";

  ss << "</body></html>";

  return html;
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
void PrebuiltPipelinesDockWidget::readSettings(QMainWindow* main, DREAM3DSettings& prefs)
{
  main->restoreDockWidget(this);

  bool b = prefs.value(objectName(), false).toBool();
  setHidden(b);

  QByteArray headerState = prefs.value("PrebuiltsHeaderState", QByteArray()).toByteArray();
  prebuiltsLibraryTree->header()->restoreState(headerState);

  prefs.beginGroup("Prebuilts Expansion Metadata");
  QTreeWidgetItemIterator iter(prebuiltsLibraryTree);

  while (*iter)
  {
    QTreeWidgetItem* item = *iter;
    if (item->type() == FilterLibraryTreeWidget::Node_Item_Type)
    {
      item->setExpanded(prefs.value(item->text(0), false).toBool());
    }

    ++iter;
  }
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrebuiltPipelinesDockWidget::writeSettings(DREAM3DSettings& prefs)
{
  prefs.setValue(objectName(), isHidden());

  prefs.setValue("PrebuiltsHeaderState", prebuiltsLibraryTree->header()->saveState());

  prefs.beginGroup("Prebuilts Expansion Metadata");
  QTreeWidgetItemIterator iter(prebuiltsLibraryTree);

  while (*iter)
  {
    QTreeWidgetItem* item = *iter;
    if (item->type() == FilterLibraryTreeWidget::Node_Item_Type)
    {
      prefs.setValue(item->text(0), item->isExpanded());
    }

    ++iter;
  }
  prefs.endGroup();
}


