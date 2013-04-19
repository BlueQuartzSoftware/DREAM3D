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
#ifndef _QHDF5Dataset_h_
#define _QHDF5Dataset_h_

//-- STL Includes
#include <iostream>

//-- Qt Includes
#include <QtCore/QObject>
#include <QtCore/QVariant>

#include "IQHDF5Dataset.h"

/**
 * @class QHDF5_2DDataset QHDF5_2DDataset.h   QHDF5_2DDataset.h
 * @brief  Concrete implementation of IQHDF5Dataset to store 2D HDF5 data sets. 
 * 
 * This class is a template class and is parameterized on the type of primitive.
 * @author mjackson
 * @date May 29, 2007
 * @version $Revision: 1.2 $
 */
template<typename T>
class QHDF5_2DDataset : public IQHDF5Dataset
{
  public:
    
    QHDF5_2DDataset() :
    _rows(1),
    _cols(1)
    {};
    
    virtual ~QHDF5_2DDataset() {};

    /**
     * @brief Returns a reference to the internal data structure that holds the data
     */
    std::vector<T>& getDataRef()
    {
      return _data;
    }
      
    /**
     * @brief sets the dimensions of the data set
     * @param dims
     */
    void setDimensions(std::vector<hsize_t> dims)
    {
      if (dims.size() == 0)
      {
        std::cout << "Error: Dimensions were empty" << std::endl;
        return;
      }
      _rows = static_cast<qint32>(dims[0]);
      if (dims.size() >= 2)
      {
        _cols = static_cast<qint32>(dims[1]);
      } else {
        _cols = 1;
      }
    }
    
    /**
     * @brief Returns the data at a given location
     * @param row The row/column values to retrieve data at
     * @param column
     * @return The data at the given location
     */
    QVariant dataAt(qint32 row, qint32 column)
    {
      qint32 data_index = (row * _cols) + column;
      return QVariant( _data[data_index] );
    }

  private:
    
    std::vector<T> _data; // Holds the data
    qint32 _rows;
    qint32 _cols;

};

#endif //_QHDF5Dataset_h_
