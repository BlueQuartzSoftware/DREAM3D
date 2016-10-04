/***
 * Name:  Vector3.cpp
 * Purpose: m3c_basics.math.vectors.Vector3 Implementation
 * Notice:  Copyright Stuart Golodetz, 2008. All rights reserved.
* This work is licensed under the Creative Commons Attribution-NonCommercial 3.0
* Unported License. To view a copy of this license, visit
* http://creativecommons.org/licenses/by-nc/3.0/
* or send a letter to Creative Commons,
* 171 Second Street, Suite 300,
* San Francisco, California, 94105, USA.
 ***/

#include "Vector3.h"

#include <cmath>

#include "SurfaceMeshing/SurfaceMeshingFilters/util/Exception.h"

namespace Vector3Detail
{

// double EPSILON = 0.0001;
double EPSILON = 0.0001f;
double PI = 3.141592654;
double SMALL_EPSILON = 0.0000000001;
}

using m3c_basics::exceptions::Exception;
using namespace Vector3Detail;

//##################RUCTORS ##################//
template <typename T>
Vector3<T>::Vector3()
: x(0)
, y(0)
, z(0)
{
}

template <typename T>
Vector3<T>::Vector3(double x_, double y_, double z_)
: x(static_cast<T>(x_))
, y(static_cast<T>(y_))
, z(static_cast<T>(z_))
{
}

template <typename T>
Vector3<T>::Vector3(float* node)
: x(static_cast<T>(node[0]))
, y(static_cast<T>(node[1]))
, z(static_cast<T>(node[2]))
{
}

//################## PUBLIC OPERATORS ##################//
template <typename T> Vector3<T>& Vector3<T>::operator+=(Vector3& rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

template <typename T> Vector3<T>& Vector3<T>::operator-=(Vector3& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  return *this;
}

template <typename T> Vector3<T>& Vector3<T>::operator*=(double factor)
{
  x *= static_cast<T>(factor);
  y *= static_cast<T>(factor);
  z *= static_cast<T>(factor);
  return *this;
}

template <typename T> Vector3<T>& Vector3<T>::operator/=(double factor)
{
  x /= static_cast<T>(factor);
  y /= static_cast<T>(factor);
  z /= static_cast<T>(factor);
  return *this;
}

template <typename T> Vector3<T> Vector3<T>::operator-()
{
  return Vector3(-x, -y, -z);
}

//################## PUBLIC METHODS ##################//
template <typename T> Vector3<T> Vector3<T>::cross(Vector3& rhs)
{
  return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}

template <typename T> double Vector3<T>::distance(Vector3& rhs)
{
  return sqrt(distance_squared(rhs));
}

template <typename T> double Vector3<T>::distance_squared(Vector3& rhs)
{
  double dx = x - rhs.x;
  double dy = y - rhs.y;
  double dz = z - rhs.z;
  return dx * dx + dy * dy + dz * dz;
}

template <typename T> double Vector3<T>::dot(Vector3& rhs)
{
  return x * rhs.x + y * rhs.y + z * rhs.z;
}

template <typename T> double Vector3<T>::dot(float* rhs)
{
  return x * rhs[0] + y * rhs[1] + z * rhs[2];
}

template <typename T> double Vector3<T>::length()
{
  return sqrt(length_squared());
}

template <typename T> double Vector3<T>::length_squared()
{
  return x * x + y * y + z * z;
}

template <typename T> Vector3<T>& Vector3<T>::negate()
{
  x = -x;
  y = -y;
  z = -z;
  return *this;
}

template <typename T> Vector3<T>& Vector3<T>::normalize()
{
  double len = length();
  if(len < SMALL_EPSILON)
  {
    throw Exception("Unable to normalize vector: too close to zero");
  }
  return (*this) *= 1.0 / len;
}

template <typename T> Vector3<T> Vector3<T>::project_onto(Vector3& rhs)
{
  // (lhs . rhs / |rhs|) * (rhs / |rhs|) = rhs * (lhs . rhs / |rhs|^2)
  return rhs * (dot(rhs) / rhs.length_squared());
}

//################## GLOBAL OPERATORS ##################//
template <typename T> Vector3<T> operator+(Vector3<T>& lhs, Vector3<T>& rhs)
{
  Vector3<T> copy = lhs;
  copy += rhs;
  return copy;
}

template <typename T> Vector3<T> operator-(Vector3<T>& lhs, Vector3<T>& rhs)
{
  Vector3<T> copy = lhs;
  copy -= rhs;
  return copy;
}

template <typename T> Vector3<T> operator*(double factor, Vector3<T>& v)
{
  Vector3<T> copy = v;
  copy *= factor;
  return copy;
}

template <typename T> Vector3<T> operator*(Vector3<T>& v, double factor)
{
  Vector3<T> copy = v;
  copy *= factor;
  return copy;
}

template <typename T> Vector3<T> operator/(Vector3<T>& v, double factor)
{
  Vector3<T> copy = v;
  copy /= factor;
  return copy;
}

template <typename T> std::ostream& operator<<(std::ostream& os, Vector3<T>& v)
{
  os << '(' << v.x << ',' << v.y << ',' << v.z << ')';
  return os;
}

//################## EXPLICIT INSTANTIATIONS ##################//
template struct Vector3<float>;
template Vector3f operator+(Vector3f& lhs, Vector3f& rhs);
template Vector3f operator-(Vector3f& lhs, Vector3f& rhs);
template Vector3f operator*(double factor, Vector3f& v);
template Vector3f operator*(Vector3f& v, double factor);
template Vector3f operator/(Vector3f& v, double factor);
template std::ostream& operator<<(std::ostream& os, Vector3f& v);

template struct Vector3<double>;
template Vector3d operator+(Vector3d& lhs, Vector3d& rhs);
template Vector3d operator-(Vector3d& lhs, Vector3d& rhs);
template Vector3d operator*(double factor, Vector3d& v);
template Vector3d operator*(Vector3d& v, double factor);
template Vector3d operator/(Vector3d& v, double factor);
template std::ostream& operator<<(std::ostream& os, Vector3d& v);
