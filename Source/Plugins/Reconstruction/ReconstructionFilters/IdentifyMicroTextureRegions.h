/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#ifndef _IdentifyMicroTextureRegions_H_
#define _IdentifyMicroTextureRegions_H_

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

/**
 * @brief The IdentifyMicroTextureRegions class. See [Filter documentation](@ref identifymicrotextureregions) for details.
 */
class IdentifyMicroTextureRegions : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    typedef boost::uniform_int<int32_t> NumberDistribution;
    typedef boost::mt19937 RandomNumberGenerator;
    typedef boost::variate_generator<RandomNumberGenerator&, NumberDistribution> Generator;

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
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, CAxisLocations)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)

    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, MTRIds)
    DEFINE_CREATED_DATAARRAY_VARIABLE(bool, Active)
    DEFINE_CREATED_DATAARRAY_VARIABLE(bool, InMTR)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, VolFrac)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, AvgCAxis)
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, PatchIds)
    DEFINE_CREATED_DATAARRAY_VARIABLE(bool, PatchActive)

    boost::shared_ptr<NumberDistribution> m_Distribution;
    boost::shared_ptr<RandomNumberGenerator> m_RandomNumberGenerator;
    boost::shared_ptr<Generator> m_NumberGenerator;
    size_t                       m_TotalRandomNumbersGenerated;

    float caxisTolerance;

    /**
     * @brief randomizeGrainIds Randomizes Feature Ids
     * @param totalPoints Size of Feature Ids array to randomize
     * @param totalFeatures Number of Features
     */
    void randomizeFeatureIds(int64_t totalPoints, int64_t totalFeatures);

    /**
     * @brief findMTRregions Determines the existing microtexture zones; CURRENTLY
     * NOT IMPLEMENTED
     */
    void findMTRregions();

    /**
     * @brief initializeVoxelSeedGenerator Initializes the boost random number generators
     * @param rangeMin Minimum range for random number selection
     * @param rangeMax Maximum range for random number selection
     */
    void initializeVoxelSeedGenerator(const int32_t rangeMin, const int32_t rangeMax);

    void dataCheck();
    void updateFeatureInstancePointers();

    IdentifyMicroTextureRegions(const IdentifyMicroTextureRegions&); // Copy Constructor Not Implemented
    void operator=(const IdentifyMicroTextureRegions&); // Operator '=' Not Implemented
};

#endif /* IdentifyMicroTextureRegions_H_ */
