
/* ============================================================================
* Copyright (c) 2012 Dr. Anthony Rollet (Carnegie-Mellon University)
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
* Neither the name of Anthony Rollet, Sukbin Lee, Jason Gruber, Stephen Sintay
* nor the names of its contributors may be used to endorse
* or promote products derived from this software without specific prior written
* permission.
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
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/*

 2) Attached are two files for mesh smoothing code, let me know if the face nodes are acting weird.
 You'd need two input files,  nodes.txt and triangles.txt from Sukbin's meshing analysis.
 And the output file has steve's up&down .inp file convention.

 -Fatma


 */

// LinearAlgebra.h
#ifndef _meshlinearalgebra_h_
#define _meshlinearalgebra_h_
#include<vector>
#include<cmath>
#include<iostream>


namespace MFE
{

  template<typename type>
  class Vector
  {
    public:
      Vector(int n)
      {
        x.assign(n, 0.0);
      }
      type& operator[](int i)
      {
        return x[i];
      }
      type operator[](int i) const
      {
        return x[i];
      }
      void operator=(const Vector&);
      void operator=(type c);
      Vector operator+(const Vector&) const;
      Vector operator-(const Vector&) const;
      Vector operator*(type c) const;
      void operator+=(const Vector&);
      void operator-=(const Vector&);
      void operator*=(type c);
      int dimension() const
      {
        return x.size();
      }
    private:
      std::vector<type> x;
  };

  template<typename type>
  void Vector<type>::operator=(const Vector<type>& v)
  {
    int n = dimension();
    for (int i = 0; i < n; i++)
    { x[i] = v[i]; }
  }

  template<typename type>
  void Vector<type>::operator=(type c)
  {
    int n = dimension();
    for (int i = 0; i < n; i++)
    { x[i] = c; }
  }

  template<typename type>
  Vector<type> Vector<type>::operator+(const Vector<type>& v) const
  {
    int n = dimension();
    Vector<type> y(n);
    for (int i = 0; i < n; i++)
    { y[i] = x[i] + v[i]; }
    return y;
  }

  template<typename type>
  Vector<type> Vector<type>::operator-(const Vector<type>& v) const
  {
    int n = dimension();
    Vector<type> y(n);
    for (int i = 0; i < n; i++)
    { y[i] = x[i] - v[i]; }
    return y;
  }

  template<typename type>
  Vector<type> Vector<type>::operator*(type c) const
  {
    int n = dimension();
    Vector<type> y(n);
    for (int i = 0; i < n; i++)
    { y[i] = c * x[i]; }
    return y;
  }

  template<typename type>
  void Vector<type>::operator+=(const Vector<type>& v)
  {
    int n = dimension();
    for (int i = 0; i < n; i++)
    { x[i] += v[i]; }
  }

  template<typename type>
  void Vector<type>::operator-=(const Vector<type>& v)
  {
    int n = dimension();
    for (int i = 0; i < n; i++)
    { x[i] -= v[i]; }
  }

  template<typename type>
  void Vector<type>::operator*=(type c)
  {
    int n = dimension();
    for (int i = 0; i < n; i++)
    { x[i] *= c; }
  }

  template<typename type>
  Vector<type> operator*(type c, const Vector<type>& x)
  {
    int n = x.dimension();
    Vector<type> y(n);
    for (int i = 0; i < n; i++)
    { y[i] = c * x[i]; }
    return y;
  }

  template<typename type>
  type inner(const Vector<type>& x, const Vector<type>& y)
  {
    int n = x.dimension();
    type inner = 0.0;
    for (int i = 0; i < n; i++)
    { inner += x[i] * y[i]; }
    return inner;
  }

  template<typename type>
  type norm(const Vector<type>& x)
  {
    int n = x.dimension();
    type norm = 0.0;

    for (int i = 0; i < n; i++)
    { norm += x[i] * x[i]; }
    return sqrt(norm);
  }

  template<typename type>
  type inorm(const Vector<type>& x)
  {
    int n = x.dimension();
    type norm = 0.0;
    for (int i = 0; i < n; i++)
    {
      type value = fabs(x[i]);
      if(value > norm) { norm = value; }
    }
    return norm;
  }

  template<typename type>
  class Matrix
  {
    public:
      Matrix(int m, int n)
      {
        x.assign(m, Vector<type>(n));
      }
      Vector<type>& operator[](int i)
      {
        return x[i];
      }
      const Vector<type>& operator[](int i) const
      {
        return x[i];
      }
      Vector<type> operator*(const Vector<type>&) const;
      int dimension1() const
      {
        return x.size();
      }
      int dimension2() const
      {
        return x[0].dimension();
      }
    private:
      std::vector<Vector<type> > x;
  };

  template<typename type>
  Vector<type> Matrix<type>::operator*(const Vector<type>& v) const
  {
    const Matrix<type>& A = *this;
    int m = dimension1();
    int n = dimension2();
    Vector<type> b(n);
    for (int i = 0; i < m; i++)
      for (int j = 0; j < n; j++)
      { b[i] += A[i][j] * v[j]; }
    return b;
  }

  template<typename type>
  type inorm(const Matrix<type>& A)
  {
    int m = A.dimension1();
    type norm = 0.0;
    std::vector<type> x;
    for (int i = 0; i < m; i++)
    {
      type value = inorm(A[i]);
      if(value > norm) { norm = value; }
    }
    return norm;
  }

  template<typename vtype = double, typename itype = unsigned int>
  class SVector
  {
      struct Sitem
      {
        vtype value;
        itype index;
      };
    public:
      SVector(int n)
      {
        d = n;
      }
      vtype& operator[](itype index);
      vtype operator[](itype index) const;
      vtype value(int i) const
      {
        return data[i].value;
      }
      itype index(int i) const
      {
        return data[i].index;
      }
      int nonzero() const
      {
        return data.size();
      }
      int dimension() const
      {
        return d;
      }
    private:
      std::vector<Sitem> data;
      int d;
  };

  template<typename vtype, typename itype>
  vtype& SVector<vtype, itype>::operator[](itype index)
  {
    int n = nonzero();
    for (int i = 0; i < n; i++)
      if(data[i].index == index) { return data[i].value; }
    Sitem item;
    item.value = 0.0;
    item.index = index;
    data.push_back(item);
    return data.back().value;
  }

  template<typename vtype, typename itype>
  vtype SVector<vtype, itype>::operator[](itype index) const
  {
    int n = nonzero();
    for (int i = 0; i < n; i++)
      if(data[i].index == index) { return data[i].value; }
    return 0.0;
  }

  template<typename vtype, typename itype>
  SVector<vtype, itype> operator*(vtype c, const SVector<vtype, itype>& x)
  {
    int n = x.nonzero();
    SVector<vtype, itype> y(x.dimension());
    for (int i = 0; i < n; i++)
    {
      itype index = x.index(i);
      vtype value = x.value(i);
      y[index] = c * value;
    }
    return y;
  }

  template<typename type>
  type inorm(const SVector<type>& x)
  {
    int n = x.nonzero();
    type norm = 0.0;
    for (int i = 0; i < n; i++)
    {
      type value = fabs(x.value(i));
      if(value > norm) { norm = value; }
    }
    return norm;
  }

  template<typename vtype = double, typename itype = unsigned int>
  class SMatrix
  {
    public:
      SMatrix(int m, int n)
      {
        data.assign(m, SVector<vtype, itype>(n));
      }
      SVector<vtype, itype>& operator[](int i)
      {
        return data[i];
      }
      const SVector<vtype, itype>& operator[](int i) const
      {
        return data[i];
      }
      Vector<vtype> operator*(const Vector<vtype>&) const;
      int dimension1() const
      {
        return data.size();
      }
      int dimension2() const
      {
        return data[0].dimension();
      }
    private:
      std::vector<SVector<vtype, itype> > data;
  };

  template<typename vtype, typename itype>
  Vector<vtype> SMatrix<vtype, itype>::operator*(const Vector<vtype>& x) const
  {
    int m = dimension1();
    int n = dimension2();
    Vector<vtype> b(n);
    for (int i = 0; i < m; i++)
    {
      int N = data[i].nonzero();
      for (int j = 0; j < N; j++)
      {
        vtype value = data[i].value(j);
        itype index = data[i].index(j);
        b[i] += value * x[index];
      }
    }

    return b;
  }

  template<typename type>
  type inorm(const SMatrix<type>& A)
  {
    int m = A.dimension1();
    type norm = 0.0;
    std::vector<type> x;
    for (int i = 0; i < m; i++)
    {
      type value = inorm(A[i]);
      if(value > norm) { norm = value; }
    }
    return norm;
  }

// Iterative solution methods

  template<typename matrix, typename vector, typename type>
  int CG(const matrix& A, vector& x, const vector& b, int max, type tolerance)
  {
    // Conjugate residual (CR) algorithm
    // Use for solving symmetric positive definite linear systems
    int n = x.dimension();
    vector r(n), p(n), q(n);
    type alpha, rho, rho1;

    r = b - A * x;
    type bnorm = norm(b);
    type rnorm = norm(r);
    if(bnorm == 0.0)
    {
      x = 0.0;
      return 0;
    }
    if((rnorm / bnorm) <= tolerance) { return 0; }

    rho = rnorm * rnorm;
    p = r;
    q = A * p;
    alpha = rho / inner(p, q);
    x = x + alpha * p;
    r = r - alpha * q;
    rnorm = norm(r);
    if((rnorm / bnorm) <= tolerance) { return 1; }
    rho1 = rho;

    for (int iteration = 2; iteration <= max; iteration++)
    {
      rho = rnorm * rnorm;
      p = r + (rho / rho1) * p;
      q = A * p;
      alpha = rho / inner(p, q);
      x = x + alpha * p;
      r = r - alpha * q;
      rnorm = norm(r);
      if((rnorm / bnorm) <= tolerance) { return iteration; }
      rho1 = rho;
    }

    return -1;
  }

  template<typename matrix, typename vector, typename type>
  int CR(const matrix& A, vector& x, const vector& b, int max, type tolerance)
  {
    // Conjugate residual (CR) algorithm
    // Use for solving symmetric, possibly indefinite linear systems
    int n = x.dimension();
    vector p(n), r(n), Ar(n), Ap(n);

    r = b - A * x;
    type bnorm = norm(b);
    type rnorm = norm(r);
    if(bnorm == 0.0)
    {
      x = 0.0;
      return 0;
    }
    if((rnorm / bnorm) <= tolerance) { return 0; }
    p = r;
    Ar = A * r;
    Ap = Ar;

    for (int iteration = 1; iteration <= max; iteration++)
    {
      type inner1 = inner(r, Ar);
      type alpha = inner1 / inner(Ap, Ap);
      x += alpha * p;
      r -= alpha * Ap;
      rnorm = norm(r);
      if(rnorm / bnorm < tolerance) { return iteration; }
      Ar = A * r;
      type beta = inner(r, Ar) / inner1;
      p = r + beta * p;
      Ap = Ar + beta * Ap;
    }

    return -1;
  }

  template<typename matrix, typename vector, typename type>
  int GMRES(const matrix& A, vector& x, const vector& b, int m, int max, type tolerance)
  {
    // Generalized minimum residual algorithm
    // Use for solving general (non-symmetric, indefinite) linear systems
    int n = x.dimension();
    Vector<type> w(n), r(n), cs(m + 1), sn(m + 1), s(m + 1), y(m);
    Matrix<type> H(m + 1, m);
    std::vector<Vector<type> > v(m + 1, Vector<type>(n));

    r = b - A * x;
    type bnorm = norm(b);
    type rnorm = norm(r);
    if(bnorm == 0.0)
    {
      x = 0.0;
      return 0;
    }
    if((rnorm / bnorm) <= tolerance) { return 0; }

    for (int iteration = 1; iteration <= max; iteration++)
    {
      v[0] = r * (1.0 / rnorm);
      s = 0.0;
      s[0] = rnorm;
      for (int i = 0; i < m; i++)
      {
        w = A * v[i];
        for (int j = 0; j <= i; j++)
        {
          H[j][i] = inner(w, v[j]);
          w -= H[j][i] * v[j];
        }
        H[i + 1][i] = norm(w);
        v[i + 1] = w * (1.0 / H[i + 1][i]);

        for (int j = 0; j < i; j++)
        {
          type val = cs[j] * H[j][i] + sn[j] * H[j + 1][i];
          H[j + 1][i] = -sn[j] * H[j][i] + cs[j] * H[j + 1][i];
          H[j][i] = val;
        }

        type value = 1.0 / sqrt(H[i][i] * H[i][i] + H[i + 1][i] * H[i + 1][i]);
        cs[i] = H[i][i] * value;
        sn[i] = H[i + 1][i] * value;

        value = cs[i] * H[i][i] + sn[i] * H[i + 1][i];
        H[i + 1][i] = -sn[i] * H[i][i] + cs[i] * H[i + 1][i];
        H[i][i] = value;

        value = cs[i] * s[i] + sn[i] * s[i + 1];
        s[i + 1] = -sn[i] * s[i] + cs[i] * s[i + 1];
        s[i] = value;

        if(fabs(s[i + 1]) / bnorm < tolerance)
        {
          y = 0.0;
          for (int j = i; j >= 0; j--)
          {
            y[j] = s[j] / H[j][j];
            for (int k = j - 1; k >= 0; k--)
            { s[k] -= H[k][j] * y[j]; }
          }
          for (int j = 0; j <= i; j++)
          { x += v[j] * y[j]; }
          return iteration;
        }
      }

      for (int i = m - 1; i >= 0; i--)
      {
        y = 0.0;
        y[i] = s[i] / H[i][i];
        for (int j = i - 1; j >= 0; j--)
        { s[j] -= H[j][i] * y[i]; }
      }
      for (int i = 0; i < m; i++)
      { x += v[i] * y[i]; }

      r = b - A * x;
      rnorm = norm(r);
      if((rnorm / bnorm) <= tolerance) { return iteration; }
    }

    return -1;
  }

// Direct solution methods

  template<typename matrix, typename vector>
  void TridiagonalSolve(const matrix& A, vector& x, const vector& b)
  {
    // Direct solver for tridiagonal linear systems
    // Use for matrices satisfying Aij=0 when |i-j|>1
    int n = x.dimension();
    vector f(n), c(n);

    f[0] = A[0][1];
    c[0] = b[0] / A[0][0];

    for (int i = 0; i < n; i++)
    {
      double d = 1.0 / (A[i][i] - A[i][i - 1] * f[i - 1]);
      f[i] = d * A[i][i + 1];
      c[i] = d * (b[i] - A[i][i - 1] * c[i - 1]);
    }

    x[n - 1] = c[n - 1];
    for (int i = n - 2; i >= 0; i--)
    { x[i] = c[i] - f[i] * x[i + 1]; }
  }

}

#endif
