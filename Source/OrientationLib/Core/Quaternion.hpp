#pragma once

#include <array>
#include <cmath>

#include "SIMPLib/Math/SIMPLibMath.h"

#include "OrientationLib/Core/Orientation.hpp"

template <typename T>
class Quaternion : public Orientation<T>
{
public:
  using EnumType = unsigned int;

  enum class Order : EnumType
  {
    ScalarVector = 0,
    VectorScalar = 1
  };

  using self = Quaternion<T>;
  using size_type = size_t;
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;

  Quaternion()
  : Orientation<T>(4)
  {
    T* quat = this->data();
    quat[0] = 0.0;
    quat[1] = 0.0;
    quat[2] = 0.0;
    quat[3] = 1.0;
  }

  Quaternion(size_type size)
  : Orientation<T>(4)
  {
    assert(size == 4);
    T* quat = this->data();
    quat[0] = 0.0;
    quat[1] = 0.0;
    quat[2] = 0.0;
    quat[3] = 1.0;
  }

  Quaternion(T x, T y, T z, T w)
  : Orientation<T>(4)
  {
    T* quat = this->data();
    quat[0] = x;
    quat[1] = y;
    quat[2] = z;
    quat[3] = w;
  }

  Quaternion(T* q, Order o = Order::VectorScalar)
  : Orientation<T>(q, static_cast<size_t>(4))
  , m_Order(o)
  {
    T* quat = this->data();
    if(o == Order::VectorScalar)
    {
      quat[0] = q[0];
      quat[1] = q[1];
      quat[2] = q[2];
      quat[3] = q[3];
    }
    else
    {
      quat[3] = q[0];
      quat[0] = q[1];
      quat[1] = q[2];
      quat[2] = q[3];
    }
  }

  reference x()
  {
    if(m_Order == Order::VectorScalar)
    {
      return (*this)[0];
    }
    return (*this)[1];
  }

  const_reference x() const
  {
    if(m_Order == Order::VectorScalar)
    {
      return (*this)[0];
    }
    return (*this)[1];
  }

  reference y()
  {
    if(m_Order == Order::VectorScalar)
    {
      return (*this)[1];
    }
    return (*this)[2];
  }

  const_reference y() const
  {
    if(m_Order == Order::VectorScalar)
    {
      return (*this)[1];
    }
    return (*this)[2];
  }

  reference z()
  {
    if(m_Order == Order::VectorScalar)
    {
      return (*this)[2];
    }
    return (*this)[3];
  }

  const_reference z() const
  {
    if(m_Order == Order::VectorScalar)
    {
      return (*this)[2];
    }
    return (*this)[3];
  }

  reference w()
  {
    if(m_Order == Order::VectorScalar)
    {
      return (*this)[3];
    }
    return (*this)[0];
  }

  const_reference w() const
  {
    if(m_Order == Order::VectorScalar)
    {
      return (*this)[3];
    }
    return (*this)[0];
  }

  /**
   * @brief Identity Sets the quaternion q to the identity quaternion (<0,0,0>,1)
   * @param q
   */
  void identity()
  {
    T* quat = this->data();
    quat[0] = 0.0;
    quat[1] = 0.0;
    quat[2] = 0.0;
    quat[3] = 1.0;
  }

  /**
   * @brief ElementWiseAbs inline assigns the absolute value of each element to itself
   * @param q
   */
  void elementWiseAbs()
  {
    T* quat = this->data();
    quat[0] = std::fabs(quat[0]);
    quat[1] = std::fabs(quat[1]);
    quat[2] = std::fabs(quat[2]);
    quat[3] = std::fabs(quat[3]);
  }

  /**
   * @brief ScalarMultiply Multiplies each element in the quaternion by the argument v
   * @param v
   */
  void scalarMultiply(T v)
  {
    T* quat = this->data();
    quat[0] = quat[0] * v;
    quat[1] = quat[1] * v;
    quat[2] = quat[2] * v;
    quat[3] = quat[3] * v;
  }

  /**
   * @brief ScalarDivide Divides each element in the quaternion by the argument v
   * @param v
   */
  void scalarDivide(T v)
  {
    T* quat = this->data();
    quat[0] = quat[0] / v;
    quat[1] = quat[1] / v;
    quat[2] = quat[2] / v;
    quat[3] = quat[3] / v;
  }

  /**
   * @brief ScalarAdd Adds value to each element of the vector and scalar part of the Quaternion
   * @param v Input Quat to add elements
   */
  void scalarAdd(T v)
  {
    T* quat = this->data();
    quat[0] = quat[0] + v;
    quat[1] = quat[1] + v;
    quat[2] = quat[2] + v;
    quat[3] = quat[3] + v;
  }

  /**
   * @brief ElementWiseAssign Assigns each element the quaternion
   * @param v Input Quat to add elements
   */
  void elementWiseAssign(T v)
  {
    this->assign(4, v);
    //    T* quat = this->data();
    //    quat[0] = v;
    //    quat[1] = v;
    //    quat[2] = v;
    //    quat[3] = v;
  }

  /**
   * @brief Negate  −q = (−a, −v) In Place operation
   */
  void negate()
  {
    T* quat = this->data();
    quat[0] = quat[0] * -1.0;
    quat[1] = quat[1] * -1.0;
    quat[2] = quat[2] * -1.0;
    quat[3] = quat[3] * -1.0;
  }

  /**
   * @brief Add   q1 + q2 = (w1+w2, v1+v2)
   * @param rhs
   * @return out
   */
  self operator+(const Quaternion& rhs) const
  {
    self out;
    const T* quat = this->data();
    out[0] = rhs[0] + quat[0];
    out[1] = rhs[1] + quat[1];
    out[2] = rhs[2] + quat[2];
    out[3] = rhs[3] + quat[3];
    return out;
  }

  /**
   * @brief Add   q1 - q2 = (w1-w2, v1-v2)
   * @param rhs
   * @return out
   */
  self operator-(const Quaternion& rhs) const
  {
    self out;
    const T* quat = this->data();
    out[0] = quat[0] - rhs[0];
    out[1] = quat[1] - rhs[1];
    out[2] = quat[2] - rhs[2];
    out[3] = quat[3] - rhs[3];
    return out;
  }

  /**
   * @brief Multiply current quaternion by another quaternion returning a third quaternion according to quaternion
   * multiplication. Note that Quaternioin multiplication is NOT cummunitive i.e., A * B != B * A
   * @param rhs Input Quaternion
   * @param out Result
   */
  self operator*(const Quaternion& rhs) const
  {
    self out;
    const T* quat = this->data();
    out[0] = rhs[0] * quat[3] + rhs[3] * quat[0] + rhs[2] * quat[1] - rhs[1] * quat[2];
    out[1] = rhs[1] * quat[3] + rhs[3] * quat[1] + rhs[0] * quat[2] - rhs[2] * quat[0];
    out[2] = rhs[2] * quat[3] + rhs[3] * quat[2] + rhs[1] * quat[0] - rhs[0] * quat[1];
    /* Verified */
    out[3] = rhs[3] * quat[3] - rhs[0] * quat[0] - rhs[1] * quat[1] - rhs[2] * quat[2];
    return out;
  }

  /**
   * @brief Multiply current quaternion by another quaternion returning a third quaternion according to quaternion
   * multiplication. Note that Quaternioin multiplication is NOT cummunitive i.e., A * B != B * A
   * @param rhs Input Quaternion
   * @param out Result
   */
  self operator*(Quaternion& rhs) const
  {
    self out;
    const T* quat = this->data();
    out[0] = rhs[0] * quat[3] + rhs[3] * quat[0] + rhs[2] * quat[1] - rhs[1] * quat[2];
    out[1] = rhs[1] * quat[3] + rhs[3] * quat[1] + rhs[0] * quat[2] - rhs[2] * quat[0];
    out[2] = rhs[2] * quat[3] + rhs[3] * quat[2] + rhs[1] * quat[0] - rhs[0] * quat[1];
    /* Verified */
    out[3] = rhs[3] * quat[3] - rhs[0] * quat[0] - rhs[1] * quat[1] - rhs[2] * quat[2];
    return out;
  }
  /**
   * @brief Conjugate Converts quaternion q into its conjugate
   * @return new quaternioin that is the conjugate of the current quaternion
   */
  self conjugate() const
  {
    self out((*this)[0] * -1.0, (*this)[1] * -1.0, (*this)[2] * -1.0, (*this)[3]);
    return out;
  }

  /**
   * @brief Norm Computes and returns the "norm" of the quaternion (x^2 + y^2 + z^2 + w^2)
   * @return
   */
  T norm()
  {
    T* quat = this->data();
    return quat[0] * quat[0] + quat[1] * quat[1] + quat[2] * quat[2] + quat[3] * quat[3];
  }

  /**
   * @brief Length Computes are returns the "length" of the quaternion which is the square root of the norm. SQRT (x^2 + y^2 + z^2 + w^2)
   * @return
   */
  T length() const
  {
    const value_type* quat = this->data();
    return std::sqrt(quat[0] * quat[0] + quat[1] * quat[1] + quat[2] * quat[2] + quat[3] * quat[3]);
  }

  /**
   * @brief UnitQuaternion (Normalize) Converts the quaternion into its normalized values (x/L, y/L, z/L, w/L) where "L"
   * is the "length" of the quaternion
   * @return qr
   */
  self unitQuaternion() const
  {
    self out;
    const value_type* quat = this->data();
    value_type l = length();
    out[0] = static_cast<T>(quat[0] / l);
    out[1] = static_cast<T>(quat[1] / l);
    out[2] = static_cast<T>(quat[2] / l);
    out[3] = static_cast<T>(quat[3] / l);
    return out;
  }

  /**
   * @brief GetMisorientationVector Converts the quaternion into a misorientation vector in the reference frame of the quaternion
   * @return misoVec
   */
  std::array<value_type, 3> getMisorientationVector() const
  {
    std::array<value_type, 3> misoVec;
    const value_type* quat = this->data();
    value_type qw = quat[3];
    SIMPLibMath::bound(qw, static_cast<value_type>(-1.0), static_cast<value_type>(1.0));
    value_type constVal = 0.0;
    if(qw == 1.0 || qw == -1.0)
    {
      constVal = 0.0;
    }
    else
    {
      constVal = 2 * std::acos(qw) / (std::sqrt(1.0 - (qw * qw)));
    }

    misoVec[0] = float(quat[0] * constVal);
    misoVec[1] = float(quat[1] * constVal);
    misoVec[2] = float(quat[2] * constVal);
    return misoVec;
  }

  /**
   * @brief MultiplyQuatVec Rotates a 3d vector 'v' by the quaternion 'q'
   * @param q Input Quaternion
   * @param v Input Vector
   * @param out Output Vector
   * SIMPLView uses
   * PASSIVE rotations by default.
   */
  std::array<value_type, 3> multiplyByVector(T* v) const
  {
    const value_type* quat = this->data();
    value_type qx2 = quat[0] * quat[0];
    value_type qy2 = quat[1] * quat[1];
    value_type qz2 = quat[2] * quat[2];
    value_type qw2 = quat[3] * quat[3];

    value_type qxy = quat[0] * quat[1];
    value_type qyz = quat[1] * quat[2];
    value_type qzx = quat[2] * quat[0];

    value_type qxw = quat[0] * quat[3];
    value_type qyw = quat[1] * quat[3];
    value_type qzw = quat[2] * quat[3];

    std::array<value_type, 3> out;

    out[0] = v[0] * (qx2 - qy2 - qz2 + qw2) + 2 * (v[1] * (qxy + qzw) + v[2] * (qzx - qyw));
    out[1] = v[1] * (qy2 - qx2 - qz2 + qw2) + 2 * (v[2] * (qyz + qxw) + v[0] * (qxy - qzw));
    out[2] = v[2] * (qz2 - qx2 - qy2 + qw2) + 2 * (v[0] * (qzx + qyw) + v[1] * (qyz - qxw));
    return out;
  }

private:
  Order m_Order = Order::VectorScalar;
};

using QuatType = Quaternion<double>;
using QuatF = Quaternion<float>;
