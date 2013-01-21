

#include <stdio.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>



#include "support/ScopedFilePointer.hpp"
#include "support/SurfaceMeshDataStructures.h"
#include "support/CellLinks.hpp"
#include "support/TriangleNeighbors.hpp"
#include "support/TriangleOps.hpp"
#include "support/NodesTriangleReader.h"





#define DREAM3D_BENCHMARKS 1

#if DREAM3D_BENCHMARKS
#include "MXA/Common/LogTime.h"

#define DEFINE_CLOCK unsigned long long int millis;

#define START_CLOCK millis = MXA::getMilliSeconds();

#define END_CLOCK(message)\
  std::cout << message << " Finish Time(ms): " << (MXA::getMilliSeconds() - millis) << std::endl;


#else
#define DEFINE_CLOCK
#define START_CLOCK
#define END_CLOCK
#endif




/**
   * @brief Sorts the 3 values from low to high
   * @param a
   * @param b
   * @param c
   * @param sorted The array to store the sorted values.
   */
template<typename T>
static void TripletSort(T a, T b, T c, T* sorted)
{
  if ( a > b && a > c)
  {
    sorted[2] = a;
    if (b > c) { sorted[1] = b; sorted[0] = c; }
    else { sorted[1] = c; sorted[0] = b; }
  }
  else if ( b > a && b > c)
  {
    sorted[2] = b;
    if (a > c) { sorted[1] = a; sorted[0] = c; }
    else { sorted[1] = c; sorted[0] = a; }
  }
  else if ( a > b )
  {
    sorted[1] = a; sorted[0] = b; sorted[2] = c;
  }
  else if (a >= c && b >=c)
  {
    sorted[0] = c; sorted[1] = a; sorted[2] = b;
  }
  else
  { sorted[0] = a; sorted[1] = b; sorted[2] = c;}
}


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

  NodesTrianglesReader reader;

  std::string nodesFileName = argv[1];
  std::string trianglesFileName = argv[2];
  int err = reader.read(nodesFileName, trianglesFileName);
  END_CLOCK("Reading Nodes & Triangles");


  NodeList_t& nodes = reader.getNodes();
  TriangleList_t& triangles = reader.getTriangles();


  START_CLOCK;
  CellLinks cellLinks;
  cellLinks.Allocate(nodes.size());
  cellLinks.generateCellLinks(nodes, triangles);
  END_CLOCK("Building Cell Links");



  START_CLOCK;
  TriangleNeighbors triNeighbors;
  triNeighbors.Allocate(triangles.size());
  triNeighbors.generateNeighborLists(nodes, triangles, cellLinks);
  END_CLOCK("Building Triangle Neighbor Lists");

  std::cout << "Finished" << std::endl;
  return err;
}
