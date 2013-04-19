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
#ifndef NORMALIZEDPIXMAPGENERATOR_H_
#define NORMALIZEDPIXMAPGENERATOR_H_

//-- MXA Includes
#include <MXA/MXATypes.h>
#include <MXA/Common/LogTime.h>
#include "H5Support/H5Lite.h"

//-- MXA Viewer Includes
#include "IQPixmapGenerator.h"

//-- Qt Includes
#include <QtGui/QImage>
#include <QtGui/QPixmap>

//-- C++ Includes
#include <limits>

/**
 * @brief Normalizes a PixMap
 * @class NormalizedPixmapGenerator NormalizedPixmapGenerator.h Shared/NormalizedPixmapGenerator.h
 * @author Mike Jackson for BlueQuartz Software.us
 * @date 2007
 * @version $Revision: 1.4 $
 */
template<typename T>
class NormalizedPixmapGenerator : public IQPixmapGenerator
{


public:
  NormalizedPixmapGenerator(hid_t locId, const std::string &datasetPath, std::vector<hsize_t> dims, QObject* parent = 0) :
    IQPixmapGenerator(parent)
   {
     _locId = locId;
     _datasetPath = datasetPath;
     _dims = dims;
   }

  virtual ~NormalizedPixmapGenerator() {}

  QPixmap generatePixmap()
  {
    // Get the Data from the HDF5 File
    std::vector<T> data;
    herr_t err = H5Lite::readVectorDataset( _locId, _datasetPath, data);
    if ( err < 0 )
    {
      std::cout << DEBUG_OUT(logTime) << "" << std::endl;
      return QPixmap();
    }

    T min = std::numeric_limits<T>::max();
    T max = std::numeric_limits<T>::min();
    for (typename std::vector<T>::iterator iter = data.begin(); iter != data.end(); ++iter )
    {
      if ( *iter < min ) min = *iter;
      if ( *iter > max ) max = *iter;
    }
    //std::cout << logTime() << "Min/Max " << min << " " << max << std::endl;
    // Filter the data in place
    T scaleFactor = static_cast<T>(1.0f/(max - min) * 255.0f);
    T val;
    for (typename std::vector<T>::iterator iter = data.begin(); iter != data.end(); ++iter )
    {
      val = *iter;
      val = (val - min) * (scaleFactor);
      *iter = val;
    }

    // Create a QPixMap* and return the pointer
    qint32 width = _dims[0];
    qint32 height = _dims[1];
    QImage dataImage(width, height , QImage::Format_RGB32);

    qint32 index;
    uint32_t gval;
    QRgb value;

    for (qint32 j=0; j<height; j++) {
      for (qint32 i =0; i<width; i++) {
        index = (j *  width) + i;
        gval = static_cast<uint32_t>( data[index]);
        value = qRgb(gval, gval, gval);
        dataImage.setPixel(i, j, value);
      }
    }

    return QPixmap::fromImage(dataImage);
  }

private:
  hid_t _locId;
  std::string _datasetPath;
  std::vector<hsize_t> _dims;
};



#endif /*NORMALIZEDPIXMAPGENERATOR_H_*/
