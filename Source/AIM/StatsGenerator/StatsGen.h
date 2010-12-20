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
#ifndef STATSGEN_H_
#define STATSGEN_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <vector>

/**
* @class StatsGen StatsGen.h
* @author Michael A. Groeber
* @date Dec 16, 2010
* @version 1.0
*/
class StatsGen
{
  public:
    StatsGen();
    virtual ~StatsGen();

	int GenBeta(double p, double q, std::vector<float> &x, std::vector<float> &y, int size);
    int GenLogNormal(double m, double s, std::vector<float> &x, std::vector<float> &y, int size);
    int GenCutOff(double m, double s, double value, std::vector<int> &cutoffs);
	double gamma(double value);

  private:
    StatsGen(const StatsGen&); // Copy Constructor Not Implemented
    void operator=(const StatsGen&); // Operator '=' Not Implemented
};

#endif /* STATSGEN_H_ */
