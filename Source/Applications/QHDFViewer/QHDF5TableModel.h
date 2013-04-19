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
#ifndef QHDF5TABLEMODEL_H_
#define QHDF5TABLEMODEL_H_

//-- HDF5 Includes
#include <hdf5.h>

//-- STL Includes
#include <string>

//--
#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

//-- Qt Includes
#include <QtCore/QAbstractTableModel>
#include <QtCore/QVarLengthArray>

#include "IQHDF5Dataset.h"
#include "QHDF5_2DDataset.h"



/**
 * @class QHDF5TableModel QHDF5TableModel.h QHDF5TableModel.h
 * @brief Model class in the MVC arch. This model stores data from an HDF5 data
 * file by reading the entire dataset from the file into memory. This means that
 * if you read large datasets you may run out of ram or make the application
 * run slowly due to swapping of memory to the Hard drive.
 * @author Mike Jackson
 * @date May 29, 2007
 * @version $Revision: 1.3 $
 */
class QHDF5TableModel : public QAbstractTableModel
{
  Q_OBJECT;

public:
  /**
   * @brief Constructor
   * @param parent The Parent QObject for this object
   */
  QHDF5TableModel( QObject* parent = 0);
  virtual ~QHDF5TableModel();

  /**
   * @brief Sets the HDF5 File properties for this Model. If you want to reuse
   * this object then simply call this method each time you would like to model
   * a new dataset.
   * @param fileId A valid HDF5 File Id
   * @param datasetPath The relative path to the dataset based on the fileId argument
   * @return Standard HDF error condition
   */
  herr_t setHDFProperties(hid_t fileId, const std::string &datasetPath);

  /**
   * @brief Effectively clears the model by setting the number of rows and colums
   * to Zero and emitting the LayoutChanged Signal.
   */
  void resetModel();

  // -----------------------------------------------------------------------------
  //  QAbstractTableModel minimal implementation
  // -----------------------------------------------------------------------------
  int32_t rowCount(const QModelIndex &) const;

  int32_t columnCount(const QModelIndex &) const;

  QVariant data(const QModelIndex &index, int role) const;


private:
  hid_t _fileId;
  std::string _datasetPath;
  int32_t _rows;
  int32_t _columns;
  IQHDF5Dataset* _data;

  /**
   * @brief Initializes the basic information about this model
   */
  herr_t _initializeInfo();

/**
 * @brief Reads the data from the HDF5 Datafile into memory
 * @param dims The dimensions of the data set which should have a rank of 2
 */
  template<typename T>
  void _setDataArray(std::vector<hsize_t> &dims)
  {
   // std::cout << "QHDF5TableModel::_setDataArray" << std::endl;
    herr_t err = -1;

    QHDF5_2DDataset<T>* hdf2DDataset = new QHDF5_2DDataset<T>;
    hdf2DDataset->setDimensions(dims);
    std::vector<T> &rawData = hdf2DDataset->getDataRef();
    err = H5Lite::readVectorDataset(_fileId, _datasetPath, rawData);
    if ( err < 0) {
      std::cout << "ERROR: QHDF5TableModel::_setDataArray Could not read data from HDF5 File." << std::endl;
      return;
    }

    if (NULL != _data) {
    //  std::cout << "QHDF5TableModel: Deleting Previous Dataset" << std::endl;
      delete _data;
      _data = NULL;
    }
    //_data->setParent(this);
    _data = dynamic_cast<IQHDF5Dataset*>(hdf2DDataset);
   // std::cout << "_data: " << _data << std::endl;
  }

  QHDF5TableModel(const QHDF5TableModel&); //Copy Constructor Not Implemented
  void operator=(const QHDF5TableModel&); //Copy Assignment Not Implemented
};

#endif /*QHDF5TABLEMODEL_H_*/
