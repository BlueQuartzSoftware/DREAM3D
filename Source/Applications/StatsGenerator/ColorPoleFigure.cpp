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

#include "ColorPoleFigure.h"

#include <iostream>

#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QFont>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ColorPoleFigure::ColorPoleFigure() :
m_KernelWeightsInited(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ColorPoleFigure::~ColorPoleFigure()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ColorPoleFigure::generateKernelWeigths(int kernelWidth, int kernelHeight)
{
//  int pX = 0;
//  int pY = 0;
  int index = 0;
  float delta = 0;
  m_KernelWeights.resize((kernelWidth+1) * (kernelHeight+1) * 4);
//  int maxDelta = kernelWidth * kernelWidth + kernelHeight * kernelHeight;

  for (int ky = -kernelHeight; ky <= kernelHeight; ++ky)
  {
    for (int kx = -kernelWidth; kx <= kernelWidth; ++kx)
    {
      delta = (kx) * (kx) + (ky) * (ky) + 1;
      m_KernelWeights[index] = 1/delta * 512.0f;
   //   printf("%04d ", m_KernelWeights[index]);
      ++index;
    }
  //  printf("\n");
  }
  m_KernelWeightsInited = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ColorPoleFigure::countPixelNeighbors(int imageWidth, int imageHeight,
                                           int pX, int pY,
                                           QVector<qint8> &data,
                                           QVector<qint32> &counts,
                                           int kernelWidth, int kernelHeight)
{
  int xCoord, yCoord;
  int index = 0;


  // This figures out if we are "inside" the circle or not
  float radSqrd = (float)(imageWidth/2.0f) * (imageWidth/2.0f);
  float cX = imageWidth/2.0f;  // Center x
  float cY = imageHeight/2.0f; // Center y
  int targetIndex = (imageWidth * pY) + pX;
  float delta = (pX-cX)*(pX-cX) + (pY-cY)*(pY-cY);
  if (delta > radSqrd)
  {
    counts[targetIndex] = 0;
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
          counts[targetIndex] += m_KernelWeights[kernelWeightIndex];
        }
      }
    }
  }
  return counts[targetIndex];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage ColorPoleFigure::generateImage(const PoleFigureData &config)
{

int kernelWidth = config.kernelRadius[0];
 int kernelHeight = config.kernelRadius[1];
int imageWidth = config.imageSize[0];
int imageHeight = config.imageSize[1];

  //  std::cout << "Size: " << size << std::endl;

  // Generate the Kernel Weights
    generateKernelWeigths(kernelWidth, kernelHeight);


    // Allocate an array to hold the image data and initialize it to all zeros
    QVector<qint8> data(imageWidth * imageHeight, 0);
    QVector<qint32> counts(imageWidth * imageHeight, 1);

    float* xPoints = config.xData;
    float* yPoints = config.yData;
    int size = config.size;


    // Loop over all the data and get the x,y coords (scaled) and turn on the data pixel
    // This discretizes the data onto the pixel grid
    for (int i = 0; i < size; ++i)
    {
      int xCoord = (xPoints[i] + 1) * imageWidth/2;
      int yCoord = (yPoints[i] + 1) * imageHeight/2;
      data[ yCoord * imageWidth + xCoord] = 1;
    }


    // Now find the neighbors
    // Find the max count
    qint32 max = -2147483646;
    qint32 min = 2147483647;
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

//    std::cout << "Max: " << max << std::endl;
//    std::cout << "Min: " << min << std::endl;
    // Scale all the counts to between 0 and 255
    QImage image (imageWidth, imageHeight, QImage::Format_ARGB32_Premultiplied);
    image.fill(0);
    qint32 numColors = max + 1;
   // image.setColorCount(numColors);

    QVector<QColor> colorTable(numColors);
    qint32 range = max - min;

    float r, g, b;
    for (int i = 0; i < numColors; i++)
    {
      int val = min + ((float)i / numColors) * range;
      getColorCorrespondingTovalue(val, r, g, b, max, min);
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

    // Get a QPainter object to add some more details to the image
    QPainter painter;
  //  painter.setRenderHint(QPainter::Antialiasing, true);




    int pxOffset = 30;
    int pyOffset = 30;

    int pImageWidth = imageWidth + pxOffset*2;
    int pImageHeight = imageHeight + pyOffset*2;

    QImage pImage(pImageWidth, pImageHeight,  QImage::Format_ARGB32_Premultiplied);
    pImage.fill(0xFFFFFFFF); // All white background
    painter.begin(&pImage);

    QFont font("Times", 18, QFont::Bold);
    painter.setFont(font);
    QFontMetrics metrics = painter.fontMetrics();
    int pxHigh = metrics.height();
    int pxWide = metrics.width(QString("TD"));

    QPoint point(pxOffset,pyOffset);
    painter.drawImage(point, image);

    qint32 penWidth = 2;
    painter.setPen(QPen(QColor(0, 0, 0, 255), penWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));

    QPainterPath circle;
    QPointF center(pImageWidth/2, pImageHeight/2);
    circle.addEllipse(center, imageWidth/2, imageHeight/2);
    painter.drawPath(circle);

    painter.drawText(pImageWidth-pxWide-2, pImageHeight/2 + pxHigh/2, "TD");

    pxWide = metrics.width(QString("RD"));
    painter.drawText(pImageWidth/2-pxWide/2, pImageHeight-pyOffset+pxHigh+2, "RD");


    pxWide = metrics.width(config.label);
    painter.drawText(2, pImageHeight-pyOffset+pxHigh+2, config.label);

    // Draw slightly transparent lines
    painter.setPen(QPen(QColor(0, 0, 0, 180), penWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.drawLine(pImageWidth/2, pImageHeight/2, pImageWidth-pxOffset, pImageHeight/2);
    painter.drawLine(pImageWidth/2, pImageHeight/2, pImageWidth/2, pImageHeight-pyOffset);


    painter.end();

    return pImage;
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
void ColorPoleFigure::getColorCorrespondingTovalue(float val,
                                                     float &r, float &g, float &b,
                                                     float max, float min)
{
  static const int numColorNodes = 4;
  float color[numColorNodes][3] =
  {
        {0.25, 0.2549, 0.7961},    // blue
        {0.8274, 0.8039, 0.0941},    // yellow
        {0.1803, 0.6655, 0.1490},    // Green
        {1.0, 0.0, 0.0}     // red
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

