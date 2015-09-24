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


#ifndef _MeshFunctions_H_
#define _MeshFunctions_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Geometry/MeshStructs.h"
#include "SIMPLib/Geometry/VertexGeom.h"

#include "SurfaceMeshing/SurfaceMeshingFilters/MeshLinearAlgebra.h"

/**
 * @brief
 */
template<typename Node, typename T>
class NodeFunctions
{
  public:
    virtual ~NodeFunctions() {}

    static T Distance(Node& n0, Node& n1)
    {
      T a = n0.pos[0] - n1.pos[0];
      T b = n0.pos[1] - n1.pos[1];
      T c = n0.pos[2] - n1.pos[2];
      return sqrt(a * a + b * b + c * c);
    }

    static int type(Node& n)
    {
      return n.nodeKind;
    }

  protected:
    NodeFunctions() {}

  private:
    NodeFunctions(const NodeFunctions&); // Copy Constructor Not Implemented
    void operator=(const NodeFunctions&); // Operator '=' Not Implemented
};

/**
 * @brief
 */
template<typename Node, typename T>
class EdgeFunctions
{
  public:
    virtual ~EdgeFunctions() {}

    static T Length(typename VertexGeom::Pointer nodes, int64_t e[2])
    {
      int nid0 = e[0];
      int nid1 = e[1];
      return NodeFunctions<Node, T>::Distance( *(nodes->getVertexPointer(nid0)), *(nodes->getVertexPointer(nid1)) );
    }

  protected:
    EdgeFunctions() {}

  private:
    EdgeFunctions(const EdgeFunctions&); // Copy Constructor Not Implemented
    void operator=(const EdgeFunctions&); // Operator '=' Not Implemented
};


/**
 * @brief
 */
template<typename Node, typename T>
class TriangleFunctions
{
  public:
    virtual ~TriangleFunctions() {}
    static T area(Node& n0, Node& n1, Node& n2)
    {
      double a[3], b[3], c[3];
      a[0] = n1.pos[0] - n0.pos[0];
      a[1] = n1.pos[1] - n0.pos[1];
      a[2] = n1.pos[2] - n0.pos[2];
      b[0] = n2.pos[0] - n0.pos[0];
      b[1] = n2.pos[1] - n0.pos[1];
      b[2] = n2.pos[2] - n0.pos[2];
      c[0] = a[1] * b[2] - a[2] * b[1];
      c[1] = a[2] * b[0] - a[0] * b[2];
      c[2] = a[0] * b[1] - a[1] * b[0];
      return 0.5 * sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]);
    }

    static T aspect(Node& n0, Node& n1, Node& n2)
    {
      T a = NodeFunctions<Node, T>::Distance(n0, n1);
      T b = NodeFunctions<Node, T>::Distance(n1, n2);
      T c = NodeFunctions<Node, T>::Distance(n2, n0);
      T min = a;
      T max = a;
      if(b > max) { max = b; }
      else if(b < min) { min = b; }
      if(c > max) { max = c; }
      else if(c < min) { min = c; }
      return max / min;
    }

    static T MinDihedral(Node& n0, Node& n1, Node& n2)
    {
      //  a points from p0 to p1; b is p0 to p2;  c points p1 to p2
      T a[3], b[3], c[3], min, w1, w2, w3;
      a[0] = n1.pos[0] - n0.pos[0];
      a[1] = n1.pos[1] - n0.pos[1];
      a[2] = n1.pos[2] - n0.pos[2];
      T norm = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
      if(norm > 0.0)
      {
        T rnorm = 1.0 / norm;
        a[0] *= rnorm;
        a[1] *= rnorm;
        a[2] *= rnorm;
      }
      //  qDebug() << "MinDIhedral, a: " << a[0] <<" " << a[1] <<" " << a[2] << "\n" ;
      b[0] = n2.pos[0] - n0.pos[0];
      b[1] = n2.pos[1] - n0.pos[1];
      b[2] = n2.pos[2] - n0.pos[2];
      norm = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
      if(norm > 0.0)
      {
        T rnorm = 1.0 / norm;
        b[0] *= rnorm;
        b[1] *= rnorm;
        b[2] *= rnorm;
      }
      //  qDebug() << "MinDIhedral, b: " << b[0] <<" " << b[1] <<" " << b[2] << "\n" ;
      c[0] = n2.pos[0] - n1.pos[0];
      c[1] = n2.pos[1] - n1.pos[1];
      c[2] = n2.pos[2] - n1.pos[2];
      norm = sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]);
      if(norm > 0.0)
      {
        T rnorm = 1.0 / norm;
        c[0] *= rnorm;
        c[1] *= rnorm;
        c[2] *= rnorm;
      }
      //  qDebug() << "MinDIhedral, c: " << c[0] <<" " << c[1] <<" " << c[2] << "\n" ;
      w1 = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
      if(w1 < -1.) { w1 = -1.; }
      if(w1 > 1.) { w1 = 1.; }
      w2 = -1. * a[0] * c[0] - a[1] * c[1] - a[2] * c[2];
      if(w2 < -1.) { w2 = -1.; }
      if(w2 > 1.) { w2 = 1.; }
      w3 = c[0] * b[0] + c[1] * b[1] + c[2] * b[2];
      if(w3 < -1.) { w3 = -1.; }
      if(w3 > 1.) { w3 = 1.; }
      //  qDebug() << "MinDIhedral, w: " << w1 <<" " << w2 <<" " << w3 << "\n" ;
      T d1 = acos(w1);
      T d2 = acos(w2);
      T d3 = acos(w3);
      // debug
      //  qDebug() << "MinDIhedral, angles: " << d1*180./PI <<" " << d2*180./PI <<" " << d3*180./PI << "\n" ;
      min = d1;
      if(d2 < min) { min = d2; }
      if(d3 < min) { min = d3; }
      return min;
    }

    static T circularity(Node& n0, Node& n1, Node& n2, T area)
    {
      T a = NodeFunctions<Node, T>::Distance(n0, n1);
      T b = NodeFunctions<Node, T>::Distance(n1, n2);
      T c = NodeFunctions<Node, T>::Distance(n2, n0);
      T s = 0.5 * (a + b + c); //  1/2 perimeter
      T r = area / s;
      T R = a * b * c / 4.0 / area;
      R = R / r;
      return R;
    }

    static MFE::Vector<T> normal(Node& n0, Node& n1, Node& n2)
    {
      MFE::Vector<T> n(3);
      T a[3], b[3];

      a[0] = n1.pos[0] - n0.pos[0];
      a[1] = n1.pos[1] - n0.pos[1];
      a[2] = n1.pos[2] - n0.pos[2];
      b[0] = n2.pos[0] - n0.pos[0];
      b[1] = n2.pos[1] - n0.pos[1];
      b[2] = n2.pos[2] - n0.pos[2];
      n[0] = a[1] * b[2] - a[2] * b[1];
      n[1] = a[2] * b[0] - a[0] * b[2];
      n[2] = a[0] * b[1] - a[1] * b[0];
      T norm = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
      if(norm > 0.0)
      {
        T rnorm = 1.0 / norm;
        n[0] *= rnorm;
        n[1] *= rnorm;
        n[2] *= rnorm;
      }
      return n;
    }

  protected:
    TriangleFunctions() {}

  private:
    TriangleFunctions(const TriangleFunctions&); // Copy Constructor Not Implemented
    void operator=(const TriangleFunctions&); // Operator '=' Not Implemented

};

#endif /* _MeshFunctions_H_ */
