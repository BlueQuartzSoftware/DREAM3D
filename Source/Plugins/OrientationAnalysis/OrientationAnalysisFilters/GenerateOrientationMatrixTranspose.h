/*
 * Your License or Copyright can go here
 */

#pragma once

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

/**
 * @brief The GenerateOrientationMatrixTranspose class. See [Filter documentation](@ref generateorientationmatrixtranspose) for details.
 */
class GenerateOrientationMatrixTranspose : public AbstractFilter
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(GenerateOrientationMatrixTranspose)
  SIMPL_FILTER_NEW_MACRO(GenerateOrientationMatrixTranspose)
  SIMPL_TYPE_MACRO_SUPER(GenerateOrientationMatrixTranspose, AbstractFilter)

  ~GenerateOrientationMatrixTranspose() override;

  SIMPL_FILTER_PARAMETER(DataArrayPath, OrientationMatrixDataArrayPath)
  Q_PROPERTY(DataArrayPath OrientationMatrixDataArrayPath READ getOrientationMatrixDataArrayPath WRITE setOrientationMatrixDataArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, OutputDataArrayPath)
  Q_PROPERTY(DataArrayPath OutputDataArrayPath READ getOutputDataArrayPath WRITE setOutputDataArrayPath)

  SIMPL_FILTER_PARAMETER(bool, DeleteOriginalData)
  Q_PROPERTY(bool DeleteOriginalData READ getDeleteOriginalData WRITE setDeleteOriginalData)

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
  GenerateOrientationMatrixTranspose();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  DEFINE_DATAARRAY_VARIABLE(float, OrientationMatrix)
  DEFINE_DATAARRAY_VARIABLE(float, OutputOrientationMatrix)

public:
  /* Rule of 5: All special member functions should be defined if any are defined.
   * https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-default-operation-define-or-delete-them-all
   */
public:
  GenerateOrientationMatrixTranspose(const GenerateOrientationMatrixTranspose&) = delete;            // Copy Constructor Not Implemented
  GenerateOrientationMatrixTranspose& operator=(const GenerateOrientationMatrixTranspose&) = delete; // Copy Assignment Not Implemented
  GenerateOrientationMatrixTranspose(GenerateOrientationMatrixTranspose&&) = delete;                 // Move Constructor Not Implemented
  GenerateOrientationMatrixTranspose& operator=(GenerateOrientationMatrixTranspose&&) = delete;      // Move Assignment Not Implemented
};
