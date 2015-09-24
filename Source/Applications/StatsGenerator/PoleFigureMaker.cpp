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


#include "PoleFigureMaker.h"

#include <iostream>

#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QFont>

#include "SIMPLib/Math/SIMPLibMath.h"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PoleFigureMaker::PoleFigureMaker() :
  m_KernelWeightsInited(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PoleFigureMaker::~PoleFigureMaker()
{
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PoleFigureMaker::generateKernelWeigths(int kernelWidth, int kernelHeight)
{
  //  int pX = 0;
  //  int pY = 0;
  int index = 0;
  float delta = 0;
  m_KernelWeights.resize((kernelWidth + 1) * (kernelHeight + 1) * 4);
  //  int maxDelta = kernelWidth * kernelWidth + kernelHeight * kernelHeight;

  for (int ky = -kernelHeight; ky <= kernelHeight; ++ky)
  {
    for (int kx = -kernelWidth; kx <= kernelWidth; ++kx)
    {
      delta = (kx) * (kx) + (ky) * (ky) + 1;
      m_KernelWeights[index] = 1 / delta * 512.0f;
      //   printf("%04d ", m_KernelWeights[index]);
      ++index;
    }
    //  printf("\n");
  }
  m_KernelWeightsInited = true;
}
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PoleFigureMaker::generateKernelWeigths(int kernelWidth, int kernelHeight)
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
int PoleFigureMaker::countPixelNeighbors(int imageWidth, int imageHeight,
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
QImage PoleFigureMaker::generateColorPoleFigureImage(const PoleFigureData& config)
{

  int kernelWidth = config.kernelRadius[0];
  int kernelHeight = config.kernelRadius[1];
  int imageWidth = config.imageSize[0];
  int imageHeight = config.imageSize[1];

  //  qDebug() << "Size: " << size << "\n";

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

  // Loop over all the data and get the x,y coords (scaled) and turn on the data pixel
  // This discretizes the data onto the pixel grid
  for (int i = 0; i < size; ++i)
  {
    int xCoord = (xPoints[i] + 1) * imageWidth / 2;
    int yCoord = (yPoints[i] + 1) * imageHeight / 2;
    data[ yCoord * imageWidth + xCoord]++;
    if (data[ yCoord * imageWidth + xCoord] > max) { max = data[ yCoord * imageWidth + xCoord];}
    if (data[ yCoord * imageWidth + xCoord] < min) { min = data[ yCoord * imageWidth + xCoord];}

  }

// qDebug() << "Max Data[]: " << max << "\n";

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
    getColorCorrespondingTovalue(val, r, g, b, max, min);
    colorTable[i] = QColor(r * 255, g * 255, b * 255, 255);
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

  return paintImage(config.imageSize[0], config.imageSize[1], config.label, image);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage PoleFigureMaker::generatePoleFigureImage(const PoleFigureData& config)
{
  int imageWidth = config.imageSize[0];
  int imageHeight = config.imageSize[1];

  QImage image(imageWidth, imageHeight, QImage::Format_ARGB32_Premultiplied);


  QVector<float> xPoints = config.xData;
  QVector<float> yPoints = config.yData;
  int size = yPoints.size();

  QColor white(255, 255, 255, 255);
  QColor black(0.25 * 255, 0.2549 * 255, 0.7961 * 255, 255);
  image.fill(white.rgba()); // Fill all white
  QRgb black_Rgba = black.rgba();

  // Flip the pixels to black that came from the data xy coords
  for (int i = 0; i < size; ++i)
  {
    //    float xp = xPoints[i];
    //    float yp = yPoints[i];
    int xCoord = (xPoints[i] + 1) * imageWidth / 2;
    int yCoord = (yPoints[i] + 1) * imageHeight / 2;
    if (xCoord > imageWidth || yCoord > imageHeight)
    {
      qDebug() << "This is bad" << "\n";
    }
    image.setPixel(xCoord, yCoord, black_Rgba);
  }

  // Flip the image so the (-1, -1) is in the lower left
  image = image.mirrored(true, false);

  return paintImage(config.imageSize[0], config.imageSize[1], config.label, image);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage PoleFigureMaker::paintImage(int imageWidth, int imageHeight, QString label, QImage image)
{
  int pxHigh = 0;
  int pxWide = 0;

  QFont font("Ariel", 16, QFont::Bold);
  {
    QPainter painter;
    QImage pImage(100, 100, QImage::Format_ARGB32_Premultiplied);
    pImage.fill(0xFFFFFFFF); // All white background
    painter.begin(&pImage);

    painter.setFont(font);
    QFontMetrics metrics = painter.fontMetrics();
    pxHigh = metrics.height();
    pxWide = metrics.width(QString("TD"));
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
  pxWide = metrics.width(QString("TD"));

  QPoint point(pxOffset, pyOffset);
  painter.drawImage(point, image); // Draw the image we just generated into the QPainter's canvas

  qint32 penWidth = 2;
  painter.setPen(QPen(QColor(0, 0, 0, 255), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

  QPainterPath circle;
  QPointF center(pImageWidth / 2, pImageHeight / 2);
  circle.addEllipse(center, imageWidth / 2, imageHeight / 2);
  painter.drawPath(circle);

  painter.drawText(pImageWidth - 2 * pxWide + 4, pImageHeight / 2 + pxHigh / 3, "TD");

  pxWide = metrics.width(QString("RD"));
  painter.drawText(pImageWidth / 2 - pxWide / 2, pImageHeight - pyOffset + pxHigh + 2, "RD");

  pxWide = metrics.width(label);
  painter.drawText(2, pxHigh, label);

  // Draw slightly transparent lines
  penWidth = 1;
  painter.setPen(QPen(QColor(0, 0, 0, 180), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  painter.drawLine(pxOffset, pImageHeight / 2, pImageWidth - pxOffset, pImageHeight / 2);
  painter.drawLine(pImageWidth / 2, pyOffset, pImageWidth / 2, pImageHeight - pyOffset);

  painter.end();
  // Scale the image down to 225 pixels
  return pImage;//.scaled(225, 225, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

///getColorCorrespondingToValue ////////////////////////////////////////////////
//
// Assumes you've already generated min and max -- the extrema for the data
// to which you're applying the color map. Then define the number of colorNodes
// and make sure there's a row of three float values (representing r, g, and b
// in a 0.0-1.0 range) for each node. Then call this method for with parameter
// val some float value between min and max inclusive. The corresponding rgb
// values will be returned in the reference-to-float parameters r, g, and b.
//
////////////////////////////////////////////////////////////////////////////////
void PoleFigureMaker::getColorCorrespondingTovalue(float val,
                                                   float& r, float& g, float& b,
                                                   float max, float min)
{
  static const int numColorNodes = 8;
  float color[numColorNodes][3] =
  {
    {0.0f, 1.0f / 255.0f, 253.0f / 255.0f}, // blue
    {105.0f / 255.0f, 145.0f / 255.0f, 2.0f / 255.0f}, // yellow
    {1.0f / 255.0f, 255.0f / 255.0f, 29.0f / 255.0f}, // Green
    {180.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f},
    {255.0f / 255.0f, 215.0f / 255.0f, 6.0f / 255.0f},
    {255.0f / 255.0f, 143.0f / 255.0f, 1.0f / 255.0f},
    {255.0f / 255.0f, 69.0f / 255.0f, 0.0f / 255.0f},
    {253.0f / 255.0f, 1.0f / 255.0f, 0.0f / 255.0f} // red
  };
  float range = max - min;
  for (int i = 0; i < (numColorNodes - 1); i++)
  {
    float currFloor = min + ((float)i / (numColorNodes - 1)) * range;
    float currCeil = min + ((float)(i + 1) / (numColorNodes - 1)) * range;

    if((val >= currFloor) && (val <= currCeil))
    {
      float currFraction = (val - currFloor) / (currCeil - currFloor);
      r = color[i][0] * (1.0 - currFraction) + color[i + 1][0] * currFraction;
      g = color[i][1] * (1.0 - currFraction) + color[i + 1][1] * currFraction;
      b = color[i][2] * (1.0 - currFraction) + color[i + 1][2] * currFraction;
    }
  }
}

