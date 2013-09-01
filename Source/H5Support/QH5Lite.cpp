/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
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
herr_t QH5Lite::writeStringDataset(hid_t loc_id, const QString& dsetName, const QString &data)
{
  return H5Lite::writeStringDataset(loc_id, dsetName.toStdString(), data.toStdString());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t QH5Lite::writeStringAttributes(hid_t loc_id,
                                      const QString &objName,
                                      const QMap<QString, QString> &attributes)
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
  return H5Lite::writeStringAttribute(loc_id, objName.toStdString(), attrName.toStdString(), data.size()+ 1, data.toAscii().data() );
}

// -----------------------------------------------------------------------------
//  Reads a String dataset into a QString
// -----------------------------------------------------------------------------
herr_t QH5Lite::readStringDataset(hid_t loc_id, const QString& dsetName, QString &data)
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
                                  const QString &dsetName,
                                  char *data)
{
  return H5Lite::readStringDataset(loc_id, dsetName.toStdString(), data);
}



// -----------------------------------------------------------------------------
//  Reads a string Attribute from the HDF file
// -----------------------------------------------------------------------------
herr_t QH5Lite::readStringAttribute(hid_t loc_id, const QString& objName, const QString& attrName,
                                    QString &data)
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
                                    char *data)
{
  return H5Lite::readStringAttribute(loc_id, objName.toStdString(), attrName.toStdString(), data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t QH5Lite::getDatasetNDims( hid_t loc_id, const QString& dsetName, hid_t &rank)
{
  return H5Lite::getDatasetNDims(loc_id, dsetName.toStdString(), rank);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hid_t QH5Lite::getAttributeNDims(hid_t loc_id, const QString& objName, const QString& attrName, hid_t &rank)
{
  return H5Lite::getAttributeNDims(loc_id, objName.toStdString(), attrName.toStdString(), rank);
}


// -----------------------------------------------------------------------------
//  Returns the type of data stored in the dataset. You MUST use H5Tclose(tid)
//  on the returned value or resource leaks will occur.
// -----------------------------------------------------------------------------
hid_t QH5Lite::getDatasetType(hid_t loc_id, const QString &dsetName)
{
  return H5Lite::getDatasetType(loc_id, dsetName.toStdString());
}

// -----------------------------------------------------------------------------
//  Get the dataset information
// -----------------------------------------------------------------------------
herr_t QH5Lite::getDatasetInfo( hid_t loc_id,
                                const QString& dsetName,
                                QVector<hsize_t> &dims,
                                H5T_class_t &classType,
                                size_t &sizeType )
{
  // Since this is a wrapper we need to pass a std::vector() then copy the values from that into our 'dims' argument
  std::vector<hsize_t> rDims;
  herr_t err = H5Lite::getDatasetInfo(loc_id, dsetName.toStdString(), rDims, classType, sizeType);
  dims.resize(rDims.size());
  for(std::vector<hsize_t>::size_type i = 0; i < rDims.size(); ++i)
  {
    dims[i] = rDims[i];
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
                                 QVector<hsize_t> &dims,
                                 H5T_class_t &type_class,
                                 size_t &type_size,
                                 hid_t &tid)
{
  std::vector<hsize_t> rDims = dims.toStdVector();
  herr_t err = H5Lite::getAttributeInfo(loc_id, objName.toStdString(), attrName.toStdString(), rDims,
                                         type_class, type_size, tid);
  dims.resize(rDims.size());
  for(std::vector<hsize_t>::size_type i = 0; i < rDims.size(); ++i)
  {
    dims[i] = rDims[i];
  }
  return err;
}


