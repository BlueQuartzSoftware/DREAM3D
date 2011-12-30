/*
 * FilterManagerTest.cpp
 *
 *  Created on: Dec 30, 2011
 *      Author: mjackson
 */


#include "QtGui/QApplication"
#include "QtGui/QWidget"
#include "QtGui/QGraphicsView"
#include "QtGui/QGraphicsScene"
#include "QtGui/QGraphicsProxyWidget"



#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/ReconstructionFilters/CleanupGrains.h"
#include "FilterWidgets/QDream3DFilterManager.h"



int main(int argc, char **argv)
{

  QApplication app(argc, argv);

  QDream3DFilterManager::Pointer fm = QDream3DFilterManager::New();



  QWidget *tabWidget = fm->getFilterWidget("Reconstruction", "CleanupGrains");
  if (tabWidget == NULL)
  {
    std::cout << "Returned Widget was NULL." << std::endl;
    return EXIT_FAILURE;
  }

  QGraphicsScene scene;
  QGraphicsProxyWidget *proxy = scene.addWidget(tabWidget);

  QGraphicsView view(&scene);
  view.show();

  return app.exec();

}


