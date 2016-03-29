/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "MeshFunctions.h"

#include "SIMPLib/Common/DREAM3DMath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NodeFunctions::NodeFunctions() {}
NodeFunctions::~NodeFunctions() {}

double NodeFunctions::Distance(Node& n0, Node& n1)
{
  double a = n0.coord[0] - n1.coord[0];
  double b = n0.coord[1] - n1.coord[1];
  double c = n0.coord[2] - n1.coord[2];
  return sqrt(a * a + b * b + c * c);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NodeFunctions::type(Node& n)
{
  return n.nodeKind;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EdgeFunctions::EdgeFunctions() {}
EdgeFunctions::~EdgeFunctions() {}


double EdgeFunctions::Length(StructArray<Node>::Pointer nodes, Segment* e)
{
  int nid0 = e->node_id[0];
  int nid1 = e->node_id[1];
  return NodeFunctions::Distance( *(nodes->GetPointer(nid0)), *(nodes->GetPointer(nid1)) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleFunctions::TriangleFunctions() {}
TriangleFunctions::~TriangleFunctions() {}

double TriangleFunctions::area(Node& n0, Node& n1, Node& n2)
{
  double a[3], b[3], c[3];

  a[0] = n1.coord[0] - n0.coord[0];
  a[1] = n1.coord[1] - n0.coord[1];
  a[2] = n1.coord[2] - n0.coord[2];
  b[0] = n2.coord[0] - n0.coord[0];
  b[1] = n2.coord[1] - n0.coord[1];
  b[2] = n2.coord[2] - n0.coord[2];
  c[0] = a[1] * b[2] - a[2] * b[1];
  c[1] = a[2] * b[0] - a[0] * b[2];
  c[2] = a[0] * b[1] - a[1] * b[0];
  return 0.5 * sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]);
}

MFE::Vector<double> TriangleFunctions::normal(Node& n0, Node& n1, Node& n2)
{
  MFE::Vector<double> n(3);
  double a[3], b[3];

  a[0] = n1.coord[0] - n0.coord[0];
  a[1] = n1.coord[1] - n0.coord[1];
  a[2] = n1.coord[2] - n0.coord[2];
  b[0] = n2.coord[0] - n0.coord[0];
  b[1] = n2.coord[1] - n0.coord[1];
  b[2] = n2.coord[2] - n0.coord[2];
  n[0] = a[1] * b[2] - a[2] * b[1];
  n[1] = a[2] * b[0] - a[0] * b[2];
  n[2] = a[0] * b[1] - a[1] * b[0];
  double norm = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
  if(norm > 0.0)
  {
    double rnorm = 1.0 / norm;
    n[0] *= rnorm;
    n[1] *= rnorm;
    n[2] *= rnorm;
  }
  return n;
}

double TriangleFunctions::aspect(Node& n0, Node& n1, Node& n2)
{
  double a = NodeFunctions::Distance(n0, n1);
  double b = NodeFunctions::Distance(n1, n2);
  double c = NodeFunctions::Distance(n2, n0);
  double min = a;
  double max = a;
  if(b > max) { max = b; }
  else if(b < min) { min = b; }
  if(c > max) { max = c; }
  else if(c < min) { min = c; }
  return max / min;
}

double TriangleFunctions::circularity(Node& n0, Node& n1, Node& n2, double area)
{
  double a = NodeFunctions::Distance(n0, n1);
  double b = NodeFunctions::Distance(n1, n2);
  double c = NodeFunctions::Distance(n2, n0);
  double s = 0.5 * (a + b + c); //  1/2 perimeter
  double r = area / s;
  double R = a * b * c / 4.0 / area;
  R = R / r;
  return R;
}

double TriangleFunctions::MinDihedral(Node& n0, Node& n1, Node& n2) //  another triangle quality measure
{
  //  a points from p0 to p1; b is p0 to p2;  c points p1 to p2
  double a[3], b[3], c[3], min, w1, w2, w3;
  a[0] = n1.coord[0] - n0.coord[0];
  a[1] = n1.coord[1] - n0.coord[1];
  a[2] = n1.coord[2] - n0.coord[2];
  double norm = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
  if(norm > 0.0)
  {
    double rnorm = 1.0 / norm;
    a[0] *= rnorm;
    a[1] *= rnorm;
    a[2] *= rnorm;
  }
  //  qDebug() << "MinDIhedral, a: " << a[0] <<" " << a[1] <<" " << a[2] << "\n" ;
  b[0] = n2.coord[0] - n0.coord[0];
  b[1] = n2.coord[1] - n0.coord[1];
  b[2] = n2.coord[2] - n0.coord[2];
  norm = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
  if(norm > 0.0)
  {
    double rnorm = 1.0 / norm;
    b[0] *= rnorm;
    b[1] *= rnorm;
    b[2] *= rnorm;
  }
  //  qDebug() << "MinDIhedral, b: " << b[0] <<" " << b[1] <<" " << b[2] << "\n" ;
  c[0] = n2.coord[0] - n1.coord[0];
  c[1] = n2.coord[1] - n1.coord[1];
  c[2] = n2.coord[2] - n1.coord[2];
  norm = sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]);
  if(norm > 0.0)
  {
    double rnorm = 1.0 / norm;
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
  double d1 = acos(w1);
  double d2 = acos(w2);
  double d3 = acos(w3);
  // debug
  //  qDebug() << "MinDIhedral, angles: " << d1*180./PI <<" " << d2*180./PI <<" " << d3*180./PI << "\n" ;
  min = d1;
  if(d2 < min) { min = d2; }
  if(d3 < min) { min = d3; }
  return min;
}
