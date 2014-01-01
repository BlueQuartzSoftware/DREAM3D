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

#include "QDream3DDocumentsDockWidget.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QUrl>

#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QMessageBox>
#include <QtGui/QDesktopServices>
#include <QtGui/QTreeWidgetItem>

#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/FilterParameters/QFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/QFilterParametersWriter.h"


#include "AddFavoriteWidget.h"

#include "QFilterListDockWidget.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDream3DDocumentsDockWidget::QDream3DDocumentsDockWidget(QWidget* parent) :
  QDockWidget(parent)
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDream3DDocumentsDockWidget::~QDream3DDocumentsDockWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDream3DDocumentsDockWidget::connectFilterList(QFilterListDockWidget* filterListWidget)
{
  connect(this, SIGNAL(filterListGenerated(const QStringList&)),
          filterListWidget, SLOT(updateFilterList(const QStringList&) ) );

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDream3DDocumentsDockWidget::setupGui()
{
  // This is just here to have the code compile. This is a QListWidget where the filters that make up
  // a prebuilt pipeline are displayed.
  //filterList = new QListWidget(this);

  // Clear out the default stuff
  filterLibraryTree->clear();


#if 1
  QTreeWidgetItem* m_favorites = new QTreeWidgetItem(filterLibraryTree, Favorite_Category_Item_Type);
  m_favorites->setText(0, DREAM3D::Settings::FavoritePipelines);
  m_favorites->setIcon(0, QIcon(":/flash.png"));

  m_favorites->setExpanded(true);
#endif
  filterLibraryTree->blockSignals(true);
  readFavoritePipelines(m_favorites);
  filterLibraryTree->blockSignals(false);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDream3DDocumentsDockWidget::readFavoritePipelines(QTreeWidgetItem* m_favorites)
{
  QString prefFile;
  {
#if defined (Q_OS_MAC)
    QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
    QString extension = ".ini";
#else
    QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
    QString extension = ".ini";
#endif
    prefFile = prefs.fileName();
  }

  QFileInfo prefFileInfo = QFileInfo(prefFile);
  QString parentPath = prefFileInfo.path();
  QString favPath = parentPath + QDir::separator() + "DREAM3D_Favorites";
  QDir favDir = QDir(favPath);

  QStringList filters;
  filters << "*.ini";
  QFileInfoList dirList = favDir.entryInfoList(filters);
  foreach(QFileInfo fi, dirList)
  {
    QString favFilePath = fi.absoluteFilePath();
    QSettings favPref(favFilePath, QSettings::IniFormat);
    QString favName;
    {
      favPref.beginGroup(DREAM3D::Settings::FavoriteConfig);
      favName = favPref.value("Name").toString();
      favPref.endGroup();
      // This next code section is here to move from the old "favorite_config" to the newer DREAM3D::Settings::PipelineBuilderGroup ini group
      if (favName.isEmpty() == false)
      {
        favPref.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
        favPref.setValue("Name", favName);
        favPref.endGroup();
        favPref.remove("favorite_config"); // Now that we transfered the value, remove the old value
      }
    }

    {
      favPref.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
      favName = favPref.value("Name").toString();
      favPref.endGroup();
    }

    // Add a tree widget item for this favorite
    QTreeWidgetItem* favoriteItem = new QTreeWidgetItem(m_favorites, Favorite_Item_Type);
    favoriteItem->setText(0, favName);
    favoriteItem->setIcon(0, QIcon(":/bullet_ball_yellow.png"));
    favoriteItem->setData(0, Qt::UserRole, QVariant(favFilePath));
    favoriteItem->setFlags(favoriteItem->flags() | Qt::ItemIsEditable);
    //   m_favoritesMap[favoriteItem] = favFilePath;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDream3DDocumentsDockWidget::on_filterLibraryTree_itemClicked( QTreeWidgetItem* item, int column )
{
  QString favoritePath = item->data(0, Qt::UserRole).toString();
  QStringList filterList = generateFilterListFromPipelineFile(favoritePath);
    emit filterListGenerated(filterList);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList QDream3DDocumentsDockWidget::generateFilterListFromPipelineFile(QString path)
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
void QDream3DDocumentsDockWidget::on_filterLibraryTree_itemChanged( QTreeWidgetItem* item, int column )
{
  if (NULL != item->parent() )
  {
    QString favoritePath = item->data(0, Qt::UserRole).toString();
    QString newFavoriteTitle = item->text(0);

    // Check favorite title for illegal characters and duplicate favorite names
    if ( checkFavoriteTitle(favoritePath, newFavoriteTitle, item) )
    {
      return;
    }

    // Create the new file path and write to it
    QString newPath = writeNewFavoriteFilePath(item->text(0), favoritePath, item);

    // Set Name in preferences group
    QSettings newFavoritePrefs(newPath, QSettings::IniFormat);
    newFavoritePrefs.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
    newFavoritePrefs.setValue("Name", item->text(0) );
    newFavoritePrefs.endGroup();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QDream3DDocumentsDockWidget::checkFavoriteTitle(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item)
{
  // Put all children (favorites) in a list
  QList<QTreeWidgetItem*> favoritesList;
  int numOfChildren = item->parent()->childCount();
  for (int i = 0; i < numOfChildren; i++)
  {
    favoritesList.append( item->parent()->child(i) );
  }

  // Check for illegal characters and duplicate favorite names
  if ( hasIllegalFavoriteName(favoritePath, newFavoriteTitle, item) ||
       hasDuplicateFavorites(favoritesList, favoritePath, newFavoriteTitle, item) )
  {
    return true;
  }
  else
  {
    return false;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QDream3DDocumentsDockWidget::hasIllegalFavoriteName(QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item)
{
  QSettings favoritePrefs(favoritePath, QSettings::IniFormat);
  QString displayText = "";

  if ( newFavoriteTitle.contains(QRegExp("[^a-zA-Z_-\\d]")) )
  {
    displayText = "The title that was chosen has illegal characters.\n\nNames can only have:\n\tLetters\n\tNumbers\n\tUnderscores\n\tDashes";
    displayText = displayText + "\n\nNo spaces allowed";

    // Change the GUI back to the old name
    favoritePrefs.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
    filterLibraryTree->blockSignals(true);
    item->setText(0, favoritePrefs.value("Name").toString() );
    filterLibraryTree->blockSignals(false);
    favoritePrefs.endGroup();

    // Display error message
    QMessageBox::critical(this, tr("Rename Favorite"), tr(displayText.toLatin1().data()),
                          QMessageBox::Ok, QMessageBox::Ok);

    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QDream3DDocumentsDockWidget::hasDuplicateFavorites(QList<QTreeWidgetItem*> favoritesList, QString favoritePath, QString newFavoriteTitle, QTreeWidgetItem* item)
{
  QSettings favoritePrefs(favoritePath, QSettings::IniFormat);
  QString displayText = "";

  for (int i = 0; i < favoritesList.size(); i++)
  {
    QSettings currentItemPrefs(favoritesList[i]->data(0, Qt::UserRole).toString(), QSettings::IniFormat);
    currentItemPrefs.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);

    // If the new title matches one of the other favorite titles
    if (currentItemPrefs.value("Name").toString() == newFavoriteTitle)
    {
      displayText = "A favorite that has this title already exists in the Favorites list.\n\n";

      // Change the GUI back to the old name
      favoritePrefs.beginGroup(DREAM3D::Settings::PipelineBuilderGroup);
      filterLibraryTree->blockSignals(true);
      item->setText(0, favoritePrefs.value("Name").toString() );
      filterLibraryTree->blockSignals(false);
      favoritePrefs.endGroup();

      // Display error message
      QMessageBox::critical(this, tr("Rename Favorite"), tr(displayText.toLatin1().data()),
                            QMessageBox::Ok, QMessageBox::Ok);

      return true;
    }
  }

  return false;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QDream3DDocumentsDockWidget::writeNewFavoriteFilePath(QString newFavoriteTitle, QString favoritePath, QTreeWidgetItem* item)
{
  QFileInfo original(favoritePath);
  QString newPath = original.canonicalPath() + QDir::separator() + newFavoriteTitle;
  if (!original.completeSuffix().isEmpty())
  { newPath += "." + original.completeSuffix(); }

  newPath = QDir::toNativeSeparators(newPath);

  QFile f(favoritePath);
  bool success = f.rename(newPath);
  if (false == success)
  {
    qDebug() << "Failed";
  }

  filterLibraryTree->blockSignals(true);
  item->setData(0, Qt::UserRole, QVariant(newPath));
  filterLibraryTree->blockSignals(false);

  return newPath;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDream3DDocumentsDockWidget::on_filterLibraryTree_currentItemChanged(QTreeWidgetItem* item, QTreeWidgetItem* previous )
{
  on_filterLibraryTree_itemClicked(item, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDream3DDocumentsDockWidget::on_filterLibraryTree_itemDoubleClicked( QTreeWidgetItem* item, int column )
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

  QString pipelinePath = item->data(0, Qt::UserRole).toString();
  if (pipelinePath.isEmpty() == false)
  {
    emit pipelineFileActivated(pipelinePath);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDream3DDocumentsDockWidget::addFavorite(QString favoriteTitle)
{
  //Remove all spaces and illegal characters from favorite name
  favoriteTitle = favoriteTitle.trimmed();
  favoriteTitle = favoriteTitle.remove(" ");
  favoriteTitle = favoriteTitle.remove(QRegExp("[^a-zA-Z_\\d\\s]"));


#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
  QString extension = ".ini";
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
  QString extension = ".ini";
#endif

  QString prefFile = prefs.fileName();
  QFileInfo prefFileInfo = QFileInfo(prefFile);
  QString parentPath = prefFileInfo.path();
  QDir parentPathDir = QDir(parentPath);

  if(parentPathDir.mkpath(parentPath))
  {
    QString newParentPrefPath = parentPath + "/DREAM3D_Favorites";
    QString newPrefPath = newParentPrefPath + "/" + favoriteTitle + extension;

    newPrefPath = QDir::toNativeSeparators(newPrefPath);
    newParentPrefPath = QDir::toNativeSeparators(newParentPrefPath);

    QDir newParentPrefPathDir = QDir(newParentPrefPath);

    if(newParentPrefPathDir.mkpath(newParentPrefPath))
    {
#ifdef __APPLE__
#warning PipelineViewWidget needs to save the actual pipeline
#endif
      //m_PipelineViewWidget->savePipeline(newPrefPath, favoriteTitle);
    }
  }

  QTreeWidgetItem* m_favorites = filterLibraryTree->invisibleRootItem();
  QList<QTreeWidgetItem*> items = m_favorites->takeChildren();
  int count = items.count();
  for(int i = 0; i < count; ++i)
  {
    delete items.at(i);
  }
  filterLibraryTree->blockSignals(true);
  readFavoritePipelines(m_favorites);
  filterLibraryTree->blockSignals(false);
  // Tell everyone to save their preferences NOW instead of waiting until the app quits
  emit fireWriteSettings();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDream3DDocumentsDockWidget::actionAddFavorite_triggered()
{
  AddFavoriteWidget* addfavoriteDialog = new AddFavoriteWidget(this);
  addfavoriteDialog->exec();

  if(addfavoriteDialog->getBtnClicked())
  {

    QString favoriteTitle = addfavoriteDialog->getFavoriteName();
    addFavorite(favoriteTitle);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDream3DDocumentsDockWidget::actionUpdateFavorite_triggered()
{
  // Lets get the name of the favorite
  QTreeWidgetItem* item = filterLibraryTree->currentItem();
  QTreeWidgetItem* parent = filterLibraryTree->currentItem()->parent();

  QString filePath = item->data(0, Qt::UserRole).toString();
  QFileInfo filePathInfo = QFileInfo(filePath);
  // QString fileParentPath = filePathInfo.path();
  QString fileName = filePathInfo.baseName();


  if (NULL != parent && parent->text(0).compare(DREAM3D::Settings::FavoritePipelines) == 0 )
  {
    removeFavorite(item);
    item = NULL;
    addFavorite(fileName);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDream3DDocumentsDockWidget::removeFavorite(QTreeWidgetItem* item)
{
  QString filePath = item->data(0, Qt::UserRole).toString();
  QFileInfo filePathInfo = QFileInfo(filePath);
  QString fileParentPath = filePathInfo.path();
  QString fileName = filePathInfo.fileName();
  QDir fileParentPathDir = QDir(fileParentPath);

  //Remove favorite's pref file
  fileParentPathDir.remove(fileName);

  //Remove favorite, graphically, from the DREAM3D interface
  filterLibraryTree->removeItemWidget(item, 0);
  delete item;

  // Write these changes out to the preferences file
  emit fireWriteSettings();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDream3DDocumentsDockWidget::actionRemoveFavorite_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();
  QTreeWidgetItem* parent = filterLibraryTree->currentItem()->parent();

  QMessageBox msgBox;
  msgBox.setText("Are you sure that you want to remove \"" + item->text(0) + "\"?");
  msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
  msgBox.setDefaultButton(QMessageBox::Yes);
  int ret = msgBox.exec();

  if (NULL != parent && parent->text(0).compare(DREAM3D::Settings::FavoritePipelines) == 0 && ret == QMessageBox::Yes)
  {
    removeFavorite(item);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDream3DDocumentsDockWidget::actionRenameFavorite_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();

  if (NULL != item->parent() && item->parent()->text(0).compare(DREAM3D::Settings::FavoritePipelines) == 0)
  {
    filterLibraryTree->editItem(item, 0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDream3DDocumentsDockWidget::actionAppendFavorite_triggered()
{
  QTreeWidgetItem* item = filterLibraryTree->currentItem();

  QString pipelinePath = item->data(0, Qt::UserRole).toString();
  if (pipelinePath.isEmpty() == false)
  {
    QFileInfo fi(pipelinePath);
    if (fi.exists() == false) { return; }
    FilterPipeline::Pointer pipeline = QFilterParametersReader::ReadPipelineFromFile(pipelinePath, QSettings::IniFormat);
#if __APPLE__
#warning  QDream3DDocumentsDockWidget::actionAppendFavorite_triggered  PipelineViewWidget needs to load the pipeline
#endif
    //m_PipelineViewWidget->loadPipeline(pipeline, true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDream3DDocumentsDockWidget::actionShowInFileSystem_triggered()
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
