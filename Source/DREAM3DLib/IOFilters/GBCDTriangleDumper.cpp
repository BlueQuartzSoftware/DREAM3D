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
#include "GBCDTriangleDumper.h"

#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"


#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/DataArrays/StringDataArray.hpp"



const static float m_pi = static_cast<float>(M_PI);
const static float m_pi2 = static_cast<float>(2*M_PI);


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GBCDTriangleDumper::GBCDTriangleDumper() :
  SurfaceMeshFilter(),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceAreasArrayName(DREAM3D::FaceData::SurfaceMeshFaceAreas),
  m_SurfaceMeshFaceNormalsArrayName(DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshPhaseLabelsArrayName(DREAM3D::FaceData::SurfaceMeshPhaseLabels),
  m_FieldEulerAnglesArrayName(DREAM3D::FieldData::EulerAngles),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_SurfaceMeshFaceAreas(NULL),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFaceNormals(NULL),
  m_FieldEulerAngles(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GBCDTriangleDumper::~GBCDTriangleDumper()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;

  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.txt");
    option->setFileType("CMU GBCD Triangles");
    option->setValueType("string");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setOutputFile(reader->readValue("OutputFile", getOutputFile() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GBCDTriangleDumper::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;

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
    ss <<  "The directory path for the output file does not exist but will be created during execution.";
    addWarningMessage(getHumanLabel(), ss.str(), -1);
  }

  if (MXAFileInfo::extension(getOutputFile()).compare("") == 0)
  {
    setOutputFile(getOutputFile().append(".txt"));
  }



  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-383);
  }
  else
  {
    // We MUST have Nodes
    if(sm->getVertices().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
      setErrorCondition(-384);
    }

    // We MUST have Triangles defined also.
    if(sm->getFaces().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -383);
      setErrorCondition(-384);
    }
    else
    {
      GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceLabels, ss, -386, int32_t, Int32ArrayType, fields, 2)
      GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceNormals, ss, -387, double, DoubleArrayType, fields, 3)
      GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshFaceAreas, ss, -388, double, DoubleArrayType, fields, 1)
      GET_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshPhaseLabels, ss, -30, int32_t, Int32ArrayType, sm->getNumFaceTuples(), 2)
    }

  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::dataCheckVoxel(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    addErrorMessage(getHumanLabel(), "VoxelDataContainer is missing", -383);
    setErrorCondition(-383);
  }
  else
  {
    GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldEulerAngles, ss, -301, float, FloatArrayType, fields, 3)
        // GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -302, int32_t, Int32ArrayType,  fields, 1)
        typedef DataArray<unsigned int> XTalStructArrayType;
    GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1)
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheckSurfaceMesh(true, 1, 1, 1);
  dataCheckVoxel(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMeshing DataContainer Object was NULL", -999);
    return;
  }
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMeshing DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  notifyStatusMessage("Starting");

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  std::string parentPath = MXAFileInfo::parentPath(getOutputFile());
  if(!MXADir::mkdir(parentPath, true))
  {
    std::stringstream ss;
    ss << "Error creating parent path '" << parentPath << "'";
    notifyErrorMessage(ss.str(), -1);
    setErrorCondition(-1);
    return;
  }

  DREAM3D::SurfaceMesh::FaceListPointer_t trianglesPtr = sm->getFaces();
  size_t totalFaces = trianglesPtr->GetNumberOfTuples();

  // Run the data check to allocate the memory for the centroid array
  // Note the use of the voxel datacontainer num ensembles to set the gbcd size
  dataCheckSurfaceMesh(false, 0, totalFaces, m->getNumEnsembleTuples());

  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembles = m->getNumEnsembleTuples();

  dataCheckVoxel(false, 0, totalFields, totalEnsembles);

  FILE* f = fopen(getOutputFile().c_str(), "wb");
  if (NULL == f)
  {
    setErrorCondition(-87000);
    notifyErrorMessage("Could not open Output file for writing.", getErrorCondition());
    return;
  }

  UInt32ArrayType::Pointer xtalPtr = boost::dynamic_pointer_cast<UInt32ArrayType>(m->getEnsembleData(m_CrystalStructuresArrayName));

  // Attempt to get the Material Name array if possible otherwise just use "Unknown" for everything
  std::vector<std::string> matNames(xtalPtr->GetSize(), "Unkonwn");
  StringDataArray::Pointer matNamesPtr = boost::dynamic_pointer_cast<StringDataArray>(m->getEnsembleData(DREAM3D::EnsembleData::MaterialName));
  if(NULL != matNamesPtr.get() )
  {
    matNames = matNamesPtr->getStringArray();
  }
  fprintf(f, "# Triangles Produced from DREAM3D version %s\n", DREAM3DLib::Version::Package().c_str() );
  fprintf(f, "# This file is intended to be used by the GBCD codes to calculate the GBCD for this structure\n");
  fprintf(f, "# Column 1-3:    right hand average orientation (phi1, PHI, phi2 in Radians)\n");
  fprintf(f, "# Column 4-6:    left hand average orientation (phi1, PHI, phi2 in Radians)\n");
  fprintf(f, "# Column 7-9:    triangle normal\n");
  fprintf(f, "# Column 8:      surface area\n");
  fprintf(f, "# Column 9,10:   Left hand Phase Id, Right hand Phase Id\n");
  fprintf(f, "# Phase indexing starts at 1. Phase Zero is for Internal DREAM3D use only\n");
  // Get a list of all the crystal structures
  std::vector<OrientationOps::Pointer> ops = OrientationOps::getOrientationOpsVector();

  size_t xtalCount = xtalPtr->GetSize();
  // Start from 1 because the Zero slot is ALWAYS a dummy crystal structure
  for(size_t i = 1; i < xtalCount; i++)
  {
    fprintf(f, "# Phase: %ld\n# Crystal Symmetry: %s\n# Material Name: %s \n", i, ops[m_CrystalStructures[i]]->getSymmetryName().c_str(), matNames[i].c_str() );
  }

  int gid0 = 0; // Grain id 0
  int gid1 = 0; // Grain id 1
  float* euAng0 = NULL;
  float* euAng1 = NULL;
  double* tNorm = NULL;
  for(size_t t = 0; t < totalFaces; ++t)
  {
    // Get the Grain Ids for the triangle
    gid0 = m_SurfaceMeshFaceLabels[t*2];
    gid1 = m_SurfaceMeshFaceLabels[t*2 + 1];

    if(gid0 < 0)
    {
      continue;
    }
    if(gid1 < 0)
    {
      continue;
    }

    // Now get the Euler Angles for that grain id, WATCH OUT: This is pointer arithmetic
    euAng0 = m_FieldEulerAngles + (gid0 * 3);
    euAng1 = m_FieldEulerAngles + (gid1 * 3);

    // Get the Triangle Normal
    tNorm = m_SurfaceMeshFaceNormals + (t * 3);

    fprintf(f, "%0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %d %d\n", euAng0[0], euAng0[1], euAng0[2],
        euAng1[0], euAng1[1], euAng1[2],
        tNorm[0], tNorm[1], tNorm[2], m_SurfaceMeshFaceAreas[t], m_SurfaceMeshPhaseLabels[t*2], m_SurfaceMeshPhaseLabels[t*2+1]);
  }


  fclose(f);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

