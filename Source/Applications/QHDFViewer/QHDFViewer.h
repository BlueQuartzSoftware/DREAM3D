///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _QMXAVIEWER_H_
#define _QMXAVIEWER_H_

//-- Qt Includes
#include <QtGui/QWidget>
#include <QtGui/QMainWindow>

#include <hdf5.h>


//-- UIC generated Header
#include <ui_QHDFViewer.h>

//-- MXAViewer Includes
#include "HDFGraphicsDelegate.h"

class IH5DataWindow;
class QDockWidget;

//TODO: Add each new file to a "Recent Files" list


/* Uses the 'Private Inheritance' method from the *.ui file */
/**
 * @brief Main Window for the Qt based HDF5 Data Viewer
 * @author Mike Jackson
 * @date June 2008
 * @version $Revision: 1.7 $
 * @class QHDFViewer QHDFViewer.h QHDFViewer.h
 */
class QHDFViewer : public QMainWindow, private Ui::QHDFViewer
{
  Q_OBJECT;


public:
  QHDFViewer(QWidget *parent = 0);

  virtual void focusInEvent(QFocusEvent* event);
  virtual void focusOutEvent(QFocusEvent* event);
  void openHDF5File(QString &hdf5File);
  QString getCurrentFile() { return _currentOpenFile; }
protected:

  /**
   * @brief Implements the CloseEvent to Quit the application and write settings
   * to the preference file
   */
  void closeEvent(QCloseEvent *event);

  /**
   * @brief Drag and drop implementation
   */
  void dragEnterEvent(QDragEnterEvent*);

  /**
   * @brief Drag and drop implementation
   */
  void dropEvent(QDropEvent*);

  /**
   * @brief Reads the Preferences from the users pref file
   */
  void readSettings();

  /**
   * @brief Writes the preferences to the users pref file
   */
  void writeSettings();

  /**
   * @brief Initializes some of the GUI elements with selections or other GUI related items
   */
  void setupGui();

  /**
   * @brief checks to make sure undocked widgets are correctly positioned on screen.
   * @param dock_widget the QDockWidget to check.
   */
  void sanityCheckDock(QDockWidget* dock_widget);

  /**
   * @brief Initializes the window by trying to open and populate the window
   * with values from the passed in file
   * @param mxaFile A Path to an MXA File
   */
  void initWithFile(const QString &mxaFile);

private slots:
  // AutoHookup Methods
  void on_actionShow_Properties_triggered();

  //Manual Hookup Menu Actions
  void actionOpen_triggered(); // Open a Data File
  void actionClose_triggered();
  void hdfTreeView_doubleClicked(const QModelIndex & index);

//--------------- Manual Connect methods ---------------------------------------
  /**
   * @brief Slot to catch events when the DataRecord TreeView selection is changed
   * @param current The currently selected Index
   * @param previous The previously selected Index
   */
  void hdfTreeView_currentChanged ( const QModelIndex & current, const QModelIndex & previous );

  /**
   * @brief Updates the QMenu 'Recent Files' with the latest list of files. This
   * should be connected to the Signal QRecentFileList->fileListChanged
   * @param file The newly added file.
   */
  void updateRecentFileList(const QString &file);

  /**
   * @brief Qt Slot that fires in response to a click on a "Recent File' Menu entry.
   */
  void openRecentFile();

private:

  QString               _openDialogLastDirectory; //Stores the last directory the user visited
  QString               _currentOpenFile;    //Stores the currently open HDF5 File
  std::string           _currentHDFDataPath;  //Stores the currently viewed HDF data path
  hid_t                 _fileId;
  QMenu*                menu_RecentFiles;
  void setupMenuBar()
    {

      QAction* actionOpen = new QAction(this);
      actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
      actionOpen->setShortcutContext(Qt::WidgetShortcut);
      actionOpen->setText(QApplication::translate("QHDFViewer", "Open ...", 0, QApplication::UnicodeUTF8));
      actionOpen->setShortcut(QApplication::translate("QHDFViewer", "Ctrl+O", 0, QApplication::UnicodeUTF8));

      QAction* actionClose = new QAction(this);
      actionClose->setObjectName(QString::fromUtf8("actionClose"));
      actionClose->setShortcutContext(Qt::WidgetShortcut);
      actionClose->setText(QApplication::translate("QHDFViewer", "Close", 0, QApplication::UnicodeUTF8));
      actionClose->setShortcut(QApplication::translate("QHDFViewer", "Ctrl+W", 0, QApplication::UnicodeUTF8));

      menuFile->addAction(actionOpen);
      menuFile->addAction(actionClose);

      menu_RecentFiles = new QMenu(menuFile);
      menu_RecentFiles->setObjectName(QString::fromUtf8("menu_RecentFiles"));
      menu_RecentFiles->setTitle(QApplication::translate("QHDFViewer", "Recent Files...", 0, QApplication::UnicodeUTF8));


      menuFile->addSeparator();
      menuFile->addAction(menu_RecentFiles->menuAction());
      menu_RecentFiles->addSeparator();

      connect(actionOpen, SIGNAL(triggered()), this, SLOT(actionOpen_triggered()));
      connect(actionClose, SIGNAL(triggered()), this, SLOT(actionClose_triggered()));


      #if defined( Q_WS_MAC )
      menubar->setParent(NULL);
      #else
      menubar->setParent(this);
      #endif
    }


  /**
   * @brief Generates the path to the HDF dataset based on the values in the
   * data dimension table and the selected data record.
   * @param model The MXADataModel
   * @param indices The Indices for the Data Dimensions
   * @param record The selected MXADataRecord
   * @return The path as a std::string object
   */
  std::string generateHDFPath();

  /**
   * @brief Updates the QGraphicsView based on the current Data Dimension and Data record values
   * @param path The path to the HDF data set
   */
  void _updateViewFromHDFPath(std::string path);

  herr_t updateAttributeTable(const QString &datasetPath);
  herr_t updateGeneralTable(const QString &path);
  void addRow(QTableWidget* table, int row, const QString &key, const QString &value);



  qint32 _checkDirtyDocument();

  IH5DataWindow* findDataWindow(const QString &hdfPath);

  void closeAllChildWindows();

  ~QHDFViewer();
  QHDFViewer(const QHDFViewer&);   //Copy Constructor Not Implemented
  void operator=(const QHDFViewer&); //Copy Assignment Not Implemented
};



#endif /* _QMXAVIEWER_H_ */

