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

#pragma once

#include <random>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationLib/LaueOps/LaueOps.h"

#include "Reconstruction/ReconstructionFilters/SegmentFeatures.h"

#include "Reconstruction/ReconstructionDLLExport.h"

/**
 * @brief The EBSDSegmentFeatures class. See [Filter documentation](@ref ebsdsegmentfeatures) for details.
 */
class Reconstruction_EXPORT EBSDSegmentFeatures : public SegmentFeatures
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(EBSDSegmentFeatures SUPERCLASS SegmentFeatures)
    PYB11_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)
    PYB11_PROPERTY(float MisorientationTolerance READ getMisorientationTolerance WRITE setMisorientationTolerance)
    PYB11_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)
    PYB11_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)
    PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
    PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
    PYB11_PROPERTY(DataArrayPath QuatsArrayPath READ getQuatsArrayPath WRITE setQuatsArrayPath)
    PYB11_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)
    PYB11_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)

public:
  SIMPL_SHARED_POINTERS(EBSDSegmentFeatures)
  SIMPL_FILTER_NEW_MACRO(EBSDSegmentFeatures)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(EBSDSegmentFeatures, AbstractFilter)

  virtual ~EBSDSegmentFeatures();

  SIMPL_FILTER_PARAMETER(QString, CellFeatureAttributeMatrixName)
  Q_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(float, MisorientationTolerance)
  Q_PROPERTY(float MisorientationTolerance READ getMisorientationTolerance WRITE setMisorientationTolerance)

  SIMPL_INSTANCE_PROPERTY(bool, RandomizeFeatureIds)

  SIMPL_FILTER_PARAMETER(bool, UseGoodVoxels)
  Q_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)

  SIMPL_FILTER_PARAMETER(DataArrayPath, GoodVoxelsArrayPath)
  Q_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
  Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, QuatsArrayPath)
  Q_PROPERTY(DataArrayPath QuatsArrayPath READ getQuatsArrayPath WRITE setQuatsArrayPath)

  SIMPL_FILTER_PARAMETER(QString, FeatureIdsArrayName)
  Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

  SIMPL_FILTER_PARAMETER(QString, ActiveArrayName)
  Q_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  const QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  const QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  void preflight() override;

protected:
  EBSDSegmentFeatures();
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
  DEFINE_DATAARRAY_VARIABLE(float, Quats)
  DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
  DEFINE_DATAARRAY_VARIABLE(bool, GoodVoxels)
  DEFINE_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)

  DEFINE_DATAARRAY_VARIABLE(bool, Active)
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)

  QVector<LaueOps::Pointer> m_OrientationOps;

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

public:
  EBSDSegmentFeatures(const EBSDSegmentFeatures&) = delete; // Copy Constructor Not Implemented
  EBSDSegmentFeatures(EBSDSegmentFeatures&&) = delete;      // Move Constructor Not Implemented
  EBSDSegmentFeatures& operator=(const EBSDSegmentFeatures&) = delete; // Copy Assignment Not Implemented
  EBSDSegmentFeatures& operator=(EBSDSegmentFeatures&&) = delete;      // Move Assignment Not Implemented
};

