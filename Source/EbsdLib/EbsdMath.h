/*
 * EbsbMath.h
 *
 *  Created on: Oct 25, 2011
 *      Author: mjackson
 */

#ifndef _ebsdmath_h_
#define _ebsdmath_h_



#if defined(_MSC_VER)
/* [i_a] MSVC again: see the comment from Microsoft's math.h below (MSVC2005).

   Other compilers may also lack M_PI / M_PI_2 which both are used in the
   OpenEXR (test) code.

   Microsoft says:

       Define _USE_MATH_DEFINES before including math.h to expose these macro
       definitions for common math constants.  These are placed under an #ifdef
       since these commonly-defined names are not part of the C/C standards.

   End of quote.

   The other defines have been added for completeness sake (they exist on
   BSD and Linux at least and other code may expect these).

   Microsoft doesn't define M_2PI ever, other compilers may lack some of these
   too, hence the sequence as it is: load math.h, then see what's
lacking still.
*/
//#define _USE_MATH_DEFINES 1
#endif

#include <math.h>

#include "EbsdLib/EbsdLib.h"

#ifndef M_E
#define M_E            2.7182818284590452354   /* e */
#endif

#ifndef M_LOG2E
#define M_LOG2E        1.4426950408889634074   /* log_2 e */
#endif

#ifndef M_LOG10E
#define M_LOG10E       0.43429448190325182765  /* log_10 e */
#endif

#ifndef M_LN2
#define M_LN2          0.69314718055994530942  /* log_e 2 */
#endif

#ifndef M_LN10
#define M_LN10         2.30258509299404568402  /* log_e 10 */
#endif

#ifndef M_PI_2
#define M_PI_2         1.57079632679489661923132169163975144   /* pi/2 */
#endif

#ifndef M_PI_4
#define M_PI_4         0.785398163397448309615660845819875721  /* pi/4 */
#endif

#ifndef M_1_PI
#define M_1_PI         0.31830988618379067154  /* 1/pi */
#endif

#ifndef M_2_PI
#define M_2_PI         0.63661977236758134308  /* 2/pi */
#endif

#ifndef M_2_SQRTPI
#define M_2_SQRTPI     1.12837916709551257390  /* 2/sqrt(pi) */
#endif

#ifndef M_SQRT2
#define M_SQRT2        1.41421356237309504880  /* sqrt(2) */
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2      0.70710678118654752440  /* 1/sqrt(2) */
#endif

#ifndef M_2PI
#define M_2PI           6.283185307179586232    /* 2*pi  */


#ifndef M_PI
#define M_PI        3.14159265358979323846  /* pi */
#endif

#endif


#endif /* EBSBMATH_H_ */

