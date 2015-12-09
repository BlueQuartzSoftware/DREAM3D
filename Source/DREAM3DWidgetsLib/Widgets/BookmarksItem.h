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

#ifndef BookmarksItem_H
#define BookmarksItem_H

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtCore/QVector>

#include <QtGui/QIcon>

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"




class DREAM3DWidgetsLib_EXPORT BookmarksItem
{
  public:
    BookmarksItem(const QVector<QVariant>& data, BookmarksItem* parent = 0);
    virtual ~BookmarksItem();

    enum ColumnData
    {
      Name,
      Path
    };


    BookmarksItem* child(int number);
    BookmarksItem* parent();

    int childCount() const;
    int columnCount() const;

    QVariant data(int column) const;
    bool setData(int column, const QVariant& value);

    bool getItemHasErrors();
    bool setItemHasErrors(const bool& value);

    QString getItemTooltip();
    bool setItemTooltip(const QString& value);

    QIcon getIcon();
    bool setIcon(const QIcon& icon);

    bool needsToBeExpanded();
    void setNeedsToBeExpanded(bool value);

    bool insertChild(int position, BookmarksItem* child);
    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);

    bool removeChild(int position);
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);

    int childNumber() const;

    void setParent(BookmarksItem* parent);

    static QString TopLevelString();

  private:
    QList<BookmarksItem*>               m_ChildItems;
    QVector<QVariant>                   m_ItemData;
    BookmarksItem*                      m_ParentItem;
    bool                                m_ItemHasErrors;
    QString                             m_ItemTooltip;
    bool                                m_NeedsToBeExpanded;
    QIcon                               m_Icon;

    BookmarksItem(const BookmarksItem&);    // Copy Constructor Not Implemented
    void operator=(const BookmarksItem&);  // Operator '=' Not Implemented
};

#endif // BookmarksItem_H
