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
   * @class Quaternions Quaternions.h AIM/Common/Quaternions.h
   * @brief This class is just designed to hold the Quaternions for the Cubic and
   * Hexagonal crystal structures. <b>Please not that for historical reasons the size of the array that
   * holds the quaternion is of size 5<b>. The first element of the array holds calculation specific
   * value. The actual quaternion is represented by Elements 1, 2, 3, 4.
   * @author Michael A. Jackson for BlueQuartz Software
   * @date Jan 19, 2011
   * @version 1.0
   */
  class Quaternions
  {
    public:
      ~Quaternions() { }

      /**
       * @brief Multiply by a unit quaternion for Hexagonal
       * @param unitQuat 5 Element array where the actual quaternion is located in elements 1,2,3,4
       * @param i The index of the reference data from AIM::Quaternions::quat_symmhex
       * @param outQuat Output Quaternion where it is a 5 Element array and where the actual 
       * calculated quaternion is located in elements 1,2,3,4
       */
      static AIMCOMMON_EXPORT void Hex_MultiplyByUnitQuaterion(double* unitQuat, size_t i, double* outQuat);

      /**
       * @brief Multiply by a unit quaternion for Cubic
       * @param unitQuat 5 Element array where the actual quaternion is located in elements 1,2,3,4
       * @param i The index of the reference data from AIM::Quaternions::quat_symmcubic
       * @param outQuat Output Quaternion where it is a 5 Element array and where the actual 
       * calculated quaternion is located in elements 1,2,3,4
       */
      static AIMCOMMON_EXPORT void Cubic_MultiplyByUnitQuaterion(double* unitQuat, size_t i, double* outQuat);

      /**
       * @brief Symmetry values for Cubic
       */
      static AIMCOMMON_EXPORT double quat_symmcubic[24][5];

      /**
       * @brief Symmetry values for Hexagonal
       */
      static AIMCOMMON_EXPORT double quat_symmhex[12][5];


    protected:
      Quaternions(){};

    private:
      Quaternions(const Quaternions&); // Copy Constructor Not Implemented
      void operator=(const Quaternions&); // Operator '=' Not Implemented
  };

} // End AIM Namespace
#endif /* QUATERNIONS_H_ */
