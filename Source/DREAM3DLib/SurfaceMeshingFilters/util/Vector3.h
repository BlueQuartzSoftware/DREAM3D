/***
 * Name:  Vector3.h
 * Purpose: m3c_basics.math.vectors.Vector3 Interface
 * Notice:  Copyright Stuart Golodetz, 2008. All rights reserved.
 ***/

#ifndef H_CENTIPEDE_BASICS_MATH_VECTORS_VECTOR3
#define H_CENTIPEDE_BASICS_MATH_VECTORS_VECTOR3

#include <QtCore/QtDebug>

#include "DREAM3DLib/DataContainers/MeshStructs.h"

template <typename T>
struct Vector3
{
  //################## PUBLIC VARIABLES ##################//
  T x, y, z;

  //##################RUCTORS ##################//
  Vector3();
  Vector3(double x_, double y_, double z_);
  Vector3(DREAM3D::Mesh::Vert_t& node);

  //################## PUBLIC OPERATORS ##################//
  Vector3& operator+=(Vector3& rhs);
  Vector3& operator-=(Vector3& rhs);
  Vector3& operator*=(double factor);
  Vector3& operator/=(double factor);
  Vector3 operator-();

  //################## PUBLIC METHODS ##################//
  Vector3 cross(Vector3& rhs);
  double distance(Vector3& rhs);
  double distance_squared(Vector3& rhs);
  double dot(Vector3& rhs);
  double dot(DREAM3D::Mesh::Vert_t& rhs);
  double length();
  double length_squared();
  Vector3& negate();
  Vector3& normalize();
  Vector3 project_onto(Vector3& rhs);
};

//################## GLOBAL OPERATORS ##################//
template <typename T> Vector3<T> operator+(Vector3<T>& lhs, Vector3<T>& rhs);
template <typename T> Vector3<T> operator-(Vector3<T>& lhs, Vector3<T>& rhs);
template <typename T> Vector3<T> operator*(double factor, Vector3<T>& v);
template <typename T> Vector3<T> operator*(Vector3<T>& v, double factor);
template <typename T> Vector3<T> operator/(Vector3<T>& v, double factor);
template <typename T> QDataStream& operator<<(QDataStream& os, Vector3<T>& v);

//################## TYPEDEFS ##################//
typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;


typedef double           FloatType;
typedef Vector3<FloatType>  VectorType;


#endif
