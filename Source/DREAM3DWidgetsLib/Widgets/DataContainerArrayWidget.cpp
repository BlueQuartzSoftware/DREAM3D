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
  dataContainerInfoLabel->setText("");
  clearStandardItemModel();

  if(NULL != m_Filter)
  {
    connect(m_Filter, SIGNAL(preflightExecuted()),
            this, SLOT(updateView()));
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayWidget::updateView()
{

  // Sanity check our filter object
  if (NULL == m_Filter) { return; }

  // Get an check out DataContainerArray object
  DataContainerArray::Pointer dca = m_Filter->getDataContainerArray();
  if(dca.get() == NULL) { return; }



  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(dcaProxyView->model());
  QVector<QString> path;
  {
    QModelIndex currIndex = dcaProxyView->currentIndex();
    QStandardItem* item = model->itemFromIndex(currIndex);
    // Get what is selected and save it

    while(NULL != item)
    {
      path.push_front(item->text());
      item = item->parent();
    }
  }
  // Clear out the view first
  clearStandardItemModel();
  model = qobject_cast<QStandardItemModel*>(dcaProxyView->model());

  QItemSelectionModel* selectionModel = dcaProxyView->selectionModel();
  if(selectionModel)
  {
    connect(selectionModel, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(dcaProxyView_indexChanged(const QModelIndex&, const QModelIndex&)) );
  }

  // Sanity check model
  if(!model)
  {
    Q_ASSERT_X(model, "Model was not a QStandardItemModel in QColumnView", "");
    return;
  }


  QStandardItem* rootItem = model->invisibleRootItem();

  // Loop over the data containers until we find the proper data container
  QList<DataContainer::Pointer> containers = dca->getDataContainers();

  QListIterator<DataContainer::Pointer> containerIter(containers);
  //QStringList dcList;
  int row0 = 0;
  while(containerIter.hasNext())
  {
    DataContainer::Pointer dc = containerIter.next();

    QStandardItem* dcItem = new QStandardItem(dc->getName());
    rootItem->appendRow(dcItem);

    dcItem->setData(dc->getInfoString(DREAM3D::HtmlFormat), Qt::UserRole + 1);

    if(path.size() > 0 && dc->getName().compare(path[0]) == 0)
    {
      dcaProxyView->setCurrentIndex(model->indexFromItem(dcItem));
    }

    // We found the proper Data Container, now populate the AttributeMatrix List
    DataContainer::AttributeMatrixMap_t attrMats = dc->getAttributeMatrices();
    QMapIterator<QString, AttributeMatrix::Pointer> attrMatsIter(attrMats);
    int row1 = 0;
    while(attrMatsIter.hasNext() )
    {
      attrMatsIter.next();
      QString amName = attrMatsIter.key();
      AttributeMatrix::Pointer am = attrMatsIter.value();

      QStandardItem* amItem = new QStandardItem(am->getName());
      dcItem->appendRow(amItem);
      amItem->setData(am->getInfoString(DREAM3D::HtmlFormat), Qt::UserRole + 1);

      if(path.size() > 1 && am->getName().compare(path[1]) == 0)
      {
        dcaProxyView->setCurrentIndex(model->indexFromItem(amItem));
      }

      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
      QList<QString> attrArrayNames = am->getAttributeArrayNames();
      QListIterator<QString> dataArraysIter(attrArrayNames);
      int row2 = 0;
      while(dataArraysIter.hasNext() )
      {
        QString attrArrayName = dataArraysIter.next();
        IDataArray::Pointer attrArray = am->getAttributeArray(attrArrayName);

        QStandardItem* aaItem = new QStandardItem(attrArrayName);
        amItem->appendRow(aaItem);
        aaItem->setData(attrArray->getInfoString(DREAM3D::HtmlFormat), Qt::UserRole + 1);

        if(path.size() > 2 && attrArrayName.compare(path[2]) == 0)
        {
          QModelIndex idx = model->indexFromItem(aaItem);
          dcaProxyView->setCurrentIndex(idx);
        }
        row2++;
      }
      row1++;
    }
    row0++;
  }

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayWidget::preflightCompleted()
{
  //  if(m_Filter)
  //  {
  //  qDebug() << "Filter: " << m_Filter->getHumanLabel() << " Filter DataContaiers: ";
  //  DataContainerArray::Pointer dcArray = m_Filter->getDataContainerArray();

  //  QList<DataContainer::Pointer> containers = dcArray->getDataContainers();

  //  for(int i = 0; i < containers.count(); i++)
  //  {
  //    DataContainer::Pointer container = containers.at(i);
  //    QString msg;
  //    QTextStream ss(&msg);
  //    ss << container->getName() << " <b>Geometry</b><br/>" << container->getGeometry()->getGeometryTypeAsString() << "\n";
  //    m_DataContainerInfos.insert(container->getName(), msg);
  //  }

  //  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayWidget::dcaProxyView_indexChanged(const QModelIndex & current, const QModelIndex & previous)
{
  QMap<int, QVariant> values = dcaProxyView->model()->itemData(current);
  QString infoString = values.value(Qt::UserRole + 1).toString();
  dataContainerInfoLabel->setText(infoString);
  //  QLabel* label = new QLabel("Foo");
  //  dcaProxyView->setPreviewWidget(label);
  // label->show();
  // dcaProxyView->update();
}

void DataContainerArrayWidget::on_dcaProxyView_updatePreviewWidget(const QModelIndex & index)
{
  //  QMap<int, QVariant> values = dcaProxyView->model()->itemData(index);
  //  QString dcName = values.value(Qt::DisplayRole).toString();
  //  qDebug() << "Update PreviewWidget for " << dcName;
  //  QLabel* label = new QLabel(dcName);
  //  dcaProxyView->setPreviewWidget(label);
}
