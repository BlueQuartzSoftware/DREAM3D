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

#error THIS FILE IS NO LONGER USED. Use DREAM3DRandom.h instead


#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include "MXA/MXATypes.h"
#include "MXA/Common/LogTime.h"

#include "DREAM3D/DREAM3DConfiguration.h"
//#include "DREAM3D/Common/AIMRandomNG.h"

/**
* @class AIMRandomNG AIMRandomNG.h AIM/Common/AIMRandomNG.h
* @brief Random number generator
* @author Michael A. Jackson for BlueQuartz Software
* @date Nov 5, 2009
* @version 1.0
*/
class DREAM3DLib_EXPORT  AIMRandomNG
{
  public:
    AIMRandomNG();
    virtual ~AIMRandomNG();

    void init_genrand(uint32_t);
    /**
     * @brief Returns a random number between 0.0 and 1.0
     */
    double genrand_res53();


    double genrand_beta(double, double);
    double genrand_norm(double, double);


  private:
    double x [ 5 ];

    AIMRandomNG(const AIMRandomNG&); // Copy Constructor Not Implemented
    void operator=(const AIMRandomNG&); // Operator '=' Not Implemented
};

#ifdef CMP_WORDS_BIGENDIAN
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

#define DREAM3D_RANDOMNG_NEW()\
AIMRandomNG rg;\
{\
unsigned long long int seed = MXA::getMilliSeconds();\
unsigned int* seedPtr = reinterpret_cast<unsigned int*> (&seed);\
rg.init_genrand(seedPtr[AIMRNG_OFFSET]);\
}

#define DREAM3D_RANDOMNG_NEW_SEEDED(seed)\
AIMRandomNG rg;\
{\
unsigned int* seedPtr = reinterpret_cast<unsigned int*> (&seed);\
rg.init_genrand(seedPtr[AIMRNG_OFFSET]);\
}

#endif /* AIMRANDOMNG_H_ */
