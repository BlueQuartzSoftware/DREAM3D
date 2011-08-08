/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _RECONSTRUCTION_H_
#define _RECONSTRUCTION_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/MXATypes.h>

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/HKL/CtfConstants.h"

#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/AbstractPipeline.h"
#include "ReconstructionFunc.h"

/**
* @class Reconstruction Reconstruction AIM/Reconstruction.h/Reconstruction.h
* @brief This class serves as the main entry point to execute the reconstruction codes
*
* @author Michael A. Jackson for BlueQuartz Software
* @author Dr. Michael Groeber, USAFRL
* @date Nov 3, 2009
* @version 1.0
*/
class Reconstruction : public AbstractPipeline
{
  public:
    MXA_SHARED_POINTERS(Reconstruction);
    MXA_TYPE_MACRO(Reconstruction);
    MXA_STATIC_NEW_MACRO(Reconstruction);

    virtual ~Reconstruction();


    MXA_INSTANCE_STRING_PROPERTY(H5AngFile)
    MXA_INSTANCE_PROPERTY(int, ZStartIndex)
    MXA_INSTANCE_PROPERTY(int, ZEndIndex)
    MXA_INSTANCE_PROPERTY(std::vector<AIM::Reconstruction::PhaseType>, PhaseTypes)
    MXA_INSTANCE_STRING_PROPERTY(OutputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(OutputFilePrefix)
    MXA_INSTANCE_PROPERTY(bool, MergeTwins)
    MXA_INSTANCE_PROPERTY(bool, MergeColonies)
    MXA_INSTANCE_PROPERTY(bool, FillinSample)
    MXA_INSTANCE_PROPERTY(int32_t, MinAllowedGrainSize)
    MXA_INSTANCE_PROPERTY(double, MinSeedConfidence)
    MXA_INSTANCE_PROPERTY(double, DownSampleFactor)
    MXA_INSTANCE_PROPERTY(double, MinSeedImageQuality)
    MXA_INSTANCE_PROPERTY(double, MisorientationTolerance)
    MXA_INSTANCE_PROPERTY(AIM::Reconstruction::AlignmentMethod, AlignmentMethod)
    MXA_INSTANCE_PROPERTY(Ebsd::Orientation, Orientation)

    MXA_INSTANCE_PROPERTY(bool, WriteBinaryVTKFiles)
    MXA_INSTANCE_PROPERTY(bool, WriteVtkFile)
    MXA_INSTANCE_PROPERTY(bool, WritePhaseId)
    MXA_INSTANCE_PROPERTY(bool, WriteImageQuality)
    MXA_INSTANCE_PROPERTY(bool, WriteIPFColor)

    MXA_INSTANCE_PROPERTY(bool, WriteDownSampledFile)
    MXA_INSTANCE_PROPERTY(bool, WriteHDF5GrainFile)
    MXA_INSTANCE_PROPERTY(bool, WriteDxFile)
    MXA_INSTANCE_PROPERTY(bool, WritePhFile)

    /**
    * @brief Main method to run the operation
    */
    virtual void execute();

    virtual void printSettings(std::ostream &stream);

  protected:
    Reconstruction();


  private:
    ReconstructionFunc::Pointer m;

    Reconstruction(const Reconstruction&);    // Copy Constructor Not Implemented
    void operator=(const Reconstruction&);  // Operator '=' Not Implemented


    template<typename EbsdReader, typename EbsdPhase>
    int loadInfo(EbsdReader* reader,
                 std::vector<float> &precipFractions,
                 std::vector<Ebsd::CrystalStructure> &crystalStructures)
    {
      reader->setFilename(m_H5AngFile);
      reader->setSliceStart(m_ZStartIndex);
      reader->setSliceEnd(m_ZEndIndex);
      reader->setOrientation(m_Orientation);

      //FIXME: Mike Groeber: Take a look at this setup for the PhaseTypes. I am
      // putting in UnknownTypes just to have something. You can change to suit
      // your needs.
      std::vector<typename EbsdPhase::Pointer> phases = reader->getPhases();
      crystalStructures.resize(phases.size()+1);

      precipFractions.resize(phases.size() + 1);
      crystalStructures[0] = Ebsd::UnknownCrystalStructure;
      m_PhaseTypes[0] = AIM::Reconstruction::UnknownPhaseType;
      precipFractions[0] = -1.0f;
      for(size_t i=0;i<phases.size();i++)
      {
        int phaseID = phases[i]->getPhaseIndex();
        crystalStructures[phaseID] = phases[i]->determineCrystalStructure();
        precipFractions[phaseID] = -1.0f;
      }
      return 0;
    }
};

#endif /* RECONSTRUCTION_H_ */
