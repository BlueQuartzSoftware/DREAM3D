/*
 * Your License or Copyright can go here
 */

#pragma once

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The GenerateQuaternionConjugate class. See [Filter documentation](@ref generatequaternionconjugate) for details.
 */
class OrientationAnalysis_EXPORT GenerateQuaternionConjugate : public AbstractFilter
{
  Q_OBJECT
  // clang-format off
    PYB11_CREATE_BINDINGS(GenerateQuaternionConjugate SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(DataArrayPath QuaternionDataArrayPath READ getQuaternionDataArrayPath WRITE setQuaternionDataArrayPath)
    PYB11_PROPERTY(DataArrayPath OutputDataArrayPath READ getOutputDataArrayPath WRITE setOutputDataArrayPath)
    PYB11_PROPERTY(bool DeleteOriginalData READ getDeleteOriginalData WRITE setDeleteOriginalData)
  // clang-format on

public:
  SIMPL_SHARED_POINTERS(GenerateQuaternionConjugate)
  SIMPL_FILTER_NEW_MACRO(GenerateQuaternionConjugate)
  SIMPL_TYPE_MACRO_SUPER(GenerateQuaternionConjugate, AbstractFilter)

  ~GenerateQuaternionConjugate() override;

  SIMPL_FILTER_PARAMETER(DataArrayPath, QuaternionDataArrayPath)
  Q_PROPERTY(DataArrayPath QuaternionDataArrayPath READ getQuaternionDataArrayPath WRITE setQuaternionDataArrayPath)

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
  GenerateQuaternionConjugate();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  DEFINE_DATAARRAY_VARIABLE(float, Quaternions)
  DEFINE_DATAARRAY_VARIABLE(float, OutputQuaternions)

public:
  /* Rule of 5: All special member functions should be defined if any are defined.
   * https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-default-operation-define-or-delete-them-all
   */
public:
  GenerateQuaternionConjugate(const GenerateQuaternionConjugate&) = delete;            // Copy Constructor Not Implemented
  GenerateQuaternionConjugate& operator=(const GenerateQuaternionConjugate&) = delete; // Copy Assignment Not Implemented
  GenerateQuaternionConjugate(GenerateQuaternionConjugate&&) = delete;                 // Move Constructor Not Implemented
  GenerateQuaternionConjugate& operator=(GenerateQuaternionConjugate&&) = delete;      // Move Assignment Not Implemented
};
