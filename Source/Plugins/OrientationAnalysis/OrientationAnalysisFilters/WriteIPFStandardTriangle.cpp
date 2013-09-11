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
#include "WriteIPFStandardTriangle.h"

#include <stdio.h>

#include <iostream>
#include <string>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QDebug>

#include <QtGui/QPainter>
#include <QtGui/QFont>
#include <QtGui/QImage>
#include <QtGui/QColor>


#include "QtSupport/PoleFigureImageUtilities.h"


#include "MXA/MXA.h"
#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/MXAFileInfo.h"

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/HKL/CtfReader.h"


#include "DREAM3DLib/Common/ModifiedLambertProjection.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteIPFStandardTriangle::WriteIPFStandardTriangle() :
  AbstractFilter(),
  m_OutputFile(""),
  m_ImageFormat(2),
  m_ImageSize(512),
  m_ColorCorrectionFactor(1.0)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteIPFStandardTriangle::~WriteIPFStandardTriangle()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  /* Place all your option initialization code here */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Image Format");
    parameter->setPropertyName("ImageFormat");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("tif");
    choices.push_back("bmp");
    choices.push_back("png");
    //   choices.push_back("jpg");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setPropertyName("ImageSize");
    option->setHumanLabel("Image Size (Square)");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Pixels");
    parameters.push_back(option);
  }
//  {
//    FilterParameter::Pointer option = FilterParameter::New();
//    option->setPropertyName("ColorCorrectionFactor");
//    option->setHumanLabel("Color Correction Factor");
//    option->setUnits("0 <= x < 1");
//    option->setWidgetType(FilterParameter::DoubleWidget);
//    option->setValueType("float");
//    parameters.push_back(option);
//  }


  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputFile( reader->readValue("OutputFile", getOutputFile()));
  setImageFormat( reader->readValue("ImageFormat", getImageFormat()));
  setImageSize( reader->readValue("ImageSize", getImageSize()));
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteIPFStandardTriangle::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile() );
  writer->writeValue("ImageFormat", getImageFormat() );
  writer->writeValue("ImageSize", getImageSize() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  
  //VolumeDataContainer* m = getVolumeDataContainer();
  /* Example code for preflighting looking for a valid string for the output file
   * but not necessarily the fact that the file exists: Example code to make sure
   * we have something in a string before proceeding.*/

  if (m_OutputFile.empty() == true)
  {
    setErrorCondition(-1003);
    addErrorMessage(getHumanLabel(), "Output File is Not set correctly", getErrorCondition());
  }

  if (MXAFileInfo::isDirectory(m_OutputFile) == true)
  {
    setErrorCondition(-1004);
    addErrorMessage(getHumanLabel(), "The path for the output file is a directory. Please specify an output file and not a directory.", getErrorCondition());
  }

  if (m_ImageSize == 0)
  {
    setErrorCondition(-1005);
    addErrorMessage(getHumanLabel(), "The size of the image is Zero and must be a positive integer.", getErrorCondition());
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::execute()
{
  int err = 0;
  setErrorCondition(err);

  QImage image = generateCubicHighTriangle();
  writeImage(image);


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage WriteIPFStandardTriangle::generateCubicHighTriangle()
{
  notifyStatusMessage("Generating Cubic IPF Triangle Legend");

  CubicOps ops;
  UInt8ArrayType::Pointer rgbaImage = ops.generateIPFTriangleLegend(getImageSize());
  QRgb* rgba = reinterpret_cast<QRgb*>(rgbaImage->GetPointer(0));

  QImage image(getImageSize(), getImageSize(), QImage::Format_ARGB32_Premultiplied);

  int xDim = getImageSize();
  int yDim = getImageSize();
  size_t idx = 0;

  for(int y = 0; y < yDim; ++y)
  {
    for(int x = 0; x < xDim; ++x)
    {
      idx = (y*xDim) + x;
      image.setPixel(x, y, rgba[idx]);
    }
  }

  image = overlayCubicHighText(image);
  return image;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage WriteIPFStandardTriangle::overlayCubicHighText(QImage image)
{
  QSize imageSize(getImageSize(), getImageSize());
  int fontHeight = 0;
  int fontWidth = 0;

  int fontScale = 24.0/256.0 * getImageSize(); // At 256 Pixel Image, we want to use 24 Point font
  if (fontScale < 10) { fontScale = 10; } // Do not use fonts below 10Point.


  QFont font("Ariel", fontScale, QFont::Bold);
  {
    QPainter painter;
    QImage pImage(100, 100, QImage::Format_ARGB32_Premultiplied);
    pImage.fill(0xFFFFFFFF); // All white background
    painter.begin(&pImage);

    painter.setFont(font);
    QFontMetrics metrics = painter.fontMetrics();
    fontHeight = metrics.height();
    fontWidth = metrics.width(QString("[0000]"));
    painter.end();
  }

  int yMargin = 10;

  int pImageWidth = imageSize.width() + yMargin;
  int pImageHeight = imageSize.height() + fontHeight * 2;

  QImage pImage(pImageWidth, pImageHeight, QImage::Format_ARGB32_Premultiplied);
  pImage.fill(0xFFFFFFFF); // All white background

  // Create a Painter backed by a QImage to draw into
  QPainter painter;
  painter.begin(&pImage);
  painter.setRenderHint(QPainter::Antialiasing, true);

  painter.setFont(font);
  QFontMetrics metrics = painter.fontMetrics();

  // Draw the Figure into the upper left of the enlarged image so all the extra space is at the bottom
  QPoint point(yMargin/2, 0);
  painter.drawImage(point, image);

  qint32 penWidth = 2;
  painter.setPen(QPen(QColor(0, 0, 0, 255), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

  // Draw the [111] label in the Upper Right corner
  QString label("[111]");
  fontWidth = metrics.width(label);
  fontHeight = metrics.height();
  painter.drawText( pImageWidth - (fontWidth * 1.25), fontHeight * 1.10, label);

  label = QString("[101]");
  fontWidth = metrics.width(label);
  fontHeight = metrics.height();
  painter.drawText( pImageWidth - (fontWidth * 1.25), pImageHeight - fontHeight, label);


  label = QString("[001]");
  fontWidth = metrics.width(label);
  fontHeight = metrics.height();
  painter.drawText( 10, pImageHeight - fontHeight, label);


  label = QString("Cubic m-3m");
  fontWidth = metrics.width(label);
  fontHeight = metrics.height();
  painter.drawText( 10, fontHeight*1.10, label);

  return pImage;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteIPFStandardTriangle::writeImage( QImage &image)
{
  
  ss << "Writing Image " << m_OutputFile;
  notifyStatusMessage(ss.str());

  QFileInfo fi(QString::fromStdString(m_OutputFile));
  QDir parent(fi.absolutePath());
  if (parent.exists() == false)
  {
    parent.mkpath(fi.absolutePath());
  }

  bool saved = image.save(QString::fromStdString(m_OutputFile));
  if(!saved)
  {
    setErrorCondition(-90011);
    ss.str("");
    ss << "The Triangle image file '" << m_OutputFile << "' was not saved.";
    notifyErrorMessage(ss.str(), getErrorCondition());
  }
}

