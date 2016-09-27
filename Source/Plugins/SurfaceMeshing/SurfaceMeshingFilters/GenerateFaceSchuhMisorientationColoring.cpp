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

#include "GenerateFaceSchuhMisorientationColoring.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#endif

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Utilities/ColorTable.h"

#include "OrientationLib/SpaceGroupOps/CubicLowOps.h"
#include "OrientationLib/SpaceGroupOps/CubicOps.h"
#include "OrientationLib/SpaceGroupOps/HexagonalLowOps.h"
#include "OrientationLib/SpaceGroupOps/HexagonalOps.h"
#include "OrientationLib/SpaceGroupOps/MonoclinicOps.h"
#include "OrientationLib/SpaceGroupOps/OrthoRhombicOps.h"
#include "OrientationLib/SpaceGroupOps/TetragonalLowOps.h"
#include "OrientationLib/SpaceGroupOps/TetragonalOps.h"
#include "OrientationLib/SpaceGroupOps/TriclinicOps.h"
#include "OrientationLib/SpaceGroupOps/TrigonalLowOps.h"
#include "OrientationLib/SpaceGroupOps/TrigonalOps.h"

#include "EbsdLib/EbsdConstants.h"

/**
 * @brief The CalculateFaceSchuhMisorientationColorsImpl class
 */
class CalculateFaceSchuhMisorientationColorsImpl
{
  int32_t* m_Labels;
  int32_t* m_Phases;
  float* m_Quats;
  uint8_t* m_Colors;
  unsigned int* m_CrystalStructures;
  QVector<SpaceGroupOps::Pointer> m_OrientationOps;

public:
  CalculateFaceSchuhMisorientationColorsImpl(int32_t* labels, int32_t* phases, float* quats, uint8_t* colors, unsigned int* crystalStructures)
  : m_Labels(labels)
  , m_Phases(phases)
  , m_Quats(quats)
  , m_Colors(colors)
  , m_CrystalStructures(crystalStructures)
  {
    m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();
  }
  virtual ~CalculateFaceSchuhMisorientationColorsImpl()
  {
  }

  /**
   * @brief generate Generates the colors for the triangles
   * @param start The starting SIMPL::SurfaceMesh::Face_t Index
   * @param end The ending SIMPL::SurfaceMesh::Face_t Index
   */
  void generate(size_t start, size_t end) const
  {
    QVector<SpaceGroupOps::Pointer> ops = SpaceGroupOps::getOrientationOpsQVector();
    SIMPL::Rgb argb = 0x00000000;

    int grain1, grain2, phase1, phase2;
    QuatF q1;
    QuatF q2;
    QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

    for(size_t i = start; i < end; i++)
    {
      grain1 = m_Labels[2 * i];
      grain2 = m_Labels[2 * i + 1];
      if(grain1 > 0)
      {
        phase1 = m_Phases[grain1];
      }
      else
      {
        phase1 = 0;
      }
      if(grain2 > 0)
      {
        phase2 = m_Phases[grain2];
      }
      else
      {
        phase2 = 0;
      }

      m_Colors[3 * i + 0] = 0;
      m_Colors[3 * i + 1] = 0;
      m_Colors[3 * i + 2] = 0;

      if(phase1 > 0)
      {
        if(phase1 == phase2)
        {
          if(m_CrystalStructures[phase1] == Ebsd::CrystalStructure::Cubic_High)
          {
            QuaternionMathF::Copy(quats[grain1], q1);
            QuaternionMathF::Copy(quats[grain2], q2);
            argb = ops[m_CrystalStructures[phase1]]->generateMisorientationColor(q1, q2);
            m_Colors[3 * i] = RgbColor::dRed(argb);
            m_Colors[3 * i + 1] = RgbColor::dGreen(argb);
            m_Colors[3 * i + 2] = RgbColor::dBlue(argb);
          }
        }
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
#include "moc_GenerateFaceSchuhMisorientationColoring.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateFaceSchuhMisorientationColoring::GenerateFaceSchuhMisorientationColoring()
: SurfaceMeshFilter()
, m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_AvgQuatsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::AvgQuats)
, m_FeaturePhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases)
, m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures)
, m_SurfaceMeshFaceSchuhMisorientationColorsArrayName(SIMPL::FaceData::SurfaceMeshFaceSchuhMisorientationColors)
, m_SurfaceMeshFaceLabels(nullptr)
, m_SurfaceMeshFaceSchuhMisorientationColors(nullptr)
, m_AvgQuats(nullptr)
, m_FeaturePhases(nullptr)
, m_CrystalStructures(nullptr)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateFaceSchuhMisorientationColoring::~GenerateFaceSchuhMisorientationColoring()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceSchuhMisorientationColoring::setupFilterParameters()
{
  FilterParameterVector parameters;

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("SurfaceMeshFaceLabels", SurfaceMeshFaceLabelsArrayPath, FilterParameter::RequiredArray, GenerateFaceSchuhMisorientationColoring, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("AvgQuats", AvgQuatsArrayPath, FilterParameter::RequiredArray, GenerateFaceSchuhMisorientationColoring, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("FeaturePhases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, GenerateFaceSchuhMisorientationColoring, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("CrystalStructures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, GenerateFaceSchuhMisorientationColoring, req));
  }

  parameters.push_back(
      SIMPL_NEW_STRING_FP("SurfaceMeshFaceSchuhMisorientationColors", SurfaceMeshFaceSchuhMisorientationColorsArrayName, FilterParameter::CreatedArray, GenerateFaceSchuhMisorientationColoring));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceSchuhMisorientationColoring::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshFaceSchuhMisorientationColorsArrayName(reader->readString("SurfaceMeshFaceSchuhMisorientationColorsArrayName", getSurfaceMeshFaceSchuhMisorientationColorsArrayName()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath()));
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceSchuhMisorientationColoring::dataCheckSurfaceMesh()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0)
  {
    return;
  }

  QVector<size_t> dims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(),
                                                                                                                   dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), m_SurfaceMeshFaceLabelsArrayPath.getAttributeMatrixName(), getSurfaceMeshFaceSchuhMisorientationColorsArrayName());
  m_SurfaceMeshFaceSchuhMisorientationColorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(
      this, tempPath, 0, dims);                                             /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceSchuhMisorientationColorsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceSchuhMisorientationColors = m_SurfaceMeshFaceSchuhMisorientationColorsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceSchuhMisorientationColoring::dataCheckVoxel()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  QVector<size_t> dims(1, 4);
  m_AvgQuatsPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AvgQuatsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(),
                                                                                                           dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock().get())                                                                  /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
    // typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                    dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceSchuhMisorientationColoring::preflight()
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
void GenerateFaceSchuhMisorientationColoring::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheckSurfaceMesh();
  if(getErrorCondition() < 0)
  {
    return;
  }

  dataCheckVoxel();
  if(getErrorCondition() < 0)
  {
    return;
  }

  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), "Starting");

  // Run the data check to allocate the memory for the centroid array
  int64_t numTriangles = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  bool doParallel = true;
#endif

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if(doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, numTriangles),
                      CalculateFaceSchuhMisorientationColorsImpl(m_SurfaceMeshFaceLabels, m_FeaturePhases, m_AvgQuats, m_SurfaceMeshFaceSchuhMisorientationColors, m_CrystalStructures),
                      tbb::auto_partitioner());
  }
  else
#endif
  {
    CalculateFaceSchuhMisorientationColorsImpl serial(m_SurfaceMeshFaceLabels, m_FeaturePhases, m_AvgQuats, m_SurfaceMeshFaceSchuhMisorientationColors, m_CrystalStructures);
    serial.generate(0, numTriangles);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateFaceSchuhMisorientationColoring::newFilterInstance(bool copyFilterParameters)
{
  GenerateFaceSchuhMisorientationColoring::Pointer filter = GenerateFaceSchuhMisorientationColoring::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFaceSchuhMisorientationColoring::getCompiledLibraryName()
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFaceSchuhMisorientationColoring::getBrandingString()
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFaceSchuhMisorientationColoring::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFaceSchuhMisorientationColoring::getGroupName()
{
  return SIMPL::FilterGroups::SurfaceMeshingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFaceSchuhMisorientationColoring::getSubGroupName()
{
  return SIMPL::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateFaceSchuhMisorientationColoring::getHumanLabel()
{
  return "Generate Face Schuh Misorientation Colors";
}
