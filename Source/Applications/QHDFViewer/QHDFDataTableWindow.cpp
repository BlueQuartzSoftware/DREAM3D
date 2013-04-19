#include "QHDFDataTableWindow.h"
//--
#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"


#include "QHDF5TableModel.h"

#include <QtCore/QFileInfo>
#include <QtGui/QCloseEvent>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDFDataTableWindow::QHDFDataTableWindow(const QString &fileName, hid_t fileId, const QString &hdfPath,QWidget *parent) :
  IH5DataWindow(parent),
  _filePath(fileName),
  _fileId(fileId),
  _hdfPath(hdfPath),
  _hdfDataTableModel(NULL)
{
  setupUi(this);
  init();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDFDataTableWindow::~QHDFDataTableWindow()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFDataTableWindow::init()
{
  setAttribute(Qt::WA_DeleteOnClose);
  QFileInfo fi(this->_filePath);
  this->setWindowTitle( fi.fileName() + "::" + this->_hdfPath );

  _hdfDataTableModel = new QHDF5TableModel(this);
  this->hdfDataTableView->setModel(_hdfDataTableModel);
  _hdfDataTableModel->setHDFProperties(this->_fileId, this->_hdfPath.toStdString() );
  // setupMenuBar();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFDataTableWindow::closeEvent(QCloseEvent *event)
{
 //std::cout << "QHDFDataTableWindow::closeEvent(QCloseEvent *event)" << std::endl;
 this->close();
 event->accept();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFDataTableWindow::focusInEvent(QFocusEvent* event)
{
 // std::cout << "QHDFDataTableWindow::focusInEvent(QFocusEvent* event)" << std::endl;
  //TODO: Implement this
  QMainWindow::focusInEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFDataTableWindow::focusOutEvent(QFocusEvent* event)
{
//  std::cout << "QHDFDataTableWindow::focusOutEvent(QFocusEvent* event)" << std::endl;
  //TODO: Implement this
  QMainWindow::focusOutEvent(event);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QHDFDataTableWindow::hdfPath()
{
  return this->_hdfPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hid_t QHDFDataTableWindow::fileId()
{
  return this->_fileId;
}

