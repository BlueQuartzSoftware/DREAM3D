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

#ifndef BookmarksModel_H
#define BookmarksModel_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>
#include <QtCore/QFileSystemWatcher>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"
#include "DREAM3DWidgetsLib/Widgets/FilterLibraryTreeWidget.h"
#include "DREAM3DWidgetsLib/Widgets/BookmarksItem.h"

enum IndexState
{
  Error,
  Default
};

class BookmarksItem;

class DREAM3DWidgetsLib_EXPORT BookmarksModel : public QAbstractItemModel
{
    Q_OBJECT

  public:
    SIMPL_TYPE_MACRO(BookmarksModel)

    ~BookmarksModel();

    static BookmarksModel* Instance();

    static BookmarksModel* NewInstanceFromFile(QString filePath);

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    virtual QModelIndex sibling(int row, int column, const QModelIndex& idx) const Q_DECL_OVERRIDE;

    bool isEmpty();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    bool insertRows(int position, int rows, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

    bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_OVERRIDE;

    bool needsToBeExpanded(const QModelIndex& index);
    void setNeedsToBeExpanded(const QModelIndex& index, bool value);

    BookmarksItem* getRootItem();

    void moveIndexInternally(const QModelIndex& index, QModelIndex& oldParent, QModelIndex& newParent);

    void addFileToTree(QString& path, QModelIndex& specifiedParent);

    QStringList getFilePaths();

    QModelIndexList findIndexByPath(QString filePath);

    void setFileSystemWatcher(QFileSystemWatcher* watcher);
    QFileSystemWatcher* getFileSystemWatcher();

  protected:
    BookmarksModel(QObject* parent = 0);

  protected slots:
    void updateRowState(const QString& path);
    void updateModel(const QModelIndex& topLeft, const QModelIndex& bottomRight);

  private:
    BookmarksItem*            rootItem;
    QFileSystemWatcher*       m_Watcher;

    static BookmarksModel* self;

    BookmarksItem* getItem(const QModelIndex& index) const;
    void copyIndexToTemp(const QModelIndex& index, const QModelIndex& oldParent, const QModelIndex& tempParent, BookmarksModel* tempModel);

    void copyTempToIndex(QModelIndex& index, QModelIndex& newParent, const QModelIndex& tempParent, BookmarksModel* tempModel);

    QStringList getFilePaths(BookmarksItem* item);

    QModelIndexList findIndexByPath(const QModelIndex& index, QString filePath);

    BookmarksModel(const BookmarksModel&);    // Copy Constructor Not Implemented
    void operator=(const BookmarksModel&);  // Operator '=' Not Implemented
};

#endif // BookmarksModel_H
