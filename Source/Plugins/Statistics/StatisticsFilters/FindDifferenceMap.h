/*
 * Your License or Copyright can go here
 */

#pragma once

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "Statistics/StatisticsDLLExport.h"

/**
 * @brief The FindDifferenceMap class. See [Filter documentation](@ref finddifferencemap) for details.
 */
class Statistics_EXPORT FindDifferenceMap : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(FindDifferenceMap SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(DataArrayPath FirstInputArrayPath READ getFirstInputArrayPath WRITE setFirstInputArrayPath)
    PYB11_PROPERTY(DataArrayPath SecondInputArrayPath READ getSecondInputArrayPath WRITE setSecondInputArrayPath)
    PYB11_PROPERTY(DataArrayPath DifferenceMapArrayPath READ getDifferenceMapArrayPath WRITE setDifferenceMapArrayPath)

public:
  SIMPL_SHARED_POINTERS(FindDifferenceMap)
  SIMPL_FILTER_NEW_MACRO(FindDifferenceMap)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(FindDifferenceMap, AbstractFilter)

  ~FindDifferenceMap() override;

  SIMPL_FILTER_PARAMETER(DataArrayPath, FirstInputArrayPath)
  Q_PROPERTY(DataArrayPath FirstInputArrayPath READ getFirstInputArrayPath WRITE setFirstInputArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SecondInputArrayPath)
  Q_PROPERTY(DataArrayPath SecondInputArrayPath READ getSecondInputArrayPath WRITE setSecondInputArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, DifferenceMapArrayPath)
  Q_PROPERTY(DataArrayPath DifferenceMapArrayPath READ getDifferenceMapArrayPath WRITE setDifferenceMapArrayPath)

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
  FindDifferenceMap();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  DEFINE_IDATAARRAY_VARIABLE(FirstInputArray)
  DEFINE_IDATAARRAY_VARIABLE(SecondInputArray)
  DEFINE_IDATAARRAY_VARIABLE(DifferenceMap)

public:
  FindDifferenceMap(const FindDifferenceMap&) = delete; // Copy Constructor Not Implemented
  FindDifferenceMap(FindDifferenceMap&&) = delete;      // Move Constructor Not Implemented
  FindDifferenceMap& operator=(const FindDifferenceMap&) = delete; // Copy Assignment Not Implemented
  FindDifferenceMap& operator=(FindDifferenceMap&&) = delete;      // Move Assignment Not Implemented
};

