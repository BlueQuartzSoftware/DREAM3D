#ifndef _CellLinks_hpp_H_
#define _CellLinks_hpp_H_

#include <boost/shared_array.hpp>


/**
 * @brief The CellLinks class contains arrays of Triangles for each Node in the mesh. This allows quick query to the node
 * to determine what Cells the node is a part of.
 */
class CellLinks
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
      static CellLinks::Link linkInit = {0,NULL};

      this->Size = sz;
      if ( this->Array != NULL )
      {
        delete [] this->Array;
      }
      this->Array = new CellLinks::Link[sz];
      this->Extend = ext;
      this->MaxId = -1;

      for (size_t i=0; i < sz; i++)
      {
        this->Array[i] = linkInit;
      }
    }

    CellLinks():Array(NULL),Size(0),MaxId(-1),Extend(1000) {}

    ~CellLinks()
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
    // Allocate memory for the list of lists of cell ids.
    void AllocateLinks(size_t n)
    {
      size_t memory_used = 0;
      for (size_t i=0; i < n; i++)
      {
        this->Array[i].cells = new int[this->Array[i].ncells];
        memory_used= memory_used + (10 + sizeof(int)*this->Array[i].ncells);
      }
      std::cout << "CellLinks Memory Used: " << memory_used << std::endl;
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
    void GetCellPoints(TriangleList_t &triangles, TriangleList_t::size_type cellId, TriangleList_t::size_type npts, NodeList_t::size_type* pts)
    {
      SurfaceMesh::Triangle_t& triangle = triangles[cellId];
      pts[0] = triangle.verts[0];
      pts[1] = triangle.verts[1];
      pts[2] = triangle.verts[2];
    }



    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void generateCellLinks(NodeList_t &nodes, TriangleList_t &triangles)
    {


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
      size_t memory_used = 0;

      for (cellId=0; cellId < numCells; cellId++)
      {
        GetCellPoints(triangles, cellId, npts, pts);
        for (TriangleList_t::size_type j=0; j < npts; j++)
        {
          this->InsertCellReference(pts[j], (linkLoc[pts[j]])++, cellId);
        }
      }

    }

  private:
    Link* Array;   // pointer to data
    size_t Size;
    size_t MaxId;
    size_t Extend;
};



#endif /* _CellLinks_hpp_H_ */
