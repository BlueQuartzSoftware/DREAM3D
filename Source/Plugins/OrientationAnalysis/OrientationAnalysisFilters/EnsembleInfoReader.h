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
#include "SIMPLib/Common/PhaseType.h"
#include "SIMPLib/CoreFilters/FileReader.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The EnsembleInfoReader class. See [Filter documentation](@ref dxreader) for details.
 */
class OrientationAnalysis_EXPORT EnsembleInfoReader : public FileReader
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(EnsembleInfoReader SUPERCLASS FileReader)
  PYB11_SHARED_POINTERS(EnsembleInfoReader)
  PYB11_FILTER_NEW_MACRO(EnsembleInfoReader)
  PYB11_FILTER_PARAMETER(DataArrayPath, DataContainerName)
  PYB11_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, InputFile)
  PYB11_FILTER_PARAMETER(QString, CrystalStructuresArrayName)
  PYB11_FILTER_PARAMETER(QString, PhaseTypesArrayName)
  PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)
  PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
  PYB11_PROPERTY(QString CrystalStructuresArrayName READ getCrystalStructuresArrayName WRITE setCrystalStructuresArrayName)
  PYB11_PROPERTY(QString PhaseTypesArrayName READ getPhaseTypesArrayName WRITE setPhaseTypesArrayName)
#endif

public:
  using Self = EnsembleInfoReader;
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
   * @brief Returns the name of the class for EnsembleInfoReader
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for EnsembleInfoReader
   */
  static QString ClassName();

  virtual ~EnsembleInfoReader();

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
   * @brief Setter property for CrystalStructuresArrayName
   */
  void setCrystalStructuresArrayName(const QString& value);
  /**
   * @brief Getter property for CrystalStructuresArrayName
   * @return Value of CrystalStructuresArrayName
   */
  QString getCrystalStructuresArrayName() const;

  Q_PROPERTY(QString CrystalStructuresArrayName READ getCrystalStructuresArrayName WRITE setCrystalStructuresArrayName)

  /**
   * @brief Setter property for PhaseTypesArrayName
   */
  void setPhaseTypesArrayName(const QString& value);
  /**
   * @brief Getter property for PhaseTypesArrayName
   * @return Value of PhaseTypesArrayName
   */
  QString getPhaseTypesArrayName() const;

  Q_PROPERTY(QString PhaseTypesArrayName READ getPhaseTypesArrayName WRITE setPhaseTypesArrayName)

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
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  void preflight() override;

protected:
  EnsembleInfoReader();

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
   * @brief updateEnsembleInstancePointers Updates raw ensemble pointers
   */
  void updateEnsembleInstancePointers();

  /**
   * @brief ensembleLookup Assings the crystal structure and phase type enumerations
   * based on the incoming file
   * @param values QStringList of incoming file declarations
   */
  void ensembleLookup(QStringList values);

private:
  std::weak_ptr<DataArray<uint32_t>> m_CrystalStructuresPtr;
  uint32_t* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<PhaseType::EnumType>> m_PhaseTypesPtr;
  PhaseType::EnumType* m_PhaseTypes = nullptr;

  DataArrayPath m_DataContainerName = {};
  QString m_CellEnsembleAttributeMatrixName = {};
  QString m_InputFile = {};
  QString m_CrystalStructuresArrayName = {};
  QString m_PhaseTypesArrayName = {};

  PhaseType::Type m_ptype;
  uint32_t m_crystruct;

public:
  EnsembleInfoReader(const EnsembleInfoReader&) = delete;            // Copy Constructor Not Implemented
  EnsembleInfoReader(EnsembleInfoReader&&) = delete;                 // Move Constructor Not Implemented
  EnsembleInfoReader& operator=(const EnsembleInfoReader&) = delete; // Copy Assignment Not Implemented
  EnsembleInfoReader& operator=(EnsembleInfoReader&&) = delete;      // Move Assignment Not Implemented
};

