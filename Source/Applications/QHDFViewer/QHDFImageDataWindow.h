///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef QHDF_IMAGE_DATA_WINDOW_H_
#define QHDF_IMAGE_DATA_WINDOW_H_

//-- Qt Includes
#include <QtGui/QWidget>
#include <QtGui/QMainWindow>

#include <hdf5.h>

//-- UIC generated Header
#include <ui_QHDFImageDataWindow.h>


#include "IH5DataWindow.h"
#include "HDFGraphicsDelegate.h"

class QHDF5TableModel;
class QGraphicsScene;

class QHDFImageDataWindow : public IH5DataWindow, private Ui::QHDFImageDataWindow
{
  Q_OBJECT;


public:
  QHDFImageDataWindow(const QString &fileName,
                      hid_t fileId,
                      const QString &hdfPath,
                      QWidget *parent = 0);

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

protected slots:
  void sceneRectChanged(const QRectF &rect);
  void zoomIn();
  void zoomOut();
  void fitToWindow();

private:
  QString           _filePath;
  hid_t             _fileId;
  QString           _hdfPath;  //Stores the currently viewed HDF data path

  QGraphicsScene*    _hdfGraphicsScene;
  HDFGraphicsDelegate*  _hdfGraphicsDelegate; //Stores the object that renders HDF data


  /**
   * @brief Initializes some of the GUI elements with selections or other GUI related items
   */
  void init();


  ~QHDFImageDataWindow();
  QHDFImageDataWindow(const QHDFImageDataWindow&);   //Copy Constructor Not Implemented
  void operator=(const QHDFImageDataWindow&); //Copy Assignment Not Implemented
};

#endif /*QHDF_IMAGE_DATA_WINDOW_H_*/
