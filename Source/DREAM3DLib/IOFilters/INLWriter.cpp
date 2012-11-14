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

#include "INLWriter.h"

#include <iostream>
#include <fstream>

#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/Common/EbsdColoring.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
INLWriter::INLWriter() :
FileWriter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
m_MaterialNamesArrayName(DREAM3D::EnsembleData::MaterialName),
m_GrainIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
INLWriter::~INLWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void INLWriter::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("txt");
    option->setFileType("INL Format");
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void INLWriter::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("OutputFile", getOutputFile());
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void INLWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1)


  #if (__APPLE__)
    #warning Fix code that checks for a required Ensemble data that is of type StringDataArray
//  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, MaterialNames, ss, -304, std::string, StringDataArray, ensembles, 1)

  #endif

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void INLWriter::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int INLWriter::writeHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int INLWriter::writeFile()
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
  dataCheck(false, totalPoints, 1, 1);
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




  // Write the header, Each line starts with a "#" symbol
  fprintf(f, "# XSTEP: %f\r\n", res[0]);
  fprintf(f, "# YSTEP: %f\r\n", res[1]);
  fprintf(f, "# ZSTEP: %f\r\n", res[2]);

  fprintf(f, "# X_DIM: %llu\r\n", static_cast<unsigned long long int>(dims[0]));
  fprintf(f, "# Y_DIM: %llu\r\n", static_cast<unsigned long long int>(dims[1]));
  fprintf(f, "# Z_DIM: %llu\r\n", static_cast<unsigned long long int>(dims[2]));

  fprintf(f, "# Column 1-3: phi1, PHI, phi2 (orientation of point in radians)\r\n");
  fprintf(f, "# Column 4-6: x, y, z (coordinates of point in microns)\r\n");
  fprintf(f, "# Column 7: Grain ID\r\n");
  fprintf(f, "# Column 8: Phase ID\r\n");
  fprintf(f, "# Column 9-11: RGB (Reference Direction [001])\r\n");

  float phi1, phi, phi2;
  float xPos, yPos, zPos;
  int32_t grainId;
  int32_t phaseId;

  unsigned char rgba[4] = {0,0,0,255};
  unsigned char hkl[3] = { 0, 0, 0 };
  float refDir[3] = {0.0f, 0.0f, 1.0f};
  uint32_t symmetry = 0;

  size_t index = 0;
  for(size_t z = 0; z < dims[2]; ++z)
  {
    for (size_t y = 0; y < dims[1]; ++y)
    {
      for (size_t x = 0; x < dims[0]; ++x)
      {
        index = (z*dims[0]*dims[1]) + (dims[0]*y) + x;
        phi1 = m_CellEulerAngles[index*3];
        phi = m_CellEulerAngles[index*3+1];
        phi2 = m_CellEulerAngles[index*3+2];
        xPos = origin[0] + (x * res[0]);
        yPos = origin[1] + (y * res[1]);
        zPos = origin[2] + (z * res[2]);
        grainId = m_GrainIds[index];
        phaseId = m_CellPhases[index];
        rgba[0] = 0; rgba[1] = 0; rgba[2] = 0; // Reset the color to black
        symmetry = m_CrystalStructures[phaseId];
        if(phaseId > 0)
        {
          if(symmetry == Ebsd::CrystalStructure::Cubic)
          {
            EbsdColoring::GenerateIPFColor(phi1, phi, phi2, refDir[0], refDir[1], refDir[2], rgba, hkl);
          }
          else if(symmetry == Ebsd::CrystalStructure::Hexagonal)
          {
            EbsdColoring::CalculateHexIPFColor(phi1, phi, phi2, refDir[0], refDir[1], refDir[2], rgba);
          }
        }


        fprintf(f, "%f %f %f %f %f %f %d %d %d\r\n",phi1, phi, phi2, xPos, yPos, zPos, grainId, phaseId, symmetry);
      }
    }
  }



  fclose(f);


  notifyStatusMessage("Complete");
  return err;
}
