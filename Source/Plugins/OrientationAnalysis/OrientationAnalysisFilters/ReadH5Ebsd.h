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
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/FilterParameters/AxisAngleInput.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "EbsdLib/H5EbsdVolumeReader.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

class H5EbsdVolumeReader;

/**
 * @brief The ReadH5Ebsd class. See [Filter documentation](@ref readh5ebsd) for details.
 */
class OrientationAnalysis_EXPORT ReadH5Ebsd : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(ReadH5Ebsd SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(ReadH5Ebsd)
  PYB11_FILTER_NEW_MACRO(ReadH5Ebsd)
  PYB11_FILTER_PARAMETER(DataArrayPath, DataContainerName)
  PYB11_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, CellAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, InputFile)
  PYB11_FILTER_PARAMETER(int, ZStartIndex)
  PYB11_FILTER_PARAMETER(int, ZEndIndex)
  PYB11_FILTER_PARAMETER(bool, UseTransformations)
  PYB11_FILTER_PARAMETER(int, AngleRepresentation)
  PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
  PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
  PYB11_PROPERTY(int ZStartIndex READ getZStartIndex WRITE setZStartIndex)
  PYB11_PROPERTY(int ZEndIndex READ getZEndIndex WRITE setZEndIndex)
  PYB11_PROPERTY(bool UseTransformations READ getUseTransformations WRITE setUseTransformations)
  PYB11_PROPERTY(int AngleRepresentation READ getAngleRepresentation WRITE setAngleRepresentation)
  PYB11_PROPERTY(QSet<QString> SelectedArrayNames READ getSelectedArrayNames WRITE setSelectedArrayNames)
#endif

public:
  using Self = ReadH5Ebsd;
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
   * @brief Returns the name of the class for ReadH5Ebsd
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for ReadH5Ebsd
   */
  static QString ClassName();

  ~ReadH5Ebsd() override;

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
   * @brief Setter property for ZStartIndex
   */
  void setZStartIndex(int value);
  /**
   * @brief Getter property for ZStartIndex
   * @return Value of ZStartIndex
   */
  int getZStartIndex() const;

  Q_PROPERTY(int ZStartIndex READ getZStartIndex WRITE setZStartIndex)

  /**
   * @brief Setter property for ZEndIndex
   */
  void setZEndIndex(int value);
  /**
   * @brief Getter property for ZEndIndex
   * @return Value of ZEndIndex
   */
  int getZEndIndex() const;

  Q_PROPERTY(int ZEndIndex READ getZEndIndex WRITE setZEndIndex)

  /**
   * @brief Setter property for UseTransformations
   */
  void setUseTransformations(bool value);
  /**
   * @brief Getter property for UseTransformations
   * @return Value of UseTransformations
   */
  bool getUseTransformations() const;

  Q_PROPERTY(bool UseTransformations READ getUseTransformations WRITE setUseTransformations)

  /**
   * @brief Setter property for SelectedArrayNames
   */
  void setSelectedArrayNames(const QSet<QString>& value);
  /**
   * @brief Getter property for SelectedArrayNames
   * @return Value of SelectedArrayNames
   */
  QSet<QString> getSelectedArrayNames() const;

  Q_PROPERTY(QSet<QString> SelectedArrayNames READ getSelectedArrayNames WRITE setSelectedArrayNames)

  /**
   * @brief Setter property for DataArrayNames
   */
  void setDataArrayNames(const QSet<QString>& value);
  /**
   * @brief Getter property for DataArrayNames
   * @return Value of DataArrayNames
   */
  QSet<QString> getDataArrayNames() const;

  Q_PROPERTY(QSet<QString> DataArrayNames READ getDataArrayNames WRITE setDataArrayNames)

  /**
   * @brief Setter property for AngleRepresentation
   */
  void setAngleRepresentation(int value);
  /**
   * @brief Getter property for AngleRepresentation
   * @return Value of AngleRepresentation
   */
  int getAngleRepresentation() const;

  Q_PROPERTY(int AngleRepresentation READ getAngleRepresentation WRITE setAngleRepresentation)

  //-------------------------------------------------------
  // Not sure why these are here. We would be reading all of these from the file
  //
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

  /**
   * @brief Setter property for SampleTransformation
   */
  void setSampleTransformation(const AxisAngleInput_t& value);
  /**
   * @brief Getter property for SampleTransformation
   * @return Value of SampleTransformation
   */
  AxisAngleInput_t getSampleTransformation() const;

  /**
   * @brief Setter property for EulerTransformation
   */
  void setEulerTransformation(const AxisAngleInput_t& value);
  /**
   * @brief Getter property for EulerTransformation
   * @return Value of EulerTransformation
   */
  AxisAngleInput_t getEulerTransformation() const;

  //-------------------------------------------------------

  /**
   * @brief Setter property for CrystalStructuresArrayName
   */
  void setCrystalStructuresArrayName(const QString& value);
  /**
   * @brief Getter property for CrystalStructuresArrayName
   * @return Value of CrystalStructuresArrayName
   */
  QString getCrystalStructuresArrayName() const;

  /**
   * @brief Setter property for LatticeConstantsArrayName
   */
  void setLatticeConstantsArrayName(const QString& value);
  /**
   * @brief Getter property for LatticeConstantsArrayName
   * @return Value of LatticeConstantsArrayName
   */
  QString getLatticeConstantsArrayName() const;

  /**
   * @brief Setter property for CellPhasesArrayName
   */
  void setCellPhasesArrayName(const QString& value);
  /**
   * @brief Getter property for CellPhasesArrayName
   * @return Value of CellPhasesArrayName
   */
  QString getCellPhasesArrayName() const;

  /**
   * @brief Setter property for CellEulerAnglesArrayName
   */
  void setCellEulerAnglesArrayName(const QString& value);
  /**
   * @brief Getter property for CellEulerAnglesArrayName
   * @return Value of CellEulerAnglesArrayName
   */
  QString getCellEulerAnglesArrayName() const;

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
  ReadH5Ebsd();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief initDataContainerDimsRes Initializes the dimensions to the correct size
   * @param dims Data container dimensions
   * @param m DataContainer instance pointer
   * @return Integer error value
   */
  int32_t initDataContainerDimsRes(int64_t dims[3], const DataContainer::Pointer& m);

  /**
   * @brief readVolumeInfo Reads the dimension information
   */
  void readVolumeInfo();

  /**
   * @brief initTSLEbsdVolumeReader Initializes the H5EbsdVolumeReader to a H5AngReader pointer
   * @return H5EbsdVolumeReader instance pointer
   */
  H5EbsdVolumeReader::Pointer initTSLEbsdVolumeReader();

  /**
   * @brief initHKLEbsdVolumeReader Initializes the H5EbsdVolumeReader to a H5CtfVolumeReader pointer
   * @return H5EbsdVolumeReader instance pointer
   */
  H5EbsdVolumeReader::Pointer initHKLEbsdVolumeReader();

  /**
   * @brief copyTSLArrays Copies the read arrays into the data container structure (TSL variant)
   * @param ebsdReader H5EbsdVolumeReader instance pointer
   */
  void copyTSLArrays(H5EbsdVolumeReader* ebsdReader);

  /**
   * @brief copyHKLArrays Copies the read arrays into the data container structure (HKL variant)
   * @param ebsdReader H5EbsdVolumeReader instance pointer
   */
  void copyHKLArrays(H5EbsdVolumeReader* ebsdReader);

  /**
  * @brief loadInfo Reads the values for the phase type, crystal structure
  * and precipitate fractions from the EBSD file.
  * @param reader EbsdReader instance pointer
  * @return Integer error value
  */
  template <typename EbsdReader, typename EbsdPhase> int32_t loadInfo(EbsdReader* reader)
  {
    DataContainer::Pointer vdc = getDataContainerArray()->getDataContainer(getDataContainerName());
    if(nullptr == vdc)
    {
      return -1;
    }
    AttributeMatrix::Pointer attrMatrix = vdc->getAttributeMatrix(getCellEnsembleAttributeMatrixName());
    if(nullptr == attrMatrix.get())
    {
      return -2;
    }

    reader->setFileName(m_InputFile);
    reader->setSliceStart(m_ZStartIndex);
    reader->setSliceEnd(m_ZEndIndex);

    QVector<typename EbsdPhase::Pointer> phases = reader->getPhases();
    if(phases.size() == 0)
    {
      setErrorCondition(reader->getErrorCode(), reader->getErrorMessage());
      return getErrorCode();
    }

    // Resize the Ensemble Attribute Matrix to be the correct number of phases.
    std::vector<size_t> tDims(1, phases.size() + 1);
    attrMatrix->resizeAttributeArrays(tDims);

    DataArray<uint32_t>::Pointer crystalStructures = DataArray<uint32_t>::CreateArray(phases.size() + 1, getCrystalStructuresArrayName(), true);
    StringDataArray::Pointer materialNames = StringDataArray::CreateArray(phases.size() + 1, getMaterialNameArrayName());
    std::vector<size_t> cDims(1, 6);
    FloatArrayType::Pointer latticeConstants = FloatArrayType::CreateArray(phases.size() + 1, cDims, getLatticeConstantsArrayName(), true);

    // Initialize the zero'th element to unknowns. The other elements will
    // be filled in based on values from the data file
    crystalStructures->setValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
    materialNames->setValue(0, "Invalid Phase");
    latticeConstants->setComponent(0, 0, 0.0f);
    latticeConstants->setComponent(0, 1, 0.0f);
    latticeConstants->setComponent(0, 2, 0.0f);
    latticeConstants->setComponent(0, 3, 0.0f);
    latticeConstants->setComponent(0, 4, 0.0f);
    latticeConstants->setComponent(0, 5, 0.0f);

    for(size_t i = 0; i < phases.size(); i++)
    {
      int32_t phaseID = phases[i]->getPhaseIndex();
      crystalStructures->setValue(phaseID, phases[i]->determineLaueGroup());
      materialNames->setValue(phaseID, phases[i]->getMaterialName());
      QVector<float> lc = phases[i]->getLatticeConstants();

      latticeConstants->setComponent(phaseID, 0, lc[0]);
      latticeConstants->setComponent(phaseID, 1, lc[1]);
      latticeConstants->setComponent(phaseID, 2, lc[2]);
      latticeConstants->setComponent(phaseID, 3, lc[3]);
      latticeConstants->setComponent(phaseID, 4, lc[4]);
      latticeConstants->setComponent(phaseID, 5, lc[5]);
    }

    attrMatrix->insertOrAssign(crystalStructures);
    m_CrystalStructuresPtr = crystalStructures;
    m_CrystalStructures = crystalStructures->getPointer(0);
    attrMatrix->insertOrAssign(latticeConstants);
    m_LatticeConstantsPtr = latticeConstants;
    m_LatticeConstants = latticeConstants->getPointer(0);
    attrMatrix->insertOrAssign(materialNames);
    m_MaterialNamesPtr = materialNames;
    return 0;
  }

private:
  std::weak_ptr<DataArray<int32_t>> m_CellPhasesPtr;
  int32_t* m_CellPhases = nullptr;
  std::weak_ptr<DataArray<float>> m_CellEulerAnglesPtr;
  float* m_CellEulerAngles = nullptr;
  std::weak_ptr<DataArray<uint32_t>> m_CrystalStructuresPtr;
  uint32_t* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<float>> m_LatticeConstantsPtr;
  float* m_LatticeConstants = nullptr;

  DataArrayPath m_DataContainerName = {};
  QString m_CellEnsembleAttributeMatrixName = {};
  QString m_CellAttributeMatrixName = {};
  QString m_PhaseNameArrayName = {};
  QString m_MaterialNameArrayName = {};
  QString m_InputFile = {};
  int m_ZStartIndex = {};
  int m_ZEndIndex = {};
  bool m_UseTransformations = {};
  QSet<QString> m_SelectedArrayNames = {};
  QSet<QString> m_DataArrayNames = {};
  int m_AngleRepresentation = {};
  uint32_t m_RefFrameZDir = {};
  Ebsd::OEM m_Manufacturer = {};
  AxisAngleInput_t m_SampleTransformation = {};
  AxisAngleInput_t m_EulerTransformation = {};
  QString m_CrystalStructuresArrayName = {};
  QString m_LatticeConstantsArrayName = {};
  QString m_CellPhasesArrayName = {};
  QString m_CellEulerAnglesArrayName = {};

  StringDataArray::WeakPointer m_MaterialNamesPtr;

public:
  ReadH5Ebsd(const ReadH5Ebsd&) = delete;            // Copy Constructor Not Implemented
  ReadH5Ebsd(ReadH5Ebsd&&) = delete;                 // Move Constructor Not Implemented
  ReadH5Ebsd& operator=(const ReadH5Ebsd&) = delete; // Copy Assignment Not Implemented
  ReadH5Ebsd& operator=(ReadH5Ebsd&&) = delete;      // Move Assignment Not Implemented
};

