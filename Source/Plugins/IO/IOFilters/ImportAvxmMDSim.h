/*
 * Your License or Copyright can go here
 */

#pragma once

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "IO/IODLLExport.h"

/**
 * @brief The ImportAvxmMDSim class. See [Filter documentation](@ref ImportAvxmMDSim) for details.
 */
class IO_EXPORT ImportAvxmMDSim : public AbstractFilter
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(ImportAvxmMDSim)
  SIMPL_FILTER_NEW_MACRO(ImportAvxmMDSim)
  SIMPL_TYPE_MACRO_SUPER(ImportAvxmMDSim, AbstractFilter)

  ~ImportAvxmMDSim() override;

  enum class SeparatorType : unsigned int
  {
    Tab,
    Space
  };

  SIMPL_FILTER_PARAMETER(FileListInfo_t, InputFileListInfo)
  Q_PROPERTY(FileListInfo_t InputFileListInfo READ getInputFileListInfo WRITE setInputFileListInfo)

  SIMPL_FILTER_PARAMETER(int, SeparatorChoice)
  Q_PROPERTY(int SeparatorChoice READ getSeparatorChoice WRITE setSeparatorChoice)

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
  ImportAvxmMDSim();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  QVector<QString> m_FilePathList;

  /**
   * @brief readLines
   * @return
   */
  QStringList readLines(const QString& filePath);

public:
  /* Rule of 5: All special member functions should be defined if any are defined.
   * https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-default-operation-define-or-delete-them-all
   */
public:
  ImportAvxmMDSim(const ImportAvxmMDSim&) = delete;            // Copy Constructor
  ImportAvxmMDSim& operator=(const ImportAvxmMDSim&) = delete; // Copy Assignment
  ImportAvxmMDSim(ImportAvxmMDSim&&) = delete;                 // Move Constructor Not Implemented
  ImportAvxmMDSim& operator=(ImportAvxmMDSim&&) = delete;      // Move Assignment Not Implemented
};
