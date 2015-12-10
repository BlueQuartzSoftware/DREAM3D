/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <H5Support/QH5Lite.h>

#include <string.h>
#include <string>

#if defined (H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif

// -----------------------------------------------------------------------------
//  Protected Constructor
// -----------------------------------------------------------------------------
QH5Lite::QH5Lite()
{
}

// -----------------------------------------------------------------------------
//  Protected Destructor
// -----------------------------------------------------------------------------
QH5Lite::~QH5Lite()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QH5Lite::disableErrorHandlers()
{
  HDF_ERROR_HANDLER_OFF;
}

// -----------------------------------------------------------------------------
//  Opens an ID for HDF5 operations
// -----------------------------------------------------------------------------
herr_t QH5Lite::openId( hid_t loc_id, const QString& obj_name, H5O_type_t obj_type)
{
  return H5Lite::openId(loc_id, obj_name.toStdString(), obj_type);

}

// -----------------------------------------------------------------------------
//  Closes the given ID
// -----------------------------------------------------------------------------
herr_t QH5Lite::closeId( hid_t obj_id, int32_t obj_type )
{
  return H5Lite::closeId(obj_id, obj_type);
}

// -----------------------------------------------------------------------------
//  Finds an Attribute given an object to look in
// -----------------------------------------------------------------------------
herr_t QH5Lite::findAttribute( hid_t loc_id, const QString& attrName )
{
  return H5Lite::findAttribute(loc_id, attrName.toStdString());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QH5Lite::datasetExists( hid_t loc_id, const QString& name )
{
  return H5Lite::datasetExists(loc_id, name.toStdString());
}

// -----------------------------------------------------------------------------
//  We assume a null terminated string
// -----------------------------------------------------------------------------
herr_t  QH5Lite::writeStringDataset (hid_t loc_id,
                                     const QString& dsetName,
                                     size_t size,
                                     const char* data)
{
  return H5Lite::writeStringDataset(loc_id, dsetName.toStdString(), size, data);
}

// -----------------------------------------------------------------------------
//  Writes a string to a HDF5 dataset
// -----------------------------------------------------------------------------
herr_t QH5Lite::writeStringDataset(hid_t loc_id, const QString& dsetName, const QString& data)
{
  return H5Lite::writeStringDataset(loc_id, dsetName.toStdString(), data.toStdString());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t QH5Lite::writeVectorOfStringsDataset(hid_t loc_id,
                                            const QString& dsetName,
                                            const QVector<QString>& data)
{
  hid_t sid = -1;
  hid_t memspace = -1;
  hid_t datatype = -1;
  hid_t did = -1;
  herr_t err = -1;
  herr_t retErr = 0;

  hsize_t  dims[1] = { static_cast<hsize_t>(data.size()) };
  if ( (sid = H5Screate_simple(sizeof(dims) / sizeof(*dims), dims, NULL)) >= 0)
  {
    dims[0] = 1;

    if( (memspace = H5Screate_simple(sizeof(dims) / sizeof(*dims), dims, NULL) ) >= 0)
    {

      datatype = H5Tcopy(H5T_C_S1);
      H5Tset_size(datatype, H5T_VARIABLE);

      if ( (did = H5Dcreate(loc_id, dsetName.toLocal8Bit().constData(), datatype, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) >= 0)
      {

        //
        // Select the "memory" to be written out - just 1 record.
        hsize_t offset_[] = { 0 };
        hsize_t count_[] = { 1 };
        H5Sselect_hyperslab(memspace, H5S_SELECT_SET, offset_, NULL, count_, NULL);
        hsize_t m_pos = 0;
        for (qint32 i = 0; i < data.size(); i++)
        {
          // Select the file position, 1 record at position 'pos'
          hsize_t count[] = { 1 };
          hsize_t offset[] = { m_pos++ };
          H5Sselect_hyperslab(sid, H5S_SELECT_SET, offset, NULL, count, NULL);
          std::string v = data[i].toStdString(); // MUST be a C String, i.e., null terminated
          const char* s = v.c_str();
          err = H5Dwrite(did, datatype, memspace, sid, H5P_DEFAULT, s);
          if (err < 0 )
          {
            qDebug() << "Error Writing String Data: " __FILE__ << "(" << __LINE__ << ")";
            retErr = err;
          }
        }
        QCloseH5D(did, err, retErr);
      }
      H5Tclose(datatype);
      QCloseH5S(memspace, err, retErr);
    }

    QCloseH5S(sid, err, retErr);

  }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t QH5Lite::writeStringAttributes(hid_t loc_id,
                                      const QString& objName,
                                      const QMap<QString, QString>& attributes)
{
  herr_t err = 0;
  QMapIterator<QString, QString> i(attributes);
  while (i.hasNext())
  {
    i.next();
    err = H5Lite::writeStringAttribute(loc_id, objName.toStdString(), i.key().toStdString(), i.value().toStdString());
    if (err < 0)
    {
      return err;
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hsize_t QH5Lite::getNumberOfElements(hid_t loc_id, const QString& dsetName)
{
  return H5Lite::getNumberOfElements(loc_id, dsetName.toStdString());
}

// -----------------------------------------------------------------------------
//  Writes a string to an HDF5 Attribute
// -----------------------------------------------------------------------------
herr_t  QH5Lite::writeStringAttribute(hid_t loc_id,
                                      const QString& objName,
                                      const QString& attrName,
                                      hsize_t size,
                                      const char* data)
{
  return H5Lite::writeStringAttribute(loc_id, objName.toStdString(), attrName.toStdString(), size, data);
}


// -----------------------------------------------------------------------------
//  Writes a string to an HDF5 Attribute
// -----------------------------------------------------------------------------
herr_t QH5Lite::writeStringAttribute(hid_t loc_id,
                                     const QString& objName,
                                     const QString& attrName,
                                     const QString& data )
{
  return H5Lite::writeStringAttribute(loc_id, objName.toStdString(), attrName.toStdString(), data.size() + 1, data.toLatin1().data() );
}

// -----------------------------------------------------------------------------
//  Reads a String dataset into a QString
// -----------------------------------------------------------------------------
herr_t QH5Lite::readStringDataset(hid_t loc_id, const QString& dsetName, QString& data)
{
  std::string readValue;
  herr_t err = H5Lite::readStringDataset(loc_id, dsetName.toStdString(), readValue);
  data = QString::fromStdString(readValue);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t QH5Lite::readStringDataset(hid_t loc_id,
                                  const QString& dsetName,
                                  char* data)
{
  return H5Lite::readStringDataset(loc_id, dsetName.toStdString(), data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t QH5Lite::readVectorOfStringDataset(hid_t loc_id,
                                          const QString& dsetName,
                                          QVector<QString>& data)
{

  hid_t did; // dataset id
  hid_t tid; // type id
  herr_t err = 0;
  herr_t retErr = 0;


  did = H5Dopen(loc_id, dsetName.toLocal8Bit().constData(), H5P_DEFAULT);
  if (did < 0)
  {
    qDebug() << "H5Lite.cpp::readVectorOfStringDataset(" << __LINE__ << ") Error opening Dataset at loc_id (" << loc_id << ") with object name (" << dsetName << ")";
    return -1;
  }
  /*
  * Get the datatype.
  */
  tid = H5Dget_type(did);
  if ( tid >= 0 )
  {
    hsize_t dims[1] = { 0 };
    /*
    * Get dataspace and allocate memory for read buffer.
    */
    hid_t sid = H5Dget_space(did);
    int ndims = H5Sget_simple_extent_dims(sid, dims, NULL);
    if(ndims != 1)
    {
      CloseH5S(sid, err, retErr);
      CloseH5T(tid, err, retErr);
      std::cout << "H5Lite.cpp::readVectorOfStringDataset(" << __LINE__ << ") Number of dims should be 1 but it was " << ndims << ". Returning early. Is your data file correct?" << std::endl;
      return -2;
    }
    std::vector<char*> rdata(dims[0]);
    for (int i = 0; i < dims[0]; i++)
    {
      rdata[i] = NULL;
    }

    /*
    * Create the memory datatype.
    */
    hid_t memtype = H5Tcopy(H5T_C_S1);
    herr_t status = H5Tset_size(memtype, H5T_VARIABLE);

    /*
    * Read the data.
    */
    status = H5Dread(did, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, &(rdata.front()));
    if(status < 0)
    {
      status = H5Dvlen_reclaim(memtype, sid, H5P_DEFAULT, &(rdata.front()));
      CloseH5S(sid, err, retErr);
      CloseH5T(tid, err, retErr);
      CloseH5T(memtype, err, retErr);
      std::cout << "H5Lite.cpp::readVectorOfStringDataset(" << __LINE__ << ") Error reading Dataset at loc_id (" << loc_id << ") with object name (" << dsetName.toStdString() << ")" << std::endl;
      return -3;
    }
    data.resize(dims[0]);
    /*
    * copy the data into the vector of strings
    */
    for (int i = 0; i < dims[0]; i++)
    {
      // printf("%s[%d]: %s\n", "VlenStrings", i, rdata[i].p);
      QString str = QString::fromLatin1(rdata[i]);
      data[i] = str;
    }
    /*
    * Close and release resources.  Note that H5Dvlen_reclaim works
    * for variable-length strings as well as variable-length arrays.
    * Also note that we must still free the array of pointers stored
    * in rdata, as H5Tvlen_reclaim only frees the data these point to.
    */
    status = H5Dvlen_reclaim(memtype, sid, H5P_DEFAULT, &(rdata.front()));
    QCloseH5S(sid, err, retErr);
    QCloseH5T(tid, err, retErr);
    QCloseH5T(memtype, err, retErr);
  }

  QCloseH5D(did, err, retErr);


  return retErr;
}

// -----------------------------------------------------------------------------
//  Reads a string Attribute from the HDF file
// -----------------------------------------------------------------------------
herr_t QH5Lite::readStringAttribute(hid_t loc_id, const QString& objName, const QString& attrName,
                                    QString& data)
{
  std::string sValue;
  herr_t err = H5Lite::readStringAttribute(loc_id, objName.toStdString(), attrName.toStdString(), sValue);
  data = QString::fromStdString(sValue);
  return err;
}

// -----------------------------------------------------------------------------
//  Reads a string Attribute from the HDF file
// -----------------------------------------------------------------------------
herr_t QH5Lite::readStringAttribute(hid_t loc_id,
                                    const QString& objName,
                                    const QString& attrName,
                                    char* data)
{
  return H5Lite::readStringAttribute(loc_id, objName.toStdString(), attrName.toStdString(), data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t QH5Lite::getDatasetNDims( hid_t loc_id, const QString& dsetName, hid_t& rank)
{
  return H5Lite::getDatasetNDims(loc_id, dsetName.toStdString(), rank);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hid_t QH5Lite::getAttributeNDims(hid_t loc_id, const QString& objName, const QString& attrName, hid_t& rank)
{
  return H5Lite::getAttributeNDims(loc_id, objName.toStdString(), attrName.toStdString(), rank);
}


// -----------------------------------------------------------------------------
//  Returns the type of data stored in the dataset. You MUST use H5Tclose(tid)
//  on the returned value or resource leaks will occur.
// -----------------------------------------------------------------------------
hid_t QH5Lite::getDatasetType(hid_t loc_id, const QString& dsetName)
{
  return H5Lite::getDatasetType(loc_id, dsetName.toStdString());
}

// -----------------------------------------------------------------------------
//  Get the dataset information
// -----------------------------------------------------------------------------
herr_t QH5Lite::getDatasetInfo( hid_t loc_id,
                                const QString& dsetName,
                                QVector<hsize_t>& dims,
                                H5T_class_t& classType,
                                size_t& sizeType )
{
  // Since this is a wrapper we need to pass a std::vector() then copy the values from that into our 'dims' argument
  std::vector<hsize_t> rDims;
  herr_t err = H5Lite::getDatasetInfo(loc_id, dsetName.toStdString(), rDims, classType, sizeType);
  dims.resize(static_cast<qint32>(rDims.size()));
  for(std::vector<hsize_t>::size_type i = 0; i < rDims.size(); ++i)
  {
    dims[static_cast<qint32>(i)] = rDims[i];
  }
  return err;
}


// -----------------------------------------------------------------------------
//  You must close the attributeType argument or resource leaks will occur. Use
//  H5Tclose(tid); after your call to this method if you do not need the id for
//   anything.
// -----------------------------------------------------------------------------
herr_t QH5Lite::getAttributeInfo(hid_t loc_id,
                                 const QString& objName,
                                 const QString& attrName,
                                 QVector<hsize_t>& dims,
                                 H5T_class_t& type_class,
                                 size_t& type_size,
                                 hid_t& tid)
{
  std::vector<hsize_t> rDims = dims.toStdVector();
  herr_t err = H5Lite::getAttributeInfo(loc_id, objName.toStdString(), attrName.toStdString(), rDims,
                                        type_class, type_size, tid);
  dims.resize(static_cast<qint32>(rDims.size()));
  for(std::vector<hsize_t>::size_type i = 0; i < rDims.size(); ++i)
  {
    dims[static_cast<qint32>(i)] = rDims[i];
  }
  return err;
}


