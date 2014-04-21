
/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef ReadH5Ebsd_H_
#define ReadH5Ebsd_H_

#include <vector>
#include <QtCore/QString>

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/HKL/CtfConstants.h"
#include "EbsdLib/HEDM/MicConstants.h"
#include "EbsdLib/H5EbsdVolumeReader.h"



#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/DataArrays/StringDataArray.hpp"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"

class H5EbsdVolumeReader;




/**
 * @class ReadH5Ebsd ReadH5Ebsd.h DREAM3DLib/ReconstructionFilters/ReadH5Ebsd.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class ReadH5Ebsd : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(ReadH5Ebsd)
    DREAM3D_STATIC_NEW_MACRO(ReadH5Ebsd)
    DREAM3D_TYPE_MACRO_SUPER(ReadH5Ebsd, AbstractFilter)

    virtual ~ReadH5Ebsd();
    DREAM3D_FILTER_PARAMETER(QString, DataContainerName)
    Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)
    DREAM3D_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
    Q_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)
    DREAM3D_FILTER_PARAMETER(QString, CellAttributeMatrixName)
    Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

    DREAM3D_INSTANCE_STRING_PROPERTY(PhaseNameArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(MaterialNameArrayName)

    DREAM3D_FILTER_PARAMETER(QString, InputFile)
    Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
    DREAM3D_FILTER_PARAMETER(int, ZStartIndex)
    Q_PROPERTY(int ZStartIndex READ getZStartIndex WRITE setZStartIndex)
    DREAM3D_FILTER_PARAMETER(int, ZEndIndex)
    Q_PROPERTY(int ZEndIndex READ getZEndIndex WRITE setZEndIndex)
    DREAM3D_FILTER_PARAMETER(bool, UseTransformations)
    Q_PROPERTY(bool UseTransformations READ getUseTransformations WRITE setUseTransformations)

    DREAM3D_INSTANCE_PROPERTY(QSet<QString>, SelectedArrayNames)
    Q_PROPERTY(QSet<QString> SelectedArrayNames READ getSelectedArrayNames WRITE setSelectedArrayNames)

    DREAM3D_INSTANCE_PROPERTY(QSet<QString>, DataArrayNames) // These are for reading the names of the arrays during a preflight
    Q_PROPERTY(QSet<QString> DataArrayNames READ getDataArrayNames WRITE setDataArrayNames)

    //-------------------------------------------------------
    // Not sure why these are here. We would be reading all of these from the file
    //
    DREAM3D_INSTANCE_PROPERTY(uint32_t, RefFrameZDir)
    DREAM3D_INSTANCE_PROPERTY(int, Manufacturer)
    DREAM3D_INSTANCE_PROPERTY(AxisAngleInput_t, SampleTransformation)
    DREAM3D_INSTANCE_PROPERTY(AxisAngleInput_t, EulerTransformation)
    //-------------------------------------------------------

    virtual const QString getCompiledLibraryName() { return OrientationAnalysis::OrientationAnalysisBaseName; }
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::InputFilters; }
    virtual const QString getHumanLabel() { return "Read H5Ebsd File"; }

    virtual void setupFilterParameters();

    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    ReadH5Ebsd();
    void dataCheck();

    int initDataContainerDimsRes(int64_t dims[3], VolumeDataContainer* m);
    void readVolumeInfo();

    H5EbsdVolumeReader::Pointer initTSLEbsdVolumeReader();
    H5EbsdVolumeReader::Pointer initHKLEbsdVolumeReader();
    H5EbsdVolumeReader::Pointer initHEDMEbsdVolumeReader();

    void copyTSLArrays(H5EbsdVolumeReader* ebsdReader);
    void copyHKLArrays(H5EbsdVolumeReader* ebsdReader);
    void copyHEDMArrays(H5EbsdVolumeReader* ebsdReader);


    /**
    * @brief This method reads the values for the phase type, crystal structure
    * and precipitate fractions from the EBSD file.
    * @param reader The EbsdReader instance
    * @param precipFractions Container to hold the precipitate fractions (out)
    * @param crystalStructures Container to hold the crystal structures (out)
    * @return Zero/Positive on Success - Negative on error.
    */
    template<typename EbsdReader, typename EbsdPhase>
    int loadInfo(EbsdReader* reader)
    {
      VolumeDataContainer* vdc = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
      if(NULL == vdc) { return -1;}
      AttributeMatrix::Pointer attrMatrix = vdc->getAttributeMatrix(getCellEnsembleAttributeMatrixName() );
      if(NULL == attrMatrix.get() ) { return -2; }

      reader->setFileName(m_InputFile);
      reader->setSliceStart(m_ZStartIndex);
      reader->setSliceEnd(m_ZEndIndex);

      QVector<typename EbsdPhase::Pointer> phases = reader->getPhases();
      if (phases.size() == 0)
      {
        setErrorCondition(reader->getErrorCode());
        notifyErrorMessage(getHumanLabel(), reader->getErrorMessage(), getErrorCondition());
        return getErrorCondition();
      }

      // Resize the Ensemble Attribute Matrix to be the correct number of phases.
      QVector<size_t> tDims(1, phases.size() + 1);
      attrMatrix->resizeAttributeArrays(tDims);

      DataArray<unsigned int>::Pointer crystalStructures = DataArray<unsigned int>::CreateArray(phases.size() + 1, getCrystalStructuresArrayName());
      StringDataArray::Pointer materialNames = StringDataArray::CreateArray(phases.size() + 1, getMaterialNameArrayName());
      QVector<size_t> cDims(1, 6);
      FloatArrayType::Pointer latticeConstants = FloatArrayType::CreateArray(phases.size() + 1, cDims, getLatticeConstantsArrayName());

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
        int phaseID = phases[i]->getPhaseIndex();
        crystalStructures->setValue(phaseID, phases[i]->determineCrystalStructure() );
        materialNames->setValue(phaseID, phases[i]->getMaterialName());
        QVector<float> lc = phases[i]->getLatticeConstants();

        latticeConstants->setComponent(phaseID, 0, lc[0]);
        latticeConstants->setComponent(phaseID, 1, lc[1]);
        latticeConstants->setComponent(phaseID, 2, lc[2]);
        latticeConstants->setComponent(phaseID, 3, lc[3]);
        latticeConstants->setComponent(phaseID, 4, lc[4]);
        latticeConstants->setComponent(phaseID, 5, lc[5]);

      }


      attrMatrix->addAttributeArray(DREAM3D::EnsembleData::CrystalStructures, crystalStructures);
      attrMatrix->addAttributeArray(DREAM3D::EnsembleData::LatticeConstants, latticeConstants);
      attrMatrix->addAttributeArray(DREAM3D::EnsembleData::MaterialName, materialNames);
      return 0;
    }

  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, CellPhases)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, CellEulerAngles)
    DEFINE_PTR_WEAKPTR_DATAARRAY(uint32_t, CrystalStructures)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, LatticeConstants)
    StringDataArray::WeakPointer  m_MaterialNamesPtr;

    int tempxpoints;
    int tempypoints;
    int totaltemppoints;

    ReadH5Ebsd(const ReadH5Ebsd&); // Copy Constructor Not Implemented
    void operator=(const ReadH5Ebsd&); // Operator '=' Not Implemented
};

#endif /* ReadH5Ebsd_H_ */



