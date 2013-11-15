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
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName)
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
void ArraySelectionExample::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    addErrorMessage(getHumanLabel(), "The VolumeDataContainer Object with the specific name " + getDataContainerName() + " was not available.", getErrorCondition());
    return;
  }

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ArraySelectionExample::execute()
{
  int err = 0;
  setErrorCondition(err);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if (NULL == m)
  {
    setErrorCondition(-1);
    QString ss = QObject::tr(" DataContainer was NULL");
    notifyErrorMessage(QObject::tr("VolumeDataContainer was NULL. Returning from Execute Method for filter %1").arg(getHumanLabel()), -1);
    return;
  }

  /* Place all your code to execute your filter here. */

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
