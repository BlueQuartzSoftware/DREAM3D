/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef _ReadOrientationData_H_
#define _ReadOrientationData_H_

#include <QtCore/QString>

#include "EbsdLib/EbsdReader.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/DataArrays/StringDataArray.hpp"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"


/**
 * @class ReadOrientationData ReadOrientationData.h /FilterCategoryFilters/ReadOrientationData.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT ReadOrientationData : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(ReadOrientationData)
    DREAM3D_STATIC_NEW_MACRO(ReadOrientationData)
    DREAM3D_TYPE_MACRO_SUPER(ReadOrientationData, AbstractFilter)

    virtual ~ReadOrientationData();
    DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEnsembleAttributeMatrixName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellAttributeMatrixName)

    DREAM3D_INSTANCE_STRING_PROPERTY(PhaseNameArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(MaterialNameArrayName)

    DREAM3D_INSTANCE_STRING_PROPERTY(InputFile)
    DREAM3D_INSTANCE_PROPERTY(Ebsd::RefFrameZDir, RefFrameZDir)
    DREAM3D_INSTANCE_PROPERTY(Ebsd::Manufacturer, Manufacturer)

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::IOFilters; }

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getSubGroupName() { return DREAM3D::FilterSubGroups::InputFilters; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel() { return "Read Orientation Data"; }

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
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

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight();

  protected:
    ReadOrientationData();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();

    /**
     * @brief readAngFile This reads the Ang file and puts the data into the Voxel Data container
     */
    void readAngFile();

    /**
     * @brief readCtfFile This reads the Ctf file and puts the data into the Voxel Data container
     */
    void readCtfFile();

    /**
     * @brief readMicFile This reads the HEDM .mic file and puts the data into the Voxel Data container
     */
    void readMicFile();



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


      QVector<typename EbsdPhase::Pointer> phases = reader->getPhaseVector();
      if (phases.size() == 0)
      {
        setErrorCondition(reader->getErrorCode());
        addErrorMessage(getHumanLabel(), reader->getErrorMessage(), getErrorCondition());
        return getErrorCondition();
      }

      DataArray<unsigned int>::Pointer crystalStructures = DataArray<unsigned int>::CreateArray(phases.size() + 1, m_CrystalStructuresArrayName);
      StringDataArray::Pointer materialNames = StringDataArray::CreateArray(phases.size() + 1, m_MaterialNameArrayName);
      QVector<int> dims(1, 6);
      FloatArrayType::Pointer latticeConstants = FloatArrayType::CreateArray(phases.size() + 1, dims, m_LatticeConstantsArrayName);

      // Initialize the zero'th element to unknowns. The other elements will
      // be filled in based on values from the data file
      crystalStructures->SetValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
      materialNames->SetValue(0, "Invalid Phase");
      latticeConstants->SetComponent(0, 0, 0.0f);
      latticeConstants->SetComponent(0, 1, 0.0f);
      latticeConstants->SetComponent(0, 2, 0.0f);
      latticeConstants->SetComponent(0, 3, 0.0f);
      latticeConstants->SetComponent(0, 4, 0.0f);
      latticeConstants->SetComponent(0, 5, 0.0f);

      for(size_t i = 0; i < phases.size(); i++)
      {
        int phaseID = phases[i]->getPhaseIndex();
        crystalStructures->SetValue(phaseID, phases[i]->determineCrystalStructure() );
        materialNames->SetValue(phaseID, phases[i]->getMaterialName());
        QVector<float> lc = phases[i]->getLatticeConstants();

        latticeConstants->SetComponent(phaseID, 0, lc[0]);
        latticeConstants->SetComponent(phaseID, 1, lc[1]);
        latticeConstants->SetComponent(phaseID, 2, lc[2]);
        latticeConstants->SetComponent(phaseID, 3, lc[3]);
        latticeConstants->SetComponent(phaseID, 4, lc[4]);
        latticeConstants->SetComponent(phaseID, 5, lc[5]);

      }
      VolumeDataContainer* vdc = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
      if(NULL == vdc) { return -1;}
      AttributeMatrix::Pointer attrMatrix = vdc->getAttributeMatrix(getCellEnsembleAttributeMatrixName() );
      if(NULL == attrMatrix.get() ) { return -2; }

      attrMatrix->addAttributeArray(DREAM3D::EnsembleData::CrystalStructures, crystalStructures);
      attrMatrix->addAttributeArray(DREAM3D::EnsembleData::MaterialName, materialNames);
      attrMatrix->addAttributeArray(DREAM3D::EnsembleData::LatticeConstants, latticeConstants);
      return 0;
    }

  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, CellPhases)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, CellEulerAngles)
    DEFINE_PTR_WEAKPTR_DATAARRAY(uint32_t, CrystalStructures)
    DEFINE_PTR_WEAKPTR_DATAARRAY(float, LatticeConstants)

    ReadOrientationData(const ReadOrientationData&); // Copy Constructor Not Implemented
    void operator=(const ReadOrientationData&); // Operator '=' Not Implemented
};

#endif /* _ReadOrientationData_H_ */
