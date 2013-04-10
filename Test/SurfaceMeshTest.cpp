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

#include <stdio.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/ScopedFileMonitor.hpp"
#include "DREAM3DLib/Common/SurfaceMeshDataContainer.h"
#include "DREAM3DLib/Common/SurfaceMeshStructs.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/SurfaceMeshingFilters/MeshVertLinks.hpp"
#include "DREAM3DLib/SurfaceMeshingFilters/MeshFaceNeighbors.hpp"
#include "DREAM3DLib/SurfaceMeshingFilters/util/TriangleOps.h"
#include "DREAM3DLib/SurfaceMeshingFilters/BinaryNodesTrianglesReader.h"

#include "DREAM3DLib/SurfaceMeshingFilters/LaplacianSmoothing.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  std::cout << "Starting SurfaceMeshTest" << std::endl;
  if (argc < 3)
  {
    std::cout << "2 Arguments needed:\n  (1) Binary Nodes File\n  (2) Binary Triangle File" << std::endl;
    return EXIT_FAILURE;
  }
  DEFINE_CLOCK;

  START_CLOCK;

  int err = 0;
  SurfaceMeshDataContainer::Pointer sm = SurfaceMeshDataContainer::New();


  std::string nodesFileName = argv[1];
  std::string trianglesFileName = argv[2];
  Observer observer;

  // This will read the mesh from the temp file and store it in the SurfaceMesh Data container
  BinaryNodesTrianglesReader::Pointer binaryReader = BinaryNodesTrianglesReader::New();
  binaryReader->setBinaryNodesFile(nodesFileName);
  binaryReader->setBinaryTrianglesFile(trianglesFileName);
  binaryReader->setMessagePrefix(binaryReader->getNameOfClass());
  binaryReader->addObserver(&observer);
  binaryReader->setSurfaceMeshDataContainer(sm.get());
  binaryReader->execute();
  if(binaryReader->getErrorCondition() < 0)
  {
    std::cout << "Error Code: "<< binaryReader->getErrorCondition()  << std::endl;
  }
  // END_CLOCK("Reading Nodes & Triangles");


  START_CLOCK;
  sm->buildMeshVertLinks();
  // END_CLOCK("Building Cell Links");

  START_CLOCK;
  sm->buildMeshFaceNeighborLists();
  // END_CLOCK("Building Triangle Neighbor Lists");

  START_CLOCK;
  LaplacianSmoothing::Pointer filter = LaplacianSmoothing::New();
  filter->setLambda(0.25f);
  filter->setTripleLineLambda(0.125f);
  filter->setQuadPointLambda(0.65f);

  filter->setSurfaceMeshDataContainer(sm.get());
  filter->setMessagePrefix(filter->getNameOfClass());
  filter->addObserver(&observer);
  filter->setIterationSteps(50);
  filter->execute();
  // END_CLOCK("Laplacian Filter");

  std::cout << "Finished" << std::endl;
  return err;
}
