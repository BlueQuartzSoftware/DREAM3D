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

#include "GenerateFaceMisorientationColoring.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#endif

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "OrientationLib/SpaceGroupOps/CubicLowOps.h"
#include "OrientationLib/SpaceGroupOps/CubicOps.h"
#include "OrientationLib/SpaceGroupOps/HexagonalOps.h"
#include "OrientationLib/SpaceGroupOps/MonoclinicOps.h"
#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"
#include "OrientationLib/SpaceGroupOps/OrthoRhombicOps.h"
#include "OrientationLib/SpaceGroupOps/TetragonalOps.h"
#include "OrientationLib/SpaceGroupOps/TrigonalOps.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

#include "EbsdLib/EbsdConstants.h"


/**
 * @brief The CalculateFaceMisorientationColorsImpl class implements a threaded algorithm that computes the misorientation
 * colors for the given list of surface mesh labels
 */
class CalculateFaceMisorientationColorsImpl
{
    int32_t* m_Labels;
    int32_t* m_Phases;
    float* m_Quats;
    float* m_Colors;
    unsigned int* m_CrystalStructures;
    QVector<SpaceGroupOps::Pointer> m_OrientationOps;

  public:
    CalculateFaceMisorientationColorsImpl(int32_t* labels, int32_t* phases, float* quats, float* colors, unsigned int* crystalStructures) :
      m_Labels(labels),
      m_Phases(phases),
      m_Quats(quats),
      m_Colors(colors),
      m_CrystalStructures(crystalStructures)
    {
      m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();
    }
    virtual ~CalculateFaceMisorientationColorsImpl() {}

    void generate(size_t start, size_t end) const
    {
      int32_t feature1 = 0, feature2 = 0, phase1 = 0, phase2 = 0;
      QuatF q1 = QuaternionMathF::New();
      QuatF q2 = QuaternionMathF::New();
      QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

      float w = 0.0f, n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
      float radToDeg = 180.0f / SIMPLib::Constants::k_Pi;
      for (size_t i = start; i < end; i++)
      {
        feature1 = m_Labels[2 * i];
        feature2 = m_Labels[2 * i + 1];
        if (feature1 > 0) { phase1 = m_Phases[feature1]; }
        else { phase1 = 0; }
        if (feature2 > 0) { phase2 = m_Phases[feature2]; }
        else { phase2 = 0; }
        if (phase1 > 0)
        {
          if (phase1 == phase2 && m_CrystalStructures[phase1] == Ebsd::CrystalStructure::Cubic_High)
          {
            QuaternionMathF::Copy(quats[feature1], q1);
            QuaternionMathF::Copy(quats[feature2], q2);
            w = m_OrientationOps[m_CrystalStructures[phase1]]->getMisoQuat(q1, q2, n1, n2, n3);
            w = w * radToDeg;
            m_Colors[3 * i + 0] = w * n1;
            m_Colors[3 * i + 1] = w * n2;
            m_Colors[3 * i + 2] = w * n3;
          }
          else if (phase1 == phase2 && m_CrystalStructures[phase1] == Ebsd::CrystalStructure::Hexagonal_High)
          {
            QuaternionMathF::Copy(quats[feature1], q1);
            QuaternionMathF::Copy(quats[feature2], q2);
            w = m_OrientationOps[m_CrystalStructures[phase1]]->getMisoQuat(q1, q2, n1, n2, n3);
            w = w * radToDeg;
            m_Colors[3 * i + 0] = w * n1;
            m_Colors[3 * i + 1] = w * n2;
            m_Colors[3 * i + 2] = w * n3;
          }
        }
        else
        {
          m_Colors[3 * i + 0] = 0;
          m_Colors[3 * i + 1] = 0;
          m_Colors[3 * i + 2] = 0;
        }
      }
    }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    /**
     * @brief operator () This is called from the TBB stye of code
     * @param r The range to compute the values
     */
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      generate(r.begin(), r.end());
    }
#endif
};

// Include the MOC generated file for this class
#include "moc_GenerateFaceMisorientationColoring.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateFaceMisorientationColoring::GenerateFaceMisorientationColoring() :
  SurfaceMeshFilter(),
  m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels),
  m_AvgQuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::AvgQuats),
  m_FeaturePhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases),
  m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures),
  m_SurfaceMeshFaceMisorientationColorsArrayName(SIMPL::FaceData::SurfaceMeshFaceMisorientationColors),
  m_SurfaceMeshFaceLabels(NULL),
  m_AvgQuats(NULL),
  m_FeaturePhases(NULL),
  m_CrystalStructures(NULL),
  m_SurfaceMeshFaceMisorientationColors(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateFaceMisorientationColoring::~GenerateFaceMisorientationColoring()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceMisorientationColoring::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, SIMPL::AttributeMatrixType::Face, SIMPL::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Face Labels", "SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(GenerateFaceMisorientationColoring, this, SurfaceMeshFaceLabelsArrayPath), SIMPL_BIND_GETTER(GenerateFaceMisorientationColoring, this, SurfaceMeshFaceLabelsArrayPath)));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 4, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);

    parameters.push_back(DataArraySelectionFilterParameter::New("Average Quaternions", "AvgQuatsArrayPath", getAvgQuatsArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(GenerateFaceMisorientationColoring, this, AvgQuatsArrayPath), SIMPL_BIND_GETTER(GenerateFaceMisorientationColoring, this, AvgQuatsArrayPath)));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);

    parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "FeaturePhasesArrayPath", getFeaturePhasesArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(GenerateFaceMisorientationColoring, this, FeaturePhasesArrayPath), SIMPL_BIND_GETTER(GenerateFaceMisorientationColoring, this, FeaturePhasesArrayPath)));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, SIMPL::AttributeMatrixType::CellEnsemble, SIMPL::GeometryType::ImageGeometry);

    parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req, SIMPL_BIND_SETTER(GenerateFaceMisorientationColoring, this, CrystalStructuresArrayPath), SIMPL_BIND_GETTER(GenerateFaceMisorientationColoring, this, CrystalStructuresArrayPath)));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Misorientation Colors", "SurfaceMeshFaceMisorientationColorsArrayName", getSurfaceMeshFaceMisorientationColorsArrayName(), FilterParameter::CreatedArray, SIMPL_BIND_SETTER(GenerateFaceMisorientationColoring, this, SurfaceMeshFaceMisorientationColorsArrayName), SIMPL_BIND_GETTER(GenerateFaceMisorientationColoring, this, SurfaceMeshFaceMisorientationColorsArrayName)));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceMisorientationColoring::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshFaceMisorientationColorsArrayName(reader->readString("SurfaceMeshFaceMisorientationColorsArrayName", getSurfaceMeshFaceMisorientationColorsArrayName() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceMisorientationColoring::dataCheckSurfaceMesh()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName());

  QVector<IDataArray::Pointer> dataArrays;

  if(getErrorCondition() >= 0) { dataArrays.push_back(triangles->getTriangles()); }

  QVector<size_t> cDims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrays.push_back(m_SurfaceMeshFaceLabelsPtr.lock()); }

  cDims[0] = 3;
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), m_SurfaceMeshFaceLabelsArrayPath.getAttributeMatrixName(), getSurfaceMeshFaceMisorientationColorsArrayName() );
  m_SurfaceMeshFaceMisorientationColorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceMisorientationColorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceMisorientationColors = m_SurfaceMeshFaceMisorientationColorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrays);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceMisorientationColoring::dataCheckVoxel()
{
  setErrorCondition(0);

  QVector<DataArrayPath> dataArrayPaths;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getAvgQuatsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 4);
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_AvgQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getAvgQuatsArrayPath()); }

  cDims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getFeaturePhasesArrayPath()); }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceMisorientationColoring::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheckSurfaceMesh();
  dataCheckVoxel();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceMisorientationColoring::execute()
{
  setErrorCondition(0);
  dataCheckSurfaceMesh();
  if(getErrorCondition() < 0) { return; }
  dataCheckVoxel();
  if(getErrorCondition() < 0) { return; }

  int64_t numTriangles = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  bool doParallel = true;
#endif

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numTriangles),
                      CalculateFaceMisorientationColorsImpl(m_SurfaceMeshFaceLabels, m_FeaturePhases, m_AvgQuats, m_SurfaceMeshFaceMisorientationColors, m_CrystalStructures), tbb::auto_partitioner());
  }
  else
#endif
  {
    CalculateFaceMisorientationColorsImpl serial(m_SurfaceMeshFaceLabels, m_FeaturePhases, m_AvgQuats, m_SurfaceMeshFaceMisorientationColors, m_CrystalStructures);
    serial.generate(0, numTriangles);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateFaceMisorientationColoring::newFilterInstance(bool copyFilterParameters)
{
  GenerateFaceMisorientationColoring::Pointer filter = GenerateFaceMisorientationColoring::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFaceMisorientationColoring::getCompiledLibraryName()
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFaceMisorientationColoring::getBrandingString()
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFaceMisorientationColoring::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFaceMisorientationColoring::getGroupName()
{ return SIMPL::FilterGroups::ProcessingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFaceMisorientationColoring::getSubGroupName()
{ return SIMPL::FilterSubGroups::CrystallographyFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFaceMisorientationColoring::getHumanLabel()
{ return "Generate Misorientation Colors (Face)"; }
