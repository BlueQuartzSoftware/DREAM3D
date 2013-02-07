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




#ifndef MicCONSTANTS_H_
#define MicCONSTANTS_H_

#include <string>
#include <vector>

#define DECLARE_STRING_CONST(var)\
  const std::string var(#var);


/**
 * @file MicConstants.h
 * @brief This file contains both c style macro definitions and C++ style const
 * declarations of HEDM OIM variables and names. If you are dealing with a .Mic
 * file from HEDM instrumentation then you MUST use the defines from this file.
 */

// -----------------------------------------------------------------------------
//  These defines are needed for the pure "C" parser
// -----------------------------------------------------------------------------


namespace Ebsd {

 namespace Mic
 {
   const std::string Manufacturer("HEDM");

   const std::string FileExt("mic");

  DECLARE_STRING_CONST(XDim)
  DECLARE_STRING_CONST(YDim)
  DECLARE_STRING_CONST(XRes)
  DECLARE_STRING_CONST(YRes)
  DECLARE_STRING_CONST(PhaseName)

   const std::string Euler1("Euler1");
   const std::string Euler2("Euler2");
   const std::string Euler3("Euler3");
   const std::string X("X");
   const std::string Y("Y");
   const std::string Confidence("Confidence");
   const std::string Phase("Phase");
   const std::string Level("Level");
   const std::string Up("Up");

//   const std::string ZStep(Mic_Z_STEP);
//   const std::string ZPos(Mic_Z_POS);
//   const std::string ZMax(Mic_Z_MAX);

 }
}


#endif /* MicCONSTANTS_H_ */
