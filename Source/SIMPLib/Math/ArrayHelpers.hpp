#ifndef _ArrayHelpers_H_
#define _ArrayHelpers_H_

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <limits>
#include <iomanip>

#include <SIMPLib/Math/SIMPLibMath.h>

/**
 * @brief ArrayHelpers: Some quick methods that act on an "array" of POD values.
 * The template parameters are:
 * @param T The type such as std::vector, QVector or another type that implements
 * the [] operators, size() functions.
 * @param K The POD type such as float or double.
 */
template<typename T, typename K>
class ArrayHelpers
{
  public:

    virtual ~ArrayHelpers(){}

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
    ArrayHelpers(){}

  private:
    ArrayHelpers(const ArrayHelpers&); // Copy Constructor Not Implemented
    void operator=(const ArrayHelpers&); // Operator '=' Not Implemented
};

#endif /* _ArrayHelpers_H_ */
