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

#include <QtCore/QFile>

// Needed for AxisAngle_t
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/HKL/DataParser.hpp"

#include "SIMPLib/CoreFilters/FileReader.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

// Forward Declare classes.
class ImageGeom;

#include "ImportExport/ImportExportDLLExport.h"

/**
 * @brief The SPParksDumpReader class. See [Filter documentation](@ref spparkstextreader) for details.
 */
class ImportExport_EXPORT SPParksDumpReader : public FileReader
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(SPParksDumpReader SUPERCLASS FileReader)
  PYB11_SHARED_POINTERS(SPParksDumpReader)
  PYB11_FILTER_NEW_MACRO(SPParksDumpReader)
  PYB11_FILTER_PARAMETER(DataArrayPath, VolumeDataContainerName)
  PYB11_FILTER_PARAMETER(QString, CellAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, InputFile)
  PYB11_FILTER_PARAMETER(FloatVec3Type, Origin)
  PYB11_FILTER_PARAMETER(FloatVec3Type, Spacing)
  PYB11_FILTER_PARAMETER(bool, OneBasedArrays)
  PYB11_FILTER_PARAMETER(QString, FeatureIdsArrayName)
  PYB11_PROPERTY(DataArrayPath VolumeDataContainerName READ getVolumeDataContainerName WRITE setVolumeDataContainerName)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
  PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
  PYB11_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)
  PYB11_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)
  PYB11_PROPERTY(bool OneBasedArrays READ getOneBasedArrays WRITE setOneBasedArrays)
  PYB11_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)
#endif

public:
  using Self = SPParksDumpReader;
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
   * @brief Returns the name of the class for SPParksDumpReader
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for SPParksDumpReader
   */
  static QString ClassName();

  ~SPParksDumpReader() override;

  /**
   * @brief Setter property for VolumeDataContainerName
   */
  void setVolumeDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for VolumeDataContainerName
   * @return Value of VolumeDataContainerName
   */
  DataArrayPath getVolumeDataContainerName() const;

  Q_PROPERTY(DataArrayPath VolumeDataContainerName READ getVolumeDataContainerName WRITE setVolumeDataContainerName)

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
   * @brief Setter property for Origin
   */
  void setOrigin(const FloatVec3Type& value);
  /**
   * @brief Getter property for Origin
   * @return Value of Origin
   */
  FloatVec3Type getOrigin() const;

  Q_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)

  /**
   * @brief Setter property for Spacing
   */
  void setSpacing(const FloatVec3Type& value);
  /**
   * @brief Getter property for Spacing
   * @return Value of Spacing
   */
  FloatVec3Type getSpacing() const;

  Q_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)

  /**
   * @brief Setter property for OneBasedArrays
   */
  void setOneBasedArrays(bool value);
  /**
   * @brief Getter property for OneBasedArrays
   * @return Value of OneBasedArrays
   */
  bool getOneBasedArrays() const;

  Q_PROPERTY(bool OneBasedArrays READ getOneBasedArrays WRITE setOneBasedArrays)

  /**
   * @brief Setter property for FeatureIdsArrayName
   */
  void setFeatureIdsArrayName(const QString& value);
  /**
   * @brief Getter property for FeatureIdsArrayName
   * @return Value of FeatureIdsArrayName
   */
  QString getFeatureIdsArrayName() const;

  Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

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
  SPParksDumpReader();

  /**
   * @brief readHeader Reimplemented from @see FileReader class
   */
  int32_t readHeader() override;

  /**
   * @brief readFile Reimplemented from @see FileReader class
   */
  int32_t readFile() override;

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief updateCellInstancePointers Updates raw cell pointers
   */
  void updateCellInstancePointers();

  /**
   * @brief getPointerType Returns the enumeration type of the incoming data
   * @param featureName Name of incoming data
   * @return Enumeration type
   */
  Ebsd::NumType getPointerType(const QString& featureName);

  /**
   * @brief getTypeSize Returns the size in bytes of the incoming data
   * @param featureName Name of incoming data
   * @return Integer number of bytes
   */
  int32_t getTypeSize(const QString& featureName);

  /**
   * @brief parseDataLine Reads a line of data from the .dump file
   * @param line Line of data to read
   * @param dims Dimensional offset
   * @param xCol X coordinate
   * @param yCol Y coordinate
   * @param zCol Z coordinate
   * @param lineNum
   */
  void parseDataLine(QByteArray& line, std::vector<size_t> dims, int64_t xCol, int64_t yCol, int64_t zCol, size_t lineNum);

private:
  DataArrayPath m_VolumeDataContainerName = {};
  QString m_CellAttributeMatrixName = {};
  QString m_InputFile = {};
  FloatVec3Type m_Origin = {};
  FloatVec3Type m_Spacing = {};
  bool m_OneBasedArrays = {};
  QString m_FeatureIdsArrayName = {};

  QFile m_InStream;
  QMap<QString, DataParser::Pointer> m_NamePointerMap;
  ImageGeom* m_CachedGeometry = nullptr;

public:
  SPParksDumpReader(const SPParksDumpReader&) = delete;            // Copy Constructor Not Implemented
  SPParksDumpReader(SPParksDumpReader&&) = delete;                 // Move Constructor Not Implemented
  SPParksDumpReader& operator=(const SPParksDumpReader&) = delete; // Copy Assignment Not Implemented
  SPParksDumpReader& operator=(SPParksDumpReader&&) = delete;      // Move Assignment Not Implemented
};
