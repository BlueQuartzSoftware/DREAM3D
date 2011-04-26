/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "RepresentationUI.h"
#include "QtSupport/QRecentFileList.h"

/**
 * @brief The Main entry point for the application
 */
int main (int argc, char *argv[])
{
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("RepresentationUI");
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

  RepresentationUI *viewer = new RepresentationUI;
  viewer->show();
  int app_return = app.exec();

  QRecentFileList::instance()->writeList(prefs);

  return app_return;
}

#if 0

#include "ReconstructionWidget.h"
#include "OIMImportWidget.h"
//-- Qt Headers
#include <QtGui/QApplication>
#include <QtGui/QGraphicsProxyWidget>
#include <QtGui/QGraphicsScene>
#include <QtCore/QStateMachine>
#include <QtCore/QAbstractTransition>
#include <QtCore>
#include <QtGui>

int main_alt(int argc, char **argv)
{
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("RepresentationUI");
#if defined( Q_WS_MAC )
  //Needed for typical Mac program behavior.
  app.setQuitOnLastWindowClosed( true );
#endif //APPLE


  OIMImportWidget* oimWidget = new OIMImportWidget;
  ReconstructionWidget* reconWidget = new ReconstructionWidget;

  QGraphicsProxyWidget* oimProxy = new QGraphicsProxyWidget;
  oimProxy->setWidget(oimWidget);
  QGraphicsProxyWidget* reconProxy = new QGraphicsProxyWidget;
  reconProxy->setWidget(reconWidget);

  QPushButton *button1 = new QPushButton("OIM Import");
  QGraphicsProxyWidget *button1Proxy = new QGraphicsProxyWidget;
  button1Proxy->setWidget(button1);

  QPushButton *button = new QPushButton("Reconstruction");
  QGraphicsProxyWidget *buttonProxy = new QGraphicsProxyWidget;
  buttonProxy->setWidget(button);



  QStateMachine machine;
  QState *state1 = new QState(&machine);
  QState *state2 = new QState(&machine);

  machine.setInitialState(state1);

//  state1->assignProperty(button, "text", "Reconstruction");
//  state1->assignProperty(button, "pos", QPoint(0,0));
  state1->assignProperty(oimProxy, "geometry", QRect(0,40, 629,528));
  state1->assignProperty(reconProxy, "geometry", QRect(700, 40, 670, 630));
  state1->assignProperty(oimProxy, "opacity", qreal(1));
  state1->assignProperty(reconProxy, "opacity", qreal(0));
  state1->assignProperty(button, "enable", false);
  state1->assignProperty(button1, "enable", true);

//  state2->assignProperty(button, "text", "OIM Import");
//  state2->assignProperty(button, "pos", QPoint(0,0));
  state2->assignProperty(oimProxy, "geometry", QRect(-700,40, 629,528));
  state2->assignProperty(reconProxy, "geometry", QRect(0, 40, 670, 630));
  state2->assignProperty(oimProxy, "opacity", qreal(0));
  state2->assignProperty(reconProxy, "opacity", qreal(1));
  state2->assignProperty(button, "enable", true);
  state2->assignProperty(button1, "enable", false);

  QAbstractTransition *t1 = state1->addTransition(button, SIGNAL(clicked()), state2);
//  QSequentialAnimationGroup *animation1SubGroup = new QSequentialAnimationGroup;
//  animation1SubGroup->addPause(250);
//  animation1SubGroup->addAnimation(new QPropertyAnimation(reconProxy, "geometry"));
//  t1->addAnimation(animation1SubGroup);

  QParallelAnimationGroup* pAnimation = new QParallelAnimationGroup;
  pAnimation->addAnimation(new QPropertyAnimation(oimProxy, "geometry"));
  pAnimation->addAnimation(new QPropertyAnimation(reconProxy, "geometry"));
  t1->addAnimation(pAnimation);

  QAbstractTransition *t2 = state2->addTransition(button1, SIGNAL(clicked()), state1);
  t2->addAnimation(new QPropertyAnimation(oimProxy, "geometry"));
  t2->addAnimation(new QPropertyAnimation(reconProxy, "geometry"));

  machine.start();


  QMainWindow* MainWindow = new QMainWindow;
#if 0
  MainWindow->setCentralWidget(graphicsView);
#else

  QGraphicsWidget *widget = new QGraphicsWidget;
  QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal, widget);
  layout->addItem(buttonProxy);
  layout->addItem(button1Proxy);
  widget->setLayout(layout);

  QGraphicsScene scene(0, 0, 800, 800);
  scene.setBackgroundBrush(scene.palette().window());
  scene.addItem(widget);
  scene.addItem(oimProxy);
  scene.addItem(reconProxy);

  QWidget *centralwidget;
  QVBoxLayout *verticalLayout;
  QGraphicsView *graphicsView;

  QMenuBar *menubar;
  QStatusBar *statusbar;

  MainWindow->resize(658, 699);
  centralwidget = new QWidget(MainWindow);
  centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
  verticalLayout = new QVBoxLayout(centralwidget);
  verticalLayout->setSpacing(2);
  verticalLayout->setContentsMargins(4, 4, 4, 4);
  verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
  graphicsView = new QGraphicsView(centralwidget);
  graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(graphicsView->sizePolicy().hasHeightForWidth());
  graphicsView->setSizePolicy(sizePolicy);
  graphicsView->setFrameShape(QFrame::Box);
  graphicsView->setFrameShadow(QFrame::Plain);
  graphicsView->setScene(&scene);
  verticalLayout->addWidget(graphicsView);

  MainWindow->setCentralWidget(centralwidget);
  menubar = new QMenuBar(MainWindow);
  menubar->setObjectName(QString::fromUtf8("menubar"));
  menubar->setGeometry(QRect(0, 0, 658, 22));
  MainWindow->setMenuBar(menubar);
  statusbar = new QStatusBar(MainWindow);
  statusbar->setObjectName(QString::fromUtf8("statusbar"));
  MainWindow->setStatusBar(statusbar);
#endif

  MainWindow->show();
  return app.exec();
}

#endif
