///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef IH5DATAWINDOW_H_
#define IH5DATAWINDOW_H_

#include <iostream>

#include <hdf5.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QMenu>



/**
* @class IH5DataWindow IH5DataWindow.h Shared/IH5DataWindow.h
* @brief Abstract (pure) class for windows that show data
* @author
* @date
* @version
*/
class IH5DataWindow : public QMainWindow
{
  Q_OBJECT;


public:
  IH5DataWindow( QWidget *parent = 0);
  virtual ~IH5DataWindow();
  virtual QString hdfPath() = 0;
  virtual hid_t   fileId() = 0;
  virtual void focusInEvent(QFocusEvent *) = 0;
  virtual void focusOutEvent(QFocusEvent *) = 0;

  virtual void setupMenuBar2()
  {
    QMenuBar* menubar = new QMenuBar(this);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 345, 22));
    QMenu* menuFile = new QMenu(menubar);
    menuFile->setObjectName(QString::fromUtf8("menuFile"));
    this->setMenuBar(menubar);


    menubar->addAction(menuFile->menuAction());

    QAction* actionOpen = new QAction(this);
    actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
    actionOpen->setShortcutContext(Qt::WidgetShortcut);
    actionOpen->setText(QApplication::translate("IH5DataWindow", "Open ...", 0, QApplication::UnicodeUTF8));
    actionOpen->setShortcut(QApplication::translate("IH5DataWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));

    QAction* actionClose = new QAction(this);
    actionClose->setObjectName(QString::fromUtf8("actionClose"));
    actionClose->setShortcutContext(Qt::WidgetShortcut);
    actionClose->setText(QApplication::translate("IH5DataWindow", "Close", 0, QApplication::UnicodeUTF8));
    actionClose->setShortcut(QApplication::translate("IH5DataWindow", "Ctrl+W", 0, QApplication::UnicodeUTF8));

    menuFile->addAction(actionOpen);
    menuFile->addAction(actionClose);
  }

public slots: // AutoHookup Methods
  //Menu Actions
#if 0
  virtual void on_actionOpen_triggered()
  {
    std::cout << "IH5DataWindow::on_actionOpen_triggered" << std::endl;
  }

  virtual void on_actionClose_triggered()
  {
    std::cout << "IH5DataWindow::on_actionClose_triggered" << std::endl;
  }
#endif

private:

  IH5DataWindow(const IH5DataWindow&);    //Not Implemented
  void operator=(const IH5DataWindow&); //Not Implemented
};


#endif /* IH5DATAWINDOW_H_ */
