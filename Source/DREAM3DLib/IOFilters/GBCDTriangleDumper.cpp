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

#include "DREAM3DLib/Common/MatrixMath.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"

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
  m_FieldEulerAnglesArrayName(DREAM3D::FieldData::FieldEulerAngles),
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
    option->setFileExtension("*.ph");
    option->setFileType("CMU Grain Growth");
    option->setValueType("string");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->writeValue("OutputFile", getOutputFile() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GBCDTriangleDumper::dataCheckSurfaceMesh(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;

  if(getOutputFile().empty() == true)
  {
    ss.str("");
    ss << ClassName() << " needs the Output File Set and it was not.";
    addErrorMessage(getHumanLabel(), ss.str(), -1);
    setErrorCondition(-387);
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
  //      GET_PREREQ_DATA(m, DREAM3D, FieldData, FieldPhases, ss, -302, int32_t, Int32ArrayType,  fields, 1)
 //       typedef DataArray<unsigned int> XTalStructArrayType;
 //   GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1)
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


  DREAM3D::SurfaceMesh::FaceListPointer_t trianglesPtr = sm->getFaces();
  size_t totalFaces = trianglesPtr->GetNumberOfTuples();

  // Run the data check to allocate the memory for the centroid array
  // Note the use of the voxel datacontainer num ensembles to set the gbcd size
  dataCheckSurfaceMesh(false, 0, totalFaces, m->getNumEnsembleTuples());

  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembles = m->getNumEnsembleTuples();

  FILE* f = fopen(getOutputFile().c_str(), "wb");
  if (NULL == f)
  {
    setErrorCondition(-87000);
    notifyErrorMessage("Could not open Output file for writing.", getErrorCondition());
    return;
  }

  dataCheckVoxel(false, 0, totalFields, totalEnsembles);


  float radToDeg = 180.0/M_PI;

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

    fprintf(f, "%0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f\n", euAng0[0]*radToDeg, euAng0[1]*radToDeg, euAng0[2]*radToDeg,
    euAng1[0]*radToDeg, euAng1[1]*radToDeg, euAng1[2]*radToDeg,
    tNorm[0], tNorm[1], tNorm[2], m_SurfaceMeshFaceAreas[t]);


  }


  fclose(f);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

