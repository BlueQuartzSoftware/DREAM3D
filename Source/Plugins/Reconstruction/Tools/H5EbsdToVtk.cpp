/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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



#include <iostream>
#include <string>
#include <vector>

#include "MXA/MXA.h"
#include "MXA/Common/MXASetGetMacros.h"

#include "EbsdLib/TSL/AngPhase.h"
#include "EbsdLib/TSL/H5AngVolumeReader.h"


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/AIMArray.hpp"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/VTKUtils/VTKWriterMacros.h"
#include "DREAM3DLib/VTKUtils/VTKFileWriters.hpp"

#include "EbsdLib/H5EbsdVolumeInfo.h"
#include "EbsdLib/H5EbsdVolumeReader.h"
#include "EbsdLib/TSL/AngDirectoryPatterns.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/TSL/AngPhase.h"
#include "EbsdLib/TSL/H5AngVolumeReader.h"
#include "EbsdLib/HKL/H5CtfVolumeReader.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/EbsdColoring.hpp"
#include "DREAM3DLib/VTKUtils/VTKFileWriters.hpp"
#include "DREAM3DLib/HDF5/H5VoxelWriter.h"
#include "DREAM3DLib/HDF5/H5GrainWriter.hpp"

#include "DREAM3DLib/Common/DataContainer.h"
#include "DREAM3DLib/ReconstructionFilters/LoadSlices.h"


#define _CHECK_FOR_ERROR(FuncClass, Message, err)\
    if(err < 0) {\
      std::cout << Message << std::endl;\
      return EXIT_FAILURE;   }

template<typename EbsdReader, typename EbsdPhase>
int loadInfo(EbsdReader* reader,
             std::vector<float> &precipFractions,
             std::vector<Ebsd::CrystalStructure> &crystalStructures,
             std::vector<DREAM3D::Reconstruction::PhaseType> &m_PhaseTypes)
{


  std::vector<typename EbsdPhase::Pointer> phases = reader->getPhases();
  if (phases.size() == 0)
  {
    return -1;
  }
  precipFractions.resize(phases.size() + 1);
  crystalStructures.resize(phases.size() + 1);
  m_PhaseTypes.resize(phases.size() + 1);

  // Initialize the zero'th element to unknowns. The other elements will
  // be filled in based on values from the data file
  crystalStructures[0] = Ebsd::UnknownCrystalStructure;
  m_PhaseTypes[0] = DREAM3D::Reconstruction::UnknownPhaseType;
  precipFractions[0] = -1.0f;
  for(size_t i=0;i<phases.size();i++)
  {
    int phaseID = phases[i]->getPhaseIndex();
    crystalStructures[phaseID] = phases[i]->determineCrystalStructure();
    precipFractions[phaseID] = -1.0f;
  }
  return 0;
}

/**
 * @brief This program takes an .h5ebsd file and writes out a binary VTK file containing
 * the IPF Color for each voxel as a rgb triplet. The VTK File is a STRUCTURED_POINTS
 * file type.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv)
{
  std::cout << "Starting Conversion of .h5ebsd to VTK with Grain ID and IPF Colors" << std::endl;
  if (argc < 3)
  {
    std::cout << "This program takes 2 arguments: Input .h5ebsd file and output vtk file." << std::endl;
    return EXIT_FAILURE;
  }


  int err = -1;

  std::string m_H5AngFile = argv[1];
  std::string vtkFile = argv[2];

  std::cout << "h5ebsd File: " << m_H5AngFile << std::endl;
  std::cout << "VTK File: " << vtkFile << std::endl;

//  int m_ZStartIndex = 0;
//  int m_ZEndIndex = 0;

  DataContainer::Pointer m = DataContainer::New();
  std::vector<DREAM3D::Reconstruction::PhaseType> m_PhaseTypes(1);
  // updateProgressAndMessage(("Gathering Size and Resolution Information from OIM Data"), 1);
   std::string manufacturer;
   // Get the Size and Resolution of the Volume
   {
     H5EbsdVolumeInfo::Pointer volumeInfoReader = H5EbsdVolumeInfo::New();
     volumeInfoReader->setFileName(m_H5AngFile);
     err = volumeInfoReader->readVolumeInfo();
     _CHECK_FOR_ERROR(ReconstructionFunc, "Error reading Volume Information from File.", err);

     volumeInfoReader->getDimsAndResolution(m->xpoints, m->ypoints, m->zpoints, m->resx, m->resy, m->resz);
     manufacturer = volumeInfoReader->getManufacturer();
     volumeInfoReader = H5EbsdVolumeInfo::NullPointer();
   }
   H5EbsdVolumeReader::Pointer ebsdReader;
   std::vector<float> precipFractions;
   std::vector<Ebsd::CrystalStructure> crystalStructures;
   if (manufacturer.compare(Ebsd::Ang::Manufacturer) == 0)
   {
     ebsdReader = H5AngVolumeReader::New();
     if (NULL == ebsdReader)
     {
       _CHECK_FOR_ERROR(ReconstructionFunc, "Error Creating the proper Ebsd Volume Reader.", -1);
     }
     H5AngVolumeReader* angReader = dynamic_cast<H5AngVolumeReader*>(ebsdReader.get());
     angReader->setFileName(m_H5AngFile);
     angReader->setSliceStart(0);
     angReader->setSliceEnd(m->zpoints-1);
     err = loadInfo<H5AngVolumeReader, AngPhase>(angReader, precipFractions, crystalStructures, m_PhaseTypes );
   }
   else if (manufacturer.compare(Ebsd::Ctf::Manufacturer) == 0)
   {
     ebsdReader = H5CtfVolumeReader::New();
     if (NULL == ebsdReader)
     {
       _CHECK_FOR_ERROR(ReconstructionFunc, "Error Creating the proper Ebsd Volume Reader.", -1);
     }
     H5CtfVolumeReader* ctfReader = dynamic_cast<H5CtfVolumeReader*>(ebsdReader.get());
     ctfReader->setFileName(m_H5AngFile);
     ctfReader->setSliceStart(0);
     ctfReader->setSliceEnd(m->zpoints-1);
     err = loadInfo<H5CtfVolumeReader, CtfPhase>(ctfReader, precipFractions, crystalStructures, m_PhaseTypes );
   }
   else
   {
     _CHECK_FOR_ERROR(ReconstructionFunc, "ReconstructionFunc Error: No Manufacturer Set for EBSD data", -1);
   }

 // DREAM3D::Reconstruction::AlignmentMethod m_AlignmentMethod = DREAM3D::Reconstruction::UnknownAlignmentMethod;

   std::cout << "Loading EBSD Data...." << std::endl;
  LoadSlices::Pointer load_slices = LoadSlices::New();
  load_slices->setDataContainer(m.get());
  load_slices->execute();
  err = load_slices->getErrorCondition();
  if (err < 0)
  {
    std::cout << load_slices->getErrorMessage() << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Writing VTK file" << std::endl;

  FILE* f = fopen(vtkFile.c_str(), "wb");

  WRITE_STRUCTURED_POINTS_HEADER("BINARY", m)

  VoxelIPFColorScalarWriter<DataContainer> ipfWriter(m.get());
  ipfWriter.m_WriteBinaryFiles = true;
  ipfWriter.writeScalars(f);

  fclose(f);

  std::cout << "Conversion Complete" << std::endl;
  return EXIT_SUCCESS;
}
