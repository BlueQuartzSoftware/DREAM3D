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

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationLib/LaueOps/LaueOps.h"

#include "Reconstruction/ReconstructionFilters/GroupFeatures.h"

#include "Reconstruction/ReconstructionDLLExport.h"

/**
 * @brief The MergeColonies class. See [Filter documentation](@ref mergecolonies) for details.
 */
class Reconstruction_EXPORT MergeColonies : public GroupFeatures
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(MergeColonies SUPERCLASS GroupFeatures)
    PYB11_PROPERTY(QString NewCellFeatureAttributeMatrixName READ getNewCellFeatureAttributeMatrixName WRITE setNewCellFeatureAttributeMatrixName)
    PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
    PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
    PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
    PYB11_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)
    PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
    PYB11_PROPERTY(QString CellParentIdsArrayName READ getCellParentIdsArrayName WRITE setCellParentIdsArrayName)
    PYB11_PROPERTY(QString GlobAlphaArrayName READ getGlobAlphaArrayName WRITE setGlobAlphaArrayName)
    PYB11_PROPERTY(QString FeatureParentIdsArrayName READ getFeatureParentIdsArrayName WRITE setFeatureParentIdsArrayName)
    PYB11_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)
    PYB11_PROPERTY(float AxisTolerance READ getAxisTolerance WRITE setAxisTolerance)
    PYB11_PROPERTY(float AngleTolerance READ getAngleTolerance WRITE setAngleTolerance)
    PYB11_PROPERTY(bool IdentifyGlobAlpha READ getIdentifyGlobAlpha WRITE setIdentifyGlobAlpha)
public:
  SIMPL_SHARED_POINTERS(MergeColonies)
  SIMPL_FILTER_NEW_MACRO(MergeColonies)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(MergeColonies, AbstractFilter)

  virtual ~MergeColonies();

  SIMPL_FILTER_PARAMETER(QString, NewCellFeatureAttributeMatrixName)
  Q_PROPERTY(QString NewCellFeatureAttributeMatrixName READ getNewCellFeatureAttributeMatrixName WRITE setNewCellFeatureAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
  Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
  Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

  SIMPL_FILTER_PARAMETER(QString, CellParentIdsArrayName)
  Q_PROPERTY(QString CellParentIdsArrayName READ getCellParentIdsArrayName WRITE setCellParentIdsArrayName)

  SIMPL_FILTER_PARAMETER(QString, GlobAlphaArrayName)
  Q_PROPERTY(QString GlobAlphaArrayName READ getGlobAlphaArrayName WRITE setGlobAlphaArrayName)

  SIMPL_FILTER_PARAMETER(QString, FeatureParentIdsArrayName)
  Q_PROPERTY(QString FeatureParentIdsArrayName READ getFeatureParentIdsArrayName WRITE setFeatureParentIdsArrayName)

  SIMPL_FILTER_PARAMETER(QString, ActiveArrayName)
  Q_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)

  SIMPL_FILTER_PARAMETER(float, AxisTolerance)
  Q_PROPERTY(float AxisTolerance READ getAxisTolerance WRITE setAxisTolerance)

  SIMPL_FILTER_PARAMETER(float, AngleTolerance)
  Q_PROPERTY(float AngleTolerance READ getAngleTolerance WRITE setAngleTolerance)

  SIMPL_INSTANCE_PROPERTY(bool, RandomizeParentIds)

  SIMPL_FILTER_PARAMETER(bool, IdentifyGlobAlpha)
  Q_PROPERTY(bool IdentifyGlobAlpha READ getIdentifyGlobAlpha WRITE setIdentifyGlobAlpha)

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
  MergeColonies();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief getSeed Reimplemented from @see GroupFeatures class
   */
  virtual int32_t getSeed(int32_t newFid);

  /**
   * @brief determineGrouping Reimplemented from @see GroupFeatures class
   */
  virtual bool determineGrouping(int32_t referenceFeature, int32_t neighborFeature, int32_t newFid);

  /**
   * @brief check_for_burgers Checks the Burgers vector between two quaternions
   * @param betaQuat Beta quaterion
   * @param alphaQuat Alpha quaternion
   * @return Boolean vector check
   */
  bool check_for_burgers(QuatF betaQuat, QuatF alphaQuat);

  /**
   * @brief characterize_colonies Characterizes colonies; CURRENTLY NOT IMPLEMENTED
   */
  void characterize_colonies();

  /**
   * @brief identify_globAlpha Identifies regions of glob alpha
   */
  void identify_globAlpha();

private:
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
  DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
  DEFINE_DATAARRAY_VARIABLE(float, AvgQuats)
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
  DEFINE_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)

  DEFINE_DATAARRAY_VARIABLE(int32_t, CellParentIds)
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureParentIds)
  DEFINE_DATAARRAY_VARIABLE(int32_t, GlobAlpha)
  DEFINE_DATAARRAY_VARIABLE(bool, Active)

  QVector<LaueOps::Pointer> m_OrientationOps;

  float m_AxisToleranceRad;

  /**
   * @brief updateFeatureInstancePointers Updates raw Feature pointers
   */
  void updateFeatureInstancePointers();

public:
  MergeColonies(const MergeColonies&) = delete;  // Copy Constructor Not Implemented
  MergeColonies(MergeColonies&&) = delete;       // Move Constructor Not Implemented
  MergeColonies& operator=(const MergeColonies&) = delete; // Copy Assignment Not Implemented
  MergeColonies& operator=(MergeColonies&&) = delete;      // Move Assignment Not Implemented
};

