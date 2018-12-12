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

#include <QtCore/QFile>

// Needed for AxisAngle_t
#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/HKL/DataParser.hpp"

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
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
  PYB11_CREATE_BINDINGS(SPParksDumpReader SUPERCLASS FileReader)
  PYB11_PROPERTY(QString VolumeDataContainerName READ getVolumeDataContainerName WRITE setVolumeDataContainerName)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
  PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
  PYB11_PROPERTY(FloatVec3_t Origin READ getOrigin WRITE setOrigin)
  PYB11_PROPERTY(FloatVec3_t Resolution READ getResolution WRITE setResolution)
  PYB11_PROPERTY(bool OneBasedArrays READ getOneBasedArrays WRITE setOneBasedArrays)
  PYB11_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

public:
  SIMPL_SHARED_POINTERS(SPParksDumpReader)
  SIMPL_FILTER_NEW_MACRO(SPParksDumpReader)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(SPParksDumpReader, FileReader)

  ~SPParksDumpReader() override;

  SIMPL_FILTER_PARAMETER(QString, VolumeDataContainerName)
  Q_PROPERTY(QString VolumeDataContainerName READ getVolumeDataContainerName WRITE setVolumeDataContainerName)

  SIMPL_FILTER_PARAMETER(QString, CellAttributeMatrixName)
  Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(QString, InputFile)
  Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

  SIMPL_FILTER_PARAMETER(FloatVec3_t, Origin)
  Q_PROPERTY(FloatVec3_t Origin READ getOrigin WRITE setOrigin)

  SIMPL_FILTER_PARAMETER(FloatVec3_t, Resolution)
  Q_PROPERTY(FloatVec3_t Resolution READ getResolution WRITE setResolution)

  SIMPL_FILTER_PARAMETER(bool, OneBasedArrays)
  Q_PROPERTY(bool OneBasedArrays READ getOneBasedArrays WRITE setOneBasedArrays)

  SIMPL_FILTER_PARAMETER(QString, FeatureIdsArrayName)
  Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

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
  void parseDataLine(QByteArray& line, QVector<size_t> dims, int64_t xCol, int64_t yCol, int64_t zCol, size_t lineNum);

private:
  QFile m_InStream;
  QMap<QString, DataParser::Pointer> m_NamePointerMap;
  ImageGeom* m_CachedGeometry = nullptr;

public:
  SPParksDumpReader(const SPParksDumpReader&) = delete;            // Copy Constructor Not Implemented
  SPParksDumpReader(SPParksDumpReader&&) = delete;                 // Move Constructor Not Implemented
  SPParksDumpReader& operator=(const SPParksDumpReader&) = delete; // Copy Assignment Not Implemented
  SPParksDumpReader& operator=(SPParksDumpReader&&) = delete;      // Move Assignment Not Implemented
};
