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

#ifndef NODE_HPP_
#define NODE_HPP_

namespace smooth
{

  class node
  {

    public:
      node(){}
      virtual ~node(){};

#if 0
      float& operator[](int i)
      {
        return pos[i];
      }
      float operator[](int i) const
      {
        return pos[i];
      }
#endif
      float pos[3];
      int nId;
      int nType;

      void setValues(int node_id, int node_type, float position[3])
      {
        nId = node_id;
        nType = node_type;
        pos[0] = position[0];
        pos[1] = position[1];
        pos[2] = position[2];
      }

      float& getPos(int i)
      {
        return pos[i];
      }
      float getPos(int i) const
      {
        return pos[i];
      }
    private:

  };


  float distance(const node& n0, const node& n1)
  {
    float a = n0.getPos(0) - n1.getPos(0);
    float b = n0.getPos(1) - n1.getPos(1);
    float c = n0.getPos(2) - n1.getPos(2);
    return sqrt(a * a + b * b + c * c);
  }

}

#endif /* NODE_HPP_ */
