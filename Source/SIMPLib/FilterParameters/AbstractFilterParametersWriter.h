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


#ifndef _AbstractFilterParametersWriter_H_
#define _AbstractFilterParametersWriter_H_

#include <set>

#include <QtCore/QString>

// Needed for AxisAngle_t
#include "EbsdLib/EbsdConstants.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/ComparisonInputs.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/SecondOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/ThirdOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/FourthOrderPolynomialFilterParameter.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"

class AbstractFilter;

#define WRITE_ARRAY_SELECTION_PARAMETER(Name)\
  writer->writeArraySelections( #Name, getSelected##Name##Arrays() );



/**
 * @class AbstractFilterParametersWriter AbstractFilterParametersWriter.h DREAM3DLib/Common/AbstractFilterParametersWriter.h
 * @brief This class writes the various parameters of a filter to an output file
 *
 * @date Jan 17, 2012
 * @version 1.0
 */
class SIMPLib_EXPORT AbstractFilterParametersWriter
{
  public:
//   SIMPL_SHARED_POINTERS(AbstractFilterParametersWriter)
    //  SIMPL_STATIC_NEW_MACRO(AbstractFilterParametersWriter)
    SIMPL_TYPE_MACRO(AbstractFilterParametersWriter)


    virtual ~AbstractFilterParametersWriter();

    virtual int openFilterGroup(AbstractFilter* filter, int index) = 0;
    virtual int closeFilterGroup() = 0;


    virtual int writeValue(const QString name, const QString value) = 0;
    virtual int writeValue(const QString name, const QVector<QString> value) = 0;
    virtual int writeValue(const QString name, const QStringList value) = 0;

    virtual int writeValue(const QString name, int8_t value) = 0;
    virtual int writeValue(const QString name, int16_t value) = 0;
    virtual int writeValue(const QString name, int32_t value) = 0;
    virtual int writeValue(const QString name, int64_t value) = 0;
    virtual int writeValue(const QString name, uint8_t value) = 0;
    virtual int writeValue(const QString name, uint16_t value) = 0;
    virtual int writeValue(const QString name, uint32_t value) = 0;
    virtual int writeValue(const QString name, uint64_t value) = 0;
    virtual int writeValue(const QString name, float value) = 0;
    virtual int writeValue(const QString name, double value) = 0;

    virtual int writeValue(const QString name, QVector<int8_t> value) = 0;
    virtual int writeValue(const QString name, QVector<int16_t> value) = 0;
    virtual int writeValue(const QString name, QVector<int32_t> value) = 0;
    virtual int writeValue(const QString name, QVector<int64_t> value) = 0;
    virtual int writeValue(const QString name, QVector<uint8_t> value) = 0;
    virtual int writeValue(const QString name, QVector<uint16_t> value) = 0;
    virtual int writeValue(const QString name, QVector<uint32_t> value) = 0;
    virtual int writeValue(const QString name, QVector<uint64_t> value) = 0;
    virtual int writeValue(const QString name, QVector<float> value) = 0;
    virtual int writeValue(const QString name, QVector<double> value) = 0;

    virtual int writeValue(const QString name, IntVec3_t v) = 0;
    virtual int writeValue(const QString name, FloatVec3_t v) = 0;
    virtual int writeValue(const QString name, Float2ndOrderPoly_t v) = 0;
    virtual int writeValue(const QString name, Float3rdOrderPoly_t v) = 0;
    virtual int writeValue(const QString name, Float4thOrderPoly_t v) = 0;
    virtual int writeValue(const QString name, FileListInfo_t v) = 0;

    virtual int writeValue(const QString name, ComparisonInput_t v) = 0;
    virtual int writeValue(const QString name, ComparisonInputs v) = 0;

    virtual int writeValue(const QString name, QVector<AxisAngleInput_t> v) = 0;

    virtual int writeArraySelections(const QString name, QSet<QString> v) = 0;

    virtual int writeValue(const QString name, const DataContainerArrayProxy& v) = 0;

    virtual int writeValue(const QString name, AxisAngleInput_t v) = 0;

    virtual int writeValue(const QString name, const DataArrayPath& path) = 0;

    virtual int writeValue(const QString name, const QVector<DataArrayPath>& path) = 0;

    virtual int writeValue(const QString name, const DynamicTableData& v) = 0;

  protected:
    AbstractFilterParametersWriter();


    virtual int writeValue(const QString name, AxisAngleInput_t v, int vectorPos) = 0;



  private:
    AbstractFilterParametersWriter(const AbstractFilterParametersWriter&); // Copy Constructor Not Implemented
    void operator=(const AbstractFilterParametersWriter&); // Operator '=' Not Implemented
};

#endif /* _AbstractFilterParametersWriter_H_ */

