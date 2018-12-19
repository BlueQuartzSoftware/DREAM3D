/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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
#include "HexagonalLowIPFLegendPainter.h"

#include <QtGui/QPainter>

#include "OrientationLib/LaueOps/HexagonalLowOps.h"
#include "OrientationLib/LaueOps/LaueOps.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HexagonalLowIPFLegendPainter::HexagonalLowIPFLegendPainter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HexagonalLowIPFLegendPainter::~HexagonalLowIPFLegendPainter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage HexagonalLowIPFLegendPainter::createLegend(int pixelWidth, int pixelHeight)
{
  HexagonalLowOps ops;
  int imageSize = std::max(pixelWidth, pixelHeight);
  UInt8ArrayType::Pointer rgbaImage = ops.generateIPFTriangleLegend(imageSize);
  QRgb* rgba = reinterpret_cast<QRgb*>(rgbaImage->getPointer(0));

  QImage image(pixelWidth, pixelHeight, QImage::Format_ARGB32_Premultiplied);

  int32_t xDim = pixelWidth;
  int32_t yDim = pixelHeight;
  size_t idx = 0;

  for(int32_t y = 0; y < yDim; ++y)
  {
    for(int32_t x = 0; x < xDim; ++x)
    {
      idx = static_cast<size_t>((y * xDim) + x);
      image.setPixel(x, y, rgba[idx]);
    }
  }

  image = overlayText(pixelWidth, pixelHeight, image, &ops);
  return image;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage HexagonalLowIPFLegendPainter::overlayText(int pixelWidth, int pixelHeight, QImage image, LaueOps* ops)
{
  QSize imageSize(pixelWidth, pixelHeight);
  int32_t fontHeight = 0;
  int32_t fontWidth = 0;
  int size = std::max(pixelWidth, pixelHeight);

  int32_t fontScale = static_cast<int32_t>( 24.0f / 256.0f * static_cast<float>(size)); // At 256 Pixel Image, we want to use 24 Point font
  if(fontScale < 10)
  {
    fontScale = 10;
  } // Do not use fonts below 10Point.

  QFont font("Lato", fontScale, QFont::Bold);
  {
    QPainter painter;
    QImage pImage(100, 100, QImage::Format_ARGB32_Premultiplied);
    pImage.fill(0xFFFFFFFF); // All white background
    painter.begin(&pImage);

    painter.setFont(font);
    QFontMetrics metrics0 = painter.fontMetrics();
    fontHeight = metrics0.height();
    fontWidth = metrics0.width(ops->getSymmetryName());
    painter.end();
  }

  int32_t xMargin = 10; // 10 pixel Margin
  int32_t pImageWidth = imageSize.width() + xMargin * 2;
  if(pImageWidth < fontWidth)
  {
    pImageWidth = fontWidth + xMargin * 2;
  }
  int32_t pImageHeight = imageSize.height() + fontHeight * 2.5;

  QImage pImage(pImageWidth, pImageHeight, QImage::Format_ARGB32_Premultiplied);
  pImage.fill(0xFFFFFFFF); // All white background

  // Create a Painter backed by a QImage to draw into
  QPainter painter;
  painter.begin(&pImage);
  painter.setRenderHint(QPainter::Antialiasing, true);

  painter.setFont(font);
  QFontMetrics metrics = painter.fontMetrics();

  // Draw the Figure starting at y=3*FontHeight. The first "Row" will be the title, the second any other text
  // and starting at the third, the figure.
  QPoint point(xMargin, fontHeight * 1); // The "Y" coordinate may have to be adjusted for each type of figure.
  painter.drawImage(point, image);

  qint32 penWidth = 2;
  painter.setPen(QPen(QColor(0, 0, 0, 255), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

  QString label("[11_20]");
  fontWidth = metrics.width(label);
  fontHeight = metrics.height();
  int x = pImageWidth/2 - fontWidth/2;
  int y = fontHeight * 2;
  paintSymmetryDirection(label, &metrics, &painter, x, y);

  label = QString("[2_1_10]"); // Green
  fontWidth = metrics.width(label);
  fontHeight = metrics.height();
  x = pImageWidth - (fontWidth * 1.05f);
  y = fontHeight * 1.90f + imageSize.height();
  paintSymmetryDirection(label, &metrics, &painter, x, y);

  label = QString("[0001]"); // Red
  fontWidth = metrics.width(label);
  fontHeight = metrics.height();
  painter.drawText(10, fontHeight*1.90 + imageSize.height(), label);

  label = ops->getSymmetryName();
  fontWidth = metrics.width(label);
  fontHeight = metrics.height();
  painter.drawText(pImageWidth/2 - fontWidth/2, fontHeight * 1.0, label);


  return pImage;
}
