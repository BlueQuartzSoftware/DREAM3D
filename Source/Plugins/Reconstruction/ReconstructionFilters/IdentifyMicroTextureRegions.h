/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *                   FA8650-07-D-5800 and FA8650-10-D-5226
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _IdentifyMicroTextureRegions_H_
#define _IdentifyMicroTextureRegions_H_

#include <vector>
#include <QtCore/QString>

///Boost Random Number generator stuff
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

typedef boost::uniform_int<int> NumberDistribution;
typedef boost::mt19937 RandomNumberGenerator;
typedef boost::variate_generator<RandomNumberGenerator&, NumberDistribution> Generator;

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "Reconstruction/ReconstructionConstants.h"

/**
 * @class IdentifyMicroTextureRegions IdentifyMicroTextureRegions.h Plugins/Reconstruction/ReconstructionFilters/IdentifyMicroTextureRegions.h
 * @brief
 * @author Michael A Groeber (AFRL) & Joseph C Tucker (UES)
 * @date Mar 25, 2014
 * @version 5.0
 */
class IdentifyMicroTextureRegions : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(IdentifyMicroTextureRegions)
    DREAM3D_STATIC_NEW_MACRO(IdentifyMicroTextureRegions)
    DREAM3D_TYPE_MACRO_SUPER(IdentifyMicroTextureRegions, AbstractFilter)

    virtual ~IdentifyMicroTextureRegions();
    DREAM3D_FILTER_PARAMETER(QString, NewCellFeatureAttributeMatrixName)
    Q_PROPERTY(QString NewCellFeatureAttributeMatrixName READ getNewCellFeatureAttributeMatrixName WRITE setNewCellFeatureAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(float, CAxisTolerance)
    Q_PROPERTY(float CAxisTolerance READ getCAxisTolerance WRITE setCAxisTolerance)
    DREAM3D_FILTER_PARAMETER(float, MinMTRSize)
    Q_PROPERTY(float MinMTRSize READ getMinMTRSize WRITE setMinMTRSize)
    DREAM3D_FILTER_PARAMETER(float, MinVolFrac)
    Q_PROPERTY(float MinVolFrac READ getMinVolFrac WRITE setMinVolFrac)

    DREAM3D_INSTANCE_PROPERTY(bool, RandomizeMTRIds)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CAxisLocationsArrayPath)
    Q_PROPERTY(DataArrayPath CAxisLocationsArrayPath READ getCAxisLocationsArrayPath WRITE setCAxisLocationsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
    Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
    Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, MTRIdsArrayName)
    Q_PROPERTY(QString MTRIdsArrayName READ getMTRIdsArrayName WRITE setMTRIdsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, ActiveArrayName)
    Q_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();
    virtual const QString getBrandingString() { return "DREAM3D Reconstruction Plugin"; }

    /**
     * @brief setupFilterParameters
     */
    virtual void setupFilterParameters();

    /**
     * @brief writeFilterParameters
     * @param writer
     * @param index
     * @return
     */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    IdentifyMicroTextureRegions();

  private:
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, MTRIds)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, CAxisLocations)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_CREATED_DATAARRAY_VARIABLE(bool, Active)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)

    //These are temp arrays only stored during this filter, but we are using the DC structure to manage them in this filter.
    DEFINE_CREATED_DATAARRAY_VARIABLE(bool, InMTR)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, VolFrac)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, AvgCAxis)
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, PatchIds)
    DEFINE_CREATED_DATAARRAY_VARIABLE(bool, PatchActive)

    float caxisTolerance;

    ///Boost Random Number generator stuff. We use the boost::shared_ptr to ensure the pointers are cleaned up when the
    ///filter is deleted
    boost::shared_ptr<NumberDistribution> m_Distribution;
    boost::shared_ptr<RandomNumberGenerator> m_RandomNumberGenerator;
    boost::shared_ptr<Generator> m_NumberGenerator;
    size_t                       m_TotalRandomNumbersGenerated;

    /**
     * @brief randomizeGrainIds
     * @param totalPoints
     * @param totalFields
     */
    void randomizeFeatureIds(int64_t totalPoints, size_t totalFeatures);

    void findMTRregions();

    /**
     * @brief initializeVoxelSeedGenerator
     * @param totalPoints
     */
    void initializeVoxelSeedGenerator(const size_t rangeMin, const size_t rangeMax);

    void dataCheck();
    void updateFeatureInstancePointers();

    IdentifyMicroTextureRegions(const IdentifyMicroTextureRegions&); // Copy Constructor Not Implemented
    void operator=(const IdentifyMicroTextureRegions&); // Operator '=' Not Implemented
};

#endif /* IdentifyMicroTextureRegions_H_ */



