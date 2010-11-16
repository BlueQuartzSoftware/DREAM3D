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


#include <MXA/MXATypes.h>

/**
* @class AIMRandomNG AIMRandomNG.h AIM/Common/AIMRandomNG.h
* @brief Random number generator
* @author Michael A. Jackson for BlueQuartz Software
* @date Nov 5, 2009
* @version 1.0
*/
class AIMRandomNG
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

#endif /* AIMRANDOMNG_H_ */
