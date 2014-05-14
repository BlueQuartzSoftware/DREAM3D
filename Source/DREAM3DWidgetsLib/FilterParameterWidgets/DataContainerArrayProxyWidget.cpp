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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "DataContainerArrayProxyWidget.h"

#include <QtCore/QMetaProperty>
#include <QtGui/QStandardItemModel>
#include <QtGui/QItemSelectionModel>

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxyWidget::DataContainerArrayProxyWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  QWidget(parent),
  m_Filter(filter),
  m_DidCausePreflight(false)
{

  m_FilterParameter = dynamic_cast<DataContainerArrayProxyFilterParameter*>(parameter);

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxyWidget::~DataContainerArrayProxyWidget()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::setupGui()
{




  connect(m_Filter, SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  connect(m_Filter, SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  connect(m_Filter, SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));


  // Put in a QStandardItemModel
  QAbstractItemModel* oldModel = dcaProxyView->model();
  QStandardItemModel* model = new QStandardItemModel;
  dcaProxyView->setModel(model);
  delete oldModel;

  //  void activated(const QModelIndex& index);
  connect(dcaProxyView, SIGNAL(clicked(const QModelIndex &)),
          this, SLOT(itemActivated(const QModelIndex)));

  if (m_FilterParameter != NULL)
  {
    QString units = m_FilterParameter->getUnits();
    if(units.isEmpty() == false)
    {
      label->setText(m_FilterParameter->getHumanLabel() + " (" + units + ")");
    }
    else
    {
      label->setText(m_FilterParameter->getHumanLabel() );
    }


    //If the filter is just being dragged into the pipeline then the filter is going to have an empty
    // Proxy object at which point nothing is going to be put into the lists. But if the Filter was
    // initialized from a pipeline file then it might actually have a proxy so we need to do something
    // with that proxy?
    m_DcaProxy = m_Filter->property(PROPERTY_NAME_AS_CHAR).value<DataContainerArrayProxy>();
    //m_DcaProxy.print("DataContainerArrayProxyWidget::setupGui()");
  }

#if 0
  // is the filter parameter tied to a boolean property of the Filter Instance, if it is then we need to make the check box visible
  if(m_FilterParameter->isConditional() == true)
  {
    bool boolProp = m_Filter->property(m_FilterParameter->getConditionalProperty().toLatin1().constData() ).toBool();
    conditionalCB->setChecked(boolProp);
    conditionalCB->setText(m_FilterParameter->getConditionalLabel());
    dataContainerList->setEnabled(boolProp);
    attributeMatrixList->setEnabled(boolProp);
    attributeArrayList->setEnabled(boolProp);
    on_conditionalCB_stateChanged(conditionalCB->checkState());
  }
  else
  {
    widgetLayout->removeWidget(conditionalCB);
    conditionalCB->deleteLater();
    widgetLayout->removeWidget(linkLeft);
    linkLeft->deleteLater();
    widgetLayout->removeWidget(linkRight);
    linkRight->deleteLater();
  }
#endif

}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::on_conditionalCB_stateChanged(int state)
{
  bool boolProp = conditionalCB->isChecked();
  dataContainerList->setEnabled(boolProp);
  dataContainerList->setVisible(boolProp);
  attributeMatrixList->setEnabled(boolProp);
  attributeMatrixList->setVisible(boolProp);
  dataArrayName->setEnabled(boolProp);
  dataArrayName->setVisible(boolProp);

  label->setVisible(boolProp);
  linkLeft->setVisible(boolProp);
  linkRight->setVisible(boolProp);
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::itemActivated(const QModelIndex& index)
{
  m_DidCausePreflight = true;
  updateProxyFromModel();
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // qDebug() << "DataContainerArrayProxyWidget::filterNeedsInputParameters(AbstractFilter* filter)";

  QVariant var;
  var.setValue(m_DcaProxy);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(m_Filter, m_FilterParameter);
  }

//  if(m_FilterParameter->isConditional() )
//  {
//    var.setValue(conditionalCB->isChecked());
//    ok = filter->setProperty(m_FilterParameter->getConditionalProperty().toLatin1().constData(), var);
//    if(false == ok)
//    {
//      FilterParameterWidgetsDialogs::ShowCouldNotSetConditionalFilterParameter(m_Filter, m_FilterParameter);
//    }
//  }

}


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
  for(int i = childCount-1; i >= 0; i--)
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
QStandardItem* getColumnItem(QStandardItem* parent, QString name, T &proxy)
{
  QStandardItem* item = NULL;
  QList<QStandardItem*> items = findChildItems(parent, name);
  if (items.count() == 0)
  { // Create a new item because we did not find this item already
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
QStandardItem* updateProxyItem(QStandardItem* parent, QString name, T &proxy)
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
void DataContainerArrayProxyWidget::updateModelFromProxy(DataContainerArrayProxy &proxy)
{
  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(dcaProxyView->model());
  if(!model)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return;
  }
  QStandardItem* rootItem = model->invisibleRootItem();

  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy> containers = proxy.list;
  QListIterator<DataContainerProxy> containerIter(containers);
  QStringList dcList;
  while(containerIter.hasNext())
  {
    DataContainerProxy dcProxy = containerIter.next();
    dcList.push_back(dcProxy.name);
    QStandardItem* dcItem = getColumnItem<DataContainerProxy>(rootItem, dcProxy.name, dcProxy);
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
      QStandardItem* amItem = getColumnItem<AttributeMatrixProxy>(dcItem, amName, attrProxy);
      assert(amItem != NULL);

      //   qDebug() << "@@@ " << amName;
      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
      QMap<QString, DataArrayProxy>& dataArrays = attrProxy.dataArrays;
      QMutableMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
      while(dataArraysIter.hasNext() )
      {
        dataArraysIter.next();
        DataArrayProxy& daProxy = dataArraysIter.value();
        QString daName = dataArraysIter.key();
        //    qDebug() << "#### " << daName;
        QStandardItem* daItem = getColumnItem<DataArrayProxy>(amItem, daName, daProxy);
        assert(daItem != NULL);

      }

      // Now remove those items that are still in the model but NOT the proxy. This can happen if a filter upstream
      // renames something
      removeNonExistantChildren(amItem, dataArrays.keys() );
    }
    // Now remove any nonexistant AttributeMatrix objects
    removeNonExistantChildren(dcItem, attrMats.keys() );
  }
  // Remove any Data Containers from the model
  removeNonExistantChildren(rootItem, dcList);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::updateProxyFromModel()
{
  // This will only work for a single selection
  DataContainerArrayProxy& proxy = m_DcaProxy;

  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(dcaProxyView->model());
  if(!model)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return;
  }
  //
  QStandardItem* rootItem = model->invisibleRootItem();
  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy>& containers = proxy.list;
  QMutableListIterator<DataContainerProxy> containerIter(containers);
  //  QStringList dcList;
  while(containerIter.hasNext())
  {
    DataContainerProxy& dcProxy = containerIter.next();
    //  dcList.push_back(dcProxy.name);
    QStandardItem* dcItem = updateProxyItem<DataContainerProxy>(rootItem, dcProxy.name, dcProxy);

    //    qDebug() << "**  " << dcProxy.name;
    // We found the proper Data Container, now populate the AttributeMatrix List
    QMap<QString, AttributeMatrixProxy>& attrMats = dcProxy.attributeMatricies;
    QMutableMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
    while(attrMatsIter.hasNext() )
    {
      attrMatsIter.next();
      QString amName = attrMatsIter.key();
      AttributeMatrixProxy& attrProxy = attrMatsIter.value();
      QStandardItem* amItem = updateProxyItem<AttributeMatrixProxy>(dcItem, amName, attrProxy);

      //   qDebug() << "@@@ " << amName;
      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
      AttributeMatrixProxy& amProxy = attrMatsIter.value();
      QMap<QString, DataArrayProxy>& dataArrays = amProxy.dataArrays;
      QMutableMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
      while(dataArraysIter.hasNext() )
      {
        dataArraysIter.next();
        DataArrayProxy& daProxy = dataArraysIter.value();
        QString daName = dataArraysIter.key();
        //    qDebug() << "#### " << daName;
        QStandardItem* daItem = updateProxyItem<DataArrayProxy>(amItem, daName, daProxy);
        Q_UNUSED(daItem)
      }
    }
  }

  //  m_DcaProxy.print("updateProxy AFTER Updating");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void copyFlagsFromProxyToProxy(DataContainerArrayProxy &proxy, QString dc_name, QString am_name, QString da_name, DataArrayProxy& destProxy)
//{
//  QList<DataContainerProxy>& containers = proxy.list;
//  // QListIterator<DataContainerProxy> containerIter(containers);
//  for(int i = 0; i < containers.size(); i++)
//  {
//    DataContainerProxy& dcProxy = containers[i]; // Make sure to get a Reference to the proxy
//    if(dc_name.compare(dcProxy.name) != 0 ) { continue; }

//    // We found the proper Data Container, now populate the AttributeMatrix List
//    QMap<QString, AttributeMatrixProxy>& attrMats = dcProxy.attributeMatricies;
//    QMutableMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
//    while(attrMatsIter.hasNext() )
//    {
//      attrMatsIter.next();
//      QString amName = attrMatsIter.key();
//      if(am_name.compare(amName) != 0) { continue; }

//      //   qDebug() << "@@@ " << amName;
//      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
//      AttributeMatrixProxy& amProxy = attrMatsIter.value(); // Make sure to get a Reference to the proxy
//      QMap<QString, DataArrayProxy>& dataArrays = amProxy.dataArrays;
//      QMutableMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
//      while(dataArraysIter.hasNext() )
//      {
//        dataArraysIter.next();
//        DataArrayProxy& daProxy = dataArraysIter.value(); // Make sure to get a Reference to the proxy
//        QString daName = dataArraysIter.key();
//        if(da_name.compare(daName) == 0)
//        {
//          destProxy.flag = daProxy.flag;
//        }
//      }
//    }
//  }
//}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void transferDataContainFlags(const DataContainerProxy& source, DataContainerArrayProxy& dest)
{

  QMutableListIterator<DataContainerProxy> dcaIter(dest.list);
  while(dcaIter.hasNext() )
  {
    DataContainerProxy& dcProxy = dcaIter.next();
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
void transferAttributeMatrixFlags(const QString dcName, const AttributeMatrixProxy& source, DataContainerArrayProxy &dest)
{

  QMutableListIterator<DataContainerProxy> dcaIter(dest.list);
  while(dcaIter.hasNext() )
  {
    DataContainerProxy& dcProxy = dcaIter.next();
    if(dcProxy.name.compare(dcName) == 0)
    {
      // we have the correct DataContainer, so transfer the flags
      //      dcProxy.flag = source.flag;
      QMutableMapIterator<QString, AttributeMatrixProxy> attrMatsIter(dcProxy.attributeMatricies);
      while(attrMatsIter.hasNext())
      {
        attrMatsIter.next();
        QString amName = attrMatsIter.key();
        if(amName.compare(source.name) == 0)
        {
          AttributeMatrixProxy& attrProxy = attrMatsIter.value();
          attrProxy.flag = source.flag;
        }
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void transferDataArrayFlags(const QString dc_name, const QString am_name, const DataArrayProxy& source, DataContainerArrayProxy &dest)
{

  QMutableListIterator<DataContainerProxy> dcaIter(dest.list);
  while(dcaIter.hasNext() )
  {
    DataContainerProxy& dcProxy = dcaIter.next();
    if(dcProxy.name.compare(dc_name) == 0)
    {
      // we have the correct DataContainer, so transfer the flags
      //dcProxy.flag = source.flag;
      QMutableMapIterator<QString, AttributeMatrixProxy> attrMatsIter(dcProxy.attributeMatricies);
      while(attrMatsIter.hasNext())
      {
        attrMatsIter.next();
        QString amName = attrMatsIter.key();
        if(amName.compare(am_name) == 0)
        {
          AttributeMatrixProxy& attrProxy = attrMatsIter.value();
          //attrProxy.flag = source.flag;

          QMutableMapIterator<QString, DataArrayProxy> dataArraysIter(attrProxy.dataArrays);
          while(dataArraysIter.hasNext() )
          {
            dataArraysIter.next();
            QString daName = dataArraysIter.key();
            if(daName.compare(source.name) == 0)
            {
              DataArrayProxy& daProxy = dataArraysIter.value();
              daProxy = source;
            }
          }
        }
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::updateProxyFromProxy(DataContainerArrayProxy &current, DataContainerArrayProxy &incoming)
{
  //  qDebug() << m_Filter->getNameOfClass() << " DataContainerArrayProxyWidget::mergeProxies";

  // Loop over the current model and only worry about getting the flag for each proxy from the current and transfering
  // that flag to the incoming. This allows us to save the selections but also update the model later on with this new
  // proxy which will have selection flags set appropriately.

  QList<DataContainerProxy> containers = current.list;
  QListIterator<DataContainerProxy> containerIter(containers);
  //  QStringList dcList;
  while(containerIter.hasNext())
  {
    DataContainerProxy dcProxy = containerIter.next();

    // We have a DataContainer from the DataContainerArrayProxy, transfer any flags from this DataContainerProxy to
    // the same one in the incoming DataContainerArrayProxy
    transferDataContainFlags(dcProxy, incoming);

    QMap<QString, AttributeMatrixProxy>& attrMats = dcProxy.attributeMatricies;
    QMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
    while(attrMatsIter.hasNext() )
    {
      attrMatsIter.next();
      QString amName = attrMatsIter.key();
      const AttributeMatrixProxy attrProxy = attrMatsIter.value();

      transferAttributeMatrixFlags(dcProxy.name, attrProxy, incoming);

      //   qDebug() << "@@@ " << amName;
      // Loop over the current AttributeMatrixProxy and see if we need to transfer any flags.
      const QMap<QString, DataArrayProxy> dataArrays = attrProxy.dataArrays;
      QMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
      while(dataArraysIter.hasNext() )
      {
        dataArraysIter.next();
        DataArrayProxy daProxy = dataArraysIter.value();

        transferDataArrayFlags(dcProxy.name, attrProxy.name, daProxy, incoming);
      }
    }
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::beforePreflight()
{
  if (m_DidCausePreflight == false)
  {
    //  qDebug() << m_Filter->getNameOfClass() << " DataContainerArrayProxyWidget::beforePreflight()";
    // Get the DataContainerArray from the Filter instance. This will have what will become the choices for the user
    // to select from.
    DataContainerArray::Pointer dca = m_Filter->getDataContainerArray();
    DataContainerArrayProxy incomingProxy = DataContainerArrayProxy(dca.get() );
    incomingProxy.setAllFlags(m_FilterParameter->getDefaultFlagValue());
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

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxyWidget::afterPreflight()
{
  // qDebug() << m_Filter->getNameOfClass() << " DataContainerArrayProxyWidget::afterPreflight()";
}
