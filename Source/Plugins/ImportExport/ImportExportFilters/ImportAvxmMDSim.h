/*
 * Your License or Copyright can go here
 */

#pragma once

#include <memory>

#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"
#include "SIMPLib/FilterParameters/StackFileListInfo.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "ImportExport/ImportExportDLLExport.h"

/**
 * @brief The ImportAvxmMDSim class. See [Filter documentation](@ref ImportAvxmMDSim) for details.
 */
class ImportExport_EXPORT ImportAvxmMDSim : public AbstractFilter
{
  Q_OBJECT

public:
  using Self = ImportAvxmMDSim;
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
   * @brief Returns the name of the class for ImportAvxmMDSim
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ImportAvxmMDSim
   */
  static QString ClassName();

  ~ImportAvxmMDSim() override;

  enum class SeparatorType : unsigned int
  {
    Tab,
    Space
  };

  /**
   * @brief Setter property for InputFileListInfo
   */
  void setInputFileListInfo(const StackFileListInfo& value);
  /**
   * @brief Getter property for InputFileListInfo
   * @return Value of InputFileListInfo
   */
  StackFileListInfo getInputFileListInfo() const;

  Q_PROPERTY(StackFileListInfo InputFileListInfo READ getInputFileListInfo WRITE setInputFileListInfo)

  /**
   * @brief Setter property for SeparatorChoice
   */
  void setSeparatorChoice(int value);
  /**
   * @brief Getter property for SeparatorChoice
   * @return Value of SeparatorChoice
   */
  int getSeparatorChoice() const;

  Q_PROPERTY(int SeparatorChoice READ getSeparatorChoice WRITE setSeparatorChoice)

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
  StackFileListInfo m_InputFileListInfo = {};
  int m_SeparatorChoice = {};

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
