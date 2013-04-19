#include "QHDFImageDataWindow.h"
//--
#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"
#include "QHDF5TableModel.h"

#include <QtCore/QByteArray>
#include <QtGui/QImage>
#include <QtCore/QFileInfo>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDFImageDataWindow::QHDFImageDataWindow(const QString &fileName, hid_t fileId, const QString &hdfPath,QWidget *parent) :
  IH5DataWindow(parent),
  _filePath(fileName),
  _fileId(fileId),
  _hdfPath(hdfPath),
  _hdfGraphicsScene(NULL),
  _hdfGraphicsDelegate(NULL)
{
  setupUi(this);
  init();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDFImageDataWindow::~QHDFImageDataWindow()
{
  //std::cout << "~QHDFImageDataWindow" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFImageDataWindow::init()
{
  setAttribute(Qt::WA_DeleteOnClose);

  // Set the Window Title to the file name
  QFileInfo fi(this->_filePath);
  this->setWindowTitle( fi.fileName() + "::" + this->_hdfPath );

  this->resize(800, 600);

  _hdfGraphicsScene = new QGraphicsScene(this);
  connect(_hdfGraphicsScene, SIGNAL(sceneRectChanged ( const QRectF & ) ),
          this, SLOT(sceneRectChanged ( const QRectF & ) ));

  hdfGraphicsView->setScene(_hdfGraphicsScene);
  _hdfGraphicsDelegate = new HDFGraphicsDelegate(this);
  _hdfGraphicsDelegate->setQGraphicsView(hdfGraphicsView);
  _hdfGraphicsDelegate->setQGraphicsScene(_hdfGraphicsScene);
  _hdfGraphicsDelegate->setQMainWindow(this);

  _hdfGraphicsDelegate->displayHDFData(this->_fileId, this->_hdfPath.toStdString());
  _hdfGraphicsDelegate->fitToWindow();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFImageDataWindow::closeEvent(QCloseEvent *event)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFImageDataWindow::focusInEvent(QFocusEvent* event)
{
  //TODO: Implement this
  QMainWindow::focusInEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFImageDataWindow::focusOutEvent(QFocusEvent* event)
{
  //TODO: Implement this
  QMainWindow::focusOutEvent(event);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFImageDataWindow::sceneRectChanged(const QRectF &rect)
{
 // std::cout << "QMXADataViewer::sceneRectChanged rect: " << rect.width() << " " << rect.height() << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QHDFImageDataWindow::hdfPath()
{
  return this->_hdfPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hid_t QHDFImageDataWindow::fileId()
{
  return this->_fileId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFImageDataWindow::zoomIn()
{
  _hdfGraphicsDelegate->increaseZoom();
  _hdfGraphicsDelegate->updateImage(); // Update the Image
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFImageDataWindow::zoomOut()
{
  _hdfGraphicsDelegate->decreaseZoom();
   _hdfGraphicsDelegate->updateImage(); // Update the Image
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFImageDataWindow::fitToWindow()
{
  _hdfGraphicsDelegate->fitToWindow();
   _hdfGraphicsDelegate->updateImage(); // Update the Image
}
