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
#include "ArraySelectionExample.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ArraySelectionExample::ArraySelectionExample() :
  AbstractFilter(),
  //  m_SelectedVoxelCellArrays(FIX_ME<<<<<<<<),
  //  m_SelectedVoxelFeatureArrays(FIX_ME<<<<<<<<),
  //  m_SelectedVoxelEnsembleArrays(FIX_ME<<<<<<<<),
  //  m_SelectedSurfaceMeshVertexArrays(FIX_ME<<<<<<<<),
  //  m_SelectedSurfaceMeshFaceArrays(FIX_ME<<<<<<<<),
  //  m_SelectedSurfaceMeshEdgeArrays(FIX_ME<<<<<<<<),
  //  m_SelectedSurfaceMeshFeatureArrays(FIX_ME<<<<<<<<),
  //  m_SelectedSurfaceMeshEnsembleArrays(FIX_ME<<<<<<<<),
  //  m_SelectedSolidMeshVertexArrays(FIX_ME<<<<<<<<),
  //  m_SelectedSolidMeshFaceArrays(FIX_ME<<<<<<<<),
  //  m_SelectedSolidMeshEdgeArrays(FIX_ME<<<<<<<<),
  //  m_CellComparisonInputs(FIX_ME<<<<<<<<),
  //  m_FeatureComparisonInputs(FIX_ME<<<<<<<<),
  //  m_EnsembleComparisonInputs(FIX_ME<<<<<<<<),
  //  m_PointComparisonInputs(FIX_ME<<<<<<<<),
  //  m_FaceComparisonInputs(FIX_ME<<<<<<<<),
  //  m_EdgeComparisonInputs(FIX_ME<<<<<<<<),
  //  m_ComparisonInputs(FIX_ME<<<<<<<<),
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ArraySelectionExample::~ArraySelectionExample()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> options;
  /* To select arrays */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Arrays to Delete");
    option->setPropertyName("ArraysToDelete");
    option->setWidgetType(FilterParameter::ArraySelectionWidget);
    options.push_back(option);
  }


  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedVoxelCellArrays( reader->readValue("SelectedVoxelCellArrays", getSelectedVoxelCellArrays()) );
  setSelectedVoxelFeatureArrays( reader->readValue("SelectedVoxelFeatureArrays", getSelectedVoxelFeatureArrays()) );
  setSelectedVoxelEnsembleArrays( reader->readValue("SelectedVoxelEnsembleArrays", getSelectedVoxelEnsembleArrays()) );
  setSelectedSurfaceMeshVertexArrays( reader->readValue("SelectedSurfaceMeshVertexArrays", getSelectedSurfaceMeshVertexArrays()) );
  setSelectedSurfaceMeshFaceArrays( reader->readValue("SelectedSurfaceMeshFaceArrays", getSelectedSurfaceMeshFaceArrays()) );
  setSelectedSurfaceMeshEdgeArrays( reader->readValue("SelectedSurfaceMeshEdgeArrays", getSelectedSurfaceMeshEdgeArrays()) );
  setSelectedSurfaceMeshFeatureArrays( reader->readValue("SelectedSurfaceMeshFeatureArrays", getSelectedSurfaceMeshFeatureArrays()) );
  setSelectedSurfaceMeshEnsembleArrays( reader->readValue("SelectedSurfaceMeshEnsembleArrays", getSelectedSurfaceMeshEnsembleArrays()) );
  setSelectedSolidMeshVertexArrays( reader->readValue("SelectedSolidMeshVertexArrays", getSelectedSolidMeshVertexArrays()) );
  setSelectedSolidMeshFaceArrays( reader->readValue("SelectedSolidMeshFaceArrays", getSelectedSolidMeshFaceArrays()) );
  setSelectedSolidMeshEdgeArrays( reader->readValue("SelectedSolidMeshEdgeArrays", getSelectedSolidMeshEdgeArrays()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ArraySelectionExample::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedVoxelCellArrays", getSelectedVoxelCellArrays());
  writer->writeValue("SelectedVoxelFeatureArrays", getSelectedVoxelFeatureArrays());
  writer->writeValue("SelectedVoxelEnsembleArrays", getSelectedVoxelEnsembleArrays());
  writer->writeValue("SelectedSurfaceMeshVertexArrays", getSelectedSurfaceMeshVertexArrays());
  writer->writeValue("SelectedSurfaceMeshFaceArrays", getSelectedSurfaceMeshFaceArrays());
  writer->writeValue("SelectedSurfaceMeshEdgeArrays", getSelectedSurfaceMeshEdgeArrays());
  writer->writeValue("SelectedSurfaceMeshFeatureArrays", getSelectedSurfaceMeshFeatureArrays());
  writer->writeValue("SelectedSurfaceMeshEnsembleArrays", getSelectedSurfaceMeshEnsembleArrays());
  writer->writeValue("SelectedSolidMeshVertexArrays", getSelectedSolidMeshVertexArrays());
  writer->writeValue("SelectedSolidMeshFaceArrays", getSelectedSolidMeshFaceArrays());
  writer->writeValue("SelectedSolidMeshEdgeArrays", getSelectedSolidMeshEdgeArrays());
  writer->closeFilterGroup();
  return index;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::setVoxelSelectedArrayNames(QSet<QString> selectedCellArrays,
                                                       QSet<QString> selectedFeatureArrays,
                                                       QSet<QString> selectedEnsembleArrays)
{
  m_SelectedVoxelCellArrays = selectedCellArrays;
  m_SelectedVoxelFeatureArrays = selectedFeatureArrays;
  m_SelectedVoxelEnsembleArrays = selectedEnsembleArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::setSurfaceMeshSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                                             QSet<QString> selectedFaceArrays,
                                                             QSet<QString> selectedEdgeArrays,
                                                             QSet<QString> selectedFeatureArrays,
                                                             QSet<QString> selectedEnsembleArrays)
{
  m_SelectedSurfaceMeshVertexArrays = selectedVertexArrays;
  m_SelectedSurfaceMeshFaceArrays = selectedFaceArrays;
  m_SelectedSurfaceMeshEdgeArrays = selectedEdgeArrays;
  m_SelectedSurfaceMeshFeatureArrays = selectedFeatureArrays;
  m_SelectedSurfaceMeshEnsembleArrays = selectedEnsembleArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::setSolidMeshSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                                           QSet<QString> selectedFaceArrays,
                                                           QSet<QString> selectedEdgeArrays)
{
  m_SelectedSolidMeshVertexArrays = selectedVertexArrays;
  m_SelectedSolidMeshFaceArrays = selectedFaceArrays;
  m_SelectedSolidMeshEdgeArrays = selectedEdgeArrays;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  /* Example code for preflighting looking for a valid string for the output file
   * but not necessarily the fact that the file exists: Example code to make sure
   * we have something in a string before proceeding.*/
  /*
  if (m_OutputFile.empty() == true)
  {
    ss << "The output file must be set before executing this filter.";
    PipelineMessage em(getNameOfClass(), "There was an error", -666);
    addErrorMessage(em);
    setErrorCondition(-1);
  }
  */

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::execute()
{
  int err = 0;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", getErrorCondition());
    return;
  }
  setErrorCondition(0);

  /* Place all your code to execute your filter here. */

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

