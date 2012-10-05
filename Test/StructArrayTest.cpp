/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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



#include <stdlib.h>

#include <iostream>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/SurfaceMeshDataContainer.h"
#include "DREAM3DLib/SurfaceMeshFilters/MMCSurfaceMeshStructs.h"
#include "DREAM3DLib/SurfaceMeshFilters/StructArray.hpp"


#include "UnitTestSupport.hpp"

#include "TestFileLocations.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestStructArray()
{

    StructArray<Node>::Pointer nodes = StructArray<Node>::CreateArray(10, DREAM3D::CellData::SurfaceMesh::Nodes);
    size_t size = nodes->GetNumberOfTuples();
    int typeSize = nodes->GetTypeSize();
    DREAM3D_REQUIRE_EQUAL(typeSize, sizeof(Node));
  for (size_t i = 0; i < size; ++i)
  {
    Node* node = nodes->GetPointer(i);
    node->newID = i;
    node->coord[0] = i+23.0f;
    node->coord[1] = i+11.0f;
    node->coord[2] = i+20.0f;
    node->nodeKind = i+i;
  }
  // Resize DOWN
  nodes->Resize(5);
  size = nodes->GetNumberOfTuples();
  for (size_t i = 0; i < size; ++i)
  {
    Node* node = nodes->GetPointer(i);
    DREAM3D_REQUIRE_EQUAL(node->newID , i);
    DREAM3D_REQUIRE_EQUAL(node->coord[0] , i+23.0f);
    DREAM3D_REQUIRE_EQUAL(node->coord[1] , i+11.0f);
    DREAM3D_REQUIRE_EQUAL(node->coord[2] , i+20.0f);
    DREAM3D_REQUIRE_EQUAL(node->nodeKind , i+i);
  }

// Resize UP
  nodes->Resize(8);
  size = nodes->GetNumberOfTuples();
  for (size_t i = 0; i < size; ++i)
  {
    Node* node = nodes->GetPointer(i);
    node->newID = i;
    node->coord[0] = i+23.0f;
    node->coord[1] = i+11.0f;
    node->coord[2] = i+20.0f;
    node->nodeKind = i+i;
  }

  for (size_t i = 0; i < size; ++i)
  {
    Node* node = nodes->GetPointer(i);
    DREAM3D_REQUIRE_EQUAL(node->newID , i);
    DREAM3D_REQUIRE_EQUAL(node->coord[0] , i+23.0f);
    DREAM3D_REQUIRE_EQUAL(node->coord[1] , i+11.0f);
    DREAM3D_REQUIRE_EQUAL(node->coord[2] , i+20.0f);
    DREAM3D_REQUIRE_EQUAL(node->nodeKind , i+i);
  }

  StructArray<Node>& vertices = *nodes; // Dereference the Shared_Pointer using the * operator which allows the [] operators
  for (size_t i = 0; i < size; ++i)
  {
    Node node = vertices[i]; // Uses the [] operator overload
    DREAM3D_REQUIRE_EQUAL(node.newID , i);
    DREAM3D_REQUIRE_EQUAL(node.coord[0] , i+23.0f);
    DREAM3D_REQUIRE_EQUAL(node.coord[1] , i+11.0f);
    DREAM3D_REQUIRE_EQUAL(node.coord[2] , i+20.0f);
    DREAM3D_REQUIRE_EQUAL(node.nodeKind , i+i);
  }

#if 0
  SurfaceMeshDataContainer::NodeArray_t nodes = SurfaceMeshDataContainer::NodeArray_t(new Node[10]);

  for (int i = 0; i < 10; ++i)
  {
    Node& node = nodes[i];
    node.newID = i;
    node.coord[0] = i*23.0f;
    node.coord[1] = i*11.0f;
    node.coord[2] = i*20.0f;
    node.nodeKind = 2;
  }
#endif
  std::cout << "Test COmplete" << std::endl;
}


// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;
#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif

  DREAM3D_REGISTER_TEST( TestStructArray() )


#if REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif


  PRINT_TEST_SUMMARY();
  return err;
}

