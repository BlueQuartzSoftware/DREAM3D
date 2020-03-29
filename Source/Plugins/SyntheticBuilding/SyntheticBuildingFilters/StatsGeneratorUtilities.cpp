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

#include "OrientationLib/LaueOps/CubicLowOps.h"
#include "OrientationLib/LaueOps/CubicOps.h"
#include "OrientationLib/LaueOps/HexagonalLowOps.h"
#include "OrientationLib/LaueOps/HexagonalOps.h"
#include "OrientationLib/LaueOps/MonoclinicOps.h"
#include "OrientationLib/LaueOps/OrthoRhombicOps.h"
#include "OrientationLib/LaueOps/TetragonalLowOps.h"
#include "OrientationLib/LaueOps/TetragonalOps.h"
#include "OrientationLib/LaueOps/TriclinicOps.h"
#include "OrientationLib/LaueOps/TrigonalLowOps.h"
#include "OrientationLib/LaueOps/TrigonalOps.h"
#include "OrientationLib/LaueOps/LaueOps.h"
#include "OrientationLib/Texture/StatsGen.hpp"
#include "OrientationLib/Texture/Texture.hpp"

#include "SIMPLib/StatsData/BoundaryStatsData.h"
#include "SIMPLib/StatsData/MatrixStatsData.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/StatsData.h"
#include "SIMPLib/StatsData/TransformationStatsData.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGeneratorUtilities::StatsGeneratorUtilities() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGeneratorUtilities::~StatsGeneratorUtilities() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUtilities::GenerateODFBinData(StatsData* statsData, PhaseType::Type phaseType, unsigned int crystalStructure, std::vector<float>& e1s, std::vector<float>& e2s,
                                                 std::vector<float>& e3s, std::vector<float>& weights, std::vector<float>& sigmas, bool computeODF)
{
  using ContainerType = std::vector<float>;

  ContainerType odf;
  size_t numEntries = e1s.size();
  //// ODF/MDF Update Codes
  if(computeODF)
  {
    LaueOps::Pointer ops = LaueOps::NullPointer();
    switch(crystalStructure)
    {
    case Ebsd::CrystalStructure::Triclinic: // 4; Triclinic -1
      Texture::CalculateODFData<float, TriclinicOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Monoclinic: // 5; Monoclinic 2/m
      Texture::CalculateODFData<float, MonoclinicOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::OrthoRhombic: // 6; Orthorhombic mmm
      Texture::CalculateODFData<float, OrthoRhombicOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Tetragonal_Low: // 7; Tetragonal-Low 4/m
      Texture::CalculateODFData<float, TetragonalLowOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Tetragonal_High: // 8; Tetragonal-High 4/mmm
      Texture::CalculateODFData<float, TetragonalOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Trigonal_Low: // 9; Trigonal-Low -3
      Texture::CalculateODFData<float, TrigonalLowOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Trigonal_High: // 10; Trigonal-High -3m
      Texture::CalculateODFData<float, TrigonalOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Hexagonal_Low: // 2; Hexagonal-Low 6/m
      Texture::CalculateODFData<float, HexagonalLowOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Hexagonal_High: // 0; Hexagonal-High 6/mmm
      Texture::CalculateODFData<float, HexagonalOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Cubic_Low: // 3; Cubic Cubic-Low m3 (Tetrahedral)
      Texture::CalculateODFData<float, CubicLowOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Cubic_High: // 1; Cubic Cubic-High m3m
      Texture::CalculateODFData<float, CubicOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    default:
      break;
    }
  }

  if(!odf.empty())
  {
    FloatArrayType::Pointer p = FloatArrayType::FromStdVector(odf, SIMPL::StringConstants::ODF);
    if(phaseType == PhaseType::Type::Primary)
    {
      PrimaryStatsData* pp = dynamic_cast<PrimaryStatsData*>(statsData);
      pp->setODF(p);
    }
    if(phaseType == PhaseType::Type::Precipitate)
    {
      PrecipitateStatsData* pp = dynamic_cast<PrecipitateStatsData*>(statsData);
      pp->setODF(p);
    }
    if(phaseType == PhaseType::Type::Transformation)
    {
      TransformationStatsData* tp = dynamic_cast<TransformationStatsData*>(statsData);
      tp->setODF(p);
    }
    if(!e1s.empty())
    {
      FloatArrayType::Pointer euler1 = FloatArrayType::FromStdVector(e1s, SIMPL::StringConstants::Euler1);
      FloatArrayType::Pointer euler2 = FloatArrayType::FromStdVector(e2s, SIMPL::StringConstants::Euler2);
      FloatArrayType::Pointer euler3 = FloatArrayType::FromStdVector(e3s, SIMPL::StringConstants::Euler3);
      FloatArrayType::Pointer sigma = FloatArrayType::FromStdVector(sigmas, SIMPL::StringConstants::Sigma);
      FloatArrayType::Pointer weight = FloatArrayType::FromStdVector(weights, SIMPL::StringConstants::Weight);

      VectorOfFloatArray odfWeights;
      odfWeights.push_back(euler1);
      odfWeights.push_back(euler2);
      odfWeights.push_back(euler3);
      odfWeights.push_back(sigma);
      odfWeights.push_back(weight);
      if(phaseType == PhaseType::Type::Primary)
      {
        PrimaryStatsData* pp = dynamic_cast<PrimaryStatsData*>(statsData);
        pp->setODF_Weights(odfWeights);
      }
      if(phaseType == PhaseType::Type::Precipitate)
      {
        PrecipitateStatsData* pp = dynamic_cast<PrecipitateStatsData*>(statsData);
        pp->setODF_Weights(odfWeights);
      }
      if(phaseType == PhaseType::Type::Transformation)
      {
        TransformationStatsData* tp = dynamic_cast<TransformationStatsData*>(statsData);
        tp->setODF_Weights(odfWeights);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUtilities::GenerateAxisODFBinData(StatsData* statsData, PhaseType::Type phaseType, std::vector<float>& e1s, std::vector<float>& e2s, std::vector<float>& e3s,
                                                     std::vector<float>& weights, std::vector<float>& sigmas, bool computeAxisODF)
{
  using ContainerType = std::vector<float>;
  ContainerType aodf;
  size_t numEntries = e1s.size();
  OrthoRhombicOps ops;
  aodf.resize(ops.getODFSize());
  if(computeAxisODF)
  {
    Texture::CalculateODFData<float, OrthoRhombicOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, aodf, numEntries);
  }

  if(!aodf.empty())
  {
    FloatArrayType::Pointer aodfData = FloatArrayType::FromStdVector(aodf, SIMPL::StringConstants::AxisOrientation);
    if(phaseType == PhaseType::Type::Primary)
    {
      PrimaryStatsData* pp = dynamic_cast<PrimaryStatsData*>(statsData);
      pp->setAxisOrientation(aodfData);
    }
    if(phaseType == PhaseType::Type::Precipitate)
    {
      PrecipitateStatsData* pp = dynamic_cast<PrecipitateStatsData*>(statsData);
      pp->setAxisOrientation(aodfData);
    }
    if(phaseType == PhaseType::Type::Transformation)
    {
      TransformationStatsData* tp = dynamic_cast<TransformationStatsData*>(statsData);
      tp->setAxisOrientation(aodfData);
    }
    if(!e1s.empty())
    {
      FloatArrayType::Pointer euler1 = FloatArrayType::FromStdVector(e1s, SIMPL::StringConstants::Euler1);
      FloatArrayType::Pointer euler2 = FloatArrayType::FromStdVector(e2s, SIMPL::StringConstants::Euler2);
      FloatArrayType::Pointer euler3 = FloatArrayType::FromStdVector(e3s, SIMPL::StringConstants::Euler3);
      FloatArrayType::Pointer sigma = FloatArrayType::FromStdVector(sigmas, SIMPL::StringConstants::Sigma);
      FloatArrayType::Pointer weight = FloatArrayType::FromStdVector(weights, SIMPL::StringConstants::Weight);

      VectorOfFloatArray aodfWeights;
      aodfWeights.push_back(euler1);
      aodfWeights.push_back(euler2);
      aodfWeights.push_back(euler3);
      aodfWeights.push_back(sigma);
      aodfWeights.push_back(weight);
      if(phaseType == PhaseType::Type::Primary)
      {
        PrimaryStatsData* pp = dynamic_cast<PrimaryStatsData*>(statsData);
        pp->setAxisODF_Weights(aodfWeights);
      }
      if(phaseType == PhaseType::Type::Precipitate)
      {
        PrecipitateStatsData* pp = dynamic_cast<PrecipitateStatsData*>(statsData);
        pp->setAxisODF_Weights(aodfWeights);
      }
      if(phaseType == PhaseType::Type::Transformation)
      {
        TransformationStatsData* tp = dynamic_cast<TransformationStatsData*>(statsData);
        tp->setAxisODF_Weights(aodfWeights);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<float> StatsGeneratorUtilities::GenerateODFData(unsigned int crystalStructure, std::vector<float>& e1s, std::vector<float>& e2s, std::vector<float>& e3s, std::vector<float>& weights,
                                                            std::vector<float>& sigmas, bool computeODF)
{

  using ContainerType = std::vector<float>;
  ContainerType odf;
  //// ODF/MDF Update Codes
  size_t numEntries = static_cast<size_t>(e1s.size());

  if(computeODF)
  {
    LaueOps::Pointer ops = LaueOps::NullPointer();
    switch(crystalStructure)
    {
    case Ebsd::CrystalStructure::Triclinic: // 4; Triclinic -1
      Texture::CalculateODFData<float, TriclinicOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Monoclinic: // 5; Monoclinic 2/m
      Texture::CalculateODFData<float, MonoclinicOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::OrthoRhombic: // 6; Orthorhombic mmm
      Texture::CalculateODFData<float, OrthoRhombicOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Tetragonal_Low: // 7; Tetragonal-Low 4/m
      Texture::CalculateODFData<float, TetragonalLowOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Tetragonal_High: // 8; Tetragonal-High 4/mmm
      Texture::CalculateODFData<float, TetragonalOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Trigonal_Low: // 9; Trigonal-Low -3
      Texture::CalculateODFData<float, TrigonalLowOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Trigonal_High: // 10; Trigonal-High -3m
      Texture::CalculateODFData<float, TrigonalOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Hexagonal_Low: // 2; Hexagonal-Low 6/m
      Texture::CalculateODFData<float, HexagonalLowOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Hexagonal_High: // 0; Hexagonal-High 6/mmm
      Texture::CalculateODFData<float, HexagonalOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Cubic_Low: // 3; Cubic Cubic-Low m3 (Tetrahedral)
      Texture::CalculateODFData<float, CubicLowOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    case Ebsd::CrystalStructure::Cubic_High: // 1; Cubic Cubic-High m3m
      Texture::CalculateODFData<float, CubicOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
      break;
    default:
      break;
    }
  }

  return odf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUtilities::GenerateMisorientationBinData(StatsData* statsData, PhaseType::Type phaseType, unsigned int crystalStructure, std::vector<float>& odf, std::vector<float>& angles,
                                                            std::vector<float>& axes, std::vector<float>& weights, bool computeMDF)
{
  using ContainerType = std::vector<float>;

  ContainerType x;
  ContainerType y;
  ContainerType mdf;
  //// ODF/MDF Update Codes
  if(computeMDF)
  {
    LaueOps::Pointer ops = LaueOps::NullPointer();
    switch(crystalStructure)
    {
    case Ebsd::CrystalStructure::Triclinic: // 4; Triclinic -1
      Texture::CalculateMDFData<float, TriclinicOps, ContainerType>(angles, axes, weights, odf, mdf, angles.size());
      break;
    case Ebsd::CrystalStructure::Monoclinic: // 5; Monoclinic 2/m
      Texture::CalculateMDFData<float, MonoclinicOps, ContainerType>(angles, axes, weights, odf, mdf, angles.size());
      break;
    case Ebsd::CrystalStructure::OrthoRhombic: // 6; Orthorhombic mmm
      Texture::CalculateMDFData<float, OrthoRhombicOps, ContainerType>(angles, axes, weights, odf, mdf, angles.size());
      break;
    case Ebsd::CrystalStructure::Tetragonal_Low: // 7; Tetragonal-Low 4/m
      Texture::CalculateMDFData<float, TetragonalLowOps, ContainerType>(angles, axes, weights, odf, mdf, angles.size());
      break;
    case Ebsd::CrystalStructure::Tetragonal_High: // 8; Tetragonal-High 4/mmm
      Texture::CalculateMDFData<float, TetragonalOps, ContainerType>(angles, axes, weights, odf, mdf, angles.size());
      break;
    case Ebsd::CrystalStructure::Trigonal_Low: // 9; Trigonal-Low -3
      Texture::CalculateMDFData<float, TrigonalLowOps, ContainerType>(angles, axes, weights, odf, mdf, angles.size());
      break;
    case Ebsd::CrystalStructure::Trigonal_High: // 10; Trigonal-High -3m
      Texture::CalculateMDFData<float, TrigonalOps, ContainerType>(angles, axes, weights, odf, mdf, angles.size());
      break;
    case Ebsd::CrystalStructure::Hexagonal_Low: // 2; Hexagonal-Low 6/m
      Texture::CalculateMDFData<float, HexagonalLowOps, ContainerType>(angles, axes, weights, odf, mdf, angles.size());
      break;
    case Ebsd::CrystalStructure::Hexagonal_High: // 0; Hexagonal-High 6/mmm
      Texture::CalculateMDFData<float, HexagonalOps, ContainerType>(angles, axes, weights, odf, mdf, angles.size());
      break;
    case Ebsd::CrystalStructure::Cubic_Low: // 3; Cubic Cubic-Low m3 (Tetrahedral)
      Texture::CalculateMDFData<float, CubicLowOps, ContainerType>(angles, axes, weights, odf, mdf, angles.size());
      break;
    case Ebsd::CrystalStructure::Cubic_High: // 1; Cubic Cubic-High m3m
      Texture::CalculateMDFData<float, CubicOps, ContainerType>(angles, axes, weights, odf, mdf, angles.size());
      break;
    default:
      break;
    }
  }

  if(!mdf.empty())
  {
    FloatArrayType::Pointer p = FloatArrayType::CopyFromPointer(mdf.data(), mdf.size(), SIMPL::StringConstants::MisorientationBins);
    if(phaseType == PhaseType::Type::Primary)
    {
      PrimaryStatsData* pp = dynamic_cast<PrimaryStatsData*>(statsData);
      pp->setMisorientationBins(p);
    }
    if(phaseType == PhaseType::Type::Precipitate)
    {
      PrecipitateStatsData* pp = dynamic_cast<PrecipitateStatsData*>(statsData);
      pp->setMisorientationBins(p);
    }
    if(phaseType == PhaseType::Type::Transformation)
    {
      TransformationStatsData* tp = dynamic_cast<TransformationStatsData*>(statsData);
      tp->setMisorientationBins(p);
    }

    if(!angles.empty())
    {
      FloatArrayType::Pointer anglesArray = FloatArrayType::CopyFromPointer(angles.data(), angles.size(), SIMPL::StringConstants::Angle);
      FloatArrayType::Pointer axisArray = FloatArrayType::CopyFromPointer(axes.data(), axes.size(), SIMPL::StringConstants::Axis);
      FloatArrayType::Pointer weightArray = FloatArrayType::CopyFromPointer(weights.data(), weights.size(), SIMPL::StringConstants::Weight);

      VectorOfFloatArray mdfWeights;
      mdfWeights.push_back(anglesArray);
      mdfWeights.push_back(axisArray);
      mdfWeights.push_back(weightArray);
      if(phaseType == PhaseType::Type::Primary)
      {
        PrimaryStatsData* pp = dynamic_cast<PrimaryStatsData*>(statsData);
        pp->setMDF_Weights(mdfWeights);
      }
      if(phaseType == PhaseType::Type::Precipitate)
      {
        PrecipitateStatsData* pp = dynamic_cast<PrecipitateStatsData*>(statsData);
        pp->setMDF_Weights(mdfWeights);
      }
      if(phaseType == PhaseType::Type::Transformation)
      {
        TransformationStatsData* tp = dynamic_cast<TransformationStatsData*>(statsData);
        tp->setMDF_Weights(mdfWeights);
      }
    }
  }
}
