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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "Reconstruction/ReconstructionDLLExport.h"

/**
 * @brief The AlignSections class. This class serves as a superclass for other classes
 * in the Reconstruction plugin.
 */
class Reconstruction_EXPORT AlignSections : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(AlignSections SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(AlignSections)
  PYB11_FILTER_NEW_MACRO(AlignSections)
  PYB11_FILTER_PARAMETER(DataArrayPath, DataContainerName)
  PYB11_FILTER_PARAMETER(QString, CellAttributeMatrixName)
  PYB11_FILTER_PARAMETER(bool, WriteAlignmentShifts)
  PYB11_FILTER_PARAMETER(QString, AlignmentShiftFileName)
  PYB11_FILTER_PARAMETER(QVector<DataArrayPath>, IgnoredDataArrayPaths)
  PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
  PYB11_PROPERTY(bool WriteAlignmentShifts READ getWriteAlignmentShifts WRITE WriteAlignmentShifts)
  PYB11_PROPERTY(QString AlignmentShiftFileName READ getAlignmentShiftFileName WRITE setAlignmentShiftFileName)
#endif

public:
  using Self = AlignSections;
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
   * @brief Returns the name of the class for AlignSections
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for AlignSections
   */
  static QString ClassName();

  ~AlignSections() override;

  /**
   * @brief Setter property for DataContainerName
   */
  void setDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for DataContainerName
   * @return Value of DataContainerName
   */
  DataArrayPath getDataContainerName() const;

  Q_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)

  /**
   * @brief Setter property for CellAttributeMatrixName
   */
  void setCellAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellAttributeMatrixName
   * @return Value of CellAttributeMatrixName
   */
  QString getCellAttributeMatrixName() const;

  /**
   * @brief Setter property for WriteAlignmentShifts
   */
  void setWriteAlignmentShifts(bool value);
  /**
   * @brief Getter property for WriteAlignmentShifts
   * @return Value of WriteAlignmentShifts
   */
  bool getWriteAlignmentShifts() const;

  Q_PROPERTY(bool WriteAlignmentShifts READ getWriteAlignmentShifts WRITE setWriteAlignmentShifts)

  /**
   * @brief Setter property for AlignmentShiftFileName
   */
  void setAlignmentShiftFileName(const QString& value);
  /**
   * @brief Getter property for AlignmentShiftFileName
   * @return Value of AlignmentShiftFileName
   */
  QString getAlignmentShiftFileName() const;

  Q_PROPERTY(QString AlignmentShiftFileName READ getAlignmentShiftFileName WRITE setAlignmentShiftFileName)

  /**
   * @brief Setter property for IgnoredDataArrayPaths
   */
  void setIgnoredDataArrayPaths(const QVector<DataArrayPath>& value);
  /**
   * @brief Getter property for IgnoredDataArrayPaths
   * @return Value of IgnoredDataArrayPaths
   */
  QVector<DataArrayPath> getIgnoredDataArrayPaths() const;

  Q_PROPERTY(QVector<DataArrayPath> IgnoredDataArrayPaths READ getIgnoredDataArrayPaths WRITE setIgnoredDataArrayPaths)

  void updateProgress(size_t p);

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
  AlignSections();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief find_shifts Determines the x and y shifts to register a stacked 3D volume
   * @param xshifts Vector of integer shifts in x direction
   * @param yshifts Vector of integer shifts in y direction
   */
  virtual void find_shifts(std::vector<int64_t>& xshifts, std::vector<int64_t>& yshifts);

private:
  DataArrayPath m_DataContainerName = {};
  QString m_CellAttributeMatrixName = {};
  bool m_WriteAlignmentShifts = {};
  QString m_AlignmentShiftFileName = {};
  QVector<DataArrayPath> m_IgnoredDataArrayPaths = {};

  size_t m_Progress = 0;
  size_t m_TotalProgress = 0;

public:
  AlignSections(const AlignSections&) = delete;  // Copy Constructor Not Implemented
  AlignSections(AlignSections&&) = delete;       // Move Constructor Not Implemented
  AlignSections& operator=(const AlignSections&) = delete; // Copy Assignment Not Implemented
  AlignSections& operator=(AlignSections&&) = delete;      // Move Assignment Not Implemented
};

