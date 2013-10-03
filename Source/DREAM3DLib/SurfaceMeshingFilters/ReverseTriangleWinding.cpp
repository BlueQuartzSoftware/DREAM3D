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

#include "ReverseTriangleWinding.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#endif


/**
 * @brief The CalculateNormalsImpl class is the actual code that does the computation and can be called either
 * from serial code or from Parallelized code (using TBB).
 */
class ReverseWindingImpl
{
    DREAM3D::Mesh::FaceListPointer_t m_Triangles;

  public:
    ReverseWindingImpl(DREAM3D::Mesh::FaceListPointer_t triangles) :
      m_Triangles(triangles)
    {}
    virtual ~ReverseWindingImpl() {}

    /**
     * @brief generate Generates the Normals for the triangles
     * @param start The starting Triangle Index
     * @param end The ending Triangle Index
     */
    void generate(size_t start, size_t end) const
    {
      DREAM3D::Mesh::Face_t* triangles = m_Triangles->GetPointer(0);

      for (size_t i = start; i < end; i++)
      {
        // Swap the indices
        int nId0 = triangles[i].verts[0];
        int nId2 = triangles[i].verts[2];

        triangles[i].verts[0] = nId2;
        triangles[i].verts[2] = nId0;
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    /**
     * @brief operator () This is called from the TBB stye of code
     * @param r The range to compute the values
     */
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      generate(r.begin(), r.end());
    }
#endif


};




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReverseTriangleWinding::ReverseTriangleWinding() :
  SurfaceMeshFilter()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReverseTriangleWinding::~ReverseTriangleWinding()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReverseTriangleWinding::setupFilterParameters()
{
  FilterParameterVector parameters;
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
void ReverseTriangleWinding::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int ReverseTriangleWinding::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
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
void ReverseTriangleWinding::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  SurfaceDataContainer* sm = getSurfaceDataContainer();
  if(NULL == sm)
  {
    setErrorCondition(-383);
    addErrorMessage(getHumanLabel(), "SurfaceDataContainer is missing", getErrorCondition());
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
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReverseTriangleWinding::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReverseTriangleWinding::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceDataContainer* m = getSurfaceDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMesh DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  notifyStatusMessage("Starting");

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  bool doParallel = true;
#endif

  DREAM3D::Mesh::FaceListPointer_t trianglesPtr = getSurfaceDataContainer()->getFaces();
  size_t totalPoints = trianglesPtr->GetNumberOfTuples();

  // Run the data check to allocate the memory for the centroid array
  dataCheck(false, trianglesPtr->GetNumberOfTuples(), 0, 0);

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                      ReverseWindingImpl(trianglesPtr), tbb::auto_partitioner());

  }
  else
#endif
  {
    ReverseWindingImpl serial(trianglesPtr);
    serial.generate(0, totalPoints);
  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
