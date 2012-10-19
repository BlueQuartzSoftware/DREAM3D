/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "YSChoiAbaqusReader.h"


// C Includes
#include <stdio.h>

// C++ Includes
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

#include <map>

#include "MXA/Common/LogTime.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/MatrixMath.h"

#define DIMS "DIMENSIONS"
#define RES "SPACING"
#define LOOKUP "LOOKUP_TABLE"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
YSChoiAbaqusReader::YSChoiAbaqusReader() :
FileReader(),
m_InputFile(""),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_SurfaceFieldsArrayName(DREAM3D::FieldData::SurfaceFields),
m_QuatsArrayName(DREAM3D::CellData::Quats),
m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
m_GrainIds(NULL),
m_CellPhases(NULL),
m_SurfaceFields(NULL),
m_Quats(NULL),
m_CellEulerAngles(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
YSChoiAbaqusReader::~YSChoiAbaqusReader()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("InputFile", getInputFile() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void YSChoiAbaqusReader::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if (getInputFile().empty() == true)
  {
    std::stringstream ss;
    ss << ClassName() << " needs the Input File Set and it was not.";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    setErrorCondition(-387);
  }

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, float, FloatArrayType, 0, voxels, 3)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, Quats, ss, float, FloatArrayType, 0, voxels, 5)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, int32_t, Int32ArrayType, 1, voxels, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, FieldData, SurfaceFields, ss, bool, BoolArrayType, false, fields, 1)
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, 0, voxels, 1)

  typedef DataArray<unsigned int> XTalStructArrayType;
  CREATE_NON_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, unsigned int, XTalStructArrayType, Ebsd::CrystalStructure::Cubic, ensembles, 1)
}

void YSChoiAbaqusReader::preflight()
{
  dataCheck(true, 1, 1, 1);
}
void YSChoiAbaqusReader::execute()
{
    VoxelDataContainer* m = getVoxelDataContainer();

  int xpoints, ypoints, zpoints, totalpoints;
  int numgrains = 0;
  float resx, resy, resz;
    float ***mat;
    const unsigned int size(1024);
    char buf[size];
    std::ifstream in(getInputFile().c_str());
    std::string word;
    bool headerdone = false;
    while (headerdone == false)
    {
      in.getline(buf, size);
      std::string line = buf;
      in >> word;
      if (DIMS == word)
      {
        in >> xpoints >> ypoints >> zpoints;
      size_t dims[3] = {xpoints, ypoints, zpoints};
      m->setDimensions(dims);
        totalpoints = xpoints * ypoints * zpoints;
        mat = new float **[totalpoints];
      }
    if (RES == word)
      {
        in >> resx >> resy >> resz;
      float res[3] = {resx, resy, resz};
    m->setResolution(res);
      }
      if (LOOKUP == word)
      {
        headerdone = true;
        in >> word;
      }
    }
  dataCheck(false, totalpoints, 1, 2);
    int gnum = 0;
    bool onedge = false;
    int col, row, plane;
    float value;
    for (int i = 0; i < totalpoints; i++)
    {
    mat[i] = new float *[3];
    for(int j=0;j<3;j++)
    {
    mat[i][j] = new float [3];
    }
      onedge = false;
      in >> gnum;
      col = i % xpoints;
      row = (i / xpoints) % ypoints;
      plane = i / (xpoints * ypoints);
      if (col == 0 || col == (xpoints - 1) || row == 0 || row == (ypoints - 1) || plane == 0 || plane == (zpoints - 1)) onedge = true;
      m_GrainIds[i] = gnum;
      if (gnum >= numgrains)
      {
        numgrains = gnum + 1;
      m->resizeFieldDataArrays(numgrains);
      dataCheck(false, totalpoints, numgrains, 2);
      }
      m_SurfaceFields[gnum] = onedge;
    }
    for (int iter1 = 0; iter1 < 3; iter1++)
    {
    for (int iter2 = 0; iter2 < 3; iter2++)
    {
      headerdone = false;
      while (headerdone == false)
      {
      in.getline(buf, size);
      std::string line = buf;
      in >> word;
      if (LOOKUP == word)
      {
        headerdone = true;
        in >> word;
      }
      }
      for (int i = 0; i < totalpoints; i++)
      {
      onedge = 0;
      in >> value;
      mat[i][iter1][iter2] = value;
      }
    }
    }
  float ea1, ea2, ea3;

  float q[5];
  float g[3][3];
  for(int i=0;i<(xpoints*ypoints*zpoints);i++)
  {
    for(int j=0;j<3;j++)
    {
      for(int k=0;k<3;k++)
      {
        g[j][k] = mat[i][j][k];
      }
    }
    MatrixMath::normalize3x3(g);
    q[0] = 1;
    q[4] = static_cast<float>( sqrt((1.0+g[0][0]+g[1][1]+g[2][2]))/2 );
    q[1] = static_cast<float>( (g[1][2]-g[2][1])/(4*q[4]) );
    q[2] = static_cast<float>( (g[2][0]-g[0][2])/(4*q[4]) );
    q[3] = static_cast<float>( (g[0][1]-g[1][0])/(4*q[4]) );
    m_Quats[5*i] = 1;
    m_Quats[5*i+1] = q[1];
    m_Quats[5*i+2] = q[2];
    m_Quats[5*i+3] = q[3];
    m_Quats[5*i+4] = q[4];
    OrientationMath::QuattoEuler(q, ea1, ea2, ea3);
    m_CellEulerAngles[3*i] = ea1;
    m_CellEulerAngles[3*i + 1] = ea2;
    m_CellEulerAngles[3*i + 2] = ea3;
      delete[] mat[i];
    }
    delete[] mat;
}
