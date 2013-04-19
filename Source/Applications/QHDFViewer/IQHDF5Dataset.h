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
#ifndef IQHDF5DATASET_H_
#define IQHDF5DATASET_H_

//-- HDF5 Includes
#include <hdf5.h>

//-- STL Includes
#include <vector>

//-- Qt Includes
#include <QtCore/QObject>
#include <QtCore/QVariant>

/**
 * @class IQHDF5Dataset IQHDF5Dataset.h   IQHDF5Dataset.h
 * @brief Base class for storing 2D HDF datasets. This is designed to be used in
 * conjunction with the QHDF5TableModel to wrap 2D data structures
 * @author Mike Jackson BlueQuartz Software
 * @date May 29, 2007
 * @version $Revision: 1.2 $
 */
class IQHDF5Dataset : public QObject
{
  Q_OBJECT;
  
  public:
  IQHDF5Dataset(QObject* parent = 0) {};
  virtual ~IQHDF5Dataset() {};
  
  /**
   * @brief Returns the data at a given row and column
   * @param row The row index (Zero Based)
   * @param column The colum index (Zero Based)
   * @return The value as a QVariant object.
   */
  virtual QVariant dataAt(qint32 row, qint32 column) = 0;
  
};


#endif /*IQHDF5DATASET_H_*/
