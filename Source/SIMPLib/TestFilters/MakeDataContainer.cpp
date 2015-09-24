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
*                           FA8650-10-D-5210
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "MakeDataContainer.h"

#include "SIMPLib/DataArrays/StringDataArray.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"

// Include the MOC generated file for this class
#include "moc_MakeDataContainer.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MakeDataContainer::MakeDataContainer() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::DataContainerName),
  m_CellEnsembleAttributeMatrixName(DREAM3D::Defaults::CellEnsembleAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_PhaseNameArrayName("Phase"),
  m_MaterialNameArrayName(DREAM3D::EnsembleData::MaterialName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellPhasesArrayName(DREAM3D::CellData::Phases),
  m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_LatticeConstantsArrayName(DREAM3D::EnsembleData::LatticeConstants),
  m_FeatureIds(NULL),
  m_CellPhases(NULL),
  m_CellEulerAngles(NULL),
  m_CrystalStructures(NULL),
  m_LatticeConstants(NULL)
{

  setupFilterParameters();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MakeDataContainer::~MakeDataContainer()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeDataContainer::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(StringFilterParameter::New("FeatureIds", "FeatureIdsArrayName", getFeatureIdsArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Euler Angles", "CellEulerAnglesArrayName", getCellEulerAnglesArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Phases", "CellPhasesArrayName", getCellPhasesArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Crystal Structures", "CrystalStructuresArrayName", getCrystalStructuresArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("LatticeConstants", "LatticeConstantsArrayName", getLatticeConstantsArrayName(), FilterParameter::CreatedArray));

  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeDataContainer::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MakeDataContainer::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  /*[]*/SIMPL_FILTER_WRITE_PARAMETER(LatticeConstantsArrayName)
  /*[]*/SIMPL_FILTER_WRITE_PARAMETER(CrystalStructuresArrayName)
  /*[]*/SIMPL_FILTER_WRITE_PARAMETER(CellPhasesArrayName)
  /*[]*/SIMPL_FILTER_WRITE_PARAMETER(CellEulerAnglesArrayName)
  /*[]*/SIMPL_FILTER_WRITE_PARAMETER(FeatureIdsArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeDataContainer::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if (getErrorCondition() < 0)
  {
    return;
  }
  QVector<size_t> tDims(3, 64);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if (getErrorCondition() < 0)
  {
    return;
  }
  //  tDims.resize(1);
  //  tDims[0] = 0;
  //  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellEnsemble);
  //  if(getErrorCondition() < 0)
  //  {
  //    return;
  //  }

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this,  m_FeatureIdsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);    /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  //ImageGeom::Pointer image = ImageGeom::CreateGeometry("TestImageGeom");
  //image->setResolution(0.1f, 0.2f, 0.3f);
  //image->setOrigin(100.3f, 987.234f, 0.0f);
  //image->setDimensions(64, 64, 64);
  //m->setGeometry(image);

  VertexGeom::Pointer vertices = VertexGeom::CreateGeometry(100, "TestVertexGeom");
  SharedVertexList::Pointer test = vertices->getVertices();
  float* verts = test->getPointer(0);
  for (int64_t i = 0; i < vertices->getNumberOfVertices(); i++)
  {
    verts[3 * i] = float(0.1 + i);
    verts[3 * i + 1] = float(0.2 + i);
    verts[3 * i + 2] = float(0.3 + i);
  }
  m->setGeometry(vertices);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MakeDataContainer::preflight()
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
void MakeDataContainer::execute()
{
  int err = 0;
  setErrorCondition(err);

  // Run the data check to get references to all of our data arrays initialized to the values stored in memory
  dataCheck();
  if (getErrorCondition() < 0)
  {
    return;
  }

  //ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getDataContainerName())->getGeometryAs<ImageGeom>();

  //size_t index;
  //size_t iDims[3] = {0, 0, 0};
  //image->getDimensions(iDims);

  //for (size_t z=0;z<image->getZPoints();z++)
  //{
  //  for (size_t y=0;y<image->getYPoints();y++)
  //  {
  //    for (size_t x=0;x<image->getXPoints();x++)
  //    {
  //      index = (z * iDims[0] * iDims[1]) + (y * iDims[0]) + x;
  //      m_FeatureIds[index] = z + x + y;
  //    }
  //  }
  //}

  VertexGeom::Pointer verts = getDataContainerArray()->getDataContainer(getDataContainerName())->getGeometryAs<VertexGeom>();

  for (int64_t i = 0; i < verts->getNumberOfVertices(); i++)
  {
    m_FeatureIds[i] = i;
  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MakeDataContainer::newFilterInstance(bool copyFilterParameters)
{
  MakeDataContainer::Pointer filter = MakeDataContainer::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MakeDataContainer::getCompiledLibraryName()
{
  return Test::TestBaseName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MakeDataContainer::getGroupName()
{
  return DREAM3D::FilterGroups::TestFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MakeDataContainer::getSubGroupName()
{
  return "Create Stuff";
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MakeDataContainer::getHumanLabel()
{
  return "Make DataContainer";
}

