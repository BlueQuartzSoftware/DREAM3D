///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#include "HDFGraphicsDelegate.h"

//-- STL includes
#include <iostream>

//-- Qt Includes
#include <QtGui/QMainWindow>
#include <QtGui/QTableWidget>
#include <QtGui/QPixmap>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QHeaderView>
#include <QtGui/QGraphicsPixmapItem>

//-- MXA Includes
#include "MXA/MXA.h"
#include <MXA/Common/LogTime.h>
#include <MXA/Common/MXATypeDefs.h>


#include "H5Support/H5Lite.h"
#include "H5Support/H5Image.h"
#include "H5Support/H5Utilities.h"

//--MXA Tools Includes
#include "IQPixmapGenerator.h"
#include "NormalizedPixmapGenerator.h"


#define ZOOM_INDEX_MAX 9
#define ZOOM_INDEX_MIN 0

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HDFGraphicsDelegate::HDFGraphicsDelegate(QObject* parent) :
  QObject(parent),
  _zoomFactor(1.0),
  _cachedImage(NULL)
{
  _graphicsScene = NULL;
  _graphicsView = NULL;
  _mainWindow = NULL;
  _currentGraphicsItem = NULL;
  _rawDataTableView = NULL;
  _rawDataTableModel = new QHDF5TableModel(this);
  _zoomFactors[0] = 0.05;
  _zoomFactors[1] = 0.1;
  _zoomFactors[2] = 0.25;
  _zoomFactors[3] = 0.50;
  _zoomFactors[4] = 1.0;
  _zoomFactors[5] = 1.5;
  _zoomFactors[6] = 2.0;
  _zoomFactors[7] = 4.0;
  _zoomFactors[8] = 6.0;
  _zoomFactors[9] = -1.0;
  _zoomIndex = 4;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HDFGraphicsDelegate::~HDFGraphicsDelegate()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HDFGraphicsDelegate::resetCaches()
{
  this->_currentDatasetPath = "";
  if (NULL != this->_cachedImage)
  {
    delete this->_cachedImage; // Delete the current Cached Image
  }
  this->_cachedImage = NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HDFGraphicsDelegate::displayTextMessage(QString message)
{
  _displayTextMessage( message );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HDFGraphicsDelegate::resetTableModel()
{
  _rawDataTableModel->resetModel(); // Clear the Raw data table
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDF5TableModel* HDFGraphicsDelegate::rawDataTableModel()
{
  return _rawDataTableModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t HDFGraphicsDelegate::displayHDFData(hid_t fileId, const std::string &datasetPath)
{
  //std::cout << "HDFGraphicsDelegate::displayHDFData: " << datasetPath << std::endl;
  if (datasetPath.compare(this->_currentDatasetPath) == 0)
  {
    this->updateImage();
    return 1;
  }

  herr_t err = 0;
  if (fileId < 0)
  {
    return fileId;
  }

  this->_currentDatasetPath = datasetPath;
  std::string attrValClass;
  err = H5Lite::readStringAttribute(fileId, datasetPath, MXA::H5Image::ImageClass, attrValClass);

  if (err >= 0 && ( attrValClass.compare(MXA::H5Image::Image) == 0) ) // The dataset is H5IM formatted Data
  {
    err = _displayHDFImage(fileId, datasetPath);
  }
  else if ( (H5Lite::readStringAttribute(fileId, datasetPath, RenderHint::RenderHint, attrValClass) ) >= 0)
  {
    // Not a 'real' image data set but the hint is telling us we can render as an image
    //TODO: Add UI widget to toggle withEqualization
    if (this->_graphicsView->isVisible() )
    {
      bool withNormalization = true; // Future feature
      err = _convertToImage(fileId, datasetPath, withNormalization);
    }
  }
  else  // The data is something that we don't recognize
  {
    _displayTextMessage(QString("Data Format is NOT image Type.\nPlease select the 'Raw Data' Tab to view the data"));
   // std::cout << "Data Format NOT Recognized for Display" << std::endl;
   // return -1;
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t HDFGraphicsDelegate::_convertToImage(hid_t fileId, const std::string &datasetPath, bool withNormalization)
{
  // std::cout << "HDFGraphicsDelegate::_convertToImage" << std::endl;
  // Get the Data
  herr_t retErr = 1;
  herr_t err = 1;
  H5T_class_t ds_type;
  size_t type_size;
  std::vector<hsize_t> dims;
  err = H5Lite::getDatasetInfo(fileId, datasetPath, dims, ds_type, type_size);
  if (err < 0)
  {
    std::cout << DEBUG_OUT(logTime)  << "Error Getting datasetInfo." << std::endl;
    return err;
  }
  hid_t typeId = 0;
  typeId = H5Lite::getDatasetType(fileId, datasetPath);
  if (typeId < 0)
  {
    std::cout << DEBUG_OUT( logTime ) << "HDFGraphicsDelegate::_convertToImage ERROR: Unable to retrieve type info for " << datasetPath << std::endl;
    return -1;
  }

  IQPixmapGenerator* pixmapGenerator = NULL;

  switch(ds_type)
  {
    case H5T_STRING:
      err = -1;
      pixmapGenerator = NULL;
      std::cout << DEBUG_OUT(logTime) << "Attempting to change string data into image data" << std::endl;
      break;
    case H5T_INTEGER:
      //std::cout << "User Meta Data Type is Integer" << std::endl;
      if ( H5Tequal(typeId, H5T_STD_U8BE) || H5Tequal(typeId,H5T_STD_U8LE) ) {
        pixmapGenerator = new NormalizedPixmapGenerator<uint8_t>(fileId, datasetPath, dims);
      } else if ( H5Tequal(typeId, H5T_STD_U16BE) || H5Tequal(typeId,H5T_STD_U16LE) ) {
        pixmapGenerator = new NormalizedPixmapGenerator<uint16_t>(fileId, datasetPath, dims);
      } else if ( H5Tequal(typeId, H5T_STD_U32BE) || H5Tequal(typeId,H5T_STD_U32LE) ) {
        pixmapGenerator = new NormalizedPixmapGenerator<uint32_t>(fileId, datasetPath, dims);
      } else if ( H5Tequal(typeId, H5T_STD_U64BE) || H5Tequal(typeId,H5T_STD_U64LE) ) {
        pixmapGenerator = new NormalizedPixmapGenerator<uint64_t>(fileId, datasetPath, dims);
      } else if ( H5Tequal(typeId, H5T_STD_I8BE) || H5Tequal(typeId,H5T_STD_I8LE) ) {
        pixmapGenerator = new NormalizedPixmapGenerator<int8_t>(fileId, datasetPath, dims);
      } else if ( H5Tequal(typeId, H5T_STD_I16BE) || H5Tequal(typeId,H5T_STD_I16LE) ) {
        pixmapGenerator = new NormalizedPixmapGenerator<int16_t>(fileId, datasetPath, dims);
      } else if ( H5Tequal(typeId, H5T_STD_I32BE) || H5Tequal(typeId,H5T_STD_I32LE) ) {
        pixmapGenerator = new NormalizedPixmapGenerator<int32_t>(fileId, datasetPath, dims);
      } else if ( H5Tequal(typeId, H5T_STD_I64BE) || H5Tequal(typeId,H5T_STD_I64LE) ) {
        pixmapGenerator = new NormalizedPixmapGenerator<int64_t>(fileId, datasetPath, dims);
      } else {
        std::cout << DEBUG_OUT(logTime) << "Unknown Type: " << typeId << " at " <<  datasetPath << std::endl;
        err = -1;
      }
      break;
    case H5T_FLOAT:
      if (type_size == 4) {
        pixmapGenerator = new NormalizedPixmapGenerator<float32>(fileId, datasetPath, dims);
      } else if (type_size == 8 ) {
        pixmapGenerator = new NormalizedPixmapGenerator<float64>(fileId, datasetPath, dims);
      } else {
        std::cout << "Unknown Floating point type" << std::endl;
        err = -1;
      }
      break;
    default:
      std::cout << "Error: readUserMetaData() Unknown attribute type: " << ds_type << std::endl;
      H5Utilities::printHDFClassType(ds_type);
  }
  CloseH5T(typeId, err, retErr); //Close the H5A type Id that was retrieved during the loop

  //Check for errors and bail early if any
  if (retErr< 0 || err < 0 || NULL == pixmapGenerator )
  {
    return err;
  }

  // Display the data
  _displayPixmap( pixmapGenerator->generatePixmap() );
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t HDFGraphicsDelegate::_displayHDFImage(hid_t fileId, const std::string &datasetPath)
{
  // std::cout << "HDFGraphicsDelegate::_displayHDFImage" << std::endl;
  herr_t err = -1;
  std::string attrValClass, attrValSubclass, attrValVersion;
  err = H5Lite::readStringAttribute(fileId, datasetPath, MXA::H5Image::ImageClass, attrValClass);
  if (err < 0)
  {
    _displayTextMessage(QString("CLASS attribute not found "));
    std::cout << "Error: CLASS attribute not found for data set path " << datasetPath << std::endl;
    return err;
  }
  err = H5Lite::readStringAttribute(fileId, datasetPath, MXA::H5Image::ImageSubclass, attrValSubclass);
  if (err < 0)
  {
    _displayTextMessage(QString("IMAGE_SUBCLASS attribute not found "));
    std::cout << "Error: IMAGE_SUBCLASS attribute not found for data set path " << datasetPath << std::endl;
    return err;
  }
  err = H5Lite::readStringAttribute(fileId, datasetPath, MXA::H5Image::ImageVersion, attrValVersion);
  if (err < 0)
  {
    attrValVersion = "Not Found";
    //std::cout << "Error: IMAGE_VERSION attribute not found for data set path " << datasetPath << std::endl;
  }

  hsize_t width = 0;
  hsize_t height = 0;
  hsize_t planes = 0;
  std::string interlace;
  hssize_t numpallets = 0;
  err = H5Image::H5IMget_image_info(fileId, datasetPath.c_str(), &width, &height, &planes, interlace, &numpallets);
  if (err < 0)
  {
    _displayTextMessage(QString("Proper H5IM Attributes were NOT found"));
    std::cout << "ERROR reading image information from dataset " << datasetPath << " Err=" << err << std::endl;
    return err;
  }

//  std::cout << "Image Subclass: " << attrValSubclass << std::endl;
//  std::cout << "Image Version: " << attrValVersion << std::endl;
//  std::cout << "Image Info: W: " << width << " H: " << height<< " Planes: " << planes
  // << " Interlace: " << interlace.toStdString()
  // << " NumPallets: " << numpallets << std::endl;

  if (attrValSubclass == MXA::H5Image::ImageGrayScale)
  {
    //std::cout << "Grayscale image" << std::endl;
    err = _displayHDFGrayscaleImage(fileId, datasetPath, width, height);
  }
  else if (attrValSubclass == MXA::H5Image::ImageIndexed)
  {
    std::cout << "Indexed Image Not supported at this time" << std::endl;
    //err = displayHDFPaletteImage(fileId, nodePath, width, height, numpallets);
  }
  else if (attrValSubclass.compare (MXA::H5Image::ImageTrueColor) == 0)
  {
    err = _displayHDFTrueColorImage(fileId, datasetPath, width, height, planes);
  }
  else
  {
    std::cout << "Error displaying HDF Image - unkonwn subclass: " << attrValSubclass << std::endl;
    return -1;
  }

  //std::cout << "------------ displayHDFImage ----------------\n" << std::endl;
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t HDFGraphicsDelegate::_displayHDFTrueColorImage(hid_t fileId, const std::string &datasetPath, hsize_t width, hsize_t height, hsize_t planes)
{
  std::cout << "HDFGraphicsDelegate::_displayHDFTrueColorImage" << std::endl;
  herr_t err = 0;
  hsize_t size = width * height * planes;
  hsize_t nPixels = width * height;
  //unsigned char data[size];
  std::vector<unsigned char> data(size);
  err = H5Image::H5IMread_image(fileId, datasetPath.c_str(), &(data.front() ) );

  if (err < 0) {
    _displayTextMessage(QString("ERROR Reading Image Data"));
    std::cout << "ERROR Reading Image Data" << std::endl;
    return err;
  }

  if (NULL != this->_cachedImage)
  {
    delete this->_cachedImage; // Delete the current Cached Image
  }
  this->_cachedImage = new QImage(width, height, QImage::Format_RGB32);
  unsigned char* imageBits = this->_cachedImage->bits();
  unsigned char* src = &(data.front());
  ::memset(imageBits, 0xff, (width*height*(planes+1) ) ); // Splat all the alpha values through the array
  for (hsize_t i = 0; i < nPixels; ++i )
  {
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    imageBits[0] = src[2];
    imageBits[1] = src[1];
    imageBits[2] = src[0];

#else
    imageBits[1] = src[0];
    imageBits[2] = src[1];
    imageBits[3] = src[2];
#endif
    src+=3; //Advance 3 bytes
    imageBits+=4; //Advance 4 bytes
  }

  QImage dataImage = _scaleImage();

  QPixmap imagePixmap(QPixmap::fromImage(dataImage));

  _displayPixmap(imagePixmap);

  return 1;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t HDFGraphicsDelegate::_displayHDFGrayscaleImage(hid_t fileId, const std::string &datasetPath, hsize_t width, hsize_t height)
{
 // std::cout << "HDFGraphicsDelegate::_displayHDFGrayscaleImage" << std::endl;
  herr_t err = 0;
  hsize_t size = width * height;
  //unsigned char data[size];
  std::vector<unsigned char> data(size);
  err = H5Image::H5IMread_image(fileId, datasetPath.c_str(), &(data.front() ) );

  if (err < 0) {
    _displayTextMessage(QString("ERROR Reading Image Data"));
    std::cout << "ERROR Reading Image Data" << std::endl;
    return err;
  }

  if (NULL != this->_cachedImage)
  {
    delete this->_cachedImage; // Delete the current Cached Image
  }
  this->_cachedImage = new QImage(width, height, QImage::Format_RGB32);
  int index;
  uint32_t gval;
  QRgb value;

  for (int j=0; j<(int)height; j++) {
    for (int i =0; i<(int)width; i++) {
      index = (j * (int) width) + i;
      gval = (uint32_t) data[index];
      value = qRgb(gval, gval, gval);
      this->_cachedImage->setPixel(i, j, value);
    }
  }

  QImage dataImage = _scaleImage();
  QPixmap imagePixmap(QPixmap::fromImage(dataImage));

  _displayPixmap(imagePixmap);

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HDFGraphicsDelegate::increaseZoom()
{
  if (_zoomIndex < ZOOM_INDEX_MAX - 1)
  {
    _zoomIndex++;
  }
  this->_zoomFactor =_zoomFactors[_zoomIndex];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HDFGraphicsDelegate::decreaseZoom()
{
  if (_zoomIndex > ZOOM_INDEX_MIN)
  {
    _zoomIndex--;
  }
  this->_zoomFactor =_zoomFactors[_zoomIndex];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HDFGraphicsDelegate::fitToWindow()
{
  _zoomIndex = ZOOM_INDEX_MAX;
  this->setZoomFactor(_zoomFactors[_zoomIndex]);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HDFGraphicsDelegate::setZoomFactor(double zoomFactor)
{
  this->_zoomFactor = zoomFactor;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage HDFGraphicsDelegate::_scaleImage()
{
  QSize imageSize = this->_cachedImage->size();
  std::cout << "imageSize (W x H) :" << imageSize.width() << " x " << imageSize.height() << std::endl;
  if (_zoomFactor > -1.0)
  {
    imageSize *= _zoomFactor;
    return this->_cachedImage->scaled(imageSize, Qt::KeepAspectRatio );
  }
  std::cout << "imageSize (W x H) :" << imageSize.width() << " x " << imageSize.height() << std::endl;
  int gvWidth = _graphicsView->size().width();
  int gvHeight = _graphicsView->size().height();
  std::cout << "_graphicsView (W x H) :" << gvWidth << " x " << gvHeight << std::endl;
  gvWidth -= 4;
  gvHeight -= 4;

  if (imageSize.width() > imageSize.height() )
  {
    double zf = (double)(gvWidth)/(double)(imageSize.width());

    for (int i = 0; i < ZOOM_INDEX_MAX - 1; ++i )
    {
      if (zf < this->_zoomFactors[i] && i > 0)
      {
        this->_zoomIndex = i -1;
        this->_zoomFactor = this->_zoomFactors[this->_zoomIndex ];
        break;
      }
    }
    return  this->_cachedImage->scaledToHeight(gvHeight);
  }

  double zf = (double)(gvHeight)/(double)(imageSize.height());

  for (int i = 0; i < ZOOM_INDEX_MAX - 1; ++i )
  {
    if (zf < this->_zoomFactors[i] && i > 0)
    {
      this->_zoomIndex = i -1;
      this->_zoomFactor = this->_zoomFactors[this->_zoomIndex ];
      break;
    }
  }
  return this->_cachedImage->scaledToWidth(gvWidth);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HDFGraphicsDelegate::updateImage()
{

  if (NULL == this->_cachedImage)
  {
    return;
  }
  QImage dataImage = _scaleImage();
  QPixmap imagePixmap(QPixmap::fromImage(dataImage));
  _displayPixmap(imagePixmap);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HDFGraphicsDelegate::_displayPixmap(QPixmap imagePixmap)
{
//  std::cout << "HDFGraphicsDelegate::_displayPixmap" << std::endl;
  if ( imagePixmap.isNull() )
  {
    return;
  }
  if ( NULL != _currentGraphicsItem )
  {
    _graphicsScene->removeItem(_currentGraphicsItem); //Remove the image that is displaying
    _currentGraphicsItem->setParentItem(NULL); // Set the parent to NULL
    delete _currentGraphicsItem; // Delete the object
  }

  _currentGraphicsItem = _graphicsScene->addPixmap(imagePixmap); // Add the new image into the display

  QRectF rect = _currentGraphicsItem->boundingRect();
  _graphicsScene->setSceneRect(rect);
  _graphicsView->setScene(_graphicsScene);
  _graphicsView->centerOn(_currentGraphicsItem);

  _graphicsScene->update(rect);

//  std::cout << "Graphics Scene Item Count: " << _graphicsScene->items().size() << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HDFGraphicsDelegate::_displayTextMessage(QString message)
{
  if ( NULL != _currentGraphicsItem )
  {
    _graphicsScene->removeItem(_currentGraphicsItem); //Remove the image that is displaying
    _currentGraphicsItem->setParentItem(NULL); // Set the parent to NULL
    delete _currentGraphicsItem; // Delete the object
  }
  _currentGraphicsItem = _graphicsScene->addText(message); // Add the new image into the display

  QRectF rect = _currentGraphicsItem->boundingRect();
  _graphicsScene->setSceneRect(rect);
  _graphicsView->setScene(_graphicsScene);
  _graphicsView->centerOn(_currentGraphicsItem);
  _graphicsScene->update(rect);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HDFGraphicsDelegate::setRawDataTableView(QTableView* tv)
  {
    if ( _rawDataTableView != tv)
    {
      if (NULL != _rawDataTableView )
      {
        this->_rawDataTableView->setModel(NULL); // remove our table model from the current Table View
      }
      this->_rawDataTableView = tv;
      if (NULL != _rawDataTableView)
      {
        _rawDataTableView->setModel(_rawDataTableModel);
      }
    }
  }
