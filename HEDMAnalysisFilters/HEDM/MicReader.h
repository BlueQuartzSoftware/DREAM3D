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
#ifndef _MicREADER_H_
#define _MicREADER_H_

#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QFile>

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdSetGetMacros.h"
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/EbsdReader.h"


#include "MicConstants.h"
#include "MicHeaderEntry.h"
#include "MicPhase.h"



/**
* @class MicReader MicReader.h EbsdLib/HEDM/MicReader.h
* @brief This class is a self contained HEDM OIM .Mic file reader and will read a
* single .Mic file and store all the data in column centric pointers.
* @author Michael A. Jackson for BlueQuartz Software
* @date Mar 1, 2010
* @version 1.0
*/
class MicReader : public EbsdReader
{
  public:
    MicReader();
    virtual ~MicReader();

    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, InfileBasename, Mic::InfileBasename)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, InfileSerialLength, Mic::InfileSerialLength)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, OutfileBasename, Mic::OutfileBasename)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, OutfileSerialLength, Mic::OutfileSerialLength)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, OutStructureBasename, Mic::OutStructureBasename)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, BCPeakDetectorOffset, Mic::BCPeakDetectorOffset)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, InFileType, Mic::InFileType)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, OutfileExtension, Mic::OutfileExtension)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, InfileExtesnion, Mic::InfileExtesnion)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, BeamEnergyWidth, Mic::BeamEnergyWidth)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, BeamDirection, Mic::BeamDirection)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, BeamDeflectionChiLaue, Mic::BeamDeflectionChiLaue)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, BeamHeight, Mic::BeamHeight)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, BeamEnergy, Mic::BeamEnergy)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, DetectorFilename, Mic::DetectorFilename)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, OptimizationConstrainFilename, Mic::OptimizationConstrainFilename)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, EtaLimit, Mic::EtaLimit)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, SampleFilename, Mic::SampleFilename)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, StructureFilename, Mic::StructureFilename)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, RotationRangeFilename, Mic::RotationRangeFilename)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, FundamentalZoneFilename, Mic::FundamentalZoneFilename)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, SampleSymmetry, Mic::SampleSymmetry)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, MinAmplitudeFraction, Mic::MinAmplitudeFraction)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, MaxQ, Mic::MaxQ)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, MaxInitSideLength, Mic::MaxInitSideLength)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, MinSideLength, Mic::MinSideLength)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, LocalOrientationGridRadius, Mic::LocalOrientationGridRadius)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, MinLocalResolution, Mic::MinLocalResolution)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, MaxLocalResolution, Mic::MaxLocalResolution)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, MaxAcceptedCost, Mic::MaxAcceptedCost)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, MaxConvergenceCost, Mic::MaxConvergenceCost)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, MaxMCSteps, Mic::MaxMCSteps)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, MCRadiusScaleFactor, Mic::MCRadiusScaleFactor)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, SuccessiveRestarts, Mic::SuccessiveRestarts)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, SecondsBetweenSave, Mic::SecondsBetweenSave)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, NumParameterOptimizationSteps, Mic::NumParameterOptimizationSteps)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, NumElementToOptimizePerPE, Mic::NumElementToOptimizePerPE)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, OptimizationFilename, Mic::OptimizationFilename)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, DetectionLimitFilename, Mic::DetectionLimitFilename)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, ParameterMCInitTemperature, Mic::ParameterMCInitTemperature)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, OrientationSearchMethod, Mic::OrientationSearchMethod)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, CoolingFraction, Mic::CoolingFraction)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, ThermalizeFraction, Mic::ThermalizeFraction)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, ParameterRefinements, Mic::ParameterRefinements)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, NumDetectors, Mic::NumDetectors)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, DetectorSpacing, Mic::DetectorSpacing)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, DetectorSpacingDeviation, Mic::DetectorSpacingDeviation)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, DetectorOrientationDeviationInEuler, Mic::DetectorOrientationDeviationInEuler)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, DetectorOrientationDeviationInSO3, Mic::DetectorOrientationDeviationInSO3)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, ParamMCMaxLocalRestarts, Mic::ParamMCMaxLocalRestarts)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, ParamMCMaxGlobalRestarts, Mic::ParamMCMaxGlobalRestarts)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, ParamMCNumGlobalSearchElements, Mic::ParamMCNumGlobalSearchElements)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, ConstrainedOptimization, Mic::ConstrainedOptimization)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, SearchVolumeReductionFactor, Mic::SearchVolumeReductionFactor)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, FileNumStart, Mic::FileNumStart)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, FileNumEnd, Mic::FileNumEnd)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, SampleLocation, Mic::SampleLocation)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, SampleOrientation, Mic::SampleOrientation)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, EnableStrain, Mic::EnableStrain)
    EbsdHeader_INSTANCE_PROPERTY(MicStringHeaderEntry, QString, SampleCenter, Mic::SampleCenter)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, SampleRadius, Mic::SampleRadius)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, MaxDeepeningHitRatio, Mic::MaxDeepeningHitRatio)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, ConsistencyError, Mic::ConsistencyError)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, BraggFilterTolerance, Mic::BraggFilterTolerance)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, MinAccelerationThreshold, Mic::MinAccelerationThreshold)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, MaxDiscreteCandidates, Mic::MaxDiscreteCandidates)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, XDim, Mic::XDim)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<int>, int, YDim, Mic::YDim)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, XRes, Mic::XRes)
    EbsdHeader_INSTANCE_PROPERTY(MicHeaderEntry<float>, float, YRes, Mic::YRes)

    EBSD_INSTANCE_PROPERTY(QVector<MicPhase::Pointer>, PhaseVector)

    EBSD_POINTER_PROPERTY(Euler1, Euler1, float)
    EBSD_POINTER_PROPERTY(Euler2, Euler2, float)
    EBSD_POINTER_PROPERTY(Euler3, Euler3, float)
    EBSD_POINTER_PROPERTY(X, X, float)
    EBSD_POINTER_PROPERTY(Y, Y, float)
    EBSD_POINTER_PROPERTY(Confidence, Conf, float)
    EBSD_POINTER_PROPERTY(Phase, Phase, int)
    EBSD_POINTER_PROPERTY(Level, Level, int)
    EBSD_POINTER_PROPERTY(Up, Up, int)

    /**
     * @brief Returns the pointer to the data for a given feature
     * @param featureName The name of the feature to return the pointer to.
     */
    void* getPointerByName(const QString& featureName);

    /**
     * @brief Returns an enumeration value that depicts the numerical
     * primitive type that the data is stored as (Int, Float, etc).
     * @param featureName The name of the feature.
     */
    Ebsd::NumType getPointerType(const QString& featureName);

    /**
    * @brief Reads the complete HEDM .Mic file.
    * @return 1 on success
    */
    virtual int readFile();

    /**
    * @brief Reads ONLY the header portion of the HEDM .Mic file
    * @return 1 on success
    */
    virtual int readHeaderOnly();

    /** @brief Allocates the proper amount of memory (after reading the header portion of the file)
    * and then splats '0' across all the bytes of the memory allocation
    */
    virtual void initPointers(size_t numElements);

    /** @brief 'free's the allocated memory and sets the pointer to NULL
    */
    virtual void deletePointers();

    virtual int getXDimension();
    virtual int getYDimension();
    virtual void setXDimension(int xD);
    virtual void setYDimension(int yD);
    virtual float getXStep();
    virtual float getYStep();

  protected:


  private:
    MicPhase::Pointer   m_CurrentPhase;

    int xDim;
    int yDim;
    float xRes;
    float yRes;

    int readMicFile();

    int readDatFile();

    /** @brief Parses the value from a single line of the header section of the HEDM .Mic file
    * @param line The line to parse
    */
    void parseHeaderLine(QByteArray& line);

    /** @brief Parses the data from a line of data from the HEDM .Mic file
      * @param line The line of data to parse
      */
    void parseDataLine(QByteArray& line, size_t i);

    MicReader(const MicReader&);    // Copy Constructor Not Implemented
    void operator=(const MicReader&);  // Operator '=' Not Implemented

};

#endif /* _MicREADER_H_ */

