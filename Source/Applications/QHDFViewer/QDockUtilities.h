///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef QDOCKUTILITIES_H_
#define QDOCKUTILITIES_H_

class QDockWidget;

/**
* @class QDockUtilities QDockUtilities.h Shared/QDockUtilities.h
* @brief Collection of utilities that perform various checks on a QDockWidget
* @author Mike Jackson @ BlueQuartz Software
* @date Oct 2008
* @version $Revision: 1.2 $
*/
class QDockUtilities
{
  public:

    /**
     * @brief Checks to make sure any floating window is within the bounds of all
     * the screens.
     * @param dock_widget The QDockWidget to check
     */
    static void sanityCheckDock(QDockWidget* dock_widget);

  protected:
    QDockUtilities();
    virtual ~QDockUtilities();


  private:
    QDockUtilities(const QDockUtilities&);    // Copy Constructor Not Implemented
      void operator=(const QDockUtilities&);  // Operator '=' Not Implemented
};

#endif /* QDOCKUTILITIES_H_ */
