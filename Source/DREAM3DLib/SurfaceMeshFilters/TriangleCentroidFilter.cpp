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
/*
 * Your License or Copyright Information can go here
 */

#include "TriangleCentroidFilter.h"


#if DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "DREAM3DLib/Common/DREAM3DMath.h"

/**
 * @brief The CalculateCentroidsImpl class
 */
class CalculateCentroidsImpl
{
    StructArray<Node>::Pointer m_Nodes;
    StructArray<Triangle>::Pointer m_Triangles;
    double* m_Centroids;

  public:
    CalculateCentroidsImpl(StructArray<Node>::Pointer nodes, StructArray<Triangle>::Pointer triangles, double* centroids) :
      m_Nodes(nodes),
      m_Triangles(triangles),
      m_Centroids(centroids)
    {}
    virtual ~CalculateCentroidsImpl(){}

    void generate(size_t start, size_t end) const
    {

      Node* nodes = m_Nodes->GetPointer(0);
      Triangle* triangles = m_Triangles->GetPointer(0);

      for (size_t i = start; i < end; i++)
      {
        m_Centroids[i*3]  = ( nodes[triangles[i].node_id[0]].coord[0] + nodes[triangles[i].node_id[1]].coord[0] + nodes[triangles[i].node_id[2]].coord[0])/3.0;
        m_Centroids[i*3 + 1] = ( nodes[triangles[i].node_id[0]].coord[1] + nodes[triangles[i].node_id[1]].coord[1] + nodes[triangles[i].node_id[2]].coord[1])/3.0;
        m_Centroids[i*3 + 2]  = ( nodes[triangles[i].node_id[0]].coord[2] + nodes[triangles[i].node_id[1]].coord[2] + nodes[triangles[i].node_id[2]].coord[2])/3.0;
      }
    }

#if DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t> &r) const
    {
      generate(r.begin(), r.end());
    }
#endif


};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleCentroidFilter::TriangleCentroidFilter() :
AbstractFilter(),
m_SurfaceMeshTriangleCentroidsArrayName(DREAM3D::CellData::SurfaceMeshTriangleCentroids),
m_SurfaceMeshTriangleCentroids(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleCentroidFilter::~TriangleCentroidFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleCentroidFilter::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  /* Place all your option initialization code here */
  /* For String input use this code */
  /* {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("STL Output Prefix");
    parameter->setPropertyName("StlFilePrefix");
    parameter->setWidgetType(FilterParameter::StringWidget);
    parameter->setValueType("string");
    parameters.push_back(parameter);
  }*/
  /*  For an Integer use this code*/
  /* {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Max Iterations");
    parameter->setPropertyName("MaxIterations");
    parameter->setWidgetType(FilterParameter::IntWidget);
    parameter->setValueType("int");
    parameters.push_back(parameter);
  }*/
  /*  For a Floating point value use this code*/
  /* {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Misorientation Tolerance");
    parameter->setPropertyName("MisorientationTolerance");
    parameter->setWidgetType(FilterParameter::DoubleWidget);
    parameter->setValueType("float");
    parameter->setCastableValueType("double");
    parameters.push_back(parameter);
  }*/
  /*   For an input file use this code*/
  /*  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Input File");
    parameter->setPropertyName("InputFile");
    parameter->setWidgetType(FilterParameter::InputFileWidget);
    parameter->setValueType("string");
    parameters.push_back(parameter);
  }*/
  /*   For an output file use this code*/
  /* {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Alignment File");
    parameter->setPropertyName("AlignmentShiftFileName");
    parameter->setWidgetType(FilterParameter::OutputFileWidget);
    parameter->setValueType("string");
    parameters.push_back(parameter);
  }*/
  /*   For a simple true/false boolean use this code*/
  /* {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Write Alignment Shift File");
    parameter->setPropertyName("WriteAlignmentShifts");
    parameter->setWidgetType(FilterParameter::BooleanWidget);
    parameter->setValueType("bool");
    parameters.push_back(parameter);
  }*/
  /*   For presenting a set of choices to the user use this code*/
  /* {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Conversion Type");
    parameter->setPropertyName("ConversionType");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Degrees To Radians");
    choices.push_back("Radians To Degrees");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }*/


  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleCentroidFilter::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleCentroidFilter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-384);
  }
  else
  {
      // We MUST have Nodes
    if(sm->getNodes().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
      setErrorCondition(-384);
    }

    // We MUST have Triangles defined also.
    if(sm->getTriangles().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -383);
      setErrorCondition(-384);
    }
    else
    {
      CREATE_NON_PREREQ_DATA(sm, DREAM3D, CellData, SurfaceMeshTriangleCentroids, ss, double, DoubleArrayType, 0, voxels, 3)
    }

  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleCentroidFilter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleCentroidFilter::execute()
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
    StructArray<Node>::Pointer nodesPtr = getSurfaceMeshDataContainer()->getNodes();

  StructArray<Triangle>::Pointer trianglesPtr = getSurfaceMeshDataContainer()->getTriangles();
  size_t totalPoints = trianglesPtr->GetNumberOfTuples();

  // Run the data check to allocate the memory for the centroid array
  dataCheck(false, trianglesPtr->GetNumberOfTuples(), 0, 0);

#if DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                    CalculateCentroidsImpl(nodesPtr, trianglesPtr, m_SurfaceMeshTriangleCentroids), tbb::auto_partitioner());

#else
  CalculateCentroidsImpl serial(nodesPtr, trianglesPtr, m_SurfaceMeshTriangleCentroids);
  serial.generate(0, totalPoints);
#endif


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
