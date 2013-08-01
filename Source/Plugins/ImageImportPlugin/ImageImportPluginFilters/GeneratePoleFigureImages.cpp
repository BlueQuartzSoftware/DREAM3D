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
#include "GeneratePoleFigureImages.h"

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

#include "QtSupport/PoleFigureGeneration.h"

#include "MXA/MXA.h"
#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXADir.h"

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/HKL/CtfReader.h"

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/StatsGen.hpp"
#include "DREAM3DLib/Common/Texture.hpp"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/Common/Texture.hpp"
#include "DREAM3DLib/Common/StatsGen.hpp"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/IOFilters/VtkRectilinearGridWriter.h"


#define SET_DIRECTION(i, j, k)\
  direction[0] = i; direction[1] = j; direction[2] = k;





// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GeneratePoleFigureImages::GeneratePoleFigureImages() :
  AbstractFilter(),
  m_ImagePrefix(""),
  m_OutputPath(""),
  m_ImageFormat(0),
  m_ImageSize(512),
  m_LambertSize(75),
  m_NumColors(32),
  m_EulersArrayName(DREAM3D::CellData::EulerAngles),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CellEulerAngles(NULL),
  m_CrystalStructures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GeneratePoleFigureImages::~GeneratePoleFigureImages()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  /* Place all your option initialization code here */
  /* To Display a Combobox with a list of current Voxel Cell Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Eulers Array");
    option->setPropertyName("EulersArrayName");
    option->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
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
  /* For String input use this code */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel(" Image Prefix");
    parameter->setPropertyName("ImagePrefix");
    parameter->setWidgetType(FilterParameter::StringWidget);
    parameter->setValueType("string");
    parameters.push_back(parameter);
  }
  /*   For an output path use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output Path");
    option->setPropertyName("OutputPath");
    option->setWidgetType(FilterParameter::OutputPathWidget);
    option->setValueType("string");
    parameters.push_back(option);
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
    option->setPropertyName("LambertSize");
    option->setHumanLabel("Lambert Image Size");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setPropertyName("NumColors");
    option->setHumanLabel("Number of Colors");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    parameters.push_back(option);
  }

    setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::writeFilterParameters(AbstractFilterParametersWriter* writer)

{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->writeValue("ImagePrefix", getImagePrefix() );
  writer->writeValue("OutputPath", getOutputPath() );
  writer->writeValue("EulersArrayName", getEulersArrayName() );
  writer->writeValue("ImageFormat", getImageFormat() );
  writer->writeValue("ImageSize", getImageSize() );
  writer->writeValue("LabertSize", getLambertSize() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  /* Example code for preflighting looking for a valid string for the output file
   * but not necessarily the fact that the file exists: Example code to make sure
   * we have something in a string before proceeding.*/

  if (m_OutputPath.empty() == true)
  {
    setErrorCondition(-1003);
    addErrorMessage(getHumanLabel(), "Output Directory is Not set correctly", getErrorCondition());
  }

  else if (MXADir::exists(m_OutputPath) == false)
  {
    ss.str("");
    ss <<  "The directory path for the output file does not exist. DREAM3D will attempt to create this path during execution of the filter.";
    addWarningMessage(getHumanLabel(), ss.str(), -1);
  }

  if(m_EulersArrayName.empty() == true)
  {
    setErrorCondition(-1004);
    addErrorMessage(getHumanLabel(), "Input Euler Array name is empty", getErrorCondition());
  }
  else
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, -300, float, FloatArrayType, voxels, 3)
        typedef DataArray<unsigned int> XTalStructArrayType;
    GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1)
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::execute()
{
  int err = 0;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Voxel DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  /* Place all your code to execute your filter here. */
  size_t dims[3];
  m->getDimensions(dims);

  dataCheck(false, m->getTotalPoints(), m->getNumFieldTuples(), m->getNumEnsembleTuples());

  FloatArrayType* eulers = FloatArrayType::SafePointerDownCast(m->getCellData(getCellEulerAnglesArrayName()).get());


  if ( Ebsd::CrystalStructure::Check::IsCubic(m_CrystalStructures[1]))
  {
    generateCubicPoleFigures(eulers);
  }
  else if (Ebsd::CrystalStructure::Check::IsHexagonal(m_CrystalStructures[1]))
  {

  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::generateCubicPoleFigures(FloatArrayType* eulers)
{
  notifyStatusMessage("Generating Cubic Based Pole Figures for <001>, <011> & <111>");
  int numOrientations = eulers->GetNumberOfTuples();


  // Create an Array to hold the XYZ Coordinates which are the coords on the sphere.
  // this is size for CUBIC ONLY, <001> Family
  FloatArrayType::Pointer xyz001 = FloatArrayType::CreateArray(numOrientations * 3, 3, "TEMP_<001>_xyzCoords");
  // this is size for CUBIC ONLY, <011> Family
  FloatArrayType::Pointer xyz011 = FloatArrayType::CreateArray(numOrientations * 6, 3, "TEMP_<011>_xyzCoords");
  // this is size for CUBIC ONLY, <111> Family
  FloatArrayType::Pointer xyz111 = FloatArrayType::CreateArray(numOrientations * 4, 3, "TEMP_<111>_xyzCoords");

  int dimension = getLambertSize();
  float resolution = sqrt(M_PI*0.5) * 2.0 / (float)(dimension);
  int poleFigureDim = getImageSize();
  float sphereRadius = 1.0f;

  // Generate the coords on the sphere
  CubicOps ops;
  ops.generateSphereCoordsFromEulers(eulers, xyz001.get(), xyz011.get(), xyz111.get());



  // Generate the modified Lambert projection images (Squares, 2 of them, 1 for northern hemisphere, 1 for southern hemisphere
  ModifiedLambertProjection::Pointer lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz001.get(), dimension, resolution, sphereRadius);
  // Now create the intensity image that will become the actual Pole figure image
  DoubleArrayType::Pointer poleFigurePtr = lambert->createStereographicProjection(poleFigureDim);
  poleFigurePtr->SetName("PoleFigure_<001>");
  QString path = generateVtkPath("001");
  writeVtkFile(path.toStdString(), poleFigurePtr.get(), poleFigureDim);
  writeImage(m_OutputPath, poleFigurePtr.get(), poleFigureDim, "001");

  // Generate the <011> pole figure
  lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz011.get(), dimension, resolution, sphereRadius);
  poleFigurePtr = lambert->createStereographicProjection(poleFigureDim);
  poleFigurePtr->SetName("PoleFigure_<011>");
  path = generateVtkPath("011");
  writeVtkFile(path.toStdString(), poleFigurePtr.get(), poleFigureDim);
  writeImage(m_OutputPath, poleFigurePtr.get(), poleFigureDim, "011");

  // Generate the <111> pole figure
  lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz111.get(), dimension, resolution, sphereRadius);
  poleFigurePtr = lambert->createStereographicProjection(poleFigureDim);
  poleFigurePtr->SetName("PoleFigure_<111>");
  path = generateVtkPath("111");
  writeVtkFile(path.toStdString(), poleFigurePtr.get(), poleFigureDim);
  writeImage(m_OutputPath, poleFigurePtr.get(), poleFigureDim, "111");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GeneratePoleFigureImages::generateVtkPath( QString label)
{
  QString path = QString::fromStdString(m_OutputPath) + QDir::separator() + QString::fromStdString(m_ImagePrefix) + label;

  path.append(".vtk");

  path = QDir::toNativeSeparators(path);
  QFileInfo fi(path);
  QDir parent(fi.absolutePath());
  if (parent.exists() == false)
  {
    parent.mkpath(fi.absolutePath());
  }

  return path;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::writeVtkFile(const std::string filename, DoubleArrayType *poleFigurePtr, int dimension)
{

  notifyStatusMessage("Writing VTK File");

  size_t dims[3] = {dimension, dimension, 1};
  float res[3] = {  2.0/(float)(dimension),
                    2.0/(float)(dimension),
                    ( 2.0/(float)(dimension) + 2.0/(float)(dimension) )/2.0 };

  int err = VtkRectilinearGridWriter::WriteDataArrayToFile(filename, poleFigurePtr, dims, res, "double", true);
  if (err < 0)
  {
    setErrorCondition(-99003);
    notifyErrorMessage("Error writing the VTK file for the Pole Figure", getErrorCondition());
  }
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
void GeneratePoleFigureImages::getColorCorrespondingTovalue(float val,
                                                            float &r, float &g, float &b,
                                                            float max, float min)
{
  static const int numColorNodes = 8;
  float color[numColorNodes][3] =
  {
    {0.0f, 1.0f/255.0f, 253.0f/255.0f},    // blue
    {105.0f/255.0f, 145.0f/255.0f, 2.0f/255.0f},    // yellow
    {1.0f/255.0f, 255.0f/255.0f, 29.0f/255.0f},    // Green
    {180.0f/255.0f, 255.0f/255.0f, 0.0f/255.0f},
    {255.0f/255.0f, 215.0f/255.0f, 6.0f/255.0f},
    {255.0f/255.0f, 143.0f/255.0f, 1.0f/255.0f},
    {255.0f/255.0f, 69.0f/255.0f, 0.0f/255.0f},
    {253.0f/255.0f, 1.0f/255.0f, 0.0f/255.0f}     // red
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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeneratePoleFigureImages::writeImage(const std::string outputPath, DoubleArrayType *poleFigurePtr, int dimension, QString label)
{
  std::stringstream ss;
  ss << "Writing Image " << poleFigurePtr->GetName();
  notifyStatusMessage(ss.str());

  QString filename = generateImagePath(label);


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

  int xpoints = dimension;
  int ypoints = dimension;

  int xpointshalf = xpoints / 2;
  int ypointshalf = ypoints / 2;

  float xres = 2.0 / (float)(xpoints);
  float yres = 2.0 / (float)(ypoints);
  float xtmp, ytmp;

  //if (max < 14) { max = 14; }
  QImage image (xpoints, ypoints, QImage::Format_ARGB32_Premultiplied);
  image.fill(0);
 // qint32 numColors = max + 1;
  qint32 numColors = getNumColors() + 1;
 // qint32 colorRange = getNumColors();

  QVector<QColor> colorTable(numColors);
  //qint32 range = max - min;

  float r, g, b;
  for (int i = 0; i < getNumColors(); i++)
  {
    //int val = min + ((float)i / numColors) * range;
    //int val = ((float)i / (float)numColors) * colorRange;
    getColorCorrespondingTovalue(i, r, g, b, getNumColors(), 0);
    colorTable[i] = QColor(r*255, g*255, b*255, 255);
  }
  // Index 0 is all white which is every pixel outside of the Pole Figure circle
  colorTable[getNumColors()] = QColor(255, 255, 255, 255);

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
        qint32 colorIndex = (cindex-min) / (max - min) * getNumColors();
        image.setPixel(x, y, colorTable[colorIndex].rgba());
      }
      else // Outside the Circle - Set pixel to White
      {
        image.setPixel(x, y, colorTable[getNumColors()].rgba());
      }
    }
  }

  // Flip the image so the (-1, -1) is in the lower left
  image = image.mirrored(false, true);

  QString imageLabel = "<" + label + ">";
  image = paintImage(xpoints, ypoints, imageLabel, image);

  bool saved = image.save(filename);
  if(!saved)
  {
    setErrorCondition(-90011);
    ss.str("");
    ss << "The Pole Figure image file '" << filename.toStdString() << "' was not saved.";
    notifyErrorMessage(ss.str(), getErrorCondition());
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GeneratePoleFigureImages::generateImagePath( QString label)
{
  QString path = QString::fromStdString(m_OutputPath) + QDir::separator() + QString::fromStdString(m_ImagePrefix) + label;
  if(m_ImageFormat == TifImageType)
  {
    path.append(".tif");
  }
  else if (m_ImageFormat == BmpImageType)
  {
    path.append(".bmp");
  }
  else if (m_ImageFormat == PngImageType)
  {
    path.append(".png");
  }
  else if (m_ImageFormat == JpgImageType)
  {
    path.append(".jpg");
  }
  path = QDir::toNativeSeparators(path);
  QFileInfo fi(path);
  QDir parent(fi.absolutePath());
  if (parent.exists() == false)
  {
    parent.mkpath(fi.absolutePath());
  }

  return path;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage GeneratePoleFigureImages::paintImage(int imageWidth, int imageHeight, QString label, QImage image)
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
  return pImage;//.scaled(225, 225, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

