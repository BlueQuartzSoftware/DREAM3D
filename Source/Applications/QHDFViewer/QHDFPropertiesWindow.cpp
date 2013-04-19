/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "QHDFPropertiesWindow.h"

#include <QtGui/QHeaderView>

//--
#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"



#include "DREAM3DLib/Common/IDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDFPropertiesWindow::QHDFPropertiesWindow(hid_t fileId, const QString &hdfPath, QWidget *parent) :
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
QHDFPropertiesWindow::~QHDFPropertiesWindow()
{
  //std::cout << "~QHDFImageDataWindow" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFPropertiesWindow::init()
{
  setAttribute(Qt::WA_DeleteOnClose);
  initGeneralTab();
  initAttributesTab();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFPropertiesWindow::initGeneralTab()
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

  H5T_class_t attr_type;
  size_t attr_size;
 // std::string res;
  std::vector<hsize_t> dims;
  err = H5Lite::getDatasetInfo(this->_fileId, datasetPath, dims, attr_type, attr_size);
  if (err < 0)
  {
    std::cout << "Could not get dataset info for " << datasetPath << std::endl;
    return;
  }

  this->numDimensionsLE->setText(QString::number(dims.size() ) );

  QString key;
  int index = 0;
  for (std::vector<hsize_t>::iterator vIter = dims.begin(); vIter != dims.end(); ++vIter)
  {
    if (index > 0)
    {
      key += " x ";
    }
    key +=  QString::number(*vIter);

    ++index;
  }
  this->dimensionSizesLE->setText(key);

  hid_t typeId = H5Lite::getDatasetType(this->_fileId, datasetPath);
  QString theType;
  if (attr_type == H5T_STRING)
  {
    theType = "H5T_STRING";
  }
  else
  {
    theType = QString::fromStdString( H5Lite::StringForHDFType(typeId) );
  }
  err = H5Tclose(typeId);
  this->dataTypeLE->setText( theType );

  // Now get the chunking and Compression Info
  hid_t dataset = H5Dopen(this->_fileId, datasetPath.c_str(), H5P_DEFAULT);
  hid_t cparms = H5Dget_create_plist(dataset); /* Get properties handle first. */

  if (H5D_CHUNKED != H5Pget_layout(cparms))  {
    this->chunkingLE->setText("NONE");
  }
  else
  {
    hsize_t chunkDims[10];
    ::memset(chunkDims, 0, 10*sizeof(hsize_t) );
    err = H5Pget_chunk(cparms, 10, chunkDims);
    key = "";
    for (int i = 0; i < 10; ++i)
    {
      if (i > 0 && chunkDims[i] > 0)
      {
        key += " x ";
      }
      if (chunkDims[i] > 0) {
        key +=  QString::number(chunkDims[i]);
      }
    }
    this->chunkingLE->setText(key);
  }

  int nFilters = H5Pget_nfilters(cparms);
  if (nFilters == 0 )
  {
    this->compressionLE->setText("NONE");
  }
  else
  {
    unsigned int flags = 0;
    size_t cd_nelmts = 0;
    unsigned int cd_values = 0;
    char filterName[256];
    ::memset(filterName, 0, 256);
    H5Z_filter_t filterType = H5Pget_filter(cparms, 0, &flags, &cd_nelmts, &cd_values, 256, filterName, NULL);

    this->compressionLE->setText( filterName );
  }

  // Get the Allocation Time for the data set
  H5D_alloc_time_t alloc_time;
  err = H5Pget_alloc_time(cparms, &alloc_time);
  switch (alloc_time)
  {
    case H5D_ALLOC_TIME_DEFAULT:
      this->allocationTimeLE->setText("Default");
      break;
    case H5D_ALLOC_TIME_EARLY:
      this->allocationTimeLE->setText("Early");
      break;
    case H5D_ALLOC_TIME_INCR:
      this->allocationTimeLE->setText("Incremental");
      break;
    case H5D_ALLOC_TIME_LATE:
      this->allocationTimeLE->setText("Late");
      break;
    default:
      this->allocationTimeLE->setText("NOT AVAILABLE");
      break;

  }
  H5Pclose(cparms);
  H5Dclose(dataset);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFPropertiesWindow::initAttributesTab()
{
  std::string datasetPath = this->_hdfPath.toStdString();
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
   if (err < 0)
   {
     attributesTable->clearContents(); // Clear the attributes Table
     //_rawDataTableModel->resetModel(); // Clear the Raw data table
     //_displayTextMessage(QString("Data Set Does Not Exist at Path given"));
     std::cout << "Data Set Does NOT exist at Path given: FileId: " << this->_fileId << std::endl;
     return ;
   }


   //Update the attributes table
   if (NULL != attributesTable)
   {
     attributesTable->clearContents(); // Clear the attributes Table
     //std::list<std::string> attributes;
     MXAAbstractAttributes attributes;
     //err = H5Utilities::getAllAttributeNames(this->_fileId, datasetPath, attributes);
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
         QTableWidgetItem* keyItem = new QTableWidgetItem ( key.c_str() );
         attributesTable->setItem(row, 0, keyItem);

         QTableWidgetItem* valueItem = new QTableWidgetItem ( attr->valueToString(' ').c_str() );
         attributesTable->setItem(row, 1, valueItem);

         QTableWidgetItem* typeItem = new QTableWidgetItem( H5Lite::StringForHDFType( attr->getDataType() ).c_str() );
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
void QHDFPropertiesWindow::closeEvent(QCloseEvent *event)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDFPropertiesWindow::addRow(QTableWidget* table, int row, const QString &key, const QString &value)
{

    QTableWidgetItem* keyItem  = new QTableWidgetItem( key );
    table->setItem(row, 0, keyItem );
    QTableWidgetItem* valueItem  = new QTableWidgetItem( value );
    table->setItem(row, 1, valueItem );
}

