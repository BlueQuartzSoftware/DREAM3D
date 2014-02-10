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
#include "TriangleDihedralAngleFilter.h"


#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Math/DREAM3DMath.h"



/**
 * @brief The CalculateDihedralAnglesImpl class
 */
class CalculateDihedralAnglesImpl
{
    DREAM3D::SurfaceMesh::VertListPointer_t m_Nodes;
    DREAM3D::SurfaceMesh::FaceListPointer_t m_Triangles;
    double* m_DihedralAngles;

  public:
    CalculateDihedralAnglesImpl(DREAM3D::SurfaceMesh::VertListPointer_t nodes, DREAM3D::SurfaceMesh::FaceListPointer_t triangles, double* DihedralAngles) :
      m_Nodes(nodes),
      m_Triangles(triangles),
      m_DihedralAngles(DihedralAngles)
    {}
    virtual ~CalculateDihedralAnglesImpl(){}

    void generate(size_t start, size_t end) const
    {

      DREAM3D::SurfaceMesh::Vert_t* nodes = m_Nodes->GetPointer(0);
      DREAM3D::SurfaceMesh::Face_t* triangles = m_Triangles->GetPointer(0);

    float radToDeg = 180.0/DREAM3D::Constants::k_Pi;

    float ABx, ABy, ABz, ACx, ACy, ACz, BCx, BCy, BCz;
    float magAB, magAC, magBC;
    float dihedralAngle1, dihedralAngle2, dihedralAngle3, minDihedralAngle;
      for (size_t i = start; i < end; i++)
      {
    minDihedralAngle = 180.0;

    ABx = nodes[triangles[i].verts[0]].pos[0] - nodes[triangles[i].verts[1]].pos[0];
    ABy = nodes[triangles[i].verts[0]].pos[1] - nodes[triangles[i].verts[1]].pos[1];
    ABz = nodes[triangles[i].verts[0]].pos[2] - nodes[triangles[i].verts[1]].pos[2];
    magAB = sqrt(ABx*ABx+ABy*ABy+ABz*ABz);

    ACx = nodes[triangles[i].verts[0]].pos[0] - nodes[triangles[i].verts[2]].pos[0];
    ACy = nodes[triangles[i].verts[0]].pos[1] - nodes[triangles[i].verts[2]].pos[1];
    ACz = nodes[triangles[i].verts[0]].pos[2] - nodes[triangles[i].verts[2]].pos[2];
    magAC = sqrt(ACx*ACx+ACy*ACy+ACz*ACz);

    BCx = nodes[triangles[i].verts[1]].pos[0] - nodes[triangles[i].verts[2]].pos[0];
    BCy = nodes[triangles[i].verts[1]].pos[1] - nodes[triangles[i].verts[2]].pos[1];
    BCz = nodes[triangles[i].verts[1]].pos[2] - nodes[triangles[i].verts[2]].pos[2];
    magBC = sqrt(BCx*BCx+BCy*BCy+BCz*BCz);

    dihedralAngle1 = radToDeg*acos(((ABx*ACx)+(ABy*ACy)+(ABz*ACz))/(magAB*magAC));
    // 180 - angle because AB points out of vertex and BC points into vertex, so angle is actually angle outside of triangle
    dihedralAngle2 = 180.0 - (radToDeg*acos(((ABx*BCx)+(ABy*BCy)+(ABz*BCz))/(magAB*magBC)));
    dihedralAngle3 = radToDeg*acos(((BCx*ACx)+(BCy*ACy)+(BCz*ACz))/(magBC*magAC));
    minDihedralAngle = dihedralAngle1;
    if(dihedralAngle2 < minDihedralAngle) minDihedralAngle = dihedralAngle2;
    if(dihedralAngle3 < minDihedralAngle) minDihedralAngle = dihedralAngle3;
        m_DihedralAngles[i]  = minDihedralAngle;

      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t> &r) const
    {
      generate(r.begin(), r.end());
    }
#endif


};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleDihedralAngleFilter::TriangleDihedralAngleFilter() :
  SurfaceMeshFilter(),
  m_SurfaceMeshTriangleDihedralAnglesArrayName(DREAM3D::FaceData::SurfaceMeshFaceDihedralAngles),
  m_SurfaceMeshTriangleDihedralAngles(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleDihedralAngleFilter::~TriangleDihedralAngleFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleDihedralAngleFilter::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleDihedralAngleFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleDihedralAngleFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleDihedralAngleFilter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", getErrorCondition());
  }
  else
  {
    // We MUST have Nodes
    if(sm->getVertices().get() == NULL)
    {
      setErrorCondition(-384);
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
    }

    // We MUST have Triangles defined also.
    if(sm->getFaces().get() == NULL)
    {
      setErrorCondition(-385);
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
    }
    else
    {
      CREATE_NON_PREREQ_DATA(sm, DREAM3D, FaceData, SurfaceMeshTriangleDihedralAngles, ss, double, DoubleArrayType, 0, voxels, 1)
    }

  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleDihedralAngleFilter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleDihedralAngleFilter::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMeshing DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  notifyStatusMessage("Starting");

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = getSurfaceMeshDataContainer()->getVertices();

  DREAM3D::SurfaceMesh::FaceListPointer_t trianglesPtr = getSurfaceMeshDataContainer()->getFaces();
  size_t totalPoints = trianglesPtr->GetNumberOfTuples();

  // Run the data check to allocate the memory for the centroid array
  dataCheck(false, trianglesPtr->GetNumberOfTuples(), 0, 0);

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                      CalculateDihedralAnglesImpl(nodesPtr, trianglesPtr, m_SurfaceMeshTriangleDihedralAngles), tbb::auto_partitioner());

  }
  else
#endif
  {
    CalculateDihedralAnglesImpl serial(nodesPtr, trianglesPtr, m_SurfaceMeshTriangleDihedralAngles);
    serial.generate(0, totalPoints);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
