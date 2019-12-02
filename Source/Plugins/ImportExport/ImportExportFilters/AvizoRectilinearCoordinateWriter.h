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

#include <stdio.h>

#include <QtCore/QString>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "ImportExport/ImportExportDLLExport.h"

/**
 * @class AvizoRectilinearCoordinateWriter AvizoRectilinearCoordinateWriter.h DREAM3DLib/IOFilters/AvizoRectilinearCoordinateWriter.h
 * @brief Writes out a native Avizo Rectilinear Coordinate file
 *
 * @date Aug 9, 2012
 * @version 1.0
 */
class ImportExport_EXPORT AvizoRectilinearCoordinateWriter : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(AvizoRectilinearCoordinateWriter SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(AvizoRectilinearCoordinateWriter)
  PYB11_FILTER_NEW_MACRO(AvizoRectilinearCoordinateWriter)
  PYB11_FILTER_PARAMETER(QString, OutputFile)
  PYB11_FILTER_PARAMETER(bool, WriteBinaryFile)
  PYB11_FILTER_PARAMETER(QString, Units)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  PYB11_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)
  PYB11_PROPERTY(bool WriteBinaryFile READ getWriteBinaryFile WRITE setWriteBinaryFile)
  PYB11_PROPERTY(QString Units READ getUnits WRITE setUnits)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
#endif

public:
  using Self = AvizoRectilinearCoordinateWriter;
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
   * @brief Returns the name of the class for AvizoRectilinearCoordinateWriter
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for AvizoRectilinearCoordinateWriter
   */
  static QString ClassName();

  ~AvizoRectilinearCoordinateWriter() override;

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
   * @brief Setter property for WriteBinaryFile
   */
  void setWriteBinaryFile(bool value);
  /**
   * @brief Getter property for WriteBinaryFile
   * @return Value of WriteBinaryFile
   */
  bool getWriteBinaryFile() const;

  Q_PROPERTY(bool WriteBinaryFile READ getWriteBinaryFile WRITE setWriteBinaryFile)

  /**
   * @brief Setter property for Units
   */
  void setUnits(const QString& value);
  /**
   * @brief Getter property for Units
   * @return Value of Units
   */
  QString getUnits() const;

  Q_PROPERTY(QString Units READ getUnits WRITE setUnits)

  /**
   * @brief Setter property for WriteFeatureIds
   */
  void setWriteFeatureIds(bool value);
  /**
   * @brief Getter property for WriteFeatureIds
   * @return Value of WriteFeatureIds
   */
  bool getWriteFeatureIds() const;

  /**
   * @brief This returns the group that the filter belonds to. You can select
   * a different group if you want. The string returned here will be displayed
   * in the GUI for the filter
   */
  /**
   * @brief Setter property for FeatureIdsArrayPath
   */
  void setFeatureIdsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureIdsArrayPath
   * @return Value of FeatureIdsArrayPath
   */
  DataArrayPath getFeatureIdsArrayPath() const;

  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

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
  AvizoRectilinearCoordinateWriter();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief Generates the Avizo Header for this file
   * @return The header as a string
   */
  void generateHeader(FILE* f);

  /**
   * @brief Writes the data to the Avizo file
   * @param f
   * @return Error code
   */
  int writeData(FILE* f);

private:
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;

  QString m_OutputFile = {};
  bool m_WriteBinaryFile = {};
  QString m_Units = {};
  bool m_WriteFeatureIds = {};
  DataArrayPath m_FeatureIdsArrayPath = {};

public:
  AvizoRectilinearCoordinateWriter(const AvizoRectilinearCoordinateWriter&) = delete;            // Copy Constructor Not Implemented
  AvizoRectilinearCoordinateWriter(AvizoRectilinearCoordinateWriter&&) = delete;                 // Move Constructor Not Implemented
  AvizoRectilinearCoordinateWriter& operator=(const AvizoRectilinearCoordinateWriter&) = delete; // Copy Assignment Not Implemented
  AvizoRectilinearCoordinateWriter& operator=(AvizoRectilinearCoordinateWriter&&) = delete;      // Move Assignment Not Implemented
};
