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

#include "DumpCellData.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DumpCellData::DumpCellData() :
  AbstractFilter()
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DumpCellData::~DumpCellData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DumpCellData::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
//  {
//    FilterParameter::Pointer parameter = FilterParameter::New();
//    parameter->setHumanLabel("Drop ALL Cell Data");
//    parameter->setPropertyName("DropAllData");
//    parameter->setWidgetType(FilterParameter::BooleanWidget);
//    parameter->setValueType("bool");
//    parameters.push_back(parameter);
//  }
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
void DumpCellData::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  //writer->writeValue("DropAllData", getDropAllData() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DumpCellData::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DumpCellData::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DumpCellData::execute()
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


//  std::list<std::string> nameList =  m->getCellArrayNameList();
//  for(std::list<std::string>::iterator iter = nameList.begin(); iter != nameList.end(); ++iter)
//  {
//    if ( (*iter).compare(m_GrainIdsArrayName) != 0)
//    {
//      m->removeCellData(*iter);
//    }
//  }
//  if (m_DropAllData == true)
//  {
//    m->removeCellData(m_GrainIdsArrayName);
//  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DumpCellData::setVoxelSelectedArrayNames(std::set<std::string> selectedCellArrays,
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
void DumpCellData::setSurfaceMeshSelectedArrayNames(std::set<std::string> selectedVertexArrays,
                                                     std::set<std::string> selectedFaceArrays,
                                                     std::set<std::string> selectedEdgeArrays)
{
  m_SelectedSurfaceMeshVertexArrays = selectedVertexArrays;
  m_SelectedSurfaceMeshFaceArrays = selectedFaceArrays;
  m_SelectedSurfaceMeshEdgeArrays = selectedEdgeArrays;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DumpCellData::setSolidMeshSelectedArrayNames(std::set<std::string> selectedVertexArrays,
                                                     std::set<std::string> selectedFaceArrays,
                                                     std::set<std::string> selectedEdgeArrays)
{
  m_SelectedSolidMeshVertexArrays = selectedVertexArrays;
  m_SelectedSolidMeshFaceArrays = selectedFaceArrays;
  m_SelectedSolidMeshEdgeArrays = selectedEdgeArrays;
}




