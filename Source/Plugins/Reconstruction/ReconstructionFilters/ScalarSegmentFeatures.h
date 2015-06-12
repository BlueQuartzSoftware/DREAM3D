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


#ifndef _ScalarSegmentFeatures_H_
#define _ScalarSegmentFeatures_H_

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "Reconstruction/ReconstructionFilters/SegmentFeatures.h"

class CompareFunctor;

/**
 * @brief The ScalarSegmentFeatures class. See [Filter documentation](@ref scalarsegmentfeatures) for details.
 */
class ScalarSegmentFeatures : public SegmentFeatures
{
    Q_OBJECT
  public:
    typedef boost::uniform_int<int64_t> NumberDistribution;
    typedef boost::mt19937 RandomNumberGenerator;
    typedef boost::variate_generator<RandomNumberGenerator&, NumberDistribution> Generator;

    DREAM3D_SHARED_POINTERS(ScalarSegmentFeatures)
    DREAM3D_STATIC_NEW_MACRO(ScalarSegmentFeatures)
    DREAM3D_TYPE_MACRO_SUPER(ScalarSegmentFeatures, AbstractFilter)

    virtual ~ScalarSegmentFeatures();

    DREAM3D_FILTER_PARAMETER(QString, CellFeatureAttributeMatrixName)
    Q_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, ScalarArrayPath)
    Q_PROPERTY(DataArrayPath ScalarArrayPath READ getScalarArrayPath WRITE setScalarArrayPath)

    DREAM3D_FILTER_PARAMETER(float, ScalarTolerance)
    Q_PROPERTY(float ScalarTolerance READ getScalarTolerance WRITE setScalarTolerance)

    DREAM3D_INSTANCE_PROPERTY(bool, RandomizeFeatureIds)

    DREAM3D_FILTER_PARAMETER(bool, UseGoodVoxels)
    Q_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, GoodVoxelsArrayPath)
    Q_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, FeatureIdsArrayName)
    Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, ActiveArrayName)
    Q_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();
    virtual const QString getHumanLabel();

    virtual void setupFilterParameters();
    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
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


  protected:
    ScalarSegmentFeatures();

    /**
     * @brief getSeed Reimplemented from @see SegmentFeatures class
     */
    virtual int64_t getSeed(int32_t gnum);

    /**
     * @brief determineGrouping Reimplemented from @see SegmentFeatures class
     */
    virtual bool determineGrouping(int64_t referencepoint, int64_t neighborpoint, int32_t gnum);

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(bool, GoodVoxels)
    DEFINE_REQUIRED_IDATAARRAY_VARIABLE(InputData)

    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_CREATED_DATAARRAY_VARIABLE(bool, Active)

    boost::shared_ptr<CompareFunctor> m_Compare;

    boost::shared_ptr<NumberDistribution> m_Distribution;
    boost::shared_ptr<RandomNumberGenerator> m_RandomNumberGenerator;
    boost::shared_ptr<Generator> m_NumberGenerator;
    size_t                       m_TotalRandomNumbersGenerated;

    /**
     * @brief randomizeGrainIds Randomizes Feature Ids
     * @param totalPoints Size of Feature Ids array to randomize
     * @param totalFeatures Number of Features
     */
    void randomizeFeatureIds(int64_t totalPoints, int64_t totalFeatures);

    /**
     * @brief initializeVoxelSeedGenerator Initializes the boost random number generators
     * @param rangeMin Minimum range for random number selection
     * @param rangeMax Maximum range for random number selection
     */
    void initializeVoxelSeedGenerator(const int64_t rangeMin, const int64_t rangeMax);

    void dataCheck();
    void updateFeatureInstancePointers();

    ScalarSegmentFeatures(const ScalarSegmentFeatures&); // Copy Constructor Not Implemented
    void operator=(const ScalarSegmentFeatures&); // Operator '=' Not Implemented
};

#endif /* ScalarSegmentFeatures_H_ */