/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "QMetaObjectUtilities.h"

#include <QtCore/QVector>
#include <QtCore/QDataStream>

#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/Common/ComparisonInputs.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"

// These Streams need to be implemented so that our 3 Element Vectors can be read/write to disk/prefs files
QDataStream& operator<<( QDataStream& out, const IntVec3_t& v)
{
  out << v.x << v.y << v.z;
  return out;
}
QDataStream& operator>>( QDataStream& in, IntVec3_t& v)
{
  in >> v.x >> v.y >> v.z;
  return in;
}

QDataStream& operator<<( QDataStream& out, const FloatVec3_t& v)
{
  out << v.x << v.y << v.z;
  return out;
}
QDataStream& operator>>( QDataStream& in, FloatVec3_t& v)
{
  in >> v.x >> v.y >> v.z;
  return in;
}

QDataStream& operator<<( QDataStream& out, const DataArrayPath& v)
{
  out << v.getDataContainerName() << "|" << v.getAttributeMatrixName() << "|" << v.getDataArrayName();
  return out;
}
//QDataStream& operator>>( QDataStream& in, DataArrayPath& v) { in >> v.getDataContainerName() >> "|" >> v.getAttributeMatrixName() >> "|" >> v.getDataArrayName(); return in;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMetaObjectUtilities::QMetaObjectUtilities()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMetaObjectUtilities::~QMetaObjectUtilities()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QMetaObjectUtilities::RegisterMetaTypes()
{
  qRegisterMetaType<DataArrayPath>("DataArrayPath");
  qRegisterMetaType<AxisAngleInput_t>("AxisAngleInput_t");
  qRegisterMetaType<ComparisonInput_t>("ComparisonInput_t");
  qRegisterMetaType<ComparisonInputs>("ComparisonInputs");
  qRegisterMetaType<DataContainerArrayProxy>("DataContainerArrayProxy");
  qRegisterMetaType<IntVec3_t>("IntVec3_t");
  qRegisterMetaType<FloatVec3_t>("FloatVec3_t");
  qRegisterMetaType<PipelineMessage>("PipelineMessage");
  qRegisterMetaType<UInt32Vector_t>("UInt32Vector_t");

  qRegisterMetaTypeStreamOperators<IntVec3_t>("IntVec3_t");
  qRegisterMetaTypeStreamOperators<FloatVec3_t>("FloatVec3_t");
//  qRegisterMetaTypeStreamOperators<DataArrayPath>("DataArrayPath");

}

