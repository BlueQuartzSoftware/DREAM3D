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
#include "DREAM3DSplitterHandle.h"

#include <QtGui/QPainter>

#include <QtCore/QObject>

#include "moc_DREAM3DSplitterHandle.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DSplitterHandle::paintEvent(QPaintEvent*)
{
  QPainter painter(this);

  QColor topColor(145, 145, 145);
  QColor bottomColor(142, 142, 142);
  QColor gradientStart(252, 252, 252);
  QColor gradientStop(223, 223, 223);

  if (orientation() == Qt::Vertical)
  {
    painter.setPen(topColor);
    painter.drawLine(0, 0, width(), 0);
    painter.setPen(bottomColor);
    painter.drawLine(0, height() - 1, width(), height() - 1);

    QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, height() - 3));
    linearGrad.setColorAt(0, gradientStart);
    linearGrad.setColorAt(1, gradientStop);
    painter.fillRect(QRect(QPoint(0, 1), size() - QSize(0, 2)), QBrush(linearGrad));
  }
  else
  {
    //            painter.setPen(topColor);
    //            painter.drawLine(0, 0, 0, height());
    painter.setPen(topColor);
    painter.drawLine(0, 0, 0, height() - 1);
    painter.setPen(bottomColor);
    painter.drawLine(width() - 1, 0, width() - 1, height() - 1);

    QLinearGradient linearGrad(QPointF(0, 0), QPointF(width() - 3, 0));
    linearGrad.setColorAt(0, gradientStart);
    linearGrad.setColorAt(1, gradientStop);
    painter.fillRect(QRect(QPoint(1, 0), size() - QSize(2, 0)), QBrush(linearGrad));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSize DREAM3DSplitterHandle::sizeHint() const
{
  QSize parent = QSplitterHandle::sizeHint();
  //if (orientation() == Qt::Vertical) {
  return parent + QSize(3, 3);
  //        } else {
  //            return QSize(1, parent.height());
  //        }
}
