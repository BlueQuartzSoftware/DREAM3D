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


#include "BadDataNeighborOrientationCheck.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"

// Include the MOC generated file for this class
#include "moc_BadDataNeighborOrientationCheck.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BadDataNeighborOrientationCheck::BadDataNeighborOrientationCheck() :
  AbstractFilter(),
  m_MisorientationTolerance(5.0f),
  m_NumberOfNeighbors(6),
  m_GoodVoxelsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Mask),
  m_CellPhasesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_CrystalStructuresArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_QuatsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Quats),
  m_Quats(NULL),
  m_GoodVoxels(NULL),
  m_CellPhases(NULL),
  m_CrystalStructures(NULL)
{
  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BadDataNeighborOrientationCheck::~BadDataNeighborOrientationCheck()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(DoubleFilterParameter::New("Misorientation Tolerance (Degrees)", "MisorientationTolerance", getMisorientationTolerance(), FilterParameter::Parameter));
  parameters.push_back(IntFilterParameter::New("Required Number of Neighbors", "NumberOfNeighbors", getNumberOfNeighbors(), FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Float, 4, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Quaternions", "QuatsArrayPath", getQuatsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Bool, 1, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Mask", "GoodVoxelsArrayPath", getGoodVoxelsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "CellPhasesArrayPath", getCellPhasesArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::UInt32, 1, DREAM3D::AttributeMatrixType::CellEnsemble, DREAM3D::GeometryType::ImageGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setQuatsArrayPath(reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setMisorientationTolerance( reader->readValue("MisorientationTolerance", getMisorientationTolerance()) );
  setNumberOfNeighbors( reader->readValue("NumberOfNeighbors", getNumberOfNeighbors()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BadDataNeighborOrientationCheck::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(QuatsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(GoodVoxelsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(MisorientationTolerance)
  SIMPL_FILTER_WRITE_PARAMETER(NumberOfNeighbors)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::dataCheck()
{
  setErrorCondition(0);

  QVector<DataArrayPath> dataArrayPaths;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getGoodVoxelsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getGoodVoxelsArrayPath()); }

  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getCellPhasesArrayPath()); }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this,  getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 4;
  m_QuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_QuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats = m_QuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getCellPhasesArrayPath()); }

  getDataContainerArray()->validateNumberOfTuples(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BadDataNeighborOrientationCheck::preflight()
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
void BadDataNeighborOrientationCheck::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_GoodVoxelsArrayPath.getDataContainerName());
  size_t totalPoints = m_GoodVoxelsPtr.lock()->getNumberOfTuples();

  m_MisorientationTolerance = m_MisorientationTolerance * SIMPLib::Constants::k_Pi / 180.0;

  size_t udims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  int32_t good = 1;
  DimType neighbor = 0;
  DimType column = 0, row = 0, plane = 0;

  DimType neighpoints[6] = { 0, 0, 0, 0, 0, 0 };
  neighpoints[0] = static_cast<DimType>(-dims[0] * dims[1]);
  neighpoints[1] = static_cast<DimType>(-dims[0]);
  neighpoints[2] = static_cast<DimType>(-1);
  neighpoints[3] = static_cast<DimType>(1);
  neighpoints[4] = static_cast<DimType>(dims[0]);
  neighpoints[5] = static_cast<DimType>(dims[0] * dims[1]);

  float w = 10000.0f;
  QuatF q1 = QuaternionMathF::New();
  QuatF q2 = QuaternionMathF::New();
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  uint32_t phase1 = 0, phase2 = 0;

  QVector<int32_t> neighborCount(totalPoints, 0);

  for (size_t i = 0; i < totalPoints; i++)
  {
    if (m_GoodVoxels[i] == false)
    {
      column = i % dims[0];
      row = (i / dims[0]) % dims[1];
      plane = i / (dims[0] * dims[1]);
      for (int32_t j = 0; j < 6; j++)
      {
        good = 1;
        neighbor = i + neighpoints[j];
        if (j == 0 && plane == 0) { good = 0; }
        if (j == 5 && plane == (dims[2] - 1)) { good = 0; }
        if (j == 1 && row == 0) { good = 0; }
        if (j == 4 && row == (dims[1] - 1)) { good = 0; }
        if (j == 2 && column == 0) { good = 0; }
        if (j == 3 && column == (dims[0] - 1)) { good = 0; }
        if (good == 1 && m_GoodVoxels[neighbor] == true)
        {
          phase1 = m_CrystalStructures[m_CellPhases[i]];
          QuaternionMathF::Copy(quats[i], q1);

          phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
          QuaternionMathF::Copy(quats[neighbor], q2);

          if (m_CellPhases[i] == m_CellPhases[neighbor] && m_CellPhases[i] > 0) { w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3); }
          if (w < m_MisorientationTolerance)
          {
            neighborCount[i]++;
          }
        }
      }
    }
  }

  int32_t currentLevel = 6;
  int32_t counter = 0;

  while (currentLevel > m_NumberOfNeighbors)
  {
    counter = 1;
    while (counter > 0)
    {
      counter = 0;
      for (size_t i = 0; i < totalPoints; i++)
      {
        if (neighborCount[i] >= currentLevel && m_GoodVoxels[i] == false)
        {
          m_GoodVoxels[i] = true;
          counter++;
          column = i % dims[0];
          row = (i / dims[0]) % dims[1];
          plane = i / (dims[0] * dims[1]);
          for (DimType j = 0; j < 6; j++)
          {
            good = 1;
            neighbor = i + neighpoints[j];
            if (j == 0 && plane == 0) { good = 0; }
            if (j == 5 && plane == (dims[2] - 1)) { good = 0; }
            if (j == 1 && row == 0) { good = 0; }
            if (j == 4 && row == (dims[1] - 1)) { good = 0; }
            if (j == 2 && column == 0) { good = 0; }
            if (j == 3 && column == (dims[0] - 1)) { good = 0; }
            if (good == 1 && m_GoodVoxels[neighbor] == false)
            {
              phase1 = m_CrystalStructures[m_CellPhases[i]];
              QuaternionMathF::Copy(quats[i], q1);

              phase2 = m_CrystalStructures[m_CellPhases[neighbor]];
              QuaternionMathF::Copy(quats[neighbor], q2);

              if (m_CellPhases[i] == m_CellPhases[neighbor] && m_CellPhases[i] > 0) { w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3); }
              if (w < m_MisorientationTolerance)
              {
                neighborCount[neighbor]++;
              }
            }
          }
        }
      }
    }
    currentLevel = currentLevel - 1;
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer BadDataNeighborOrientationCheck::newFilterInstance(bool copyFilterParameters)
{
  BadDataNeighborOrientationCheck::Pointer filter = BadDataNeighborOrientationCheck::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString BadDataNeighborOrientationCheck::getCompiledLibraryName()
{ return OrientationAnalysisConstants::OrientationAnalysisBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString BadDataNeighborOrientationCheck::getGroupName()
{ return DREAM3D::FilterGroups::ProcessingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString BadDataNeighborOrientationCheck::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CleanupFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString BadDataNeighborOrientationCheck::getHumanLabel()
{ return "Neighbor Orientation Comparison (Bad Data)"; }
