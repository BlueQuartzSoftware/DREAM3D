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
#include "GenerateIPFTriangle.h"

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
GenerateIPFTriangle::GenerateIPFTriangle() :
  AbstractFilter(),
  m_OutputFile(""),
  m_ImageFormat(0),
  m_ImageSize(1024),
  m_ColorCorrectionFactor(1.0)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateIPFTriangle::~GenerateIPFTriangle()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFTriangle::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
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
    std::vector<std::string> choices;
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
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setPropertyName("ColorCorrectionFactor");
    option->setHumanLabel("Color Correction Factor");
    option->setUnits("0 <= x < 1");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    parameters.push_back(option);
  }


  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFTriangle::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int GenerateIPFTriangle::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
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
void GenerateIPFTriangle::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  //VoxelDataContainer* m = getVoxelDataContainer();
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

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFTriangle::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFTriangle::execute()
{
  int err = 0;
  setErrorCondition(err);


  generateCubicTriangle();

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFTriangle::writeImage( QImage &image)
{
  std::stringstream ss;
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

/**
 * @brief Sorts the 3 values from low to high
 * @param a
 * @param b
 * @param c
 * @param sorted The array to store the sorted values.
 */
template<typename T>
void _TripletSort(T a, T b, T c, T* sorted)
{
  if ( a > b && a > c)
  {
    sorted[2] = a;
    if (b > c) { sorted[1] = b; sorted[0] = c; }
    else { sorted[1] = c; sorted[0] = b; }
  }
  else if ( b > a && b > c)
  {
    sorted[2] = b;
    if (a > c) { sorted[1] = a; sorted[0] = c; }
    else { sorted[1] = c; sorted[0] = a; }
  }
  else if ( a > b )
  {
    sorted[1] = a; sorted[0] = b; sorted[2] = c;
  }
  else if (a >= c && b >=c)
  {
    sorted[0] = c; sorted[1] = a; sorted[2] = b;
  }
  else
  { sorted[0] = a; sorted[1] = b; sorted[2] = c;}
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFTriangle::findColor(float x, float y, float z, unsigned char* rgb)
{
  float cd[3];
  float d[3];

  float _rgb[3] = { 0.0, 0.0, 0.0 };

//3) move that direction to a single standard triangle - using the 001-011-111 triangle)
  cd[0] = fabs(x);
  cd[1] = fabs(y);
  cd[2] = fabs(z);

  // Sort the cd array from smallest to largest
  _TripletSort(cd[0], cd[1], cd[2], cd);

  float theta = (cd[0] * 0) + (cd[1] * -DREAM3D::Constants::k_HalfSqrt2) + (cd[2] * DREAM3D::Constants::k_HalfSqrt2);
  theta = (DREAM3D::Constants::k_RadToDeg) * acos(theta);
  _rgb[0] = (90.0f - theta) / 45.0f;
  d[0] = (cd[1] * 1) - (cd[2] * 0);
  d[1] = (cd[2] * 0) - (cd[0] * 1);
  d[2] = (cd[0] * 0) - (cd[1] * 0);
  d[0] = -(d[1] + d[2]) / d[0];
  d[1] = 1;
  d[2] = 1;
  float norm = sqrt(((d[0] * d[0]) + (d[1] * d[1]) + (d[2] * d[2])));
  d[0] = d[0] / norm;
  d[1] = d[1] / norm;
  d[2] = d[2] / norm;
  float phi_local = (d[0] * 0) + (d[1] * DREAM3D::Constants::k_HalfSqrt2) + (d[2] * DREAM3D::Constants::k_HalfSqrt2);
  phi_local = (DREAM3D::Constants::k_RadToDeg) * acos(phi_local);
  _rgb[1] = (1 - _rgb[0]) * ((35.26f - phi_local) / 35.26f);
  _rgb[2] = (1 - _rgb[0]) - _rgb[1];
  float max = _rgb[0];
  if (_rgb[1] > max) max = _rgb[1];
  if (_rgb[2] > max) max = _rgb[2];

  _rgb[0] = _rgb[0] / max;
  _rgb[1] = _rgb[1] / max;
  _rgb[2] = _rgb[2] / max;

  // Multiply by 255 to get an R/G/B value
  _rgb[0] = _rgb[0] * 255.0f;
  _rgb[1] = _rgb[1] * 255.0f;
  _rgb[2] = _rgb[2] * 255.0f;

  rgb[0] = static_cast<unsigned char>(_rgb[0]);
  rgb[1] = static_cast<unsigned char>(_rgb[1]);
  rgb[2] = static_cast<unsigned char>(_rgb[2]);


}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIPFTriangle::generateCubicTriangle()
{
  notifyStatusMessage("Generating Cubic Based Pole Figures for <001>, <011> & <111>");
  QImage image(m_ImageSize, m_ImageSize, QImage::Format_ARGB32_Premultiplied);

  float indexConst1 = 0.414 / m_ImageSize;
  float indexConst2 = 0.207 / m_ImageSize;
  float tslConst1 = (90.0f * M_PI) / 180.0f;
  float temp = 0.0f;
  float red1 = 0.0f;
  float green1 = 0.0f;
  float blue1 = 0.0f;
  float red2 = 0.0f;
  float green2 = 0.0f;
  float blue2 = 0.0f;
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  float a = 0.0f;
  float b = 0.0f;
  float c = 0.0f;
  float check1 = 0.0f;
  float check2 = 0.0f;
  float val = 0.0f;
  float x1 = 0.0f;
  float y1 = 0.0f;
  float z1 = 0.0f;
  float denom = 0.0f;
  float phi = 0.0f;
  float x1alt = 0.0f;
  float theta = 0.0f;

  uint8_t rgb[3];
 // CubicOps ops;
  QRgb color;
 // float dir0 = 0.0f, dir1=0.0f, dir2=1.0f;
  for (size_t yIndex = 0; yIndex < m_ImageSize; ++yIndex)
  {
    if (yIndex == m_ImageSize)
    {
      std::cout << "here" << std::endl;
    }
    for (size_t xIndex = 0; xIndex < m_ImageSize; ++xIndex)
    {

      temp = 0;
      red1 = 0;
      green1 = 0;
      blue1 = 0;
      red2 = 0;
      green2 = 0;
      blue2 = 0;
      x = xIndex * indexConst1 + indexConst2;
      y = yIndex * indexConst1 + indexConst2;
      z = -1.0;
      a = (x * x + y * y + 1);
      b = (2 * x * x + 2 * y * y);
      c = (x * x + y * y - 1);
      check1 = b * b;
      check2 = 4 * a * c;
      val = (-b + sqrtf(b * b - 4 * a * c)) / (2 * a);
      x1 = (1 + val) * x;
      y1 = (1 + val) * y;
      z1 = val;
      denom = (x1 * x1) + (y1 * y1) + (z1 * z1);
      denom = sqrtf(denom);
      x1 = x1 / denom;
      y1 = y1 / denom;
      z1 = z1 / denom;

      red1 = x1 * (-0.707f) + z1 * 0.707f;
      phi = acos(red1);
      x1alt = x1 / 0.707;
      x1alt = x1alt / sqrt((x1alt * x1alt) + (y1 * y1));
      theta = acos(x1alt / cos((tslConst1) - phi));
      theta = acos(x1alt);

      if (phi < (45 * DREAM3D::Constants::k_PiOver180) ||
          phi > (90 * DREAM3D::Constants::k_PiOver180) ||
          theta > (35.26 * DREAM3D::Constants::k_PiOver180))
      {
        color = qRgba(255, 255, 255, 255);
      }
      else
      {
       // ops.generateIPFColor(x1, y1, z1, dir0, dir1, dir2, rgb, false);
        findColor(x1, y1, z1, rgb);
        color = qRgba(rgb[0], rgb[1], rgb[2], 255);
      }
      image.setPixel(xIndex, yIndex, color);
    }
  }

  image = image.mirrored(false, true);
  writeImage(image);
}
