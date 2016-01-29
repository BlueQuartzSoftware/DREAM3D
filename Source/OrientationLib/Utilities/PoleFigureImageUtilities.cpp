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

#include "PoleFigureImageUtilities.h"

#include <QtCore/QDebug>
#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QFont>


#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Utilities/ColorTable.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PoleFigureImageUtilities::PoleFigureImageUtilities() :
  m_KernelWeightsInited(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PoleFigureImageUtilities::~PoleFigureImageUtilities()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PoleFigureImageUtilities::generateKernelWeigths(int kernelWidth, int kernelHeight)
{
  //  int pX = 0;
  //  int pY = 0;
  int index = 0;
  float delta = 0;
  m_KernelWeights.resize((kernelWidth + 1) * (kernelHeight + 1) * 4);


  float sigma = 1.0;
  float mu = 0.0;
  float root2Pi = sqrt(2.0 * M_PI);
  float OneOverSigmaRoot2Pi = 1.0 / (sigma * root2Pi);
  float TwoSigmaSqrd = 2.0 * sigma * sigma;


  for (int ky = -kernelHeight; ky <= kernelHeight; ++ky)
  {
    for (int kx = -kernelWidth; kx <= kernelWidth; ++kx)
    {
      delta = (kx) * (kx) + (ky) * (ky) + 1;
      m_KernelWeights[index] = OneOverSigmaRoot2Pi * exp(-(delta - mu) * (delta - mu) / TwoSigmaSqrd );
      ++index;
    }
    //  printf("\n");
  }
  m_KernelWeightsInited = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PoleFigureImageUtilities::countPixelNeighbors(int imageWidth, int imageHeight,
                                                  int pX, int pY,
                                                  QVector<qint32>& data,
                                                  QVector<qint32>& counts,
                                                  int kernelWidth, int kernelHeight, bool genmask)
{
  int xCoord, yCoord;
  int index = 0;


  // This figures out if we are "inside" the circle or not
  float radSqrd = (float)(imageWidth / 2.0f) * (imageWidth / 2.0f);
  float cX = imageWidth / 2.0f; // Center x
  float cY = imageHeight / 2.0f; // Center y
  int targetIndex = (imageWidth * pY) + pX;
  float delta = (pX - cX) * (pX - cX) + (pY - cY) * (pY - cY);

  if (genmask == true)
  {
    if (delta > radSqrd)
    {
      counts[targetIndex] = 0;
    }
    return counts[targetIndex];
  }


  // This pixel (px, py) is inside the circle so figure out the count
  int kernelWeightIndex = -1;
  for(int kY = -kernelHeight; kY <= kernelHeight; ++kY)
  {
    for(int kX = -kernelWidth; kX <= kernelWidth; ++kX)
    {
      ++kernelWeightIndex;
      xCoord = kX + pX;
      yCoord = kY + pY;
      if (xCoord < 0 || yCoord < 0) { continue; } // outside physical image to left/top
      if (xCoord >= imageWidth || yCoord >= imageHeight) { continue; } // Outside to right/bottom
      index = (imageWidth * yCoord) + xCoord; // Compute the index into the main image array
      if (data[index] > 0) // If the pixel is "on" it has a point in it
      {
        // Increment the neighbor count for this pixel only if it is within the circle
        delta = (xCoord - cX) * (xCoord - cX) + (yCoord - cY) * (yCoord - cY);
        if (delta <= radSqrd)
        {
          counts[targetIndex] += ( data[index] /* m_KernelWeights[kernelWeightIndex] */ );
        }
      }
    }
  }
  return counts[targetIndex];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage PoleFigureImageUtilities::GenerateScalarBar(int imageWidth, int imageHeight, PoleFigureConfiguration_t& config)
{
  int numColors = config.numColors;
  QImage pImage(imageWidth, imageHeight, QImage::Format_ARGB32_Premultiplied);
  pImage.fill(0xFFFFFFFF); // All white background

  // Create a Painter backed by a QImage to draw into
  QPainter painter;
  painter.begin(&pImage);
  painter.setRenderHint(QPainter::Antialiasing, true);

  int penWidth = 1;

#if 0
  // DRAW A BORDER AROUND THE IMAGE FOR DEBUGGING
  QColor c(RgbColor::dRgb(255, 0, 0, 255));
  painter.setPen(QPen(c, penWidth, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));

  painter.drawLine(0, 0, imageWidth, 0); // Top
  painter.drawLine(0, 0, 0, imageHeight); // Left
  painter.drawLine(imageWidth, 0, imageWidth, imageHeight); // Right
  painter.drawLine(0, imageHeight, imageWidth, imageHeight); // Bottom
  //-----------------
#endif

  //Get all the colors that we will need
  QVector<SIMPL::Rgb> colorTable(numColors);
  QVector<float> colors(3 * numColors, 0.0);
  SIMPLColorTable::GetColorTable(numColors, colors); // Generate the color table values
  float r = 0.0, g = 0.0, b = 0.0;
  for (int i = 0; i < numColors; i++) // Convert them to QRgbColor values
  {
    r = colors[3 * i];
    g = colors[3 * i + 1];
    b = colors[3 * i + 2];
    colorTable[i] = RgbColor::dRgb(r * 255, g * 255, b * 255, 255);
  }

  // Now start from the bottom and draw colored lines up the scale bar
  // A Slight Indentation for the scalar bar
  float margin = 0.05f;
  float scaleBarRelativeWidth = 0.10f;
  float scaleBarRelativeHeight = 1.0f - (margin * 2);

  int colorHeight = int( (imageHeight * scaleBarRelativeHeight) / numColors);

  QPointF topLeft(imageWidth * margin, imageHeight * margin);
  QSizeF size(imageWidth * scaleBarRelativeWidth, imageHeight * scaleBarRelativeHeight);

  int yLinePos = topLeft.y();

  QPointF start = topLeft;
  QPointF end = topLeft;

  for(int i = numColors - 1; i >= 0; i--)
  {
    QColor c(colorTable[i]);
    painter.setPen(QPen(c, penWidth, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
    for(int j = 0; j < colorHeight; j++)
    {
      start.setY(yLinePos);
      end.setX(topLeft.x() + (imageWidth * scaleBarRelativeWidth));
      end.setY(yLinePos);
      painter.drawLine(start, end);
      yLinePos++;
    }
  }

  // Draw the border of the scale bar
  size = QSizeF(imageWidth * scaleBarRelativeWidth, numColors * colorHeight); // Add two pixel to the height so we don't over write part of the scale bar
  QRectF scaleBorder(topLeft, size);
  penWidth = 2;
  painter.setPen(QPen(QColor(0, 0, 0, 255), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  painter.drawRect(scaleBorder);


  // Draw the Text Labels of the Scale Bar
  int startFontPtSize = 10;
  QFont font("Ariel", startFontPtSize, QFont::Bold);

  QFontMetrics metrics(font);
  int fontPixelsHeight = metrics.height();
  while(fontPixelsHeight < colorHeight * 2)
  {
    startFontPtSize++;
    font = QFont("Ariel", startFontPtSize, QFont::Bold);
    metrics = QFontMetrics(font);
    fontPixelsHeight = metrics.height();
  }
  painter.setFont(font);

  // Draw some more information to the right of the Scale Bar
  QString maxStr = QString::number(config.maxScale, 'f', 3);
  painter.drawText(topLeft.x() + (imageWidth * scaleBarRelativeWidth) + 10, topLeft.y() + fontPixelsHeight, maxStr);

  QString minStr = QString::number(config.minScale, 'f', 3);
  painter.drawText(topLeft.x() + (imageWidth * scaleBarRelativeWidth) + 10, topLeft.y() + size.height(), minStr);
  //------------------------------


  // Draw some statistics for the pole figures
  startFontPtSize = 8; // Set it to a rather large point size so we can fit the text perfectly. Only an insanely large Polefigure would go past this size
  int labelWidth = 0;

  // Make sure the size of font we just picked will allow the string to NOT be clipped because the rendered
  // pixel width is past the right side of the image
  int endOfStringYPos = topLeft.x() + (imageWidth * scaleBarRelativeWidth) + 10 + labelWidth;
  while(endOfStringYPos < imageWidth)
  {
    startFontPtSize++;
    font = QFont("Ariel", startFontPtSize, QFont::Bold);
    metrics = QFontMetrics(font);
    QString label("Upper & Lower");
    QString label2 = QString("Samples: ") + QString::number(config.eulers->getNumberOfTuples());
    fontPixelsHeight = metrics.height(); // Update the font height
    int labelWidth = metrics.width(label); // Figure out which string is longer (pixel wise)
    if (labelWidth < metrics.width(label2))
    {
      labelWidth = metrics.width(label2);
    }
    endOfStringYPos = topLeft.x() + (imageWidth * scaleBarRelativeWidth) + 10 + labelWidth;
  }
  startFontPtSize--;
  font = QFont("Ariel", startFontPtSize, QFont::Bold);
  metrics = QFontMetrics(font);
  QString label("Upper & Lower");
  QString label2 = QString("Samples: ") + QString::number(config.eulers->getNumberOfTuples());

  labelWidth = metrics.width(label);
  if (labelWidth < metrics.width(label2))
  {
    labelWidth = metrics.width(label2);
  }

  // Set the font into the Painter
  painter.setFont(font);

  QPointF statsPoint(topLeft.x() + (imageWidth * scaleBarRelativeWidth) + 10, imageHeight * .5);

  // Draw some more Statistics Text
  painter.drawText(statsPoint, label);
  statsPoint.setY(imageHeight * .5 + fontPixelsHeight);
  painter.drawText(statsPoint, label2);

  painter.end();
  // Scale the image down to 225 pixels
  return pImage;

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage PoleFigureImageUtilities::PaintPoleFigureOverlay(int imageWidth, int imageHeight, QString label, QImage image)
{
  int pxHigh = 0;
  int pxWide = 0;

  // Scale the Font Point size to something reasonable to the size of the image. Here our standard was 14Pt Font when the
  // Pole figure was 512 Pixels square.
  int fontPtSize = imageHeight / 32;

  QFont font("Ariel", fontPtSize, QFont::Bold);
  QFontMetrics metrics(font);
  pxHigh = metrics.height();
  pxWide = metrics.width(QString("Y"));

  int pxOffset = 2 * pxWide;
  int pyOffset = 2 * pxHigh;

  int pImageWidth = imageWidth + pxOffset * 2;
  int pImageHeight = imageHeight + pyOffset * 2;

  QImage pImage(pImageWidth, pImageHeight, QImage::Format_ARGB32_Premultiplied);
  pImage.fill(0xFFFFFFFF); // All white background

  // Create a Painter backed by a QImage to draw into
  QPainter painter;
  painter.begin(&pImage);
  painter.setRenderHint(QPainter::Antialiasing, true);
  qint32 penWidth = 1;

#if 0
  // DRAW A BORDER AROUND THE IMAGE FOR DEBUGGING
  QColor c(RgbColor::dRgb(255, 0, 0, 255));
  painter.setPen(QPen(c, penWidth, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));

  painter.drawLine(0, 0, pImageWidth, 0); // Top
  painter.drawLine(0, 0, 0, pImageHeight); // Left
  painter.drawLine(pImageWidth, 0, pImageWidth, pImageHeight); // Right
  painter.drawLine(0, pImageHeight, pImageWidth, pImageHeight); // Bottom
  //-----------------
#endif

  painter.setFont(font);
//  metrics = painter.fontMetrics();
//  pxHigh = metrics.height();
//  pxWide = metrics.width(QString("Y"));

  // Draw the Pole Figure into the center of the canvas
  QPoint point(pxOffset, pyOffset);
  painter.drawImage(point, image);

  // Scale pen width based on the size of the image
  penWidth = imageHeight / 256;
  painter.setPen(QPen(QColor(0, 0, 0, 255), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

  // Draw the Outer circular border around the pole figure
  QPainterPath circle;
  QPointF center(pImageWidth / 2, pImageHeight / 2);
  circle.addEllipse(center, imageWidth / 2, imageHeight / 2);
  painter.drawPath(circle);

  // Label the X Axis
  painter.drawText(pImageWidth - (pxWide * 1.5), pImageHeight / 2 + pxHigh / 3, "X");
  // Label the Y Axis
  pxWide = metrics.width(QString("Y"));
  painter.drawText(pImageWidth / 2 - pxWide / 2, pyOffset - penWidth - 1, "Y");

  // Draw the name of the Pole Figure
  int labelWidth = 0;
  int maxWidth = pImageWidth / 5; // No more than a Quarter of the width of the image
  while(labelWidth < maxWidth)
  {
    fontPtSize++;
    font = QFont("Ariel", fontPtSize, QFont::Bold);
    metrics = QFontMetrics(font);
    labelWidth = metrics.width(label); // Figure out which string is longer (pixel wise)
  }
  painter.setFont(font);
  pxHigh = metrics.height() + 2;
// pxWide = metrics.width(label);
  painter.drawText(pxOffset, pxHigh, label);

  // Draw slightly transparent lines
  //penWidth = 1;
  painter.setPen(QPen(QColor(0, 0, 0, 180), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  // Draw the X Axis
  painter.drawLine(pxOffset, pImageHeight / 2, pImageWidth - pxOffset, pImageHeight / 2);
  // Draw the Y Axis
  painter.drawLine(pImageWidth / 2, pyOffset, pImageWidth / 2, pImageHeight - pyOffset);

  painter.end();
  return pImage;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage PoleFigureImageUtilities::CreateQImageFromRgbaArray(UInt8ArrayType* poleFigurePtr, int imageDimension, bool includeOverlay)
{
  uint32_t* rgbaPtr = reinterpret_cast<uint32_t*>(poleFigurePtr->getPointer(0));

  // Create a QImage
  QImage image(imageDimension, imageDimension, QImage::Format_ARGB32_Premultiplied);

  size_t idx = 0;
  // Now copy the data from the rgbaImage into the QImage. We do this because QImage is very finicky about byte alignment
  for(int y = 0; y < imageDimension; ++y)
  {
    for (int x = 0; x < imageDimension; ++x)
    {
      idx = (imageDimension * y) + x;
      image.setPixel(x, y, rgbaPtr[idx]);
    }
  }

  // Flip the image so the (-1, -1) is in the lower left
  image = image.mirrored(false, true);

  QString imageLabel = (poleFigurePtr->getName());
  if(includeOverlay == true)
  {
    image = PoleFigureImageUtilities::PaintPoleFigureOverlay(imageDimension, imageDimension, imageLabel, image);
  }
  return image;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage PoleFigureImageUtilities::Create3ImagePoleFigure(UInt8ArrayType* i0, UInt8ArrayType* i1, UInt8ArrayType* i2,
                                                        PoleFigureConfiguration_t& config, int32_t layout)
{

  // Create a QImage that is the width of the first 2 images and the height of the first and third
  QImage img0 = PoleFigureImageUtilities::CreateQImageFromRgbaArray(i0, config.imageDim, true);
  QImage img1 = PoleFigureImageUtilities::CreateQImageFromRgbaArray(i1, config.imageDim, true);
  QImage img2 = PoleFigureImageUtilities::CreateQImageFromRgbaArray(i2, config.imageDim, true);
  // Create the Scalar Bar image
  QImage scalarBar = PoleFigureImageUtilities::GenerateScalarBar(img0.width(), img0.height(), config);

  int pImageWidth = 0;
  int pImageHeight = 0;

  QPoint pos1;
  QPoint pos2;
  QPoint pos3;
  QPoint pos4;

  if(layout == SIMPL::Layout::Horizontal)
  {
    pImageWidth = img0.width() * 4;
    pImageHeight = img0.height();
    pos1 = QPoint(0, 0);
    pos2 = QPoint(img0.width(), 0);
    pos3 = QPoint(img0.width() * 2, 0);
    pos4 = QPoint(img0.width() * 3, 0);
  }
  else if(layout == SIMPL::Layout::Vertical)
  {
    pImageWidth = img0.width();
    pImageHeight = img0.height() * 4;
    pos1 = QPoint(0, 0);
    pos2 = QPoint(0, img0.height());
    pos3 = QPoint(0, img0.height() * 2);
    pos4 = QPoint(0, img0.height() * 3);
  }
  else if(layout == SIMPL::Layout::Square)
  {
    pImageWidth = img0.width() + img1.width();
    pImageHeight = img0.height() + img2.height();
    pos1 = QPoint(0, 0);
    pos2 = QPoint(pImageWidth / 2, 0);
    pos3 = QPoint(0, pImageHeight / 2);
    pos4 = QPoint(pImageWidth / 2, pImageHeight / 2);
  }

  QImage pImage(pImageWidth, pImageHeight, QImage::Format_ARGB32_Premultiplied);
  pImage.fill(0xFFFFFFFF); // All white background

  // Create a Painter backed by a QImage to draw into
  QPainter painter;
  painter.begin(&pImage);
  painter.setRenderHint(QPainter::Antialiasing, true);

  painter.drawImage(pos1, img0); // Draw the first image in the upper Left
  painter.drawImage(pos2, img1); // Draw the second image in the upper right
  painter.drawImage(pos3, img2); // Draw the third image in the lower Left
  painter.drawImage(pos4, scalarBar); // Draw the Scalar Bar

  painter.end();
  // Scale the image down to 225 pixels
  return pImage;
}


