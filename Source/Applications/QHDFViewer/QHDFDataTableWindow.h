///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef QHDFDATATABLEWINDOW_H_
#define QHDFDATATABLEWINDOW_H_

//-- Qt Includes
#include <QtGui/QWidget>
#include <QtGui/QMainWindow>

#include <hdf5.h>

//-- UIC generated Header
#include <ui_QHDFDataTableWindow.h>
#include "IH5DataWindow.h"

class QHDF5TableModel;


class QHDFDataTableWindow : public IH5DataWindow, private Ui::QHDFDataTableWindow
{
  Q_OBJECT;


public:
  QHDFDataTableWindow(const QString &fileName, hid_t fileId, const QString &hdfPath, QWidget *parent = 0);

  /**
   * @brief Implements the CloseEvent to Quit the application and write settings
   * to the preference file
   */
  void closeEvent(QCloseEvent *event);
  virtual void focusInEvent(QFocusEvent* event);
  virtual void focusOutEvent(QFocusEvent* event);

  QString hdfPath();
  hid_t   fileId();


protected:


private:
  QString           _filePath;
  hid_t             _fileId;
  QString           _hdfPath;  //Stores the currently viewed HDF data path
  QHDF5TableModel*  _hdfDataTableModel;

  /**
   * @brief Initializes some of the GUI elements with selections or other GUI related items
   */
  void init();


  ~QHDFDataTableWindow();
  QHDFDataTableWindow(const QHDFDataTableWindow&);   //Copy Constructor Not Implemented
  void operator=(const QHDFDataTableWindow&); //Copy Assignment Not Implemented
};

#endif /*QHDFDATATABLEWINDOW_H_*/
