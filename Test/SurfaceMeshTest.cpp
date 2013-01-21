

#include <stdio.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>

#include <boost/shared_array.hpp>

#include "MXA/Common/LogTime.h"

#define DREAM3D_BENCHMARKS 1

#if DREAM3D_BENCHMARKS

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
 * @brief The ScopedFileMonitor class will automatically close an open FILE pointer
 * when the object goes out of scope.
 */
class ScopedFileMonitor
{
  public:
    ScopedFileMonitor(FILE* f) : m_File(f) {}
    virtual ~ScopedFileMonitor() { fclose(m_File);}
  private:
    FILE* m_File;
    ScopedFileMonitor(const ScopedFileMonitor&); // Copy Constructor Not Implemented
    void operator=(const ScopedFileMonitor&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class NodesTrianglesReader
{
  public:
    NodesTrianglesReader();
    virtual ~NodesTrianglesReader();

    int read(const std::string &nodesFileName, const std::string &trianglesFileName);

    void setErrorCondition(int err);
    int getErrorCondition();
    std::string getNameOfClass();
    void notifyStatusMessage(const std::string &message);

  private:
    int m_ErrorCondition;

    NodesTrianglesReader(const NodesTrianglesReader&); // Copy Constructor Not Implemented
    void operator=(const NodesTrianglesReader&); // Operator '=' Not Implemented
};


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
      for (size_t i=0; i < n; i++)
      {
        this->Array[i].cells = new int[this->Array[i].ncells];
      }
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
NodesTrianglesReader::NodesTrianglesReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NodesTrianglesReader::~NodesTrianglesReader()
{

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesReader::setErrorCondition(int err)
{
  m_ErrorCondition = err;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NodesTrianglesReader::getErrorCondition()
{
  return m_ErrorCondition;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string NodesTrianglesReader::getNameOfClass()
{
  return std::string("NodesTrianglesReader");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesReader::notifyStatusMessage(const std::string &message)
{
  std::cout << getNameOfClass() << ": " << message << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NodesTrianglesReader::read(const std::string &nodesFileName, const std::string &trianglesFileName)
{

  DEFINE_CLOCK

      //int err = 0;
      std::stringstream s;
  // Open the Nodes file for reading
  FILE* nodesFile = fopen(nodesFileName.c_str(), "rb+");
  if(nodesFile == NULL)
  {
    s.str("");
    s << "Error opening nodes file '" << nodesFileName << "'";
    setErrorCondition(-1);
    //    PipelineMessage em (getHumanLabel(), s.str(), -1);
    //    addErrorMessage(em);
    //    notifyMessage(em);
    return getErrorCondition();
  }
  ScopedFileMonitor nodesMonitor(nodesFile);

  // Calculate how many nodes are in the file based on the file size
  fseek(nodesFile, 0, SEEK_END);
  size_t fLength = ftell(nodesFile);
  size_t nNodes = fLength / SurfaceMesh::NodesFile::ByteCount;
  fseek(nodesFile, 0, SEEK_SET);
  fLength = ftell(nodesFile);
  if(0 != fLength)
  {
    s.str("");
    s << getNameOfClass() << ": Error Could not rewind to beginning of file after nodes count.'" << nodesFileName << "'";
    setErrorCondition(-1);
    //    PipelineMessage em (getHumanLabel(), s.str(), -1);
    //    addErrorMessage(em);
    //    notifyMessage(em);
    return getErrorCondition();
  }
  s.str("");
  s << "Calc Node Count from Nodes.bin File: " << nNodes;
  notifyStatusMessage(s.str());

  // Open the triangles file for reading
  FILE* triFile = fopen(trianglesFileName.c_str(), "rb+");
  if(triFile == NULL)
  {
    s.str("");
    s << getNameOfClass() << ": Error opening Triangles file '" << trianglesFileName << "'";
    setErrorCondition(-1);
    //    PipelineMessage em (getHumanLabel(), s.str(), -1);
    //    addErrorMessage(em);
    //    notifyMessage(em);
    return getErrorCondition();
  }

  ScopedFileMonitor trianglesMonitor(triFile);
  // Calculate how many Triangles are in the file based in the file size
  fseek(triFile, 0, SEEK_END);
  fLength = ftell(triFile);
  size_t nTriangles = fLength / SurfaceMesh::TrianglesFile::ByteCount;
  fseek(triFile, 0, SEEK_SET);
  fLength = ftell(triFile);
  if(0 != fLength)
  {
    s.str("");
    s << getNameOfClass() << ": Error Could not rewind to beginning of file after triangles count.'" << trianglesFileName << "'";
    setErrorCondition(-1);
    //    PipelineMessage em (getHumanLabel(), s.str(), -1);
    //    addErrorMessage(em);
    //    notifyMessage(em);
    return getErrorCondition();
  }
  s.str("");
  s << "Calc Triangle Count from Triangles.bin File: " << nTriangles;
  notifyStatusMessage(s.str());

  // Allocate all the nodes
  std::vector<SurfaceMesh::Node_t> nodes(nNodes);

  s.str("");
  s << "Reading Nodes file into Memory";
  notifyStatusMessage(s.str());
  size_t nread = 0;
  SurfaceMesh::NodesFile::NodesFileRecord_t nRecord;
  for (size_t i = 0; i < nNodes; i++)
  {

    nread = fread(&nRecord, SurfaceMesh::NodesFile::ByteCount, 1, nodesFile); // Read one set of positions from the nodes file
    if(nread != 1)
    {
      break;
    }
    SurfaceMesh::Node_t& node = nodes[nRecord.nodeId];
    node.x = nRecord.x;
    node.y = nRecord.y;
    node.z = nRecord.z;
  }


  START_CLOCK;
  s.str("");
  s << "Reading Triangles file into Memory";
  notifyStatusMessage(s.str());

  // Allocate all the Triangle Objects
  std::vector<SurfaceMesh::Triangle_t> triangles(nTriangles);

  SurfaceMesh::TrianglesFile::TrianglesFileRecord_t tRecord;
  for (size_t i = 0; i < nTriangles; i++)
  {
    // Read from the Input Triangles Temp File
    nread = fread(&tRecord, SurfaceMesh::TrianglesFile::ByteCount, 1, triFile);
    if(nread != 1)
    {
      break;
    }

    SurfaceMesh::Triangle_t& triangle = triangles[tRecord.triId];

    triangle.verts[0] = tRecord.nodeId_0;
    triangle.verts[1] = tRecord.nodeId_1;
    triangle.verts[2] = tRecord.nodeId_2;
    triangle.labels[0] = tRecord.label_0;
    triangle.labels[1] = tRecord.label_1;
  }

  END_CLOCK("Reading Triangles");
  // The ScopedFileMonitor classes will take care of closing the files
  START_CLOCK;
  CellLinks cellLinks;
  cellLinks.Allocate(nodes.size());
  cellLinks.generateCellLinks(nodes, triangles);
  END_CLOCK("Building Cell Links");

//  SurfaceMesh::Edge_t edges[3];
//  unsigned long long int* edgesPtr = reinterpret_cast<unsigned long long int*>(&edges);

//  SurfaceMesh::Edge_t vEdges[3];
//  unsigned long long int* vEdgesPtr = reinterpret_cast<unsigned long long int*>(&vEdges);
  START_CLOCK;
  std::vector<std::vector<int> > triangleNeighbors(nTriangles, std::vector<int>(3));

  // Build up the Triangle Adjacency list now that we have the cell links
  for(size_t t = 0; t < nTriangles; ++t)
  {
 //   std::cout << "Analyzing Triangle " << t << std::endl;
    SurfaceMesh::Triangle_t& triangle = triangles[t];
    std::vector<int>& neighbors = triangleNeighbors[t];
    for(size_t v = 0; v < 3; ++v)
    {
   //   std::cout << " vert " << v << std::endl;
      int nTris = cellLinks.GetNcells(triangle.verts[v]);
      int* vertIdxs = cellLinks.GetCells(triangle.verts[v]);

      for(int vt = 0; vt < nTris; ++vt)
      {
        if (vertIdxs[vt] == t) { continue; }
  //      std::cout << "   Comparing Triangle " << vertIdxs[vt] << std::endl;
        SurfaceMesh::Triangle_t& vertTri = triangles[vertIdxs[vt]];
        int vCount = 0;
        for(int e = 0; e < 3; ++e)
        {
          if (triangle.verts[e] == vertTri.verts[0] || triangle.verts[e] == vertTri.verts[1] || triangle.verts[e] == vertTri.verts[2]  )
          {
            vCount++;
          }
        }
        assert(vCount < 3); // No way can 2 triangles share all 3 vertices. Something is VERY wrong at this point
        if (vCount == 2)
        {
          //std::cout << "       Neighbor: " << vertIdxs[vt] << std::endl;
          neighbors.push_back(vertIdxs[vt]);
        }
      }
    }
    assert(neighbors.size() > 2);
 //   triangleNeighbors.push_back(neighbors);
  }

  END_CLOCK("Building Triangle Neighbor Lists");
  return 0;
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
  NodesTrianglesReader reader;

  std::string nodesFileName = argv[1];
  std::string trianglesFileName = argv[2];
  int err = reader.read(nodesFileName, trianglesFileName);
  std::cout << "Finished" << std::endl;
  return err;
}
