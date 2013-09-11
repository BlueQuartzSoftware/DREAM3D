/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _AbstractFilterParametersWriter_H_
#define _AbstractFilterParametersWriter_H_

#include <string>
#include <set>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/FilterParameter.h"


class AbstractFilter;


/**
 * @class AbstractFilterParametersWriter AbstractFilterParametersWriter.h DREAM3DLib/Common/AbstractFilterParametersWriter.h
 * @brief This class writes the various parameters of a filter to an output file
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 17, 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT AbstractFilterParametersWriter
{
  public:
 //   DREAM3D_SHARED_POINTERS(AbstractFilterParametersWriter)
  //  DREAM3D_STATIC_NEW_MACRO(AbstractFilterParametersWriter)
    DREAM3D_TYPE_MACRO(AbstractFilterParametersWriter)


    virtual ~AbstractFilterParametersWriter();

    virtual int openFilterGroup(AbstractFilter *filter, int index) = 0;
    virtual int closeFilterGroup() = 0;

    virtual int writeValue(const QString name, const QString value) = 0;
    virtual int writeValue(const QString name, const QVector<QString> value) = 0;

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

    virtual int writeValue(const QString name, IntVec3Widget_t v) = 0;
    virtual int writeValue(const QString name, FloatVec3Widget_t v) = 0;
    virtual int writeValue(const QString name, ComparisonInput_t v, int vectorPos) = 0;
    virtual int writeValue(const QString name, QVector<ComparisonInput_t> v) = 0;
    virtual int writeValue(const QString name, AxisAngleInput_t v, int vectorPos) = 0;
    virtual int writeValue(const QString name, QVector<AxisAngleInput_t> v) = 0;
    virtual int writeValue(const QString name, QSet<QString> v) = 0;



  protected:
    AbstractFilterParametersWriter();

  private:
    AbstractFilterParametersWriter(const AbstractFilterParametersWriter&); // Copy Constructor Not Implemented
    void operator=(const AbstractFilterParametersWriter&); // Operator '=' Not Implemented
};

#endif /* _AbstractFilterParametersWriter_H_ */
