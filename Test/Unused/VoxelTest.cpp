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

#include <iostream>
#include <vector>
#include <string>


class Patch
{
  public:
    int verts[3];
    virtual ~Patch() {}
    Patch() {};
    Patch(const Patch& p)
    {
      std::cout << "Patch(const Patch& p)" << std::endl;
//      verts[0] = p.verts[0];
//      verts[1] = p.verts[1];
//      verts[2] = p.verts[2];
    }

    void print()
    {
      std::cout << "vert[0]: " << verts[0] << "  vert[1]: " << verts[1] << "  vert[2]: " << verts[2] << std::endl;
    }

  private:

    //void operator=(const Patch&); // Operator '=' Not Implemented


};

int main(int argc, char** argv)
{
  std::cout << "Starting...." << std::endl;

  std::vector<Patch> ps(2);
  ps[0].verts[0] = 111;
  ps[0].verts[1] = 111;
  ps[0].verts[2] = 111;

  Patch& p0 = ps[0];
  p0.verts[0] = 222;
  ps[0].print();
  p0.print();

  p0 = ps[1];
  p0.verts[0] = 333;
  ps[1].print();
  p0.print();


  return 0;

}

