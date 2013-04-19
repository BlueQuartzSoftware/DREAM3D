#include "QHDF5TableModel.h"

#include "H5Support/H5Utilities.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDF5TableModel::QHDF5TableModel(  QObject* parent) :
  QAbstractTableModel(parent),
  _fileId (-1),
  _datasetPath(""),
  _rows(0),
  _columns(0) // Always at least 1 column unless no data
{
  _data = NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDF5TableModel::~QHDF5TableModel()
{
  if (_data != NULL) { delete _data; }
  //std::cout << "DESTRUCTING QHDF5TableModel" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t QHDF5TableModel::setHDFProperties(hid_t fileId, const std::string &datasetPath)
{
  this->_fileId = fileId;
  this->_datasetPath = datasetPath;
  if (this->_initializeInfo() < 0)  //Get the data set from the data file
  {
    //std::cout << "ERROR: QHDF5TableModel::setHDFProperties Could not initianlize data set" << std::endl;
    return -1;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDF5TableModel::resetModel()
{
  _rows = 0;
  _columns = 0;
  reset();
  if ( NULL != _data) { delete _data; _data = NULL;}
  emit layoutChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t QHDF5TableModel::_initializeInfo()
{
  herr_t err = -1;
  herr_t retErr = 1;
  if ( _fileId > 0) // Good File ID
  {
    H5O_info_t statbuf;
    err = H5Oget_info_by_name(this->_fileId, _datasetPath.c_str(),  &statbuf, H5P_DEFAULT);
    if ( err < 0  ) // Dataset does not exist
    {
      _rows = 0;
      _columns = 0;
      retErr = -1;
      reset();
      emit layoutChanged();
    }
    else if ( H5Utilities::isGroup(_fileId, _datasetPath) == false) // Not a Group
    {
      std::vector<hsize_t> dims;
      H5T_class_t ds_type;
      size_t type_size;
      err = H5Lite::getDatasetInfo(_fileId, _datasetPath, dims, ds_type, type_size);
      if (err < 0)
      {
        std::cout << "ERROR: Could not get dataset Info for " << _datasetPath << std::endl;
        return -1;
      }

      _rows = static_cast<int32_t>(dims[0]);
      _columns = 1;
      if ( dims.size() == 2) // Is this at least a 2D data set
      {
        _columns = static_cast<int32_t>(dims[1]);
      }
      if ( dims.size() > 2 )  // More than 2 dimensions
      {
        _columns = static_cast<int32_t>(dims[1]);
        std::cout << "WARNING: Data set has more than 2 Dimensions. Only showing first 2. Data Set Path: " << _datasetPath << std::endl;
      }
      //Get the data from the file
      hid_t typeId = 0;
      typeId = H5Lite::getDatasetType(_fileId, _datasetPath);
      if (typeId < 0)
      {
        std::cout << "QHDF5TableModel::_initializeInfo  ERROR: Unable to retrieve type info for " << _datasetPath << " typeId=" << typeId << std::endl;
        return -1;
      }
       std::string res;
        switch(ds_type)
        {
        case H5T_STRING:
          err = H5Lite::readStringDataset(_fileId, _datasetPath, res);
          //TODO: Do somethign with the data
          break;
        case H5T_INTEGER:
          //std::cout << "User Meta Data Type is Integer" << std::endl;
          if ( H5Tequal(typeId, H5T_STD_U8BE) || H5Tequal(typeId,H5T_STD_U8LE) ) {
            _setDataArray<quint8>(dims);
          } else if ( H5Tequal(typeId, H5T_STD_U16BE) || H5Tequal(typeId,H5T_STD_U16LE) ) {
            _setDataArray<quint16>(dims);
          } else if ( H5Tequal(typeId, H5T_STD_U32BE) || H5Tequal(typeId,H5T_STD_U32LE) ) {
            _setDataArray<quint32>(dims);
          } else if ( H5Tequal(typeId, H5T_STD_U64BE) || H5Tequal(typeId,H5T_STD_U64LE) ) {
            _setDataArray<quint64>(dims);
          } else if ( H5Tequal(typeId, H5T_STD_I8BE) || H5Tequal(typeId,H5T_STD_I8LE) ) {
            _setDataArray<qint8>(dims);
          } else if ( H5Tequal(typeId, H5T_STD_I16BE) || H5Tequal(typeId,H5T_STD_I16LE) ) {
            _setDataArray<qint16>(dims);
          } else if ( H5Tequal(typeId, H5T_STD_I32BE) || H5Tequal(typeId,H5T_STD_I32LE) ) {
            _setDataArray<qint32>(dims);
          } else if ( H5Tequal(typeId, H5T_STD_I64BE) || H5Tequal(typeId,H5T_STD_I64LE) ) {
            _setDataArray<qint64>(dims);
          } else {
            std::cout << "Unknown Type: " << typeId << " at " <<  _datasetPath << std::endl;
            err = -1;
          }
          break;
        case H5T_FLOAT:
          if (type_size == 4) {
            _setDataArray<float32>(dims);
          } else if (type_size == 8 ) {
            _setDataArray<float64>(dims);
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
        reset();
        emit layoutChanged();
    }
    else
    {
      _rows = 0;
      _columns = 0;
      retErr = -1;
      reset();
      emit layoutChanged();
    }
  }
  else //Bad File Id
  {
    _rows = 0;
    _columns = 0;
    retErr = -1;
    reset();
    emit layoutChanged();
  }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QHDF5TableModel::rowCount(const QModelIndex &index) const
{
  return _rows;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QHDF5TableModel::columnCount(const QModelIndex &index) const
{
  return _columns;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant QHDF5TableModel::data(const QModelIndex &index, int32_t role) const
{
  if (this->_fileId < 0)
  {
    return QVariant();
  }
  if (! index.isValid())
  {
    return QVariant();
  }

  if (role == Qt::TextAlignmentRole) {
    return int(Qt::AlignRight | Qt::AlignVCenter);
  }
  else if (role == Qt::DisplayRole)
  {
    if ( _data == NULL) {
      return QVariant();
    }
    return _data->dataAt(index.row() , index.column() );
  }

  return QVariant();
}
