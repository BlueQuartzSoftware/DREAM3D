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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "QScrollContentsWidget.h"

#include <iostream>

#include <QtGui/QMouseEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QVBoxLayout>

#include "PipelineBuilder/FilterWidgets/QFilterWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QScrollContentsWidget::QScrollContentsWidget(QWidget* parent) :
QWidget(parent)
{
  m_InsertedLabel.setText("Temp Label");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QScrollContentsWidget::~QScrollContentsWidget()
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QScrollContentsWidget::dragEnterEvent( QDragEnterEvent* event)
{
  event->acceptProposedAction();
  std::cout << "QScrollContentsWidget::dragEnterEvent: " << event->pos().x() << ", " << event->pos().y() << std::endl;
  QFilterWidget* w = qobject_cast<QFilterWidget*>(childAt(event->pos()));
  if (w != NULL)
  {
    std::cout << "QFilterWidget Found: " << w->getFilter()->getNameOfClass() << std::endl;
  }
  QVBoxLayout* l = qobject_cast<QVBoxLayout*>(childAt(event->pos()));
  if (l != NULL)
  {
    std::cout << "QScrollContentsWidget::dragEnterEvent: Found the QVBoxLayout" << std::endl;
  }
  QScrollContentsWidget* o = qobject_cast<QScrollContentsWidget*>(childAt(event->pos()));
  if (o != NULL)
  {
    std::cout << "QScrollContentsWidget::dragEnterEvent: " << o->objectName().toStdString() << std::endl;

  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QScrollContentsWidget::dragMoveEvent( QDragMoveEvent* event)
{
  std::cout << "QScrollContentsWidget::dragMoveEvent: " << event->pos().x() << ", " << event->pos().y() << std::endl;
  QFilterWidget* w = qobject_cast<QFilterWidget*>(childAt(event->pos()));
  if (w != NULL)
  {
    std::cout << "QScrollContentsWidget::dragMoveEvent: QFilterWidget Found: " << w->getFilter()->getNameOfClass() << std::endl;
  }
  QVBoxLayout* l = qobject_cast<QVBoxLayout*>(childAt(event->pos()));
  if (l != NULL)
  {
    std::cout << "QScrollContentsWidget::dragMoveEvent: Found the QVBoxLayout" << std::endl;
  }
  QObject* o = qobject_cast<QObject*>(childAt(event->pos()));
  if (o == NULL)
  {
    std::cout << "QScrollContentsWidget::dragMoveEvent: No Child deteected" << std::endl;
    QVBoxLayout* l = qobject_cast<QVBoxLayout*>(layout());
    if (l) {
      int count = l->count();



    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QScrollContentsWidget::dropEvent(QDropEvent *event)
{
  std::cout << "QScrollContentsWidget::dropEvent: " << event->pos().x() << ", " << event->pos().y() << std::endl;

}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QScrollContentsWidget::mousePressEvent(QMouseEvent *event)
{
  std::cout << "QScrollContentsWidget::mousePressEvent: " << event->pos().x() << ", " << event->pos().y() << std::endl;
    QFilterWidget* w = static_cast<QFilterWidget*>(childAt(event->pos()));
    if (!w)
        return;

    QPixmap pixmap = QPixmap::grabWidget(w);
    if (pixmap.isNull() == false)
    {
      std::cout << "We got an image of the widget." << std::endl;
    }

}
#endif
