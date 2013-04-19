///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#include "QDockUtilities.h"

#include <QtGui/QDockWidget>
#include <QtGui/QDesktopWidget>

QDockUtilities::QDockUtilities()
{
  // TODO Auto-generated constructor stub

}

QDockUtilities::~QDockUtilities()
{
  // TODO Auto-generated destructor stub
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDockUtilities::sanityCheckDock(QDockWidget* dock_widget)
{
  QDesktopWidget desktop;
  int screen = -1;
  if (NULL == dock_widget)
  {
    return;
  }

  QPoint dockTopLeft = dock_widget->pos();
  QRect dockRect(dockTopLeft, dock_widget->size());

  QRect geometry = QRect(dockTopLeft, dock_widget->frameSize());
  int titleBarHeight = geometry.height() - dockRect.height();

  screen = desktop.screenNumber(dock_widget);
  if (screen == -1) // Dock is at least partially on a screen
  {
    screen = desktop.screenNumber(dockTopLeft);
  }

  QRect screenRect = desktop.availableGeometry(screen);
  QRect desktopRect = desktop.availableGeometry(); // SHould give us the entire Desktop geometry
  // Ensure the top left corner of the window is on the screen
  if (!screenRect.contains(dockTopLeft))
  {
    // Are we High?
    if (dockTopLeft.y() < screenRect.y()) {
      dock_widget->move(dockRect.x(), screenRect.y());
      dockTopLeft = dock_widget->pos();
      dockRect = QRect(dockTopLeft, dock_widget->frameSize());
   }
    // Are we low
    if (dockTopLeft.y() > screenRect.y() + screenRect.height()) {
      dock_widget->move(dockRect.x(), screenRect.y() + screenRect.height() - 20);
      dockTopLeft = dock_widget->pos();
      dockRect = QRect(dockTopLeft, dock_widget->frameSize());
      }
    // Are we left
    if (dockTopLeft.x() < screenRect.x()) {
      dock_widget->move(screenRect.x(), dockRect.y());
      dockTopLeft = dock_widget->pos();
      dockRect = QRect(dockTopLeft, dock_widget->frameSize());
      }
    // Are we right
    if (dockTopLeft.x() > screenRect.x() + screenRect.width()) {
        dock_widget->move(screenRect.x() + screenRect.width() - dockRect.width(), dockRect.y());
        dockTopLeft = dock_widget->pos();
        dockRect = QRect(dockTopLeft, dock_widget->frameSize());
     }

    dockTopLeft = dock_widget->pos();
    dockRect = QRect(dockTopLeft, dock_widget->frameSize());
    }

  if ( !desktopRect.contains(dockRect))
  {
    //Are we too wide
    if (dockRect.x() + dockRect.width() > screenRect.x() + screenRect.width())
    {
      if (screenRect.x() + screenRect.width() - dockRect.width() > screenRect.x() )
      {
        // Move dock side to side
        dockRect.setX(screenRect.x() + screenRect.width() - dockRect.width() );
        dock_widget->move(dockRect.x(), dockRect.y());
        dockTopLeft = dock_widget->pos();
        dockRect = QRect(dockTopLeft, dock_widget->frameSize());
     }
      else
      {
        // Move dock side to side + resize to fit
        dockRect.setX(screenRect.x() + screenRect.width() - dockRect.width() );
        dockRect.setWidth(screenRect.width());
        dock_widget->resize(dockRect.width(), dockRect.height());
        dock_widget->move(dockRect.x(), dockRect.y());
        dockTopLeft = dock_widget->pos();
        dockRect = QRect(dockTopLeft, dock_widget->frameSize());
         }
    }

    dockTopLeft = dock_widget->pos();
    dockRect = QRect(dockTopLeft, dock_widget->frameSize());
    // Are we too Tall
    if (dockRect.y() + dockRect.height() > screenRect.y() + screenRect.height())
    {
      // See if we can move it more on screen so that the entire dock is on screen
      if (screenRect.y() + screenRect.height() - dockRect.height() > screenRect.y() )
      {
        // Move dock up
        dockRect.setY(screenRect.y() + screenRect.height() - dockRect.height() );
        dock_widget->move(dockRect.x(), dockRect.y());
        dockTopLeft = dock_widget->pos();
        dockRect = QRect(dockTopLeft, dock_widget->frameSize());
      }
      else
      {
        // Move dock up + resize to fit
        dock_widget->resize(dockRect.width(), screenRect.height() - titleBarHeight);
        dock_widget->move(dockRect.x(), screenRect.y());
        dockTopLeft = dock_widget->pos();
        dockRect = QRect(dockTopLeft, dock_widget->frameSize());
       }
    }
  }

}

