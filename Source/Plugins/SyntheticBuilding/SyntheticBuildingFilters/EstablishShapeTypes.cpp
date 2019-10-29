/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include <memory>

#include "EstablishShapeTypes.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/ShapeTypeSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"
#include "SyntheticBuilding/SyntheticBuildingVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EstablishShapeTypes::EstablishShapeTypes()
: m_InputPhaseTypesArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::PhaseTypes)
, m_ShapeTypesArrayName(SIMPL::EnsembleData::ShapeTypes)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EstablishShapeTypes::~EstablishShapeTypes() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishShapeTypes::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Any);
    IGeometry::Types geomTypes;
    geomTypes.push_back(IGeometry::Type::Image);
    geomTypes.push_back(IGeometry::Type::Unknown);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phase Types", InputPhaseTypesArrayPath, FilterParameter::RequiredArray, EstablishShapeTypes, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Shape Types", ShapeTypesArrayName, InputPhaseTypesArrayPath, InputPhaseTypesArrayPath, FilterParameter::CreatedArray, EstablishShapeTypes));
  ShapeTypeSelectionFilterParameter::Pointer sType_parameter =
      SIMPL_NEW_SHAPETYPE_SELECTION_FP("Shape Types", ShapeTypeData, FilterParameter::CreatedArray, EstablishShapeTypes, "PhaseCount", "InputPhaseTypesArrayPath");
  parameters.push_back(sType_parameter);
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishShapeTypes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputPhaseTypesArrayPath(reader->readDataArrayPath("InputPhaseTypesArrayPath", getInputPhaseTypesArrayPath()));
  setShapeTypesArrayName(reader->readString("ShapeTypesArrayName", getShapeTypesArrayName()));
  ShapeType::Types data = getShapeTypeData();
  // Convert to QVector<unsigned int>
  QVector<ShapeType::EnumType> vec = ShapeType::ToQVector(data);
  // Read from the source
  vec = reader->readArray("ShapeTypeData", vec);
  // convert back to enum type
  data = ShapeType::FromQVector(vec);
  setShapeTypeData(data);
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishShapeTypes::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EstablishShapeTypes::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  DataContainerArray::Pointer dca = getDataContainerArray();

  std::vector<size_t> cDims(1, 1);
  m_PhaseTypesPtr = dca->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getInputPhaseTypesArrayPath(), cDims);
  if(nullptr != m_PhaseTypesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Get the DataContainer first - same as phase types
  DataContainer::Pointer m = dca->getPrereqDataContainer(this, getInputPhaseTypesArrayPath().getDataContainerName());
  if(getErrorCode() < 0)
  {
    return;
  }

  // Now get the AttributeMatrix that the user wants to use to store the ShapeTypes array - same as phase types
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getPrereqAttributeMatrix(this, getInputPhaseTypesArrayPath().getAttributeMatrixName(), -990);
  if(getErrorCode() < 0 || nullptr == cellEnsembleAttrMat.get())
  {
    return;
  }
  // Now create the output Shape Types Array
  tempPath.update(getInputPhaseTypesArrayPath().getDataContainerName(), getInputPhaseTypesArrayPath().getAttributeMatrixName(), getShapeTypesArrayName());
  m_ShapeTypesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt32ArrayType, AbstractFilter>(this, tempPath, true, cDims);           /* Assigns the shared_ptr<>(this, tempPath, true, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_ShapeTypesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_ShapeTypes = m_ShapeTypesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
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
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  // Copy the data from the internal QVector into the actual ShapeTypes array from the data container
  for(int32_t i = 0; i < m_ShapeTypeData.size(); i++)
  {
    m_ShapeTypesPtr.lock()->setValue(i, static_cast<ShapeType::EnumType>(m_ShapeTypeData[i]));
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EstablishShapeTypes::getPhaseCount()
{
  DataContainerArray::Pointer dca = getDataContainerArray();

  AttributeMatrix::Pointer inputAttrMat = dca->getAttributeMatrix(getInputPhaseTypesArrayPath());

  if(nullptr == inputAttrMat.get())
  {
    return 0;
  }

  if(inputAttrMat->getType() < AttributeMatrix::Type::VertexEnsemble || inputAttrMat->getType() > AttributeMatrix::Type::CellEnsemble)
  {
    return 0;
  }

  std::vector<size_t> tupleDims = inputAttrMat->getTupleDimensions();
  size_t phaseCount = 1;
  for(int32_t i = 0; i < tupleDims.size(); i++)
  {
    phaseCount = phaseCount * tupleDims[i];
  }
  return phaseCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer EstablishShapeTypes::newFilterInstance(bool copyFilterParameters) const
{
  EstablishShapeTypes::Pointer filter = EstablishShapeTypes::New();
  if(copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters());
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EstablishShapeTypes::getCompiledLibraryName() const
{
  return SyntheticBuildingConstants::SyntheticBuildingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EstablishShapeTypes::getBrandingString() const
{
  return "SyntheticBuilding";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EstablishShapeTypes::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SyntheticBuilding::Version::Major() << "." << SyntheticBuilding::Version::Minor() << "." << SyntheticBuilding::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EstablishShapeTypes::getGroupName() const
{
  return SIMPL::FilterGroups::SyntheticBuildingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid EstablishShapeTypes::getUuid() const
{
  return QUuid("{4edbbd35-a96b-5ff1-984a-153d733e2abb}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EstablishShapeTypes::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EstablishShapeTypes::getHumanLabel() const
{
  return "Establish Shape Types";
}

// -----------------------------------------------------------------------------
EstablishShapeTypes::Pointer EstablishShapeTypes::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<EstablishShapeTypes> EstablishShapeTypes::New()
{
  struct make_shared_enabler : public EstablishShapeTypes
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString EstablishShapeTypes::getNameOfClass() const
{
  return QString("EstablishShapeTypes");
}

// -----------------------------------------------------------------------------
QString EstablishShapeTypes::ClassName()
{
  return QString("EstablishShapeTypes");
}

// -----------------------------------------------------------------------------
void EstablishShapeTypes::setInputPhaseTypesArrayPath(const DataArrayPath& value)
{
  m_InputPhaseTypesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath EstablishShapeTypes::getInputPhaseTypesArrayPath() const
{
  return m_InputPhaseTypesArrayPath;
}

// -----------------------------------------------------------------------------
void EstablishShapeTypes::setShapeTypesArrayName(const QString& value)
{
  m_ShapeTypesArrayName = value;
}

// -----------------------------------------------------------------------------
QString EstablishShapeTypes::getShapeTypesArrayName() const
{
  return m_ShapeTypesArrayName;
}

// -----------------------------------------------------------------------------
void EstablishShapeTypes::setShapeTypeData(const ShapeType::Types& value)
{
  m_ShapeTypeData = value;
}

// -----------------------------------------------------------------------------
ShapeType::Types EstablishShapeTypes::getShapeTypeData() const
{
  return m_ShapeTypeData;
}
