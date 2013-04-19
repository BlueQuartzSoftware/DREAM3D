#include <QtGui>
 #include <QIcon>
 #include <QPixmap>

#include "H5Support/H5Utilities.h"

#include "QHDF5TreeModel.h"
#include "QHDF5TreeModelItem.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDF5TreeModel::QHDF5TreeModel(hid_t fileId, QObject *parent) :
  QAbstractItemModel(parent), _fileId(fileId)
{
  rootItem = new QHDF5TreeModelItem(_fileId, "HEADER");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QHDF5TreeModel::~QHDF5TreeModel()
{
  delete rootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QHDF5TreeModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return static_cast<QHDF5TreeModelItem*>(parent.internalPointer())->columnCount();
  else
    return rootItem->columnCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant QHDF5TreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  QHDF5TreeModelItem *item = static_cast<QHDF5TreeModelItem*>(index.internalPointer());

  if (role == Qt::DecorationRole) {
     return item->icon();
  }

  if (role != Qt::DisplayRole)
    return QVariant();



  return item->data(index.column());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags QHDF5TreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant QHDF5TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    return QVariant("HEADER");
   // return rootItem->data(section);
  }
  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex QHDF5TreeModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  QHDF5TreeModelItem *parentItem;

  if (!parent.isValid())
    parentItem = rootItem;
  else
    parentItem = static_cast<QHDF5TreeModelItem*>(parent.internalPointer());

  QHDF5TreeModelItem *childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex QHDF5TreeModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  QHDF5TreeModelItem *childItem = static_cast<QHDF5TreeModelItem*>(index.internalPointer());
  QHDF5TreeModelItem *parentItem = childItem->parent();

  if (parentItem == rootItem)
    return QModelIndex();

  return createIndex(parentItem->row(), 0, parentItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QHDF5TreeModel::hasChildren(const QModelIndex &parent) const
{
//  if (!parent.isValid()) //
//      return true; //

  if (parent.column() > 0)
      return false;

  QHDF5TreeModelItem *parentItem;
  if (!parent.isValid())
    parentItem = rootItem;
  else
    parentItem = static_cast<QHDF5TreeModelItem*>(parent.internalPointer());

  return parentItem->isGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QHDF5TreeModel::rowCount(const QModelIndex &parent) const
{
  QHDF5TreeModelItem *parentItem;
  if (parent.column() > 0)
    return 0;

  if (!parent.isValid())
    parentItem = rootItem;
  else
    parentItem = static_cast<QHDF5TreeModelItem*>(parent.internalPointer());

  return parentItem->childCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QHDF5TreeModel::setupModelData()
{
  if (this->_fileId < 0)
  {
    return;
  }

  this->rootItem->appendChild(new QHDF5TreeModelItem(this->_fileId, QString("/"), this->rootItem));

#if 0
  std::list<std::string> itemList;
  herr_t err = H5Utilities::getGroupObjects(this->_fileId, H5Utilities::MXA_ANY, itemList);
  if (err < 0)
  {
    std::cout << "Error getting group objects. " <<__FILE__ << ":" << __LINE__ << std::endl;
    return;
  }


  for (std::list<std::string>::iterator iter = itemList.begin(); iter != itemList.end(); ++iter )
  {
    this->rootItem->appendChild(new QHDF5TreeModelItem(this->_fileId, QString( (*iter).c_str() ), this->rootItem) );
  }
#endif
  //parents.last()->appendChild(new QHDF5TreeModelItem(columnData, parents.last()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QHDF5TreeModel::hdfPathForIndex(const QModelIndex &index)
{
  QHDF5TreeModelItem* item = static_cast<QHDF5TreeModelItem*>(index.internalPointer());
  return item->generateHDFPath();
}


