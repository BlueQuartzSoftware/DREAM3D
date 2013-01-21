#ifndef _SurfaceMeshDataStructures_H_
#define _SurfaceMeshDataStructures_H_

#include <vector>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace SurfaceMesh
{
  namespace NodesFile
  {
    typedef struct
    {
        int nodeId;
        int nodeKind;
        float x;
        float y;
        float z;
    } NodesFileRecord_t;
    const int ByteCount = sizeof(NodesFileRecord_t);
  }

  namespace TrianglesFile
  {
    typedef struct
    {
        int triId;
        int nodeId_0;
        int nodeId_1;
        int nodeId_2;
        int label_0;
        int label_1;
    } TrianglesFileRecord_t;
    const int ByteCount = sizeof(TrianglesFileRecord_t);
  }


  typedef struct
  {
      int verts[3];
      int labels[2];
  } Triangle_t;

  typedef struct
  {
      float x;
      float y;
      float z;
  } Node_t;

  typedef struct
  {
      int vert[2];
  } Edge_t;

}


typedef std::vector<SurfaceMesh::Node_t> NodeList_t;
typedef std::vector<SurfaceMesh::Triangle_t> TriangleList_t;


#endif /* _SurfaceMeshDataStructures_H_ */
