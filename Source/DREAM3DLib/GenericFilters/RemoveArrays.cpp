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
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_SurfaceDataContainerName(DREAM3D::HDF5::SurfaceDataContainerName),
  m_EdgeDataContainerName(DREAM3D::HDF5::EdgeDataContainerName),
  m_VertexDataContainerName(DREAM3D::HDF5::VertexDataContainerName)
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
  FilterParameterVector parameters;
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
  READ_ARRAY_SELECTION_PARAMETER(VolumeVertex)
  READ_ARRAY_SELECTION_PARAMETER(VolumeEdge)
  READ_ARRAY_SELECTION_PARAMETER(VolumeFace)
  READ_ARRAY_SELECTION_PARAMETER(VolumeCell)
  READ_ARRAY_SELECTION_PARAMETER(VolumeCellField)
  READ_ARRAY_SELECTION_PARAMETER(VolumeCellEnsemble)

  READ_ARRAY_SELECTION_PARAMETER(SurfaceVertex)
  READ_ARRAY_SELECTION_PARAMETER(SurfaceEdge)
  READ_ARRAY_SELECTION_PARAMETER(SurfaceFace)
  READ_ARRAY_SELECTION_PARAMETER(SurfaceFaceField)
  READ_ARRAY_SELECTION_PARAMETER(SurfaceFaceEnsemble)

  READ_ARRAY_SELECTION_PARAMETER(EdgeVertex)
  READ_ARRAY_SELECTION_PARAMETER(EdgeEdge)
  READ_ARRAY_SELECTION_PARAMETER(EdgeEdgeField)
  READ_ARRAY_SELECTION_PARAMETER(EdgeEdgeEnsemble)

  READ_ARRAY_SELECTION_PARAMETER(VertexVertex)
  READ_ARRAY_SELECTION_PARAMETER(VertexVertexField)
  READ_ARRAY_SELECTION_PARAMETER(VertexVertexEnsemble)
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RemoveArrays::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeVertex)
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeEdge)
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeFace)
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeCell)
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeCellField)
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeCellEnsemble)

  WRITE_ARRAY_SELECTION_PARAMETER(SurfaceVertex)
  WRITE_ARRAY_SELECTION_PARAMETER(SurfaceEdge)
  WRITE_ARRAY_SELECTION_PARAMETER(SurfaceFace)
  WRITE_ARRAY_SELECTION_PARAMETER(SurfaceFaceField)
  WRITE_ARRAY_SELECTION_PARAMETER(SurfaceFaceEnsemble)

  WRITE_ARRAY_SELECTION_PARAMETER(EdgeVertex)
  WRITE_ARRAY_SELECTION_PARAMETER(EdgeEdge)
  WRITE_ARRAY_SELECTION_PARAMETER(EdgeEdgeField)
  WRITE_ARRAY_SELECTION_PARAMETER(EdgeEdgeEnsemble)

  WRITE_ARRAY_SELECTION_PARAMETER(VertexVertex)
  WRITE_ARRAY_SELECTION_PARAMETER(VertexVertexField)
  WRITE_ARRAY_SELECTION_PARAMETER(VertexVertexEnsemble)
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  typedef QSet<QString> NameList_t;

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  if (NULL != m)
  {
    for(NameList_t::iterator iter = m_SelectedVolumeCellArrays.begin(); iter != m_SelectedVolumeCellArrays.end(); ++iter)
    {
      m->removeCellData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedVolumeCellFieldArrays.begin(); iter != m_SelectedVolumeCellFieldArrays.end(); ++iter)
    {
      m->removeCellFieldData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedVolumeCellEnsembleArrays.begin(); iter != m_SelectedVolumeCellEnsembleArrays.end(); ++iter)
    {
      m->removeCellEnsembleData(*iter);
    }
  }



  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  if(NULL == sm)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  if (NULL != sm)
  {
    for(NameList_t::iterator iter = m_SelectedSurfaceVertexArrays.begin(); iter != m_SelectedSurfaceVertexArrays.end(); ++iter)
    {
      sm->removeVertexData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedSurfaceFaceArrays.begin(); iter != m_SelectedSurfaceFaceArrays.end(); ++iter)
    {
      sm->removeFaceData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedSurfaceEdgeArrays.begin(); iter != m_SelectedSurfaceEdgeArrays.end(); ++iter)
    {
      sm->removeEdgeData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedSurfaceFaceFieldArrays.begin(); iter != m_SelectedSurfaceFaceFieldArrays.end(); ++iter)
    {
      sm->removeFaceFieldData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedSurfaceFaceEnsembleArrays.begin(); iter != m_SelectedSurfaceFaceEnsembleArrays.end(); ++iter)
    {
      sm->removeFaceEnsembleData(*iter);
    }
  }

  EdgeDataContainer* e = getDataContainerArray()->getDataContainerAs<EdgeDataContainer>(getEdgeDataContainerName());
  if (NULL != e)
  {
    for(NameList_t::iterator iter = m_SelectedEdgeVertexArrays.begin(); iter != m_SelectedEdgeVertexArrays.end(); ++iter)
    {
      e->removeVertexData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedEdgeEdgeArrays.begin(); iter != m_SelectedEdgeEdgeArrays.end(); ++iter)
    {
      e->removeEdgeData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedEdgeEdgeFieldArrays.begin(); iter != m_SelectedEdgeEdgeFieldArrays.end(); ++iter)
    {
      e->removeEdgeFieldData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedEdgeEdgeEnsembleArrays.begin(); iter != m_SelectedEdgeEdgeEnsembleArrays.end(); ++iter)
    {
      e->removeEdgeEnsembleData(*iter);
    }
  }

  VertexDataContainer* v = getDataContainerArray()->getDataContainerAs<VertexDataContainer>(getVertexDataContainerName());
  if (NULL != v)
  {
    for(NameList_t::iterator iter = m_SelectedVertexVertexArrays.begin(); iter != m_SelectedVertexVertexArrays.end(); ++iter)
    {
      v->removeVertexData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedVertexVertexFieldArrays.begin(); iter != m_SelectedVertexVertexFieldArrays.end(); ++iter)
    {
      v->removeVertexFieldData(*iter);
    }
    for(NameList_t::iterator iter = m_SelectedVertexVertexEnsembleArrays.begin(); iter != m_SelectedVertexVertexEnsembleArrays.end(); ++iter)
    {
      v->removeVertexEnsembleData(*iter);
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
  QString ss;
  setErrorCondition(err);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The Volume DataContainer Object was NULL", -999);
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
void RemoveArrays::setVolumeSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                               QSet<QString> selectedEdgeArrays,
                                               QSet<QString> selectedFaceArrays,
                                               QSet<QString> selectedCellArrays,
                                               QSet<QString> selectedFieldArrays,
                                               QSet<QString> selectedEnsembleArrays)
{
  m_SelectedVolumeCellArrays = selectedVertexArrays;
  m_SelectedVolumeCellArrays = selectedEdgeArrays;
  m_SelectedVolumeCellArrays = selectedFaceArrays;
  m_SelectedVolumeCellArrays = selectedCellArrays;
  m_SelectedVolumeCellFieldArrays = selectedFieldArrays;
  m_SelectedVolumeCellEnsembleArrays = selectedEnsembleArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::setSurfaceSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                                QSet<QString> selectedEdgeArrays,
                                                QSet<QString> selectedFaceArrays,
                                                QSet<QString> selectedFieldArrays,
                                                QSet<QString> selectedEnsembleArrays)
{
  m_SelectedSurfaceVertexArrays = selectedVertexArrays;
  m_SelectedSurfaceEdgeArrays = selectedEdgeArrays;
  m_SelectedSurfaceFaceArrays = selectedFaceArrays;
  m_SelectedSurfaceFaceFieldArrays = selectedFieldArrays;
  m_SelectedSurfaceFaceEnsembleArrays = selectedEnsembleArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::setEdgeSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                             QSet<QString> selectedEdgeArrays,
                                             QSet<QString> selectedFieldArrays,
                                             QSet<QString> selectedEnsembleArrays)
{
  m_SelectedEdgeVertexArrays = selectedVertexArrays;
  m_SelectedEdgeEdgeArrays = selectedEdgeArrays;
  m_SelectedEdgeEdgeFieldArrays = selectedFieldArrays;
  m_SelectedEdgeEdgeEnsembleArrays = selectedEnsembleArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveArrays::setVertexSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                               QSet<QString> selectedFieldArrays,
                                               QSet<QString> selectedEnsembleArrays)
{
  m_SelectedVertexVertexArrays = selectedVertexArrays;
  m_SelectedVertexVertexFieldArrays = selectedFieldArrays;
  m_SelectedVertexVertexEnsembleArrays = selectedEnsembleArrays;
}




