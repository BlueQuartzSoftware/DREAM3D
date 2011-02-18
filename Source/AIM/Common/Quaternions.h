/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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

#ifndef QUATERNIONS_H_
#define QUATERNIONS_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif


#include "AIM/Common/AIMCommonConfiguration.h"

#include <string>

namespace AIM
{
  /**
   * @brief This class is just designed to hold the Quarterions for the Cubic and
   * hexagonal crystal structures.
   */
  class Quaternions
  {
    public:
      ~Quaternions() { }
      static AIMCOMMON_EXPORT void Hex_MultiplyByUnitQuaterion(double* unitQuat, size_t i, double* outQuat);
      static AIMCOMMON_EXPORT void Cubic_MultiplyByUnitQuaterion(double* unitQuat, size_t i, double* outQuat);
      static AIMCOMMON_EXPORT double quat_symmcubic[24][5];
      static AIMCOMMON_EXPORT double quat_symmhex[12][5];
    protected:
      Quaternions(){};

    private:
      Quaternions(const Quaternions&); // Copy Constructor Not Implemented
      void operator=(const Quaternions&); // Operator '=' Not Implemented
  };

} // End AIM Namespace
#endif /* QUATERNIONS_H_ */
