/* ============================================================================
 * Copyright (c) 2009-2019 BlueQuartz Software, LLC
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
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/TSL/AngPhase.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

class EbsdReader;
class IDataArray;
using IDataArrayShPtrType = std::shared_ptr<IDataArray>;
class DataContainer;
// our PIMPL private class
class ImportH5OimDataPrivate;

/**
 * @brief The ImportH5OimData class. See [Filter documentation](@ref ImportH5OimData) for details.
 */
class OrientationAnalysis_EXPORT ImportH5OimData : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(ImportH5OimData SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(ImportH5OimData)
  PYB11_FILTER_NEW_MACRO(ImportH5OimData)
  PYB11_FILTER_PARAMETER(QString, InputFile)
  PYB11_FILTER_PARAMETER(QStringList, SelectedScanNames)
  PYB11_FILTER_PARAMETER(int, NumberOfScans)
  PYB11_FILTER_PARAMETER(double, ZSpacing)
  PYB11_FILTER_PARAMETER(FloatVec3Type, Origin)
  PYB11_FILTER_PARAMETER(DataArrayPath, DataContainerName)
  PYB11_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, CellAttributeMatrixName)
  PYB11_FILTER_PARAMETER(bool, ReadPatternData)
  PYB11_FILTER_PARAMETER(bool, FileWasRead)
  PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
  PYB11_PROPERTY(QStringList SelectedScanNames READ getSelectedScanNames WRITE setSelectedScanNames)
  PYB11_PROPERTY(int NumberOfScans READ getNumberOfScans WRITE setNumberOfScans)
  PYB11_PROPERTY(double ZSpacing READ getZSpacing WRITE setZSpacing)
  PYB11_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)
  PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
  PYB11_PROPERTY(bool ReadPatternData READ getReadPatternData WRITE setReadPatternData)
  PYB11_PROPERTY(bool FileWasRead READ getFileWasRead WRITE setFileWasRead)
#endif

  Q_DECLARE_PRIVATE(ImportH5OimData)

public:
  using Self = ImportH5OimData;
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
   * @brief Returns the name of the class for ImportH5OimData
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ImportH5OimData
   */
  static QString ClassName();

  ~ImportH5OimData() override;

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
   * @brief Setter property for SelectedScanNames
   */
  void setSelectedScanNames(const QStringList& value);
  /**
   * @brief Getter property for SelectedScanNames
   * @return Value of SelectedScanNames
   */
  QStringList getSelectedScanNames() const;

  Q_PROPERTY(QStringList SelectedScanNames READ getSelectedScanNames WRITE setSelectedScanNames)

  /**
   * @brief Setter property for NumberOfScans
   */
  void setNumberOfScans(int value);
  /**
   * @brief Getter property for NumberOfScans
   * @return Value of NumberOfScans
   */
  int getNumberOfScans() const;

  Q_PROPERTY(int NumberOfScans READ getNumberOfScans WRITE setNumberOfScans)

  /**
   * @brief Setter property for ZSpacing
   */
  void setZSpacing(double value);
  /**
   * @brief Getter property for ZSpacing
   * @return Value of ZSpacing
   */
  double getZSpacing() const;

  Q_PROPERTY(double ZSpacing READ getZSpacing WRITE setZSpacing)

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
   * @brief Setter property for ReadPatternData
   */
  void setReadPatternData(bool value);
  /**
   * @brief Getter property for ReadPatternData
   * @return Value of ReadPatternData
   */
  bool getReadPatternData() const;

  Q_PROPERTY(bool ReadPatternData READ getReadPatternData WRITE setReadPatternData)

  /**
   * @brief Setter property for FileWasRead
   */
  void setFileWasRead(bool value);
  /**
   * @brief Getter property for FileWasRead
   * @return Value of FileWasRead
   */
  bool getFileWasRead() const;

  Q_PROPERTY(bool FileWasRead READ getFileWasRead)

  /**
   * @brief Setter property for PhaseNameArrayName
   */
  void setPhaseNameArrayName(const QString& value);
  /**
   * @brief Getter property for PhaseNameArrayName
   * @return Value of PhaseNameArrayName
   */
  QString getPhaseNameArrayName() const;

  /**
   * @brief Setter property for MaterialNameArrayName
   */
  void setMaterialNameArrayName(const QString& value);
  /**
   * @brief Getter property for MaterialNameArrayName
   * @return Value of MaterialNameArrayName
   */
  QString getMaterialNameArrayName() const;

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

  /* These are non-exposed to the user through the GUI. Manual Pipelines are OK to set them */
  /**
   * @brief Setter property for RefFrameZDir
   */
  void setRefFrameZDir(uint32_t value);
  /**
   * @brief Getter property for RefFrameZDir
   * @return Value of RefFrameZDir
   */
  uint32_t getRefFrameZDir() const;

  /**
   * @brief Setter property for Manufacturer
   */
  void setManufacturer(const Ebsd::OEM& value);
  /**
   * @brief Getter property for Manufacturer
   * @return Value of Manufacturer
   */
  Ebsd::OEM getManufacturer() const;

  using IDataArrayMap = QMap<QString, IDataArrayShPtrType>;

  /**
   * @brief Setter property for EbsdArrayMap
   */
  void setEbsdArrayMap(const IDataArrayMap& value);
  /**
   * @brief Getter property for EbsdArrayMap
   * @return Value of EbsdArrayMap
   */
  IDataArrayMap getEbsdArrayMap() const;

  /**
   * @brief Setter property for InputFile_Cache
   */
  void setInputFile_Cache(const QString& value);
  /**
   * @brief Getter property for InputFile_Cache
   * @return Value of InputFile_Cache
   */
  QString getInputFile_Cache() const;

  /**
   * @brief Setter property for TimeStamp_Cache
   */
  void setTimeStamp_Cache(const QDateTime& value);
  /**
   * @brief Getter property for TimeStamp_Cache
   * @return Value of TimeStamp_Cache
   */
  QDateTime getTimeStamp_Cache() const;

  /**
   * @brief Setter property for Data
   */
  void setData(const Ang_Private_Data& value);
  /**
   * @brief Getter property for Data
   * @return Value of Data
   */
  Ang_Private_Data getData() const;

  Q_PROPERTY(Ang_Private_Data Data READ getData WRITE setData)

  /**
   * @brief Setter property for FileScanNames
   */
  void setFileScanNames(const QStringList& value);
  /**
   * @brief Getter property for FileScanNames
   * @return Value of FileScanNames
   */
  QStringList getFileScanNames() const;

  Q_PROPERTY(QStringList FileScanNames READ getFileScanNames WRITE setFileScanNames)

  /**
   * @brief Setter property for PatternDims
   */
  void setPatternDims(const QVector<int>& value);
  /**
   * @brief Getter property for PatternDims
   * @return Value of PatternDims
   */
  QVector<int> getPatternDims() const;

  Q_PROPERTY(QVector<int> PatternDims READ getPatternDims WRITE setPatternDims)

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

public slots:
  /**
   * @brief flushCache Resets the cache file state
   */
  void flushCache();

protected:
  ImportH5OimData();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief copyRawEbsdData Reads the H5OIM file and puts the data into the data container
   * @param reader H5OIMReader instance pointer
   * @param tDims Tuple dimensions
   * @param cDims Component dimensions
   * @param index Current slice index
   */
  virtual void copyRawEbsdData(EbsdReader* reader, std::vector<size_t>& tDims, std::vector<size_t>& cDims, int index);

  /**
   * @brief loadMaterialInfo Reads the values for the phase type, crystal structure
   * and precipitate fractions from the EBSD file
   * @param reader H5OIMReader instance pointer
   * @return Integer error value
   */
  virtual int32_t loadMaterialInfo(EbsdReader* reader);

  /**
   * @brief readDataFile Reads the H5OIM file
   * @param reader H5OIMReader instance pointer
   * @param m DataContainer instance pointer
   * @param tDims Tuple dimensions
   */
  virtual void readDataFile(EbsdReader* reader, DataContainer* m, std::vector<size_t>& tDims, const QString& scanName, ANG_READ_FLAG flag);

  /**
   * @brief readManufacturer
   * @return
   */
  virtual Ebsd::OEM readManufacturer() const;

  /**
   * @brief dataCheckEdax
   */
  virtual void dataCheckOEM();

private:
  std::weak_ptr<DataArray<int32_t>> m_CellPhasesPtr;
  int32_t* m_CellPhases = nullptr;
  std::weak_ptr<DataArray<float>> m_CellEulerAnglesPtr;
  float* m_CellEulerAngles = nullptr;
  std::weak_ptr<DataArray<uint8_t>> m_CellPatternDataPtr;
  uint8_t* m_CellPatternData = nullptr;
  std::weak_ptr<DataArray<uint32_t>> m_CrystalStructuresPtr;
  uint32_t* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<float>> m_LatticeConstantsPtr;
  float* m_LatticeConstants = nullptr;

  QString m_InputFile = {};
  QStringList m_SelectedScanNames = {};
  int m_NumberOfScans = {};
  double m_ZSpacing = {};
  FloatVec3Type m_Origin = {};
  DataArrayPath m_DataContainerName = {};
  QString m_CellEnsembleAttributeMatrixName = {};
  QString m_CellAttributeMatrixName = {};
  bool m_ReadPatternData = {};
  bool m_FileWasRead = {};
  QString m_PhaseNameArrayName = {};
  QString m_MaterialNameArrayName = {};
  uint32_t m_RefFrameZDir = {};
  Ebsd::OEM m_Manufacturer = {};
  IDataArrayMap m_EbsdArrayMap = {};

  QScopedPointer<ImportH5OimDataPrivate> const d_ptr;

public:
  ImportH5OimData(const ImportH5OimData&) = delete;            // Copy Constructor Not Implemented
  ImportH5OimData(ImportH5OimData&&) = delete;                 // Move Constructor Not Implemented
  ImportH5OimData& operator=(const ImportH5OimData&) = delete; // Copy Assignment Not Implemented
  ImportH5OimData& operator=(ImportH5OimData&&) = delete;      // Move Assignment Not Implemented
};
