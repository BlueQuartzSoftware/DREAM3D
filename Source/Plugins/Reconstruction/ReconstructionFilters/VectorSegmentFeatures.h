/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef _VectorSegmentFeatures_H_
#define _VectorSegmentFeatures_H_

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

#include "Reconstruction/ReconstructionFilters/SegmentFeatures.h"
#include "Reconstruction/ReconstructionConstants.h"


class CompareFunctor;

/**
 * @class VectorSegmentFeatures VectorSegmentFeatures.h DREAM3DLib/ReconstructionFilters/VectorSegmentFeatures.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class VectorSegmentFeatures : public SegmentFeatures
{
    Q_OBJECT
  public:
    DREAM3D_SHARED_POINTERS(VectorSegmentFeatures)
    DREAM3D_STATIC_NEW_MACRO(VectorSegmentFeatures)
    DREAM3D_TYPE_MACRO_SUPER(VectorSegmentFeatures, AbstractFilter)

    virtual ~VectorSegmentFeatures();
    DREAM3D_FILTER_PARAMETER(QString, CellFeatureAttributeMatrixName)
    Q_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SelectedVectorArrayPath)
    Q_PROPERTY(DataArrayPath SelectedVectorArrayPath READ getSelectedVectorArrayPath WRITE setSelectedVectorArrayPath)

    DREAM3D_FILTER_PARAMETER(float, AngleTolerance)
    Q_PROPERTY(float AngleTolerance READ getAngleTolerance WRITE setAngleTolerance)

    DREAM3D_INSTANCE_PROPERTY(bool, RandomizeFeatureIds)

    DREAM3D_FILTER_PARAMETER(bool, UseGoodVoxels)
    Q_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, GoodVoxelsArrayPath)
    Q_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, FeatureIdsArrayName)
    Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

    DREAM3D_FILTER_PARAMETER(QString, ActiveArrayName)
    Q_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)

    virtual const QString getCompiledLibraryName() { return Reconstruction::ReconstructionBaseName; }
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::ReconstructionFilters; }
    virtual const QString getSubGroupName() {return DREAM3D::FilterSubGroups::SegmentationFilters;}
    virtual const QString getHumanLabel() { return "Segment Features (Vector)"; }
    virtual const QString getBrandingString() { return "DREAM3D Reconstruction Plugin"; }

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
    VectorSegmentFeatures();

    virtual int64_t getSeed(size_t gnum);
    virtual bool determineGrouping(int64_t referencepoint, int64_t neighborpoint, size_t gnum);

  private:
    IDataArray::Pointer m_InputData;

    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, Vectors)
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(bool, GoodVoxels)
    DEFINE_CREATED_DATAARRAY_VARIABLE(bool, Active)

    ///Boost Random Number generator stuff. We use the boost::shared_ptr to ensure the pointers are cleaned up when the
    ///filter is deleted
    boost::shared_ptr<NumberDistribution> m_Distribution;
    boost::shared_ptr<RandomNumberGenerator> m_RandomNumberGenerator;
    boost::shared_ptr<Generator> m_NumberGenerator;
    size_t                       m_TotalRandomNumbersGenerated;

    float angleTolerance;

    /**
     * @brief randomizeGrainIds
     * @param totalPoints
     * @param totalFields
     */
    void randomizeFeatureIds(int64_t totalPoints, size_t totalFeatures);

    /**
     * @brief initializeVoxelSeedGenerator
     * @param totalPoints
     */
    void initializeVoxelSeedGenerator(const size_t rangeMin, const size_t rangeMax);

    void dataCheck();
    void updateFeatureInstancePointers();

    bool missingGoodVoxels;

    VectorSegmentFeatures(const VectorSegmentFeatures&); // Copy Constructor Not Implemented
    void operator=(const VectorSegmentFeatures&); // Operator '=' Not Implemented
};

#endif /* VectorSegmentFeatures_H_ */


