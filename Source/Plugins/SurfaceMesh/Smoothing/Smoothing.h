/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
#ifndef SMOOTHGRAIN3D_H_
#define SMOOTHGRAIN3D_H_

#include <string>

#include <MXA/Common/MXASetGetMacros.h>

#include "DREAM3D/DREAM3DConfiguration.h"


namespace DREAM3D {
  namespace SurfaceSmoothing
  {
    const std::string Smooth3DIterationFile("Smooth3DIterationFile");
    const std::string NodesSmoothedFile("NodesSmoothedFile.txt");
  }
}

namespace smooth {

class Smoothing
{
  public:
    MXA_SHARED_POINTERS(Smoothing);
    MXA_STATIC_NEW_MACRO(Smoothing);

    virtual ~Smoothing();

    MXA_INSTANCE_STRING_PROPERTY(NodesFile);
    MXA_INSTANCE_STRING_PROPERTY(TrianglesFile);
    MXA_INSTANCE_STRING_PROPERTY(OutputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(OutputFilePrefix)
    MXA_INSTANCE_PROPERTY(size_t, OutputInterval);
    MXA_INSTANCE_PROPERTY(size_t, Iterations);
    MXA_INSTANCE_PROPERTY(bool, LockQuads);

    int execute();



  protected:
    Smoothing();

  private:
    Smoothing(const Smoothing&); // Copy Constructor Not Implemented
    void operator=(const Smoothing&); // Operator '=' Not Implemented
};

}

#endif /* SMOOTHGRAIN3D_H_ */
