/************************** MOTHER.CPP ****************** AgF 1999-03-03 *
*  'Mother-of-All' random number generator                               *
*                                                                        *
*  This is a multiply-with-carry type of random number generator         *
*  invented by George Marsaglia.  The algorithm is:                      *
*  S = 2111111111*X[n-4] + 1492*X[n-3] + 1776*X[n-2] + 5115*X[n-1] + C   *
*  X[n] = S modulo 2^32                                                  *
*  C = floor(S / 2^32)                                                   *
*                                                                        *
*  IMPORTANT:
*  This implementation uses a long double for C. Note that not all       *
*  computers and compilers support the long double (80-bit) floating     *
*  point format. It is recommended to use a Borland or Gnu compiler on   *
*  a PC. The Microsoft compiler doesn't support the long double format.  *
*  You will get an error message if your system doesn't support this.    *
*                                                                        *
* © 2002 A. Fog. GNU General Public License www.gnu.org/copyleft/gpl.html*
*************************************************************************/
#ifndef AIMRANDOMNG_H_
#define AIMRANDOMNG_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include "AIM/Common/AIMCommonConfiguration.h"
#include <MXA/MXATypes.h>

/**
* @class AIMRandomNG AIMRandomNG.h AIM/Common/AIMRandomNG.h
* @brief Random number generator
* @author Michael A. Jackson for BlueQuartz Software
* @date Nov 5, 2009
* @version 1.0
*/
class AIMCOMMON_EXPORT  AIMRandomNG
{
  public:
    AIMRandomNG();
    virtual ~AIMRandomNG();

    void RandomInit(uint32_t);
    double Random();
    double RandBeta(double, double);
    double RandNorm(double, double);
    double x [ 5 ];

  private:
    AIMRandomNG(const AIMRandomNG&); // Copy Constructor Not Implemented
    void operator=(const AIMRandomNG&); // Operator '=' Not Implemented
};

#if CMP_WORDS_BIGENDIAN
#define AIMRNG_OFFSET 1
#else
#define AIMRNG_OFFSET 0
#endif

/* Get a seed value based off the system clock. The issue is that this will
  * be a 64 bit unsigned integer where the high 32 bits will basically not
  * change where as the lower 32 bits will. The following lines of code will
  * pull off the low 32 bits from the number. This operation depends on most
  * significant byte ordering which is different between Big Endian and
  * Little Endian machines. For Big endian machines the Most Significant Byte
  * (MSB) is the first 32 bits. For Little Endian machines the MSB is the
  * second 32 bits.
  */

#define AIM_RANDOMNG_NEW()\
AIMRandomNG rg;\
{\
unsigned long long int seed = MXA::getMilliSeconds();\
unsigned int* seedPtr = reinterpret_cast<unsigned int*> (&seed);\
rg.RandomInit(seedPtr[AIMRNG_OFFSET]);\
}

#endif /* AIMRANDOMNG_H_ */
