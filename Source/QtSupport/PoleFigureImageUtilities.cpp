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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage PoleFigureImageUtilities::CreateQImage(DoubleArrayType *poleFigurePtr, int imageDimension, int nColors, QString label, bool includeOverlay)
{
  size_t npoints = poleFigurePtr->GetNumberOfTuples();
  double max = std::numeric_limits<double>::min();
  double min = std::numeric_limits<double>::max();
  double value = 0.0;
  for(size_t i = 0; i < npoints; ++i)
  {
    value = poleFigurePtr->GetValue(i);
    if (value < 0) {continue;}
    if (value > max) { max = value;}
    if (value < min) { min = value;}
  }

  //std::cout << "Min: " << min << "   Max: " << max << std::endl;

  int xpoints = imageDimension;
  int ypoints = imageDimension;

  int xpointshalf = xpoints / 2;
  int ypointshalf = ypoints / 2;

  float xres = 2.0 / (float)(xpoints);
  float yres = 2.0 / (float)(ypoints);
  float xtmp, ytmp;

  //if (max < 14) { max = 14; }
  QImage image (xpoints, ypoints, QImage::Format_ARGB32_Premultiplied);
  image.fill(0);
  // qint32 numColors = max + 1;
  qint32 numColors = nColors + 1;
  // qint32 colorRange = getNumColors();

  QVector<QColor> colorTable(numColors);
  //qint32 range = max - min;

  float r, g, b;
  for (int i = 0; i < nColors; i++)
  {
    //int val = min + ((float)i / numColors) * range;
    //int val = ((float)i / (float)numColors) * colorRange;
    ColorTable::GetColorCorrespondingToValue(i, r, g, b, nColors, 0);
    colorTable[i] = QColor(r*255, g*255, b*255, 255);
  }
  // Index 0 is all white which is every pixel outside of the Pole Figure circle
  colorTable[nColors] = QColor(255, 255, 255, 255);

  //*********************** NOTE ************************************
  // In the below loop over the Pole Figure Image we are swapping the
  // X & Y coordinates when we place the RGBA value into the image. This
  // is because for some reason the data is rotated 90 degrees. Since
  // the image is square we can easily do this swap and effectively
  // rotate the image 90 degrees.
  for (int64_t y = 0; y < ypoints; y++)
  {
    for (int64_t x = 0; x < xpoints; x++)
    {
      xtmp = float(x-xpointshalf)*xres+(xres * 0.5);
      ytmp = float(y-ypointshalf)*yres+(yres * 0.5);
      if( ( xtmp * xtmp + ytmp * ytmp) <= 1.0) // Inside the circle
      {
        qint32 cindex = qint32(poleFigurePtr->GetValue(y * xpoints + x));
        qint32 colorIndex = (cindex-min) / (max - min) * nColors;
        image.setPixel(x, y, colorTable[colorIndex].rgba());
      }
      else // Outside the Circle - Set pixel to White
      {
        image.setPixel(x, y, colorTable[nColors].rgba());
      }
    }
  }

  // Flip the image so the (-1, -1) is in the lower left
  image = image.mirrored(false, true);

  QString imageLabel = "<" + label + ">";
  if(includeOverlay == true)
  {
    image = PoleFigureImageUtilities::PaintOverlay(xpoints, ypoints, imageLabel, image);
  }
  return image;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage PoleFigureImageUtilities::PaintOverlay(int imageWidth, int imageHeight, QString label, QImage image)
{
  int pxHigh = 0;
  int pxWide = 0;

  QFont font("Ariel", 18, QFont::Bold);
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

  painter.drawText(pImageWidth - 2*pxWide +4, pImageHeight / 2 + pxHigh / 3, "RD");

  pxWide = metrics.width(QString("RD"));
  painter.drawText(pImageWidth / 2 - pxWide / 2, pImageHeight - pyOffset + pxHigh + 2, "TD");

  pxWide = metrics.width(label);
  painter.drawText(pImageWidth / 2 - pxWide / 2, pxHigh, label);

  // Draw slightly transparent lines
  penWidth = 1;
  painter.setPen(QPen(QColor(0, 0, 0, 180), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  painter.drawLine(pxOffset, pImageHeight / 2, pImageWidth - pxOffset, pImageHeight / 2);
  painter.drawLine(pImageWidth / 2, pyOffset, pImageWidth / 2, pImageHeight - pyOffset);

  painter.end();
  // Scale the image down to 225 pixels
  return pImage;
}


