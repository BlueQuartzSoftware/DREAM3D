/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Softwae, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "IPFLegendPainter.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IPFLegendPainter::IPFLegendPainter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IPFLegendPainter::~IPFLegendPainter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IPFLegendPainter::paintSymmetryDirection(const QString& text, QFontMetrics *metrics, QPainter *painter, int x, int y)
{

  QVector<int> offsets;
  int i = 0;
  QString mod;
  int fontHeight = metrics->height();

  while(i < text.length())
  {
    if(text.at(i) == '_')
    {
      int offset = metrics->width(mod);
      i++;
      mod = mod + text.at(i);
      int width = metrics->width(text.at(i)) * .80;
      painter->drawRect(x + offset, y - 0.80 * fontHeight, width, 1);
    }
    else
    {
      mod = mod + text.at(i);
    }
    i++;
  }
  painter->drawText(x, y, mod);
}
