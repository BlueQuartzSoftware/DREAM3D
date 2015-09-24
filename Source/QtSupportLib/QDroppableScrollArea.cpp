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

#include "QDroppableScrollArea.h"

#include <iostream>

#include <QtCore/QString>
#include <QtCore/QtDebug>
#include <QtCore/QMimeData>
#include <QtGui/QMouseEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDragMoveEvent>

#include "moc_QDroppableScrollArea.cpp"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDroppableScrollArea::QDroppableScrollArea(QWidget* parent)
  : QScrollArea(parent)
{
//  setAcceptDrops(true);
//  setFrameShape(QFrame::Box);
//  setFrameShadow(QFrame::Plain);
//  QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
//  sizePolicy2.setHorizontalStretch(0);
//  sizePolicy2.setVerticalStretch(0);
//  setSizePolicy(sizePolicy2);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDroppableScrollArea::dragEnterEvent(QDragEnterEvent* event)
{
  // accept just text/uri-list mime format
// if (event->mimeData()->hasFormat("text/uri-list"))
  {
    event->acceptProposedAction();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDroppableScrollArea::dropEvent(QDropEvent* event)
{
  qDebug() << "QDroppableScrollArea::dropEvent" << "\n";
  if (event->mimeData()->hasText())
  {
    QByteArray dropData =  event->mimeData()->data("text/plain");
    QString name(dropData);
    //  qDebug() << "name: " << name() << "\n";
    emit filterDropped(name);
  }


//  event->acceptProposedAction();
//  event->setAccepted(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QDroppableScrollArea::dragMoveEvent( QDragMoveEvent* event)
{
// qDebug() << "QScrollContentsWidget::dragMoveEvent: " << event->pos().x() << ", " << event->pos().y() << "\n";
// QObject* w = qobject_cast<QObject*>(childAt(event->pos()));
// if (w != NULL)
  {
//   qDebug() << "--- Child at Pos: " << w->objectName() << "\n";
  }
}

