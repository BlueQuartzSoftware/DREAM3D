/*
 * Your License or Copyright can go here
 */

#pragma once

#include <memory>

#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The ConvertQuaternion class. See [Filter documentation](@ref convertquaternion) for details.
 */
class OrientationAnalysis_EXPORT ConvertQuaternion : public AbstractFilter
{
  Q_OBJECT
    // clang-format off

#ifdef SIMPL_ENABLE_PYTHON
    PYB11_CREATE_BINDINGS(ConvertQuaternion SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(ConvertQuaternion)
  PYB11_FILTER_NEW_MACRO(ConvertQuaternion)
  PYB11_FILTER_PARAMETER(DataArrayPath, QuaternionDataArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, OutputDataArrayPath)
  PYB11_FILTER_PARAMETER(bool, DeleteOriginalData)
  PYB11_FILTER_PARAMETER(int32_t, ConversionType)
    PYB11_PROPERTY(DataArrayPath QuaternionDataArrayPath READ getQuaternionDataArrayPath WRITE setQuaternionDataArrayPath)
    PYB11_PROPERTY(DataArrayPath OutputDataArrayPath READ getOutputDataArrayPath WRITE setOutputDataArrayPath)
    PYB11_PROPERTY(bool DeleteOriginalData READ getDeleteOriginalData WRITE setDeleteOriginalData)
    PYB11_PROPERTY(int32_t ConversionType READ getConversionType WRITE setConversionType)
#endif

  // clang-format on

public:
  using Self = ConvertQuaternion;
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
   * @brief Returns the name of the class for ConvertQuaternion
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ConvertQuaternion
   */
  static QString ClassName();

  ~ConvertQuaternion() override;

  /**
   * @brief Setter property for QuaternionDataArrayPath
   */
  void setQuaternionDataArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for QuaternionDataArrayPath
   * @return Value of QuaternionDataArrayPath
   */
  DataArrayPath getQuaternionDataArrayPath() const;

  Q_PROPERTY(DataArrayPath QuaternionDataArrayPath READ getQuaternionDataArrayPath WRITE setQuaternionDataArrayPath)

  /**
   * @brief Setter property for OutputDataArrayPath
   */
  void setOutputDataArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for OutputDataArrayPath
   * @return Value of OutputDataArrayPath
   */
  DataArrayPath getOutputDataArrayPath() const;

  Q_PROPERTY(DataArrayPath OutputDataArrayPath READ getOutputDataArrayPath WRITE setOutputDataArrayPath)

  /**
   * @brief Setter property for DeleteOriginalData
   */
  void setDeleteOriginalData(bool value);
  /**
   * @brief Getter property for DeleteOriginalData
   * @return Value of DeleteOriginalData
   */
  bool getDeleteOriginalData() const;

  Q_PROPERTY(bool DeleteOriginalData READ getDeleteOriginalData WRITE setDeleteOriginalData)

  /**
   * @brief Setter property for ConversionType
   */
  void setConversionType(int32_t value);
  /**
   * @brief Getter property for ConversionType
   * @return Value of ConversionType
   */
  int32_t getConversionType() const;

  Q_PROPERTY(int32_t ConversionType READ getConversionType WRITE setConversionType)

  static const int32_t k_ToScalarVector = 0;
  static const int32_t k_ToVectorScalar = 1;
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
  ConvertQuaternion();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<float>> m_QuaternionsPtr;
  float* m_Quaternions = nullptr;
  std::weak_ptr<DataArray<float>> m_OutputQuaternionsPtr;
  float* m_OutputQuaternions = nullptr;

  DataArrayPath m_QuaternionDataArrayPath = {};
  DataArrayPath m_OutputDataArrayPath = {};
  bool m_DeleteOriginalData = {};
  int32_t m_ConversionType = {};

public:
  /* Rule of 5: All special member functions should be defined if any are defined.
   * https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-default-operation-define-or-delete-them-all
   */
  ConvertQuaternion(const ConvertQuaternion&) = delete;            // Copy Constructor Not Implemented
  ConvertQuaternion& operator=(const ConvertQuaternion&) = delete; // Copy Assignment Not Implemented
  ConvertQuaternion(ConvertQuaternion&&) = delete;                 // Move Constructor Not Implemented
  ConvertQuaternion& operator=(ConvertQuaternion&&) = delete;      // Move Assignment Not Implemented

};

