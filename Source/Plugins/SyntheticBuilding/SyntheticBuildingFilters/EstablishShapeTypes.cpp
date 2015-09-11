/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "EstablishShapeTypes.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/ShapeTypeSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EstablishShapeTypes::EstablishShapeTypes() :
  AbstractFilter(),
  m_InputPhaseTypesArrayPath(DREAM3D::Defaults::StatsGenerator, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::PhaseTypes),
  m_ShapeTypesArrayName(DREAM3D::EnsembleData::ShapeTypes),
  m_PhaseTypes(NULL),
  m_ShapeTypes(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EstablishShapeTypes::~EstablishShapeTypes()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishShapeTypes::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::UInt32, 1, DREAM3D::AttributeMatrixType::CellEnsemble, DREAM3D::Defaults::AnyGeometry);
    QVector<uint32_t> geomTypes;
    geomTypes.push_back(DREAM3D::GeometryType::ImageGeometry);
    geomTypes.push_back(DREAM3D::GeometryType::UnknownGeometry);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(DataArraySelectionFilterParameter::New("Phase Types", "InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Shape Types", "ShapeTypesArrayName", getShapeTypesArrayName(), FilterParameter::CreatedArray));
  ShapeTypeSelectionFilterParameter::Pointer sType_parameter = ShapeTypeSelectionFilterParameter::New( "Shape Types", "ShapeTypeData", "UInt32Vector_t", "PhaseCount", "InputPhaseTypesArrayPath", FilterParameter::CreatedArray);
  parameters.push_back(sType_parameter);
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishShapeTypes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputPhaseTypesArrayPath(reader->readDataArrayPath("InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath() ) );
  setShapeTypesArrayName(reader->readString("ShapeTypesArrayName", getShapeTypesArrayName() ) );
  QVector<uint32_t> data = getShapeTypeData().d;
  data = reader->readArray("ShapeTypeData", data);
  UInt32Vector_t vec;
  vec.d = data;
  setShapeTypeData(vec);
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EstablishShapeTypes::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(InputPhaseTypesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(ShapeTypesArrayName)
  writer->writeValue("ShapeTypeData", getShapeTypeData().d );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishShapeTypes::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  DataContainerArray::Pointer dca = getDataContainerArray();

  QVector<size_t> cDims(1, 1);
  m_PhaseTypesPtr = dca->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getInputPhaseTypesArrayPath(), cDims);
  if( NULL != m_PhaseTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Get the DataContainer first - same as phase types
  DataContainer::Pointer m = dca->getPrereqDataContainer<AbstractFilter>(this, getInputPhaseTypesArrayPath().getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  // Now get the AttributeMatrix that the user wants to use to store the ShapeTypes array - same as phase types
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getInputPhaseTypesArrayPath().getAttributeMatrixName(), -990);
  if(getErrorCondition() < 0 || NULL == cellEnsembleAttrMat.get()) { return; }
  // Now create the output Shape Types Array
  tempPath.update(getInputPhaseTypesArrayPath().getDataContainerName(), getInputPhaseTypesArrayPath().getAttributeMatrixName(), getShapeTypesArrayName() );
  m_ShapeTypesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt32ArrayType, AbstractFilter>(this, tempPath, true, cDims); /* Assigns the shared_ptr<>(this, tempPath, true, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ShapeTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ShapeTypes = m_ShapeTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishShapeTypes::preflight()
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
void EstablishShapeTypes::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  // Copy the data from the internal QVector into the actual ShapeTypes array from the data container
  for(int32_t i = 0; i < m_ShapeTypeData.d.size(); i++)
  {
    m_ShapeTypesPtr.lock()->setValue(i, m_ShapeTypeData.d[i]);
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EstablishShapeTypes::getPhaseCount()
{
  DataContainerArray::Pointer dca = getDataContainerArray();

  AttributeMatrix::Pointer inputAttrMat = dca->getAttributeMatrix(getInputPhaseTypesArrayPath());

  if (NULL == inputAttrMat.get() ) { return 0; }

  if (__SHOW_DEBUG_MSG__)
  {
    qDebug() << "  data->getNumberOfTuples(): " << inputAttrMat->getTupleDimensions();
    qDebug() << "Name" << inputAttrMat->getName();
  }

  if (inputAttrMat->getType() < DREAM3D::AttributeMatrixType::VertexEnsemble
      || inputAttrMat->getType() > DREAM3D::AttributeMatrixType::CellEnsemble )
  {
    return 0;
  }

  QVector<size_t> tupleDims = inputAttrMat->getTupleDimensions();
  size_t phaseCount = 1;
  for (int32_t i = 0; i < tupleDims.size(); i++)
  {
    phaseCount = phaseCount * tupleDims[i];
  }
  return phaseCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer EstablishShapeTypes::newFilterInstance(bool copyFilterParameters)
{
  EstablishShapeTypes::Pointer filter = EstablishShapeTypes::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters());
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EstablishShapeTypes::getCompiledLibraryName()
{ return SyntheticBuildingConstants::SyntheticBuildingBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EstablishShapeTypes::getGroupName()
{return DREAM3D::FilterGroups::SyntheticBuildingFilters;}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EstablishShapeTypes::getSubGroupName()
{ return DREAM3D::FilterSubGroups::GenerationFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString EstablishShapeTypes::getHumanLabel()
{return "Establish Shape Types";}
