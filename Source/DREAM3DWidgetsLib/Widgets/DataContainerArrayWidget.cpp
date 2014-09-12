#include "DataContainerArrayWidget.h"

#include <QtCore/QMetaProperty>

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayWidget::DataContainerArrayWidget(QWidget* parent) :
  QWidget(parent),
  m_Filter(NULL)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayWidget::DataContainerArrayWidget(AbstractFilter* filter, QWidget* parent) :
  QWidget(parent),
  m_Filter(filter)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayWidget::~DataContainerArrayWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayWidget::initializeWithFilter(AbstractFilter* filter)
{
  m_Filter = filter;
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayWidget::clearStandardItemModel()
{
  // Put in a QStandardItemModel
  //qDebug() << "clearStandardItemModel()";
  QAbstractItemModel* oldModel = dcaProxyView->model();
  QStandardItemModel* model = new QStandardItemModel;
  dcaProxyView->setModel(model);
  delete oldModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayWidget::setupGui()
{
  QAbstractItemModel* oldModel = dcaProxyView->model();
  QStandardItemModel* model = new QStandardItemModel;
  dcaProxyView->setModel(model);
  delete oldModel;

  if(NULL != m_Filter)
  {
    connect(m_Filter, SIGNAL(preflightExecuted()),
            this, SLOT(updateView()));

    updateView();
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayWidget::updateView()
{
  if (NULL == m_Filter) { return; }

  DataContainerArray::Pointer dca = m_Filter->getDataContainerArray();
  DataContainerArrayProxy proxy(dca.get());
  updateModelFromProxy(proxy);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QStandardItem*> DataContainerArrayWidget::findChildItems(QStandardItem* parent, QString text)
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
void DataContainerArrayWidget::removeNonExistantChildren(QStandardItem* parent, QStringList possibleNames)
{
  int childCount = parent->rowCount();

  // iterate from back to front as we may pop values out of the model which would screw up the index
  for(int i = childCount - 1; i >= 0; i--)
  {
    QStandardItem* item = parent->child(i);
    QStringList list = possibleNames.filter(item->text() );
    if(list.size() == 0) // the name is in the model but NOT in the proxy so we need to remove it
    {
      //qDebug() << "!! Removing " << item->text();
      parent->removeRow(i);
    }
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayWidget::updateModelFromProxy(DataContainerArrayProxy& proxy)
{
  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(dcaProxyView->model());
  if(!model)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return;
  }
  model->clear();

  QStandardItem* rootItem = model->invisibleRootItem();

  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy> containers = proxy.list;
  //qDebug() << "containers.size() = " << containers.size();

  QListIterator<DataContainerProxy> containerIter(containers);
  QStringList dcList;
  while(containerIter.hasNext())
  {
    DataContainerProxy dcProxy = containerIter.next();
    dcList.push_back(dcProxy.name);
    QStandardItem* dcItem = getColumnItem<DataContainerProxy>(rootItem, dcProxy.name, dcProxy);
    assert(dcItem != NULL);
    //qDebug() << "**  " << dcProxy.name;
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

      //qDebug() << "@@@ " << amName;
      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
      QMap<QString, DataArrayProxy>& dataArrays = attrProxy.dataArrays;
      QMutableMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
      while(dataArraysIter.hasNext() )
      {
        dataArraysIter.next();
        DataArrayProxy& daProxy = dataArraysIter.value();
        QString daName = dataArraysIter.key();
        //qDebug() << "#### " << daName;
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


