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


#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtGui/QImage>


#include "MXA/MXA.h"
#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/Common/Texture.hpp"
#include "DREAM3DLib/Common/StatsGen.hpp"
#include "DREAM3DLib/OrientationOps/CubicOps.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GeneratePoleFigureImages::GeneratePoleFigureImages() :
  AbstractFilter(),
  m_ImagePrefix(""),
  m_OutputPath(""),
  m_ImageFormat(0),
  m_ImageSize(256),
  m_KernelSize(4),
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
    option->setPropertyName("KernelSize");
    option->setHumanLabel("Kernel Size");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    option->setUnits("Pixels");
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
  writer->writeValue("KernelSize", getKernelSize() );
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
  std::stringstream ss;
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
  size_t numEntries = m->getTotalPoints();
  FloatArrayType::Pointer e0 = FloatArrayType::CreateArray(numEntries, 1, "TEMP_Phi1");
  FloatArrayType::Pointer e1 = FloatArrayType::CreateArray(numEntries, 1, "TEMP_Phi");
  FloatArrayType::Pointer e2 = FloatArrayType::CreateArray(numEntries, 1, "TEMP_Phi2");
  FloatArrayType::Pointer weights = FloatArrayType::CreateArray(numEntries, 1, "TEMP_Weights");
  weights->initializeWithValues(1.0);
  FloatArrayType::Pointer sigmas = FloatArrayType::CreateArray(numEntries, 1, "TEMP_Sigmas");
  sigmas->initializeWithValues(0.0);

  notifyStatusMessage("Copying Euler Data.....");
  // Copy the Euler Angles from their usual place into the individual arrays needed by the StatsGen and Texture
  for(size_t i = 0; i < numEntries; ++i)
  {
    e0->SetValue(i, m_CellEulerAngles[i*3]);
    e1->SetValue(i, m_CellEulerAngles[i*3+1]);
    e2->SetValue(i, m_CellEulerAngles[i*3+2]);
  }



  if ( Ebsd::CrystalStructure::Check::IsCubic(m_CrystalStructures[1]))
  {
    generateCubicPoleFigures(e0, e1, e2, weights, sigmas);
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
void GeneratePoleFigureImages::generateCubicPoleFigures(FloatArrayType::Pointer e0, FloatArrayType::Pointer e1, FloatArrayType::Pointer e2,
                                                        FloatArrayType::Pointer weights, FloatArrayType::Pointer sigmas)
{
  QVector<float> odf(CubicOps::k_OdfSize);
  size_t numEntries = e0->GetNumberOfTuples();

  notifyStatusMessage("Sampling Euler Data.....");

  Texture::CalculateCubicODFData(e0->GetPointer(0), e1->GetPointer(0), e2->GetPointer(0),
                                 weights->GetPointer(0), sigmas->GetPointer(0), true,
                                 odf.data(), numEntries);
  size_t npoints = 5000;
  QVector<float > x001(npoints * 3);
  QVector<float > y001(npoints * 3);
  QVector<float > x011(npoints * 6);
  QVector<float > y011(npoints * 6);
  QVector<float > x111(npoints * 4);
  QVector<float > y111(npoints * 4);

  qint32 kRad[2] = {m_KernelSize, m_KernelSize};
  qint32 pfSize[2] = {m_ImageSize, m_ImageSize};

  notifyStatusMessage("Generating raw XY plot data .....");

  int err = StatsGen::GenCubicODFPlotData(odf.data(), x001.data(), y001.data(),
                                          x011.data(), y011.data(), x111.data(), y111.data(), npoints);
  if(err < 0)
  {
    setErrorCondition(-99001);
    notifyErrorMessage("Cubic ODF Generation caused errors.", getErrorCondition());
    return;
  }


  PoleFigureData pf001(x001, y001, QString("<001>"), kRad, pfSize);
  PoleFigureData pf011(x011, y011, QString("<011>"), kRad, pfSize);
  PoleFigureData pf111(x111, y111, QString("<111>"), kRad, pfSize);


  notifyStatusMessage("Generating 001 Pole Figure ....");
  generateImage(pf001, QString("001"));
  notifyStatusMessage("Generating 011 Pole Figure ....");
  generateImage(pf011, QString("011"));
  notifyStatusMessage("Generating 111 Pole Figure ....");
  generateImage(pf111, QString("111"));
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GeneratePoleFigureImages::generateImage(PoleFigureData data, QString label)
{
  std::stringstream ss;
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

  PoleFigureGeneration colorPoleFigure;
  QImage image = colorPoleFigure.generateColorPoleFigureImage(data);
  bool saved = image.save(path);
  if (!saved)
  {
    ss.str("");
    setErrorCondition(-99000);
    ss << "Pole Figure image '" << path.toStdString() << "' not saved.";
    notifyErrorMessage(ss.str(), getErrorCondition());
  }
  return getErrorCondition();
}
