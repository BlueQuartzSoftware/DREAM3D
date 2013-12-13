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
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName)
//  m_SelectedVolumeVertexArrays(FIX_ME<<<<<<<<),
//  m_SelectedVolumeEdgeArrays(FIX_ME<<<<<<<<),
//  m_SelectedVolumeFaceArrays(FIX_ME<<<<<<<<),
//  m_SelectedVolumeCellArrays(FIX_ME<<<<<<<<),
//  m_SelectedVolumeCellFeatureArrays(FIX_ME<<<<<<<<),
//  m_SelectedVolumeCellEnsembleArrays(FIX_ME<<<<<<<<),
//  m_SelectedSurfaceVertexArrays(FIX_ME<<<<<<<<),
//  m_SelectedSurfaceEdgeArrays(FIX_ME<<<<<<<<),
//  m_SelectedSurfaceFaceArrays(FIX_ME<<<<<<<<),
//  m_SelectedSurfaceFaceFeatureArrays(FIX_ME<<<<<<<<),
//  m_SelectedSurfaceFaceEnsembleArrays(FIX_ME<<<<<<<<),
//  m_SelectedEdgeVertexArrays(FIX_ME<<<<<<<<),
//  m_SelectedEdgeEdgeArrays(FIX_ME<<<<<<<<),
//  m_SelectedEdgeEdgeFeatureArrays(FIX_ME<<<<<<<<),
//  m_SelectedEdgeEdgeEnsembleArrays(FIX_ME<<<<<<<<),
//  m_SelectedVertexVertexArrays(FIX_ME<<<<<<<<),
//  m_SelectedVertexVertexFeatureArrays(FIX_ME<<<<<<<<),
//  m_SelectedVertexVertexEnsembleArrays(FIX_ME<<<<<<<<)
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
    option->setHumanLabel("Arrays to Remember");
    option->setPropertyName("ArraysToRemember");
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
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/

  reader->openFilterGroup(this, index);
  READ_ARRAY_SELECTION_PARAMETER(VolumeVertex)
  READ_ARRAY_SELECTION_PARAMETER(VolumeEdge)
  READ_ARRAY_SELECTION_PARAMETER(VolumeFace)
  READ_ARRAY_SELECTION_PARAMETER(VolumeCell)
  READ_ARRAY_SELECTION_PARAMETER(VolumeCellFeature)
  READ_ARRAY_SELECTION_PARAMETER(VolumeCellEnsemble)

  READ_ARRAY_SELECTION_PARAMETER(SurfaceVertex)
  READ_ARRAY_SELECTION_PARAMETER(SurfaceEdge)
  READ_ARRAY_SELECTION_PARAMETER(SurfaceFace)
  READ_ARRAY_SELECTION_PARAMETER(SurfaceFaceFeature)
  READ_ARRAY_SELECTION_PARAMETER(SurfaceFaceEnsemble)

  READ_ARRAY_SELECTION_PARAMETER(EdgeVertex)
  READ_ARRAY_SELECTION_PARAMETER(EdgeEdge)
  READ_ARRAY_SELECTION_PARAMETER(EdgeEdgeFeature)
  READ_ARRAY_SELECTION_PARAMETER(EdgeEdgeEnsemble)

  READ_ARRAY_SELECTION_PARAMETER(VertexVertex)
  READ_ARRAY_SELECTION_PARAMETER(VertexVertexFeature)
  READ_ARRAY_SELECTION_PARAMETER(VertexVertexEnsemble)
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ArraySelectionExample::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeVertex)
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeEdge)
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeFace)
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeCell)
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeCellFeature)
  WRITE_ARRAY_SELECTION_PARAMETER(VolumeCellEnsemble)

  WRITE_ARRAY_SELECTION_PARAMETER(SurfaceVertex)
  WRITE_ARRAY_SELECTION_PARAMETER(SurfaceEdge)
  WRITE_ARRAY_SELECTION_PARAMETER(SurfaceFace)
  WRITE_ARRAY_SELECTION_PARAMETER(SurfaceFaceFeature)
  WRITE_ARRAY_SELECTION_PARAMETER(SurfaceFaceEnsemble)

  WRITE_ARRAY_SELECTION_PARAMETER(EdgeVertex)
  WRITE_ARRAY_SELECTION_PARAMETER(EdgeEdge)
  WRITE_ARRAY_SELECTION_PARAMETER(EdgeEdgeFeature)
  WRITE_ARRAY_SELECTION_PARAMETER(EdgeEdgeEnsemble)

  WRITE_ARRAY_SELECTION_PARAMETER(VertexVertex)
  WRITE_ARRAY_SELECTION_PARAMETER(VertexVertexFeature)
  WRITE_ARRAY_SELECTION_PARAMETER(VertexVertexEnsemble)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::setVolumeSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                                        QSet<QString> selectedEdgeArrays,
                                                        QSet<QString> selectedFaceArrays,
                                                        QSet<QString> selectedCellArrays,
                                                        QSet<QString> selectedFeatureArrays,
                                                        QSet<QString> selectedEnsembleArrays)
{
  m_SelectedVolumeVertexArrays = selectedVertexArrays;
  m_SelectedVolumeEdgeArrays = selectedEdgeArrays;
  m_SelectedVolumeFaceArrays = selectedFaceArrays;
  m_SelectedVolumeCellArrays = selectedCellArrays;
  m_SelectedVolumeCellFeatureArrays = selectedFeatureArrays;
  m_SelectedVolumeCellEnsembleArrays = selectedEnsembleArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::setSurfaceSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                                         QSet<QString> selectedEdgeArrays,
                                                         QSet<QString> selectedFaceArrays,
                                                         QSet<QString> selectedFeatureArrays,
                                                         QSet<QString> selectedEnsembleArrays)
{
  m_SelectedSurfaceVertexArrays = selectedVertexArrays;
  m_SelectedSurfaceEdgeArrays = selectedEdgeArrays;
  m_SelectedSurfaceFaceArrays = selectedFaceArrays;
  m_SelectedSurfaceFaceFeatureArrays = selectedFeatureArrays;
  m_SelectedSurfaceFaceEnsembleArrays = selectedEnsembleArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::setEdgeSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                                      QSet<QString> selectedEdgeArrays,
                                                      QSet<QString> selectedFeatureArrays,
                                                      QSet<QString> selectedEnsembleArrays)
{
  m_SelectedEdgeVertexArrays = selectedVertexArrays;
  m_SelectedEdgeEdgeArrays = selectedEdgeArrays;
  m_SelectedEdgeEdgeFeatureArrays = selectedFeatureArrays;
  m_SelectedEdgeEdgeEnsembleArrays = selectedEnsembleArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::setVertexSelectedArrayNames(QSet<QString> selectedVertexArrays,
                                                        QSet<QString> selectedFeatureArrays,
                                                        QSet<QString> selectedEnsembleArrays)
{
  m_SelectedVertexVertexArrays = selectedVertexArrays;
  m_SelectedVertexVertexFeatureArrays = selectedFeatureArrays;
  m_SelectedVertexVertexEnsembleArrays = selectedEnsembleArrays;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::preflight()
{
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::execute()
{
  int err = 0;
  setErrorCondition(err);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

  /* Place all your code to execute your filter here. */

  /* Let the GUI know we are done with this filter */
  emit filterGeneratedMessage(PipelineMessage::CreateStatusMessage(getHumanLabel(), "Complete") );
}

