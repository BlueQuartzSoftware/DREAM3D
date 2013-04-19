

//-- STL includes
#include <iostream>

//-- Qt Includes
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QDateTime>
#include <QtCore/QVariant>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QComboBox>
#include <QtGui/QHeaderView>
#include <QtGui/QDockWidget>


//-- H5 Support
#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

//-- QHDFViewer Includes
#include "QHDFViewer.h"
#include "QHDF5TreeModel.h"
#include "QHDF5TreeModelItem.h"
#include "QHDFDataTableWindow.h"
#include "QHDFStringDataWindow.h"
#include "QHDFImageDataWindow.h"
#include "QHDFPropertiesWindow.h"
#include "QHDFGroupPropertiesWindow.h"
#include "ImageDataToolbar.h"
#include "QDockUtilities.h"
#include "QRecentFileList.h"


#include "DREAM3DLib/Common/IDataArray.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDFViewer::QHDFViewer(QWidget *parent) :
  QMainWindow(parent),
#if defined(Q_WS_WIN)
      _openDialogLastDirectory("C:\\"),
#else
      _openDialogLastDirectory("~/"),
#endif
      _currentOpenFile(""),
      _currentHDFDataPath(""),
      _fileId(-1)
{
  setupUi(this);
  readSettings();
  setupGui();

  QRecentFileList* recentFileList = QRecentFileList::instance();
  connect(recentFileList, SIGNAL (fileListChanged(const QString &)), this, SLOT(updateRecentFileList(const QString &)) );
  // Get out initial Recent File List
  this->updateRecentFileList(QString::null);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDFViewer::~QHDFViewer()
{
//  std::cout << "QHDFViewer::~QHDFViewer()" << std::endl;
  if (this->_fileId > 0)
  {
    H5Fclose(this->_fileId);
  }
}

// -----------------------------------------------------------------------------
//  Called when the main window is closed.
// -----------------------------------------------------------------------------
void QHDFViewer::closeEvent(QCloseEvent *event)
{
 // std::cout << "QHDFViewer::closeEvent(QCloseEvent *event)" << std::endl;
  closeAllChildWindows();
  qint32 err = _checkDirtyDocument();
  if (this->_fileId > 0)
  {
    H5Fclose(this->_fileId);
  }
  if (err < 0)
  {
    event->ignore();
  }
  else
  {
    writeSettings();
    event->accept();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::dragEnterEvent(QDragEnterEvent* e)
{
  const QMimeData* dat = e->mimeData();
  QList<QUrl> urls = dat->urls();
  QString file = urls.count() ? urls[0].toLocalFile() : QString();
  QDir parent(file);
  this->_openDialogLastDirectory = parent.dirName();
  QFileInfo fi(file );
  QString ext = fi.suffix();
  if (fi.exists() && fi.isFile() && ( ext.compare("mxa") || ext.compare("h5") || ext.compare("hdf5") ) )
  {
    e->accept();
  }
  else
  {
    e->ignore();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::dropEvent(QDropEvent* e)
{
  const QMimeData* dat = e->mimeData();
  QList<QUrl> urls = dat->urls();
  QString file = urls.count() ? urls[0].toLocalFile() : QString();
  QDir parent(file);
  this->_openDialogLastDirectory = parent.dirName();
  QFileInfo fi(file );
  QString ext = fi.suffix();
  file = QDir::toNativeSeparators(file);
  if (fi.exists() && fi.isFile() &&( ext.compare("mxa") || ext.compare("h5") || ext.compare("hdf5") ) )
  {
    QDir parent (file);
    this->_openDialogLastDirectory = parent.dirName();
    openHDF5File(file);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::focusInEvent(QFocusEvent* event)
{
  //TODO: Implement this
  QMainWindow::focusInEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::focusOutEvent(QFocusEvent* event)
{
  //TODO: Implement this
  QMainWindow::focusOutEvent(event);
}


// -----------------------------------------------------------------------------
//  Write our Prefs to file
// -----------------------------------------------------------------------------
void QHDFViewer::writeSettings()
{
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, "MXA", "QHDFViewer", this);
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, "MXA", "QHDFViewer", this);
#endif

  QString groupName = "MainWindow-";
  groupName.append( QRecentFileList::instance()->parentAndFileName(this->_currentOpenFile) );
  prefs.beginGroup(groupName);
  prefs.setValue("Position", this->pos());
  prefs.setValue("Size", this->size());
  prefs.setValue("Layout", this->saveState());
  prefs.endGroup();
  prefs.beginGroup("Splitter");
  prefs.setValue("splitterSizes", splitter->saveState());
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//  Read the prefs from the local storage file
// -----------------------------------------------------------------------------
void QHDFViewer::readSettings()
{
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, "MXA", "QHDFViewer", this);
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, "MXA", "QHDFViewer", this);
#endif
  QString groupName = "MainWindow-";
  groupName.append( QRecentFileList::instance()->parentAndFileName(this->_currentOpenFile) );
  prefs.beginGroup(groupName);

  if(prefs.contains("Size"))
    {
    this->resize(prefs.value("Size").toSize());
    }

  if(prefs.contains("Position"))
    {
    QPoint windowTopLeft = prefs.value("Position").toPoint();
    QRect mwRect(windowTopLeft, this->size());

    QDesktopWidget desktop;
    QRect desktopRect = desktop.availableGeometry( desktop.primaryScreen() );
    // try moving it to keep size
    if(!desktopRect.contains(mwRect))
      {
      mwRect = QRect(desktopRect.topLeft(), this->size());
      }
    // still doesn't fit, resize it
    if(!desktopRect.contains(mwRect))
      {
      mwRect = QRect(desktopRect.topLeft(), this->size());
      this->resize(desktopRect.size());
      }
    this->move(mwRect.topLeft());
    }

  if(prefs.contains("Layout"))
    {
    this->restoreState(prefs.value("Layout").toByteArray());

    QList<QDockWidget*> dockWidgets = this->findChildren<QDockWidget*>();
    foreach(QDockWidget* dock_widget, dockWidgets)
      {
      if (dock_widget->isFloating() == true )
        {
        QDockUtilities::sanityCheckDock(dock_widget);
        }
      }
   }

  prefs.endGroup();

  prefs.beginGroup("Splitter");
  splitter->restoreState(prefs.value("splitterSizes").toByteArray());
  prefs.endGroup();
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::setupGui()
{
  hdfTreeView->addAction(actionShow_Properties);
  setupMenuBar();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::updateRecentFileList(const QString &file)
{
 // std::cout << "QDataViewer::updateRecentFileList->" << this->windowTitle().toStdString() << std::endl;

  // Clear the Recent Items Menu
  this->menu_RecentFiles->clear();

  // Get the list from the static object
  QStringList files = QRecentFileList::instance()->fileList();
  foreach (QString file, files)
    {
      QAction* action = new QAction(this->menu_RecentFiles);
      action->setText(QRecentFileList::instance()->parentAndFileName(file));
      action->setData(file);
      action->setVisible(true);
      this->menu_RecentFiles->addAction(action);
      connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::openRecentFile()
{
  //std::cout << "QRecentFileList::openRecentFile()" << std::endl;

  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
  {
    //std::cout << "Opening Recent file: " << action->data().toString().toStdString() << std::endl;
    QString file = action->data().toString();
    openHDF5File( file );
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::actionOpen_triggered()
{
  //std::cout << "on_actionOpen_triggered" << std::endl;
  QString hdfFile = QFileDialog::getOpenFileName(this, tr("Open MXA File"), _openDialogLastDirectory, tr("All Files (*);;HDF5 (*.h5 *.mxa)") );

  if ( true == hdfFile.isNull() )
  {
    return;
  }
  openHDF5File(hdfFile);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::actionClose_triggered() {
  std::cout << "QHDFViewer::on_actionClose_triggered" << std::endl;
  qint32 err = -1;
  err = _checkDirtyDocument();
  if (err >= 0)
  {
    // Close the window. Files have been saved if needed
    if (QApplication::activeWindow() == this)
    {
      this->closeAllChildWindows();
      this->close();
    }
    else
    {
      QApplication::activeWindow()->close();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::on_actionShow_Properties_triggered()
{
  QModelIndex current = this->hdfTreeView->selectionModel()->currentIndex();
  QHDF5TreeModel* model = static_cast<QHDF5TreeModel*>(hdfTreeView->model() );
  QString path = model->hdfPathForIndex(current);

  // std::cout << "Path is: " << path.toStdString() << std::endl;

  std::string datasetPath = path.toStdString();
  bool isGroup = H5Utilities::isGroup(this->_fileId, datasetPath);
  if (true == isGroup)
  {
    QHDFGroupPropertiesWindow* propDialog = new QHDFGroupPropertiesWindow(this->_fileId, path, this);
    propDialog->setVisible(true);
  }
  else
  {
    QHDFPropertiesWindow* propDialog = new QHDFPropertiesWindow(this->_fileId, path, this);
    propDialog->setVisible(true);
  }

/*  updateGeneralTable(path);
 *
  updateAttributeTable(path);*/
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::openHDF5File(QString &hdfFile)
{
  if ( true == hdfFile.isNull() ) // User cancelled the operation
  {
    return;
  }
  if (this->_currentOpenFile.isEmpty() ) // No file has been opened in this window yet
  {
    this->initWithFile(hdfFile);
  }
  else  // Current Window has a file already opened, so open a new window
  {
    QHDFViewer* mainWindow = new QHDFViewer;
    mainWindow->initWithFile(hdfFile);
    mainWindow->show();
  }
  // Tell the RecentFileList to update itself then broadcast those changes.
  QRecentFileList::instance()->addFile(hdfFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 QHDFViewer::_checkDirtyDocument()
{
  qint32 err = 1;

#if 0
  This section should be enabled if we are going to do any editing of the document
  if (this->isWindowModified() == true)
  {
    int r = QMessageBox::warning(this, tr("MXA Editor"),
                            tr("The Model has been modified.\nDo you want to save your changes?"),
                            QMessageBox::Save | QMessageBox::Default,
                            QMessageBox::Discard,
                            QMessageBox::Cancel | QMessageBox::Escape);
    if (r == QMessageBox::Save)
    {
      err = this->_saveDataModel();
    }
    else if (r == QMessageBox::Discard)
    {
      err = 1;
    }
    else if (r == QMessageBox::Cancel)
    {
      err = -1;
    }
  }
  else
  {
    err = 1;
  }
#endif
  return err;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::initWithFile(const QString &hdfFile)
{
  //std::cout << "QHDFViewer::initWithFile" << std::endl;
  // Save the last place the user visited while opening the file
  QFileInfo fileInfo(hdfFile);
  this->_openDialogLastDirectory = fileInfo.path();
  _currentOpenFile = hdfFile;

  _fileId = H5Utilities::openFile(hdfFile.toStdString(), true);
  if (_fileId < 0)
  {
    std::cout << "Error Reading HDF5 file: " << hdfFile.toStdString() << std::endl;
    return;
  }

  // Set the Window Title to the file name
  this->setWindowTitle(fileInfo.fileName());


  //Get the QHDF5TreeModel and set the Root Node
  QHDF5TreeModel* treeModel = new QHDF5TreeModel(this->_fileId, this->hdfTreeView);
  this->hdfTreeView->setModel(treeModel); //Set the QAbstractItemModel into the tree view
  //this->hdfTreeView->header()->hide();    // Hide the header
  //this->hdfTreeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  //this->hdfTreeView->setTextElideMode(Qt::ElideMiddle);
  //this->hdfTreeView->setFrameStyle(QFrame::NoFrame);
#if defined( Q_WS_MAC )
  this->hdfTreeView->setAttribute(Qt::WA_MacShowFocusRect, false);
#endif
  //this->hdfTreeView->setAutoFillBackground(true);


  //Connect the Tree View selection Model to a method in this class
  connect(hdfTreeView->selectionModel(), SIGNAL(currentChanged( QModelIndex, QModelIndex ) ), this, SLOT(hdfTreeView_currentChanged( QModelIndex, QModelIndex) ));

  connect(hdfTreeView, SIGNAL(doubleClicked( QModelIndex ) ), this, SLOT(hdfTreeView_doubleClicked( QModelIndex) ));

  this->attributesTable->horizontalHeader()->setStretchLastSection(true); // Stretch the last column to fit to the viewport

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string QHDFViewer::generateHDFPath()
{
  std::string frontPath;
  //TODO: Implelement this. or maybe in the model....
  return frontPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::_updateViewFromHDFPath(std::string dataPath)
{
    _currentHDFDataPath = dataPath;
  //  _hdfGraphicsDelegate.displayHDFData(this->_dataFilePtr->getFileId(), dataPath, attributesTable);
    QString message("Current Dataset:");
    message.append(dataPath.c_str() );
    this->statusBar()->showMessage(message);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::hdfTreeView_currentChanged(const QModelIndex & current, const QModelIndex & previous)
{
  //Check to make sure we have a data file opened
  if (this->_fileId < 0)
  {
    std::cout << "\t No data file is opened" << std::endl;
    return;
  }

  QHDF5TreeModel* model = static_cast<QHDF5TreeModel*>(hdfTreeView->model() );
  QString path = model->hdfPathForIndex(current);

  updateGeneralTable(path);
  updateAttributeTable(path);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t QHDFViewer::updateGeneralTable(const QString &path)
{
  std::string datasetPath = path.toStdString();
  std::string objName = H5Utilities::extractObjectName(datasetPath);
  QString objType;

  //std::cout << "HDFGraphicsDelegate::displayHDFData: " << datasetPath << std::endl;

  herr_t err = 0;
  if (this->_fileId < 0)
  {
    std::cout << "Error: FileId is Invalid: " << this->_fileId << std::endl;
    // _displayTextMessage(QString("HDF5 File ID Invalid") );
    return this->_fileId;
  }
  // Test for Dataset Existance
  H5O_info_t statbuf;
  err = H5Oget_info_by_name(this->_fileId, datasetPath.c_str(),  &statbuf, H5P_DEFAULT);
  bool isGroup = false;
  if (err < 0)
  {
    generalTable->clearContents(); // Clear the attributes Table
    std::cout << "Data Set Does NOT exist at Path given: FileId: " << this->_fileId << std::endl;
    return err;
  }
  generalTable->clearContents(); // Clear the attributes Table
  // Put the basic info in first
  generalTable->setRowCount(4);
  generalTable->setColumnCount(2);
  generalTable->verticalHeader()->hide();
  generalTable->horizontalHeader()->hide();


#if 0
  QStringList headerLabels;
  headerLabels.insert(0, tr("Key"));
  headerLabels.insert(1, tr("Value"));
  generalTable->setHorizontalHeaderLabels( headerLabels );
#endif

  switch (statbuf.type)
  {
    case H5O_TYPE_GROUP:
      isGroup = true;
      objName = "HDF5 Group";
      break;
    case H5O_TYPE_DATASET:
      objName = "HDF5 Dataset";
      isGroup = false;
      break;
    case H5O_TYPE_NAMED_DATATYPE:
      isGroup = false;
      objName = "Named DataType";
      break;
    default:
      isGroup = false;
      objName = "UNKNOWN";
  }
  //Update the attributes table
  int row = 0;

  addRow(generalTable, row, "Name", QString::fromStdString(objName));
  ++row;
  addRow(generalTable, row, "Path", path);
  ++row;
  addRow(generalTable, row, "Type", objType);
  ++row;

  QString objectIdString = QString::number(statbuf.addr);
  addRow(generalTable, row, "Object ID", objectIdString);
  ++row;

#if 1
  if (isGroup == false)
  {
    generalTable->setRowCount(7);
    H5T_class_t attr_type;
    size_t attr_size;
   // std::string res;
    std::vector<hsize_t> dims;
    err = H5Lite::getDatasetInfo(this->_fileId, datasetPath, dims, attr_type, attr_size);
    if (err < 0)
    {
      std::cout << "Could not get dataset info for " << datasetPath << std::endl;
      return err;
    }

    addRow(generalTable, row, "No. of Dimension(s)", QString::number(dims.size() ) );
    ++row;

    QString key;
    int index = 0;
    for (std::vector<hsize_t>::iterator vIter = dims.begin(); vIter != dims.end(); ++vIter)
    {
      if (index > 0)
      {
        key += " x ";
      }
      key +=  QString::number(*vIter);

      ++index;
    }
    addRow(generalTable, row, "Dimensions Size(s)", key);
    row++;
    hid_t typeId = H5Lite::getDatasetType(this->_fileId, datasetPath);
    QString theType;
    if (attr_type == H5T_STRING)
    {
      theType = "H5T_STRING";
    }
    else
    {
      theType = QString::fromStdString( H5Lite::StringForHDFType(typeId) );
    }
    err = H5Tclose(typeId);
    addRow(generalTable, row, "Data Type", theType);
    row++;
  }
  else
  {
    generalTable->setRowCount(5);
    hid_t grpId = H5Gopen(this->_fileId, datasetPath.c_str(), H5P_DEFAULT );
    hsize_t num_objs = 0;

    H5G_info_t group_info;
    err = H5Gget_info(grpId, &group_info);
    if (err < 0) {
      std::cout << "Error getting number of objects for group: "
          << grpId << std::endl;
      return err;
    }
    num_objs = group_info.nlinks;




    addRow(generalTable, row, "Number of Members", QString::number(num_objs));
    ++row;
#if 0
    int objType = 0;
    QString objTypeStr;
    // This section will NOT scale well for a group that has a large number of objects in it
    // Some sort of threading API might be a better idea to use for this.
    for (hsize_t i = 0; i < num_objs; ++i)
    {
      generalTable->setRowCount(row + 1);
      objType = H5Gget_objtype_by_idx(grpId, i);
      switch (objType)
      {
        case 0:
          objTypeStr = "HDF5 Link";
          break;
        case 1:
          objTypeStr = "HDF5 Group";
          break;
        case 2:
          objTypeStr = "HDF5 Dataset";
          break;
        case 3:
          objTypeStr = "HDF5 Named Data type";
          break;
        default:
          objTypeStr = "UNKNOWN";
      }
      addRow(generalTable, row, QString::number(i), objTypeStr );
      ++row;
    }
#endif
    H5Gclose(grpId);


  }

#endif


  generalTable->resizeColumnsToContents();
  generalTable->horizontalHeader()->setStretchLastSection(true);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::addRow(QTableWidget* table, int row, const QString &key, const QString &value)
{

    QTableWidgetItem* keyItem  = new QTableWidgetItem( key );
    table->setItem(row, 0, keyItem );
    QTableWidgetItem* valueItem  = new QTableWidgetItem( value );
    table->setItem(row, 1, valueItem );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t QHDFViewer::updateAttributeTable(const QString &path)
{
  std::string datasetPath = path.toStdString();
  //std::cout << "HDFGraphicsDelegate::displayHDFData: " << datasetPath << std::endl;

  herr_t err = 0;
  if (this->_fileId < 0)
  {
    std::cout << "Error: FileId is Invalid: " << this->_fileId << std::endl;
   // _displayTextMessage(QString("HDF5 File ID Invalid") );
    return this->_fileId;
  }
  // Test for Dataset Existance
  H5O_info_t statbuf;
  err = H5Oget_info_by_name(this->_fileId, datasetPath.c_str(),  &statbuf, H5P_DEFAULT);
  if (err < 0)
  {
    attributesTable->clearContents(); // Clear the attributes Table
    //_rawDataTableModel->resetModel(); // Clear the Raw data table
    //_displayTextMessage(QString("Data Set Does Not Exist at Path given"));
    std::cout << "Data Set Does NOT exist at Path given: FileId: " << this->_fileId << std::endl;
    return err;
  }


  //Update the attributes table
  if (NULL != attributesTable)
  {
    attributesTable->clearContents(); // Clear the attributes Table
    //std::list<std::string> attributes;
    MXAAbstractAttributes attributes;
    //err = H5Utilities::getAllAttributeNames(this->_fileId, datasetPath, attributes);
    err = H5Utilities::readAllAttributes(this->_fileId, datasetPath, attributes);
    if (err < 0)
    {
      std::cout << "Error Reading Attributes for datasetPath: " << datasetPath << std::endl;
    }
    else
    {
      attributesTable->setRowCount(attributes.size());
      attributesTable->setColumnCount(2);
      attributesTable->verticalHeader()->hide();
      QStringList headerLabels;
      headerLabels.insert(0, tr("Name"));
      headerLabels.insert(1, tr("Value"));
      attributesTable->setHorizontalHeaderLabels( headerLabels );
      IDataArray::Pointer attr;
      qint32 row = 0;
      for (MXAAbstractAttributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
      {
        std::string key = (*iter).first;
        attr = (*iter).second;
        QTableWidgetItem* keyItem = new QTableWidgetItem ( key.c_str() );
        attributesTable->setItem(row, 0, keyItem);

        QTableWidgetItem* valueItem = new QTableWidgetItem ( attr->valueToString(' ').c_str() );
        attributesTable->setItem(row, 1, valueItem);


        ++row;
      }
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::hdfTreeView_doubleClicked(const QModelIndex & index)
{
  //Check to make sure we have a data file opened
  if (this->_fileId < 0)
  {
    std::cout << "\t No data file is opened" << std::endl;
    return;
  }

  QHDF5TreeModel* model = static_cast<QHDF5TreeModel*> (hdfTreeView->model());
  QString path = model->hdfPathForIndex(index);
  QHDF5TreeModelItem* item = static_cast<QHDF5TreeModelItem*>(index.internalPointer());

  //std::cout << "path double clicked: " << path.toStdString() << std::endl;

  if (false == item->isGroup() )
  {
    // Figure out what type of data to show
    IH5DataWindow* dataWindow = findDataWindow(path);
    if (NULL == dataWindow)
    {
      if (item->isString())
      {
        dataWindow = new QHDFStringDataWindow(this->_currentOpenFile, this->_fileId, path, this);
      }
      else if (item->isImage())
      {
        dataWindow = new QHDFImageDataWindow(this->_currentOpenFile, this->_fileId, path, this);
        ImageDataToolbar toolbar(dataWindow);
      }
      else if (item->numDimensions() > 0)
      {
        dataWindow = new QHDFDataTableWindow(this->_currentOpenFile, this->_fileId, path, this);
      }
    }
    dataWindow->show(); // Bring the window to the front
    dataWindow->raise();
    dataWindow->activateWindow();
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IH5DataWindow* QHDFViewer::findDataWindow(const QString &hdfPath)
{
    //QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QObject* widget, this->children() )
    {
      IH5DataWindow *mainWin = qobject_cast<IH5DataWindow*>(widget);
        if (mainWin && mainWin->hdfPath() == hdfPath && mainWin->fileId() == this->_fileId)
        {
            return mainWin;
        }
    }
    return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFViewer::closeAllChildWindows()
{
    //QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QWidget *widget, qApp->topLevelWidgets())
    {
      QWidget *mainWin = qobject_cast<QWidget *>(widget);
        if (mainWin && mainWin->parent() == this)
        {
            mainWin->close();
        }
    }
}
