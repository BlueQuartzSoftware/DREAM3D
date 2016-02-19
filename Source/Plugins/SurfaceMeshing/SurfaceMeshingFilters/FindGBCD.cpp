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

#include "FindGBCD.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include <QtCore/QDateTime>

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Utilities/TimeUtilities.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "OrientationLib/OrientationMath/OrientationArray.hpp"
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"
#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"

/**
 * @brief The CalculateGBCDImpl class implements a threaded algorithm that calculates the
 * grain boundary character distribution (GBCD) for a surface mesh
 */
class CalculateGBCDImpl
{
    size_t startOffset;
    size_t numEntriesPerTri;
    Int32ArrayType::Pointer m_LabelsArray;
    DoubleArrayType::Pointer m_NormalsArray;
    Int32ArrayType::Pointer m_PhasesArray;
    FloatArrayType::Pointer m_EulersArray;

    FloatArrayType::Pointer m_GbcdDeltasArray;
    FloatArrayType::Pointer m_GbcdLimitsArray;
    Int32ArrayType::Pointer m_GbcdSizesArray;
    Int32ArrayType::Pointer m_GbcdBinsArray;
    BoolArrayType::Pointer  m_GbcdHemiCheckArray;

    UInt32ArrayType::Pointer m_CrystalStructuresArray;
    QVector<SpaceGroupOps::Pointer> m_OrientationOps;

  public:
    CalculateGBCDImpl(size_t i, size_t numMisoReps, Int32ArrayType::Pointer Labels, DoubleArrayType::Pointer Normals, FloatArrayType::Pointer Eulers,
                      Int32ArrayType::Pointer Phases, UInt32ArrayType::Pointer CrystalStructures,
                      Int32ArrayType::Pointer Bins, BoolArrayType::Pointer HemiCheck,
                      FloatArrayType::Pointer GBCDdeltas, Int32ArrayType::Pointer  GBCDsizes,
                      FloatArrayType::Pointer GBCDlimits) :
      startOffset(i),
      numEntriesPerTri(numMisoReps),
      m_LabelsArray(Labels),
      m_NormalsArray(Normals),
      m_PhasesArray(Phases),
      m_EulersArray(Eulers),
      m_GbcdDeltasArray(GBCDdeltas),
      m_GbcdLimitsArray(GBCDlimits),
      m_GbcdSizesArray(GBCDsizes),
      m_GbcdBinsArray(Bins),
      m_GbcdHemiCheckArray(HemiCheck),
      m_CrystalStructuresArray(CrystalStructures)
    {
      m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();
    }
    virtual ~CalculateGBCDImpl() {}

    void generate(size_t start, size_t end) const
    {

      // We want to work with the raw pointers for speed so get those pointers.
      float* m_GBCDdeltas = m_GbcdDeltasArray->getPointer(0);
      float* m_GBCDlimits = m_GbcdLimitsArray->getPointer(0);
      int* m_GBCDsizes = m_GbcdSizesArray->getPointer(0);
      int32_t* m_Bins = m_GbcdBinsArray->getPointer(0);
      bool* m_HemiCheck = m_GbcdHemiCheckArray->getPointer(0);

      int32_t* m_Labels = m_LabelsArray->getPointer(0);
      double* m_Normals = m_NormalsArray->getPointer(0);
      int32_t* m_Phases = m_PhasesArray->getPointer(0);
      float* m_Eulers = m_EulersArray->getPointer(0);
      uint32_t* m_CrystalStructures = m_CrystalStructuresArray->getPointer(0);


      int32_t j = 0;//, j4;
      int32_t k = 0;//, k4;
      int32_t m = 0;
      int32_t temp = 0;
      //bool gbcd_indices_good;
      int32_t feature1 = 0, feature2 = 0;
      int32_t inversion = 1;
      float g1ea[3] = { 0.0f, 0.0f, 0.0f }, g2ea[3] = { 0.0f, 0.0f, 0.0f };
      float g1[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, g2[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
      float g1s[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, g2s[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
      float sym1[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, sym2[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
      float g2t[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, dg[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
      float euler_mis[3] = { 0.0f, 0.0f, 0.0f };
      float normal[3] = { 0.0f, 0.0f, 0.0f };
      float xstl1_norm1[3] = { 0.0f, 0.0f, 0.0f };
      int32_t gbcd_index = 0;
      float sqCoord[2] = { 0.0f, 0.0f }, sqCoordInv[2] = { 0.0f, 0.0f };
      bool nhCheck = false, nhCheckInv = true;
      int32_t SYMcounter = 0;
      int64_t TRIcounter = static_cast<int64_t>(start - startOffset);
      int64_t TRIcounterShift = 0;

      for (size_t i = start; i < end; i++)
      {
        SYMcounter = 0;
        feature1 = m_Labels[2 * i];
        feature2 = m_Labels[2 * i + 1];
        normal[0] = m_Normals[3 * i];
        normal[1] = m_Normals[3 * i + 1];
        normal[2] = m_Normals[3 * i + 2];

        if (feature1 < 0 || feature2 < 0) { continue; }

        if (m_Phases[feature1] == m_Phases[feature2] && m_Phases[feature1] > 0)
        {
          TRIcounterShift = (TRIcounter * numEntriesPerTri);
          uint32_t cryst = m_CrystalStructures[m_Phases[feature1]];
          for (int32_t q = 0; q < 2; q++)
          {
            if (q == 1)
            {
              temp = feature1;
              feature1 = feature2;
              feature2 = temp;
              normal[0] = -normal[0];
              normal[1] = -normal[1];
              normal[2] = -normal[2];
            }
            for (m = 0; m < 3; m++)
            {
              g1ea[m] = m_Eulers[3 * feature1 + m];
              g2ea[m] = m_Eulers[3 * feature2 + m];
            }

            FOrientArrayType om(9, 0.0f);
            FOrientTransformsType::eu2om(FOrientArrayType(g1ea, 3), om);
            om.toGMatrix(g1);

            FOrientTransformsType::eu2om(FOrientArrayType(g2ea, 3), om);
            om.toGMatrix(g2);

            int32_t nsym = m_OrientationOps[cryst]->getNumSymOps();
            for (j = 0; j < nsym; j++)
            {
              // rotate g1 by symOp
              m_OrientationOps[cryst]->getMatSymOp(j, sym1);
              MatrixMath::Multiply3x3with3x3(sym1, g1, g1s);
              // get the crystal directions along the triangle normals
              MatrixMath::Multiply3x3with3x1(g1s, normal, xstl1_norm1);
              // get coordinates in square projection of crystal normal parallel to boundary normal
              nhCheck = getSquareCoord(xstl1_norm1, sqCoord);
              if (inversion == 1)
              {
                sqCoordInv[0] = -sqCoord[0];
                sqCoordInv[1] = -sqCoord[1];
                if (nhCheck == false) { nhCheckInv = true; }
                else { nhCheckInv = false; }
              }

              for (k = 0; k < nsym; k++)
              {
                // calculate the symmetric misorienation
                m_OrientationOps[cryst]->getMatSymOp(k, sym2);
                // rotate g2 by symOp
                MatrixMath::Multiply3x3with3x3(sym2, g2, g2s);
                // transpose rotated g2
                MatrixMath::Transpose3x3(g2s, g2t);
                // calculate delta g
                MatrixMath::Multiply3x3with3x3(g1s, g2t, dg);
                // translate matrix to euler angles
                FOrientArrayType om(dg);

                FOrientArrayType eu(euler_mis, 3);
                FOrientTransformsType::om2eu(om, eu);

                if (euler_mis[0] < SIMPLib::Constants::k_PiOver2 && euler_mis[1] < SIMPLib::Constants::k_PiOver2 && euler_mis[2] < SIMPLib::Constants::k_PiOver2)
                {
                  // PHI euler angle is stored in GBCD as cos(PHI)
                  euler_mis[1] = cosf(euler_mis[1]);
                  //get the indexes that this point would be in the GBCD histogram
                  gbcd_index = GBCDIndex(m_GBCDdeltas, m_GBCDsizes, m_GBCDlimits, euler_mis, sqCoord);
                  if (gbcd_index != -1)
                  {
                    m_HemiCheck[TRIcounterShift + SYMcounter] = nhCheck;
                    m_Bins[TRIcounterShift + SYMcounter] = gbcd_index;
                  }
                  SYMcounter++;
                  if (inversion == 1)
                  {
                    gbcd_index = GBCDIndex(m_GBCDdeltas, m_GBCDsizes, m_GBCDlimits, euler_mis, sqCoordInv);
                    if (gbcd_index != -1)
                    {
                      m_HemiCheck[TRIcounterShift + SYMcounter] = nhCheckInv;
                      m_Bins[TRIcounterShift + SYMcounter] = gbcd_index;
                    }
                    SYMcounter++;
                  }
                }
                else { SYMcounter += 2; }
              }
            }
          }
        }
        TRIcounter++;
      }
    }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      generate(r.begin(), r.end());
    }
#endif

    int32_t GBCDIndex(float* gbcddelta, int32_t* gbcdsz, float* gbcdlimits, float* eulerN, float* sqCoord) const
    {
      int32_t gbcd_index;
      int32_t i = 0, index[5] = { 0, 0, 0, 0, 0 };
      int32_t n1 = 0, n1n2 = 0, n1n2n3 = 0, n1n2n3n4 = 0;
      int32_t flag_good = 1;
      float mis_eulerNorm[5] = { 0, 0, 0, 0, 0 };

      // concatonate the normalized euler angles and normalized spherical corrdinate normal
      for (i = 0; i < 3; i++) { mis_eulerNorm[i] = eulerN[i]; }
      for (i = 0; i < 2; i++) { mis_eulerNorm[i + 3] = sqCoord[i]; }
      // Check for a valid point in the GBCD space
      for (i = 0; i < 5; i++)
      {
        if (mis_eulerNorm[i] < gbcdlimits[i]) { flag_good = 0; }
        if (mis_eulerNorm[i] > gbcdlimits[i + 5]) { flag_good = 0; }
      }

      if (flag_good == 0) { return -1; } //does not fit in the gbcd space

      n1 =  gbcdsz[0];
      n1n2 = n1 * ( gbcdsz[1]);
      n1n2n3 = n1n2 * ( gbcdsz[2]);
      n1n2n3n4 = n1n2n3 * ( gbcdsz[3]);

      // determine the bin that the point should go into.
      for (i = 0; i < 5; i++)
      {
        index[i] = (int32_t)((mis_eulerNorm[i] - gbcdlimits[i]) / gbcddelta[i]);
        if (index[i] >  (gbcdsz[i] - 1))
        {
          index[i] =  (gbcdsz[i] - 1);
        }
        if (index[i] <  0)
        {
          index[i] =  0;
        }
      }

      gbcd_index = index[0] + n1 * index[1] + n1n2 * index[2] + n1n2n3 * index[3] + n1n2n3n4 * index[4];

      return gbcd_index;
    }

    bool getSquareCoord(float* xstl1_norm1, float* sqCoord) const
    {
      bool nhCheck = false;
      float adjust = 1.0;
      if (xstl1_norm1[2] >= 0.0)
      {
        adjust = -1.0;
        nhCheck = true;
      }
      if (fabsf(xstl1_norm1[0]) >= fabsf(xstl1_norm1[1]))
      {
        sqCoord[0] = (xstl1_norm1[0] / fabsf(xstl1_norm1[0])) * sqrtf(2.0f * 1.0f * (1.0f + (xstl1_norm1[2] * adjust))) * (SIMPLib::Constants::k_SqrtPi / 2.0f);
        sqCoord[1] = (xstl1_norm1[0] / fabsf(xstl1_norm1[0])) * sqrtf(2.0f * 1.0f * (1.0f + (xstl1_norm1[2] * adjust))) * ((2.0f / SIMPLib::Constants::k_SqrtPi) * atanf(xstl1_norm1[1] / xstl1_norm1[0]));
      }
      else
      {
        sqCoord[0] = (xstl1_norm1[1] / fabsf(xstl1_norm1[1])) * sqrtf(2.0 * 1.0 * (1.0 + (xstl1_norm1[2] * adjust))) * ((2.0f / SIMPLib::Constants::k_SqrtPi) * atanf(xstl1_norm1[0] / xstl1_norm1[1]));
        sqCoord[1] = (xstl1_norm1[1] / fabsf(xstl1_norm1[1])) * sqrtf(2.0 * 1.0 * (1.0 + (xstl1_norm1[2] * adjust))) * (SIMPLib::Constants::k_SqrtPi / 2.0f);
      }
      return nhCheck;
    }
};

// Include the MOC generated file for this class
#include "moc_FindGBCD.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGBCD::FindGBCD() :
  SurfaceMeshFilter(),
  m_FaceEnsembleAttributeMatrixName(SIMPL::Defaults::FaceEnsembleAttributeMatrixName),
  m_GBCDRes(9.0f),
  m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceNormalsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshFaceAreasArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceAreas),
  m_FeatureEulerAnglesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::EulerAngles),
  m_FeaturePhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases),
  m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures),
  m_GBCDArrayName(SIMPL::EnsembleData::GBCD),
  m_SurfaceMeshFaceAreas(NULL),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFaceNormals(NULL),
  m_FeatureEulerAngles(NULL),
  m_FeaturePhases(NULL),
  m_CrystalStructures(NULL),
  m_GBCD(NULL),
  m_GbcdDeltas(NULL),
  m_GbcdSizes(NULL),
  m_GbcdLimits(NULL),
  m_GbcdBins(NULL),
  m_HemiCheck(NULL)
{
  m_GbcdDeltasArray = FloatArrayType::NullPointer();
  m_GbcdSizesArray = Int32ArrayType::NullPointer();
  m_GbcdLimitsArray = FloatArrayType::NullPointer();
  m_GbcdBinsArray = Int32ArrayType::NullPointer();
  m_GbcdHemiCheckArray = BoolArrayType::NullPointer();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGBCD::~FindGBCD()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(DoubleFilterParameter::New("GBCD Resolution (Degrees)", "GBCDRes", getGBCDRes(), FilterParameter::Parameter));
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, SIMPL::AttributeMatrixType::Face, SIMPL::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Face Labels", "SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Double, 3, SIMPL::AttributeMatrixType::Face, SIMPL::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Face Normals", "SurfaceMeshFaceNormalsArrayPath", getSurfaceMeshFaceNormalsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Double, 1, SIMPL::AttributeMatrixType::Face, SIMPL::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Face Areas", "SurfaceMeshFaceAreasArrayPath", getSurfaceMeshFaceAreasArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);

    parameters.push_back(DataArraySelectionFilterParameter::New("Average Euler Angles", "FeatureEulerAnglesArrayPath", getFeatureEulerAnglesArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);

    parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "FeaturePhasesArrayPath", getFeaturePhasesArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, SIMPL::AttributeMatrixType::CellEnsemble, SIMPL::GeometryType::ImageGeometry);

    parameters.push_back(DataArraySelectionFilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath", getCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Face Ensemble Attribute Matrix", "FaceEnsembleAttributeMatrixName", getFaceEnsembleAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("GBCD", "GBCDArrayName", getGBCDArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setGBCDArrayName(reader->readString("GBCDArrayName", getGBCDArrayName() ) );
  setFaceEnsembleAttributeMatrixName(reader->readString("FaceEnsembleAttributeMatrixName", getFaceEnsembleAttributeMatrixName() ) );
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath() ) );
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath() ) );
  setFeatureEulerAnglesArrayPath(reader->readDataArrayPath("FeatureEulerAnglesArrayPath", getFeatureEulerAnglesArrayPath() ) );
  setSurfaceMeshFaceAreasArrayPath(reader->readDataArrayPath("SurfaceMeshFaceAreasArrayPath", getSurfaceMeshFaceAreasArrayPath() ) );
  setSurfaceMeshFaceNormalsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceNormalsArrayPath", getSurfaceMeshFaceNormalsArrayPath() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  setGBCDRes( reader->readValue("GBCDRes", getGBCDRes()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindGBCD::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(FaceEnsembleAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(GBCDArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(FeaturePhasesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(FeatureEulerAnglesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFaceAreasArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFaceNormalsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(GBCDRes)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::dataCheckSurfaceMesh()
{
  setErrorCondition(0);

  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == sm.get()) { return; }

  QVector<size_t> tDims(1, m_CrystalStructuresPtr.lock()->getNumberOfTuples());
  sm->createNonPrereqAttributeMatrix<AbstractFilter>(this, getFaceEnsembleAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::FaceEnsemble);

  QVector<size_t> cDims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  m_SurfaceMeshFaceNormalsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceNormalsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceNormalsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  m_SurfaceMeshFaceAreasPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceAreasArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceAreasPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceAreas = m_SurfaceMeshFaceAreasPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // call the sizeGBCD function to get the GBCD ranges, dimensions, etc.  Note that the input parameters do not affect the size and can be dummy values here;
  sizeGBCD(0, 0);
  cDims.resize(6);
  cDims[0] = m_GbcdSizes[0];
  cDims[1] = m_GbcdSizes[1];
  cDims[2] = m_GbcdSizes[2];
  cDims[3] = m_GbcdSizes[3];
  cDims[4] = m_GbcdSizes[4];
  cDims[5] = 2;

  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getFaceEnsembleAttributeMatrixName(), getGBCDArrayName() );
  m_GBCDPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this,  tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GBCDPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GBCD = m_GBCDPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::dataCheckVoxel()
{
  setErrorCondition(0);

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureEulerAnglesArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 3);
  m_FeatureEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getFeatureEulerAnglesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeaturePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheckVoxel();
  // order here matters...because we are going to use the size of the crystal structures out of the dataCheckVoxel to size the faceAttrMat in dataCheckSurfaceMesh
  if (getErrorCondition() >= 0) { dataCheckSurfaceMesh(); }
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::execute()
{
  setErrorCondition(0);
  dataCheckVoxel();
  if(getErrorCondition() < 0) { return; }
  // order here matters...because we are going to use the size of the crystal structures out of the dataCheckVoxel to size the faceAttrMat in dataCheckSurfaceMesh
  dataCheckSurfaceMesh();
  if(getErrorCondition() < 0) { return; }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  size_t totalPhases = m_CrystalStructuresPtr.lock()->getNumberOfTuples();
  size_t totalFaces = m_SurfaceMeshFaceLabelsPtr.lock()->getNumberOfTuples();
  size_t faceChunkSize = 50000;
  size_t numMisoReps = 576 * 4;
  if (totalFaces < faceChunkSize) { faceChunkSize = totalFaces; }
  // call the sizeGBCD function with proper chunkSize and numMisoReps to get Bins array set up properly
  sizeGBCD(faceChunkSize, numMisoReps);
  int32_t totalGBCDBins = m_GbcdSizes[0] * m_GbcdSizes[1] * m_GbcdSizes[2] * m_GbcdSizes[3] * m_GbcdSizes[4] * 2;

  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;
  uint64_t startMillis = millis;
  uint64_t estimatedTime = 0;
  float timeDiff = 0.0f;
  startMillis =  QDateTime::currentMSecsSinceEpoch();
  int32_t hemisphere = 0;

  //create an array to hold the total face area for each phase and initialize the array to 0.0
  DoubleArrayType::Pointer totalFaceAreaPtr = DoubleArrayType::CreateArray(totalPhases, "totalFaceArea");
  totalFaceAreaPtr->initializeWithValue(0.0);
  double* totalFaceArea = totalFaceAreaPtr->getPointer(0);

  QString ss = QObject::tr("Calculating GBCD || 0/%1 Completed").arg(totalFaces);
  for (size_t i = 0; i < totalFaces; i = i + faceChunkSize)
  {
    if(getCancel() == true) { return; }
    if (i + faceChunkSize >= totalFaces)
    {
      faceChunkSize = totalFaces - i;
    }
    m_GbcdBinsArray->initializeWithValue(-1);
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(i, i + faceChunkSize),
                        CalculateGBCDImpl(i, numMisoReps, m_SurfaceMeshFaceLabelsPtr.lock(), m_SurfaceMeshFaceNormalsPtr.lock(), m_FeatureEulerAnglesPtr.lock(), m_FeaturePhasesPtr.lock(), m_CrystalStructuresPtr.lock(), m_GbcdBinsArray, m_GbcdHemiCheckArray, m_GbcdDeltasArray, m_GbcdSizesArray, m_GbcdLimitsArray), tbb::auto_partitioner());

    }
    else
#endif
    {
      CalculateGBCDImpl serial(i, numMisoReps, m_SurfaceMeshFaceLabelsPtr.lock(), m_SurfaceMeshFaceNormalsPtr.lock(), m_FeatureEulerAnglesPtr.lock(), m_FeaturePhasesPtr.lock(), m_CrystalStructuresPtr.lock(), m_GbcdBinsArray, m_GbcdHemiCheckArray, m_GbcdDeltasArray, m_GbcdSizesArray, m_GbcdLimitsArray);
      serial.generate(i, i + faceChunkSize);
    }

    currentMillis = QDateTime::currentMSecsSinceEpoch();
    if (currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Calculating GBCD || Triangles %1/%2 Completed").arg(i).arg(totalFaces);
      timeDiff = ((float)i / (float)(currentMillis - startMillis));
      estimatedTime = (float)(totalFaces - i) / timeDiff;
      ss = ss + QObject::tr(" || Est. Time Remain: %1").arg(DREAM3D::convertMillisToHrsMinSecs(estimatedTime));
      millis = QDateTime::currentMSecsSinceEpoch();
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    }

    if(getCancel() == true) { return; }

    int32_t phase = 0;
    int32_t feature = 0;
    double area = 0.0;
    for (size_t j = 0; j < faceChunkSize; j++)
    {
      area = m_SurfaceMeshFaceAreas[i + j];
      feature = m_SurfaceMeshFaceLabels[2 * (i + j)];
      phase = m_FeaturePhases[feature];
      for (size_t k = 0; k < numMisoReps; k++)
      {
        if (m_GbcdBins[(j * numMisoReps) + (k)] >= 0)
        {
          hemisphere = 0;
          if (m_HemiCheck[(j * numMisoReps) + k] == false) { hemisphere = 1; }
          m_GBCD[(phase * totalGBCDBins) + (2 * m_GbcdBins[(j * numMisoReps) + (k)] + hemisphere)] += area;
          totalFaceArea[phase] += area;
        }
      }
    }
  }

  ss = QObject::tr("Starting GBCD Normalization");
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

  for (int32_t i = 0; i < totalPhases; i++)
  {
    size_t phaseShift = i * totalGBCDBins;
    double MRDfactor = double(totalGBCDBins) / totalFaceArea[i];
    for (int32_t j = 0; j < totalGBCDBins; j++)
    {
      m_GBCD[phaseShift + j] *= MRDfactor;
    }
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCD::sizeGBCD(size_t faceChunkSize, size_t numMisoReps)
{
  m_GbcdDeltasArray = FloatArrayType::CreateArray(5, "GBCDDeltas");
  m_GbcdDeltasArray->initializeWithZeros();
  m_GbcdLimitsArray = FloatArrayType::CreateArray(10, "GBCDLimits");
  m_GbcdLimitsArray->initializeWithZeros();
  m_GbcdSizesArray = Int32ArrayType::CreateArray(5, "GBCDSizes");
  m_GbcdSizesArray->initializeWithZeros();
  QVector<size_t> cDims(1, numMisoReps);
  m_GbcdBinsArray = Int32ArrayType::CreateArray(faceChunkSize, cDims, "GBCDBins");
  m_GbcdBinsArray->initializeWithZeros();
  m_GbcdHemiCheckArray = BoolArrayType::CreateArray(faceChunkSize, cDims, "GBCDHemiCheck");
  m_GbcdHemiCheckArray->initializeWithValue(false);

  m_GbcdDeltas = m_GbcdDeltasArray->getPointer(0);
  m_GbcdSizes = m_GbcdSizesArray->getPointer(0);
  m_GbcdLimits = m_GbcdLimitsArray->getPointer(0);
  m_GbcdBins = m_GbcdBinsArray->getPointer(0);
  m_HemiCheck = m_GbcdHemiCheckArray->getPointer(0);

  //Original Ranges from Dave R.
  //m_GBCDlimits[0] = 0.0f;
  //m_GBCDlimits[1] = cosf(1.0f*m_pi);
  //m_GBCDlimits[2] = 0.0f;
  //m_GBCDlimits[3] = 0.0f;
  //m_GBCDlimits[4] = cosf(1.0f*m_pi);
  //m_GBCDlimits[5] = 2.0f*m_pi;
  //m_GBCDlimits[6] = cosf(0.0f);
  //m_GBCDlimits[7] = 2.0f*m_pi;
  //m_GBCDlimits[8] = 2.0f*m_pi;
  //m_GBCDlimits[9] = cosf(0.0f);

  // Greg's Ranges
  m_GbcdLimits[0] = 0.0f;
  m_GbcdLimits[1] = 0.0f;
  m_GbcdLimits[2] = 0.0f;
  m_GbcdLimits[3] = 0.0f;
  m_GbcdLimits[4] = 0.0f;
  m_GbcdLimits[5] = SIMPLib::Constants::k_PiOver2;
  m_GbcdLimits[6] = 1.0f;
  m_GbcdLimits[7] = SIMPLib::Constants::k_PiOver2;
  m_GbcdLimits[8] = 1.0f;
  m_GbcdLimits[9] = SIMPLib::Constants::k_2Pi;

  float binsize = m_GBCDRes * SIMPLib::Constants::k_PiOver180;
  float binsize2 = binsize * (2.0 / SIMPLib::Constants::k_Pi);
  m_GbcdDeltas[0] = binsize;
  m_GbcdDeltas[1] = binsize2;
  m_GbcdDeltas[2] = binsize;
  m_GbcdDeltas[3] = binsize2;
  m_GbcdDeltas[4] = binsize;

  m_GbcdSizes[0] = int32_t(0.5 + (m_GbcdLimits[5] - m_GbcdLimits[0]) / m_GbcdDeltas[0]);
  m_GbcdSizes[1] = int32_t(0.5 + (m_GbcdLimits[6] - m_GbcdLimits[1]) / m_GbcdDeltas[1]);
  m_GbcdSizes[2] = int32_t(0.5 + (m_GbcdLimits[7] - m_GbcdLimits[2]) / m_GbcdDeltas[2]);
  m_GbcdSizes[3] = int32_t(0.5 + (m_GbcdLimits[8] - m_GbcdLimits[3]) / m_GbcdDeltas[3]);
  m_GbcdSizes[4] = int32_t(0.5 + (m_GbcdLimits[9] - m_GbcdLimits[4]) / m_GbcdDeltas[4]);

  // reset the 3rd and 4th dimensions using the square grid approach
  float totalNormalBins = m_GbcdSizes[3] * m_GbcdSizes[4];
  m_GbcdSizes[3] = int32_t(sqrtf(totalNormalBins) + 0.5f);
  m_GbcdSizes[4] = int32_t(sqrtf(totalNormalBins) + 0.5f);
  m_GbcdLimits[3] = -sqrtf(SIMPLib::Constants::k_PiOver2);
  m_GbcdLimits[4] = -sqrtf(SIMPLib::Constants::k_PiOver2);
  m_GbcdLimits[8] = sqrtf(SIMPLib::Constants::k_PiOver2);
  m_GbcdLimits[9] = sqrtf(SIMPLib::Constants::k_PiOver2);
  m_GbcdDeltas[3] = (m_GbcdLimits[8] - m_GbcdLimits[3]) / float(m_GbcdSizes[3]);
  m_GbcdDeltas[4] = (m_GbcdLimits[9] - m_GbcdLimits[4]) / float(m_GbcdSizes[4]);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindGBCD::newFilterInstance(bool copyFilterParameters)
{
  FindGBCD::Pointer filter = FindGBCD::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCD::getCompiledLibraryName()
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCD::getBrandingString()
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCD::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCD::getGroupName()
{ return SIMPL::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCD::getSubGroupName()
{ return SIMPL::FilterSubGroups::CrystallographicFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCD::getHumanLabel()
{ return "Find GBCD"; }
