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

//TODO: Display Data as Image if possible
//TODO: Export Data Set as CSV or Image
//TODO: Add Rendering Hints to datasets


//-- MXA Headers
#include <QHDFViewer.h>
#include "QRecentFileList.h"
#include "QHDFViewerApplication.h"


#include <QtGui/QApplication>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
/**
 * @brief The Main entry point for the application
 */
int main (int argc, char *argv[])
{
  QHDFViewerApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("QHDFViewer");

#if defined( Q_WS_MAC )
  //Needed for typical Mac program behavior.
  app.setQuitOnLastWindowClosed( true );
#endif //APPLE

#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, "bluequartz.net", "QHDFViewer");
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, "bluequartz.net", "QHDFViewer");
#endif
  QRecentFileList::instance()->readList(prefs);

  QHDFViewer *viewer = new QHDFViewer;
  viewer->show();
  viewer->raise();
  viewer->activateWindow();
  int app_return = app.exec();

  QRecentFileList::instance()->writeList(prefs);

  return app_return;
}



