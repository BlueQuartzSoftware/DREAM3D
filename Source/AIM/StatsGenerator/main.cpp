///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

#include "StatsGeneratorUI.h"

#include <AIM/Common/Qt/QRecentFileList.h>

//-- Qt Headers
#include <QtGui/QApplication>

/**
 * @brief The Main entry point for the application
 */
int main (int argc, char *argv[])
{
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("StatsGeneratorUI");
#if defined( Q_WS_MAC )
  //Needed for typical Mac program behavior.
  app.setQuitOnLastWindowClosed( true );
#endif //APPLE

#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif
  QRecentFileList::instance()->readList(prefs);


  StatsGeneratorUI *viewer = new StatsGeneratorUI;
  viewer->show();
  int app_return = app.exec();

  QRecentFileList::instance()->writeList(prefs);

  return app_return;
}

