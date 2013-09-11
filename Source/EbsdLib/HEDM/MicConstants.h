/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */




#ifndef MicCONSTANTS_H_
#define MicCONSTANTS_H_

#include <string>
#include <vector>

#define DECLARE_STRING_CONST(var)\
  const QString var(#var);


/**
 * @file MicConstants.h
 * @brief This file contains both c style macro definitions and C++ style const
 * declarations of HEDM OIM variables and names. If you are dealing with a .Mic
 * file from HEDM instrumentation then you MUST use the defines from this file.
 */

// -----------------------------------------------------------------------------
//  These defines are needed for the pure "C" parser
// -----------------------------------------------------------------------------


namespace Ebsd {

 namespace Mic
 {
   const QString Manufacturer("HEDM");

  DECLARE_STRING_CONST(Cubic)
  DECLARE_STRING_CONST(Hexagonal)
  DECLARE_STRING_CONST(OrthoRhombic)
  DECLARE_STRING_CONST(Tetragonal)

   const QString FileExt("mic");

  DECLARE_STRING_CONST(InfileBasename)
  DECLARE_STRING_CONST(InfileSerialLength)
  DECLARE_STRING_CONST(OutfileBasename)
  DECLARE_STRING_CONST(OutfileSerialLength)
  DECLARE_STRING_CONST(OutStructureBasename)
  DECLARE_STRING_CONST(BCPeakDetectorOffset)
  DECLARE_STRING_CONST(InFileType)
  DECLARE_STRING_CONST(OutfileExtension)
  DECLARE_STRING_CONST(InfileExtesnion)
  DECLARE_STRING_CONST(BeamEnergyWidth)
  DECLARE_STRING_CONST(BeamDirection)
  DECLARE_STRING_CONST(BeamDeflectionChiLaue)
  DECLARE_STRING_CONST(BeamHeight)
  DECLARE_STRING_CONST(BeamEnergy)
  DECLARE_STRING_CONST(DetectorFilename)
  DECLARE_STRING_CONST(OptimizationConstrainFilename)
  DECLARE_STRING_CONST(EtaLimit)
  DECLARE_STRING_CONST(SampleFilename)
  DECLARE_STRING_CONST(StructureFilename)
  DECLARE_STRING_CONST(RotationRangeFilename)
  DECLARE_STRING_CONST(FundamentalZoneFilename)
  DECLARE_STRING_CONST(SampleSymmetry)
  DECLARE_STRING_CONST(MinAmplitudeFraction)
  DECLARE_STRING_CONST(MaxQ)
  DECLARE_STRING_CONST(MaxInitSideLength)
  DECLARE_STRING_CONST(MinSideLength)
  DECLARE_STRING_CONST(LocalOrientationGridRadius)
  DECLARE_STRING_CONST(MinLocalResolution)
  DECLARE_STRING_CONST(MaxLocalResolution)
  DECLARE_STRING_CONST(MaxAcceptedCost)
  DECLARE_STRING_CONST(MaxConvergenceCost)
  DECLARE_STRING_CONST(MaxMCSteps)
  DECLARE_STRING_CONST(MCRadiusScaleFactor)
  DECLARE_STRING_CONST(SuccessiveRestarts)
  DECLARE_STRING_CONST(SecondsBetweenSave)
  DECLARE_STRING_CONST(NumParameterOptimizationSteps)
  DECLARE_STRING_CONST(NumElementToOptimizePerPE)
  DECLARE_STRING_CONST(OptimizationFilename)
  DECLARE_STRING_CONST(DetectionLimitFilename)
  DECLARE_STRING_CONST(ParameterMCInitTemperature)
  DECLARE_STRING_CONST(OrientationSearchMethod)
  DECLARE_STRING_CONST(CoolingFraction)
  DECLARE_STRING_CONST(ThermalizeFraction)
  DECLARE_STRING_CONST(ParameterRefinements)
  DECLARE_STRING_CONST(NumDetectors)
  DECLARE_STRING_CONST(DetectorSpacing)
  DECLARE_STRING_CONST(DetectorSpacingDeviation)
  DECLARE_STRING_CONST(DetectorOrientationDeviationInEuler)
  DECLARE_STRING_CONST(DetectorOrientationDeviationInSO3)
  DECLARE_STRING_CONST(ParamMCMaxLocalRestarts)
  DECLARE_STRING_CONST(ParamMCMaxGlobalRestarts)
  DECLARE_STRING_CONST(ParamMCNumGlobalSearchElements)
  DECLARE_STRING_CONST(ConstrainedOptimization)
  DECLARE_STRING_CONST(SearchVolumeReductionFactor)
  DECLARE_STRING_CONST(FileNumStart)
  DECLARE_STRING_CONST(FileNumEnd)
  DECLARE_STRING_CONST(SampleLocation)
  DECLARE_STRING_CONST(SampleOrientation)
  DECLARE_STRING_CONST(EnableStrain)
  DECLARE_STRING_CONST(SampleCenter)
  DECLARE_STRING_CONST(SampleRadius)
  DECLARE_STRING_CONST(MaxDeepeningHitRatio)
  DECLARE_STRING_CONST(ConsistencyError)
  DECLARE_STRING_CONST(BraggFilterTolerance)
  DECLARE_STRING_CONST(MinAccelerationThreshold)
  DECLARE_STRING_CONST(MaxDiscreteCandidates)

  DECLARE_STRING_CONST(LatticeConstants)
  DECLARE_STRING_CONST(LatticeAngles)
  DECLARE_STRING_CONST(BasisAtoms)
  DECLARE_STRING_CONST(Symmetry)
  DECLARE_STRING_CONST(ZandCoordinates)

  DECLARE_STRING_CONST(XDim)
  DECLARE_STRING_CONST(YDim)
  DECLARE_STRING_CONST(XRes)
  DECLARE_STRING_CONST(YRes)
  DECLARE_STRING_CONST(PhaseName)

   const QString Euler1("Euler1");
   const QString Euler2("Euler2");
   const QString Euler3("Euler3");
   const QString X("X");
   const QString Y("Y");
   const QString Confidence("Confidence");
   const QString Phase("Phase");
   const QString Level("Level");
   const QString Up("Up");

//   const QString ZStep(Mic_Z_STEP);
//   const QString ZPos(Mic_Z_POS);
//   const QString ZMax(Mic_Z_MAX);

 }
}


#endif /* MicCONSTANTS_H_ */
