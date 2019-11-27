/*
 * Your License or Copyright can go here
 */

#pragma once

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "OrientationLib/LaueOps/LaueOps.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The GenerateFZQuaternions class. See [Filter documentation](@ref generatefzquaternions) for details.
 */
class OrientationAnalysis_EXPORT GenerateFZQuaternions : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(GenerateFZQuaternions SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(GenerateFZQuaternions)
  PYB11_FILTER_NEW_MACRO(GenerateFZQuaternions)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, QuatsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_FILTER_PARAMETER(bool, UseGoodVoxels)
  PYB11_FILTER_PARAMETER(DataArrayPath, GoodVoxelsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FZQuatsArrayPath)
  PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath QuatsArrayPath READ getQuatsArrayPath WRITE setQuatsArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)
  PYB11_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)
  PYB11_PROPERTY(DataArrayPath FZQuatsArrayPath READ getFZQuatsArrayPath WRITE setFZQuatsArrayPath)
#endif

public:
  using Self = GenerateFZQuaternions;
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
   * @brief Returns the name of the class for GenerateFZQuaternions
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for GenerateFZQuaternions
   */
  static QString ClassName();

  ~GenerateFZQuaternions() override;

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
   * @brief Setter property for FZQuatsArrayPath
   */
  void setFZQuatsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FZQuatsArrayPath
   * @return Value of FZQuatsArrayPath
   */
  DataArrayPath getFZQuatsArrayPath() const;

  Q_PROPERTY(DataArrayPath FZQuatsArrayPath READ getFZQuatsArrayPath WRITE setFZQuatsArrayPath)

  /**
   * @brief incrementPhaseWarningCount
   */
  void incrementPhaseWarningCount();

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
  GenerateFZQuaternions();

  /**
  * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
  */
  void dataCheck();

  /**
  * @brief Initializes all the private instance variables.
  */
  void initialize();

private:
  std::weak_ptr<DataArray<int32_t>> m_CellPhasesPtr;
  int32_t* m_CellPhases = nullptr;
  std::weak_ptr<DataArray<float>> m_QuatsPtr;
  float* m_Quats = nullptr;
  std::weak_ptr<DataArray<uint32_t>> m_CrystalStructuresPtr;
  uint32_t* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<bool>> m_GoodVoxelsPtr;
  bool* m_GoodVoxels = nullptr;
  std::weak_ptr<DataArray<float>> m_FZQuatsPtr;
  float* m_FZQuats = nullptr;

  DataArrayPath m_CellPhasesArrayPath = {};
  DataArrayPath m_QuatsArrayPath = {};
  DataArrayPath m_CrystalStructuresArrayPath = {};
  bool m_UseGoodVoxels = {};
  DataArrayPath m_GoodVoxelsArrayPath = {};
  DataArrayPath m_FZQuatsArrayPath = {};

  QVector<LaueOps::Pointer> m_OrientationOps;
  int32_t m_PhaseWarningCount = 0;

public:
  GenerateFZQuaternions(const GenerateFZQuaternions&) = delete; // Copy Constructor Not Implemented
  GenerateFZQuaternions(GenerateFZQuaternions&&) = delete;      // Move Constructor Not Implemented
  GenerateFZQuaternions& operator=(const GenerateFZQuaternions&) = delete; // Copy Assignment Not Implemented
  GenerateFZQuaternions& operator=(GenerateFZQuaternions&&) = delete;      // Move Assignment Not Implemented
};

