
#include "ImageDataToolbar.h"

#include <QtGui/QMainWindow>
#include <QtGui/QToolBar>
#include <QtGui/QAction>


ImageDataToolbar::ImageDataToolbar(QMainWindow* parent) :
  QObject(parent)
{
  // TODO Auto-generated constructor stub
  initToolbar(parent);
}

ImageDataToolbar::~ImageDataToolbar()
{
  // TODO Auto-generated destructor stub
}


void ImageDataToolbar::initToolbar(QMainWindow* parent)
{
  QToolBar* tb = new QToolBar(parent);

  QAction* zoomIn = new QAction(QIcon(":zoom_in.png"), tr("Zoom In"), parent);
  connect(zoomIn, SIGNAL(triggered()), parent, SLOT(zoomIn() ));

  QAction* zoomOut = new QAction(QIcon(":zoom_out.png"), tr("Zoom Out"), parent);
  connect(zoomOut, SIGNAL(triggered()), parent, SLOT(zoomOut() ));

  QAction* fitToWindow = new QAction(QIcon(":fitToWindow.png"), tr("Fit To Window"), parent);
  connect(fitToWindow, SIGNAL(triggered()), parent, SLOT(fitToWindow() ));


  tb->addAction(zoomIn);
  tb->addAction(zoomOut);
  tb->addAction(fitToWindow);
  parent->addToolBar(tb);
}
