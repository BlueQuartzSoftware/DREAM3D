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
*  2002 A. Fog. GNU General Public License www.gnu.org/copyleft/gpl.html*
*************************************************************************/
#include "AIMRandomNG.h"
#include "DREAM3D/Common/AIMMath.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMRandomNG::AIMRandomNG()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMRandomNG::~AIMRandomNG()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double AIMRandomNG::Random()
{
  double c;
  c = (double)2111111111.0 * x[3] +
      1492.0 * (x[3] = x[2]) +
      1776.0 * (x[2] = x[1]) +
      5115.0 * (x[1] = x[0]) +
      x[4];
  x[4] = floorl(c);
  x[0] = c - x[4];
  x[4] = x[4] * (1./(65536.*65536.));
  return x[0];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMRandomNG::RandomInit (uint32_t s)
{
  int i;
  //uint32_t s = seed;
  // make random numbers and put them into the buffer
  for (i=0; i<5; i++) {
    s = s * 29943829 - 1;
    x[i] = s * (1./(65536.*65536.));}
  // randomize some more
  for (i=0; i<19; i++) Random();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double AIMRandomNG::RandBeta(double aa,double bb)
{
#define expmax 89.0
#define infnty 1.0E38

static double olda = -1.0;
static double oldb = -1.0;
static double genbet,u1,u2,a,alpha,b,beta,delta,gamma,k1,k2,r,s,t,v,w,y,z;
static long qsame;

    qsame = olda == aa && oldb == bb;
    if(qsame) goto S20;
    olda = aa;
    oldb = bb;
S20:

    if(aa > 1.0 && bb > 1.0) goto S100;
    if(qsame) goto S30;
    a = aa;
    b = bb;
  if(a > b)
  {
    double temp = a;
    a = b;
    b = temp;
  }
    alpha = a+b;
    beta = sqrt((alpha-2.0)/(2.0*a*b-alpha));
    gamma = a+1.0/beta;
S30:
S40:
  u1 = Random();
    v = beta*log(u1/(1.0-u1));
    if(!(v > expmax)) goto S50;
    w = infnty;
    goto S60;
S50:
    w = a*exp(v);
S60:
    z = powf(u1,2.0)*u2;
    r = gamma*v-1.3862944;
    s = a+r-w;
    if(s+2.609438 >= 5.0*z) goto S70;
    t = log(z);
    if(s > t) goto S70;
    if(r+alpha*log(alpha/(b+w)) < t) goto S40;
S70:
    if(!(aa == a)) goto S80;
    genbet = w/(b+w);
    goto S90;
S80:
    genbet = b/(b+w);
S90:
    goto S230;
S100:
    if(qsame) goto S110;
    a = aa;
    b = bb;
  if(b > a)
  {
    double temp = a;
    a = b;
    b = temp;
  }
    alpha = a+b;
    beta = 1.0/b;
    delta = 1.0+a-b;
    k1 = delta*(1.38889E-2+4.16667E-2*b)/(a*beta-0.777778);
    k2 = 0.25+(0.5+0.25/delta)*b;
S110:
S120:
  u1 = Random();
  u2 = Random();
    if(u1 >= 0.5) goto S130;
    y = u1*u2;
    z = u1*y;
    if(0.25*u2+z-y >= k1) goto S120;
    goto S170;
S130:
    z = powf(u1,2.0)*u2;
    if(!(z <= 0.25)) goto S160;
    v = beta*log(u1/(1.0-u1));
    if(!(v > expmax)) goto S140;
    w = infnty;
    goto S150;
S140:
    w = a*exp(v);
S150:
    goto S200;
S160:
    if(z >= k2) goto S120;
S170:
    v = beta*log(u1/(1.0-u1));
    if(!(v > expmax)) goto S180;
    w = infnty;
    goto S190;
S180:
    w = a*exp(v);
S190:
    if(alpha*(log(alpha/(b+w))+v)-1.3862944 < log(z)) goto S120;
S200:
    if(!(a == aa)) goto S210;
    genbet = w/(b+w);
    goto S220;
S210:
    genbet = b/(b+w);
S230:
S220:
    return genbet;
#undef expmax
#undef infnty
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double AIMRandomNG::RandNorm(double m, double s)
{
  const double p0 = 0.322232431088;     const double q0 = 0.099348462606;
  const double p1 = 1.0;                const double q1 = 0.588581570495;
  const double p2 = 0.342242088547;     const double q2 = 0.531103462366;
  const double p3 = 0.204231210245e-1;  const double q3 = 0.103537752850;
  const double p4 = 0.453642210148e-4;  const double q4 = 0.385607006340e-2;
  double u,t, p, q, z;

  u = Random();
  if (u < 0.5)
    t = sqrt(-2.0 * log(u));
  else
    t = sqrt(-2.0 * log(1.0 - u));
  p   = p0 + t * (p1 + t * (p2 + t * (p3 + t * p4)));
  q   = q0 + t * (q1 + t * (q2 + t * (q3 + t * q4)));
  if (u < 0.5)
    z = (p / q) - t;
  else
    z = t - (p / q);
  return (m + s * z);
}
