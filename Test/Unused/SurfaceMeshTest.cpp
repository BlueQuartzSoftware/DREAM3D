/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#include <stdio.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/ScopedFileMonitor.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/MeshStructs.h"
#include "SIMPLib/Common/Observer.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/util/TriangleOps.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/BinaryNodesTrianglesReader.h"

#include "SurfaceMeshing/SurfaceMeshingFilters/LaplacianSmoothing.h"


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
  SurfaceDataContainer::Pointer sm = SurfaceDataContainer::New();
  sm->setName(DREAM3D::Defaults::DataContainerName);
  DataContainerArray::Pointer dca = DataContainerArray::New();
  dca->pushBack(sm);

  QString nodesFileName = argv[1];
  QString trianglesFileName = argv[2];
  Observer observer;

  // This will read the mesh from the temp file and store it in the SurfaceMesh Data container
  BinaryNodesTrianglesReader::Pointer binaryReader = BinaryNodesTrianglesReader::New();
  binaryReader->setBinaryNodesFile(nodesFileName);
  binaryReader->setBinaryTrianglesFile(trianglesFileName);
  binaryReader->setMessagePrefix(binaryReader->getNameOfClass());
  binaryReader->setDataContainerArray(dca);
  binaryReader->execute();
  if(binaryReader->getErrorCondition() < 0)
  {
    std::cout << "Error Code: " << binaryReader->getErrorCondition()  << std::endl;
  }
  // END_CLOCK("Reading Nodes & Triangles");


  START_CLOCK;
  sm->getFaces()->findFacesContainingVert();
  // END_CLOCK("Building Cell Links");

  START_CLOCK;
  sm->getFaces()->findFaceNeighbors();
  // END_CLOCK("Building Triangle Neighbor Lists");

  START_CLOCK;
  LaplacianSmoothing::Pointer filter = LaplacianSmoothing::New();
  filter->setLambda(0.25f);
  filter->setTripleLineLambda(0.125f);
  filter->setQuadPointLambda(0.65f);

  filter->setDataContainerArray(dca);
  filter->setMessagePrefix(filter->getNameOfClass());
  filter->setIterationSteps(50);
  filter->execute();
  // END_CLOCK("Laplacian Filter");

  std::cout << "Finished" << std::endl;
  return err;
}
