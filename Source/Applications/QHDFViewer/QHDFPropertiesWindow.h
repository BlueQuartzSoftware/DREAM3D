///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, mjackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////
#ifndef QHDFPROPERTIESWINDOW_H_
#define QHDFPROPERTIESWINDOW_H_

//-- Qt Includes
#include <QtCore/QObject>
#include <QtGui/QDialog>

//-- UIC generated Header
#include <ui_QHDFPropertiesWindow.h>

//-- HDF5 include
#include <hdf5.h>





class QHDFPropertiesWindow : public QDialog, private Ui::HDFPropertiesWindow
{

  Q_OBJECT;

  public:
  QHDFPropertiesWindow(hid_t fileId, const QString &hdfPath, QWidget *parent = 0);
  /**
   * @brief Implements the CloseEvent to Quit the application and write settings
   * to the preference file
   */
  void closeEvent(QCloseEvent *event);

  protected:
    void addRow(QTableWidget* table, int row, const QString &key, const QString &value);

  protected slots:

  private:
  hid_t _fileId;
  QString _hdfPath; //Stores the currently viewed HDF data path


  /**
   * @brief Initializes some of the GUI elements with selections or other GUI related items
   */
  void init();
  void initGeneralTab();
  void initAttributesTab();


  ~QHDFPropertiesWindow();
  QHDFPropertiesWindow(const QHDFPropertiesWindow&); //Copy Constructor Not Implemented
  void operator=(const QHDFPropertiesWindow&); //Copy Assignment Not Implemented

};

#endif /* QHDFPROPERTIESWINDOW_H_ */
