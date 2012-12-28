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

#include "GrainCurvatureFilter.h"



#include "DREAM3DLib/SurfaceMeshFilters/GenerateNodeTriangleConectivity.h"
#include "DREAM3DLib/SurfaceMeshFilters/TriangleCentroidFilter.h"
#include "DREAM3DLib/SurfaceMeshFilters/TriangleNormalFilter.h"

#include "CalculateTriangleGroupCurvatures.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainCurvatureFilter::GrainCurvatureFilter() :
AbstractFilter(),
  m_SurfaceMeshUniqueEdgesArrayName(DREAM3D::CellData::SurfaceMeshUniqueEdges),
  m_SurfaceMeshTriangleEdgesArrayName(DREAM3D::CellData::SurfaceMeshTriangleEdges),
  m_SurfaceMeshUniqueEdges(NULL),
  m_SurfaceMeshTriangleEdges(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainCurvatureFilter::~GrainCurvatureFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainCurvatureFilter::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> options;
  /* Place all your option initialization code here */
   /* For String input use this code */
  /* {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("STL Output Prefix");
    option->setPropertyName("StlFilePrefix");
    option->setWidgetType(FilterParameter::StringWidget);
    option->setValueType("string");
    options.push_back(option);
  }*/
   /*  For an Integer use this code*/
  /* {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Max Iterations");
    option->setPropertyName("MaxIterations");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }*/
   /*  For a Floating point value use this code*/
  /* {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Misorientation Tolerance");
    option->setPropertyName("MisorientationTolerance");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    options.push_back(option);
  }*/
  /*   For an input file use this code*/
 /*  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }*/
  /*   For an output file use this code*/
  /* {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Alignment File");
    option->setPropertyName("AlignmentShiftFileName");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }*/
  /*   For a simple true/false boolean use this code*/
  /* {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Alignment Shift File");
    option->setPropertyName("WriteAlignmentShifts");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }*/
  /*   For presenting a set of choices to the user use this code*/
  /* {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Conversion Type");
    option->setPropertyName("ConversionType");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Degrees To Radians");
    choices.push_back("Radians To Degrees");
    option->setChoices(choices);
    options.push_back(option);
  }*/


  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainCurvatureFilter::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainCurvatureFilter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
      // This depends on the triangles array already being created
      int size = sm->getTriangles()->GetNumberOfTuples();
      CREATE_NON_PREREQ_DATA(sm, DREAM3D, CellData, SurfaceMeshTriangleEdges, ss, int32_t, Int32ArrayType, 0, size, 3)
    }

    // We do not know the size of the array so we can not use the macro so we just manually call
    // the needed methods that will propagate these array additions to the pipeline
    DataArray<int>::Pointer uniqueEdgesArray = DataArray<int>::CreateArray(1, 2, DREAM3D::CellData::SurfaceMeshUniqueEdges);
    sm->addCellData(DREAM3D::CellData::SurfaceMeshUniqueEdges, uniqueEdgesArray);

    // This is just for tracking what Arrays are being created by this filter. Normally the macro
    // would do this for us.
    addCreatedCellData(DREAM3D::CellData::SurfaceMeshUniqueEdges);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainCurvatureFilter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainCurvatureFilter::execute()
{
 int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The SurfaceMesh DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  // Just to double check we have everything.
  dataCheck(false, 0,0,0);
  if (getErrorCondition() < 0)
  {
    return;
  }

  // We need to make sure we have up to date values for the Triangle Centroids and Triangle Normals
  // so we are going to recalculate them all just in case they are stale
  TriangleCentroidFilter::Pointer centroidFilter = TriangleCentroidFilter::New();
  centroidFilter->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
  centroidFilter->setMessagePrefix(getMessagePrefix());
  centroidFilter->execute();
  if (centroidFilter->getErrorCondition() < 0)
  {
    notifyErrorMessage("Error Generating the triangle centroids", -801);
    return;
  }

  TriangleNormalFilter::Pointer normalsFilter = TriangleNormalFilter::New();
  normalsFilter->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
  normalsFilter->setMessagePrefix(getMessagePrefix());
  normalsFilter->execute();
  if (normalsFilter->getErrorCondition() < 0)
  {
    notifyErrorMessage("Error Generating the triangle normals", -801);
    return;
  }


  // Make sure the Triangle Connectivity is created
  GenerateNodeTriangleConectivity::Pointer connectivity = GenerateNodeTriangleConectivity::New();
  connectivity->setSurfaceMeshDataContainer(getSurfaceMeshDataContainer());
  connectivity->setMessagePrefix(getMessagePrefix());
  connectivity->execute();
  if (connectivity->getErrorCondition() < 0)
  {
    notifyErrorMessage("Error Generating the Mesh Connectivity", -800);
    return;
  }
  NodeTrianglesMap_t& node2Triangle = connectivity->getNode2TriangleMap();


  // Get our Reference counted Array of Triangle Structures
  StructArray<Triangle>::Pointer trianglesPtr = getSurfaceMeshDataContainer()->getTriangles();
  if(NULL == trianglesPtr.get())
  {
    setErrorCondition(-556);
    notifyErrorMessage("The SurfaceMesh DataContainer Does NOT contain Triangles", -556);
    return;
  }
  int ntri = trianglesPtr->GetNumberOfTuples();


  // get the triangle definitions - use the pointer to the start of the Struct Array
  Triangle* triangles = trianglesPtr->GetPointer(0);


  // We need a filter to group triangles by grain face. Each Grain Face can belong to 2 different grains


// *********************** This next section should be task parallelized using TBB::task() ****************************
  // Test out the Curvature Codes for Grain ID = 10;
  int targetGrainId = 10;
  ss.str("");
  ss << "Finding all Triangles for Grain " << targetGrainId;
  notifyStatusMessage(ss.str());

  std::vector<int> triangleIds;
  for(int i = 0; i < ntri; ++i)
  {
    Triangle& tri = triangles[i];
    if (tri.nSpin[0] == targetGrainId || tri.nSpin[1] == targetGrainId)
    {
      triangleIds.push_back(i);
    }
  }


  ss.str("");
  ss << "Calculating Curvature for Grain " << targetGrainId;
  notifyStatusMessage(ss.str());
  CalculateTriangleGroupCurvatures curvature(3, triangleIds, targetGrainId, &node2Triangle, getSurfaceMeshDataContainer());
  curvature.execute();
// *********************** END END END END END END  ********************************************************************

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
