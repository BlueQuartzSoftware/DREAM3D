#ifndef _OrientationMathHelpers_H_
#define _OrientationMathHelpers_H_

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <limits>
#include <iomanip>

#include <SIMPLib/Math/SIMPLibMath.h>


template<typename T, typename K>
class OrientationMathHelpers
{
  public:

    virtual ~OrientationMathHelpers(){}

    static void splat(T& a, const K val)
    {
      size_t size = a.size();
      for(size_t i = 0; i < size; i++)
      {
        a[i] = val;
      }
    }

    static T multiply(const T& a, const T& b)
    {
      T c(a.size());
      for(size_t i = 0; i < c.size(); i++)
      {
        c[i] = a[i] * b[i];
      }
      return c;
    }

    static T multiply(const T& a, const T& b, size_t max)
    {
      T c(max);
      for(size_t i = 0; i < max; i++)
      {
        c[i] = a[i] * b[i];
      }
      return c;
    }

    static void scalarMultiply(T& a, K b)
    {
      size_t size = a.size();
      for(size_t i = 0; i < size; i++)
      {
        a[i] = a[i] * b;
      }
    }

    static void scalarDivide(T& a, K b)
    {
      size_t size = a.size();
      for(size_t i = 0; i < size; i++)
      {
        a[i] = a[i] / b;
      }
    }


    static K sum(const T& a)
    {
      K s = static_cast<K>(0);
      for(size_t i = 0; i < a.size(); i++)
      {
        s += a[i];
      }
      return s;
    }


    static K sum(const T& a, size_t max)
    {
      K s = static_cast<K>(0);
      for(size_t i = 0; i < max; i++)
      {
        s += a[i];
      }
      return s;
    }

    static K sumofSquares(const T& a)
    {
      K s = static_cast<K>(0);
      for(size_t i = 0; i < a.size(); i++)
      {
        s += a[i] * a[i];
      }
      return s;
    }

    static K maxval(const T& a)
    {
      K s = a[0];
      for(size_t i = 1; i < a.size(); i++)
      {
        if (a[i] > s) { s = a[i]; }
      }
      return s;
    }

    static T absValue(const T& a)
    {
      T c(a.size());
      for(size_t i = 0; i < c.size(); i++)
      {
        c[i] = (a[i] < 0.0 ? -a[i] : a[i]);
      }
      return c;
    }

    static T transpose(const T& a)
    {
      T c(a.size());
      c[0] = a[0];
      c[1] = a[3];
      c[2] = a[6];
      c[3] = a[1];
      c[4] = a[4];
      c[5] = a[7];
      c[6] = a[2];
      c[7] = a[5];
      c[8] = a[8];
      return c;
    }

    static void transpose(const T& a, T& c)
    {
   //   T c(a.size());
      c[0] = a[0];
      c[1] = a[3];
      c[2] = a[6];
      c[3] = a[1];
      c[4] = a[4];
      c[5] = a[7];
      c[6] = a[2];
      c[7] = a[5];
      c[8] = a[8];
     // return c;
    }

    static T matmul3x3(const T& a, const T& b)
    {
      T c(a.size());
      c[0] = a[0] * b[0] + a[1] * b[3] + a[2] * b[6];
      c[1] = a[0] * b[1] + a[1] * b[4] + a[2] * b[7];
      c[2] = a[0] * b[2] + a[1] * b[5] + a[2] * b[8];
      c[3] = a[3] * b[0] + a[4] * b[3] + a[5] * b[6];
      c[4] = a[3] * b[1] + a[4] * b[4] + a[5] * b[7];
      c[5] = a[3] * b[2] + a[4] * b[5] + a[5] * b[8];
      c[6] = a[6] * b[0] + a[7] * b[3] + a[8] * b[6];
      c[7] = a[6] * b[1] + a[7] * b[4] + a[8] * b[7];
      c[8] = a[6] * b[2] + a[7] * b[5] + a[8] * b[8];
      return c;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------

    static bool closeEnough(const K& a, const K& b,
                            const K& epsilon = std::numeric_limits<K>::epsilon())
    {
      return (epsilon > fabs(a - b));
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static K transfer_sign(K a, K b)
    {
      if( a > 0.0 && b > 0.0) { return a; }
      if( a < 0.0 && b > 0.0) { return -1 * a; }

      if( a < 0.0 && b < 0.0) { return a; }

      return -1 * a;

    }

    /* Auxiliary routine: printing eigenvectors */
    static void print_eigenvectors( const char* desc, int n, float* wi, float* v, int ldv )
    {
      int i, j;
      printf( "\n %s\n", desc );
      for( i = 0; i < n; i++ )
      {
        j = 0;
        while( j < n )
        {
          if( wi[j] == (float)0.0 )
          {
            printf( " %6.2f", v[i + j * ldv] );
            j++;
          }
          else
          {
            printf( " (%6.2f,%6.2f)", v[i + j * ldv], v[i + (j + 1)*ldv] );
            printf( " (%6.2f,%6.2f)", v[i + j * ldv], -v[i + (j + 1)*ldv] );
            j += 2;
          }
        }
        printf( "\n" );
      }
    }

    /**
     * @brief print
     * @param c
     * @param out
     */
    static void print(const T& c, std::ostream &out)
    {
      out.width(16);
      for(size_t i = 0; i < c.size(); i++)
      {
        out << std::fixed << std::setprecision( 16 ) << c[i];
        if(i < c.size() - 1) { out << " "; }
        if (i == c.size() - 1) { out << std::endl; }
      }
    }


  protected:
    OrientationMathHelpers(){}

  private:
    OrientationMathHelpers(const OrientationMathHelpers&); // Copy Constructor Not Implemented
    void operator=(const OrientationMathHelpers&); // Operator '=' Not Implemented
};

#endif /* _OrientationMathHelpers_H_ */
