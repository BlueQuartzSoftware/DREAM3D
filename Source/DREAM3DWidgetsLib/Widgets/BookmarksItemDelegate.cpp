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

#include <QtCore/QFileInfo>

#include <QtWidgets/QLineEdit>

#include <QtGui/QIntValidator>
#include <QtGui/QPainter>

#include "DREAM3DWidgetsLib/Widgets/BookmarksItemDelegate.h"
#include "DREAM3DWidgetsLib/Widgets/BookmarksItem.h"
#include "DREAM3DWidgetsLib/Widgets/BookmarksModel.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_BookmarksItemDelegate.cpp"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItemDelegate::BookmarksItemDelegate(QObject* parent) :
  QStyledItemDelegate(parent)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BookmarksItemDelegate::~BookmarksItemDelegate()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* BookmarksItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QLineEdit* editor = new QLineEdit(parent);
  return editor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  QString value = index.model()->data(index, Qt::DisplayRole).toString();
  QLineEdit* line = static_cast<QLineEdit*>(editor);
  line->setText(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  BookmarksModel* bModel = qobject_cast<BookmarksModel*>(model);

  QLineEdit* line = static_cast<QLineEdit*>(editor);
  QString value = line->text();

  if (value.isEmpty() == false)
  {
    QModelIndex bIndex = bModel->index(index.row(), BookmarksItem::Name, index.parent());
    bModel->setData(bIndex, value, Qt::DisplayRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  editor->setGeometry(option.rect);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BookmarksItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
// Place any painting code here

  QStyledItemDelegate::paint(painter, option, index);
}
