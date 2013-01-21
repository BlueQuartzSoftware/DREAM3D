#ifndef _TriangleNeighbors_hpp_H_
#define _TriangleNeighbors_hpp_H_

#include <string>
#include <vector>

#include <boost/shared_array.hpp>

#include "SurfaceMeshDataStructures.h"

/**
 * @brief The TriangleNeighbors class contains arrays of Triangles for each Node in the mesh. This allows quick query to the node
 * to determine what Cells the node is a part of.
 */
class TriangleNeighbors
{
  public:

    class Link {
      public:
        unsigned short ncells;
        int* cells;
    };


    //----------------------------------------------------------------------------
    void Allocate(size_t sz, size_t ext=1000)
    {
      static TriangleNeighbors::Link linkInit = {0,NULL};

      this->Size = sz;
      if ( this->Array != NULL )
      {
        delete [] this->Array;
      }
      this->Array = new TriangleNeighbors::Link[sz];
      this->Extend = ext;
      this->MaxId = -1;

      for (size_t i=0; i < sz; i++)
      {
        this->Array[i] = linkInit;
      }
    }

    TriangleNeighbors():Array(NULL),Size(0),MaxId(-1),Extend(1000) {}

    ~TriangleNeighbors()
    {
      if ( this->Array == NULL )
      {
        return;
      }

      for (size_t i=0; i<=this->MaxId; i++)
      {
        if ( this->Array[i].cells != NULL )
        {
          delete [] this->Array[i].cells;
        }
      }

      delete [] this->Array;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void IncrementLinkCount(size_t ptId)
    {
      this->Array[ptId].ncells++;
    }



    //----------------------------------------------------------------------------
    inline void InsertCellReference(size_t ptId,
                                    unsigned short pos,
                                    size_t cellId)
    {
      this->Array[ptId].cells[pos] = cellId;
    }
    // Description:
    // Get a link structure given a point id.
    Link& GetLink(size_t ptId) {
      return this->Array[ptId];
    }

    // Description:
    // Get the number of cells using the point specified by ptId.
    unsigned short GetNcells(size_t ptId) {
      return this->Array[ptId].ncells;
    }
    // Description:
    // Return a list of cell ids using the point.
    int* GetCells(size_t ptId) {
      return this->Array[ptId].cells;
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void generateNeighborLists(NodeList_t &nodes, TriangleList_t &triangles, CellLinks &cellLinks)
    {

      size_t nTriangles = triangles.size();

      // Allocate an array of bools that we use each iteration of triangle so that we don't put duplicates into the array
      boost::shared_array<bool> visitedPtr(new bool[nTriangles]);
      bool* visited = visitedPtr.get();
      ::memset(visitedPtr.get(), 0, nTriangles);



      // now allocate storage for the links
      //this->AllocateLinks(nTriangles);
      this->MaxId = nTriangles - 1;

      // Reuse this vector for each loop. Avoids re-allocating the memory each time through the loop
      std::vector<int> loop_neighbors(32, 0);

      // Build up the Triangle Adjacency list now that we have the cell links
      for(size_t t = 0; t < nTriangles; ++t)
      {
        //   std::cout << "Analyzing Triangle " << t << std::endl;
        SurfaceMesh::Triangle_t& seedTriangle = triangles[t];
        for(size_t v = 0; v < 3; ++v)
        {
          //   std::cout << " vert " << v << std::endl;
          int nTris = cellLinks.GetNcells(seedTriangle.verts[v]);
          int* vertIdxs = cellLinks.GetCells(seedTriangle.verts[v]);

          for(int vt = 0; vt < nTris; ++vt)
          {
            if (vertIdxs[vt] == t) { continue; } // This is the same triangle as our "source" triangle
            if (visited[vertIdxs[vt]] == true) { continue; } // We already added this triangle so loop again
            //      std::cout << "   Comparing Triangle " << vertIdxs[vt] << std::endl;
            SurfaceMesh::Triangle_t& vertTri = triangles[vertIdxs[vt]];
            int vCount = 0;
            // Loop over all the vertex indices of this triangle and try to match 2 of them to the current loop triangle
            // If there are 2 matches then that triangle is a neighbor of this triangle. if there are more than 2 matches
            // then there is a real problem with the mesh and the program is going to assert.

            int seedTriVert0 = seedTriangle.verts[0];
            int seedTriVert1 = seedTriangle.verts[1];
            int seedTriVert2 = seedTriangle.verts[2];
            int trgtTriVert0 = vertTri.verts[0];
            int trgtTriVert1 = vertTri.verts[1];
            int trgtTriVert2 = vertTri.verts[2];

//            for(int e = 0; e < 3; ++e)
//            {
//              if (seedTriangle.verts[e] == vertTri.verts[0] || seedTriangle.verts[e] == vertTri.verts[1] || seedTriangle.verts[e] == vertTri.verts[2]  )
//              {
//                vCount++;
//              }
//            }
            if (seedTriVert0 == trgtTriVert0 || seedTriVert0 == trgtTriVert1 || seedTriVert0 == trgtTriVert2  )
            {
              vCount++;
            }
            if (seedTriVert1 == trgtTriVert0 || seedTriVert1 == trgtTriVert1 || seedTriVert1 == trgtTriVert2  )
            {
              vCount++;
            }
            if (seedTriVert2 == trgtTriVert0 || seedTriVert2 == trgtTriVert1 || seedTriVert2 == trgtTriVert2  )
            {
              vCount++;
            }

            assert(vCount < 3); // No way 2 triangles can share all 3 vertices. Something is VERY wrong at this point

            // So if our vertex match count is 2 and we have not visited the triangle in question then add this triangle index
            // into the list of Triangle Indices as neighbors for the source triangle.
            if (vCount == 2)
            {
              //std::cout << "       Neighbor: " << vertIdxs[vt] << std::endl;
              // Use the current count of neighbors as the index
              // into the loop_neighbors vector and place the value of the vertex triangle at that index
              loop_neighbors[this->Array[t].ncells] = vertIdxs[vt];
              this->Array[t].ncells++;// Increment the count for the next time through
              if (this->Array[t].ncells >= loop_neighbors.size())
              {
                loop_neighbors.resize(loop_neighbors.size() + 10);
              }
              visited[vertIdxs[vt]] = true; // Set this triangle as visited so we do NOT add it again
            }
          }
        }
        assert(this->Array[t].ncells > 2);
        // Reset all the visited triangle indexs back to false (zero)
        for(size_t k = 0;k < this->Array[t].ncells; ++k)
        {
          visited[loop_neighbors[k]] = false;
        }
        // Allocate the array storage for the current triangle to hold its Triangle list
        this->Array[t].cells = new int[this->Array[t].ncells];
        // Only copy the first "N" values from the loop_neighbors vector into the storage array
        ::memcpy(this->Array[t].cells, &(loop_neighbors[0]), sizeof(int) * this->Array[t].ncells);
      }

      size_t memory_used = 0;
      for(size_t t = 0; t < nTriangles; ++t)
      {
        memory_used= memory_used + (10 + sizeof(int)*this->Array[t].ncells);
      }
      std::cout << "Triangle Neighbors Memory Used: " << memory_used << std::endl;

    }

  private:
    Link* Array;   // pointer to data
    size_t Size;
    size_t MaxId;
    size_t Extend;

    //----------------------------------------------------------------------------
    // Allocate memory for the list of lists of cell ids.
    void AllocateLinks(size_t n)
    {
      for (size_t i=0; i < n; i++)
      {
        this->Array[i].cells = new int[this->Array[i].ncells];
      }
    }


};

#if 0

NodeList_t::size_type numPts = nodes.size();
TriangleList_t::size_type numCells = triangles.size();

TriangleList_t::size_type cellId;
unsigned short* linkLoc;

// fill out lists with number of references to cells
typedef boost::shared_array<unsigned short> SharedShortArray_t;
SharedShortArray_t linkLocPtr(new unsigned short[numPts]);
linkLoc = linkLocPtr.get();

memset(linkLoc, 0, numPts*sizeof(unsigned short));


NodeList_t::size_type pts[3];
TriangleList_t::size_type npts = 3; // ALWAYS 3 points for a triangle

//vtkPolyData *pdata = static_cast<vtkPolyData *>(data);
// traverse data to determine number of uses of each point
for (cellId=0; cellId < numCells; cellId++)
{
  GetCellPoints(triangles, cellId, npts, pts);
  for (TriangleList_t::size_type j=0; j < npts; j++)
  {
    this->IncrementLinkCount(pts[j]);
  }
}

// now allocate storage for the links
this->AllocateLinks(numPts);
this->MaxId = numPts - 1;

for (cellId=0; cellId < numCells; cellId++)
{
  GetCellPoints(triangles, cellId, npts, pts);
  for (TriangleList_t::size_type j=0; j < npts; j++)
  {
    this->InsertCellReference(pts[j], (linkLoc[pts[j]])++, cellId);
  }
}
#endif

#endif /* _TriangleNeighbors_hpp_H_ */
