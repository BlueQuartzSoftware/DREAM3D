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
#ifndef _caxissegmentfeatures_h_
#define _caxissegmentfeatures_h_

#include <random>

#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationLib/LaueOps/LaueOps.h"

#include "Reconstruction/ReconstructionFilters/SegmentFeatures.h"

/**
 * @brief The CAxisSegmentFeatures class. See [Filter documentation](@ref caxissegmentfeatures) for details.
 */
class CAxisSegmentFeatures : public SegmentFeatures
{
  Q_OBJECT
public:
  SIMPL_SHARED_POINTERS(CAxisSegmentFeatures)
  SIMPL_STATIC_NEW_MACRO(CAxisSegmentFeatures)
  SIMPL_TYPE_MACRO_SUPER(CAxisSegmentFeatures, SegmentFeatures)

  virtual ~CAxisSegmentFeatures();

  SIMPL_FILTER_PARAMETER(QString, CellFeatureAttributeMatrixName)
  Q_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(float, MisorientationTolerance)
  Q_PROPERTY(float MisorientationTolerance READ getMisorientationTolerance WRITE setMisorientationTolerance)

  SIMPL_INSTANCE_PROPERTY(bool, RandomizeFeatureIds)

  SIMPL_FILTER_PARAMETER(bool, UseGoodVoxels)
  Q_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
  Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, QuatsArrayPath)
  Q_PROPERTY(DataArrayPath QuatsArrayPath READ getQuatsArrayPath WRITE setQuatsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, GoodVoxelsArrayPath)
  Q_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)

  SIMPL_FILTER_PARAMETER(QString, FeatureIdsArrayName)
  Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

  SIMPL_FILTER_PARAMETER(QString, ActiveArrayName)
  Q_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  virtual const QString getCompiledLibraryName();

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  virtual const QString getBrandingString();

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  virtual const QString getFilterVersion();

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  virtual const QString getGroupName();

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  virtual const QString getSubGroupName();

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  virtual const QString getHumanLabel();

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  virtual void setupFilterParameters();

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  virtual void execute();

  /**
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  virtual void preflight();

protected:
  CAxisSegmentFeatures();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief getSeed Reimplemented from @see SegmentFeatures class
   */
  virtual int64_t getSeed(int32_t gnum, int64_t nextSeed);

  /**
   * @brief determineGrouping Reimplemented from @see SegmentFeatures class
   */
  virtual bool determineGrouping(int64_t referencepoint, int64_t neighborpoint, int32_t gnum);

private:
  QVector<LaueOps::Pointer> m_OrientationOps;

  DEFINE_DATAARRAY_VARIABLE(float, Quats)
  DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
  DEFINE_DATAARRAY_VARIABLE(bool, GoodVoxels)
  DEFINE_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)

  DEFINE_DATAARRAY_VARIABLE(bool, Active)
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)

  std::random_device m_RandomDevice;
  std::mt19937_64 m_Generator;
  std::uniform_int_distribution<int64_t> m_Distribution;

  float m_MisoTolerance;

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

  /**
   * @brief updateFeatureInstancePointers Updates raw Feature pointers
   */
  void updateFeatureInstancePointers();

  CAxisSegmentFeatures(const CAxisSegmentFeatures&); // Copy Constructor Not Implemented
  void operator=(const CAxisSegmentFeatures&);       // Operator '=' Not Implemented
};

#endif /* CAxisSegmentFeatures_H_ */
