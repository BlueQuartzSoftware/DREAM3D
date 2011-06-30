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
#include <iostream>

#include <boost/shared_array.hpp>
#include "MXA/Common/MXASetGetMacros.h"
#include "DREAM3D/Common/Constants.h"


class Grain
{
public:
  MXA_SHARED_POINTERS(Grain);
  MXA_STATIC_NEW_MACRO(Grain);
  MXA_TYPE_MACRO(Grain);
  virtual ~Grain()
  {
    std::cout << "~Grain" << std::endl;
  }
 IntVectorType     neighborlist;
protected:
  Grain()
  {
    neighborlist = IntVectorType(new std::vector<int>( 0) );
    for(int i = 0; i < 6; ++i)
    {
      neighborlist->push_back(i);
    }
  }

private:
  Grain(const Grain&); // Copy Constructor Not Implemented
  void operator=(const Grain&); // Operator '=' Not Implemented
};


// =======================================================================================
class GrainGeneratorFunc
{
public:
  MXA_SHARED_POINTERS(GrainGeneratorFunc)
    MXA_STATIC_NEW_MACRO(GrainGeneratorFunc)

  virtual ~GrainGeneratorFunc() 
  {  
    std::cout << "~GrainGeneratorFunc" << std::endl;
    std::vector<int*> pointers;
    // Get the pointer to the internal data structure of each vector.
    for(int i = 0; i < 10; ++i)
    {
      pointers.push_back( &(m_Grains[i]->neighborlist->front() ) );
    }

    std::vector<int>* neighborListVectPointer = m_Grains[0]->neighborlist.get();

    m_Grains.clear();
  
    // Now try accessing the pointers
    for(int i = 0; i < 10; ++i)
    {
      int* p = pointers[i];
      std::cout << "Value: " << p[5] << std::endl;
    }
    //try accessing the pointer after it has been deleted
    std::cout << neighborListVectPointer->at(4) << std::endl;
  }
   
  
  std::vector<Grain::Pointer> m_Grains;

    void init()
    {
      for(int i = 0; i < 10; ++i)
      {
        m_Grains.push_back(Grain::New());
      }
    }
protected:
  GrainGeneratorFunc() {}
private:
  GrainGeneratorFunc(const GrainGeneratorFunc& );
  void operator =(const GrainGeneratorFunc& );
};


int main(int argc, char **argv)
{
  std::cout << "Starting...." << std::endl;

  std::vector<int> test(100000000);

  test.resize(0);






  GrainGeneratorFunc::Pointer m = GrainGeneratorFunc::New();
  m->init();

  m = GrainGeneratorFunc::NullPointer();

  std::cout << "Ending " << std::endl;

  return 0;

}

