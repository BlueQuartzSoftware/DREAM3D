/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "PipelineBuilderWidget.h"

#include <string>
#include <set>

//-- Qt Includes
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QThread>
#include <QtCore/QFileInfoList>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QResource>
#include <QtGui/QFileDialog>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QColor>
#include <QtGui/QBrush>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DREAM3DFilters.h"

#include "QFilterWidget.h"
#include "AddFavoriteWidget.h"

namespace Detail
{
  const QString Library("Filter Library");
  const QString PrebuiltPipelines("Prebuilt Pipelines");
  const QString FavoritePipelines("Favorite Pipelines");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineBuilderWidget::PipelineBuilderWidget(QWidget *parent) :
DREAM3DPluginFrame(parent),
m_FilterPipeline(NULL),
m_WorkerThread(NULL),
m_DocErrorTabsIsOpen(false),
#if defined(Q_WS_WIN)
m_OpenDialogLastDirectory("C:\\")
#else
m_OpenDialogLastDirectory("~/")
#endif
{
  setupUi(this);
  setupGui();
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineBuilderWidget::~PipelineBuilderWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::readSettings(QSettings &prefs)
{
  prefs.beginGroup("PipelineBuilder");

//  bool ok = false;
  splitter_1->restoreState(prefs.value("splitter_1").toByteArray());
  splitter_2->restoreState(prefs.value("splitter_2").toByteArray());
  prefs.endGroup();

  readSettings(prefs, m_PipelineViewWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::readSettings(QSettings &prefs, PipelineViewWidget* viewWidget)
{
  // Clear Any Existing Pipeline
  m_PipelineViewWidget->clearWidgets();

  prefs.beginGroup("PipelineBuilder");

  bool ok = false;
  int filterCount = prefs.value("Number_Filters").toInt(&ok);
  prefs.endGroup();

  if (false == ok) {filterCount = 0;}
  for (int i = 0; i < filterCount; ++i)
  {
    QString gName = QString::number(i);

    prefs.beginGroup(gName);

    QString filterName = prefs.value("Filter_Name", "").toString();

    QFilterWidget* w = viewWidget->addFilter(filterName); // This will set the variable m_SelectedFilterWidget
    if(w) {
      w->blockSignals(true);
      w->readOptions(prefs);
      w->blockSignals(false);
      w->emitParametersChanged();
    }
    prefs.endGroup();
  }
  m_PipelineViewWidget->preflightPipeline();


  //Get Favorites from Pref File and Update Tree Widget
  prefs.beginGroup(Detail::FavoritePipelines);

  int favoriteCount = prefs.value("count").toInt(&ok);
  if (false == ok) {
    favoriteCount = 0;
  }

  for(int r = 0; r < favoriteCount; ++r)
  {
    QString favNameKey = QString::number(r) + QString("_Favorite_Name");
    QString favName = prefs.value(favNameKey).toString();

    QString favFilePath = QString::number(r) + QString("_Favorite_File");
    QString favPath = prefs.value(favFilePath).toString();

    m_favoritesMap[favName] = favPath;

    QTreeWidgetItem* favoriteItem = new QTreeWidgetItem(m_favorites);
    favoriteItem->setText(0, favName);
    favoriteItem->setIcon(0, QIcon(":/bullet_ball_yellow.png"));
  }

  prefs.endGroup();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::writeSettings(QSettings &prefs)
{

  prefs.setValue("splitter_1", splitter_1->saveState());
  prefs.setValue("splitter_2", splitter_2->saveState());



  prefs.beginGroup(Detail::FavoritePipelines);
  prefs.clear();
  prefs.setValue( "count", m_favoritesMap.size() );

  int index = 0;
  QMapIterator<QString, QString> i(m_favoritesMap);
  while (i.hasNext()) {
    i.next();
    prefs.setValue( QString::number(index) + "_Favorite_Name", i.key() );
    prefs.setValue( QString::number(index) + "_Favorite_File", i.value() );
    index++;
  }
  prefs.endGroup();

  writeSettings(prefs, m_PipelineViewWidget);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::writeSettings(QSettings &prefs, PipelineViewWidget* viewWidget)
{
  prefs.beginGroup("PipelineBuilder");
  qint32 count = m_PipelineViewWidget->filterCount();
  prefs.setValue("Number_Filters", count);
  prefs.endGroup();

  for(qint32 i = 0; i < count; ++i)
  {
    QFilterWidget* fw = viewWidget->filterWidgetAt(i);
    if (fw)
    {
      QString groupName = QString::number(i);
      prefs.beginGroup(groupName);
      fw->writeOptions(prefs);
      prefs.endGroup();
    }
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::setupGui()
{
  m_hasErrors = false;
  m_hasWarnings = false;

  // Get the QFilterWidget Manager Instance
  FilterWidgetManager::Pointer fm = FilterWidgetManager::Instance();

  std::set<std::string> groupNames = fm->getGroupNames();

  QTreeWidgetItem* library = new QTreeWidgetItem(filterLibraryTree);
  library->setText(0, Detail::Library);
  library->setIcon(0, QIcon(":/cubes.png"));

  QTreeWidgetItem* presets = new QTreeWidgetItem(filterLibraryTree);
  presets->setText(0, Detail::PrebuiltPipelines);
  presets->setIcon(0, QIcon(":/flag_blue_scroll.png"));
  presets->setExpanded(true);


  m_favorites = new QTreeWidgetItem(filterLibraryTree);
  m_favorites->setText(0, Detail::FavoritePipelines);
  m_favorites->setIcon(0, QIcon(":/flash.png"));

  m_favorites->setExpanded(true);

//  std::cout << "Groups Found: " << std::endl;
  for(std::set<std::string>::iterator iter = groupNames.begin(); iter != groupNames.end(); ++iter)
  {
 //   std::cout << *iter << std::endl;
    QString iconName(":/");
    iconName.append( QString::fromStdString(*iter));
    iconName.append("_Icon.png");
    // Validate the icon is in the resource system
    QFileInfo iconInfo(iconName);
    if (iconInfo.exists() == false)
    {
      iconName = ":/Plugin_Icon.png"; // Switch to our generic icon for Plugins that do not provide their own
    }

    QIcon icon(iconName);
    QTreeWidgetItem* filterGroup = new QTreeWidgetItem(library);
    filterGroup->setText(0, QString::fromStdString(*iter));
    filterGroup->setIcon(0, icon);
  }
  library->setExpanded(true);

  m_PipelineViewWidget->setErrorsTextArea(tableWidget);

  m_DocErrorTabsIsOpen = false;

  toggleDocs->setChecked(false);
  showErrors->setChecked(false);
  on_toggleDocs_clicked();
  //on_showErrors_clicked();

  on_filterLibraryTree_itemClicked(library, 0);

  {
  QTreeWidgetItem* presetFilter = new QTreeWidgetItem(presets);
  presetFilter->setText(0, "Ebsd 3D Reconstruction");
  presetFilter->setIcon(0, QIcon(":/scroll.png"));
  QStringList presetFilterList;
  presetFilterList << "EbsdToH5Ebsd" << "ReadH5Ebsd" << "AlignSectionsMisorientation" << "EBSDSegmentGrains" <<
    "DataContainerWriter" << "VtkRectilinearGridWriter";
  m_presetMap["Ebsd 3D Reconstruction"] = presetFilterList;
  }

  {
  QTreeWidgetItem* presetFilter = new QTreeWidgetItem(presets);
  presetFilter->setText(0, "Statistics");
  presetFilter->setIcon(0, QIcon(":/scroll.png"));
  QStringList presetFilterList;
  presetFilterList << "DataContainerReader" << "FindSizes" << "FindNeighborhoods" << "FindAvgOrientations" <<
    "FindShapes" << "FindAxisODF" << "FindLocalMisorientationGradients" << "FindSchmids" << "FindMDF" <<
      "FindODF" << "FieldDataCSVWriter" << "DataContainerWriter";
  m_presetMap["Statistics"] = presetFilterList;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_filterLibraryTree_currentItemChanged(QTreeWidgetItem* item, QTreeWidgetItem* previous )
{
  // Get the QFilterWidget Manager Instance
  FilterWidgetManager::Pointer fm = FilterWidgetManager::Instance();
  FilterWidgetManager::Collection factories;

  //If the user clicks on Detail::Library, display all
  if ( item->text(0).compare(Detail::Library) == 0)
  {
    factories = fm->getFactories();
  }
  else if (NULL != item->parent() && item->parent()->text(0).compare(Detail::Library) == 0)
  {
    factories = fm->getFactories(item->text(0).toStdString());
  }

  updateFilterGroupList(factories);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_filterLibraryTree_itemClicked( QTreeWidgetItem* item, int column )
{
  // Get the QFilterWidget Manager Instance
  FilterWidgetManager::Pointer fm = FilterWidgetManager::Instance();
  FilterWidgetManager::Collection factories;
  if (item->parent() == NULL && item->text(0).compare(Detail::Library) == 0)
  {
    factories = fm->getFactories();
    updateFilterGroupList(factories);
  }
  else if (item->parent() != NULL && item->parent()->text(0).compare(Detail::Library) == 0)
  {
    factories = fm->getFactories(item->text(0).toStdString());
    updateFilterGroupList(factories);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_filterLibraryTree_itemDoubleClicked( QTreeWidgetItem* item, int column )
{
  // Get the QFilterWidget Manager Instance
  QTreeWidgetItem* parent = item->parent();
  if (NULL != parent) {
    if (parent->text(0).compare(Detail::PrebuiltPipelines) == 0) {
      QString text = item->text(0);
      QStringList presetList = m_presetMap[text];
      loadPreset(presetList);
    }
    else if (parent->text(0).compare(Detail::FavoritePipelines) == 0) {
      QString favoriteName = item->text(0);
      QString favoritePath = m_favoritesMap[favoriteName];
      loadFavorites(favoritePath);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::updateFilterGroupList(FilterWidgetManager::Collection &factories)
{
  // Clear all the current items from the list
  filterList->clear();

  for (FilterWidgetManager::Collection::iterator factory = factories.begin(); factory != factories.end(); ++factory)
  {
    IFilterWidgetFactory::Pointer wigFactory = (*factory).second;
    if (NULL == wigFactory.get() ) {
      continue;
    }
    QString humanName = QString::fromStdString(wigFactory->getFilterHumanLabel());
    QString iconName(":/");
    iconName.append( QString::fromStdString(wigFactory->getFilterGroup()));
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
    QString filterName = QString::fromStdString((*factory).first);
    filterItem->setData( Qt::UserRole, filterName);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_filterList_currentItemChanged ( QListWidgetItem * item, QListWidgetItem * previous )
{
  if (NULL == item) { return; }
  QString filterName = item->data(Qt::UserRole).toString();
  FilterWidgetManager::Pointer wm = FilterWidgetManager::Instance();
  if (NULL == wm.get()) { return; }
  IFilterWidgetFactory::Pointer wf = wm->getFactoryForFilter(filterName.toStdString());
  if (NULL == wf.get())
  {
    return;
  }
  AbstractFilter::Pointer filter = wf->getFilterInstance();
  if (NULL == filter.get())
  {
    helpTextEdit->setHtml("");
    return;
  }

  QString html;
  QString resName = QString(":/%1Filters/%2.html").arg(filter->getGroupName().c_str()).arg(filter->getNameOfClass().c_str());
  QFile f(resName);
  if ( f.open(QIODevice::ReadOnly) )
  {
    html = QLatin1String(f.readAll());
  }
  else
  {
    html.append("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">");
    html.append("<html>");
    html.append("<head>");
    html.append("<meta name=\"qrichtext\" content=\"1\" />");
    html.append("<style type=\"text/css\">p, li { white-space: pre-wrap; }</style>");
    html.append("</head>");
    html.append("<body>\n");
    html.append("Documentation file was not found. ");
    html.append(resName);
    html.append("</body></html>\n");
  }

  helpTextEdit->setHtml(html);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_filterList_itemDoubleClicked( QListWidgetItem* item )
{
  m_PipelineViewWidget->addFilter(item->data(Qt::UserRole).toString());
  m_PipelineViewWidget->preflightPipeline();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_toggleDocs_clicked()
{

  if(docErrorTabs->currentIndex() == 0 || (m_DocErrorTabsIsOpen == false))
  {
    docErrorTabs->setCurrentIndex(0);
    m_DocErrorTabsIsOpen = !m_DocErrorTabsIsOpen;
#if 1
    docErrorTabs->setHidden(!m_DocErrorTabsIsOpen);
#else
    QPropertyAnimation *animation1 = new QPropertyAnimation(docErrorTabs, "maximumHeight");
    if(m_DocErrorTabsIsOpen)
    {
      int start = 0;
      int end = 350;
      docErrorTabs->setMaximumHeight(end);
      animation1->setDuration(250);
      animation1->setStartValue(start);
      animation1->setEndValue(end);
    }
    else //open
    {
      int start = docErrorTabs->maximumHeight();
      int end = 0;
      animation1->setDuration(250);
      animation1->setStartValue(start);
      animation1->setEndValue(end);
    }
    animation1->start();
#endif
  }
  else
  {
    docErrorTabs->setCurrentIndex(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_showErrors_clicked()
{
  if(docErrorTabs->currentIndex() == 1 || (m_DocErrorTabsIsOpen == false))
  {
    docErrorTabs->setCurrentIndex(1);
    m_DocErrorTabsIsOpen = !m_DocErrorTabsIsOpen;
#if 1
    docErrorTabs->setHidden(!m_DocErrorTabsIsOpen);
#else
    int deltaX;
    QPropertyAnimation *animation1 = new QPropertyAnimation(docErrorTabs, "maximumHeight");

    if(m_DocErrorTabsIsOpen)
    {
      int start = 0;
      int end = 350;
      docErrorTabs->setMaximumHeight(end);
      deltaX = start;

      animation1->setDuration(250);
      animation1->setStartValue(start);
      animation1->setEndValue(end);
    }
    else //open
    {
      int start = docErrorTabs->maximumHeight();
      int end = 0;
      animation1->setDuration(250);
      animation1->setStartValue(start);
      animation1->setEndValue(end);
    }
    animation1->start();
#endif
  }
  else
  {
    docErrorTabs->setCurrentIndex(1);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_m_LoadSettingsBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Settings File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Settings File (*.txt)") );
  if ( true == file.isEmpty() ) { return; }
  QSettings prefs(file, QSettings::IniFormat, this);
  readSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_m_SaveSettingsBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "PipelineBuilderSettings.txt";
  QString filePath = QFileDialog::getSaveFileName(this, tr("Save PipelineBuilder Settings"),
                                              proposedFile,
                                              tr("*.txt") );
  if ( true == filePath.isEmpty() ) { return; }

  //If the filePath already exists - delete it so that we get a clean write to the file
  QFileInfo fi(filePath);
  if (fi.exists() == true)
  {
    QFile f(filePath);
    f.remove();
  }
  QSettings prefs(filePath, QSettings::IniFormat, this);
  writeSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::checkIOFiles()
{
#if 0
  // Use this code as an example of using some macros to make the validation of
  // input/output files easier
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, m_, CrystallographicErrorFile)

  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, m_ , IPFVizFile)
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_m_GoBtn_clicked()
{
  QTableWidget* errorTableWidget = m_PipelineViewWidget->getTableWidget();
  QStringList m_PipelineErrorList = m_PipelineViewWidget->getPipelineErrorList();

  // clear all the error messages
  m_PipelineErrorList.clear();

  for (int i=0; i<errorTableWidget->rowCount(); ++i) {
    errorTableWidget->removeRow(i);
  }
  errorTableWidget->setRowCount(0);

  if (m_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_FilterPipeline!= NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit cancelPipeline();
    }
    return;
  }



  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource

  m_FilterPipeline = new QFilterPipeline(NULL);

  // Move the PipelineBuilder object into the thread that we just created.
  m_FilterPipeline->moveToThread(m_WorkerThread);

  //
  qint32 count = m_PipelineViewWidget->filterCount();
  for(qint32 i = 0; i < count; ++i)
  {
    QFilterWidget* fw = m_PipelineViewWidget->filterWidgetAt(i);
    if (fw)
    {
      m_FilterPipeline->pushBack(fw->getFilter());
    }
  }

  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * PipelineBuilder object. Since the PipelineBuilder object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the PipelineBuilder going
  connect(m_WorkerThread, SIGNAL(started()),
          m_FilterPipeline, SLOT(run()));

  // When the PipelineBuilder ends then tell the QThread to stop its event loop
  connect(m_FilterPipeline, SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( pipelineComplete() ) );

  // If the use clicks on the "Cancel" button send a message to the PipelineBuilder object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelPipeline() ),
          m_FilterPipeline, SLOT (on_CancelWorker() ) , Qt::DirectConnection);

  // Send Progress from the PipelineBuilder to this object for display
  connect(m_FilterPipeline, SIGNAL (updateProgress(int)),
          this, SLOT(pipelineProgress(int) ) );

  // Send progress messages from PipelineBuilder to this object for display
  connect(m_FilterPipeline, SIGNAL (progressMessage(QString)),
          this, SLOT(addProgressMessage(QString) ));

  // Send progress messages from PipelineBuilder to this object for display
  connect(m_FilterPipeline, SIGNAL (warningMessage(QString, QString, int)),
          this, SLOT(addWarningMessage(QString, QString, int) ));

  // Send progress messages from PipelineBuilder to this object for display
  connect(m_FilterPipeline, SIGNAL (errorMessage(QString, QString, int)),
          this, SLOT(addErrorMessage(QString, QString, int) ));



  setWidgetListEnabled(false);
  emit pipelineStarted();
  m_WorkerThread->start();
  m_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::pipelineComplete()
{
 // std::cout << "PipelineBuilderWidget::PipelineBuilder_Finished()" << std::endl;
  if (m_hasErrors || m_hasWarnings) {
    displayDialogBox(QString::fromStdString("Pipeline Errors | Warnings"),
      QString::fromStdString("Errors and/or warnings occurred during processing.\nPlease check the error table for more information."),
        QMessageBox::Critical);
  }
  m_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->m_progressBar->setValue(0);
  emit pipelineEnded();
  checkIOFiles();
  m_FilterPipeline->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::pipelineProgress(int val)
{
  this->m_progressBar->setValue( val );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::addErrorMessage(QString errName, QString errDesc, int errNum)
{
  m_hasErrors = true;

  QTableWidget* errorTableWidget = m_PipelineViewWidget->getTableWidget();

  int rc = errorTableWidget->rowCount();

  errorTableWidget->insertRow(rc);

  QTableWidgetItem* filterNameWidgetItem = new QTableWidgetItem(errName);
  filterNameWidgetItem->setTextAlignment(Qt::AlignCenter);
  QTableWidgetItem* errorDescriptionWidgetItem = new QTableWidgetItem(errDesc);
  QTableWidgetItem* errorCodeWidgetItem = new QTableWidgetItem( QString::number(errNum) );
  errorCodeWidgetItem->setTextAlignment(Qt::AlignCenter);

  QColor errColor(255, 191, 193);
  QBrush errBrush(errColor);

  filterNameWidgetItem->setBackground(errBrush);
  errorDescriptionWidgetItem->setBackground(errBrush);
  errorCodeWidgetItem->setBackground(errBrush);

  errorTableWidget->setItem(rc, 0, filterNameWidgetItem);
  errorTableWidget->setItem(rc, 1, errorDescriptionWidgetItem);
  errorTableWidget->setItem(rc, 2, errorCodeWidgetItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::addWarningMessage(QString warnName, QString warnDesc, int warnNum)
{
  m_hasWarnings = true;

  //QString title = QString::fromStdString("PipelineBuilderWidget Warning");
  //displayDialogBox(title, message, QMessageBox::Warning);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::addProgressMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::loadPreset(QStringList filterList) {
  //Clear any existing pipeline
  m_PipelineViewWidget->clearWidgets();

  for (int i=0; i< filterList.size(); i++) {
    m_PipelineViewWidget->addFilter(filterList[i]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_addFavoriteBtn_clicked() {
  AddFavoriteWidget* addfavoriteDialog = new AddFavoriteWidget(this);
  addfavoriteDialog->exec();

  QString favoriteTitle = addfavoriteDialog->getFavoriteName();

  //Remove all spaces and illegal characters from favorite name
  favoriteTitle = favoriteTitle.trimmed();
  favoriteTitle = favoriteTitle.remove(" ");
  favoriteTitle = favoriteTitle.remove(QRegExp("[^a-zA-Z_\\d\\s]"));

  if(addfavoriteDialog->getBtnClicked())
  {
    QTreeWidgetItem* favName = new QTreeWidgetItem();
    favName->setText(0, favoriteTitle);
    favName->setIcon(0, QIcon(":/bullet_ball_yellow.png"));

    for (int i = 0; i <= m_favoritesMap.size(); i++)
    {
      QString listLower;
      QChar listChar;
      if(m_favoritesMap.size() != i)
      {
        listLower = m_favorites->child(i)->text(0).toLower();
        listChar = m_favorites->child(i)->text(0).at(0).toLower();
      }
      QString insertLower = favoriteTitle.toLower();
      QChar insertChar = favoriteTitle.at(0).toLower();

      //Handle insert-empty and insert-last cases
      if(m_favoritesMap.size() == i)
      {
        m_favorites->insertChild(i, favName);
        break;
      }

      //Handle duplicate case
      else if(listLower == insertLower)
      {
        QMessageBox::critical(this, QString("DREAM3D"), QString("A favorite named " + favoriteTitle + " already exists.\nPlease try again."));
        delete favName;
        break;
      }

      //Insert in alphabetical order
      else if(listChar > insertChar)
      {
        m_favorites->insertChild(i, favName);
        break;
      }
    }

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

      m_favoritesMap[favoriteTitle] = newPrefPath;

      QDir newParentPrefPathDir = QDir(newParentPrefPath);

      if(newParentPrefPathDir.mkpath(newParentPrefPath))
      {
        QSettings newPrefs(newPrefPath, QSettings::IniFormat);
        newPrefs.beginGroup("favorite_config");
        newPrefs.setValue("Name", favoriteTitle);
        newPrefs.endGroup();
        writeSettings(newPrefs, m_PipelineViewWidget);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::on_removeFavoriteBtn_clicked() {
  QTreeWidgetItem* item = filterLibraryTree->currentItem();
  QTreeWidgetItem* parent = filterLibraryTree->currentItem()->parent();
  if (NULL != parent && parent->text(0).compare(Detail::FavoritePipelines) == 0) {
    QString favoriteName = item->text(0);
    QString filePath = m_favoritesMap[item->text(0)];
    QFileInfo filePathInfo = QFileInfo(filePath);
    QString fileParentPath = filePathInfo.path();
    QString fileName = filePathInfo.fileName();
    QDir fileParentPathDir = QDir(fileParentPath);

    //Remove favorite's pref file
    fileParentPathDir.remove(fileName);

    //Remove favorite from favoritesMap
    m_favoritesMap.remove(favoriteName);

    //Remove favorite, graphically, from the DREAM3D interface
    filterLibraryTree->removeItemWidget(item, 0);
    delete item;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderWidget::loadFavorites(QString path) {
  QSettings prefs(path, QSettings::IniFormat);
  readSettings(prefs, m_PipelineViewWidget);
}
