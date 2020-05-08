/* ============================================================================
 * Copyright (c) 2018 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>
#include <map>

#include <QtCore/QString>

#include "EbsdLib/IO/TSL/AngPhase.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/FilterParameters/MontageFileListInfo.h"

#include "OrientationAnalysis/FilterParameters/EbsdMontageImportFilterParameter.h"
#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The ImportEbsdMontage class. See [Filter documentation](@ref importebsdmontage) for details.
 */
class OrientationAnalysis_EXPORT ImportEbsdMontage : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(ImportEbsdMontage SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(ImportEbsdMontage)
  PYB11_FILTER_NEW_MACRO(ImportEbsdMontage)
  PYB11_PROPERTY(QString MontageName READ getMontageName WRITE setMontageName)
  PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
  PYB11_PROPERTY(EbsdMontageListInfo InputFileListInfo READ getInputFileListInfo WRITE setInputFileListInfo)
  PYB11_PROPERTY(bool GenerateIPFColorMap READ getGenerateIPFColorMap WRITE setGenerateIPFColorMap)
  PYB11_PROPERTY(QString CellIPFColorsArrayName READ getCellIPFColorsArrayName WRITE setCellIPFColorsArrayName)
  PYB11_PROPERTY(int32_t DefineScanOverlap READ getDefineScanOverlap WRITE setDefineScanOverlap)
  PYB11_PROPERTY(FloatVec2Type ScanOverlapPercent READ getScanOverlapPercent WRITE setScanOverlapPercent)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = ImportEbsdMontage;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;

  using EnumType = int32_t;
  enum class OverlapType : EnumType
  {
    None = 0,    //!<
    Pixels = 1,  //!<
    Percent = 2, //!<
  };

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
   * @brief Returns the name of the class for ImportEbsdMontage
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ImportEbsdMontage
   */
  static QString ClassName();

  ~ImportEbsdMontage() override;

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
   * @brief Setter property for MontageName
   */
  void setMontageName(const QString& value);
  /**
   * @brief Getter property for MontageName
   * @return Value of MontageName
   */
  QString getMontageName() const;
  Q_PROPERTY(QString MontageName READ getMontageName WRITE setMontageName)

  /**
   * @brief Setter property for CellEnsembleAttributeMatrixName
   */
  void setCellEnsembleAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellEnsembleAttributeMatrixName
   * @return Value of CellEnsembleAttributeMatrixName
   */
  QString getCellEnsembleAttributeMatrixName() const;
  Q_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

  /**
   * @brief Setter property for CellAttributeMatrixName
   */
  void setCellAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellAttributeMatrixName
   * @return Value of CellAttributeMatrixName
   */
  QString getCellAttributeMatrixName() const;
  Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  /**
   * @brief Setter property for InputFileListInfo
   */
  void setInputFileListInfo(const MontageFileListInfo& value);
  /**
   * @brief Getter property for InputFileListInfo
   * @return Value of InputFileListInfo
   */
  MontageFileListInfo getInputFileListInfo() const;
  Q_PROPERTY(MontageFileListInfo InputFileListInfo READ getInputFileListInfo WRITE setInputFileListInfo)

  /**
   * @brief Setter property for GenerateIPFColorMap
   */
  void setGenerateIPFColorMap(bool value);
  /**
   * @brief Getter property for GenerateIPFColorMap
   * @return Value of GenerateIPFColorMap
   */
  bool getGenerateIPFColorMap() const;
  Q_PROPERTY(bool GenerateIPFColorMap READ getGenerateIPFColorMap WRITE setGenerateIPFColorMap)

  /**
   * @brief Setter property for CellIPFColorsArrayName
   */
  void setCellIPFColorsArrayName(const QString& value);
  /**
   * @brief Getter property for CellIPFColorsArrayName
   * @return Value of CellIPFColorsArrayName
   */
  QString getCellIPFColorsArrayName() const;
  Q_PROPERTY(QString CellIPFColorsArrayName READ getCellIPFColorsArrayName WRITE setCellIPFColorsArrayName)

  /**
   * @brief Setter property for DefineScanOverlap
   */
  void setDefineScanOverlap(int32_t value);
  /**
   * @brief Getter property for DefineScanOverlap
   * @return Value of DefineScanOverlap
   */
  int32_t getDefineScanOverlap() const;
  Q_PROPERTY(int32_t DefineScanOverlap READ getDefineScanOverlap WRITE setDefineScanOverlap)

  /**
   * @brief Setter property for ScanOverlap
   */
  void setScanOverlapPercent(const FloatVec2Type& value);
  /**
   * @brief Getter property for ScanOverlap
   * @return Value of ScanOverlap
   */
  FloatVec2Type getScanOverlapPercent() const;
  Q_PROPERTY(FloatVec2Type ScanOverlapPercent READ getScanOverlapPercent WRITE setScanOverlapPercent)

  /**
   * @brief Setter property for ScanOverlap
   */
  void setScanOverlapPixel(const IntVec2Type& value);
  /**
   * @brief Getter property for ScanOverlap
   * @return Value of ScanOverlap
   */
  IntVec2Type getScanOverlapPixel() const;
  Q_PROPERTY(IntVec2Type ScanOverlapPixel READ getScanOverlapPixel WRITE setScanOverlapPixel)

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

protected:
  ImportEbsdMontage();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  QString m_MontageName = {"Montage"};
  DataArrayPath m_DataContainerName = {"EBSD", "", ""};
  QString m_CellEnsembleAttributeMatrixName = {"Phase Data"};
  QString m_CellAttributeMatrixName = {"Scan Data"};

  MontageFileListInfo m_InputFileListInfo = {};
  OverlapType m_DefineScanOverlap = OverlapType::None;
  FloatVec2Type m_ScanOverlapPercent = {0.0f, 0.0f};
  IntVec2Type m_ScanOverlapPixel = {0, 0};

  std::map<QString, AbstractFilter::Pointer> m_FilterCache;
  FloatVec3Type m_ReferenceDir = {0.0f, 0.0f, 1.0f};

  bool m_GenerateIPFColorMap = false;
  QString m_CellIPFColorsArrayName = QString(SIMPL::CellData::IPFColor);

public:
  ImportEbsdMontage(const ImportEbsdMontage&) = delete;            // Copy Constructor Not Implemented
  ImportEbsdMontage& operator=(const ImportEbsdMontage&) = delete; // Copy Assignment Not Implemented
  ImportEbsdMontage(ImportEbsdMontage&&) = delete;                 // Move Constructor Not Implemented
  ImportEbsdMontage& operator=(ImportEbsdMontage&&) = delete;      // Move Assignment Not Implemented
};
