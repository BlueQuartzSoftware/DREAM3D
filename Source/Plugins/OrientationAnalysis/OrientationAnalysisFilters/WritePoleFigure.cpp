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

#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include <QtGui/QPainter>
#include <QtGui/QFont>
#include <QtGui/QImage>
#include <QtGui/QColor>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/HKL/CtfReader.h"


#include "DREAM3DLib/Common/ModifiedLambertProjection.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Utilities/DREAM3DEndian.h"
#include "OrientationLib/Math/OrientationMath.h"
#include "OrientationLib/OrientationOps/CubicOps.h"
#include "OrientationLib/OrientationOps/CubicLowOps.h"
#include "OrientationLib/OrientationOps/HexagonalOps.h"
#include "OrientationLib/OrientationOps/HexagonalLowOps.h"
#include "OrientationLib/OrientationOps/OrthoRhombicOps.h"
#include "OrientationLib/OrientationOps/TrigonalOps.h"
#include "OrientationLib/OrientationOps/TetragonalOps.h"
#include "OrientationLib/OrientationOps/TrigonalLowOps.h"
#include "OrientationLib/OrientationOps/TetragonalLowOps.h"
#include "OrientationLib/OrientationOps/TriclinicOps.h"
#include "OrientationLib/OrientationOps/MonoclinicOps.h"
#include "DREAM3DLib/Utilities/ColorTable.h"
#include "OrientationLib/Utilities/PoleFigureUtilities.h"

#include "QtSupport/PoleFigureImageUtilities.h"

#define SET_DIRECTION(i, j, k)\
  direction[0] = i; direction[1] = j; direction[2] = k;



#define WRITE_EULERS_TEXT_FILE 1

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WritePoleFigure::WritePoleFigure() :
  AbstractFilter(),
  m_ImagePrefix(""),
  m_OutputPath(""),
  m_ImageFormat(0),
  m_ImageSize(512),
  m_LambertSize(32),
  m_NumColors(32),
  m_ImageLayout(DREAM3D::Layout::Square),
  m_CellEulerAnglesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::EulerAngles),
  m_CellPhasesArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_GoodVoxelsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::GoodVoxels),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellEulerAngles(NULL),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellPhases(NULL),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures(NULL),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_GoodVoxels(NULL)
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
  FilterParameterVector parameters;
  /* Place all your option initialization code here */
  /* To Display a Combobox with a list of current Voxel Cell Arrays in it */
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Image Format");
    parameter->setPropertyName("ImageFormat");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    QVector<QString> choices;
    choices.push_back("tif");
    choices.push_back("bmp");
    choices.push_back("png");
    //   choices.push_back("jpg");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }


  /* For String input use this code */
  parameters.push_back(FilterParameter::New("Image Prefix", "ImagePrefix", FilterParameterWidgetType::StringWidget, getImagePrefix(), false));
  /*   For an output path use this code*/
  parameters.push_back(FileSystemFilterParameter::New("Output Path", "OutputPath", FilterParameterWidgetType::OutputPathWidget, getOutputPath(), false));
  parameters.push_back(FilterParameter::New("Image Size (Square)", "ImageSize", FilterParameterWidgetType::IntWidget, getImageSize(), false, "Pixels"));

  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Cell Euler Angles", "CellEulerAnglesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellEulerAnglesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Phases", "CellPhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCellPhasesArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("GoodVoxels", "GoodVoxelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getGoodVoxelsArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("Lambert Image Size", "LambertSize", FilterParameterWidgetType::IntWidget, getLambertSize(), true, "Pixels"));
  parameters.push_back(FilterParameter::New("Number of Colors", "NumColors", FilterParameterWidgetType::IntWidget, getNumColors(), true));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Image Layout");
    parameter->setPropertyName("ImageLayout");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    QVector<QString> choices;
    choices.push_back("Horizontal");
    choices.push_back("Vertical");
    choices.push_back("Square");
    //   choices.push_back("jpg");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setCellEulerAnglesArrayPath(reader->readDataArrayPath("CellEulerAnglesArrayPath", getCellEulerAnglesArrayPath() ) );
  setImagePrefix( reader->readString("ImagePrefix", getImagePrefix()));
  setOutputPath( reader->readString("OutputPath", getOutputPath()));
  setImageFormat( reader->readValue("ImageFormat", getImageFormat()));
  setImageLayout( reader->readValue("ImageLayout", getImageLayout()));
  setImageSize( reader->readValue("ImageSize", getImageSize()));
  setLambertSize( reader->readValue("LambertSize", getLambertSize()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WritePoleFigure::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(GoodVoxelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellEulerAnglesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ImagePrefix)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ImageFormat)
  DREAM3D_FILTER_WRITE_PARAMETER(ImageSize)
  DREAM3D_FILTER_WRITE_PARAMETER(LambertSize)
  DREAM3D_FILTER_WRITE_PARAMETER(ImageLayout)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::dataCheck()
{
  setErrorCondition(0);

  /* Example code for preflighting looking for a valid string for the output file
   * but not necessarily the fact that the file exists: Example code to make sure
   * we have something in a string before proceeding.*/
  QDir path(getOutputPath());

  if (m_OutputPath.isEmpty() == true)
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "Output Directory is Not set correctly", getErrorCondition());
  }
  else if (path.exists() == false)
  {
    QString ss = QObject::tr("The directory path for the output file does not exist. DREAM3D will attempt to create this path during execution of the filter.");
    notifyWarningMessage(getHumanLabel(), ss, -1);
  }

  if(m_CellEulerAnglesArrayName.isEmpty() == true)
  {
    setErrorCondition(-1004);
    notifyErrorMessage(getHumanLabel(), "Input Euler Array name is empty", getErrorCondition());
  }
  else
  {
    QVector<size_t> dims(1, 3);
    m_CellEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellEulerAnglesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    dims[0] = 1;
    m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

    //typedef DataArray<unsigned int> XTalStructArrayType;
    m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
  QVector<size_t> dims(1, 1);
  m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(NULL != m_GoodVoxelsPtr.lock().get())
  {
    if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  else
  {
    m_GoodVoxels = NULL;
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::preflight()
{
  setInPreflight(true);
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);

  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
  setErrorCondition(0xABABABAB);
  QString ss = QObject::tr("Filter is NOT updated for IGeometry Redesign. A Programmer needs to check this filter. Please report this to the DREAM3D developers.");
  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename Ops>
QVector<UInt8ArrayType::Pointer> makePoleFigures(PoleFigureConfiguration_t& config)
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
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_CellPhasesArrayPath.getDataContainerName());

  /* Place all your code to execute your filter here. */
  size_t dims[3];
  /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getDimensions(dims);

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QDir path(getOutputPath());

  if(!path.mkpath(".") )
  {
    QString ss = QObject::tr("Error creating parent path '%1'").arg(path.absolutePath());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }

  bool missingGoodVoxels = true;

  if (NULL != m_GoodVoxels)
  {
    missingGoodVoxels = false;
  }
  // Find how many phases we have by getting the number of Crystal Structures
  int64_t numPoints = /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getNumberOfTuples();
  int numPhases = m_CrystalStructuresPtr.lock()->getNumberOfTuples();
  size_t count = 0;
  // Loop over all the voxels gathering the Eulers for a specific phase into an array
  for(int phase = 1; phase < numPhases; ++phase)
  {
    // First find out how many voxels we are going to have. This is probably faster to loop twice than to
    // keep allocating memory everytime we find one.
    for(int64_t i = 0; i < numPoints; ++i)
    {
      if (m_CellPhases[i] == phase)
      {
        if(missingGoodVoxels == true || m_GoodVoxels[i] == true)
        {
          count++;
        }
      }
    }
    size_t eulerCompDim = 3;
    FloatArrayType::Pointer subEulers = FloatArrayType::CreateArray(count, 1, &eulerCompDim, "Eulers_Per_Phase");
    subEulers->initializeWithValue(-1);
    float* eu = subEulers->getPointer(0);

    // Now loop through the eulers again and this time add them to the subEulers Array
    count = 0;
    for(int64_t i = 0; i < numPoints; ++i)
    {
      if (m_CellPhases[i] == phase)
      {
        if(missingGoodVoxels == true || m_GoodVoxels[i] == true)
        {
          eu[count * 3] = m_CellEulerAngles[i * 3];
          eu[count * 3 + 1] = m_CellEulerAngles[i * 3 + 1];
          eu[count * 3 + 2] = m_CellEulerAngles[i * 3 + 2];
          count++;
        }
      }
    }
    if (subEulers->getNumberOfTuples() == 0) { continue; } // Skip because we have no Pole Figure data



    QVector<UInt8ArrayType::Pointer> figures;

    PoleFigureConfiguration_t config;
    config.eulers = subEulers.get();
    config.imageDim = getImageSize();
    config.lambertDim = getLambertSize();
    config.numColors = getNumColors();

    QString label("Phase_");
    label.append(QString::number(phase));

    //QString filename = generateImagePath(label);
    QString ss = QObject::tr("Generating Pole Figures for Phase %1").arg(phase);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);


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
        notifyWarningMessage(getHumanLabel(), "Trigonal High Symmetry is not supported for Pole figures. This phase will be omitted from results", -1010);
        break;
      case Ebsd::CrystalStructure::Trigonal_Low:
        //  figures = makePoleFigures<TrigonalLowOps>(config);
        notifyWarningMessage(getHumanLabel(), "Trigonal Low Symmetry is not supported for Pole figures. This phase will be omitted from results", -1010);
        break;
      case Ebsd::CrystalStructure::Tetragonal_High:
        //  figures = makePoleFigures<TetragonalOps>(config);
        notifyWarningMessage(getHumanLabel(), "Tetragonal High Symmetry is not supported for Pole figures. This phase will be omitted from results", -1010);
        break;
      case Ebsd::CrystalStructure::Tetragonal_Low:
        //  figures = makePoleFigures<TetragonalLowOps>(config);
        notifyWarningMessage(getHumanLabel(), "Tetragonal Low Symmetry is not supported for Pole figures. This phase will be omitted from results", -1010);
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
      QImage combinedImage = PoleFigureImageUtilities::Create3ImagePoleFigure(figures[0].get(), figures[1].get(), figures[2].get(), config, getImageLayout());
      writeImage(m_OutputPath, combinedImage, combinedImage.width(), label);
    }


  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WritePoleFigure::writeImage(const QString outputPath, QImage image, int dimension, QString label)
{
  QString filename = generateImagePath(label);
  QString ss = QObject::tr("Writing Image %1").arg(filename);
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  bool saved = image.save(filename);
  if(!saved)
  {
    setErrorCondition(-90011);
    QString ss = QObject::tr("The Pole Figure image file '%1' was not saved.").arg(filename);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString WritePoleFigure::generateImagePath( QString label)
{
  QString path = m_OutputPath + "/" + m_ImagePrefix + label;
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
AbstractFilter::Pointer WritePoleFigure::newFilterInstance(bool copyFilterParameters)
{
  WritePoleFigure::Pointer filter = WritePoleFigure::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getCompiledLibraryName()
{ return OrientationAnalysis::OrientationAnalysisBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getSubGroupName()
{ return "Output"; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WritePoleFigure::getHumanLabel()
{ return "Write Pole Figure Image"; }

