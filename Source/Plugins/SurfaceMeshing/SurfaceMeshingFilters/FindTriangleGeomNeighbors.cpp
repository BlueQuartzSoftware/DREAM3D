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

#include "FindTriangleGeomNeighbors.h"

#include <QtCore/QDateTime>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

// Include the MOC generated file for this class
#include "moc_FindTriangleGeomNeighbors.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTriangleGeomNeighbors::FindTriangleGeomNeighbors()
: AbstractFilter()
, m_FeatureAttributeMatrixPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceFeatureAttributeMatrixName, "")
, m_NeighborListArrayName(SIMPL::FeatureData::NeighborList)
, m_FaceLabelsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_NumNeighborsArrayName(SIMPL::FeatureData::NumNeighbors)
, m_FaceLabels(nullptr)
, m_NumNeighbors(nullptr)
{
  m_NeighborList = NeighborList<int32_t>::NullPointer();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTriangleGeomNeighbors::~FindTriangleGeomNeighbors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomNeighbors::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", FaceLabelsArrayPath, FilterParameter::RequiredArray, FindTriangleGeomNeighbors, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Feature Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req =
        AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::FaceFeature, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Face Feature Attribute Matrix", FeatureAttributeMatrixPath, FilterParameter::RequiredArray, FindTriangleGeomNeighbors, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Number of Neighbors", NumNeighborsArrayName, FilterParameter::CreatedArray, FindTriangleGeomNeighbors));
  parameters.push_back(SIMPL_NEW_STRING_FP("Neighbor List", NeighborListArrayName, FilterParameter::CreatedArray, FindTriangleGeomNeighbors));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomNeighbors::initialize()
{
  m_NeighborList = NeighborList<int32_t>::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomNeighbors::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  initialize();
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getFaceLabelsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 2);
  m_FaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFaceLabelsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FaceLabelsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
	  m_FaceLabels = m_FaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getFeatureAttributeMatrixPath(), -301);

  cDims[0] = 1;

  tempPath.update(getFeatureAttributeMatrixPath().getDataContainerName(), getFeatureAttributeMatrixPath().getAttributeMatrixName(), getNumNeighborsArrayName());
  m_NumNeighborsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims);                /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_NumNeighborsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_NumNeighbors = m_NumNeighborsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
  // Do this whole block FIRST otherwise the side effect is that a call to m->getNumCellFeatureTuples will = 0
  // because we are just creating an empty NeighborList object.
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  tempPath.update(getFeatureAttributeMatrixPath().getDataContainerName(), getFeatureAttributeMatrixPath().getAttributeMatrixName(), getNeighborListArrayName());
  m_NeighborList = getDataContainerArray()->createNonPrereqArrayFromPath<NeighborList<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomNeighbors::preflight()
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
void FindTriangleGeomNeighbors::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FaceLabelsArrayPath.getDataContainerName());
  size_t totalFaces = m_FaceLabelsPtr.lock()->getNumberOfTuples();
  size_t totalFeatures = m_NumNeighborsPtr.lock()->getNumberOfTuples();

  int32_t feature1 = 0;
  int32_t feature2 = 0;
  int32_t nnum = 0;

  std::vector<std::vector<int32_t>> neighborlist;

  int32_t nListSize = 100;
  neighborlist.resize(totalFeatures);

  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;

  for(size_t i = 1; i < totalFeatures; i++)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if(currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Finding Neighbors || Initializing Neighbor Lists || %1% Complete").arg((static_cast<float>(i) / totalFeatures) * 100);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      millis = QDateTime::currentMSecsSinceEpoch();
    }

    if(getCancel() == true)
    {
      return;
    }

    m_NumNeighbors[i] = 0;
    neighborlist[i].resize(nListSize);
  }

  for(size_t j = 0; j < totalFaces; j++)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if(currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Finding Neighbors || Determining Neighbor Lists || %1% Complete").arg((static_cast<float>(j) / totalFaces) * 100);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      millis = QDateTime::currentMSecsSinceEpoch();
    }

    if(getCancel() == true)
    {
      return;
    }

	feature1 = m_FaceLabels[2 * j];
	feature2 = m_FaceLabels[2 * j + 1];
	if (feature1 > 0)
	{
		if (feature2 > 0)
		{
			nnum = m_NumNeighbors[feature1];
			neighborlist[feature1].push_back(feature2);
			nnum++;
			m_NumNeighbors[feature1] = nnum;
		}
	}
	if (feature2 > 0)
	{
		if (feature1 > 0)
		{
			nnum = m_NumNeighbors[feature2];
			neighborlist[feature2].push_back(feature1);
			nnum++;
			m_NumNeighbors[feature2] = nnum;
		}
	}
  }

  // We do this to create new set of NeighborList objects
  for(size_t i = 1; i < totalFeatures; i++)
  {
    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if(currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Finding Neighbors || Calculating Surface Areas || %1% Complete").arg(((float)i / totalFeatures) * 100);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      millis = QDateTime::currentMSecsSinceEpoch();
    }

    if(getCancel() == true)
    {
      return;
    }

    QMap<int32_t, int32_t> neighToCount;
    int32_t numneighs = static_cast<int32_t>(neighborlist[i].size());

    // this increments the voxel counts for each feature
    for(int32_t j = 0; j < numneighs; j++)
    {
      neighToCount[neighborlist[i][j]]++;
    }

    QMap<int32_t, int32_t>::Iterator neighiter = neighToCount.find(0);
    neighToCount.erase(neighiter);
    neighiter = neighToCount.find(-1);
    neighToCount.erase(neighiter);
    // Resize the features neighbor list to zero
    neighborlist[i].resize(0);

    for(QMap<int32_t, int32_t>::iterator iter = neighToCount.begin(); iter != neighToCount.end(); ++iter)
    {
      int32_t neigh = iter.key();    // get the neighbor feature

      // Push the neighbor feature id back onto the list so we stay synced up
      neighborlist[i].push_back(neigh);
    }
    m_NumNeighbors[i] = int32_t(neighborlist[i].size());

    // Set the vector for each list into the NeighborList Object
    NeighborList<int32_t>::SharedVectorType sharedNeiLst(new std::vector<int32_t>);
    sharedNeiLst->assign(neighborlist[i].begin(), neighborlist[i].end());
    m_NeighborList.lock()->setList(static_cast<int32_t>(i), sharedNeiLst);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindTriangleGeomNeighbors::newFilterInstance(bool copyFilterParameters)
{
  FindTriangleGeomNeighbors::Pointer filter = FindTriangleGeomNeighbors::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomNeighbors::getCompiledLibraryName()
{
	return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomNeighbors::getBrandingString()
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomNeighbors::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomNeighbors::getGroupName()
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomNeighbors::getSubGroupName()
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomNeighbors::getHumanLabel()
{
  return "Find Feature Neighbors from Triangle Geometry";
}
