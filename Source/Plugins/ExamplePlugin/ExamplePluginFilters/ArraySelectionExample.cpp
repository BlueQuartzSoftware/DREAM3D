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
  AbstractFilter()
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
  setSelectedVolumeCellArrays( reader->readValue("SelectedVolumeCellArrays", getSelectedVolumeCellArrays() ) );
  setSelectedVolumeCellFeatureArrays( reader->readValue("SelectedVolumeCellFeatureArrays", getSelectedVolumeCellFeatureArrays() ) );
  setSelectedVolumeCellEnsembleArrays( reader->readValue("SelectedVolumeCellEnsembleArrays", getSelectedVolumeCellEnsembleArrays() ) );
  setSelectedSurfaceVertexArrays( reader->readValue("SelectedSurfaceVertexArrays", getSelectedSurfaceVertexArrays() ) );
  setSelectedSurfaceFaceArrays( reader->readValue("SelectedSurfaceFaceArrays", getSelectedSurfaceFaceArrays() ) );
  setSelectedSurfaceEdgeArrays( reader->readValue("SelectedSurfaceEdgeArrays", getSelectedSurfaceEdgeArrays() ) );
  setSelectedSurfaceFaceFeatureArrays( reader->readValue("SelectedSurfaceFaceFeatureArrays", getSelectedSurfaceFaceFeatureArrays() ) );
  setSelectedSurfaceFaceEnsembleArrays( reader->readValue("SelectedSurfaceFaceEnsembleArrays", getSelectedSurfaceFaceEnsembleArrays() ) );
  setSelectedVertexVertexArrays( reader->readValue("SelectedVertexVertexArrays", getSelectedVertexVertexArrays() ) );
  setSelectedVertexVertexFeatureArrays( reader->readValue("SelectedVertexVertexFeatureArrays", getSelectedVertexVertexFeatureArrays() ) );
  setSelectedVertexVertexEnsembleArrays( reader->readValue("SelectedVertexVertexEnsembleArrays", getSelectedVertexVertexEnsembleArrays() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ArraySelectionExample::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedVolumeCellArrays", getSelectedVolumeCellArrays() );
  writer->writeValue("SelectedVolumeCellFeatureArrays", getSelectedVolumeCellFeatureArrays() );
  writer->writeValue("SelectedVolumeCellEnsembleArrays", getSelectedVolumeCellEnsembleArrays() );
  writer->writeValue("SelectedSurfaceVertexArrays", getSelectedSurfaceVertexArrays() );
  writer->writeValue("SelectedSurfaceFaceArrays", getSelectedSurfaceFaceArrays() );
  writer->writeValue("SelectedSurfaceEdgeArrays", getSelectedSurfaceEdgeArrays() );
  writer->writeValue("SelectedSurfaceFaceFeatureArrays", getSelectedSurfaceFaceFeatureArrays() );
  writer->writeValue("SelectedSurfaceFaceEnsembleArrays", getSelectedSurfaceFaceEnsembleArrays() );
  writer->writeValue("SelectedVertexVertexArrays", getSelectedVertexVertexArrays() );
  writer->writeValue("SelectedVertexVertexFeatureArrays", getSelectedVertexVertexFeatureArrays() );
  writer->writeValue("SelectedVertexVertexEnsembleArrays", getSelectedVertexVertexEnsembleArrays() );
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
  VolumeDataContainer* m = getVolumeDataContainer();
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
