#ifndef _TriangleOps_H_
#define _TriangleOps_H_


#include "SurfaceMeshDataStructures.h"


/**
 * @brief The TriangleOps class
 */
class TriangleOps
{
  public:
    static int getLabelIndex(SurfaceMesh::Triangle_t& t, int label)
    {
      if (label == t.labels[0]) return 0;
      if (label == t.labels[1]) return 1;
      return 2; // Error condition. Valid values are 0 or 1 since there are only 2 elements to the array.
    }

    static void getWindingIndices4(SurfaceMesh::Triangle_t& triangle, int ids[4], int32_t label)
    {
      int idx = TriangleOps::getLabelIndex(triangle, label);

      if (idx == 1)
      {
        ids[0] = triangle.verts[2];
        ids[1] = triangle.verts[1];
        ids[2] = triangle.verts[0];
        ids[3] = triangle.verts[2];
      }
      else
      {
        ids[0] = triangle.verts[0];
        ids[1] = triangle.verts[1];
        ids[2] = triangle.verts[2];
        ids[3] = triangle.verts[0];
      }
    }

    static bool verifyWinding(SurfaceMesh::Triangle_t& source, SurfaceMesh::Triangle_t& tri, int32_t label)
    {
      int ids[4];
      int nids[4];
      bool flipped = false;
      TriangleOps::getWindingIndices4(source, ids, label);
      TriangleOps::getWindingIndices4(tri, nids, label);
      //  int idx = 0;
      // There are 2 indices that are shared between the two triangles
      // Find them
      int i0, i1;
      //  bool flip = false;
      bool done = false;
      for (int i = 0; i < 3; ++i)
      {
        i0 = ids[i];
        i1 = ids[i + 1];
        for (int j = 0; j < 3; ++j)
        {
          if (i0 == nids[j + 1] && i1 == nids[j])
          {
            //    std::cout << ">>>>>> Winding OK "<< tIndex << " <-> "<< tri->tIndex << std::endl;
            done = true;
            break;
          }
          else if (i0 == nids[j] && i1 == nids[j + 1])
          {
            //std::cout << "!!!!!! Winding Bad " << std::endl;
            done = true;
            TriangleOps::flipWinding(tri);
            flipped = true;
            break;
          }
        }
        if (done) break;
      }
      return flipped;
    }

    static void flipWinding(SurfaceMesh::Triangle_t& t)
    {
      int tmp = t.verts[0];
      t.verts[0] = t.verts[2];
      t.verts[2] = tmp;
    }


  private:
    TriangleOps(const TriangleOps&); // Copy Constructor Not Implemented
    void operator=(const TriangleOps&); // Operator '=' Not Implemented
};



#endif /* _TriangleOps_H_ */
