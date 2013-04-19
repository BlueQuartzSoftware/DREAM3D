///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////
#ifndef QHDF5TreeMODEL_H_
#define QHDF5TreeMODEL_H_

 #include <QAbstractItemModel>
 #include <QModelIndex>
 #include <QVariant>
 #include <QFileIconProvider>

#include <hdf5.h>

 class QHDF5TreeModelItem;

 class QHDF5TreeModel : public QAbstractItemModel
 {
     Q_OBJECT

 public:
     QHDF5TreeModel(hid_t fileId, QObject *parent = 0);
     ~QHDF5TreeModel();

     QVariant data(const QModelIndex &index, int role) const;
     Qt::ItemFlags flags(const QModelIndex &index) const;
     QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const;
     QModelIndex index(int row, int column,
                       const QModelIndex &parent = QModelIndex()) const;
     QModelIndex parent(const QModelIndex &index) const;
     int rowCount(const QModelIndex &parent = QModelIndex()) const;
     int columnCount(const QModelIndex &parent = QModelIndex()) const;

     QString hdfPathForIndex(const QModelIndex &index);

     bool hasChildren(const QModelIndex &parent) const;
 private:
     void setupModelData();

     QHDF5TreeModelItem *rootItem;
     hid_t _fileId;
     QFileIconProvider iconProvider;
 };


#endif /*QHDF5TreeMODEL_H_*/
