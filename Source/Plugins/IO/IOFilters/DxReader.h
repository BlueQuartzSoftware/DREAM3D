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

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/FileReader.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

// our PIMPL private class
class DxReaderPrivate;

#include "IO/IODLLExport.h"

/**
 * @brief The DxReader class. See [Filter documentation](@ref dxreader) for details.
 */
class IO_EXPORT DxReader : public FileReader
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(DxReader SUPERCLASS FileReader)
    PYB11_PROPERTY(QString VolumeDataContainerName READ getVolumeDataContainerName WRITE setVolumeDataContainerName)
    PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
    PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
    PYB11_PROPERTY(FloatVec3_t Origin READ getOrigin WRITE setOrigin)
    PYB11_PROPERTY(FloatVec3_t Resolution READ getResolution WRITE setResolution)
    PYB11_PROPERTY(bool FileWasRead READ getFileWasRead WRITE setFileWasRead)
    PYB11_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)
  Q_DECLARE_PRIVATE(DxReader)
public:
  SIMPL_SHARED_POINTERS(DxReader)
  SIMPL_FILTER_NEW_MACRO(DxReader)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(DxReader, FileReader)

  ~DxReader() override;

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

  SIMPL_FILTER_PARAMETER(bool, FileWasRead)
  Q_PROPERTY(bool FileWasRead READ getFileWasRead)

  SIMPL_FILTER_PARAMETER(QString, FeatureIdsArrayName)
  Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

  SIMPL_PIMPL_PROPERTY_DECL(QVector<size_t>, Dims)
  SIMPL_PIMPL_PROPERTY_DECL(QString, InputFile_Cache)
  SIMPL_PIMPL_PROPERTY_DECL(QDateTime, LastRead)

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

public slots:
  /**
   * @brief flushCache Clears the input file cache
   */
  void flushCache();

protected:
  DxReader();

  /**
   * @brief readHeader Reimplemented from @see FileReader class
   */
  virtual int32_t readHeader();

  /**
   * @brief readFile Reimplemented from @see FileReader class
   */
  virtual int32_t readFile();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief updateCellInstancePointers Updates raw cell pointers
   */
  void updateCellInstancePointers();

private:
  QScopedPointer<DxReaderPrivate> const d_ptr;

  DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
  size_t m_Dims[3];
  QFile m_InStream;

public:
  DxReader(const DxReader&) = delete;            // Copy Constructor Not Implemented
  DxReader(DxReader&&) = delete;                 // Move Constructor Not Implemented
  DxReader& operator=(const DxReader&) = delete; // Copy Assignment Not Implemented
  DxReader& operator=(DxReader&&) = delete;      // Move Assignment Not Implemented
};

