/* ============================================================================
 * This filter has been created by Krzysztof Glowinski (kglowinski at ymail.com).
 * It includes an implementation of the algorithm described in:
 * K.Glowinski, A.Morawiec, "Analysis of experimental grain boundary distributions
 * based on boundary-space metrics", Metall. Mater. Trans. A 45, 3189-3194 (2014).
 * Besides the algorithm itself, many parts of the code come from
 * the sources of other filters, mainly "Find GBCD" and "Write GBCD Pole Figure (GMT5)".
 * Therefore, the below copyright notice applies.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

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

#include "FindGBCDMetricBased.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AxisAngleFilterParameter.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/OutputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"
#include "OrientationLib/LaueOps/LaueOps.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include "tbb/concurrent_vector.h"
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

// Include the MOC generated file for this class
#include "moc_FindGBCDMetricBased.cpp"

const float FindGBCDMetricBased::k_ResolutionChoices[FindGBCDMetricBased::k_NumberResolutionChoices][2] = {{3.0f, 7.0f}, {5.0f, 5.0f}, {5.0f, 7.0f}, {5.0f, 8.0f},
                                                                                                           {6.0f, 7.0f}, {7.0f, 7.0f}, {8.0f, 8.0f}}; // { for misorient., for planes }

const double FindGBCDMetricBased::k_BallVolumesM3M[FindGBCDMetricBased::k_NumberResolutionChoices] = {0.0000641361, 0.000139158, 0.000287439, 0.00038019, 0.000484151, 0.000747069, 0.00145491};

namespace GBCDMetricBased
{

/**
 * @brief The TriAreaAndNormals class defines a container that stores the area of a given triangle
 * and the two normals for grains on either side of the triangle
 */
class TriAreaAndNormals
{
public:
  double area;
  float normal_grain1_x;
  float normal_grain1_y;
  float normal_grain1_z;
  float normal_grain2_x;
  float normal_grain2_y;
  float normal_grain2_z;

  TriAreaAndNormals(double __area, float n1x, float n1y, float n1z, float n2x, float n2y, float n2z)
  : area(__area)
  , normal_grain1_x(n1x)
  , normal_grain1_y(n1y)
  , normal_grain1_z(n1z)
  , normal_grain2_x(n2x)
  , normal_grain2_y(n2y)
  , normal_grain2_z(n2z)
  {
  }

  TriAreaAndNormals()
  {
    TriAreaAndNormals(0.0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  }
};

/**
 * @brief The TrisSelector class implements a threaded algorithm that determines which triangles to
 * include in the GBCD calculation
 */
class TrisSelector
{
  bool m_ExcludeTripleLines;
  int64_t* m_Triangles;
  int8_t* m_NodeTypes;

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::concurrent_vector<TriAreaAndNormals>* selectedTris;
#else
  QVector<TriAreaAndNormals>* selectedTris;
#endif
  QVector<int8_t>* triIncluded;
  float m_misorResol;
  int32_t m_PhaseOfInterest;
  float (&gFixedT)[3][3];

  QVector<LaueOps::Pointer> m_OrientationOps;
  uint32_t cryst;
  int32_t nsym;

  uint32_t* m_CrystalStructures;
  float* m_Eulers;
  int32_t* m_Phases;
  int32_t* m_FaceLabels;
  double* m_FaceNormals;
  double* m_FaceAreas;

public:
  TrisSelector(bool __m_ExcludeTripleLines, int64_t* __m_Triangles, int8_t* __m_NodeTypes,

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
               tbb::concurrent_vector<TriAreaAndNormals>* __selectedTris,
#else
               QVector<TriAreaAndNormals>* __selectedTris,
#endif
               QVector<int8_t>* __triIncluded, float __m_misorResol, int32_t __m_PhaseOfInterest, float (&__gFixedT)[3][3], uint32_t* __m_CrystalStructures, float* __m_Eulers, int32_t* __m_Phases,
               int32_t* __m_FaceLabels, double* __m_FaceNormals, double* __m_FaceAreas)
  : m_ExcludeTripleLines(__m_ExcludeTripleLines)
  , m_Triangles(__m_Triangles)
  , m_NodeTypes(__m_NodeTypes)
  , selectedTris(__selectedTris)
  , triIncluded(__triIncluded)
  , m_misorResol(__m_misorResol)
  , m_PhaseOfInterest(__m_PhaseOfInterest)
  , gFixedT(__gFixedT)
  , m_CrystalStructures(__m_CrystalStructures)
  , m_Eulers(__m_Eulers)
  , m_Phases(__m_Phases)
  , m_FaceLabels(__m_FaceLabels)
  , m_FaceNormals(__m_FaceNormals)
  , m_FaceAreas(__m_FaceAreas)
  {
    m_OrientationOps = LaueOps::getOrientationOpsQVector();
    cryst = __m_CrystalStructures[__m_PhaseOfInterest];
    nsym = m_OrientationOps[cryst]->getNumSymOps();
  }

  virtual ~TrisSelector()
  {
  }

  void select(size_t start, size_t end) const
  {
    float g1ea[3] = {0.0f, 0.0f, 0.0f};
    float g2ea[3] = {0.0f, 0.0f, 0.0f};

    float g1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float g2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

    float g1s[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float g2s[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

    float sym1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float sym2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

    float g2sT[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

    float dg[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    float dgT[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

    float diffFromFixed[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

    float normal_lab[3] = {0.0f, 0.0f, 0.0f};
    float normal_grain1[3] = {0.0f, 0.0f, 0.0f};
    float normal_grain2[3] = {0.0f, 0.0f, 0.0f};

    for(size_t triIdx = start; triIdx < end; triIdx++)
    {

      int32_t feature1 = m_FaceLabels[2 * triIdx];
      int32_t feature2 = m_FaceLabels[2 * triIdx + 1];

      if(feature1 < 1 || feature2 < 1)
      {
        continue;
      }
      if(m_Phases[feature1] != m_Phases[feature2])
      {
        continue;
      }
      if(m_Phases[feature1] != m_PhaseOfInterest || m_Phases[feature2] != m_PhaseOfInterest)
      {
        continue;
      }

      if(m_ExcludeTripleLines == true)
      {
        int64_t node1 = m_Triangles[triIdx * 3];
        int64_t node2 = m_Triangles[triIdx * 3 + 1];
        int64_t node3 = m_Triangles[triIdx * 3 + 2];

        if(m_NodeTypes[node1] != 2 || m_NodeTypes[node2] != 2 || m_NodeTypes[node3] != 2)
        {
          continue;
        }
      }

      (*triIncluded)[triIdx] = 1;

      normal_lab[0] = static_cast<float>(m_FaceNormals[3 * triIdx]);
      normal_lab[1] = static_cast<float>(m_FaceNormals[3 * triIdx + 1]);
      normal_lab[2] = static_cast<float>(m_FaceNormals[3 * triIdx + 2]);

      for(int whichEa = 0; whichEa < 3; whichEa++)
      {
        g1ea[whichEa] = m_Eulers[3 * feature1 + whichEa];
        g2ea[whichEa] = m_Eulers[3 * feature2 + whichEa];
      }

      FOrientArrayType om(9, 0.0f);
      FOrientTransformsType::eu2om(FOrientArrayType(g1ea, 3), om);
      om.toGMatrix(g1);
      FOrientTransformsType::eu2om(FOrientArrayType(g2ea, 3), om);
      om.toGMatrix(g2);

      for(int j = 0; j < nsym; j++)
      {
        // rotate g1 by symOp
        m_OrientationOps[cryst]->getMatSymOp(j, sym1);
        MatrixMath::Multiply3x3with3x3(sym1, g1, g1s);
        // get the crystal directions along the triangle normals
        MatrixMath::Multiply3x3with3x1(g1s, normal_lab, normal_grain1);

        for(int k = 0; k < nsym; k++)
        {
          // calculate the symmetric misorienation
          m_OrientationOps[cryst]->getMatSymOp(k, sym2);
          // rotate g2 by symOp
          MatrixMath::Multiply3x3with3x3(sym2, g2, g2s);
          // transpose rotated g2
          MatrixMath::Transpose3x3(g2s, g2sT);
          // calculate delta g
          MatrixMath::Multiply3x3with3x3(g1s, g2sT, dg); // dg -- the misorientation between adjacent grains
          MatrixMath::Transpose3x3(dg, dgT);

          for(int transpose = 0; transpose <= 1; transpose++)
          {
            // check if dg is close to gFix
            if(transpose == 0)
            {
              MatrixMath::Multiply3x3with3x3(dg, gFixedT, diffFromFixed);
            }
            else
            {
              MatrixMath::Multiply3x3with3x3(dgT, gFixedT, diffFromFixed);
            }

            float diffAngle = acosf((diffFromFixed[0][0] + diffFromFixed[1][1] + diffFromFixed[2][2] - 1.0f) * 0.5f);

            if(diffAngle < m_misorResol)
            {
              MatrixMath::Multiply3x3with3x1(dgT, normal_grain1, normal_grain2); // minus sign before normal_grain2 will be added later

              if(transpose == 0)
              {
                (*selectedTris).push_back(TriAreaAndNormals(m_FaceAreas[triIdx], normal_grain1[0], normal_grain1[1], normal_grain1[2], -normal_grain2[0], -normal_grain2[1], -normal_grain2[2]));
              }
              else
              {
                (*selectedTris).push_back(TriAreaAndNormals(m_FaceAreas[triIdx], -normal_grain2[0], -normal_grain2[1], -normal_grain2[2], normal_grain1[0], normal_grain1[1], normal_grain1[2]));
              }
            }
          }
        }
      }
    }
  }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    select(r.begin(), r.end());
  }
#endif
};

/**
 * @brief The ProbeDistrib class implements a threaded algorithm that determines the distribution values
 * for the GBCD
 */
class ProbeDistrib
{
  QVector<double>* distribValues;
  QVector<double>* errorValues;
  QVector<float> samplPtsX;
  QVector<float> samplPtsY;
  QVector<float> samplPtsZ;
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::concurrent_vector<TriAreaAndNormals> selectedTris;
#else
  QVector<TriAreaAndNormals> selectedTris;
#endif
  float planeResolSq;
  double totalFaceArea;
  int numDistinctGBs;
  double ballVolume;
  float (&gFixedT)[3][3];

public:
  ProbeDistrib(QVector<double>* __distribValues, QVector<double>* __errorValues, QVector<float> __samplPtsX, QVector<float> __samplPtsY, QVector<float> __samplPtsZ,
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
               tbb::concurrent_vector<TriAreaAndNormals> __selectedTris,
#else
               QVector<TriAreaAndNormals> __selectedTris,
#endif
               float __planeResolSq, double __totalFaceArea, int __numDistinctGBs, double __ballVolume, float (&__gFixedT)[3][3])
  : distribValues(__distribValues)
  , errorValues(__errorValues)
  , samplPtsX(__samplPtsX)
  , samplPtsY(__samplPtsY)
  , samplPtsZ(__samplPtsZ)
  , selectedTris(__selectedTris)
  , planeResolSq(__planeResolSq)
  , totalFaceArea(__totalFaceArea)
  , numDistinctGBs(__numDistinctGBs)
  , ballVolume(__ballVolume)
  , gFixedT(__gFixedT)
  {
  }

  virtual ~ProbeDistrib()
  {
  }

  void probe(size_t start, size_t end) const
  {
    for(size_t ptIdx = start; ptIdx < end; ptIdx++)
    {
      float fixedNormal1[3] = {samplPtsX.at(ptIdx), samplPtsY.at(ptIdx), samplPtsZ.at(ptIdx)};
      float fixedNormal2[3] = {0.0f, 0.0f, 0.0f};
      MatrixMath::Multiply3x3with3x1(gFixedT, fixedNormal1, fixedNormal2);

      for(int triRepresIdx = 0; triRepresIdx < static_cast<int>(selectedTris.size()); triRepresIdx++)
      {
        for(int inversion = 0; inversion <= 1; inversion++)
        {
          float sign = 1.0f;
          if(inversion == 1)
            sign = -1.0f;

          float theta1 = acosf(sign * (selectedTris[triRepresIdx].normal_grain1_x * fixedNormal1[0] + selectedTris[triRepresIdx].normal_grain1_y * fixedNormal1[1] +
                                       selectedTris[triRepresIdx].normal_grain1_z * fixedNormal1[2]));

          float theta2 = acosf(-sign * (selectedTris[triRepresIdx].normal_grain2_x * fixedNormal2[0] + selectedTris[triRepresIdx].normal_grain2_y * fixedNormal2[1] +
                                        selectedTris[triRepresIdx].normal_grain2_z * fixedNormal2[2]));

          float distSq = 0.5f * (theta1 * theta1 + theta2 * theta2);

          if(distSq < planeResolSq)
          {
            (*distribValues)[ptIdx] += selectedTris[triRepresIdx].area;
          }
        }
      }
      (*errorValues)[ptIdx] = sqrt((*distribValues)[ptIdx] / totalFaceArea / double(numDistinctGBs)) / ballVolume;

      (*distribValues)[ptIdx] /= totalFaceArea;
      (*distribValues)[ptIdx] /= ballVolume;
    }
  }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    probe(r.begin(), r.end());
  }
#endif
};

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGBCDMetricBased::FindGBCDMetricBased()
: SurfaceMeshFilter()
, m_PhaseOfInterest(1)
, m_ChosenLimitDists(FindGBCDMetricBased::k_DefaultResolutionChoice)
, m_NumSamplPts(3000)
, m_ExcludeTripleLines(false)
, m_DistOutputFile("")
, m_ErrOutputFile("")
, m_SaveRelativeErr(false)
, m_CrystalStructuresArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures)
, m_FeatureEulerAnglesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::AvgEulerAngles)
, m_FeaturePhasesArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases)
, m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_SurfaceMeshFaceNormalsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceNormals)
, m_SurfaceMeshFaceAreasArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceAreas)
, m_SurfaceMeshFeatureFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceFeatureAttributeMatrixName, "FaceLabels")
, m_NodeTypesArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::VertexAttributeMatrixName, SIMPL::VertexData::SurfaceMeshNodeType)
, m_CrystalStructures(nullptr)
, m_FeatureEulerAngles(nullptr)
, m_FeaturePhases(nullptr)
, m_SurfaceMeshFaceAreas(nullptr)
, m_SurfaceMeshFaceLabels(nullptr)
, m_SurfaceMeshFaceNormals(nullptr)
, m_SurfaceMeshFeatureFaceLabels(nullptr)
, m_NodeTypes(nullptr)
{
  m_MisorientationRotation.angle = 17.9f;
  m_MisorientationRotation.h = 1.0f;
  m_MisorientationRotation.k = 1.0f;
  m_MisorientationRotation.l = 1.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindGBCDMetricBased::~FindGBCDMetricBased()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCDMetricBased::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Phase of Interest", PhaseOfInterest, FilterParameter::Parameter, FindGBCDMetricBased));
  parameters.push_back(SIMPL_NEW_AXISANGLE_FP("Fixed Misorientation", MisorientationRotation, FilterParameter::Parameter, FindGBCDMetricBased));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Limiting Distances");
    parameter->setPropertyName("ChosenLimitDists");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(FindGBCDMetricBased, this, ChosenLimitDists));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(FindGBCDMetricBased, this, ChosenLimitDists));

    QVector<QString> choices;

    for(int choiceIdx = 0; choiceIdx < FindGBCDMetricBased::k_NumberResolutionChoices; choiceIdx++)
    {
      QString misorResStr;
      QString planeResStr;
      QString degSymbol = QChar(0x00B0);

      misorResStr.setNum(FindGBCDMetricBased::k_ResolutionChoices[choiceIdx][0], 'f', 0);
      planeResStr.setNum(FindGBCDMetricBased::k_ResolutionChoices[choiceIdx][1], 'f', 0);

      choices.push_back(misorResStr + degSymbol + " for Misorientations; " + planeResStr + degSymbol + " for Plane Inclinations");
    }

    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Sampling Points (on a Hemisphere)", NumSamplPts, FilterParameter::Parameter, FindGBCDMetricBased));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Exclude Triangles Directly Neighboring Triple Lines", ExcludeTripleLines, FilterParameter::Parameter, FindGBCDMetricBased));
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output Distribution File", DistOutputFile, FilterParameter::Parameter, FindGBCDMetricBased, "*.dat", "DAT File"));
  parameters.push_back(SIMPL_NEW_OUTPUT_FILE_FP("Output Distribution Errors File", ErrOutputFile, FilterParameter::Parameter, FindGBCDMetricBased, "*.dat", "DAT File"));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Save Relative Errors Instead of Their Absolute Values", SaveRelativeErr, FilterParameter::Parameter, FindGBCDMetricBased));
  parameters.push_back(SeparatorFilterParameter::New("Vertex Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int8, 1, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Node Types", NodeTypesArrayPath, FilterParameter::RequiredArray, FindGBCDMetricBased, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", SurfaceMeshFaceLabelsArrayPath, FilterParameter::RequiredArray, FindGBCDMetricBased, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Double, 3, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Normals", SurfaceMeshFaceNormalsArrayPath, FilterParameter::RequiredArray, FindGBCDMetricBased, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Double, 1, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Areas", SurfaceMeshFaceAreasArrayPath, FilterParameter::RequiredArray, FindGBCDMetricBased, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::FaceFeature, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Face Labels", SurfaceMeshFeatureFaceLabelsArrayPath, FilterParameter::RequiredArray, FindGBCDMetricBased, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Average Euler Angles", FeatureEulerAnglesArrayPath, FilterParameter::RequiredArray, FindGBCDMetricBased, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, FindGBCDMetricBased, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Type::CellEnsemble, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, FindGBCDMetricBased, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCDMetricBased::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setPhaseOfInterest(reader->readValue("PhaseOfInterest", getPhaseOfInterest()));
  setMisorientationRotation(reader->readAxisAngle("MisorientationRotation", getMisorientationRotation(), -1));
  setChosenLimitDists(reader->readValue("ChosenLimitDists", getChosenLimitDists()));
  setNumSamplPts(reader->readValue("NumSamplPts", getNumSamplPts()));
  setExcludeTripleLines(reader->readValue("ExcludeTripleLines", getExcludeTripleLines()));
  setDistOutputFile(reader->readString("DistOutputFile", getDistOutputFile()));
  setErrOutputFile(reader->readString("ErrOutputFile", getErrOutputFile()));
  setSaveRelativeErr(reader->readValue("SaveRelativeErr", getSaveRelativeErr()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructures", getCrystalStructuresArrayPath()));
  setFeatureEulerAnglesArrayPath(reader->readDataArrayPath("FeatureEulerAngles", getFeatureEulerAnglesArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhases", getFeaturePhasesArrayPath()));
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabels", getSurfaceMeshFaceLabelsArrayPath()));
  setSurfaceMeshFaceNormalsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceNormals", getSurfaceMeshFaceNormalsArrayPath()));
  setSurfaceMeshFeatureFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFeatureFaceLabels", getSurfaceMeshFeatureFaceLabelsArrayPath()));
  setSurfaceMeshFaceAreasArrayPath(reader->readDataArrayPath("SurfaceMeshFaceAreas", getSurfaceMeshFaceAreasArrayPath()));
  setNodeTypesArrayPath(reader->readDataArrayPath("NodeTypes", getNodeTypesArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCDMetricBased::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCDMetricBased::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  // Fixed Misorientation (filter params.)
  if(getMisorientationRotation().angle <= 0.0 || getMisorientationRotation().angle > 180.0)
  {
    QString degSymbol = QChar(0x00B0);
    setErrorCondition(-1000);
    QString ss = "The misorientation angle should be in the range (0, 180" + degSymbol + "]";
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getMisorientationRotation().h == 0.0f && getMisorientationRotation().k == 0.0f && getMisorientationRotation().l == 0.0f)
  {
    setErrorCondition(-1001);
    QString ss = QObject::tr("All three indices of the misorientation axis cannot be 0");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  // Number of Sampling Points (filter params.)
  if(getNumSamplPts() < 1)
  {
    setErrorCondition(-1002);
    QString ss = QObject::tr("The number of sampling points must be greater than zero");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  // Set some reasonable value, but allow user to use more if he/she knows what he/she does
  if(getNumSamplPts() > 5000)
  {
    setWarningCondition(-1003);
    QString ss = QObject::tr("The number of sampling points is greater than 5000, but it is unlikely that many are needed");
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
  }

  // Output files (filter params.)
  if(getDistOutputFile().isEmpty() == true)
  {
    setErrorCondition(-1004);
    QString ss = QObject::tr("The output file for saving the distribution must be set");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getErrOutputFile().isEmpty() == true)
  {
    setErrorCondition(-1005);
    QString ss = QObject::tr("The output file for saving the distribution errors must be set");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QFileInfo distOutFileInfo(getDistOutputFile());
  QDir distParentPath = distOutFileInfo.path();
  if(distParentPath.exists() == false)
  {
    setWarningCondition(-1006);
    QString ss = QObject::tr("The directory path for the distribution output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
  }

  QFileInfo errOutFileInfo(getErrOutputFile());
  QDir errParentPath = errOutFileInfo.path();
  if(errParentPath.exists() == false)
  {
    setWarningCondition(-1007);
    QString ss = QObject::tr("The directory path for the distribution errors output file does not exist. DREAM.3D will attempt to create this path during execution of the filter");
    notifyWarningMessage(getHumanLabel(), ss, getWarningCondition());
  }

  if(distOutFileInfo.suffix().compare("") == 0)
  {
    setDistOutputFile(getDistOutputFile().append(".dat"));
  }

  if(errOutFileInfo.suffix().compare("") == 0)
  {
    setErrOutputFile(getErrOutputFile().append(".dat"));
  }

  // Make sure the file name ends with _1 so the GMT scripts work correctly
  QString distFName = distOutFileInfo.baseName();
  if(!distFName.isEmpty() && distFName.endsWith("_1") == false)
  {
    distFName = distFName + "_1";
    QString absPath = distOutFileInfo.absolutePath() + "/" + distFName + ".dat";
    setDistOutputFile(absPath);
  }

  QString errFName = errOutFileInfo.baseName();
  if(!errFName.isEmpty() && errFName.endsWith("_1") == false)
  {
    errFName = errFName + "_1";
    QString absPath = errOutFileInfo.absolutePath() + "/" + errFName + ".dat";
    setErrOutputFile(absPath);
  }

  if(getDistOutputFile().isEmpty() == false && getDistOutputFile() == getErrOutputFile())
  {
    setErrorCondition(-1008);
    QString ss = QObject::tr("The output files must be different");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  // Crystal Structures
  QVector<size_t> cDims(1, 1);
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Phase of Interest  (filter params.)
  if(nullptr != m_CrystalStructuresPtr.lock().get())
  {
    if(getPhaseOfInterest() >= static_cast<int>(m_CrystalStructuresPtr.lock()->getNumberOfTuples()) || getPhaseOfInterest() <= 0)
    {
      setErrorCondition(-1009);
      QString ss = QObject::tr("The phase index is either larger than the number of Ensembles or smaller than 1");
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }

  // Euler Angles
  cDims[0] = 3;
  m_FeatureEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getFeatureEulerAnglesArrayPath(),
                                                                                                              cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureEulerAnglesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Phases
  cDims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Face Labels
  cDims[0] = 2;
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(),
                                                                                                                   cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Face Normals
  cDims[0] = 3;
  m_SurfaceMeshFaceNormalsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceNormalsArrayPath(),
                                                                                                                   cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceNormalsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Face Areas
  cDims[0] = 1;
  m_SurfaceMeshFaceAreasPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceAreasArrayPath(),
                                                                                                                 cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceAreasPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceAreas = m_SurfaceMeshFaceAreasPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Face Labels
  cDims[0] = 2;
  m_SurfaceMeshFeatureFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(
      this, getSurfaceMeshFeatureFaceLabelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFeatureFaceLabelsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFeatureFaceLabels = m_SurfaceMeshFeatureFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Node Types
  cDims[0] = 1;
  m_NodeTypesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, getNodeTypesArrayPath(),
                                                                                                      cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_NodeTypesPtr.lock().get())                                                                  /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_NodeTypes = m_NodeTypesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCDMetricBased::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindGBCDMetricBased::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  // -------------------- set resolutions and 'ball volumes' based on user's selection -------------
  float m_misorResol = FindGBCDMetricBased::k_ResolutionChoices[getChosenLimitDists()][0];
  float m_planeResol = FindGBCDMetricBased::k_ResolutionChoices[getChosenLimitDists()][1];

  m_misorResol *= SIMPLib::Constants::k_PiOver180;
  m_planeResol *= SIMPLib::Constants::k_PiOver180;
  float m_PlaneResolSq = m_planeResol * m_planeResol;

  // We want to work with the raw pointers for speed so get those pointers.
  uint32_t* m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  float* m_Eulers = m_FeatureEulerAnglesPtr.lock()->getPointer(0);
  int32_t* m_Phases = m_FeaturePhasesPtr.lock()->getPointer(0);
  int32_t* m_FaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  double* m_FaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0);
  double* m_FaceAreas = m_SurfaceMeshFaceAreasPtr.lock()->getPointer(0);
  int32_t* m_FeatureFaceLabels = m_SurfaceMeshFeatureFaceLabelsPtr.lock()->getPointer(0);
  int8_t* m_NodeTypes = m_NodeTypesPtr.lock()->getPointer(0);

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceAreasArrayPath().getDataContainerName());
  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();
  SharedTriList::Pointer m_TrianglesPtr = triangleGeom->getTriangles();
  int64_t* m_Triangles = m_TrianglesPtr->getPointer(0);

  // -------------------- check if directiories are ok and if output files can be opened -----------

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QFileInfo distOutFileInfo(getDistOutputFile());
  QDir distOutFileDir(distOutFileInfo.path());
  if(!distOutFileDir.mkpath("."))
  {
    QString ss;
    ss = QObject::tr("Error creating parent path '%1'").arg(distOutFileDir.path());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }

  QFile distOutFile(getDistOutputFile());
  if(!distOutFile.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Error opening output file '%1'").arg(getDistOutputFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QFileInfo errOutFileInfo(getDistOutputFile());

  QDir errOutFileDir(errOutFileInfo.path());
  if(!errOutFileDir.mkpath("."))
  {
    QString ss;
    ss = QObject::tr("Error creating parent path '%1'").arg(errOutFileDir.path());
    notifyErrorMessage(getHumanLabel(), ss, -1);
    setErrorCondition(-1);
    return;
  }

  QFile errOutFile(getDistOutputFile());
  if(!errOutFile.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Error opening output file '%1'").arg(getDistOutputFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // Open the output files, should be opened and checked before starting computations
  FILE* fDist = nullptr;
  fDist = fopen(m_DistOutputFile.toLatin1().data(), "wb");
  if(nullptr == fDist)
  {
    QString ss = QObject::tr("Error opening distribution output file '%1'").arg(m_DistOutputFile);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  FILE* fErr = nullptr;
  fErr = fopen(m_ErrOutputFile.toLatin1().data(), "wb");
  if(nullptr == fErr)
  {
    QString ss = QObject::tr("Error opening distribution errors output file '%1'").arg(m_ErrOutputFile);
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // ------------------- before computing the distribution, we must find normalization factors -----
  double ballVolume = FindGBCDMetricBased::k_BallVolumesM3M[getChosenLimitDists()];
  {
    QVector<LaueOps::Pointer> m_OrientationOps = LaueOps::getOrientationOpsQVector();
    int32_t cryst = m_CrystalStructures[m_PhaseOfInterest];
    int32_t nsym = m_OrientationOps[cryst]->getNumSymOps();

    if(cryst != 1)
    {
      double symFactor = double(nsym) / 24.0;
      symFactor *= symFactor;
      ballVolume *= symFactor;
    }
  }

  // ------------------------------ generation of sampling points ----------------------------------
  QString ss = QObject::tr("|| Generating sampling points");
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

  // generate "Golden Section Spiral", see http://www.softimageblog.com/archives/115
  int numSamplPts_WholeSph = 2 * m_NumSamplPts; // here we generate points on the whole sphere
  QVector<float> samplPtsX(0);
  QVector<float> samplPtsY(0);
  QVector<float> samplPtsZ(0);

  float _inc = 2.3999632f; // = pi * (3 - sqrt(5))
  float _off = 2.0f / float(numSamplPts_WholeSph);

  for(int ptIdx_WholeSph = 0; ptIdx_WholeSph < numSamplPts_WholeSph; ptIdx_WholeSph++)
  {
    if(getCancel() == true)
    {
      return;
    }

    float _y = (float(ptIdx_WholeSph) * _off) - 1.0f + (0.5f * _off);
    float _r = sqrtf(fmaxf(1.0f - _y * _y, 0.0f));
    float _phi = float(ptIdx_WholeSph) * _inc;

    float z = sinf(_phi) * _r;

    if(z > 0.0f)
    {
      samplPtsX.push_back(cosf(_phi) * _r);
      samplPtsY.push_back(_y);
      samplPtsZ.push_back(z);
    }
  }

  // Add points at the equator for better performance of some plotting tools
  for(double phi = 0.0; phi <= SIMPLib::Constants::k_2Pi; phi += m_planeResol)
  {
    samplPtsX.push_back(cosf(static_cast<float>(phi)));
    samplPtsY.push_back(sinf(static_cast<float>(phi)));
    samplPtsZ.push_back(0.0f);
  }

  // Convert axis angle to matrix representation of misorientation
  float gFixed[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float gFixedT[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

  {
    float gFixedAngle = static_cast<float>(m_MisorientationRotation.angle * SIMPLib::Constants::k_PiOver180);
    float gFixedAxis[3] = {m_MisorientationRotation.h, m_MisorientationRotation.k, m_MisorientationRotation.l};
    MatrixMath::Normalize3x1(gFixedAxis);
    FOrientArrayType om(9);
    FOrientTransformsType::ax2om(FOrientArrayType(gFixedAxis[0], gFixedAxis[1], gFixedAxis[2], gFixedAngle), om);
    om.toGMatrix(gFixed);
  }

  MatrixMath::Transpose3x3(gFixed, gFixedT);

  size_t numMeshTris = m_SurfaceMeshFaceAreasPtr.lock()->getNumberOfTuples();

// ---------  find triangles (and equivalent crystallographic parameters) with +- the fixed misorientation ---------
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
  tbb::concurrent_vector<GBCDMetricBased::TriAreaAndNormals> selectedTris(0);
#else
  QVector<GBCDMetricBased::TriAreaAndNormals> selectedTris(0);
#endif

  QVector<int8_t> triIncluded(numMeshTris, 0);

  size_t trisChunkSize = 50000;
  if(numMeshTris < trisChunkSize)
  {
    trisChunkSize = numMeshTris;
  }

  for(size_t i = 0; i < numMeshTris; i = i + trisChunkSize)
  {
    if(getCancel() == true)
    {
      return;
    }
    ss = QObject::tr("|| Step 1/2: Selecting Triangles with the Specified Misorientation (%1% completed)").arg(int(100.0 * float(i) / float(numMeshTris)));
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    if(i + trisChunkSize >= numMeshTris)
    {
      trisChunkSize = numMeshTris - i;
    }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if(doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(i, i + trisChunkSize), GBCDMetricBased::TrisSelector(m_ExcludeTripleLines, m_Triangles, m_NodeTypes, &selectedTris, &triIncluded, m_misorResol, m_PhaseOfInterest,
                                                                                       gFixedT, m_CrystalStructures, m_Eulers, m_Phases, m_FaceLabels, m_FaceNormals, m_FaceAreas),
                        tbb::auto_partitioner());
    }
    else
#endif
    {
      GBCDMetricBased::TrisSelector serial(m_ExcludeTripleLines, m_Triangles, m_NodeTypes, &selectedTris, &triIncluded, m_misorResol, m_PhaseOfInterest, gFixedT, m_CrystalStructures, m_Eulers, m_Phases, m_FaceLabels,
                          m_FaceNormals, m_FaceAreas);
      serial.select(i, i + trisChunkSize);
    }
  }

  // ------------------------  find the number of distinct boundaries ------------------------------
  int32_t numDistinctGBs = 0;
  int32_t numFaceFeatures = m_SurfaceMeshFeatureFaceLabelsPtr.lock()->getNumberOfTuples();

  for(int featureFaceIdx = 0; featureFaceIdx < numFaceFeatures; featureFaceIdx++)
  {
    int32_t feature1 = m_FeatureFaceLabels[2 * featureFaceIdx];
    int32_t feature2 = m_FeatureFaceLabels[2 * featureFaceIdx + 1];

    if(feature1 < 1 || feature2 < 1)
    {
      continue;
    }
    if(m_FeaturePhases[feature1] != m_FeaturePhases[feature2])
    {
      continue;
    }
    if(m_FeaturePhases[feature1] != m_PhaseOfInterest || m_FeaturePhases[feature2] != m_PhaseOfInterest)
    {
      continue;
    }

    numDistinctGBs++;
  }

  // ----------------- determining distribution values at the sampling points (and their errors) ---
  double totalFaceArea = 0.0;
  for(size_t triIdx = 0; triIdx < numMeshTris; triIdx++)
  {
    totalFaceArea += m_FaceAreas[triIdx] * double(triIncluded.at(triIdx));
  }

  QVector<double> distribValues(samplPtsX.size(), 0.0);
  QVector<double> errorValues(samplPtsX.size(), 0.0);

  int32_t pointsChunkSize = 100;
  if(samplPtsX.size() < pointsChunkSize)
  {
    pointsChunkSize = samplPtsX.size();
  }

  for(int32_t i = 0; i < samplPtsX.size(); i = i + pointsChunkSize)
  {
    if(getCancel() == true)
    {
      return;
    }
    ss = QObject::tr("|| Step 2/2: Computing Distribution Values at the Section of Interest (%1% completed)").arg(int(100.0 * float(i) / float(samplPtsX.size())));
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    if(i + pointsChunkSize >= samplPtsX.size())
    {
      pointsChunkSize = samplPtsX.size() - i;
    }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if(doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(i, i + pointsChunkSize),
                        GBCDMetricBased::ProbeDistrib(&distribValues, &errorValues, samplPtsX, samplPtsY, samplPtsZ, selectedTris, m_PlaneResolSq, totalFaceArea, numDistinctGBs, ballVolume, gFixedT),
                        tbb::auto_partitioner());
    }
    else
#endif
    {
      GBCDMetricBased::ProbeDistrib serial(&distribValues, &errorValues, samplPtsX, samplPtsY, samplPtsZ, selectedTris, m_PlaneResolSq, totalFaceArea, numDistinctGBs, ballVolume, gFixedT);
      serial.probe(i, i + pointsChunkSize);
    }
  }

  // ------------------------------------------- writing the output --------------------------------
  fprintf(fDist, "%.1f %.1f %.1f %.1f\n", m_MisorientationRotation.h, m_MisorientationRotation.k, m_MisorientationRotation.l, m_MisorientationRotation.angle);
  fprintf(fErr, "%.1f %.1f %.1f %.1f\n", m_MisorientationRotation.h, m_MisorientationRotation.k, m_MisorientationRotation.l, m_MisorientationRotation.angle);

  for(int ptIdx = 0; ptIdx < samplPtsX.size(); ptIdx++)
  {

    float zenith = acosf(samplPtsZ.at(ptIdx));
    float azimuth = atan2f(samplPtsY.at(ptIdx), samplPtsX.at(ptIdx));

    float zenithDeg = static_cast<float>(SIMPLib::Constants::k_180OverPi * zenith);
    float azimuthDeg = static_cast<float>(SIMPLib::Constants::k_180OverPi * azimuth);

    fprintf(fDist, "%.2f %.2f %.4f\n", azimuthDeg, 90.0f - zenithDeg, distribValues[ptIdx]);

    if(m_SaveRelativeErr == false)
    {
      fprintf(fErr, "%.2f %.2f %.4f\n", azimuthDeg, 90.0f - zenithDeg, errorValues[ptIdx]);
    }
    else
    {
      double saneErr = 100.0;
      if(distribValues[ptIdx] > 1e-10)
      {
        saneErr = fmin(100.0, 100.0 * errorValues[ptIdx] / distribValues[ptIdx]);
      }
      fprintf(fErr, "%.2f %.2f %.2f\n", azimuthDeg, 90.0f - zenithDeg, saneErr);
    }
  }
  fclose(fDist);
  fclose(fErr);

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindGBCDMetricBased::newFilterInstance(bool copyFilterParameters)
{
  FindGBCDMetricBased::Pointer filter = FindGBCDMetricBased::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCDMetricBased::getCompiledLibraryName()
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCDMetricBased::getBrandingString()
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCDMetricBased::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCDMetricBased::getGroupName()
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCDMetricBased::getSubGroupName()
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindGBCDMetricBased::getHumanLabel()
{
  return "Find GBCD (Metric-Based Approach)";
}
