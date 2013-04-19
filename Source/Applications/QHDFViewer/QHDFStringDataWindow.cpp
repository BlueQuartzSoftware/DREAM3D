#include "QHDFStringDataWindow.h"
//--
#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"
#include "QHDF5TableModel.h"

#include <QtCore/QByteArray>
#include <QtCore/QFileInfo>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDFStringDataWindow::QHDFStringDataWindow(const QString &fileName, hid_t fileId, const QString &hdfPath,QWidget *parent) :
  IH5DataWindow(parent),
  _filePath(fileName),
  _fileId(fileId),
  _hdfPath(hdfPath)
{
  setupUi(this);
  init();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDFStringDataWindow::~QHDFStringDataWindow()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFStringDataWindow::init()
{
  setAttribute(Qt::WA_DeleteOnClose);

  // Set the Window Title to the file name
  QFileInfo fi(this->_filePath);
  this->setWindowTitle( fi.fileName() + "::" + this->_hdfPath );

  QByteArray data;

  hid_t fileId = this->_fileId;
  if (fileId < 0)
  {
    return;
  }
  herr_t err = -1;
  H5T_class_t attr_type;
  size_t attr_size;
  std::string res;

  std::vector<hsize_t> dims;
  err = H5Lite::getDatasetInfo(fileId, this->_hdfPath.toStdString(), dims, attr_type, attr_size);
  if (err < 0)
  {
    return;
  }
  int size = dims[0];

  if (size > 0) // NOT an empty String
  {
    data.resize( size );
    uint8_t* dest = (uint8_t*)(data.data());
    if (NULL != dest)
    {
      err = H5Lite::readStringDataset(fileId, this->_hdfPath.toStdString(), dest);
      if (err >= 0)
      {
        this->stringData->setText( QString(data.constData() ) );
      }
      else
      {
        std::cout << "Error reading data set " << this->_hdfPath.toStdString() << std::endl;
      }
    }

  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFStringDataWindow::closeEvent(QCloseEvent *event)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFStringDataWindow::focusInEvent(QFocusEvent* event)
{
  //TODO: Implement this
  QMainWindow::focusInEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFStringDataWindow::focusOutEvent(QFocusEvent* event)
{
  //TODO: Implement this
  QMainWindow::focusOutEvent(event);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QHDFStringDataWindow::hdfPath()
{
  return this->_hdfPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hid_t QHDFStringDataWindow::fileId()
{
  return this->_fileId;
}

