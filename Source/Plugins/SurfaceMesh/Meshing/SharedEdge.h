/* ============================================================================
 * Copyright (c)  2010 Dr.Sukbin Lee slee@andrew.cmu.edu
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
 * Neither the name of Dr.Sukbin Lee, Carnegie-Mellon University nor the names of
 * its contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef H_m3c_CONSTRUCTION_EDGE
#define H_m3c_CONSTRUCTION_EDGE

#include <set>

#include "MXA/Common/MXASetGetMacros.h"
#include "MXA/MXATypes.h"
#include "DREAM3D/DREAM3DConfiguration.h"

namespace meshing
{

  /**
   * @class SharedEdge SharedEdge.h SurfaceMesh/SharedEdge.h
   * @brief This class represents an Edge as defined by 2 vertices from a triangle.
   * The Edge can be shared by multiple triangles from different grains but never for
   * than 2 triangles for any single grain.
   * @author Michael A. Jackson for BlueQuartz Software
   * @date Jul 11, 2011
   * @version 1.0
   */
  class  SharedEdge
  {
    public:
      /**
       *
       */
      int u, v;
      std::set<int> triangles;

      MXA_SHARED_POINTERS(SharedEdge)

      /**
       *
       */
      static Pointer New(int u_, int v_)
      {
        Pointer sharedPtr(new SharedEdge(u_, v_));
        return sharedPtr;
      }

      /**
       * @brief Return a 64 unsigned integer as the unique ID of this Shared Edge
       * which is a concatenation of the 32 bit integer values for each of the nodes
       */
      uint64_t getId()
      {
        uint64_t edgeId;
        int32_t* e0 = (int32_t*)(&edgeId);
        int32_t* e1 = e0 + 1;
        if (u < v)
        {
          *e0 = u;
          *e1 = v;
        }
        else
        {
          *e0 = v;
          *e1 = u;
        }
        return edgeId;
      }

    protected:
      SharedEdge(int u_, int v_) :
          u(u_),
          v(v_)
      {
      }

    private:
      SharedEdge(const SharedEdge&); // Copy Constructor Not Implemented
      void operator=(const SharedEdge&); // Operator '=' Not Implemented
  };

//  bool operator<(const SharedEdge& lhs, const SharedEdge& rhs);

}

#endif
