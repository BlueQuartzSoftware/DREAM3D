/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "PoleFigureImageUtilities.h"
#include <QtCore/QDebug>
#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QFont>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/ColorTable.h"
#include "DREAM3DLib/Utilities/ImageUtilities.h"

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
  m_KernelWeights.resize((kernelWidth+1) * (kernelHeight+1) * 4);


  float sigma = 1.0;
  float mu = 0.0;
  float root2Pi = sqrt(2.0* M_PI);
  float OneOverSigmaRoot2Pi = 1.0/(sigma * root2Pi);
  float TwoSigmaSqrd = 2.0*sigma*sigma;


  for (int ky = -kernelHeight; ky <= kernelHeight; ++ky)
  {
    for (int kx = -kernelWidth; kx <= kernelWidth; ++kx)
    {
      delta = (kx) * (kx) + (ky) * (ky) + 1;
      m_KernelWeights[index] = OneOverSigmaRoot2Pi*exp(-(delta-mu)*(delta-mu)/TwoSigmaSqrd );
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
                                                  QVector<qint32> &data,
                                                  QVector<qint32> &counts,
                                                  int kernelWidth, int kernelHeight, bool genmask)
{
  int xCoord, yCoord;
  int index = 0;


  // This figures out if we are "inside" the circle or not
  float radSqrd = (float)(imageWidth/2.0f) * (imageWidth/2.0f);
  float cX = imageWidth/2.0f;  // Center x
  float cY = imageHeight/2.0f; // Center y
  int targetIndex = (imageWidth * pY) + pX;
  float delta = (pX-cX)*(pX-cX) + (pY-cY)*(pY-cY);

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
        delta = (xCoord-cX)*(xCoord-cX) + (yCoord-cY)*(yCoord-cY);
        if (delta <= radSqrd) {
          counts[targetIndex] += ( data[index] /* m_KernelWeights[kernelWeightIndex] */ );
        }
      }
    }
  }
  return counts[targetIndex];
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage PoleFigureImageUtilities::generateColorPoleFigureImage(const PoleFigureData &config)
{

  int kernelWidth = config.kernelRadius[0];
  int kernelHeight = config.kernelRadius[1];
  int imageWidth = config.imageSize[0];
  int imageHeight = config.imageSize[1];

  //  std::cout << "Size: " << size << std::endl;

  // Generate the Kernel Weights
  generateKernelWeigths(kernelWidth, kernelHeight);


  // Allocate an array to hold the image data and initialize it to all zeros
  QVector<qint32> data(imageWidth * imageHeight, 1);
  QVector<qint32> counts(imageWidth * imageHeight, 0);

  QVector<float> xPoints = config.xData;
  QVector<float> yPoints = config.yData;
  int size = yPoints.size();

  // Find the max count
  qint32 max = -2147483646;
  qint32 min = 2147483647;

  // Loop over all the data and get the x,y coords (scaled) and increment the value (intensity) in that grid square
  for (int i = 0; i < size; ++i)
  {
    int xCoord = (xPoints[i] + 1) * imageWidth/2;
    int yCoord = (yPoints[i] + 1) * imageHeight/2;
    data[ yCoord * imageWidth + xCoord]++;
    if (data[ yCoord * imageWidth + xCoord] > max) { max = data[ yCoord * imageWidth + xCoord];}
    if (data[ yCoord * imageWidth + xCoord] < min) { min = data[ yCoord * imageWidth + xCoord];}

  }

  // std::cout << "Max Data[]: " << max << std::endl;

  qint32 value = 0;
  for (int yCoord = 0; yCoord < imageHeight; ++yCoord)
  {
    for (int xCoord = 0; xCoord < imageWidth; ++xCoord)
    {
      value = countPixelNeighbors(imageWidth, imageHeight, xCoord, yCoord, data, counts, kernelWidth, kernelHeight);
      if (value > max) { max = value;}
      if (value < min) { min = value;}
    }
  }



  value = 0;
  for (int yCoord = 0; yCoord < imageHeight; ++yCoord)
  {
    for (int xCoord = 0; xCoord < imageWidth; ++xCoord)
    {
      value = countPixelNeighbors(imageWidth, imageHeight, xCoord, yCoord, data, counts, kernelWidth, kernelHeight, true);
    }
  }

  if (max < 14) { max = 14; }
  QImage image (imageWidth, imageHeight, QImage::Format_ARGB32_Premultiplied);
  image.fill(0);
  qint32 numColors = max + 1;

  QVector<QColor> colorTable(numColors);
  qint32 range = max - min;

  float r, g, b;
  for (int i = 0; i < numColors; i++)
  {
    int val = min + ((float)i / numColors) * range;
    ColorTable::GetColorCorrespondingToValue(val, r, g, b, max, min);
    colorTable[i] = QColor(r*255, g*255, b*255, 255);
  }
  // Index 0 is all white which is every pixel outside of the ODF circle
  colorTable[0] = QColor(255, 255, 255, 255);

  for (int yCoord = 0; yCoord < imageHeight; ++yCoord)
  {
    for (int xCoord = 0; xCoord < imageWidth; ++xCoord)
    {
      quint32 colorIndex = counts[yCoord * imageWidth + xCoord];
      image.setPixel(xCoord, yCoord, colorTable[colorIndex].rgba());
    }
  }

  // Flip the image so the (-1, -1) is in the lower left
  image = image.mirrored(true, false);

  return PaintOverlay(config.imageSize[0], config.imageSize[1], config.label, image);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage PoleFigureImageUtilities::generatePoleFigureImage(const PoleFigureData &config)
{
  int imageWidth = config.imageSize[0];
  int imageHeight = config.imageSize[1];

  QImage image(imageWidth, imageHeight, QImage::Format_ARGB32_Premultiplied);


  QVector<float> xPoints = config.xData;
  QVector<float> yPoints = config.yData;
  int size = yPoints.size();

  QColor white(255, 255, 255, 255);
  QColor black(0.25*255, 0.2549*255, 0.7961*255, 255);
  image.fill(white.rgba()); // Fill all white
  QRgb black_Rgba = black.rgba();

  // Flip the pixels to black that came from the data xy coords
  for (int i = 0; i < size; ++i)
  {
    //    float xp = xPoints[i];
    //    float yp = yPoints[i];
    int xCoord = (xPoints[i] + 1) * imageWidth/2;
    int yCoord = (yPoints[i] + 1) * imageHeight/2;
    if (xCoord > imageWidth || yCoord > imageHeight)
    {
      qDebug() << "This is bad";
    }
    image.setPixel(xCoord, yCoord, black_Rgba);
  }

  // Flip the image so the (-1, -1) is in the lower left
  image = image.mirrored(true, false);

  return PoleFigureImageUtilities::PaintOverlay(config.imageSize[0], config.imageSize[1], config.label, image);

}
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void drawScaleBar(QPainter &painter, int imageWidth, int imageHeight, PoleFigureConfiguration_t &config)
{
  int numColors = config.numColors;
  int scaleHeight = imageHeight * 0.25;
  int scaleWidth = 50;
  int colorWidth = int(scaleHeight/numColors);
  scaleHeight = colorWidth*numColors;

  QPointF topLeft(imageWidth * .625, imageHeight * .625);

  QSizeF size(scaleWidth, scaleHeight);

  //Get all the colors that we will need

  QVector<DREAM3D::Rgb> colorTable(numColors);
  QVector<float> colors(3*numColors, 0.0);
  ColorTable::GetColorTable(numColors, colors);

  float r=0.0, g=0.0, b=0.0;

  for (int i = 0; i < numColors; i++)
  {
    r = colors[3*i];
    g = colors[3*i+1];
    b = colors[3*i+2];
    colorTable[i] = RgbColor::dRgb(r*255, g*255, b*255, 255);
  }

  int penWidth = 1;

  // Now start from the top and draw colored lines down the scale bar
  int yLinePos = topLeft.y() + size.height();
  QPointF start = topLeft;
  QPointF end = topLeft;
  QRectF scaleBorder(topLeft, size);

  for(int i = 0; i < numColors; ++i)
  {
    for(int j = 0; j < colorWidth; j++)
    {
      QColor c(colorTable[i]);
      painter.setPen(QPen(c, penWidth, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
      start.setY(yLinePos);
      end.setX(topLeft.x() + scaleWidth);
      end.setY(yLinePos);
      painter.drawLine(start, end);
      yLinePos--;
    }
  }

  qRgba(10, 20, 30, 255);
  // Draw the border of the scale bar
  penWidth = 2;
  painter.setPen(QPen(QColor(0, 0, 0, 255), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  painter.drawRect(scaleBorder);

  // Draw some more information to the right of the Scale Bar
  QString maxStr = QString::number(config.maxScale, 'f', 3);
  painter.drawText(topLeft.x() + scaleWidth + 10, topLeft.y(), maxStr);

  QString minStr = QString::number(config.minScale, 'f', 3);
  painter.drawText(topLeft.x() + scaleWidth + 10, topLeft.y() + size.height(), minStr);

  QFontMetrics metrics = painter.fontMetrics();
  int  fontHigh = metrics.height();
  int  labelWidth = metrics.width(maxStr);
  if (labelWidth < metrics.width(minStr))
  {
    labelWidth = metrics.width(minStr);
  }

  QPointF statsPoint(topLeft.x() + scaleWidth + 10 + labelWidth + 10, imageHeight * .625);

  // Draw some more Statistics Text
  QString label("Upper & Lower");
  painter.drawText(statsPoint, label);

  label = QString("Samples: ") + QString::number(config.eulers->GetNumberOfTuples());
  statsPoint.setY( statsPoint.y() + fontHigh*1.5);
  painter.drawText(statsPoint, label);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage PoleFigureImageUtilities::PaintPoleFigureOverlay(int imageWidth, int imageHeight, QString label, QImage image)
{
  int pxHigh = 0;
  int pxWide = 0;

  QFont font("Ariel", 24, QFont::Bold);
  {
    QPainter painter;
    QImage pImage(100, 100, QImage::Format_ARGB32_Premultiplied);
    pImage.fill(0xFFFFFFFF); // All white background
    painter.begin(&pImage);

    painter.setFont(font);
    QFontMetrics metrics = painter.fontMetrics();
    pxHigh = metrics.height();
    pxWide = metrics.width(QString("Y"));
    painter.end();
  }


  int pxOffset = 2 * pxWide;
  int pyOffset = 2 * pxHigh;
  // Get a QPainter object to add some more details to the image


  int pImageWidth = imageWidth + pxOffset * 2;
  int pImageHeight = imageHeight + pyOffset * 2;

  QImage pImage(pImageWidth, pImageHeight, QImage::Format_ARGB32_Premultiplied);
  pImage.fill(0xFFFFFFFF); // All white background

  // Create a Painter backed by a QImage to draw into
  QPainter painter;
  painter.begin(&pImage);
  painter.setRenderHint(QPainter::Antialiasing, true);

  painter.setFont(font);
  QFontMetrics metrics = painter.fontMetrics();
  pxHigh = metrics.height();
  pxWide = metrics.width(QString("Y"));

  // Draw the Pole Figure into the center of the canvas
  QPoint point(pxOffset, pyOffset);
  painter.drawImage(point, image);

  qint32 penWidth = 2;
  painter.setPen(QPen(QColor(0, 0, 0, 255), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

  // Draw the Outer circular border around the pole figure
  QPainterPath circle;
  QPointF center(pImageWidth / 2, pImageHeight / 2);
  circle.addEllipse(center, imageWidth / 2, imageHeight / 2);
  painter.drawPath(circle);

  // Label the X Axis
  painter.drawText(pImageWidth - 2*pxWide +4, pImageHeight / 2 + pxHigh / 3, "X");
  // Label the Y Axis
  pxWide = metrics.width(QString("Y"));
  painter.drawText(pImageWidth / 2 - pxWide/2, pyOffset - 4, "Y");

  // Draw the name of the Pole Figure
  pxWide = metrics.width(label);
  painter.drawText(pImageWidth/4, pxHigh, label);

  // Draw slightly transparent lines
  penWidth = 1;
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
  uint32_t* rgbaPtr = reinterpret_cast<uint32_t*>(poleFigurePtr->GetPointer(0));

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

  QString imageLabel = QString::fromStdString(poleFigurePtr->GetName());
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
                                                        PoleFigureConfiguration_t &config)
{

  // Create a QImage that is the width of the first 2 images and the height of the first and third
  QImage img0 = PoleFigureImageUtilities::CreateQImageFromRgbaArray(i0, config.imageDim, true);
  QImage img1 = PoleFigureImageUtilities::CreateQImageFromRgbaArray(i1, config.imageDim, true);
  QImage img2 = PoleFigureImageUtilities::CreateQImageFromRgbaArray(i2, config.imageDim, true);


  int pImageWidth = img0.width() + img1.width();
  int pImageHeight = img0.height() + img2.height();

  QImage pImage(pImageWidth, pImageHeight, QImage::Format_ARGB32_Premultiplied);
  pImage.fill(0xFFFFFFFF); // All white background

  // Create a Painter backed by a QImage to draw into
  QPainter painter;
  painter.begin(&pImage);
  painter.setRenderHint(QPainter::Antialiasing, true);

  QFont font("Ariel", 18, QFont::Bold);
  painter.setFont(font);
  QFontMetrics metrics = painter.fontMetrics();
//  int pxHigh = metrics.height();
//  int pxWide = metrics.width(QString("Y"));

  painter.drawImage(QPoint(0, 0), img0); // Draw the first image in the upper Left
  painter.drawImage(QPoint(pImageWidth/2, 0), img1); // Draw the first image in the upper Left
  painter.drawImage(QPoint(0, pImageHeight/2), img2); // Draw the first image in the upper Left

  drawScaleBar(painter, pImageWidth, pImageHeight, config);

  painter.end();
  // Scale the image down to 225 pixels
  return pImage;
}




