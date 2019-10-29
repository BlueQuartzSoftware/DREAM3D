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

#include "FindBoundaryCells.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "Generic/GenericConstants.h"
#include "Generic/GenericVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundaryCells::FindBoundaryCells()
: m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_BoundaryCellsArrayName(SIMPL::CellData::BoundaryCells)
, m_IgnoreFeatureZero(true)
, m_IncludeVolumeBoundary(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindBoundaryCells::~FindBoundaryCells() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryCells::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_BOOL_FP("Ignore Feature 0", IgnoreFeatureZero, FilterParameter::Parameter, FindBoundaryCells));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Include Volume Boundary", IncludeVolumeBoundary, FilterParameter::Parameter, FindBoundaryCells));

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, FindBoundaryCells, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Boundary Cells", BoundaryCellsArrayName, FeatureIdsArrayPath, FeatureIdsArrayPath, FilterParameter::CreatedArray, FindBoundaryCells));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryCells::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setBoundaryCellsArrayName(reader->readString("BoundaryCellsArrayName", getBoundaryCellsArrayName()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryCells::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryCells::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  std::vector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }

  tempPath.update(getFeatureIdsArrayPath().getDataContainerName(), getFeatureIdsArrayPath().getAttributeMatrixName(), getBoundaryCellsArrayName());
  m_BoundaryCellsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter, int8_t>(
      this, tempPath, 0, cDims);
  if(nullptr != m_BoundaryCellsPtr.lock())
  {
    m_BoundaryCells = m_BoundaryCellsPtr.lock()->getPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindBoundaryCells::preflight()
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
void FindBoundaryCells::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  int64_t xPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints());
  int64_t yPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getYPoints());
  int64_t zPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getZPoints());

  int64_t neighpoints[6] = {0, 0, 0, 0, 0, 0};
  neighpoints[0] = -1 * (xPoints * yPoints);
  neighpoints[1] = -1 * (xPoints);
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xPoints;
  neighpoints[5] = xPoints * yPoints;

  int32_t feature = 0;
  int32_t onsurf = 0;
  int32_t good = 0;
  int64_t neighbor = 0;

  int ignoreFeatureZeroVal = 0;
  if(!m_IgnoreFeatureZero)
  {
    ignoreFeatureZeroVal = -1;
  }

  int64_t zStride = 0, yStride = 0;
  for(int64_t i = 0; i < zPoints; i++)
  {
    zStride = i * xPoints * yPoints;
    for(int64_t j = 0; j < yPoints; j++)
    {
      yStride = j * xPoints;
      for(int64_t k = 0; k < xPoints; k++)
      {
        onsurf = 0;
        feature = m_FeatureIds[zStride + yStride + k];
        if(feature >= 0)
        {
          if(m_IncludeVolumeBoundary)
          {
            if (xPoints > 2 && (k == 0 || k == xPoints - 1)) { onsurf++; }
            if (yPoints > 2 && (j == 0 || j == yPoints - 1)) { onsurf++; }
            if (zPoints > 2 && (i == 0 || i == zPoints - 1)) { onsurf++; }

            if(onsurf > 0 && feature == 0) { onsurf = 0; }
          }

          for(int64_t l = 0; l < 6; l++)
          {
            good = 1;
            neighbor = zStride + yStride + k + neighpoints[l];
            if(l == 0 && i == 0)
            {
              good = 0;
            }
            if(l == 5 && i == (zPoints - 1))
            {
              good = 0;
            }
            if(l == 1 && j == 0)
            {
              good = 0;
            }
            if(l == 4 && j == (yPoints - 1))
            {
              good = 0;
            }
            if(l == 2 && k == 0)
            {
              good = 0;
            }
            if(l == 3 && k == (xPoints - 1))
            {
              good = 0;
            }
            if(good == 1 && m_FeatureIds[neighbor] != feature && m_FeatureIds[neighbor] > ignoreFeatureZeroVal)
            {
              onsurf++;
            }
          }
        }
        m_BoundaryCells[zStride + yStride + k] = onsurf;
      }
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindBoundaryCells::newFilterInstance(bool copyFilterParameters) const
{
  FindBoundaryCells::Pointer filter = FindBoundaryCells::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBoundaryCells::getCompiledLibraryName() const
{
  return GenericConstants::GenericBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBoundaryCells::getBrandingString() const
{
  return "Generic";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBoundaryCells::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Generic::Version::Major() << "." << Generic::Version::Minor() << "." << Generic::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBoundaryCells::getGroupName() const
{
  return SIMPL::FilterGroups::Generic;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindBoundaryCells::getUuid() const
{
  return QUuid("{8a1106d4-c67f-5e09-a02a-b2e9b99d031e}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBoundaryCells::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::SpatialFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindBoundaryCells::getHumanLabel() const
{
  return "Find Boundary Cells (Image)";
}

// -----------------------------------------------------------------------------
FindBoundaryCells::Pointer FindBoundaryCells::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindBoundaryCells> FindBoundaryCells::New()
{
  struct make_shared_enabler : public FindBoundaryCells
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindBoundaryCells::getNameOfClass() const
{
  return QString("FindBoundaryCells");
}

// -----------------------------------------------------------------------------
QString FindBoundaryCells::ClassName()
{
  return QString("FindBoundaryCells");
}

// -----------------------------------------------------------------------------
void FindBoundaryCells::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindBoundaryCells::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void FindBoundaryCells::setBoundaryCellsArrayName(const QString& value)
{
  m_BoundaryCellsArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindBoundaryCells::getBoundaryCellsArrayName() const
{
  return m_BoundaryCellsArrayName;
}

// -----------------------------------------------------------------------------
void FindBoundaryCells::setIgnoreFeatureZero(bool value)
{
  m_IgnoreFeatureZero = value;
}

// -----------------------------------------------------------------------------
bool FindBoundaryCells::getIgnoreFeatureZero() const
{
  return m_IgnoreFeatureZero;
}

// -----------------------------------------------------------------------------
void FindBoundaryCells::setIncludeVolumeBoundary(bool value)
{
  m_IncludeVolumeBoundary = value;
}

// -----------------------------------------------------------------------------
bool FindBoundaryCells::getIncludeVolumeBoundary() const
{
  return m_IncludeVolumeBoundary;
}
