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

#include "LosAlamosFFTWriter.h"

#include <iostream>
#include <fstream>

#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"

#include "EbsdLib/TSL/AngConstants.h"

#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LosAlamosFFTWriter::LosAlamosFFTWriter() :
  FileWriter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_GrainIds(NULL),
  m_CellPhases(NULL),
  m_CellEulerAngles(NULL)

{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LosAlamosFFTWriter::~LosAlamosFFTWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LosAlamosFFTWriter::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.txt");
    option->setFileType("FFT Format");
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LosAlamosFFTWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputFile( reader->readValue( "OutputFile", getOutputFile() ) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LosAlamosFFTWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LosAlamosFFTWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  if(getOutputFile().empty() == true)
  {
    ss.str("");
    ss << ClassName() << " needs the Output File Set and it was not.";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    setErrorCondition(-387);
  }


  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -302, int32_t, Int32ArrayType, voxels, 1)
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, -305, float, FloatArrayType, voxels, 3)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LosAlamosFFTWriter::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LosAlamosFFTWriter::writeHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LosAlamosFFTWriter::writeFile()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if (NULL == m)
  {
    std::stringstream ss;
    ss << "DataContainer Pointer was NULL and Must be valid." << __FILE__ << "(" << __LINE__<<")";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    setErrorCondition(-1);
    return -1;
  }
  std::stringstream ss;
  int64_t totalPoints = m->getTotalPoints();
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, numgrains, numensembles);
  if(getErrorCondition() < 0)
  {
    return -40;
  }

  int err = 0;
  size_t dims[3] =
  { 0, 0, 0 };
  m->getDimensions(dims);

  float res[3];
  m->getResolution(res);

  float origin[3];
  m->getOrigin(origin);

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  std::string parentPath = MXAFileInfo::parentPath(getOutputFile());
  if(!MXADir::mkdir(parentPath, true))
  {
    ss.str("");
    ss << "Error creating parent path '" << parentPath << "'";
    notifyErrorMessage(ss.str(), -1);
    setErrorCondition(-1);
    return -1;
  }

  FILE* f = fopen(getOutputFile().c_str(), "wb");
  if(NULL == f)
  {
    ss.str("");
    ss << "Error Opening File for writing '" << getOutputFile() << "'";
    notifyErrorMessage(ss.str(), -1);
    setErrorCondition(-1);
    return -1;
  }

  float phi1, phi, phi2;
  int32_t grainId;
  int32_t phaseId;

  size_t index = 0;
  for(size_t z = 0; z < dims[2]; ++z)
  {
    for (size_t y = 0; y < dims[1]; ++y)
    {
      for (size_t x = 0; x < dims[0]; ++x)
      {
        index = (z*dims[0]*dims[1]) + (dims[0]*y) + x;
        phi1 = m_CellEulerAngles[index*3] * 180.0*DREAM3D::Constants::k_1OverPi;
        phi = m_CellEulerAngles[index*3+1] * 180.0*DREAM3D::Constants::k_1OverPi;
        phi2 = m_CellEulerAngles[index*3+2] * 180.0*DREAM3D::Constants::k_1OverPi;

        grainId = m_GrainIds[index];
        phaseId = m_CellPhases[index];

        fprintf(f, "%.3f %.3f %.3f %lu %lu %lu %d %d\n",phi1, phi, phi2, x+1, y+1, z+1, grainId, phaseId);
      }
    }
  }



  fclose(f);


  notifyStatusMessage("Complete");
  return err;
}
