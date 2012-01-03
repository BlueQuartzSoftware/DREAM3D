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
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QSpacerItem>
#include <QtGui/QPushButton>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "FilterWidgets/QDream3DFilterManager.h"


#include "DREAM3DLib/ReconstructionFilters/LoadSlices.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void addFilter(QDream3DFilterManager::Pointer fm,
               const std::string &groupName,
               const std::string &filterName,
               QVBoxLayout* vertLayout)
{
  QFilterWidget* filterWidget = fm->getFilterWidget(groupName, filterName);
  if (filterWidget != NULL)
  {
    vertLayout->addWidget(filterWidget);
  }
}


int main(int argc, char **argv)
{

  QApplication app(argc, argv);

  QDream3DFilterManager::Pointer fm = QDream3DFilterManager::New();


  QWidget* top = new QWidget;
  delete top->layout();
  QVBoxLayout* vertLayout = new QVBoxLayout(top);

  addFilter(fm, DREAM3D::FilterGroups::ReconstructionFilters, LoadSlices::ClassName(), vertLayout);
  addFilter(fm, DREAM3D::FilterGroups::ReconstructionFilters, AlignSections::ClassName(), vertLayout);
  addFilter(fm, DREAM3D::FilterGroups::ReconstructionFilters, SegmentGrains::ClassName(), vertLayout);
  addFilter(fm, DREAM3D::FilterGroups::ReconstructionFilters, CleanupGrains::ClassName(), vertLayout);



  QPushButton* button = new QPushButton("Start");
  QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  vertLayout->addSpacerItem(verticalSpacer);
  vertLayout->addWidget(button);

 // QGraphicsScene scene;
 // QGraphicsProxyWidget *proxy = scene.addWidget(top);

 // QGraphicsView view(&scene);
 // view.show();
  top->show();
  return app.exec();

}


