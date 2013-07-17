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

#include "DxWriter.h"

#include <iostream>
#include <fstream>

#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxWriter::DxWriter() :
    FileWriter(), m_GrainIdsArrayName(DREAM3D::CellData::GrainIds), m_AddSurfaceLayer(false), m_GrainIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DxWriter::~DxWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxWriter::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.dx");
    option->setFileType("Open DX Visualization");
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Add Surface Layer");
    option->setPropertyName("AddSurfaceLayer");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxWriter::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DxWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile());
  writer->writeValue("AddSurfaceLayer", getAddSurfaceLayer());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if (getOutputFile().empty() == true)
  {
    ss <<  ": The output file must be set before executing this filter.";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    setErrorCondition(-1);
  }

  std::string parentPath = MXAFileInfo::parentPath(getOutputFile());
  if (MXADir::exists(parentPath) == false)
  {
    ss.str("");
    ss <<  "The directory path for the output file does not exist.";
    addWarningMessage(getHumanLabel(), ss.str(), -1);
  }

  if (MXAFileInfo::extension(getOutputFile()).compare("") == 0)
  {
    setOutputFile(getOutputFile().append(".dx"));
  }

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DxWriter::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DxWriter::writeHeader()
{
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DxWriter::writeFile()
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

  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, totalPoints, 1, 1);
  if(getErrorCondition() < 0)
  {
    return -40;
  }

  //GET_NAMED_ARRAY_SIZE_CHK_NOMSG_RET(m, Cell, DREAM3D::CellData::GrainIds, Int32ArrayType, int32_t, totalPoints, grain_indicies);

  int err = 0;
  size_t udims[3] =
  { 0, 0, 0 };
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  { static_cast<DimType>(udims[0]), static_cast<DimType>(udims[1]), static_cast<DimType>(udims[2]), };
  // std::cout << "Write Dx Grain File:  x, y, z: " << dims[0] << " " << dims[1] << " " << dims[2] << std::endl;

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  std::string parentPath = MXAFileInfo::parentPath(getOutputFile());
  if(!MXADir::mkdir(parentPath, true))
  {
    std::stringstream ss;
    ss << "Error creating parent path '" << parentPath << "'";
    notifyErrorMessage(ss.str(), -1);
    setErrorCondition(-1);
    return -1;
  }

  std::ofstream out(getOutputFile().c_str(), std::ios_base::binary);
  if(out.is_open() == false)
  {
    return -1;
  }

  DimType fileXDim = dims[0];
  DimType fileYDim = dims[1];
  DimType fileZDim = dims[2];

  DimType posXDim = fileXDim + 1;
  DimType posYDim = fileYDim + 1;
  DimType posZDim = fileZDim + 1;

  if(m_AddSurfaceLayer)
  {
    fileXDim = dims[0] + 2;
    fileYDim = dims[1] + 2;
    fileZDim = dims[2] + 2;

    posXDim = fileXDim + 1;
    posYDim = fileYDim + 1;
    posZDim = fileZDim + 1;
  }
  totalPoints = fileXDim * fileYDim * fileZDim;
  //Write the header
  out << "object 1 class gridpositions counts " << posZDim << " " << posYDim << " " << posXDim << std::endl;
  out << "origin 0 0 0" << std::endl;
  out << "delta  1 0 0" << std::endl;
  out << "delta  0 1 0" << std::endl;
  out << "delta  0 0 1" << std::endl;
  out << std::endl;
  out << "object 2 class gridconnections counts " << posZDim << " " << posYDim << " " << posXDim << std::endl;
  out << std::endl;
  out << "object 3 class array type int rank 0 items " << totalPoints << " data follows" << std::endl;

  // Add a complete layer of surface voxels
  size_t rnIndex = 1;
  if(m_AddSurfaceLayer)
  {
    for (int i = 0; i < (fileXDim * fileYDim); ++i)
    {
      out << "-3 ";
      if(rnIndex == 20)
      {
        rnIndex = 0;
        out << std::endl;
      }
      rnIndex++;
    }
  }

  int index = 0;
  for (DimType z = 0; z < dims[2]; ++z)
  {
    // Add a leading surface Row for this plane if needed
    if(m_AddSurfaceLayer)
    {
      for (int i = 0; i < fileXDim; ++i)
      {
        out << "-4 ";
      }
      out << std::endl;
    }
    for (DimType y = 0; y < dims[1]; ++y)
    {
      // write leading surface voxel for this row
      if(m_AddSurfaceLayer)
      {
        out << "-5 ";
      }
      // Write the actual voxel data
      for (DimType x = 0; x < dims[0]; ++x)
      {
        if(m_GrainIds[index] == 0)
        {
          out << "0" << " ";
        }
        else
        {
          out << m_GrainIds[index] << " ";
        }
        ++index;
      }
      // write trailing surface voxel for this row
      if(m_AddSurfaceLayer)
      {
        out << "-6 ";
      }
      out << std::endl;
    }
    // Add a trailing surface Row for this plane if needed
    if(m_AddSurfaceLayer)
    {
      for (int i = 0; i < fileXDim; ++i)
      {
        out << "-7 ";
      }
      out << std::endl;
    }
  }

  // Add a complete layer of surface voxels
  if(m_AddSurfaceLayer)
  {
    rnIndex = 1;
    for (int i = 0; i < (fileXDim * fileYDim); ++i)
    {
      out << "-8 ";
      if(rnIndex == 20)
      {
        out << std::endl;
        rnIndex = 0;
      }
      rnIndex++;
    }
  }
  out << std::endl;
  out << "attribute \"dep\" string \"connections\"" << std::endl;
  out << std::endl;
  out << "object \"DREAM3D Generated\" class field" << std::endl;
  out << "component  \"positions\"    value 1" << std::endl;
  out << "component  \"connections\"  value 2" << std::endl;
  out << "component  \"data\"         value 3" << std::endl;
  out << "" << std::endl;
  out << "end" << std::endl;

  out.close();
#if 0
  out.open("/tmp/m3cmesh.raw", std::ios_base::binary);
  out.write((const char*)(&dims[0]), 4);
  out.write((const char*)(&dims[1]), 4);
  out.write((const char*)(&dims[2]), 4);
  getTotalPoints = dims[0] * dims[1] * dims[2];
  int32_t d = 0;
  for(int index = 0; index < getTotalPoints; ++index)
  {
    d = grainIds[index];
    if (d == 0)
    { d = -3;}
    out.write((const char*)(&d), sizeof(d));
  }

  out.close();
#endif

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Complete");
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

