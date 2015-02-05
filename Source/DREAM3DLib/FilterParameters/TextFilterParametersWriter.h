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
#ifndef _TEXTFilterParametersWRITER_H_
#define _TEXTFilterParametersWRITER_H_

#include <QtCore/QString>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/FilterParameters/FilterParameter.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"


/**
 * @class
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 17, 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT TextFilterParametersWriter : public AbstractFilterParametersWriter
{
  public:
    DREAM3D_SHARED_POINTERS(TextFilterParametersWriter)
    DREAM3D_STATIC_NEW_MACRO(TextFilterParametersWriter)
    DREAM3D_TYPE_MACRO_SUPER(TextFilterParametersWriter, AbstractFilterParametersWriter)


    virtual ~TextFilterParametersWriter();

    virtual int openFilterGroup(AbstractFilter* filter, int index);
    virtual int closeFilterGroup();
    virtual int writeValue(const QString name, const QString value);
    virtual int writeValue(const QString name, const QVector<QString> value);

    virtual int writeValue(const QString name, int8_t value);
    virtual int writeValue(const QString name, int16_t value);
    virtual int writeValue(const QString name, int32_t value);
    virtual int writeValue(const QString name, int64_t value);
    virtual int writeValue(const QString name, uint8_t value);
    virtual int writeValue(const QString name, uint16_t value);
    virtual int writeValue(const QString name, uint32_t value);
    virtual int writeValue(const QString name, uint64_t value);
    virtual int writeValue(const QString name, float value);
    virtual int writeValue(const QString name, double value);

    virtual int writeValue(const QString name, QVector<int8_t> value);
    virtual int writeValue(const QString name, QVector<int16_t> value);
    virtual int writeValue(const QString name, QVector<int32_t> value);
    virtual int writeValue(const QString name, QVector<int64_t> value);
    virtual int writeValue(const QString name, QVector<uint8_t> value);
    virtual int writeValue(const QString name, QVector<uint16_t> value);
    virtual int writeValue(const QString name, QVector<uint32_t> value);
    virtual int writeValue(const QString name, QVector<uint64_t> value);
    virtual int writeValue(const QString name, QVector<float> value);
    virtual int writeValue(const QString name, QVector<double> value);

    virtual int writeValue(const QString name, IntVec3_t v);
    virtual int writeValue(const QString name, FloatVec3_t v);
    virtual int writeValue(const QString name, FloatVec4_t v);
    virtual int writeValue(const QString name, FloatVec21_t v);
    virtual int writeValue(const QString name, Float2ndOrderPoly_t v);
    virtual int writeValue(const QString name, Float3rdOrderPoly_t v);
    virtual int writeValue(const QString name, Float4thOrderPoly_t v);
    virtual int writeValue(const QString name, FileListInfo_t v);

    virtual int writeValue(const QString name, ComparisonInput_t v);
    virtual int writeValue(const QString name, ComparisonInputs v);

    virtual int writeValue(const QString name, QVector<AxisAngleInput_t> v);
    virtual int writeValue(const QString name, AxisAngleInput_t v, int vectorPos);

    virtual int writeArraySelections(const QString name, QSet<QString> v);
    virtual int writeValue(const QString name, DataContainerArrayProxy& v);
    virtual int writeValue(const QString name, AxisAngleInput_t v);

    virtual int writeValue(const QString name, const DataArrayPath& v);

  protected:
    TextFilterParametersWriter();

  private:
    TextFilterParametersWriter(const TextFilterParametersWriter&); // Copy Constructor Not Implemented
    void operator=(const TextFilterParametersWriter&); // Operator '=' Not Implemented
};

#endif /* TEXTFilterParametersWRITER_H_ */

