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

#ifndef TRIANGLE_HPP_
#define TRIANGLE_HPP_

#include <vector>

#include "Node.hpp"


namespace smooth
{
  class triangle
  {
    public:

      virtual ~triangle() {}
      int region1;
      int region2;

      node& getNode(int i)
      {
        return *p[i];
      }

      void setNode(int i, node* n)
      {
        p[i] = n;
      }

      void setNodes(node* n0, node* n1, node* n2)
      {
        p[0] = n0;
        p[1] = n1;
        p[2] = n2;
      }
#if 0
      node& operator[](int i)
      {
        return *p[i];
      }
      const node& operator[](int i) const
      {
        return *p[i];
      }
      node*& operator()(int i)
      {
        return p[i];
      }
      node* const & operator()(int i) const
      {
        return p[i];
      }
#endif

      float area() const
      {
        float a[3], b[3], c[3];
        a[0] = (*p[1]).getPos(0) - (*p[0]).getPos(0);
        a[1] = (*p[1]).getPos(1) - (*p[0]).getPos(1);
        a[2] = (*p[1]).getPos(2) - (*p[0]).getPos(2);
        b[0] = (*p[2]).getPos(0) - (*p[0]).getPos(0);
        b[1] = (*p[2]).getPos(1) - (*p[0]).getPos(1);
        b[2] = (*p[2]).getPos(2) - (*p[0]).getPos(2);
        c[0] = a[1] * b[2] - a[2] * b[1];
        c[1] = a[2] * b[0] - a[0] * b[2];
        c[2] = a[0] * b[1] - a[1] * b[0];
        return 0.5 * sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]);
      }

      Vector<float> normal() const
      {
        Vector<float> n(3);
        float a[3], b[3];
        a[0] = (*p[1]).getPos(0) - (*p[0]).getPos(0);
        a[1] = (*p[1]).getPos(1) - (*p[0]).getPos(1);
        a[2] = (*p[1]).getPos(2) - (*p[0]).getPos(2);
        b[0] = (*p[2]).getPos(0) - (*p[0]).getPos(0);
        b[1] = (*p[2]).getPos(1) - (*p[0]).getPos(1);
        b[2] = (*p[2]).getPos(2) - (*p[0]).getPos(2);
        n[0] = a[1] * b[2] - a[2] * b[1];
        n[1] = a[2] * b[0] - a[0] * b[2];
        n[2] = a[0] * b[1] - a[1] * b[0];
        float norm = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
        if (norm > 0.0)
        {
          float rnorm = 1.0 / norm;
          n[0] *= rnorm;
          n[1] *= rnorm;
          n[2] *= rnorm;
        }
        return n;
      }

      float aspect() const
      {
        float a = distance(*p[0], *p[1]);
        float b = distance(*p[1], *p[2]);
        float c = distance(*p[2], *p[0]);
        float min = a;
        float max = a;
        if (b > max) max = b;
        else if (b < min) min = b;
        if (c > max) max = c;
        else if (c < min) min = c;
        return max / min;
      }

      float circularity(float area) const
      {
        float a = distance(*p[0], *p[1]);
        float b = distance(*p[1], *p[2]);
        float c = distance(*p[2], *p[0]);
        float s = 0.5 * (a + b + c);
        float r = area / s;
        float R = a * b * c / 4 / area;
        return R / r;
      }


    private:
      node* p[3];
  };



}

#endif /* TRIANGLE_HPP_ */
