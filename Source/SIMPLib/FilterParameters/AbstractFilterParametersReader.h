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


#ifndef _AbstractFilterParametersReader_H_
#define _AbstractFilterParametersReader_H_

#include <QtCore/QString>
#include <set>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/ComparisonInputs.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec4FilterParameter.h"
#include "SIMPLib/FilterParameters/Symmetric6x6FilterParameter.h"
#include "SIMPLib/FilterParameters/SecondOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/ThirdOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/FourthOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"

class AbstractFilter;

#define READ_ARRAY_SELECTION_PARAMETER(Name)\
  setSelected##Name##Arrays( reader->readArraySelections(#Name, getSelected##Name##Arrays() ) );



/**
 * @class AbstractFilterParametersReader AbstractFilterParametersReader.h DREAM3DLib/Common/AbstractFilterParametersReader.h
 * @brief This class reads the various parameters of a filter
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 17, 2012
 * @version 1.0
 */
class SIMPLib_EXPORT AbstractFilterParametersReader
{
  public:

    SIMPL_TYPE_MACRO(AbstractFilterParametersReader)


    virtual ~AbstractFilterParametersReader();

    virtual int openFilterGroup(AbstractFilter* filter, int index) = 0;
    virtual int closeFilterGroup() = 0;


    virtual QString readString(const QString name, QString value) = 0;
    virtual QVector<QString> readStrings(const QString name, QVector<QString> value) = 0;

    virtual int8_t readValue(const QString name, int8_t value) = 0;
    virtual int16_t readValue(const QString name, int16_t value) = 0;
    virtual int32_t readValue(const QString name, int32_t value) = 0;
    virtual int64_t readValue(const QString name, int64_t value) = 0;
    virtual uint8_t readValue(const QString name, uint8_t value) = 0;
    virtual uint16_t readValue(const QString name, uint16_t value) = 0;
    virtual uint32_t readValue(const QString name, uint32_t value) = 0;
    virtual uint64_t readValue(const QString name, uint64_t value) = 0;
    virtual float readValue(const QString name, float value) = 0;
    virtual double readValue(const QString name, double value) = 0;
    virtual bool readValue(const QString name, bool value) = 0;

    virtual QVector<int8_t> readArray(const QString name, QVector<int8_t> value) = 0;
    virtual QVector<int16_t> readArray(const QString name, QVector<int16_t> value) = 0;
    virtual QVector<int32_t> readArray(const QString name, QVector<int32_t> value) = 0;
    virtual QVector<int64_t> readArray(const QString name, QVector<int64_t> value) = 0;
    virtual QVector<uint8_t> readArray(const QString name, QVector<uint8_t> value) = 0;
    virtual QVector<uint16_t> readArray(const QString name, QVector<uint16_t> value) = 0;
    virtual QVector<uint32_t> readArray(const QString name, QVector<uint32_t> value) = 0;
    virtual QVector<uint64_t> readArray(const QString name, QVector<uint64_t> value) = 0;
    virtual QVector<float> readArray(const QString name, QVector<float> value) = 0;
    virtual QVector<double> readArray(const QString name, QVector<double> value) = 0;

    virtual IntVec3_t readIntVec3(const QString name, IntVec3_t v) = 0;
    virtual FloatVec3_t readFloatVec3(const QString name, FloatVec3_t v) = 0;
    virtual FloatVec4_t readFloatVec4(const QString name, FloatVec4_t v) = 0;
    virtual FloatVec21_t readFloatVec21(const QString name, FloatVec21_t v) = 0;
    virtual Float2ndOrderPoly_t readFloat2ndOrderPoly(const QString name, Float2ndOrderPoly_t v) = 0;
    virtual Float3rdOrderPoly_t readFloat3rdOrderPoly(const QString name, Float3rdOrderPoly_t v) = 0;
    virtual Float4thOrderPoly_t readFloat4thOrderPoly(const QString name, Float4thOrderPoly_t v) = 0;
    virtual FileListInfo_t readFileListInfo(const QString name, FileListInfo_t v) = 0;
    virtual ComparisonInput_t readComparisonInput(const QString name, ComparisonInput_t v, int vectorPos) = 0;
    virtual ComparisonInputs readComparisonInputs(const QString name, ComparisonInputs v) = 0;
    virtual AxisAngleInput_t readAxisAngle(const QString name, AxisAngleInput_t v, int vectorPos) = 0;
    virtual QVector<AxisAngleInput_t> readAxisAngles(const QString name, QVector<AxisAngleInput_t> v) = 0;
    virtual QSet<QString> readArraySelections(const QString name, QSet<QString> v) = 0;

    virtual DataContainerArrayProxy readDataContainerArrayProxy(const QString& name, DataContainerArrayProxy v) = 0;

    virtual DataArrayPath readDataArrayPath(const QString& name, DataArrayPath v) = 0;
    virtual QVector<DataArrayPath> readDataArrayPathVector(const QString& name, QVector<DataArrayPath> v) = 0;

    virtual DynamicTableData readDynamicTableData(const QString& name, DynamicTableData v) = 0;

  protected:
    AbstractFilterParametersReader();

  private:
    AbstractFilterParametersReader(const AbstractFilterParametersReader&); // Copy Constructor Not Implemented
    void operator=(const AbstractFilterParametersReader&); // Operator '=' Not Implemented
};

#endif /* _AbstractFilterParametersReader_H_ */

