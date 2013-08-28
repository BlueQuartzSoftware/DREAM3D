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

#include "RemoveArrays.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveArrays::RemoveArrays() :
  AbstractFilter()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveArrays::~RemoveArrays()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Arrays to Delete");
    parameter->setPropertyName("ArraysToDelete");
    parameter->setWidgetType(FilterParameter::ArraySelectionWidget);
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedVoxelCellArrays( reader->readValue("SelectedVoxelCellArrays", getSelectedVoxelCellArrays() ) );
  setSelectedVoxelFieldArrays( reader->readValue("SelectedVoxelFieldArrays", getSelectedVoxelFieldArrays() ) );
  setSelectedVoxelEnsembleArrays( reader->readValue("SelectedVoxelEnsembleArrays", getSelectedVoxelEnsembleArrays() ) );
  setSelectedSurfaceMeshVertexArrays( reader->readValue("SelectedSurfaceMeshVertexArrays", getSelectedSurfaceMeshVertexArrays() ) );
  setSelectedSurfaceMeshFaceArrays( reader->readValue("SelectedSurfaceMeshFaceArrays", getSelectedSurfaceMeshFaceArrays() ) );
  setSelectedSurfaceMeshEdgeArrays( reader->readValue("SelectedSurfaceMeshEdgeArrays", getSelectedSurfaceMeshEdgeArrays() ) );
  setSelectedSurfaceMeshFieldArrays( reader->readValue("SelectedSurfaceMeshFieldArrays", getSelectedSurfaceMeshFieldArrays() ) );
  setSelectedSurfaceMeshEnsembleArrays( reader->readValue("SelectedSurfaceMeshEnsembleArrays", getSelectedSurfaceMeshEnsembleArrays() ) );
  setSelectedSolidMeshVertexArrays( reader->readValue("SelectedSolidMeshVertexArrays", getSelectedSolidMeshVertexArrays() ) );
  setSelectedSolidMeshFaceArrays( reader->readValue("SelectedSolidMeshFaceArrays", getSelectedSolidMeshFaceArrays() ) );
  setSelectedSolidMeshEdgeArrays( reader->readValue("SelectedSolidMeshEdgeArrays", getSelectedSolidMeshEdgeArrays() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RemoveArrays::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedVoxelCellArrays", getSelectedVoxelCellArrays() );
  writer->writeValue("SelectedVoxelFieldArrays", getSelectedVoxelFieldArrays() );
  writer->writeValue("SelectedVoxelEnsembleArrays", getSelectedVoxelEnsembleArrays() );
  writer->writeValue("SelectedSurfaceMeshVertexArrays", getSelectedSurfaceMeshVertexArrays() );
  writer->writeValue("SelectedSurfaceMeshFaceArrays", getSelectedSurfaceMeshFaceArrays() );
  writer->writeValue("SelectedSurfaceMeshEdgeArrays", getSelectedSurfaceMeshEdgeArrays() );
  writer->writeValue("SelectedSurfaceMeshFieldArrays", getSelectedSurfaceMeshFieldArrays() );
  writer->writeValue("SelectedSurfaceMeshEnsembleArrays", getSelectedSurfaceMeshEnsembleArrays() );
  writer->writeValue("SelectedSolidMeshVertexArrays", getSelectedSolidMeshVertexArrays() );
  writer->writeValue("SelectedSolidMeshFaceArrays", getSelectedSolidMeshFaceArrays() );
  writer->writeValue("SelectedSolidMeshEdgeArrays", getSelectedSolidMeshEdgeArrays() );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  typedef std::set<std::string> NameList_t;

  VolumeDataContainer* m = getVolumeDataContainer();
  if (NULL != m)
  {
    for(NameList_t::iterator iter = m_SelectedVoxelCellArrays.begin(); iter != m_SelectedVoxelCellArrays.end(); ++iter)
    {
      m->removeCellData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedVoxelFieldArrays.begin(); iter != m_SelectedVoxelFieldArrays.end(); ++iter)
    {
      m->removeFieldData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedVoxelEnsembleArrays.begin(); iter != m_SelectedVoxelEnsembleArrays.end(); ++iter)
    {
      m->removeEnsembleData(*iter);
    }
  }



  SurfaceDataContainer* sm = getSurfaceDataContainer();
  if (NULL != sm)
  {
    for(NameList_t::iterator iter = m_SelectedSurfaceMeshVertexArrays.begin(); iter != m_SelectedSurfaceMeshVertexArrays.end(); ++iter)
    {
      sm->removeVertexData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedSurfaceMeshFaceArrays.begin(); iter != m_SelectedSurfaceMeshFaceArrays.end(); ++iter)
    {
      sm->removeFaceData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedSurfaceMeshEdgeArrays.begin(); iter != m_SelectedSurfaceMeshEdgeArrays.end(); ++iter)
    {
      sm->removeEdgeData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedSurfaceMeshFieldArrays.begin(); iter != m_SelectedSurfaceMeshFieldArrays.end(); ++iter)
    {
      sm->removeFieldData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedSurfaceMeshEnsembleArrays.begin(); iter != m_SelectedSurfaceMeshEnsembleArrays.end(); ++iter)
    {
      sm->removeEnsembleData(*iter);
    }
  }

  VertexDataContainer* sol = getVertexDataContainer();
  if (NULL != sol)
  {
    for(NameList_t::iterator iter = m_SelectedSolidMeshVertexArrays.begin(); iter != m_SelectedSolidMeshVertexArrays.end(); ++iter)
    {
      sol->removeVertexData(*iter);
    }
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Voxel DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  // Simply running the preflight will do what we need it to.
  dataCheck(false, 0, 0, 0);
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::setVoxelSelectedArrayNames(std::set<std::string> selectedCellArrays,
                                                     std::set<std::string> selectedFieldArrays,
                                                     std::set<std::string> selectedEnsembleArrays)
{
  m_SelectedVoxelCellArrays = selectedCellArrays;
  m_SelectedVoxelFieldArrays = selectedFieldArrays;
  m_SelectedVoxelEnsembleArrays = selectedEnsembleArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::setSurfaceMeshSelectedArrayNames(std::set<std::string> selectedVertexArrays,
                                                           std::set<std::string> selectedFaceArrays,
                                                           std::set<std::string> selectedEdgeArrays,
                                                           std::set<std::string> selectedFieldArrays,
                                                           std::set<std::string> selectedEnsembleArrays)
{
  m_SelectedSurfaceMeshVertexArrays = selectedVertexArrays;
  m_SelectedSurfaceMeshFaceArrays = selectedFaceArrays;
  m_SelectedSurfaceMeshEdgeArrays = selectedEdgeArrays;
  m_SelectedSurfaceMeshFieldArrays = selectedFieldArrays;
  m_SelectedSurfaceMeshEnsembleArrays = selectedEnsembleArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::setSolidMeshSelectedArrayNames(std::set<std::string> selectedVertexArrays,
                                                     std::set<std::string> selectedFaceArrays,
                                                     std::set<std::string> selectedEdgeArrays)
{
  m_SelectedSolidMeshVertexArrays = selectedVertexArrays;
  m_SelectedSolidMeshFaceArrays = selectedFaceArrays;
  m_SelectedSolidMeshEdgeArrays = selectedEdgeArrays;
}




