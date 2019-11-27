/*
 * Your License or Copyright can go here
 */

#pragma once

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

class IDataArray;
using IDataArrayWkPtrType = std::weak_ptr<IDataArray>;

#include "Statistics/StatisticsDLLExport.h"

/**
 * @brief The FindDifferenceMap class. See [Filter documentation](@ref finddifferencemap) for details.
 */
class Statistics_EXPORT FindDifferenceMap : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindDifferenceMap SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindDifferenceMap)
  PYB11_FILTER_NEW_MACRO(FindDifferenceMap)
  PYB11_FILTER_PARAMETER(DataArrayPath, FirstInputArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SecondInputArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, DifferenceMapArrayPath)
  PYB11_PROPERTY(DataArrayPath FirstInputArrayPath READ getFirstInputArrayPath WRITE setFirstInputArrayPath)
  PYB11_PROPERTY(DataArrayPath SecondInputArrayPath READ getSecondInputArrayPath WRITE setSecondInputArrayPath)
  PYB11_PROPERTY(DataArrayPath DifferenceMapArrayPath READ getDifferenceMapArrayPath WRITE setDifferenceMapArrayPath)
#endif

public:
  using Self = FindDifferenceMap;
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
   * @brief Returns the name of the class for FindDifferenceMap
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindDifferenceMap
   */
  static QString ClassName();

  ~FindDifferenceMap() override;

  /**
   * @brief Setter property for FirstInputArrayPath
   */
  void setFirstInputArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FirstInputArrayPath
   * @return Value of FirstInputArrayPath
   */
  DataArrayPath getFirstInputArrayPath() const;

  Q_PROPERTY(DataArrayPath FirstInputArrayPath READ getFirstInputArrayPath WRITE setFirstInputArrayPath)

  /**
   * @brief Setter property for SecondInputArrayPath
   */
  void setSecondInputArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SecondInputArrayPath
   * @return Value of SecondInputArrayPath
   */
  DataArrayPath getSecondInputArrayPath() const;

  Q_PROPERTY(DataArrayPath SecondInputArrayPath READ getSecondInputArrayPath WRITE setSecondInputArrayPath)

  /**
   * @brief Setter property for DifferenceMapArrayPath
   */
  void setDifferenceMapArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for DifferenceMapArrayPath
   * @return Value of DifferenceMapArrayPath
   */
  DataArrayPath getDifferenceMapArrayPath() const;

  Q_PROPERTY(DataArrayPath DifferenceMapArrayPath READ getDifferenceMapArrayPath WRITE setDifferenceMapArrayPath)

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
  IDataArrayWkPtrType m_FirstInputArrayPtr;
  void* m_FirstInputArray = nullptr;
  IDataArrayWkPtrType m_SecondInputArrayPtr;
  void* m_SecondInputArray = nullptr;
  IDataArrayWkPtrType m_DifferenceMapPtr;
  void* m_DifferenceMap = nullptr;

  DataArrayPath m_FirstInputArrayPath = {};
  DataArrayPath m_SecondInputArrayPath = {};
  DataArrayPath m_DifferenceMapArrayPath = {};

public:
  FindDifferenceMap(const FindDifferenceMap&) = delete; // Copy Constructor Not Implemented
  FindDifferenceMap(FindDifferenceMap&&) = delete;      // Move Constructor Not Implemented
  FindDifferenceMap& operator=(const FindDifferenceMap&) = delete; // Copy Assignment Not Implemented
  FindDifferenceMap& operator=(FindDifferenceMap&&) = delete;      // Move Assignment Not Implemented
};

