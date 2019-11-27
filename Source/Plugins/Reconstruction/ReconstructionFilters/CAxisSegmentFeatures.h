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

#include "OrientationLib/LaueOps/LaueOps.h"

#include "Reconstruction/ReconstructionFilters/SegmentFeatures.h"

#include "Reconstruction/ReconstructionDLLExport.h"

/**
 * @brief The CAxisSegmentFeatures class. See [Filter documentation](@ref caxissegmentfeatures) for details.
 */
class Reconstruction_EXPORT CAxisSegmentFeatures : public SegmentFeatures
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(CAxisSegmentFeatures SUPERCLASS SegmentFeatures)
  PYB11_SHARED_POINTERS(CAxisSegmentFeatures)
  PYB11_FILTER_NEW_MACRO(CAxisSegmentFeatures)
  PYB11_FILTER_PARAMETER(QString, CellFeatureAttributeMatrixName)
  PYB11_FILTER_PARAMETER(float, MisorientationTolerance)
  PYB11_FILTER_PARAMETER(bool, UseGoodVoxels)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, QuatsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, GoodVoxelsArrayPath)
  PYB11_FILTER_PARAMETER(QString, FeatureIdsArrayName)
  PYB11_FILTER_PARAMETER(QString, ActiveArrayName)
  PYB11_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)
  PYB11_PROPERTY(float MisorientationTolerance READ getMisorientationTolerance WRITE setMisorientationTolerance)
  PYB11_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)
  PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(DataArrayPath QuatsArrayPath READ getQuatsArrayPath WRITE setQuatsArrayPath)
  PYB11_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)
  PYB11_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)
  PYB11_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)
#endif

public:
  using Self = CAxisSegmentFeatures;
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
   * @brief Returns the name of the class for CAxisSegmentFeatures
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for CAxisSegmentFeatures
   */
  static QString ClassName();

  virtual ~CAxisSegmentFeatures();

  /**
   * @brief Setter property for CellFeatureAttributeMatrixName
   */
  void setCellFeatureAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellFeatureAttributeMatrixName
   * @return Value of CellFeatureAttributeMatrixName
   */
  QString getCellFeatureAttributeMatrixName() const;

  Q_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

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
   * @brief Setter property for RandomizeFeatureIds
   */
  void setRandomizeFeatureIds(bool value);
  /**
   * @brief Getter property for RandomizeFeatureIds
   * @return Value of RandomizeFeatureIds
   */
  bool getRandomizeFeatureIds() const;

  /**
   * @brief Setter property for UseGoodVoxels
   */
  void setUseGoodVoxels(bool value);
  /**
   * @brief Getter property for UseGoodVoxels
   * @return Value of UseGoodVoxels
   */
  bool getUseGoodVoxels() const;

  Q_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)

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
   * @brief Setter property for FeatureIdsArrayName
   */
  void setFeatureIdsArrayName(const QString& value);
  /**
   * @brief Getter property for FeatureIdsArrayName
   * @return Value of FeatureIdsArrayName
   */
  QString getFeatureIdsArrayName() const;

  Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

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
  std::weak_ptr<DataArray<float>> m_QuatsPtr;
  float* m_Quats = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_CellPhasesPtr;
  int32_t* m_CellPhases = nullptr;
  std::weak_ptr<DataArray<bool>> m_GoodVoxelsPtr;
  bool* m_GoodVoxels = nullptr;
  std::weak_ptr<DataArray<uint32_t>> m_CrystalStructuresPtr;
  uint32_t* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<bool>> m_ActivePtr;
  bool* m_Active = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;

  QString m_CellFeatureAttributeMatrixName = {};
  float m_MisorientationTolerance = {};
  bool m_RandomizeFeatureIds = {};
  bool m_UseGoodVoxels = {};
  DataArrayPath m_CellPhasesArrayPath = {};
  DataArrayPath m_CrystalStructuresArrayPath = {};
  DataArrayPath m_QuatsArrayPath = {};
  DataArrayPath m_GoodVoxelsArrayPath = {};
  QString m_FeatureIdsArrayName = {};
  QString m_ActiveArrayName = {};

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
  CAxisSegmentFeatures(const CAxisSegmentFeatures&) = delete; // Copy Constructor Not Implemented
  CAxisSegmentFeatures(CAxisSegmentFeatures&&) = delete;      // Move Constructor Not Implemented
  CAxisSegmentFeatures& operator=(const CAxisSegmentFeatures&) = delete; // Copy Assignment Not Implemented
  CAxisSegmentFeatures& operator=(CAxisSegmentFeatures&&) = delete;      // Move Assignment Not Implemented
};

