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

#include "Reconstruction/ReconstructionFilters/AlignSections.h"

#include "Reconstruction/ReconstructionDLLExport.h"

/**
 * @brief The AlignSectionsList class. See [Filter documentation](@ref alignsectionslist) for details.
 */
class Reconstruction_EXPORT AlignSectionsList : public AlignSections
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(AlignSectionsList SUPERCLASS AlignSections)
  PYB11_SHARED_POINTERS(AlignSectionsList)
  PYB11_FILTER_NEW_MACRO(AlignSectionsList)
  PYB11_FILTER_PARAMETER(QString, InputFile)
  PYB11_FILTER_PARAMETER(bool, DREAM3DAlignmentFile)
  PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
  PYB11_PROPERTY(bool DREAM3DAlignmentFile READ getDREAM3DAlignmentFile WRITE setDREAM3DAlignmentFile)
#endif

public:
  using Self = AlignSectionsList;
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
   * @brief Returns the name of the class for AlignSectionsList
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for AlignSectionsList
   */
  static QString ClassName();

  virtual ~AlignSectionsList();

  /**
   * @brief Setter property for InputFile
   */
  void setInputFile(const QString& value);
  /**
   * @brief Getter property for InputFile
   * @return Value of InputFile
   */
  QString getInputFile() const;

  Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

  /**
   * @brief Setter property for DREAM3DAlignmentFile
   */
  void setDREAM3DAlignmentFile(bool value);
  /**
   * @brief Getter property for DREAM3DAlignmentFile
   * @return Value of DREAM3DAlignmentFile
   */
  bool getDREAM3DAlignmentFile() const;

  Q_PROPERTY(bool DREAM3DAlignmentFile READ getDREAM3DAlignmentFile WRITE setDREAM3DAlignmentFile)

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

protected:
  AlignSectionsList();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief find_shifts Reimplemented from @see AlignSections class
   */
  virtual void find_shifts(std::vector<int64_t>& xshifts, std::vector<int64_t>& yshifts);

public:
  AlignSectionsList(const AlignSectionsList&) = delete; // Copy Constructor Not Implemented
  AlignSectionsList(AlignSectionsList&&) = delete;      // Move Constructor Not Implemented
  AlignSectionsList& operator=(const AlignSectionsList&) = delete; // Copy Assignment Not Implemented
  AlignSectionsList& operator=(AlignSectionsList&&) = delete;      // Move Assignment Not Implemented

private:
  QString m_InputFile = {};
  bool m_DREAM3DAlignmentFile = {};
};

