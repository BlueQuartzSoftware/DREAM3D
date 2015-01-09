/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "NearestPointFuseRegularGrids.h"

#include <QtCore/QMap>


#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NearestPointFuseRegularGrids::NearestPointFuseRegularGrids() :
  AbstractFilter(),
  m_ReferenceCellAttributeMatrixPath("", "", ""),
  m_SamplingCellAttributeMatrixPath("", "", "")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NearestPointFuseRegularGrids::~NearestPointFuseRegularGrids()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NearestPointFuseRegularGrids::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Reference Cell Attribute Matrix", "ReferenceCellAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getReferenceCellAttributeMatrixPath(), false));
  parameters.push_back(FilterParameter::New("Sampling Cell Attribute Matrix", "SamplingCellAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getSamplingCellAttributeMatrixPath(), false));
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
void NearestPointFuseRegularGrids::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setReferenceCellAttributeMatrixPath( reader->readDataArrayPath("ReferenceCellAttributeMatrixPath", getReferenceCellAttributeMatrixPath() ) );
  setSamplingCellAttributeMatrixPath( reader->readDataArrayPath("SamplingCellAttributeMatrixPath", getSamplingCellAttributeMatrixPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NearestPointFuseRegularGrids::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceCellAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SamplingCellAttributeMatrixPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NearestPointFuseRegularGrids::dataCheck()
{
  int err = 0;
  AttributeMatrix::Pointer refAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<VolumeDataContainer, AbstractFilter>(this, m_ReferenceCellAttributeMatrixPath, err);
  AttributeMatrix::Pointer sampleAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<VolumeDataContainer, AbstractFilter>(this, m_SamplingCellAttributeMatrixPath, err);
  if(getErrorCondition() < 0) { return; }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NearestPointFuseRegularGrids::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NearestPointFuseRegularGrids::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* refDC = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getReferenceCellAttributeMatrixPath().getDataContainerName());
  VolumeDataContainer* sampleDC = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getSamplingCellAttributeMatrixPath().getDataContainerName());

  AttributeMatrix::Pointer refAttrMat = refDC->getAttributeMatrix(m_ReferenceCellAttributeMatrixPath.getAttributeMatrixName());
  AttributeMatrix::Pointer sampleAttrMat = sampleDC->getAttributeMatrix(m_ReferenceCellAttributeMatrixPath.getAttributeMatrixName());

  //get dimensions and resolutions of two grids
  size_t refDims[3];
  size_t sampleDims[3];
  float refRes[3];
  float sampleRes[3];
  float refOrigin[3];
  float sampleOrigin[3];
  refDC->getDimensions(refDims);
  sampleDC->getDimensions(sampleDims);
  refDC->getResolution(refRes);
  sampleDC->getResolution(sampleRes);
  refDC->getOrigin(refOrigin);
  sampleDC->getOrigin(sampleOrigin);

  size_t numRefTuples = refDims[0]*refDims[1]*refDims[2];

  float x, y, z;
  size_t col, row, plane;
  size_t refIndex;
  size_t sampleIndex;
  size_t planeComp, rowComp;

  //create arrays on the reference grid to hold data present on the sampling grid
  QList<QString> voxelArrayNames = sampleAttrMat->getAttributeArrayNames();
  for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
  {
    QString name = *iter;
    IDataArray::Pointer p = sampleAttrMat->getAttributeArray(*iter);
    // Make a copy of the 'p' array that has the same name. When placed into
    // the data container this will over write the current array with
    // the same name. At least in theory
    IDataArray::Pointer data = p->createNewArray(numRefTuples, p->getComponentDimensions(), p->getName());
    refAttrMat->addAttributeArray(p->getName(), data);
  }

  bool outside  = false;
  for (int i = 0; i < refDims[2]; i++)
  {
    planeComp = i*refDims[0]*refDims[1];
    for (int j = 0; j < refDims[1]; j++)
    {
      rowComp = j*refDims[0];
      for (int k = 0; k < refDims[0]; k++)
      {
        outside = false;
        x = (k * refRes[0] + refOrigin[0]);
        y = (j * refRes[1] + refOrigin[1]);
        z = (i * refRes[2] + refOrigin[2]);
        col = int((x - sampleOrigin[0]) / sampleRes[0]);
        row = int((y - sampleOrigin[1]) / sampleRes[1]);
        plane = int((z - sampleOrigin[2]) / sampleRes[2]);
        if(col < 0 || col > sampleDims[0] ||  row < 0 || row > sampleDims[1] ||  plane < 0 || plane > sampleDims[2]) outside = true;
        sampleIndex = (plane * sampleDims[0] * sampleDims[1]) + (row * sampleDims[0]) + col;
        refIndex = planeComp + rowComp + k;
        if(outside == false)
        {
          for (QList<QString>::iterator iter = voxelArrayNames.begin(); iter != voxelArrayNames.end(); ++iter)
          {
            QString name = *iter;
            IDataArray::Pointer p = sampleAttrMat->getAttributeArray(*iter);
            // Make a copy of the 'p' array that has the same name. When placed into
            // the data container this will over write the current array with
            // the same name. At least in theory
            IDataArray::Pointer data = refAttrMat->getAttributeArray(*iter);
            void* source = NULL;
            void* destination = NULL;
            int nComp = data->getNumberOfComponents();

            source = p->getVoidPointer((nComp * sampleIndex));
            destination = data->getVoidPointer((nComp * refIndex));
            ::memcpy(destination, source, p->getTypeSize() * nComp);
          }
        }
      }
    }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer NearestPointFuseRegularGrids::newFilterInstance(bool copyFilterParameters)
{
  NearestPointFuseRegularGrids::Pointer filter = NearestPointFuseRegularGrids::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NearestPointFuseRegularGrids::getCompiledLibraryName()
{ return Sampling::SamplingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NearestPointFuseRegularGrids::getGroupName()
{ return DREAM3D::FilterGroups::SamplingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString NearestPointFuseRegularGrids::getHumanLabel()
{ return "Fuse Regular Grid Datasets"; }

