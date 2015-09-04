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





#include "SIMPLib/Math/QuaternionMath.hpp"
#include "OrientationLib/OrientationMath/OrientationMath.h"


#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "OrientationLibTestFileLocations.h"


void Print(std::ostream& o, QuatF& q)
{
  o << "{" << "<" << q.w << "> (" << q.x << ", " << q.y << ", " << q.z << ") }";
}


// Floating-point modulo
// The result (the remainder) has same sign as the divisor.
// Similar to matlab's mod(); Not similar to fmod() -   Mod(-3,4)= 1   fmod(-3,4)= -3
template<typename T>
class Mod
{
  public:
    Mod() {}
    virtual ~Mod() {}

    T operator()(T x, T y)
    {
      //ASSERT(!std::numeric_limits<T>::is_exact , "Mod: floating-point type expected");

      if (0. == y)
      { return x; }

      double m = x - y * floor(x / y);

      // handle boundary cases resulted from floating-point cut off:

      if (y > 0)              // modulo range: [0..y)
      {
        if (m >= y)         // Mod(-1e-16             , 360.    ): m= 360.
        { return 0; }

        if (m < 0 )
        {
          if (y + m == y)
          { return 0  ; } // just in case...
          else
          { return y + m; } // Mod(106.81415022205296 , _TWO_PI ): m= -1.421e-14
        }
      }
      else                    // modulo range: (y..0]
      {
        if (m <= y)         // Mod(1e-16              , -360.   ): m= -360.
        { return 0; }

        if (m > 0 )
        {
          if (y + m == y)
          { return 0  ; } // just in case...
          else
          { return y + m; } // Mod(-106.81415022205296, -_TWO_PI): m= 1.421e-14
        }
      }

      return m;
    }

    // wrap [rad] angle to [-PI..PI)
    inline static T WrapPosNegPI(T fAng)
    {
      return Mod()(fAng + SIMPLib::Constants::k_Pi, SIMPLib::Constants::k_2Pi) - SIMPLib::Constants::k_Pi;
    }

    // wrap [rad] angle to [0..TWO_PI)
    inline T WrapTwoPI(T fAng)
    {
      return Mod()(fAng, SIMPLib::Constants::k_2Pi);
    }

    // wrap [deg] angle to [-180..180)
    inline T WrapPosNeg180(T fAng)
    {
      return Mod()(fAng + 180.0, 360.0) - 180.0;
    }

    // wrap [deg] angle to [0..360)
    inline T Wrap360(T fAng)
    {
      return Mod()(fAng , 360.0);
    }

  private:
    Mod(const Mod&); // Copy Constructor Not Implemented
    void operator=(const Mod&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestFMod()
{
  Mod<float> fmod;
  float pi = SIMPLib::Constants::k_Pi;

  pi = fmod.WrapTwoPI(pi);

  pi = pi * -1.0;
  pi = fmod.WrapTwoPI(pi);

  pi = 0.0;
  pi = fmod.WrapTwoPI(pi);

  pi = -1.0f;
  pi = fmod.WrapTwoPI(pi);

  pi = 7.0f;
  pi = fmod.WrapTwoPI(pi);

}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( TestFMod() )

  PRINT_TEST_SUMMARY();
  return err;
}




