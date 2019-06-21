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

#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

class LaueOps;

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The WriteIPFStandardTriangle class. See [Filter documentation](@ref writeipfstandardtriangle) for details.
 */
class OrientationAnalysis_EXPORT WriteIPFStandardTriangle : public AbstractFilter
{
  Q_OBJECT
  // PYB11_CREATE_BINDINGS(WriteIPFStandardTriangle SUPERCLASS AbstractFilter)
  // PYB11_PROPERTY(int LaueClass READ getLaueClass WRITE setLaueClass)
  // PYB11_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)
  // PYB11_PROPERTY(int ImageSize READ getImageSize WRITE setImageSize)
public:
  using Self = WriteIPFStandardTriangle;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  static Pointer NullPointer();

  static std::shared_ptr<WriteIPFStandardTriangle> New();

  /**
   * @brief Returns the name of the class for WriteIPFStandardTriangle
   */
  const QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for WriteIPFStandardTriangle
   */
  static QString ClassName();

  ~WriteIPFStandardTriangle() override;

  /**
   * @brief Setter property for LaueClass
   */
  void setLaueClass(int value);
  /**
   * @brief Getter property for LaueClass
   * @return Value of LaueClass
   */
  int getLaueClass() const;

  Q_PROPERTY(int LaueClass READ getLaueClass WRITE setLaueClass)

  /**
   * @brief Setter property for OutputFile
   */
  void setOutputFile(const QString& value);
  /**
   * @brief Getter property for OutputFile
   * @return Value of OutputFile
   */
  QString getOutputFile() const;

  Q_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)

  /**
   * @brief Setter property for ImageSize
   */
  void setImageSize(int value);
  /**
   * @brief Getter property for ImageSize
   * @return Value of ImageSize
   */
  int getImageSize() const;

  Q_PROPERTY(int ImageSize READ getImageSize WRITE setImageSize)

  enum ImageFormatType
  {
    TifImageType = 0,
    BmpImageType = 1,
    PngImageType = 2,
    JpgImageType = 3
  };

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
  WriteIPFStandardTriangle();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief writeImage Writes the generated IPF triangle to a file
   * @param image Image to write
   */
  void writeImage(QImage& image);

  /**
   * @brief generateCubicHighTriangle Generates the standard IPF triangle for a cubic symmetry
   * @return QImage of cubic IPF triangle
   */
  QImage generateCubicHighTriangle();

  /**
   * @brief overlayCubicHighText Overlays the legend of the IPF triangle
   * @param image QImage of cubic IPF triangle
   * @return
   */
  QImage overlayText(QImage image, LaueOps* ops);

public:
  WriteIPFStandardTriangle(const WriteIPFStandardTriangle&) = delete; // Copy Constructor Not Implemented
  WriteIPFStandardTriangle(WriteIPFStandardTriangle&&) = delete;      // Move Constructor Not Implemented
  WriteIPFStandardTriangle& operator=(const WriteIPFStandardTriangle&) = delete; // Copy Assignment Not Implemented
  WriteIPFStandardTriangle& operator=(WriteIPFStandardTriangle&&) = delete;      // Move Assignment Not Implemented

private:
  int m_LaueClass = {};
  QString m_OutputFile = {};
  int m_ImageSize = {};
};

