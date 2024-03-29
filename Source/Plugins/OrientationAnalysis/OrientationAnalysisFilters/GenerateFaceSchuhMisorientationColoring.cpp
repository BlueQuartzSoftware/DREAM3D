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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "GenerateFaceSchuhMisorientationColoring.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#endif

#include <QtCore/QTextStream>

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Utilities/ColorTable.h"

#include "EbsdLib/Core/EbsdLibConstants.h"
#include "EbsdLib/LaueOps/CubicLowOps.h"
#include "EbsdLib/LaueOps/CubicOps.h"
#include "EbsdLib/LaueOps/HexagonalLowOps.h"
#include "EbsdLib/LaueOps/HexagonalOps.h"
#include "EbsdLib/LaueOps/MonoclinicOps.h"
#include "EbsdLib/LaueOps/OrthoRhombicOps.h"
#include "EbsdLib/LaueOps/TetragonalLowOps.h"
#include "EbsdLib/LaueOps/TetragonalOps.h"
#include "EbsdLib/LaueOps/TriclinicOps.h"
#include "EbsdLib/LaueOps/TrigonalLowOps.h"
#include "EbsdLib/LaueOps/TrigonalOps.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

using LaueOpsShPtrType = std::shared_ptr<LaueOps>;
using LaueOpsContainer = std::vector<LaueOpsShPtrType>;

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
  LaueOpsContainer m_OrientationOps;

public:
  CalculateFaceSchuhMisorientationColorsImpl(int32_t* labels, int32_t* phases, float* quats, uint8_t* colors, unsigned int* crystalStructures)
  : m_Labels(labels)
  , m_Phases(phases)
  , m_Quats(quats)
  , m_Colors(colors)
  , m_CrystalStructures(crystalStructures)
  {
    m_OrientationOps = LaueOps::GetAllOrientationOps();
  }
  virtual ~CalculateFaceSchuhMisorientationColorsImpl() = default;

  /**
   * @brief generate Generates the colors for the triangles
   * @param start The starting SIMPL::SurfaceMesh::Face_t Index
   * @param end The ending SIMPL::SurfaceMesh::Face_t Index
   */
  void generate(size_t start, size_t end) const
  {
    std::vector<LaueOps::Pointer> ops = LaueOps::GetAllOrientationOps();
    SIMPL::Rgb argb = 0x00000000;

    int grain1, grain2, phase1, phase2;

    // QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);

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
          if(m_CrystalStructures[phase1] == EbsdLib::CrystalStructure::Cubic_High)
          {
            float* quatPtr = m_Quats + i * grain1;
            QuatD q1(quatPtr[0], quatPtr[1], quatPtr[2], quatPtr[3]);
            quatPtr = m_Quats + i * grain2;
            QuatD q2(quatPtr[0], quatPtr[1], quatPtr[2], quatPtr[3]);

            argb = ops[m_CrystalStructures[phase1]]->generateMisorientationColor(q1, q2);
            m_Colors[3 * i] = RgbColor::dRed(argb);
            m_Colors[3 * i + 1] = RgbColor::dGreen(argb);
            m_Colors[3 * i + 2] = RgbColor::dBlue(argb);
          }
        }
      }
    }
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateFaceSchuhMisorientationColoring::GenerateFaceSchuhMisorientationColoring() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateFaceSchuhMisorientationColoring::~GenerateFaceSchuhMisorientationColoring() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceSchuhMisorientationColoring::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("SurfaceMeshFaceLabels", SurfaceMeshFaceLabelsArrayPath, FilterParameter::Category::RequiredArray, GenerateFaceSchuhMisorientationColoring, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("AvgQuats", AvgQuatsArrayPath, FilterParameter::Category::RequiredArray, GenerateFaceSchuhMisorientationColoring, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("FeaturePhases", FeaturePhasesArrayPath, FilterParameter::Category::RequiredArray, GenerateFaceSchuhMisorientationColoring, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("CrystalStructures", CrystalStructuresArrayPath, FilterParameter::Category::RequiredArray, GenerateFaceSchuhMisorientationColoring, req));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("SurfaceMeshFaceSchuhMisorientationColors", SurfaceMeshFaceSchuhMisorientationColorsArrayName, FilterParameter::Category::CreatedArray,
                                           GenerateFaceSchuhMisorientationColoring));

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
  clearErrorCode();
  clearWarningCode();

  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer(this, m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), false);
  if(getErrorCode() < 0)
  {
    return;
  }

  std::vector<size_t> dims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getSurfaceMeshFaceLabelsArrayPath(), dims);
  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock())
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 3;
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), m_SurfaceMeshFaceLabelsArrayPath.getAttributeMatrixName(), getSurfaceMeshFaceSchuhMisorientationColorsArrayName());
  m_SurfaceMeshFaceSchuhMisorientationColorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>>(this, tempPath, 0, dims);
  if(nullptr != m_SurfaceMeshFaceSchuhMisorientationColorsPtr.lock())
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
  clearErrorCode();
  clearWarningCode();

  std::vector<size_t> dims(1, 4);
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>>(this, getAvgQuatsArrayPath(), dims);
  if(nullptr != m_AvgQuatsPtr.lock())
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getFeaturePhasesArrayPath(), dims);
  if(nullptr != m_FeaturePhasesPtr.lock())
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  // typedef DataArray<unsigned int> XTalStructArrayType;
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>>(this, getCrystalStructuresArrayPath(), dims);
  if(nullptr != m_CrystalStructuresPtr.lock())
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceSchuhMisorientationColoring::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  dataCheckSurfaceMesh();
  dataCheckVoxel();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFaceSchuhMisorientationColoring::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheckSurfaceMesh();
  if(getErrorCode() < 0)
  {
    return;
  }

  dataCheckVoxel();
  if(getErrorCode() < 0)
  {
    return;
  }

  notifyStatusMessage("Starting");

  // Run the data check to allocate the memory for the centroid array
  int64_t numTriangles = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(true)
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
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateFaceSchuhMisorientationColoring::newFilterInstance(bool copyFilterParameters) const
{
  GenerateFaceSchuhMisorientationColoring::Pointer filter = GenerateFaceSchuhMisorientationColoring::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateFaceSchuhMisorientationColoring::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateFaceSchuhMisorientationColoring::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateFaceSchuhMisorientationColoring::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateFaceSchuhMisorientationColoring::getGroupName() const
{
  return SIMPL::FilterGroups::ProcessingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid GenerateFaceSchuhMisorientationColoring::getUuid() const
{
  return QUuid("{0850a882-ca6f-52c6-aa0b-885359b84c32}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateFaceSchuhMisorientationColoring::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateFaceSchuhMisorientationColoring::getHumanLabel() const
{
  return "Generate Face Schuh Misorientation Colors";
}

// -----------------------------------------------------------------------------
GenerateFaceSchuhMisorientationColoring::Pointer GenerateFaceSchuhMisorientationColoring::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<GenerateFaceSchuhMisorientationColoring> GenerateFaceSchuhMisorientationColoring::New()
{
  struct make_shared_enabler : public GenerateFaceSchuhMisorientationColoring
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString GenerateFaceSchuhMisorientationColoring::getNameOfClass() const
{
  return QString("GenerateFaceSchuhMisorientationColoring");
}

// -----------------------------------------------------------------------------
QString GenerateFaceSchuhMisorientationColoring::ClassName()
{
  return QString("GenerateFaceSchuhMisorientationColoring");
}

// -----------------------------------------------------------------------------
void GenerateFaceSchuhMisorientationColoring::setSurfaceMeshFaceLabelsArrayPath(const DataArrayPath& value)
{
  m_SurfaceMeshFaceLabelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateFaceSchuhMisorientationColoring::getSurfaceMeshFaceLabelsArrayPath() const
{
  return m_SurfaceMeshFaceLabelsArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateFaceSchuhMisorientationColoring::setAvgQuatsArrayPath(const DataArrayPath& value)
{
  m_AvgQuatsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateFaceSchuhMisorientationColoring::getAvgQuatsArrayPath() const
{
  return m_AvgQuatsArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateFaceSchuhMisorientationColoring::setFeaturePhasesArrayPath(const DataArrayPath& value)
{
  m_FeaturePhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateFaceSchuhMisorientationColoring::getFeaturePhasesArrayPath() const
{
  return m_FeaturePhasesArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateFaceSchuhMisorientationColoring::setCrystalStructuresArrayPath(const DataArrayPath& value)
{
  m_CrystalStructuresArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateFaceSchuhMisorientationColoring::getCrystalStructuresArrayPath() const
{
  return m_CrystalStructuresArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateFaceSchuhMisorientationColoring::setSurfaceMeshFaceSchuhMisorientationColorsArrayName(const QString& value)
{
  m_SurfaceMeshFaceSchuhMisorientationColorsArrayName = value;
}

// -----------------------------------------------------------------------------
QString GenerateFaceSchuhMisorientationColoring::getSurfaceMeshFaceSchuhMisorientationColorsArrayName() const
{
  return m_SurfaceMeshFaceSchuhMisorientationColorsArrayName;
}
