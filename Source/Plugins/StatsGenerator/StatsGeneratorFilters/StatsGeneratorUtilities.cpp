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
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "StatsGeneratorUtilities.h"

#include "EbsdLib/EbsdConstants.h"


#include "OrientationLib/Texture/Texture.hpp"

#include "SIMPLib/StatsData/StatsData.h"
#include "SIMPLib/StatsData/BoundaryStatsData.h"
#include "SIMPLib/StatsData/MatrixStatsData.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/TransformationStatsData.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGeneratorUtilities::StatsGeneratorUtilities()
{}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGeneratorUtilities::~StatsGeneratorUtilities()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUtilities::GenerateODFBinData(StatsData* statsData,
                                                 unsigned int phaseType, unsigned int crystalStructure,
                                                 QVector<float> &e1s, QVector<float> &e2s,
                                                 QVector<float> &e3s, QVector<float> &weights,
                                                 QVector<float> &sigmas, bool computeODF)
{

  QVector<float> odf;
  size_t numEntries = e1s.size();

  if ( Ebsd::CrystalStructure::Cubic_High == crystalStructure)
  {
    odf.resize(CubicOps::k_OdfSize);
    if (computeODF) { Texture::CalculateCubicODFData(e1s.data(), e2s.data(), e3s.data(),
                                   weights.data(), sigmas.data(), true,
                                   odf.data(), numEntries); }
  }
  else if ( Ebsd::CrystalStructure::Hexagonal_High == crystalStructure)
  {
    odf.resize(HexagonalOps::k_OdfSize);
    if (computeODF) { Texture::CalculateHexODFData(e1s.data(), e2s.data(), e3s.data(),
                                 weights.data(), sigmas.data(), true,
                                 odf.data(), numEntries); }
  }
  if (odf.size() > 0)
  {
    FloatArrayType::Pointer p = FloatArrayType::FromQVector(odf, SIMPL::StringConstants::ODF);
    if (phaseType == SIMPL::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
      pp->setODF(p);
    }
    if (phaseType == SIMPL::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
      pp->setODF(p);
    }
    if (phaseType == SIMPL::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
      tp->setODF(p);
    }
    if (e1s.size() > 0)
    {
      FloatArrayType::Pointer euler1 = FloatArrayType::FromQVector(e1s, SIMPL::StringConstants::Euler1);
      FloatArrayType::Pointer euler2 = FloatArrayType::FromQVector(e2s, SIMPL::StringConstants::Euler2);
      FloatArrayType::Pointer euler3 = FloatArrayType::FromQVector(e3s, SIMPL::StringConstants::Euler3);
      FloatArrayType::Pointer sigma = FloatArrayType::FromQVector(sigmas, SIMPL::StringConstants::Sigma);
      FloatArrayType::Pointer weight = FloatArrayType::FromQVector(weights, SIMPL::StringConstants::Weight);

      VectorOfFloatArray odfWeights;
      odfWeights.push_back(euler1);
      odfWeights.push_back(euler2);
      odfWeights.push_back(euler3);
      odfWeights.push_back(sigma);
      odfWeights.push_back(weight);
      if (phaseType == SIMPL::PhaseType::PrimaryPhase)
      {
        PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
        pp->setODF_Weights(odfWeights);
      }
      if (phaseType == SIMPL::PhaseType::PrecipitatePhase)
      {
        PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
        pp->setODF_Weights(odfWeights);
      }
      if (phaseType == SIMPL::PhaseType::TransformationPhase)
      {
        TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
        tp->setODF_Weights(odfWeights);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUtilities::GenerateAxisODFBinData(StatsData* statsData, unsigned int phaseType,
                                                     QVector<float> &e1s, QVector<float> &e2s,
                                                     QVector<float> &e3s, QVector<float> &weights,
                                                     QVector<float> &sigmas, bool computeAxisODF)
{
  QVector<float> aodf;
  size_t numEntries = e1s.size();

  aodf.resize(OrthoRhombicOps::k_OdfSize);
  if (computeAxisODF) { Texture::CalculateOrthoRhombicODFData(e1s.data(), e2s.data(), e3s.data(),
                                        weights.data(), sigmas.data(), true,
                                        aodf.data(), numEntries); }

  if (aodf.size() > 0)
  {
    FloatArrayType::Pointer aodfData = FloatArrayType::FromQVector(aodf, SIMPL::StringConstants::AxisOrientation);
    if (phaseType == SIMPL::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
      pp->setAxisOrientation(aodfData);
    }
    if (phaseType == SIMPL::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
      pp->setAxisOrientation(aodfData);
    }
    if (phaseType == SIMPL::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
      tp->setAxisOrientation(aodfData);
    }
    if (e1s.size() > 0)
    {
      FloatArrayType::Pointer euler1 = FloatArrayType::FromQVector(e1s, SIMPL::StringConstants::Euler1);
      FloatArrayType::Pointer euler2 = FloatArrayType::FromQVector(e2s, SIMPL::StringConstants::Euler2);
      FloatArrayType::Pointer euler3 = FloatArrayType::FromQVector(e3s, SIMPL::StringConstants::Euler3);
      FloatArrayType::Pointer sigma = FloatArrayType::FromQVector(sigmas, SIMPL::StringConstants::Sigma);
      FloatArrayType::Pointer weight = FloatArrayType::FromQVector(weights, SIMPL::StringConstants::Weight);

      VectorOfFloatArray aodfWeights;
      aodfWeights.push_back(euler1);
      aodfWeights.push_back(euler2);
      aodfWeights.push_back(euler3);
      aodfWeights.push_back(sigma);
      aodfWeights.push_back(weight);
      if (phaseType == SIMPL::PhaseType::PrimaryPhase)
      {
        PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
        pp->setAxisODF_Weights(aodfWeights);
      }
      if (phaseType == SIMPL::PhaseType::PrecipitatePhase)
      {
        PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
        pp->setAxisODF_Weights(aodfWeights);
      }
      if (phaseType == SIMPL::PhaseType::TransformationPhase)
      {
        TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
        tp->setAxisODF_Weights(aodfWeights);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<float> StatsGeneratorUtilities::GenerateODFData(unsigned int crystalStructure,
                                                 QVector<float> &e1s, QVector<float> &e2s,
                                                 QVector<float> &e3s, QVector<float> &weights,
                                                 QVector<float> &sigmas, bool computeODF)
{
  QVector<float> odf;

  size_t numEntries = static_cast<size_t>(e1s.size());

  if (Ebsd::CrystalStructure::Cubic_High == crystalStructure)
  {

    odf.resize(CubicOps::k_OdfSize);
    if (computeODF) { Texture::CalculateCubicODFData(e1s.data(), e2s.data(), e3s.data(),
                                   weights.data(), sigmas.data(), true,
                                   odf.data(), numEntries); }
  }
  else if (Ebsd::CrystalStructure::Hexagonal_High == crystalStructure)
  {
    odf.resize(HexagonalOps::k_OdfSize);
    if (computeODF) { Texture::CalculateHexODFData(e1s.data(), e2s.data(), e3s.data(),
                                 weights.data(), sigmas.data(), true,
                                 odf.data(), numEntries); }
  }
  return odf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUtilities::GenerateMisorientationBinData(StatsData* statsData,
                                                            unsigned int phaseType, unsigned int crystalStruct,
                                                            QVector<float> &odf,
                                                            QVector<float> &angles, QVector<float> &axes,
                                                            QVector<float> &weights, bool computeMDF)
{
  QVector<float> x;
  QVector<float> y;
  QVector<float> mdf;

  if (Ebsd::CrystalStructure::Cubic_High == crystalStruct)
  {
    mdf.resize(CubicOps::k_MdfSize);
    if (computeMDF) { Texture::CalculateMDFData<float, CubicOps>(angles.data(), axes.data(), weights.data(), odf.data(), mdf.data(), angles.size()); }
    //nElements = 18 * 18 * 18;
  }
  else if (Ebsd::CrystalStructure::Hexagonal_High == crystalStruct)
  {
    mdf.resize(HexagonalOps::k_MdfSize);
    if (computeMDF) { Texture::CalculateMDFData<float, HexagonalOps>(angles.data(), axes.data(), weights.data(), odf.data(), mdf.data(), angles.size()); }
    //nElements = 36 * 36 * 12;
  }
  if (mdf.size() > 0)
  {
    FloatArrayType::Pointer p = FloatArrayType::FromPointer(mdf.data(), mdf.size(), SIMPL::StringConstants::MisorientationBins);
    if (phaseType == SIMPL::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
      pp->setMisorientationBins(p);
    }
    if (phaseType == SIMPL::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
      pp->setMisorientationBins(p);
    }
    if (phaseType == SIMPL::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
      tp->setMisorientationBins(p);
    }

    if (angles.size() > 0)
    {
      FloatArrayType::Pointer anglesArray = FloatArrayType::FromPointer(angles.data(), angles.size(), SIMPL::StringConstants::Angle);
      FloatArrayType::Pointer axisArray = FloatArrayType::FromPointer(axes.data(), axes.size(), SIMPL::StringConstants::Axis);
      FloatArrayType::Pointer weightArray = FloatArrayType::FromPointer(weights.data(), weights.size(), SIMPL::StringConstants::Weight);

      VectorOfFloatArray mdfWeights;
      mdfWeights.push_back(anglesArray);
      mdfWeights.push_back(axisArray);
      mdfWeights.push_back(weightArray);
      if (phaseType == SIMPL::PhaseType::PrimaryPhase)
      {
        PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
        pp->setMDF_Weights(mdfWeights);
      }
      if (phaseType == SIMPL::PhaseType::PrecipitatePhase)
      {
        PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
        pp->setMDF_Weights(mdfWeights);
      }
      if (phaseType == SIMPL::PhaseType::TransformationPhase)
      {
        TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
        tp->setMDF_Weights(mdfWeights);
      }
    }
  }
}
