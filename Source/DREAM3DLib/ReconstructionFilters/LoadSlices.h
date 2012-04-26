
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

#ifndef LOADSLICES_H_
#define LOADSLICES_H_

#include <vector>
#include <string>

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/HKL/CtfConstants.h"


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DataContainer.h"
#include "DREAM3DLib/Common/QualityMetricFilter.h"
#include "DREAM3DLib/Common/OrientationMath.h"


/**
 * @class LoadSlices LoadSlices.h DREAM3DLib/ReconstructionFilters/LoadSlices.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT LoadSlices : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(LoadSlices);
    DREAM3D_STATIC_NEW_MACRO(LoadSlices);
    DREAM3D_TYPE_MACRO_SUPER(LoadSlices, AbstractFilter);

    virtual ~LoadSlices();

	//------ Created Cell Data
	DREAM3D_INSTANCE_STRING_PROPERTY(CellEulerAnglesArrayName)
	DREAM3D_INSTANCE_STRING_PROPERTY(CellPhasesArrayName)
	//------ Created Ensemble Data
	DREAM3D_INSTANCE_STRING_PROPERTY(CrystalStructuresArrayName)

    DREAM3D_INSTANCE_STRING_PROPERTY(H5EbsdFile)
    DREAM3D_INSTANCE_PROPERTY(Ebsd::RefFrameZDir, RefFrameZDir)
    DREAM3D_INSTANCE_PROPERTY(int, ZStartIndex);
    DREAM3D_INSTANCE_PROPERTY(int, ZEndIndex);
    DREAM3D_INSTANCE_PROPERTY(DataArray<unsigned int>::Pointer, PhaseTypes)
    DREAM3D_INSTANCE_PROPERTY(std::vector<QualityMetricFilter::Pointer>, QualityMetricFilters)


    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::ReconstructionFilters; }
    virtual const std::string getHumanLabel() { return "Load EBSD Slices"; }

	virtual void writeFilterOptions(AbstractFilterOptionsWriter* writer);

	/**
     * @brief Reimplemented from @see AbstractFilter class
     */
    virtual void execute();
    virtual void preflight();

  protected:
    LoadSlices();

  private:
    int32_t* m_CellPhases;
    float* m_CellEulerAngles;

    unsigned int* m_CrystalStructures;

    unsigned long long int Seed;

    int tempxpoints;
    int tempypoints;
    int totaltemppoints;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    LoadSlices(const LoadSlices&); // Copy Constructor Not Implemented
    void operator=(const LoadSlices&); // Operator '=' Not Implemented

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
      reader->setFileName(m_H5EbsdFile);
      reader->setSliceStart(m_ZStartIndex);
      reader->setSliceEnd(m_ZEndIndex);

      std::vector<typename EbsdPhase::Pointer> phases = reader->getPhases();
      if (phases.size() == 0)
      {
        return -1;
      }

      DataArray<unsigned int>::Pointer crystalStructures = DataArray<unsigned int>::CreateArray(phases.size() + 1, DREAM3D::EnsembleData::CrystalStructures);

      // Initialize the zero'th element to unknowns. The other elements will
      // be filled in based on values from the data file
      crystalStructures->SetValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
      if (m_PhaseTypes.get() == NULL || m_PhaseTypes->GetSize() == 0)
      {
        return -1;
      }
      m_PhaseTypes->SetValue(0, DREAM3D::PhaseType::UnknownPhaseType);
      for(size_t i=0;i<phases.size();i++)
      {
        int phaseID = phases[i]->getPhaseIndex();
        crystalStructures->SetValue(phaseID, phases[i]->determineCrystalStructure() );
      }
      getDataContainer()->addEnsembleData(DREAM3D::EnsembleData::CrystalStructures, crystalStructures);
      getDataContainer()->addEnsembleData(DREAM3D::EnsembleData::PhaseTypes, m_PhaseTypes);
      getDataContainer()->setNumEnsembleTuples(crystalStructures->GetNumberOfTuples());
      return 0;
    }
};

#endif /* LOADSLICES_H_ */
