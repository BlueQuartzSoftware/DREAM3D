/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Michael A. Groeber (US Air Force Research Laboratory)
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

#ifndef RECONSTRUCTIONOUTPUTWRITERS_H_
#define RECONSTRUCTIONOUTPUTWRITERS_H_

#include <string>

#include "MXA/Common/MXASetGetMacros.h"

#include "AIM/Common/ReconstructionFunc.h"


/**
 * @class ReconstructionOutputWriters ReconstructionOutputWriters.h AIM/Common/ReconstructionOutputWriters.h
 * @brief This class holds the functions to write the various output files that
 * result from the Reconstruction Module.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Feb 19, 2011
 * @version 1.0
 */
class ReconstructionVTKWriter
{
  public:
    MXA_SHARED_POINTERS(ReconstructionVTKWriter)
    MXA_STATIC_NEW_MACRO(ReconstructionVTKWriter)
    MXA_TYPE_MACRO(ReconstructionVTKWriter)

      /**
       * @brief Writes a VTK visualization file
       * @param Output file name
       * @return 0 on Success
       */
      int writeVisualizationFile(ReconstructionFunc* r, const std::string &file); // DONE

      /**
       * @brief Writes a VTK visualization file with vector arrays for the Inverse
       * pole figure colors and grain ID.
       * @param Output file name
       * @return 0 on Success
       */
      int writeIPFVizFile(ReconstructionFunc* r, const std::string &file);

      /**
       * @brief Writes a VTK visualization file with vector arrays for the disorientation colors and grain ID.
       * @param Output file name
       * @return 0 on Success
       */
      int writeDisorientationVizFile(ReconstructionFunc* r, const std::string &file);

      /**
       * @brief Writes a VTK visualization file with vector arrays for the Image Quality and grain ID.
       * @param Output file name
       * @return 0 on Success
       */
      int writeImageQualityVizFile(ReconstructionFunc* r, const std::string &file);

      /**
        * @brief Writes a VTK visualization file with vector arrays for the Schmid Factor and grain ID.
        * @param Output file name
        * @return 0 on Success
        */
      int writeSchmidFactorVizFile(ReconstructionFunc* r, const std::string &file);

      /**
        * @brief Writes a low resolution VTK visualization file.
        * @param Output file name
        * @return 0 on Success
        */
      int writeDownSampledVizFile(ReconstructionFunc* r, const std::string &file);

    virtual ~ReconstructionVTKWriter();

  protected:
    ReconstructionVTKWriter();

  private:
    ReconstructionVTKWriter(const ReconstructionVTKWriter&); // Copy Constructor Not Implemented
    void operator=(const ReconstructionVTKWriter&); // Operator '=' Not Implemented
};

#endif /* RECONSTRUCTIONOUTPUTWRITERS_H_ */
