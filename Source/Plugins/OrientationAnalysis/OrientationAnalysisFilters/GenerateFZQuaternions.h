/*
 * Your License or Copyright can go here
 */

#ifndef _generatefzquaternions_h_
#define _generatefzquaternions_h_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationLib/LaueOps/LaueOps.h"

/**
 * @brief The GenerateFZQuaternions class. See [Filter documentation](@ref generatefzquaternions) for details.
 */
class GenerateFZQuaternions : public AbstractFilter
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(GenerateFZQuaternions)
  SIMPL_FILTER_NEW_MACRO(GenerateFZQuaternions)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(GenerateFZQuaternions, AbstractFilter)

  ~GenerateFZQuaternions() override;

  SIMPL_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
  Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, QuatsArrayPath)
  Q_PROPERTY(DataArrayPath QuatsArrayPath READ getQuatsArrayPath WRITE setQuatsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

  SIMPL_FILTER_PARAMETER(bool, UseGoodVoxels)
  Q_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)

  SIMPL_FILTER_PARAMETER(DataArrayPath, GoodVoxelsArrayPath)
  Q_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FZQuatsArrayPath)
  Q_PROPERTY(DataArrayPath FZQuatsArrayPath READ getFZQuatsArrayPath WRITE setFZQuatsArrayPath)

  /**
   * @brief incrementPhaseWarningCount
   */
  void incrementPhaseWarningCount();

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
  DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
  DEFINE_DATAARRAY_VARIABLE(float, Quats)
  DEFINE_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)
  DEFINE_DATAARRAY_VARIABLE(bool, GoodVoxels)
  DEFINE_DATAARRAY_VARIABLE(float, FZQuats)

  QVector<LaueOps::Pointer> m_OrientationOps;
  int32_t m_PhaseWarningCount = 0;

public:
  GenerateFZQuaternions(const GenerateFZQuaternions&) = delete; // Copy Constructor Not Implemented
  GenerateFZQuaternions(GenerateFZQuaternions&&) = delete;      // Move Constructor
  GenerateFZQuaternions& operator=(const GenerateFZQuaternions&) = delete; // Copy Assignment Not Implemented
  GenerateFZQuaternions& operator=(GenerateFZQuaternions&&) = delete;      // Move Assignment Not Implemented
};

#endif /* _GenerateFZQuaternions_H_ */
