

#include "DREAM3DFileStructure.h"


#include "H5Support/H5Support.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/QH5Utilities.h"
#include "H5Support/HDF5ScopedFileSentinel.h"

#include "DREAM3DLib/Common/Constants.h"

#define __SHOW_DEBUG_MSG__ true

typedef struct
{
    QString path;
    QString name;
    bool read;
    QVector<size_t> tupleDims;
    QVector<size_t> compDims;
    QString objectType;
    int version;
} DataArray_T;

class AttributeMatrix_T
{
  public:
    bool read;
    QString name;
    unsigned int amType;
    QMap<QString, DataArray_T> dataArrays;

};

class DataContainer_T
{
  public:
    bool read;
    QString name;
    unsigned int dcType;
    QMap<QString, AttributeMatrix_T> attributeMatricies;

};

typedef QMap<QString, DataContainer_T> DataContainerArray_T;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DFileStructure::DREAM3DFileStructure()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DFileStructure::~DREAM3DFileStructure()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void _readDataArrays(hid_t attrMatGid, AttributeMatrix_T &attrMatrix, QString h5InternalPath)
{
  QList<QString> dataArrayNames;
  QH5Utilities::getGroupObjects(attrMatGid, H5Utilities::H5Support_DATASET, dataArrayNames);
  foreach(QString dataArrayName, dataArrayNames)
  {
    if(__SHOW_DEBUG_MSG__) { std::cout << "        DataArray: " << dataArrayName.toStdString()  << std::endl; }

    DataArray_T proxy;
    proxy.path = h5InternalPath;
    proxy.name = dataArrayName;
    proxy.read = true;

    herr_t err = QH5Lite::readVectorAttribute(attrMatGid, dataArrayName, DREAM3D::HDF5::TupleDimensions, proxy.tupleDims);
    if(err < 0) { std::cout << "Error Reading the Tuple Dimensions for DataArray " << dataArrayName.toStdString() << std::endl; }

    err = QH5Lite::readVectorAttribute(attrMatGid, dataArrayName, DREAM3D::HDF5::ComponentDimensions, proxy.compDims);
    if(err < 0) { std::cout << "Error Reading the Component Dimensions for DataArray " << dataArrayName.toStdString() << std::endl; }

    err = QH5Lite::readScalarAttribute(attrMatGid, dataArrayName, DREAM3D::HDF5::DataArrayVersion, proxy.version);
    if(err < 0) { std::cout << "Error Reading the Version for DataArray " << dataArrayName.toStdString() << std::endl; }

    err = QH5Lite::readStringAttribute(attrMatGid, dataArrayName, DREAM3D::HDF5::ObjectType, proxy.objectType);
    if(err < 0) { std::cout << "Error Reading the Object Type for DataArray " << dataArrayName.toStdString() << std::endl; }

    attrMatrix.dataArrays.insert(dataArrayName, proxy);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void _readAttributeMatrix(hid_t containerId, DataContainer_T& dataContainer, QString h5InternalPath)
{
  QList<QString> attributeMatrixNames;
  QH5Utilities::getGroupObjects(containerId, H5Utilities::H5Support_GROUP, attributeMatrixNames);
  foreach(QString attributeMatrixName, attributeMatrixNames)
  {
    if(__SHOW_DEBUG_MSG__) { std::cout << "    AttributeMatrix: " << attributeMatrixName.toStdString()  << std::endl; }
    hid_t attrMatGid = H5Gopen(containerId, attributeMatrixName.toAscii().constData(), H5P_DEFAULT);
    if (attrMatGid < 0) { continue; }
    HDF5ScopedGroupSentinel sentinel(&attrMatGid, false);

    AttributeMatrix_T attrMatrix;
    attrMatrix.name = attributeMatrixName;
    attrMatrix.read = true;
    herr_t err = QH5Lite::readScalarAttribute(containerId, attributeMatrixName, DREAM3D::StringConstants::AttributeMatrixType, attrMatrix.amType);
    if(err < 0) { std::cout << "Error Reading the AttributeMatrix Type for AttributeMatrix " << attributeMatrixName.toStdString() << std::endl; }

    QString h5Path = h5InternalPath + "/" + attributeMatrixName;

    // Read in the names of the Data Arrays that make up the AttributeMatrix
    _readDataArrays(attrMatGid, attrMatrix, h5Path);

    // Insert the AttributeMatrix_T proxy into the dataContainer proxy
    dataContainer.attributeMatricies.insert(attributeMatrixName, attrMatrix);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void _readDataContainers(hid_t dcArrayGroupId, DataContainerArray_T &proxy, QString h5InternalPath)
{
  QList<QString> dataContainers;
  QH5Utilities::getGroupObjects(dcArrayGroupId, H5Utilities::H5Support_GROUP, dataContainers);
  foreach(QString dataContainerName, dataContainers)
  {
    if(__SHOW_DEBUG_MSG__) { std::cout << "Data Container:" << dataContainerName.toStdString() << std::endl; }
    hid_t containerGid = H5Gopen(dcArrayGroupId, dataContainerName.toAscii().constData(), H5P_DEFAULT);
    if (containerGid < 0) { continue; }
    HDF5ScopedGroupSentinel sentinel(&containerGid, false);
    DataContainer_T dataContainer;
    dataContainer.name = dataContainerName;
    dataContainer.read = true;
    herr_t err = QH5Lite::readScalarAttribute(dcArrayGroupId, dataContainerName, DREAM3D::StringConstants::DataContainerType, dataContainer.dcType);
    if(err < 0) { std::cout << "Error Reading the DataContainer Type for DataContainer " << dataContainerName.toStdString() << std::endl; }

    QString h5Path = h5InternalPath + "/" + dataContainerName;
    // Read the Attribute Matricies for this Data Container
    _readAttributeMatrix(containerGid, dataContainer, h5Path);

    // Insert the DataContainer_T proxy into the Proxy Map
    proxy.insert(dataContainerName, dataContainer);

  }


}

// -----------------------------------------------------------------------------
/*
 * This function shows creating a DataContainer from the "Proxy" data container
 * structure that has been generated by walking the file. This function could be
 * transferred to the DataContainerReader as a static function perhaps or used
 * elsewhere in the code. The GUI would generate the "Proxy" structure then pass
 * that Proxy Structure to the DataContainer reader as an DREAM3D_PROPERTY. If
 * the Proxy was NULL in the DataContainer then that would signal the DataContainerReader
 * to read EVERY thing from the file. If the Proxy was valid but essentially empty
 * then nothing is read from the file. If the Proxy has some structure saved in it
 * then the DataContainer reader would know exactly what to read.
 */
// -----------------------------------------------------------------------------
DataContainerArray::Pointer _createDataContainerArray(DataContainerArray_T &dcaProxy)
{
  DataContainerArray::Pointer dcArray = DataContainerArray::New();

  QMapIterator<QString, DataContainer_T> dcIter(dcaProxy);
  while (dcIter.hasNext()) // DataContainerLevel
  {
    dcIter.next();
    std::cout << "Found " << dcIter.key().toStdString() << ": " << std::endl;

    const DataContainer_T& dcProxy = dcIter.value();
    if(dcProxy.read == false) { continue; } // Skip to the next DataContainer if we are not reading this one.

    std::cout << "  reading " << dcIter.key().toStdString() << ": " << std::endl;
    // Create the real DataContainer Object and push it into the DataContainerArray object
    DataContainer::Pointer dataContainer = DataContainer::New();
    dataContainer->setName(dcIter.key());
    dcArray->pushBack(dataContainer);

    QMapIterator<QString, AttributeMatrix_T> amIter(dcProxy.attributeMatricies);
    while(amIter.hasNext()) // AttributeMatrixLevel
    {
      amIter.next();
      std::cout << "  Found " << amIter.key().toStdString() << ": " << std::endl;

      const AttributeMatrix_T& amProxy = amIter.value();
      if(amProxy.read == false) { continue; } // Skip to the next AttributeMatrix if not reading this one

      std::cout << "    reading " << amIter.key().toStdString() << std::endl;
      QVector<size_t> tupleDims(1,0);
      AttributeMatrix::Pointer attrMatrix = AttributeMatrix::New(tupleDims, amProxy.name, amProxy.amType);

      QMapIterator<QString, DataArray_T> dIter(amProxy.dataArrays);
      while(dIter.hasNext()) // DataArray Level
      {
        dIter.next();
        std::cout << "    Found " << dIter.key().toStdString() << ": " << std::endl;

        const DataArray_T& daProxy = dIter.value();
        if(daProxy.read == false) { continue; } // Skip to the next DataArray if not reading this one

        std::cout << "      reading " << dIter.key().toStdString() << std::endl;

        if(daProxy.objectType.compare("DataArray<int8_t>") == 0 ){
          // attrMatrix->createAndAddAttributeArray /* If we did this then the array is going to get allocated and that may be a LOT of RAM to use for nothing. */
          /* We can not use AttributeMatrix->addAttributeArray() because that will validate the number of Tuples matches
           * the number of tuples in the DataArray. If we set everything to Zero sizes then we loose the meta data of the
           * tuple dimensions at this point which we will need later when we finally read the arrays. */
          }
        else if(daProxy.objectType.compare("DataArray<uint8_t>") == 0 ){}
        else if(daProxy.objectType.compare("DataArray<int16_t>") == 0 ){}
        else if(daProxy.objectType.compare("DataArray<uint16_t>") == 0 ){}
        else if(daProxy.objectType.compare("DataArray<int32_t>") == 0 ){}
        else if(daProxy.objectType.compare("DataArray<uint32_t>") == 0 ){}
        else if(daProxy.objectType.compare("DataArray<int64_t>") == 0 ){}
        else if(daProxy.objectType.compare("DataArray<uint64_t>") == 0 ){}
        else if(daProxy.objectType.compare("DataArray<float>") == 0 ){}
        else if(daProxy.objectType.compare("DataArray<double>") == 0 ){}


      }

      dataContainer->addAttributeMatrix(amProxy.name, attrMatrix);
    }
  }

  return dcArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer DREAM3DFileStructure::ReadFileStructure(const QString &filePath)
{
  DataContainerArray::Pointer dcArray = DataContainerArray::NullPointer();
  herr_t err = 0;
  hid_t fileId = QH5Utilities::openFile(filePath, true);
  if(fileId < 0) { return dcArray; }
  HDF5ScopedFileSentinel sentinel(&fileId, false); // Make sure the file gets closed automatically if we return early

  //Check the DREAM3D File Version to make sure we are reading the proper version
  QString d3dVersion;
  err = QH5Lite::readStringAttribute(fileId, "/", DREAM3D::HDF5::DREAM3DVersion, d3dVersion);
  if (err < 0) { std::cout << "Attribute '" << DREAM3D::HDF5::DREAM3DVersion.toStdString() << " was not found" << std::endl; }
  else { std::cout << DREAM3D::HDF5::DREAM3DVersion.toStdString() << ":" << d3dVersion.toStdString() << std::endl; }
  QString fileVersion;
  err = QH5Lite::readStringAttribute(fileId, "/", DREAM3D::HDF5::FileVersionName, fileVersion);
  if (err < 0) { std::cout << "Attribute '" << DREAM3D::HDF5::FileVersionName.toStdString() << " was not found" << std::endl; }
  else { std::cout << DREAM3D::HDF5::FileVersionName.toStdString() << ":" << fileVersion.toStdString() << std::endl; }

  hid_t dcArrayGroupId = H5Gopen(fileId, DREAM3D::StringConstants::DataContainerGroupName.toAscii().constData(), H5P_DEFAULT);
  if (dcArrayGroupId < 0) { return dcArray; }
  sentinel.addGroupId(&dcArrayGroupId);
  DataContainerArray_T proxy;

  QString h5InternalPath = QString("/") + DREAM3D::StringConstants::DataContainerGroupName;

  // Read the entire structure of the file into the proxy
  _readDataContainers(dcArrayGroupId, proxy, h5InternalPath);

  dcArray = _createDataContainerArray(proxy);

  //Now generate a concrete instance of the DataContainerArrayClass.
  fileId = -1;
  return dcArray;
}



