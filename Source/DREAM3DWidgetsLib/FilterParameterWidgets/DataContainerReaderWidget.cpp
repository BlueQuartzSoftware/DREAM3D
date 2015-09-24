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

#include "DataContainerReaderWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QDateTime>

#include <QtWidgets/QLabel>
#include <QtWidgets/QFileDialog>
#include <QtGui/QStandardItemModel>
#include <QtCore/QItemSelectionModel>
#include <QtWidgets/QListWidget>

#include "SIMPLib/CoreFilters/DataContainerReader.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// Initialize private static member variable
QString DataContainerReaderWidget::m_OpenDialogLastDirectory = "";

namespace Detail
{
  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  QList<QStandardItem*> findChildItems(QStandardItem* parent, QString text)
  {
    QList<QStandardItem*> list;
    if (parent->hasChildren() == false) { return list; } // No children, nothing to find
    int childCount = parent->rowCount();

    for(int i = 0; i < childCount; i++)
    {
      QStandardItem* item = parent->child(i);
      if(text.compare(item->text()) == 0)
      {
        list.push_back(item);
      }
    }
    return list;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void removeNonExistantChildren(QStandardItem* parent, QStringList possibleNames)
  {
    int childCount = parent->rowCount();

    // iterate from back to front as we may pop values out of the model which would screw up the index
    for(int i = childCount - 1; i >= 0; i--)
    {
      QStandardItem* item = parent->child(i);
      QStringList list = possibleNames.filter(item->text() );
      if(list.size() == 0) // the name is in the model but NOT in the proxy so we need to remove it
      {
        // qDebug() << "!! Removing " << item->text();
        parent->removeRow(i);
      }
    }


  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template<typename T>
  QStandardItem* getColumnItem(QStandardItem* parent, QString name, T& proxy)
  {
    QStandardItem* item = NULL;
    QList<QStandardItem*> items = findChildItems(parent, name);
    if (items.count() == 0)
    {
      // Create a new item because we did not find this item already
      item = new QStandardItem(proxy.name);
      item->setCheckState( (proxy.flag == 2 ? Qt::Checked : Qt::Unchecked) );
      item->setCheckable(true);
      parent->appendRow(item);
    }
    else if (items.count() > 1)
    {
      item = NULL;
    }
    else
    {
      item = items.at(0);
      item->setCheckState( (proxy.flag == 2 ? Qt::Checked : Qt::Unchecked) );
      item->setCheckable(true);
    }

    return item;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template<typename T>
  QStandardItem* updateProxyItem(QStandardItem* parent, QString name, T& proxy)
  {
    QStandardItem* item = NULL;
    if(NULL == parent) { return item; }
    QList<QStandardItem*> items = findChildItems(parent, name);
    if (items.count() == 1)
    {
      item = items.at(0);
      //   qDebug() << parent->text() << " | " << item->text() << " ::"  << proxy.flag << " (Going to Change to) " << item->checkState();
      proxy.flag = item->checkState();
    }

    return item;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void transferDataContainFlags(const DataContainerProxy& source, DataContainerArrayProxy& dest)
  {
    QMap<QString, DataContainerProxy>& dcProxies = dest.dataContainers;
    for (QMap<QString, DataContainerProxy>::iterator dcIter = dcProxies.begin(); dcIter != dcProxies.end(); ++dcIter)
    {
      DataContainerProxy& dcProxy = dcIter.value();
      if(dcProxy.name.compare(source.name) == 0)
      {
        // we have the correct DataContainer, so transfer the flags
        dcProxy.flag = source.flag;
      }
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void transferAttributeMatrixFlags(const QString dcName, const AttributeMatrixProxy& source, DataContainerArrayProxy& dest)
  {
    QMap<QString, DataContainerProxy>& dcProxies = dest.dataContainers;
    for (QMap<QString, DataContainerProxy>::iterator dcIter = dcProxies.begin(); dcIter != dcProxies.end(); ++dcIter)
    {
      DataContainerProxy& dcProxy = dcIter.value();
      if (dcProxy.name.compare(dcName) == 0)
      {
        // we have the correct DataContainer, so transfer the flags
        //      dcProxy.flag = source.flag;
        QMap<QString, AttributeMatrixProxy>& amProxies = dcProxy.attributeMatricies;
        for (QMap<QString, AttributeMatrixProxy>::iterator amIter = amProxies.begin(); amIter != amProxies.end(); ++amIter)
        {
          QString amName = amIter.key();
          if (amName.compare(source.name) == 0)
          {
            AttributeMatrixProxy& attrProxy = amIter.value();
            attrProxy.flag = source.flag;
          }
        }
      }
    }
  }


  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void transferDataArrayFlags(const QString dc_name, const QString am_name, const DataArrayProxy& source, DataContainerArrayProxy& dest)
  {
    QMap<QString, DataContainerProxy>& dcProxies = dest.dataContainers;
    for (QMap<QString, DataContainerProxy>::iterator dcIter = dcProxies.begin(); dcIter != dcProxies.end(); ++dcIter)
    {
      DataContainerProxy& dcProxy = dcIter.value();
      if (dcProxy.name.compare(dc_name) == 0)
      {
        // we have the correct DataContainer, so transfer the flags
        //dcProxy.flag = source.flag;
        QMap<QString, AttributeMatrixProxy>& amProxies = dcProxy.attributeMatricies;
        for (QMap<QString, AttributeMatrixProxy>::iterator amIter = amProxies.begin(); amIter != amProxies.end(); ++amIter)
        {
          QString amName = amIter.key();
          if (amName.compare(am_name) == 0)
          {
            AttributeMatrixProxy& amProxy = amIter.value();
            //attrProxy.flag = source.flag;

            QMap<QString, DataArrayProxy>& daProxies = amProxy.dataArrays;
            for (QMap<QString, DataArrayProxy>::iterator daIter = daProxies.begin(); daIter != daProxies.end(); ++daIter)
            {
              QString daName = daIter.key();
              if (daName.compare(source.name) == 0)
              {
                DataArrayProxy& daProxy = daIter.value();
                daProxy = source;
              }
            }
          }
        }
      }
    }
  }


} /* end namespace Detail */


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReaderWidget::DataContainerReaderWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<DataContainerReaderFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "DataContainerReaderWidget can ONLY be used with a DataContainerReaderFilterParameter object");
  m_Filter = dynamic_cast<DataContainerReader*>(filter);
  Q_ASSERT_X(getFilter() != NULL, "NULL Pointer", "DataContainerReaderWidget can ONLY be used with a DataContainerReader object");

  if ( m_OpenDialogLastDirectory.isEmpty() )
  {
    m_OpenDialogLastDirectory = QDir::homePath();
  }

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReaderWidget::DataContainerReaderWidget(QWidget* parent) :
  FilterParameterWidget(NULL, NULL, parent),
  m_Filter(NULL),
  m_FilterParameter(NULL),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReaderWidget::~DataContainerReaderWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::setFilter(AbstractFilter* value)
{
  m_Filter = dynamic_cast<DataContainerReader*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter* DataContainerReaderWidget::getFilter() const
{
  return m_Filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::setFilterParameter(FilterParameter* value)
{
  m_FilterParameter = dynamic_cast<DataContainerReaderFilterParameter*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter* DataContainerReaderWidget::getFilterParameter() const
{
  return m_FilterParameter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::setupGui()
{

  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));


  // Put in a QStandardItemModel
  QAbstractItemModel* oldModel = dcaProxyView->model();
  QStandardItemModel* model = new QStandardItemModel;
  dcaProxyView->setModel(model);
  delete oldModel;

  //  void activated(const QModelIndex& index);
  connect(dcaProxyView, SIGNAL(clicked(const QModelIndex&)),
          this, SLOT(itemActivated(const QModelIndex)));

  if (getFilterParameter() != NULL)
  {
    label->setText(getFilterParameter()->getHumanLabel() );
  }

  if(getFilter() != NULL)
  {
    QString path = m_Filter->getInputFile();

    filePath->setText(path);
    on_filePath_fileDropped(path);
  }


}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::itemActivated(const QModelIndex& index)
{
  m_DidCausePreflight = true;
  updateProxyFromModel();
  m_Filter->setInputFileDataContainerArrayProxy(m_DcaProxy);    // Set the new proxy into filter

  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::updateModelFromProxy(DataContainerArrayProxy& proxy)
{
  m_DcaProxy = proxy;
  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(dcaProxyView->model());
  if(!model)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return;
  }
  QStandardItem* rootItem = model->invisibleRootItem();

  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy> containers = proxy.dataContainers.values();
  QListIterator<DataContainerProxy> containerIter(containers);
  QStringList dcList;
  while(containerIter.hasNext())
  {
    DataContainerProxy dcProxy = containerIter.next();
    dcList.push_back(dcProxy.name);
    QStandardItem* dcItem = Detail::getColumnItem<DataContainerProxy>(rootItem, dcProxy.name, dcProxy);
    assert(dcItem != NULL);
    //    qDebug() << "**  " << dcProxy.name;
    // We found the proper Data Container, now populate the AttributeMatrix List
    QMap<QString, AttributeMatrixProxy>& attrMats = dcProxy.attributeMatricies;
    QMutableMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
    while(attrMatsIter.hasNext() )
    {
      attrMatsIter.next();
      QString amName = attrMatsIter.key();
      AttributeMatrixProxy& attrProxy = attrMatsIter.value();
      QStandardItem* amItem = Detail::getColumnItem<AttributeMatrixProxy>(dcItem, amName, attrProxy);
      assert(amItem != NULL);

      //  qDebug() << "@@@ " << amName;
      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
      QMap<QString, DataArrayProxy>& dataArrays = attrProxy.dataArrays;
      QMutableMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
      while(dataArraysIter.hasNext() )
      {
        dataArraysIter.next();
        DataArrayProxy& daProxy = dataArraysIter.value();
        QString daName = dataArraysIter.key();
        //   qDebug() << "#### " << daName;
        QStandardItem* daItem = Detail::getColumnItem<DataArrayProxy>(amItem, daName, daProxy);
        if (NULL == daItem)
        {
          Q_ASSERT_X(daItem != NULL, "daItem was NULL. This can not happen", "");
        }

      }

      // Now remove those items that are still in the model but NOT the proxy. This can happen if a filter upstream
      // renames something
      Detail::removeNonExistantChildren(amItem, dataArrays.keys() );
    }
    // Now remove any nonexistant AttributeMatrix objects
    Detail::removeNonExistantChildren(dcItem, attrMats.keys() );
  }
  // Remove any Data Containers from the model
  Detail::removeNonExistantChildren(rootItem, dcList);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::updateProxyFromModel()
{
  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(dcaProxyView->model());
  if (!model)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return;
  }
  //
  QStandardItem* rootItem = model->invisibleRootItem();
  // Loop over the data containers until we find the proper data container
  QMap<QString, DataContainerProxy>& dcProxies = m_DcaProxy.dataContainers;
  for (QMap<QString, DataContainerProxy>::iterator dcIter = dcProxies.begin(); dcIter != dcProxies.end(); ++dcIter)
  {
    DataContainerProxy& dcProxy = dcIter.value();
    //  dcList.push_back(dcProxy.name);
    QStandardItem* dcItem = Detail::updateProxyItem<DataContainerProxy>(rootItem, dcProxy.name, dcProxy);

    //    qDebug() << "**  " << dcProxy.name;
    // We found the proper Data Container, now populate the AttributeMatrix List
    QMap<QString, AttributeMatrixProxy>& amProxies = dcProxy.attributeMatricies;
    for (QMap<QString, AttributeMatrixProxy>::iterator amIter = amProxies.begin(); amIter != amProxies.end(); ++amIter)
    {
      QString amName = amIter.key();
      AttributeMatrixProxy& amProxy = amIter.value();
      QStandardItem* amItem = Detail::updateProxyItem<AttributeMatrixProxy>(dcItem, amName, amProxy);

      //   qDebug() << "@@@ " << amName;
      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
      QMap<QString, DataArrayProxy>& daProxies = amProxy.dataArrays;
      for (QMap<QString, DataArrayProxy>::iterator daIter = daProxies.begin(); daIter != daProxies.end(); ++daIter)
      {
        DataArrayProxy& daProxy = daIter.value();
        QString daName = daIter.key();
        //    qDebug() << "#### " << daName;
        QStandardItem* daItem = Detail::updateProxyItem<DataArrayProxy>(amItem, daName, daProxy);
        Q_UNUSED(daItem)
      }
    }
  }

  //  m_DcaProxy.print("updateProxy AFTER Updating");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::updateProxyFromProxy(DataContainerArrayProxy& current, DataContainerArrayProxy& incoming)
{
  //  qDebug() << getFilter()->getNameOfClass() << " DataContainerReaderWidget::mergeProxies";

  // Loop over the current model and only worry about getting the flag for each proxy from the current and transfering
  // that flag to the incoming. This allows us to save the selections but also update the model later on with this new
  // proxy which will have selection flags set appropriately.

  QList<DataContainerProxy> containers = current.dataContainers.values();
  QListIterator<DataContainerProxy> containerIter(containers);
  //  QStringList dcList;
  while(containerIter.hasNext())
  {
    DataContainerProxy dcProxy = containerIter.next();

    // We have a DataContainer from the DataContainerArrayProxy, transfer any flags from this DataContainerProxy to
    // the same one in the incoming DataContainerArrayProxy
    Detail::transferDataContainFlags(dcProxy, incoming);

    QMap<QString, AttributeMatrixProxy>& attrMats = dcProxy.attributeMatricies;
    QMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
    while(attrMatsIter.hasNext() )
    {
      attrMatsIter.next();
      QString amName = attrMatsIter.key();
      const AttributeMatrixProxy attrProxy = attrMatsIter.value();

      Detail::transferAttributeMatrixFlags(dcProxy.name, attrProxy, incoming);

      //   qDebug() << "@@@ " << amName;
      // Loop over the current AttributeMatrixProxy and see if we need to transfer any flags.
      const QMap<QString, DataArrayProxy> dataArrays = attrProxy.dataArrays;
      QMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
      while(dataArraysIter.hasNext() )
      {
        dataArraysIter.next();
        DataArrayProxy daProxy = dataArraysIter.value();

        Detail::transferDataArrayFlags(dcProxy.name, attrProxy.name, daProxy, incoming);
      }
    }
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::beforePreflight()
{
#if 0
  if (m_DidCausePreflight == false)
  {
    //  qDebug() << getFilter()->getNameOfClass() << " DataContainerReaderWidget::beforePreflight()";
    // Get the DataContainerArray from the Filter instance. This will have what will become the choices for the user
    // to select from.
    DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
    DataContainerArrayProxy incomingProxy = DataContainerArrayProxy(dca.get() );
    incomingProxy.setAllFlags(getFilterParameter()->getDefaultFlagValue());
    //incomingProxy.print("BeforePreflight INCOMING");
    //Now the idea becomes to save the selections that the user has made and transfer those changes to the incoming
    // proxy object
    updateProxyFromProxy(m_DcaProxy, incomingProxy);
    //proxy.print("'proxy' beforePreflight AFTER updateProxyFromProxy()");

    m_DcaProxy = incomingProxy;
    // Now that the proxy was updated with our selections, make the updated incoming proxy into our cache
    // Now update the Model
    updateModelFromProxy(m_DcaProxy);
  }
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
// qDebug() << "DataContainerReaderWidget::filterNeedsInputParameters()";
  m_Filter->setInputFile(filePath->text());
  updateProxyFromModel(); // Will update m_DcaProxy with the latest selections from the Model
  m_Filter->setInputFileDataContainerArrayProxy(m_DcaProxy);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::afterPreflight()
{
  // Make sure the model has the latest data from the proxy
  DataContainerArrayProxy p = m_Filter->getInputFileDataContainerArrayProxy();
  updateModelFromProxy(p);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerReaderWidget::verifyPathExists(QString filePath, QFSDropLabel* lineEdit)
{
  QFileInfo fileinfo(filePath);
  if (false == fileinfo.exists())
  {
    //lineEdit->setStyleSheet("border: 1px solid red;");
    lineEdit->changeStyleSheet(QFSDropLabel::FS_DOESNOTEXIST_STYLE);
  }
  else
  {
    lineEdit->changeStyleSheet(QFSDropLabel::FS_STANDARD_STYLE);
  }
  return fileinfo.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::on_filePath_fileDropped(const QString& text)
{
  DataContainerArrayProxy proxy;

  setOpenDialogLastDirectory(text);
  // Set/Remove the red outline if the file does exist

  if (verifyPathExists(text, filePath) == true)
  {
    if(getFilter() != NULL)
    {
      if(m_Filter->getLastFileRead().compare(text) != 0)
      {
        QStandardItemModel* model = qobject_cast<QStandardItemModel*>(dcaProxyView->model());
        if(NULL != model)
        {
          model->clear();
        }

        if (m_Filter->getInputFileDataContainerArrayProxy().dataContainers.size() > 0 && (text == m_Filter->getLastFileRead() || m_Filter->getLastFileRead().isEmpty()))
        {
          proxy = m_Filter->getInputFileDataContainerArrayProxy();
        }
        else
        {
          proxy = m_Filter->readDataContainerArrayStructure(text);
          m_Filter->setLastRead(QDateTime::currentDateTime());
        }

        m_Filter->setLastFileRead(text); // Update the cached file path in the filter
      }

      updateModelFromProxy(proxy);
    }
    emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::on_selectBtn_clicked()
{
  QString currentPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).toString();
  QString Ftype = ""; //getFilterParameter()->getFileType();
  QString ext = "*.dream3d"; //getFilterParameter()->getFileExtension();
  QString s = Ftype + QString(" Files (") + ext + QString(");;All Files(*.*)");
  QString defaultName = m_OpenDialogLastDirectory + QDir::separator() + "Untitled";
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"), defaultName, s);

  if(true == file.isEmpty())
  {
    return;
  }
  file = QDir::toNativeSeparators(file);
  // Store the last used directory into the private instance variable
  QFileInfo fi(file);
  m_OpenDialogLastDirectory = fi.path();
  filePath->setText(file);
  on_filePath_fileDropped(file);

  // filterNeedsInputParameters(getFilter());
  // emit parametersChanged(); // This should force the preflight to run because we are emitting a signal
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerReaderWidget::on_dcaProxyView_updatePreviewWidget(const QModelIndex& index)
{
//    QVariant var = index.data();
//    QString name = var.toString();

//    DataArrayInformationDisplayWidget::Pointer widget = DataArrayInformationDisplayWidget::New();
//    widget->setNameText(name);

//    // Create widget and add to dcaProxyView
//    dcaProxyView->setPreviewWidget(widget.get());
}
