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
#ifndef _HKLConstants_h_
#define _HKLConstants_h_

#include <string>

#define DECLARE_STRING_CONST(var)\
  const std::string var(#var);


namespace Ebsd {

namespace Ctf
{
  const std::string FileExt("ctf");

  const std::string Header("Header");
  const std::string ZStartIndex("ZStartIndex");
  const std::string ZEndIndex("ZEndIndex");
  const std::string ZResolution("ZResolution");
  const std::string Manufacturer("Manufacturer");
  const std::string HKL("HKL");
  const std::string OriginalFile("OriginalFile");
  const std::string Index("Index");
  const std::string Data("Data");

// These are Header related
  const std::string ChannelTextFile("Channel Text File");
  DECLARE_STRING_CONST(Prj);
  DECLARE_STRING_CONST(Author);
  DECLARE_STRING_CONST(JobMode);
  DECLARE_STRING_CONST(XCells);
  DECLARE_STRING_CONST(YCells);
  DECLARE_STRING_CONST(XStep);
  DECLARE_STRING_CONST(YStep);
  DECLARE_STRING_CONST(AcqE1);
  DECLARE_STRING_CONST(AcqE2);
  DECLARE_STRING_CONST(AcqE3);
  DECLARE_STRING_CONST(Euler);
  DECLARE_STRING_CONST(Mag);
  DECLARE_STRING_CONST(Coverage);
  DECLARE_STRING_CONST(Device);
  DECLARE_STRING_CONST(KV);
  DECLARE_STRING_CONST(TiltAngle);
  DECLARE_STRING_CONST(TiltAxis);
  DECLARE_STRING_CONST(Phases);

  // These are phase related
  DECLARE_STRING_CONST(LatticeDimensions);
  DECLARE_STRING_CONST(LatticeAngles);
  DECLARE_STRING_CONST(PhaseName);
  DECLARE_STRING_CONST(LaueGroup);
  DECLARE_STRING_CONST(Section4);
  DECLARE_STRING_CONST(Section5);
  DECLARE_STRING_CONST(Section6);
  DECLARE_STRING_CONST(Comment);

  // These are the names of the Data Columns
  DECLARE_STRING_CONST(Phase);
  DECLARE_STRING_CONST(X);
  DECLARE_STRING_CONST(Y);
  DECLARE_STRING_CONST(BandCount);
  DECLARE_STRING_CONST(Error);
  DECLARE_STRING_CONST(Euler1);
  DECLARE_STRING_CONST(Euler2);
  DECLARE_STRING_CONST(Euler3);
  DECLARE_STRING_CONST(MeanAngularDeviation);
  DECLARE_STRING_CONST(BandContrast); // BC
  DECLARE_STRING_CONST(BandSlope); // BS

} // End namespace Ctf

} // End Namespace Ebsd

#endif /* _HKLConstants.h_  */
