/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *                   FA8650-07-D-5800 and FA8650-13-M-5048
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "SaveImages.h"


#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtGui/QImage>


#include "MXA/MXA.h"
#include "MXA/Utilities/MXADir.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SaveImages::SaveImages() :
  AbstractFilter(),
  m_ImagePrefix(""),
  m_OutputPath(""),
  m_ImageFormat(0),
  m_ColorsArrayName(""),
  m_Colors(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SaveImages::~SaveImages()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImages::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  /* Place all your option initialization code here */
  /* To Display a Combobox with a list of current Voxel Cell Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel(" Colors Array");
    option->setPropertyName("ColorsArrayName");
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


  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImages::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImages::writeFilterParameters(AbstractFilterParametersWriter* writer)

{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
   writer->writeValue("ImagePrefix", getImagePrefix() );
   writer->writeValue("OutputPath", getOutputPath() );
   writer->writeValue("ColorsArrayName", getColorsArrayName() );
   writer->writeValue("ImageFormat", getImageFormat() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImages::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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

  if(m_ColorsArrayName.empty() == true)
  {
    setErrorCondition(-1004);
    addErrorMessage(getHumanLabel(), "Input  Color Array name is empty", getErrorCondition());
  }
  else
  {
      GET_PREREQ_DATA_2(m, CellData, Colors, ss, -300, uint8_t, UInt8ArrayType, voxels, 3)
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImages::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImages::execute()
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

 dataCheck(false, dims[0] * dims[1] * dims[2], 1, 1);



  size_t index = 0;
  uint8_t* slice = NULL;
  for(size_t z = 0; z < dims[2]; ++z)
  {
    index = z * (dims[0] * dims[1]) * 3;
    // Get the pointer to the data.
    slice = m_Colors + index;

    err = saveImage(slice, z, dims);

  }




  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SaveImages::saveImage(uint8_t* ipfColors, size_t slice, size_t* dims)
{
  int err = 0;

  QString path = QString::fromStdString(m_OutputPath) + QDir::separator() + QString::fromStdString(m_ImagePrefix) + QString::number(slice);
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

  int index = 0;
  QImage image(dims[0], dims[1], QImage::Format_ARGB32);
  for(size_t y = 0; y < dims[1]; ++y)
  {
     uint8_t* scanLine = image.scanLine(y);
     for(size_t x = 0; x < dims[0]; ++x)
    {
      #if defined (CMP_WORDS_BIGENDIAN)
        #error
      #else
        scanLine[x*4 + 3] = 0xFF;
        index = y * dims[0] * 3 + x * 3;
        scanLine[x*4 + 2] = ipfColors[index + 0];
        scanLine[x*4 + 1] = ipfColors[index + 1];
        scanLine[x*4 + 0] = ipfColors[index + 2];
      #endif

//      index = y * dims[0] + x;
//      QRgb rgb(ipfColors[y*x*3 + 0], ipfColors[x*3 + 1], ipfColors[x*3 + 2]);
//      image.setColor(index, rgb);
    }

  }
  image.setText("", "DREAM3D generated Image");
  bool success = image.save(path);
  if (success) { err = 0;}
  else { err = -1;}
  return err;
}
