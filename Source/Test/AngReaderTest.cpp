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
#include <string>


#include "AIM/ANG/AngReader.h"

#if _WIN32_

#else
  #define ANG_FILE "/Users/Shared/Data/Ang_Data/Test1/post_HT_25.ang"
#endif

int main(int argc, char **argv)
{
  std::cout << "Starting AngReaderTest...." << std::endl;
  std::string angFile(ANG_FILE);
  AngReader reader;
  reader.setFileName(angFile);
  int err = reader.readHeaderOnly();

  std::vector<AngPhase::Pointer> phases = reader.getPhases();
  if (phases.size() != 2)
  {
    std::cout << "Phase Count is Wrong. Should be 2 and is " << phases.size() << std::endl;
  }

  for (std::vector<AngPhase::Pointer>::iterator iter = phases.begin(); iter != phases.end(); ++iter )
  {
    (*iter)->printSelf(std::cout);
  }

  if (err < 0)
  {
    std::cout << "Error reading file" << angFile << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Ending AngReaderTest" << std::endl;
  return EXIT_SUCCESS;
}
