///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _HDF_GRAPHICS_DELEGATE_H_
#define _HDF_GRAPHICS_DELEGATE_H_

// HDF5 Includes
#include "hdf5.h"
//-- STL Includes
#include <string>
//-- Qt Includes
#include <QtCore/QObject>
//-- QMXADataViewer includes
#include "QHDF5TableModel.h"







// --- Forward declarations of Classes ----
class QMainWindow;
class QGraphicsItem;
class QGraphicsView;
class QGraphicsScene;
class QTableView;
class QTableWidget;

/**
 * @class HDFGraphicsDelegate HDFGraphicsDelegate.h HDFGraphicsDelegate.h
 * @brief A Delegate class that renders HDF5 Data to various outputs
 * @author Mike Jackson
 * @date May 29, 2007
 * @version $Revision: 1.5 $
 */
class HDFGraphicsDelegate : public QObject
{
  Q_OBJECT;


  public:
  HDFGraphicsDelegate(QObject* parent = 0);
  virtual ~HDFGraphicsDelegate();

  /**
   * @brief Sets the QMainWindow object
   * @param mainWindow The QMainWindow Object
   */
  void setQMainWindow(QMainWindow* mainWindow)
  { this->_mainWindow = mainWindow;}

  /**
   * @brief Sets the QGraphicsView object
   * @param gv The QGraphicsView object
   */
  void setQGraphicsView(QGraphicsView* gv)
  { this->_graphicsView = gv;}

  /**
   * @brief Returns the QGraphicsView that is being used by this class.
   * @return
   */
  QGraphicsView* graphicsView()
  { return this->_graphicsView; }

  /**
   * @brief Sets the QGraphicsScene Object
   * @param gs The QGraphicsScene Object
   */
  void setQGraphicsScene(QGraphicsScene* gs)
  { this->_graphicsScene = gs;}


  QImage* cachedImage()
  { return this->_cachedImage; }

  /**
   * @brief Sets the HDFRawData TableView pointer
   * @param tv A valied QTableView pointer
   */
  void setRawDataTableView(QTableView* tv);

  /**
   * @brief Returns the QHDF5TableModel used in this class.
   * @return
   */
  QHDF5TableModel* rawDataTableModel();

  /**
   * @brief Resets the RawDataTableMocel that is used in this class.
   */
  void resetTableModel();

  /**
   * @brief Sets the zoom value. This is expressed in a floating point number
   * where 1.0 is 100% or NO ZOOM and -1.0 represents fit to current window.
   * @param zoomFactor The value of the zoom Factor
   */
  void setZoomFactor(double zoomFactor);

  void increaseZoom();

  void decreaseZoom();

  void fitToWindow();

  void updateImage();

  /**
   * @brief Displays a Text message in the graphics view. This is typically used
   * when there is an error. You should NOT use this to display general String
   * data. Use an QHDFStringDataWindow instead
   * @param message The message to display
   */
  void displayTextMessage(QString message);

  /**
   * @brief sets all cached values to NULL or empty
   */
  void resetCaches();

  /**
   * @brief Displays HDFData as either a table of values or an Image
   * @param fileId A Valid HDF5 File or group ID
   * @param datasetPath The path to dataset to be viewed
   * @param attributesTable The Table of attribute values to be updated
   * @return Error Condition. Error >= 0 is success
   */
  herr_t displayHDFData(hid_t fileId, const std::string &datasetPath );

 private:
  QGraphicsView*    _graphicsView;
  QGraphicsScene*   _graphicsScene;
  QMainWindow*      _mainWindow;
  QGraphicsItem*    _currentGraphicsItem;
  QTableView*       _rawDataTableView;
  QHDF5TableModel*  _rawDataTableModel;
  double            _zoomFactor;
  QImage*           _cachedImage;
  std::string       _currentDatasetPath;
  double            _zoomFactors[10];
  int               _zoomIndex;

  /**
   * @brief Dislays the dataset as an Image
   * @param fileId An HDF File or Group ID
   * @param datasetPath The relative path to the dataset based on the fileId from abive
   * @return HDF error condition
   */
  herr_t _displayHDFImage(hid_t fileId, const std::string &datasetPath);

  /**
   * @brief Displays a dataset as a grayscale image
   * @param fileId An HDF File or Group ID
   * @param datasetPath The relative path to the dataset based on the fileId from above
   * @param width The width in pixels of the image
   * @param height The height in pixels of the image
   * @return HDF error condition
   */
  herr_t _displayHDFGrayscaleImage(hid_t fileId, const std::string &datasetPath, hsize_t width, hsize_t height);

  /**
   * @brief Displays a dataset as a true color image
   * @param fileId An HDF File or Group ID
   * @param datasetPath The relative path to the dataset based on the fileId from above
   * @param width The width in pixels of the image
   * @param height The height in pixels of the image
   * @return HDF error condition
   */
  herr_t _displayHDFTrueColorImage(hid_t fileId, const std::string &datasetPath, hsize_t width, hsize_t height, hsize_t planes);

  /**
   * @brief Converts a data set into data that is appropriate for an image
   * @param fileId An HDF File or Group ID
   * @param datasetPath The relative path to the dataset based on the fileId from above
   * @param withNormalization Normalize the data between 0 and 255
   * @return HDF error condition
   */
  herr_t _convertToImage(hid_t fileId, const std::string &datasetPath, bool withNormalization);

  /**
   * @brief Displays a QPixmap object in the QGraphicsScene
   * @param imagePixmap
   */
  void _displayPixmap(QPixmap imagePixmap);

  /**
   * @brief Displays a Text message in the QGraphicsScene
   * @param message The message to display
   */
  void _displayTextMessage(QString message);

  /**
   * @brief Scales the image by a _zoomFactor
   * @return Returns a QImage that is scaled copy of the cached image
   */
  QImage _scaleImage();

  HDFGraphicsDelegate(const HDFGraphicsDelegate&); //Copy Constructor Not Implemented
  void operator=(const HDFGraphicsDelegate&); //Copy Assignment Not Implemented
};

#endif /*_HDF_GRAPHICS_DELEGATE_H_*/
