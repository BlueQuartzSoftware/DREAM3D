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

#include <random>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "Reconstruction/ReconstructionDLLExport.h"

/**
 * @brief The IdentifyMicroTextureRegions class. See [Filter documentation](@ref identifymicrotextureregions) for details.
 */
class Reconstruction_EXPORT IdentifyMicroTextureRegions : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(IdentifyMicroTextureRegions SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(IdentifyMicroTextureRegions)
  PYB11_FILTER_NEW_MACRO(IdentifyMicroTextureRegions)
  PYB11_FILTER_PARAMETER(QString, NewCellFeatureAttributeMatrixName)
  PYB11_FILTER_PARAMETER(float, CAxisTolerance)
  PYB11_FILTER_PARAMETER(float, MinMTRSize)
  PYB11_FILTER_PARAMETER(float, MinVolFrac)
  PYB11_FILTER_PARAMETER(DataArrayPath, CAxisLocationsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_FILTER_PARAMETER(QString, MTRIdsArrayName)
  PYB11_FILTER_PARAMETER(QString, ActiveArrayName)
  PYB11_PROPERTY(QString NewCellFeatureAttributeMatrixName READ getNewCellFeatureAttributeMatrixName WRITE setNewCellFeatureAttributeMatrixName)
  PYB11_PROPERTY(float CAxisTolerance READ getCAxisTolerance WRITE setCAxisTolerance)
  PYB11_PROPERTY(float MinMTRSize READ getMinMTRSize WRITE setMinMTRSize)
  PYB11_PROPERTY(float MinVolFrac READ getMinVolFrac WRITE setMinVolFrac)
  PYB11_PROPERTY(DataArrayPath CAxisLocationsArrayPath READ getCAxisLocationsArrayPath WRITE setCAxisLocationsArrayPath)
  PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(QString MTRIdsArrayName READ getMTRIdsArrayName WRITE setMTRIdsArrayName)
  PYB11_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)
#endif

public:
  using Self = IdentifyMicroTextureRegions;
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
   * @brief Returns the name of the class for IdentifyMicroTextureRegions
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for IdentifyMicroTextureRegions
   */
  static QString ClassName();

  ~IdentifyMicroTextureRegions() override;

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
   * @brief Setter property for CAxisTolerance
   */
  void setCAxisTolerance(float value);
  /**
   * @brief Getter property for CAxisTolerance
   * @return Value of CAxisTolerance
   */
  float getCAxisTolerance() const;

  Q_PROPERTY(float CAxisTolerance READ getCAxisTolerance WRITE setCAxisTolerance)

  /**
   * @brief Setter property for MinMTRSize
   */
  void setMinMTRSize(float value);
  /**
   * @brief Getter property for MinMTRSize
   * @return Value of MinMTRSize
   */
  float getMinMTRSize() const;

  Q_PROPERTY(float MinMTRSize READ getMinMTRSize WRITE setMinMTRSize)

  /**
   * @brief Setter property for MinVolFrac
   */
  void setMinVolFrac(float value);
  /**
   * @brief Getter property for MinVolFrac
   * @return Value of MinVolFrac
   */
  float getMinVolFrac() const;

  Q_PROPERTY(float MinVolFrac READ getMinVolFrac WRITE setMinVolFrac)

  /**
   * @brief Setter property for RandomizeMTRIds
   */
  void setRandomizeMTRIds(bool value);
  /**
   * @brief Getter property for RandomizeMTRIds
   * @return Value of RandomizeMTRIds
   */
  bool getRandomizeMTRIds() const;

  /**
   * @brief Setter property for CAxisLocationsArrayPath
   */
  void setCAxisLocationsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CAxisLocationsArrayPath
   * @return Value of CAxisLocationsArrayPath
   */
  DataArrayPath getCAxisLocationsArrayPath() const;

  Q_PROPERTY(DataArrayPath CAxisLocationsArrayPath READ getCAxisLocationsArrayPath WRITE setCAxisLocationsArrayPath)

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
   * @brief Setter property for MTRIdsArrayName
   */
  void setMTRIdsArrayName(const QString& value);
  /**
   * @brief Getter property for MTRIdsArrayName
   * @return Value of MTRIdsArrayName
   */
  QString getMTRIdsArrayName() const;

  Q_PROPERTY(QString MTRIdsArrayName READ getMTRIdsArrayName WRITE setMTRIdsArrayName)

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

signals:
  /**
   * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
   * be pushed from a user-facing control (such as a widget)
   * @param filter Filter instance pointer
   */
  void updateFilterParameters(AbstractFilter* filter);

  /**
   * @brief parametersChanged Emitted when any Filter parameter is changed internally
   */
  void parametersChanged();

  /**
   * @brief preflightAboutToExecute Emitted just before calling dataCheck()
   */
  void preflightAboutToExecute();

  /**
   * @brief preflightExecuted Emitted just after calling dataCheck()
   */
  void preflightExecuted();

protected:
  IdentifyMicroTextureRegions();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<float>> m_CAxisLocationsPtr;
  float* m_CAxisLocations = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_CellPhasesPtr;
  int32_t* m_CellPhases = nullptr;
  std::weak_ptr<DataArray<unsigned int>> m_CrystalStructuresPtr;
  unsigned int* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_MTRIdsPtr;
  int32_t* m_MTRIds = nullptr;
  std::weak_ptr<DataArray<bool>> m_ActivePtr;
  bool* m_Active = nullptr;
  std::weak_ptr<DataArray<bool>> m_InMTRPtr;
  bool* m_InMTR = nullptr;
  std::weak_ptr<DataArray<float>> m_VolFracPtr;
  float* m_VolFrac = nullptr;
  std::weak_ptr<DataArray<float>> m_AvgCAxisPtr;
  float* m_AvgCAxis = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_PatchIdsPtr;
  int32_t* m_PatchIds = nullptr;
  std::weak_ptr<DataArray<bool>> m_PatchActivePtr;
  bool* m_PatchActive = nullptr;

  QString m_NewCellFeatureAttributeMatrixName = {};
  float m_CAxisTolerance = {};
  float m_MinMTRSize = {};
  float m_MinVolFrac = {};
  bool m_RandomizeMTRIds = {};
  DataArrayPath m_CAxisLocationsArrayPath = {};
  DataArrayPath m_CellPhasesArrayPath = {};
  DataArrayPath m_CrystalStructuresArrayPath = {};
  QString m_MTRIdsArrayName = {};
  QString m_ActiveArrayName = {};

  std::random_device m_RandomDevice;
  std::mt19937_64 m_Generator;
  std::uniform_int_distribution<int64_t> m_Distribution;

  size_t m_TotalRandomNumbersGenerated;

  float m_CAxisToleranceRad;

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

  /**
   * @brief updateFeatureInstancePointers Updates raw Feature pointers
   */
  void updateFeatureInstancePointers();

public:
  IdentifyMicroTextureRegions(const IdentifyMicroTextureRegions&) = delete; // Copy Constructor Not Implemented
  IdentifyMicroTextureRegions(IdentifyMicroTextureRegions&&) = delete;      // Move Constructor Not Implemented
  IdentifyMicroTextureRegions& operator=(const IdentifyMicroTextureRegions&) = delete; // Copy Assignment Not Implemented
  IdentifyMicroTextureRegions& operator=(IdentifyMicroTextureRegions&&) = delete;      // Move Assignment Not Implemented
};

