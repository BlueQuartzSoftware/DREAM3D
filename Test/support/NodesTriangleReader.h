#ifndef _NodesTriangleReader_H_
#define _NodesTriangleReader_H_


#include <string>

#include "SurfaceMeshDataStructures.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class NodesTrianglesReader
{
  public:
    NodesTrianglesReader();
    virtual ~NodesTrianglesReader();

    int read(const std::string &nodesFileName, const std::string &trianglesFileName);
    NodeList_t& getNodes();
    TriangleList_t& getTriangles();



    void setErrorCondition(int err);
    int getErrorCondition();
    std::string getNameOfClass();
    void notifyStatusMessage(const std::string &message);

  private:
    int m_ErrorCondition;
    NodeList_t m_NodeList;
    TriangleList_t m_TriangleList;

    NodesTrianglesReader(const NodesTrianglesReader&); // Copy Constructor Not Implemented
    void operator=(const NodesTrianglesReader&); // Operator '=' Not Implemented
};







#endif /* _NodesTriangleReader_H_ */
