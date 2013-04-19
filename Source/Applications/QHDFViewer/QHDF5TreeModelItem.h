///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _QHDF5TreeModelItem_H_
#define _QHDF5TreeModelItem_H_

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtGui/QIcon>


#include <hdf5.h>

class QHDF5TreeModelItem
{
  public:
    QHDF5TreeModelItem(hid_t fileId, const QString &data, QHDF5TreeModelItem *parent = 0);
    ~QHDF5TreeModelItem();

    void appendChild(QHDF5TreeModelItem *child);

    QHDF5TreeModelItem *child(int row);
    int childCount() ;
    int columnCount() ;
    QVariant data(int column) ;
    int row() ;
    QHDF5TreeModelItem *parent();

    QString generateHDFPath();

    bool isGroup();
    bool isImage();
    bool isTable();
    bool isString();

    int numAttributes();
    int numDimensions();

    QIcon icon();

  protected:
    void initializeChildItems();
    void initializeChildCount();

  private:
    QList<QHDF5TreeModelItem*> childItems;
    int childItemsInitialized;
    int _childCount;
    QVariant itemData;
    QHDF5TreeModelItem *parentItem;
    hid_t _fileId;
    int _num_attrs;
    int _numDims;
    bool _isGroup;
    bool _isImage;
    bool _isTable;
    bool _isString;
    std::string _dataType;
};

#endif /* _QHDF5TreeModelItem_H_ */
