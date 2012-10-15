#ifndef _MeshFunctions_H_
#define _MeshFunctions_H_


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/SurfaceMeshFilters/MMCSurfaceMeshStructs.h"
#include "DREAM3DLib/SurfaceMeshFilters/StructArray.hpp"
#include "DREAM3DLib/SurfaceMeshFilters/MeshLinearAlgebra.h"



class NodeFunctions
{
  public:
    virtual ~NodeFunctions();

    static double Distance(Node& n0, Node& n1);
    static int type(Node& n);

  protected:
    NodeFunctions();

  private:
    NodeFunctions(const NodeFunctions&); // Copy Constructor Not Implemented
    void operator=(const NodeFunctions&); // Operator '=' Not Implemented
};


class EdgeFunctions
{
    public:
    virtual ~EdgeFunctions();

    static double Length(StructArray<Node>::Pointer nodes, Segment* e);

  protected:
    EdgeFunctions();

  private:
    EdgeFunctions(const EdgeFunctions&); // Copy Constructor Not Implemented
    void operator=(const EdgeFunctions&); // Operator '=' Not Implemented
};

class TriangleFunctions
{
  public:
    virtual ~TriangleFunctions();
    static double area(Node &n0, Node &n1, Node &n2);
    static double aspect(Node &n0, Node &n1, Node &n2);
    static double MinDihedral(Node &n0, Node &n1, Node &n2);
    static double circularity(Node &n0, Node &n1, Node &n2, double area);
    static MFE::Vector<double> normal(Node &n0, Node &n1, Node &n2);

  protected:
     TriangleFunctions();

  private:
     TriangleFunctions(const TriangleFunctions&); // Copy Constructor Not Implemented
     void operator=(const TriangleFunctions&); // Operator '=' Not Implemented

};

#endif /* _MeshFunctions_H_ */
