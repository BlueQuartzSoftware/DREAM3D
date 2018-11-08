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

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "EbsdLib/TSL/AngPhase.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

class H5OIMReader;

// our PIMPL private class
class ReadEdaxH5DataPrivate;

/**
 * @brief The ReadEdaxH5Data class. See [Filter documentation](@ref readedaxh5data) for details.
 */
class OrientationAnalysis_EXPORT ReadEdaxH5Data : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(ReadEdaxH5Data SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
    PYB11_PROPERTY(QStringList SelectedScanNames READ getSelectedScanNames WRITE setSelectedScanNames)
    PYB11_PROPERTY(int NumberOfScans READ getNumberOfScans WRITE setNumberOfScans)
    PYB11_PROPERTY(double ZSpacing READ getZSpacing WRITE setZSpacing)
    PYB11_PROPERTY(FloatVec3_t Origin READ getOrigin WRITE setOrigin)
    PYB11_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)
    PYB11_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)
    PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
    PYB11_PROPERTY(bool ReadPatternData READ getReadPatternData WRITE setReadPatternData)
    PYB11_PROPERTY(bool FileWasRead READ getFileWasRead WRITE setFileWasRead)
  Q_DECLARE_PRIVATE(ReadEdaxH5Data)

public:
  SIMPL_SHARED_POINTERS(ReadEdaxH5Data)
  SIMPL_FILTER_NEW_MACRO(ReadEdaxH5Data)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ReadEdaxH5Data, AbstractFilter)

  ~ReadEdaxH5Data() override;

  SIMPL_FILTER_PARAMETER(QString, InputFile)
  Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

  SIMPL_FILTER_PARAMETER(QStringList, SelectedScanNames)
  Q_PROPERTY(QStringList SelectedScanNames READ getSelectedScanNames WRITE setSelectedScanNames)

  SIMPL_FILTER_PARAMETER(int, NumberOfScans)
  Q_PROPERTY(int NumberOfScans READ getNumberOfScans WRITE setNumberOfScans)

  SIMPL_FILTER_PARAMETER(double, ZSpacing)
  Q_PROPERTY(double ZSpacing READ getZSpacing WRITE setZSpacing)

  SIMPL_FILTER_PARAMETER(FloatVec3_t, Origin)
  Q_PROPERTY(FloatVec3_t Origin READ getOrigin WRITE setOrigin)

  SIMPL_FILTER_PARAMETER(QString, DataContainerName)
  Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

  SIMPL_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
  Q_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(QString, CellAttributeMatrixName)
  Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(bool, ReadPatternData)
  Q_PROPERTY(bool ReadPatternData READ getReadPatternData WRITE setReadPatternData)

  SIMPL_FILTER_PARAMETER(bool, FileWasRead)
  Q_PROPERTY(bool FileWasRead READ getFileWasRead)

  SIMPL_INSTANCE_STRING_PROPERTY(PhaseNameArrayName)

  SIMPL_INSTANCE_STRING_PROPERTY(MaterialNameArrayName)

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

  /* These are non-exposed to the user through the GUI. Manual Pipelines are OK to set them */
  SIMPL_INSTANCE_PROPERTY(uint32_t, RefFrameZDir)

  SIMPL_INSTANCE_PROPERTY(int, Manufacturer)

  typedef QMap<QString, IDataArray::Pointer> IDataArrayMap;

  SIMPL_INSTANCE_PROPERTY(IDataArrayMap, EbsdArrayMap)

  SIMPL_PIMPL_PROPERTY_DECL(QString, InputFile_Cache)

  SIMPL_PIMPL_PROPERTY_DECL(QDateTime, TimeStamp_Cache)

  SIMPL_PIMPL_PROPERTY_DECL(Ang_Private_Data, Data)

  Q_PROPERTY(Ang_Private_Data Data READ getData WRITE setData)

  SIMPL_PIMPL_PROPERTY_DECL(QStringList, FileScanNames)
  Q_PROPERTY(QStringList FileScanNames READ getFileScanNames WRITE setFileScanNames)

  SIMPL_PIMPL_PROPERTY_DECL(QVector<int>, PatternDims)
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
  ReadEdaxH5Data();
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
  void copyRawEbsdData(H5OIMReader* reader, QVector<size_t>& tDims, QVector<size_t>& cDims, int index);

  /**
  * @brief loadMaterialInfo Reads the values for the phase type, crystal structure
  * and precipitate fractions from the EBSD file
  * @param reader H5OIMReader instance pointer
  * @return Integer error value
  */
  int32_t loadMaterialInfo(H5OIMReader* reader);

  /**
   * @brief readDataFile Reads the H5OIM file
   * @param reader H5OIMReader instance pointer
   * @param m DataContainer instance pointer
   * @param tDims Tuple dimensions
   */
  void readDataFile(H5OIMReader* reader, DataContainer::Pointer m, QVector<size_t>& tDims, const QString& scanName, ANG_READ_FLAG flag);

private:
  DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
  DEFINE_DATAARRAY_VARIABLE(float, CellEulerAngles)
  DEFINE_DATAARRAY_VARIABLE(uint8_t, CellPatternData)
  DEFINE_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)
  DEFINE_DATAARRAY_VARIABLE(float, LatticeConstants)

  QScopedPointer<ReadEdaxH5DataPrivate> const d_ptr;

public:
  ReadEdaxH5Data(const ReadEdaxH5Data&) = delete; // Copy Constructor Not Implemented
  ReadEdaxH5Data(ReadEdaxH5Data&&) = delete;      // Move Constructor Not Implemented
  ReadEdaxH5Data& operator=(const ReadEdaxH5Data&) = delete; // Copy Assignment Not Implemented
  ReadEdaxH5Data& operator=(ReadEdaxH5Data&&) = delete;      // Move Assignment Not Implemented
};

