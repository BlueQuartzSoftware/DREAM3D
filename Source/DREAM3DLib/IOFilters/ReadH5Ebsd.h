
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
#include <string>

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/HKL/CtfConstants.h"
#include "EbsdLib/HEDM/MicConstants.h"
#include "EbsdLib/H5EbsdVolumeReader.h"



#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/VoxelDataContainer.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/StringDataArray.hpp"

class H5EbsdVolumeReader;




/**
 * @class ReadH5Ebsd ReadH5Ebsd.h DREAM3DLib/ReconstructionFilters/ReadH5Ebsd.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT ReadH5Ebsd : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(ReadH5Ebsd)
    DREAM3D_STATIC_NEW_MACRO(ReadH5Ebsd)
    DREAM3D_TYPE_MACRO_SUPER(ReadH5Ebsd, AbstractFilter)

    virtual ~ReadH5Ebsd();

    //------ Created Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEulerAnglesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellPhasesArrayName)
    //------ Created Ensemble Data
    DREAM3D_INSTANCE_STRING_PROPERTY(CrystalStructuresArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(PhaseNameArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(MaterialNameArrayName)


    DREAM3D_INSTANCE_STRING_PROPERTY(InputFile)
    DREAM3D_INSTANCE_PROPERTY(Ebsd::RefFrameZDir, RefFrameZDir)
    DREAM3D_INSTANCE_PROPERTY(int, ZStartIndex)
    DREAM3D_INSTANCE_PROPERTY(int, ZEndIndex)
    DREAM3D_INSTANCE_PROPERTY(bool, UseTransformations)
    DREAM3D_INSTANCE_PROPERTY(Ebsd::Manufacturer, Manufacturer)
    DREAM3D_INSTANCE_PROPERTY(float, SampleTransformationAngle)
    DREAM3D_INSTANCE_PROPERTY(std::vector<float>, SampleTransformationAxis)
    DREAM3D_INSTANCE_PROPERTY(float, EulerTransformationAngle)
    DREAM3D_INSTANCE_PROPERTY(std::vector<float>, EulerTransformationAxis)


    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
    virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::InputFilters; }
    virtual const std::string getHumanLabel() { return "Read H5Ebsd File"; }

    virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();
    virtual void preflight();

    virtual void setVoxelSelectedArrayNames(std::set<std::string> selectedCellArrays,   std::set<std::string> selectedFieldArrays, std::set<std::string> selectedEnsembleArrays);
    virtual void setSurfaceMeshSelectedArrayNames(std::set<std::string> selectedVertexArrays,   std::set<std::string> selectedFaceArrays, std::set<std::string> selectedEdgeArrays);
    virtual void setSolidMeshSelectedArrayNames(std::set<std::string> selectedVertexArrays,   std::set<std::string> selectedFaceArrays, std::set<std::string> selectedEdgeArrays);

  protected:
    ReadH5Ebsd();
    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    int initDataContainerDimsRes(int64_t dims[3], VoxelDataContainer* m);

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
      reader->setFileName(m_InputFile);
      reader->setSliceStart(m_ZStartIndex);
      reader->setSliceEnd(m_ZEndIndex);

      std::vector<typename EbsdPhase::Pointer> phases = reader->getPhases();
      if (phases.size() == 0)
      {
        setErrorCondition(reader->getErrorCode());
        addErrorMessage(getHumanLabel(), reader->getErrorMessage(), getErrorCondition());
        return getErrorCondition();
      }

      DataArray<unsigned int>::Pointer crystalStructures = DataArray<unsigned int>::CreateArray(phases.size() + 1, m_CrystalStructuresArrayName);
      StringDataArray::Pointer materialNames = StringDataArray::CreateArray(phases.size() + 1, m_MaterialNameArrayName);

      // Initialize the zero'th element to unknowns. The other elements will
      // be filled in based on values from the data file
      crystalStructures->SetValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
      materialNames->SetValue(0, "Invalid Phase");

      for(size_t i=0;i<phases.size();i++)
      {
        int phaseID = phases[i]->getPhaseIndex();
        crystalStructures->SetValue(phaseID, phases[i]->determineCrystalStructure() );
        materialNames->SetValue(phaseID, phases[i]->getMaterialName());

      }
      getVoxelDataContainer()->addEnsembleData(DREAM3D::EnsembleData::CrystalStructures, crystalStructures);
      getVoxelDataContainer()->addEnsembleData(DREAM3D::EnsembleData::MaterialName, materialNames);
      getVoxelDataContainer()->setNumEnsembleTuples(crystalStructures->GetNumberOfTuples());
      return 0;
    }

  private:
    int32_t* m_CellPhases;
    float* m_CellEulerAngles;

    uint32_t* m_CrystalStructures;

    int tempxpoints;
    int tempypoints;
    int totaltemppoints;

    std::set<std::string> m_SelectedVoxelCellArrays;
    std::set<std::string> m_SelectedVoxelFieldArrays;
    std::set<std::string> m_SelectedVoxelEnsembleArrays;



    ReadH5Ebsd(const ReadH5Ebsd&); // Copy Constructor Not Implemented
    void operator=(const ReadH5Ebsd&); // Operator '=' Not Implemented
};

#endif /* ReadH5Ebsd_H_ */
