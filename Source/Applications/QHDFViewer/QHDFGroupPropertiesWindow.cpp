

#include "QHDFGroupPropertiesWindow.h"

#include <QtGui/QHeaderView>

//--
#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "DREAM3DLib/Common/IDataArray.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDFGroupPropertiesWindow::QHDFGroupPropertiesWindow(hid_t fileId, const QString &hdfPath, QWidget *parent) :
  QDialog(parent),
  _fileId(fileId),
  _hdfPath(hdfPath)
{
  setupUi(this);
  init();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDFGroupPropertiesWindow::~QHDFGroupPropertiesWindow()
{
  //std::cout << "~QHDFImageDataWindow" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFGroupPropertiesWindow::init()
{
  setAttribute(Qt::WA_DeleteOnClose);

  initGeneralTab();
  initAttributesTab();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFGroupPropertiesWindow::initGeneralTab()
{
  std::string datasetPath = this->_hdfPath.toStdString();
  std::string objName = H5Utilities::extractObjectName(datasetPath);
  QString objTypeStr;

  this->setWindowTitle("Properties - " + QString::fromStdString(objName) );
  //std::cout << "HDFGraphicsDelegate::displayHDFData: " << datasetPath << std::endl;

  herr_t err = 0;
  if (this->_fileId < 0)
  {
    std::cout << "Error: FileId is Invalid: " << this->_fileId << std::endl;
    // _displayTextMessage(QString("HDF5 File ID Invalid") );
    return;
  }
  // Test for Dataset Existance
  H5O_info_t statbuf;
  err = H5Oget_info_by_name(this->_fileId, datasetPath.c_str(),  &statbuf, H5P_DEFAULT);
  bool isGroup = false;
  if (err < 0)
  {
    generalTable->clearContents(); // Clear the attributes Table
    std::cout << "Data Set Does NOT exist at Path given: FileId: " << this->_fileId << std::endl;
    return;
  }

  switch (statbuf.type)
  {
    case H5O_TYPE_GROUP:
      isGroup = true;
      objTypeStr = "HDF5 Group";
      break;
    case H5O_TYPE_DATASET:
      objTypeStr = "HDF5 Dataset";
      isGroup = false;
      break;
    case H5O_TYPE_NAMED_DATATYPE:
      isGroup = false;
      objTypeStr = "Named DataType";
      break;
    default:
      isGroup = false;
      objTypeStr = "UNKNOWN";
  }
  //Update the attributes table


  this->nameLE->setText(QString::fromStdString(objName));
  this->pathLE->setText(this->_hdfPath);
  this->typeLE->setText(objTypeStr);
  QString objectIdString = QString::number(statbuf.addr);
  this->objectIDLE->setText(objectIdString);

#if 1
  generalTable->clearContents(); // Clear the attributes Table
  // Put the basic info in first

  generalTable->setColumnCount(2);
  generalTable->verticalHeader()->hide();
  // generalTable->horizontalHeader()->hide();

  QStringList headerLabels;
  headerLabels.insert(0, tr("Name"));
  headerLabels.insert(1, tr("Type"));
  generalTable->setHorizontalHeaderLabels(headerLabels);
#endif
  generalTable->setRowCount(5);
  hid_t grpId = H5Gopen(this->_fileId, datasetPath.c_str(), H5P_DEFAULT);
  hsize_t num_objs = 0;
  H5G_info_t group_info;
  err = H5Gget_info(grpId, &group_info);
  num_objs = group_info.nlinks;
  this->numberOfMembers->setText(QString::number(num_objs));

#if 1
  int row = 0;
  H5O_type_t objType;
//  size_t size=0;
  H5O_info_t objInfo;
  // This section will NOT scale well for a group that has a large number of objects in it
  // Some sort of threading API might be a better idea to use for this.
  for (hsize_t i = 0; i < num_objs; ++i)
  {
    generalTable->setRowCount(row + 1);
    err = H5Oget_info_by_idx(grpId, ".", H5_INDEX_NAME, H5_ITER_INC, i, &objInfo, H5P_DEFAULT);
    objType = objInfo.type;

    switch (objType)
    {
      case H5O_TYPE_UNKNOWN:
        objTypeStr = "Unknown";
        break;
      case H5O_TYPE_GROUP:
        objTypeStr = "HDF5 Group";
        break;
      case H5O_TYPE_DATASET:
        objTypeStr = "HDF5 Dataset";
        break;
      case H5O_TYPE_NAMED_DATATYPE:
        objTypeStr = "HDF5 Named Data type";
        break;
      default:
        objTypeStr = "No Matching Enumeration";
    }
    addRow(generalTable, row, QString::number(i), objTypeStr);
    ++row;
  }
#endif
  H5Gclose(grpId);

  generalTable->resizeColumnsToContents();
  generalTable->horizontalHeader()->setStretchLastSection(true);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFGroupPropertiesWindow::initAttributesTab()
{
  std::string datasetPath = this->_hdfPath.toStdString();
   //std::cout << "HDFGraphicsDelegate::displayHDFData: " << datasetPath << std::endl;

   herr_t err = 0;
   if (this->_fileId < 0)
   {
     std::cout << "Error: FileId is Invalid: " << this->_fileId << std::endl;
     return;
   }
   // Test for Dataset Existance
   // Test for Dataset Existance
   H5O_info_t statbuf;
   err = H5Oget_info_by_name(this->_fileId, datasetPath.c_str(),  &statbuf, H5P_DEFAULT);
   if (err < 0)
   {
     attributesTable->clearContents(); // Clear the attributes Table
     std::cout << "Data Set Does NOT exist at Path given: FileId: " << this->_fileId << std::endl;
     return ;
   }


   //Update the attributes table
   if (NULL != attributesTable)
   {
     attributesTable->clearContents(); // Clear the attributes Table
     MXAAbstractAttributes attributes;
     err = H5Utilities::readAllAttributes(this->_fileId, datasetPath, attributes);

     if (err < 0)
     {
       std::cout << "Error Reading Attributes for datasetPath: " << datasetPath << std::endl;
     }
     else
     {
       attributesTable->setRowCount(attributes.size());
       attributesTable->setColumnCount(3);
       attributesTable->verticalHeader()->hide();
       QStringList headerLabels;
       headerLabels.insert(0, tr("Name"));
       headerLabels.insert(1, tr("Value"));
       headerLabels.insert(2, tr("Type"));
       attributesTable->setHorizontalHeaderLabels( headerLabels );
       IDataArray::Pointer attr;
       qint32 row = 0;
       for (MXAAbstractAttributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
       {

         std::string key = (*iter).first;
         attr = (*iter).second;
         int32_t attrInt = attr->getDataType();
         std::string aType = H5Lite::StringForHDFType( attrInt );

         QTableWidgetItem* keyItem = new QTableWidgetItem ( key.c_str() );
         attributesTable->setItem(row, 0, keyItem);

         QTableWidgetItem* valueItem = new QTableWidgetItem ( attr->valueToString(' ').c_str() );
         attributesTable->setItem(row, 1, valueItem);

         QTableWidgetItem* typeItem = new QTableWidgetItem( aType.c_str() );
         attributesTable->setItem(row, 2, typeItem);

         ++row;
       }
       this->numberOfAttributes->setText(QString::number(attributes.size()) );
     }
   }


   attributesTable->resizeColumnsToContents();
   attributesTable->horizontalHeader()->setStretchLastSection(true);

   return ;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFGroupPropertiesWindow::closeEvent(QCloseEvent *event)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFGroupPropertiesWindow::addRow(QTableWidget* table, int row, const QString &key, const QString &value)
{

    QTableWidgetItem* keyItem  = new QTableWidgetItem( key );
    table->setItem(row, 0, keyItem );
    QTableWidgetItem* valueItem  = new QTableWidgetItem( value );
    table->setItem(row, 1, valueItem );
}

