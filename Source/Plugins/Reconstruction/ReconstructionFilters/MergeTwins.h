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

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"


#include "EbsdLib/EbsdConstants.h"

#include "Reconstruction/ReconstructionDLLExport.h"
#include "Reconstruction/ReconstructionFilters/GroupFeatures.h"

/**
 * @brief The MergeTwins class. See [Filter documentation](@ref mergetwins) for details.
 */
class Reconstruction_EXPORT MergeTwins : public GroupFeatures
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(MergeTwins SUPERCLASS GroupFeatures)
  PYB11_SHARED_POINTERS(MergeTwins)
  PYB11_FILTER_NEW_MACRO(MergeTwins)
  PYB11_FILTER_PARAMETER(QString, NewCellFeatureAttributeMatrixName)
  PYB11_FILTER_PARAMETER(float, AxisTolerance)
  PYB11_FILTER_PARAMETER(float, AngleTolerance)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_FILTER_PARAMETER(QString, CellParentIdsArrayName)
  PYB11_FILTER_PARAMETER(QString, FeatureParentIdsArrayName)
  PYB11_FILTER_PARAMETER(QString, ActiveArrayName)
  PYB11_PROPERTY(QString NewCellFeatureAttributeMatrixName READ getNewCellFeatureAttributeMatrixName WRITE setNewCellFeatureAttributeMatrixName)
  PYB11_PROPERTY(float AxisTolerance READ getAxisTolerance WRITE setAxisTolerance)
  PYB11_PROPERTY(float AngleTolerance READ getAngleTolerance WRITE setAngleTolerance)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(QString CellParentIdsArrayName READ getCellParentIdsArrayName WRITE setCellParentIdsArrayName)
  PYB11_PROPERTY(QString FeatureParentIdsArrayName READ getFeatureParentIdsArrayName WRITE setFeatureParentIdsArrayName)
  PYB11_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)

	// These were supposed to be inherited from GroupFeatures
	PYB11_PROPERTY(DataArrayPath ContiguousNeighborListArrayPath READ getContiguousNeighborListArrayPath WRITE setContiguousNeighborListArrayPath)
	PYB11_PROPERTY(DataArrayPath NonContiguousNeighborListArrayPath READ getNonContiguousNeighborListArrayPath WRITE setNonContiguousNeighborListArrayPath)
	PYB11_PROPERTY(bool UseNonContiguousNeighbors READ getUseNonContiguousNeighbors WRITE setUseNonContiguousNeighbors)
#endif

public:
  using Self = MergeTwins;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  
  /**
   * @brief Returns a NullPointer wrapped by a shared_ptr<>
   * @return
   */
  static Pointer NullPointer();

  /**
   * @brief Creates a new object wrapped in a shared_ptr<>
   * @return
   */
  static Pointer New();

  /**
   * @brief Returns the name of the class for MergeTwins
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for MergeTwins
   */
  static QString ClassName();

  virtual ~MergeTwins();

  /**
   * @brief Setter property for NewCellFeatureAttributeMatrixName
   */
  void setNewCellFeatureAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for NewCellFeatureAttributeMatrixName
   * @return Value of NewCellFeatureAttributeMatrixName
   */
  QString getNewCellFeatureAttributeMatrixName() const;

  Q_PROPERTY(QString NewCellFeatureAttributeMatrixName READ getNewCellFeatureAttributeMatrixName WRITE setNewCellFeatureAttributeMatrixName)

  /**
   * @brief Setter property for AxisTolerance
   */
  void setAxisTolerance(float value);
  /**
   * @brief Getter property for AxisTolerance
   * @return Value of AxisTolerance
   */
  float getAxisTolerance() const;

  Q_PROPERTY(float AxisTolerance READ getAxisTolerance WRITE setAxisTolerance)

  /**
   * @brief Setter property for AngleTolerance
   */
  void setAngleTolerance(float value);
  /**
   * @brief Getter property for AngleTolerance
   * @return Value of AngleTolerance
   */
  float getAngleTolerance() const;

  Q_PROPERTY(float AngleTolerance READ getAngleTolerance WRITE setAngleTolerance)

  /**
   * @brief Setter property for RandomizeParentIds
   */
  void setRandomizeParentIds(bool value);
  /**
   * @brief Getter property for RandomizeParentIds
   * @return Value of RandomizeParentIds
   */
  bool getRandomizeParentIds() const;

  /**
   * @brief Setter property for FeatureIdsArrayPath
   */
  void setFeatureIdsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureIdsArrayPath
   * @return Value of FeatureIdsArrayPath
   */
  DataArrayPath getFeatureIdsArrayPath() const;

  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  /**
   * @brief Setter property for FeaturePhasesArrayPath
   */
  void setFeaturePhasesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeaturePhasesArrayPath
   * @return Value of FeaturePhasesArrayPath
   */
  DataArrayPath getFeaturePhasesArrayPath() const;

  Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

  /**
   * @brief Setter property for AvgQuatsArrayPath
   */
  void setAvgQuatsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AvgQuatsArrayPath
   * @return Value of AvgQuatsArrayPath
   */
  DataArrayPath getAvgQuatsArrayPath() const;

  Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

  /**
   * @brief Setter property for CrystalStructuresArrayPath
   */
  void setCrystalStructuresArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CrystalStructuresArrayPath
   * @return Value of CrystalStructuresArrayPath
   */
  DataArrayPath getCrystalStructuresArrayPath() const;

  Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

  /**
   * @brief Setter property for CellParentIdsArrayName
   */
  void setCellParentIdsArrayName(const QString& value);
  /**
   * @brief Getter property for CellParentIdsArrayName
   * @return Value of CellParentIdsArrayName
   */
  QString getCellParentIdsArrayName() const;

  Q_PROPERTY(QString CellParentIdsArrayName READ getCellParentIdsArrayName WRITE setCellParentIdsArrayName)

  /**
   * @brief Setter property for FeatureParentIdsArrayName
   */
  void setFeatureParentIdsArrayName(const QString& value);
  /**
   * @brief Getter property for FeatureParentIdsArrayName
   * @return Value of FeatureParentIdsArrayName
   */
  QString getFeatureParentIdsArrayName() const;

  Q_PROPERTY(QString FeatureParentIdsArrayName READ getFeatureParentIdsArrayName WRITE setFeatureParentIdsArrayName)

  /**
   * @brief Setter property for ActiveArrayName
   */
  void setActiveArrayName(const QString& value);
  /**
   * @brief Getter property for ActiveArrayName
   * @return Value of ActiveArrayName
   */
  QString getActiveArrayName() const;

  Q_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  QString getHumanLabel() const override;

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
  MergeTwins();
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
   * @brief characterize_twins Characterizes twins; CURRENTLY NOT IMPLEMENTED
   */
  void characterize_twins();

private:
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;
  std::weak_ptr<DataArray<float>> m_AvgQuatsPtr;
  float* m_AvgQuats = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_FeaturePhasesPtr;
  int32_t* m_FeaturePhases = nullptr;
  std::weak_ptr<DataArray<unsigned int>> m_CrystalStructuresPtr;
  unsigned int* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<bool>> m_ActivePtr;
  bool* m_Active = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_CellParentIdsPtr;
  int32_t* m_CellParentIds = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_FeatureParentIdsPtr;
  int32_t* m_FeatureParentIds = nullptr;

  QString m_NewCellFeatureAttributeMatrixName = {};
  float m_AxisTolerance = {};
  float m_AngleTolerance = {};
  bool m_RandomizeParentIds = {};
  DataArrayPath m_FeatureIdsArrayPath = {};
  DataArrayPath m_FeaturePhasesArrayPath = {};
  DataArrayPath m_AvgQuatsArrayPath = {};
  DataArrayPath m_CrystalStructuresArrayPath = {};
  QString m_CellParentIdsArrayName = {};
  QString m_FeatureParentIdsArrayName = {};
  QString m_ActiveArrayName = {};

  float m_AxisToleranceRad = 0.0f;

  /**
   * @brief updateFeatureInstancePointers Updates raw Feature pointers
   */
  void updateFeatureInstancePointers();

public:
  MergeTwins(const MergeTwins&) = delete;            // Copy Constructor Not Implemented
  MergeTwins(MergeTwins&&) = delete;                 // Move Constructor Not Implemented
  MergeTwins& operator=(const MergeTwins&) = delete; // Copy Assignment Not Implemented
  MergeTwins& operator=(MergeTwins&&) = delete;      // Move Assignment Not Implemented
};

