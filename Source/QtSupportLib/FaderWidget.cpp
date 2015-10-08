/****************************************************************************
**
** Copyright (C) 2006 Trolltech AS. All rights reserved.
**
** This file is part of the documentation of Qt. It was originally
** published as part of Qt Quarterly.
**
** This file may be used under the terms of the GNU General Public License
** version 2.0 as published by the Free Software Foundation or under the
** terms of the Qt Commercial License Agreement. The respective license
** texts for these are provided with the open source and commercial
** editions of Qt.
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "FaderWidget.h"

#include <QtWidgets>

#include "moc_FaderWidget.cpp"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FaderWidget::FaderWidget(QWidget* parent)
  : QWidget(parent),
    timer(NULL)
{
  if (parent)
  { startColor = Qt::white; /* parent->palette().window().color();*/ }
  else
  { startColor = Qt::white; }

  fadeIn = true;

  currentAlpha = 0;
  duration = 300;

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));

  setAttribute(Qt::WA_DeleteOnClose);
  if(parent) { resize(parent->size()); }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FaderWidget::~FaderWidget()
{
  if(timer)
  {
    delete timer;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FaderWidget::setFadeIn()
{
  fadeIn = true;
  currentAlpha = 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FaderWidget::setFadeOut()
{
  fadeIn = false;
  currentAlpha = 254;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FaderWidget::setStartColor(QColor color)
{
  startColor = color;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QColor FaderWidget::getStartColor()
{
  return startColor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FaderWidget::start()
{
  currentAlpha = 255;
  if(!fadeIn) { currentAlpha = 0; }
  timer->start(50);
  show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FaderWidget::paintEvent(QPaintEvent* /* event */)
{
  QPainter painter(this);
  QColor semiTransparentColor = startColor;
  semiTransparentColor.setAlpha(currentAlpha);
  painter.fillRect(rect(), semiTransparentColor);
  if(fadeIn) { currentAlpha -= 255 * timer->interval() / duration;}
  else { currentAlpha += 255 * timer->interval() / duration; }

  // qDebug() << currentAlpha;

  if (currentAlpha <= 0 || currentAlpha >= 255)
  {
    timer->stop();
    close();
    emit animationComplete();
  }
}
