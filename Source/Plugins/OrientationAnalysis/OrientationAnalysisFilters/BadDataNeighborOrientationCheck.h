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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

class LaueOps;
using LaueOpsShPtrType = std::shared_ptr<LaueOps>;
using LaueOpsContainer = std::vector<LaueOpsShPtrType>;

/**
 * @brief The BadDataNeighborOrientationCheck class. See [Filter documentation](@ref baddataneighbororientationcheck) for details.
 */
class OrientationAnalysis_EXPORT BadDataNeighborOrientationCheck : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(BadDataNeighborOrientationCheck SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(BadDataNeighborOrientationCheck)
  PYB11_FILTER_NEW_MACRO(BadDataNeighborOrientationCheck)
  PYB11_PROPERTY(float MisorientationTolerance READ getMisorientationTolerance WRITE setMisorientationTolerance)
  PYB11_PROPERTY(int NumberOfNeighbors READ getNumberOfNeighbors WRITE setNumberOfNeighbors)
  PYB11_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)
  PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(DataArrayPath QuatsArrayPath READ getQuatsArrayPath WRITE setQuatsArrayPath)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = BadDataNeighborOrientationCheck;
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
   * @brief Returns the name of the class for BadDataNeighborOrientationCheck
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for BadDataNeighborOrientationCheck
   */
  static QString ClassName();

  ~BadDataNeighborOrientationCheck() override;

  /**
   * @brief Setter property for MisorientationTolerance
   */
  void setMisorientationTolerance(float value);
  /**
   * @brief Getter property for MisorientationTolerance
   * @return Value of MisorientationTolerance
   */
  float getMisorientationTolerance() const;
  Q_PROPERTY(float MisorientationTolerance READ getMisorientationTolerance WRITE setMisorientationTolerance)

  /**
   * @brief Setter property for NumberOfNeighbors
   */
  void setNumberOfNeighbors(int value);
  /**
   * @brief Getter property for NumberOfNeighbors
   * @return Value of NumberOfNeighbors
   */
  int getNumberOfNeighbors() const;
  Q_PROPERTY(int NumberOfNeighbors READ getNumberOfNeighbors WRITE setNumberOfNeighbors)

  /**
   * @brief Setter property for GoodVoxelsArrayPath
   */
  void setGoodVoxelsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for GoodVoxelsArrayPath
   * @return Value of GoodVoxelsArrayPath
   */
  DataArrayPath getGoodVoxelsArrayPath() const;
  Q_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)

  /**
   * @brief Setter property for CellPhasesArrayPath
   */
  void setCellPhasesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CellPhasesArrayPath
   * @return Value of CellPhasesArrayPath
   */
  DataArrayPath getCellPhasesArrayPath() const;
  Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

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
   * @brief Setter property for QuatsArrayPath
   */
  void setQuatsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for QuatsArrayPath
   * @return Value of QuatsArrayPath
   */
  DataArrayPath getQuatsArrayPath() const;
  Q_PROPERTY(DataArrayPath QuatsArrayPath READ getQuatsArrayPath WRITE setQuatsArrayPath)

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

protected:
  BadDataNeighborOrientationCheck();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<float>> m_QuatsPtr;
  float* m_Quats = nullptr;
  std::weak_ptr<DataArray<bool>> m_GoodVoxelsPtr;
  bool* m_GoodVoxels = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_CellPhasesPtr;
  int32_t* m_CellPhases = nullptr;
  std::weak_ptr<DataArray<uint32_t>> m_CrystalStructuresPtr;
  uint32_t* m_CrystalStructures = nullptr;

  float m_MisorientationTolerance = {5.0f};
  int m_NumberOfNeighbors = {6};
  DataArrayPath m_GoodVoxelsArrayPath = {SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Mask};
  DataArrayPath m_CellPhasesArrayPath = {SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Phases};
  DataArrayPath m_CrystalStructuresArrayPath = {SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures};
  DataArrayPath m_QuatsArrayPath = {SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::Quats};

  LaueOpsContainer m_OrientationOps;

public:
  BadDataNeighborOrientationCheck(const BadDataNeighborOrientationCheck&) = delete;            // Copy Constructor Not Implemented
  BadDataNeighborOrientationCheck(BadDataNeighborOrientationCheck&&) = delete;                 // Move Constructor Not Implemented
  BadDataNeighborOrientationCheck& operator=(const BadDataNeighborOrientationCheck&) = delete; // Copy Assignment Not Implemented
  BadDataNeighborOrientationCheck& operator=(BadDataNeighborOrientationCheck&&) = delete;      // Move Assignment Not Implemented
};
