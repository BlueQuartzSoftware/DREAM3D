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
#include "WritePoleFigure.h"

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
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/CubicLowOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalLowOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/OrientationOps/TrigonalOps.h"
#include "DREAM3DLib/OrientationOps/TetragonalOps.h"
#include "DREAM3DLib/OrientationOps/TrigonalLowOps.h"
#include "DREAM3DLib/OrientationOps/TetragonalLowOps.h"
#include "DREAM3DLib/OrientationOps/TriclinicOps.h"
#include "DREAM3DLib/OrientationOps/MonoclinicOps.h"

#include "DREAM3DLib/IOFilters/VtkRectilinearGridWriter.h"

#include "DREAM3DLib/Utilities/ColorTable.h"
#include "DREAM3DLib/Utilities/PoleFigureUtilities.h"

#define SET_DIRECTION(i, j, k)\
  direction[0] = i; direction[1] = j; direction[2] = k;



#define WRITE_EULERS_TEXT_FILE 1

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WritePoleFigure::WritePoleFigure() :
  AbstractFilter(),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_ImagePrefix(""),
  m_OutputPath(""),
  m_ImageFormat(0),
  m_ImageSize(512),
  m_LambertSize(32),
  m_NumColors(32),
  m_CellEulerAngles(NULL),
  m_CellPhases(NULL),
  m_CrystalStructures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WritePoleFigure::~WritePoleFigure()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  /* Place all your option initialization code here */
  /* To Display a Combobox with a list of current Voxel Cell Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Eulers Array");
    option->setPropertyName("CellEulerAnglesArrayName");
    option->setWidgetType(FilterParameter::VolumeCellArrayNameSelectionWidget);
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
    QVector<QString> choices;
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
void WritePoleFigure::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setImagePrefix( reader->readValue("ImagePrefix", getImagePrefix()));
  setOutputPath( reader->readValue("OutputPath", getOutputPath()));
  setCellEulerAnglesArrayName( reader->readValue("CellEulerAnglesArrayName", getCellEulerAnglesArrayName()));
  setImageFormat( reader->readValue("ImageFormat", getImageFormat()));
  setImageSize( reader->readValue("ImageSize", getImageSize()));
  setLambertSize( reader->readValue("LambertSize", getLambertSize()));
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WritePoleFigure::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("ImagePrefix", getImagePrefix() );
  writer->writeValue("OutputPath", getOutputPath() );
  writer->writeValue("CellEulerAnglesArrayName", getCellEulerAnglesArrayName() );
  writer->writeValue("ImageFormat", getImageFormat() );
  writer->writeValue("ImageSize", getImageSize() );
  writer->writeValue("LambertSize", getLambertSize() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  
  VolumeDataContainer* m = getVolumeDataContainer();
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

  if(m_CellEulerAnglesArrayName.empty() == true)
  {
    setErrorCondition(-1004);
    addErrorMessage(getHumanLabel(), "Input Euler Array name is empty", getErrorCondition());
  }
  else
  {
    GET_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, -300, float, FloatArrayType, voxels, 3)
        GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, -301, int32_t, Int32ArrayType, voxels, 1)

        typedef DataArray<unsigned int> XTalStructArrayType;
    GET_PREREQ_DATA(m, DREAM3D, CellEnsembleData, CrystalStructures, -304, unsigned int, XTalStructArrayType, ensembles, 1)
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename Ops>
QVector<UInt8ArrayType::Pointer> makePoleFigures(PoleFigureConfiguration_t &config)
{
  Ops ops;
  return ops.generatePoleFigure(config);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::execute()
{
  int err = 0;
  setErrorCondition(err);
  VolumeDataContainer* m = getVolumeDataContainer();
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

  dataCheck(false, m->getTotalPoints(), m->getNumCellFieldTuples(), m->getNumCellEnsembleTuples());


  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QString parentPath = MXAFileInfo::parentPath(getOutputPath());
  if(!MXADir::mkdir(parentPath, true))
  {
    
    ss << "Error creating parent path '" << parentPath << "'";
    notifyErrorMessage(ss.str(), -1);
    setErrorCondition(-1);
    return;
  }


  bool* m_GoodVoxels;
  BoolArrayType* goodVoxels = NULL;
  bool missingGoodVoxels = false;
  IDataArray::Pointer gvPtr = m->getCellData(m_GoodVoxelsArrayName);

  if (m->getCellData(m_GoodVoxelsArrayName).get() == NULL)
  {
    missingGoodVoxels = true;
  }
  else
  {
    goodVoxels = BoolArrayType::SafePointerDownCast(gvPtr.get());
    m_GoodVoxels = goodVoxels->GetPointer(0);
  }

  // Find how many phases we have by getting the number of Crystal Structures
  IDataArray::Pointer crystalStructures = m->getCellEnsembleData(m_CrystalStructuresArrayName);
  int64_t numPoints = m->getTotalPoints();
  int numPhases = crystalStructures->GetNumberOfTuples();
  size_t count = 0;
  // Loop over all the voxels gathering the Eulers for a specific phase into an array
  for(int phase = 1; phase < numPhases; ++phase)
  {
    // First find out how many voxels we are going to have. This is probably faster to loop twice than to
    // keep allocating memory everytime we find one.
    for(int64_t i = 0; i < numPoints; ++i)
    {
      if (m_CellPhases[i] == phase) {
        if(missingGoodVoxels == true || m_GoodVoxels[i] == true)
        {
          count++;
        }
      }
    }

    FloatArrayType::Pointer subEulers = FloatArrayType::CreateArray(count, 3, "Eulers_Per_Phase");
    subEulers->initializeWithValues(-1);
    float* eu = subEulers->GetPointer(0);
    //  qDebug() << count << "\n";

    // Now loop through the eulers again and this time add them to the subEulers Array
    count = 0;
    for(int64_t i = 0; i < numPoints; ++i)
    {
      if (m_CellPhases[i] == phase)
      {
        if(missingGoodVoxels == true || m_GoodVoxels[i] == true)
        {
          eu[count*3] = m_CellEulerAngles[i*3];
          eu[count*3+1] = m_CellEulerAngles[i*3+1];
          eu[count*3+2] = m_CellEulerAngles[i*3+2];

          //    qDebug() << eu[count*3] << " " << eu[count*3+1] << " " << eu[count*3+2] << "\n";

          count++;
        }
      }
    }
    if (subEulers->GetNumberOfTuples() == 0) { continue; } // Skip because we have no Pole Figure data

    QVector<UInt8ArrayType::Pointer> figures;

    PoleFigureConfiguration_t config;
    config.eulers = subEulers.get();
    config.imageDim = getImageSize();
    config.lambertDim = getLambertSize();
    config.numColors = getNumColors();

    QString label("Phase_");
    label.append(QString::number(phase));

    switch(m_CrystalStructures[phase])
    {
      case Ebsd::CrystalStructure::Cubic_High:
        figures = makePoleFigures<CubicOps>(config);
        break;
      case Ebsd::CrystalStructure::Cubic_Low:
        figures = makePoleFigures<CubicLowOps>(config);
        break;
      case Ebsd::CrystalStructure::Hexagonal_High:
        figures = makePoleFigures<HexagonalOps>(config);
        break;
      case Ebsd::CrystalStructure::Hexagonal_Low:
        figures = makePoleFigures<HexagonalLowOps>(config);
        break;
      case Ebsd::CrystalStructure::Trigonal_High:
     //   figures = makePoleFigures<TrigonalOps>(config);
        addWarningMessage(getHumanLabel(), "Trigonal High Symmetry is not supported for Pole figures. This phase will be omitted from results", -1010);
        break;
      case Ebsd::CrystalStructure::Trigonal_Low:
      //  figures = makePoleFigures<TrigonalLowOps>(config);
        addWarningMessage(getHumanLabel(), "Trigonal Low Symmetry is not supported for Pole figures. This phase will be omitted from results", -1010);
        break;
      case Ebsd::CrystalStructure::Tetragonal_High:
      //  figures = makePoleFigures<TetragonalOps>(config);
        addWarningMessage(getHumanLabel(), "Tetragonal High Symmetry is not supported for Pole figures. This phase will be omitted from results", -1010);
        break;
      case Ebsd::CrystalStructure::Tetragonal_Low:
      //  figures = makePoleFigures<TetragonalLowOps>(config);
        addWarningMessage(getHumanLabel(), "Trigonal Low Symmetry is not supported for Pole figures. This phase will be omitted from results", -1010);
        break;
      case Ebsd::CrystalStructure::OrthoRhombic:
        figures = makePoleFigures<OrthoRhombicOps>(config);
        break;
      case Ebsd::CrystalStructure::Monoclinic:
        figures = makePoleFigures<MonoclinicOps>(config);
        break;
      case Ebsd::CrystalStructure::Triclinic:
        figures = makePoleFigures<TriclinicOps>(config);
        break;
      default:
        break;

    }

    if (figures.size() == 3)
    {
      QImage combinedImage = PoleFigureImageUtilities::Create3ImagePoleFigure(figures[0].get(), figures[1].get(), figures[2].get(), config);
      writeImage(m_OutputPath, combinedImage, combinedImage.width(), label);
    }
  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WritePoleFigure::generateVtkPath( QString label)
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
void WritePoleFigure::writeVtkFile(const QString filename, DoubleArrayType *poleFigurePtr, int dimension)
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




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::writeImage(const QString outputPath, QImage image, int dimension, QString label)
{
  
  ss << "Writing Image " << outputPath;
  notifyStatusMessage(ss.str());

  QString filename = generateImagePath(label);
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
QString WritePoleFigure::generateImagePath( QString label)
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

