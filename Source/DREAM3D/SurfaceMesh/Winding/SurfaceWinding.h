/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef SURFACEWINDING_H_
#define SURFACEWINDING_H_


#include <string>


#include <MXA/MXATypes.h>
#include "MXA/Common/MXASetGetMacros.h"
#include "DREAM3D/DREAM3DConfiguration.h"

#include "DREAM3D/DREAM3DConfiguration.h"
#include "Facet.hpp"

namespace m3c {
/*
 *
 */
class DREAM3DLib_EXPORT SurfaceWinding
{
  public:
    SurfaceWinding();
    virtual ~SurfaceWinding();


    typedef int16_t Label;
    typedef m3c::Facet<Label>                   TriangleType;

    typedef TriangleType::ContainerT                  TriangleListType;
    typedef TriangleType::ContainerPType              TriangleListPtrType;
    typedef std::map<Label, int >                     LabelTriangleMapType;
    typedef TriangleType::ContainerPType              TriangleContainerPointerType;
    typedef std::map<uint64_t, m3c::Edge::Pointer>    EdgeMapType;
    typedef boost::shared_ptr<EdgeMapType>   EdgeMapPointer;


    TriangleListType masterTriangleList;


    void analyzeWinding(int nNodes, int nTriangle, const std::string &triangleFile);
    void debugPrintConnectivity(int nNodes, int nTriangle, const std::string &triangleFile);

    SurfaceWinding::TriangleContainerPointerType
          findAdjacentTriangles( TriangleType::Pointer triangle, Label label);

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

};

}

#endif /* SURFACEWINDING_H_ */
