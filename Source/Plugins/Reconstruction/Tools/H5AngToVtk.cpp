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

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"

#include "TSLLib/AngPhase.h"

#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/AIMArray.hpp"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/VTKUtils/VTKWriterMacros.h"
#include "DREAM3D/Common/VTKUtils/VTKFileWriters.hpp"


#include "Reconstruction/ReconstructionFunc.h"

#include "EbsdSupport/AbstractAngDataLoader.h"
#include "EbsdSupport/AngDataLoader.h"
#include "EbsdSupport/H5AngDataLoader.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::cout << "Starting Conversion of H5Ang to VTK with Grain ID and IPF Colors" << std::endl;
  if (argc < 3)
  {
    std::cout << "This program takes 2 arguments: Input .h5ang file and output vtk file." << std::endl;
    return EXIT_FAILURE;
  }


  int err = -1;

  std::string m_H5AngFile = argv[1];
  std::string vtkFile = argv[2];

  std::cout << "H5Ang File: " << m_H5AngFile << std::endl;
  std::cout << "VTK File: " << vtkFile << std::endl;

  int zStart = 0;
  int zEnd = 0;

  ReconstructionFunc::Pointer m = ReconstructionFunc::New();

  AbstractEbsdDataLoader::Pointer oimDataLoader = H5AngVolumeReader::New();
  H5AngVolumeReader* h5io = dynamic_cast<H5AngVolumeReader*>(oimDataLoader.get());
  h5io->setFilename(m_H5AngFile);

  // Read the Header
  err = h5io->readZHeader(zStart, zEnd, m->resz);
  h5io->setZStartIndex(zStart);
  h5io->setZEndIndex(zEnd);

  // Get the dimensions
  err = h5io->getSizeAndResolution(m->xpoints, m->ypoints, m->zpoints, m->resx, m->resy, m->resz);

  h5io->setZStartIndex(zStart);
  h5io->setZEndIndex(zEnd);
  h5io->setOrientation(Ang::NoOrientation);

  AIM::Reconstruction::AlignmentMethod m_AlignmentMethod = AIM::Reconstruction::UnknownAlignmentMethod;
  std::vector<AIM::Reconstruction::PhaseType> m_PhaseTypes;
  std::vector<float> precipFractions;
  std::vector<Ebsd::CrystalStructure> crystalStructures;
  std::vector<AngPhase::Pointer> phases = h5io->getPhases();
  crystalStructures.resize(phases.size()+1);
  m_PhaseTypes.resize(phases.size() + 1);
  precipFractions.resize(phases.size() + 1);
  crystalStructures[0] = Ebsd::UnknownCrystalStructure;
  m_PhaseTypes[0] = AIM::Reconstruction::UnknownPhaseType;
  precipFractions[0] = -1.0f;
  for(size_t i=0;i<phases.size();i++)
  {
    int phaseID = phases[i]->getPhase();
    Ebsd::Ang::PhaseSymmetry symmetry = phases[i]->getSymmetry();
    Ebsd::CrystalStructure crystal_structure = Ebsd::UnknownCrystalStructure;
    if(symmetry == Ebsd::Ang::CubicSymmetry) crystal_structure = Ebsd::Cubic;
    else if(symmetry == Ebsd::Ang::HexagonalSymmetry) crystal_structure = Ebsd::Hexagonal;
    crystalStructures[phaseID] = crystal_structure;

    precipFractions[phaseID] = -1.0f;
  }
  m->initialize(m->xpoints, m->ypoints, m->zpoints,
                m->resx, m->resy, m->resz, 0, 0, 0, 0, 0, 0, 0,
                crystalStructures, m_PhaseTypes, precipFractions,
                m_AlignmentMethod);
  std::cout << "Loading EBSD Data...." << std::endl;
  oimDataLoader->loadData(m.get());
  m->initializeQuats();

  std::cout << "Writing VTK file" << std::endl;

  FILE* f = fopen(vtkFile.c_str(), "wb");

  WRITE_STRUCTURED_POINTS_HEADER("BINARY", m)

  VoxelIPFColorScalarWriter<ReconstructionFunc> ipfWriter(m.get());
  ipfWriter.m_WriteBinaryFiles = true;
  ipfWriter.writeScalars(f);

 // WRITE_VTK_GRAIN_IDS_ASCII(ptr, AIM::VTK::GrainIdScalarName)

  fclose(f);

  std::cout << "Conversion Complete" << std::endl;
  return EXIT_SUCCESS;
}
