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
#include "SIMPLib/FilterParameters/AxisAngleInput.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "EbsdLib/H5EbsdVolumeReader.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

class H5EbsdVolumeReader;

/**
 * @brief The ReadH5Ebsd class. See [Filter documentation](@ref readh5ebsd) for details.
 */
class OrientationAnalysis_EXPORT ReadH5Ebsd : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(ReadH5Ebsd SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
    PYB11_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)
    PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
    PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
    PYB11_PROPERTY(int ZStartIndex READ getZStartIndex WRITE setZStartIndex)
    PYB11_PROPERTY(int ZEndIndex READ getZEndIndex WRITE setZEndIndex)
    PYB11_PROPERTY(bool UseTransformations READ getUseTransformations WRITE setUseTransformations)
    PYB11_PROPERTY(int AngleRepresentation READ getAngleRepresentation WRITE setAngleRepresentation)
    PYB11_PROPERTY(QSet<QString> SelectedArrayNames READ getSelectedArrayNames WRITE setSelectedArrayNames)

public:
  SIMPL_SHARED_POINTERS(ReadH5Ebsd)
  SIMPL_FILTER_NEW_MACRO(ReadH5Ebsd)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ReadH5Ebsd, AbstractFilter)

  ~ReadH5Ebsd() override;

  SIMPL_FILTER_PARAMETER(DataArrayPath, DataContainerName)
  Q_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)

  SIMPL_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
  Q_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(QString, CellAttributeMatrixName)
  Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  SIMPL_INSTANCE_STRING_PROPERTY(PhaseNameArrayName)

  SIMPL_INSTANCE_STRING_PROPERTY(MaterialNameArrayName)

  SIMPL_FILTER_PARAMETER(QString, InputFile)
  Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

  SIMPL_FILTER_PARAMETER(int, ZStartIndex)
  Q_PROPERTY(int ZStartIndex READ getZStartIndex WRITE setZStartIndex)

  SIMPL_FILTER_PARAMETER(int, ZEndIndex)
  Q_PROPERTY(int ZEndIndex READ getZEndIndex WRITE setZEndIndex)

  SIMPL_FILTER_PARAMETER(bool, UseTransformations)
  Q_PROPERTY(bool UseTransformations READ getUseTransformations WRITE setUseTransformations)

  SIMPL_INSTANCE_PROPERTY(QSet<QString>, SelectedArrayNames)
  Q_PROPERTY(QSet<QString> SelectedArrayNames READ getSelectedArrayNames WRITE setSelectedArrayNames)

  SIMPL_INSTANCE_PROPERTY(QSet<QString>, DataArrayNames) // These are for reading the names of the arrays during a preflight
  Q_PROPERTY(QSet<QString> DataArrayNames READ getDataArrayNames WRITE setDataArrayNames)

  SIMPL_FILTER_PARAMETER(int, AngleRepresentation)
  Q_PROPERTY(int AngleRepresentation READ getAngleRepresentation WRITE setAngleRepresentation)

  //-------------------------------------------------------
  // Not sure why these are here. We would be reading all of these from the file
  //
  SIMPL_INSTANCE_PROPERTY(uint32_t, RefFrameZDir)
  SIMPL_INSTANCE_PROPERTY(Ebsd::OEM, Manufacturer)
  SIMPL_INSTANCE_PROPERTY(AxisAngleInput_t, SampleTransformation)
  SIMPL_INSTANCE_PROPERTY(AxisAngleInput_t, EulerTransformation)
  //-------------------------------------------------------

  SIMPL_INSTANCE_PROPERTY(QString, CrystalStructuresArrayName)
  SIMPL_INSTANCE_PROPERTY(QString, LatticeConstantsArrayName)
  SIMPL_INSTANCE_PROPERTY(QString, CellPhasesArrayName)
  SIMPL_INSTANCE_PROPERTY(QString, CellEulerAnglesArrayName)

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
  int32_t initDataContainerDimsRes(int64_t dims[3], DataContainer::Pointer m);

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
  DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
  DEFINE_DATAARRAY_VARIABLE(float, CellEulerAngles)
  DEFINE_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)
  DEFINE_DATAARRAY_VARIABLE(float, LatticeConstants)

  StringDataArray::WeakPointer m_MaterialNamesPtr;

public:
  ReadH5Ebsd(const ReadH5Ebsd&) = delete;            // Copy Constructor Not Implemented
  ReadH5Ebsd(ReadH5Ebsd&&) = delete;                 // Move Constructor Not Implemented
  ReadH5Ebsd& operator=(const ReadH5Ebsd&) = delete; // Copy Assignment Not Implemented
  ReadH5Ebsd& operator=(ReadH5Ebsd&&) = delete;      // Move Assignment Not Implemented
};

