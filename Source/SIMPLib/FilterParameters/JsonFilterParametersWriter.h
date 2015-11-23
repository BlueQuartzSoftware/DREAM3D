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

#ifndef _JsonFilterParametersWriter_H_
#define _JsonFilterParametersWriter_H_

#include <QtCore/QString>


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"


/**
 * @class
 * @brief
 *
 * @date Jan 17, 2012
 * @version 1.0
 */
class SIMPLib_EXPORT JsonFilterParametersWriter : public AbstractFilterParametersWriter
{
  public:
    SIMPL_SHARED_POINTERS(JsonFilterParametersWriter)
    SIMPL_STATIC_NEW_MACRO(JsonFilterParametersWriter)
    SIMPL_TYPE_MACRO_SUPER(JsonFilterParametersWriter, AbstractFilterParametersWriter)

    SIMPL_INSTANCE_PROPERTY(QString, FileName)
    SIMPL_INSTANCE_PROPERTY(QString, PipelineName)

    JsonFilterParametersWriter(QString& fileName, QString& pipelineName, int& numFilters);

    virtual ~JsonFilterParametersWriter();

    /**
    * @brief WritePipelineToFile This function will write a pipeline to a
    * JSON file. The file path passed in <b>WILL BE OVER WRITTEN</b> by this
    * function <b>WITHOUT WARNING</b>
    * @param pipeline The pipeline to be written
    * @param filePath The file path to write
    * @param name The name of the pipeline (Typically the name of the file)
    * @param obs Any observer that we can pass error/warning messages back to in case something goes wrong.
    * @return
    */
    static int WritePipelineToFile(FilterPipeline::Pointer pipeline, QString filePath, QString name, IObserver* obs = NULL);

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

    virtual int writeValue(const QString name, Float2ndOrderPoly_t v);
    virtual int writeValue(const QString name, Float3rdOrderPoly_t v);
    virtual int writeValue(const QString name, Float4thOrderPoly_t v);
    virtual int writeValue(const QString name, FileListInfo_t v);

    virtual int writeValue(const QString name, ComparisonInput_t v);
    virtual int writeValue(const QString name, ComparisonInputs v);

    virtual int writeValue(const QString name, QVector<AxisAngleInput_t> v);
    virtual int writeValue(const QString name, AxisAngleInput_t v, int vectorPos);

    virtual int writeArraySelections(const QString name, QSet<QString> v);
    virtual int writeValue(const QString name, const DataContainerArrayProxy& v);
    virtual int writeValue(const QString name, AxisAngleInput_t v);

    virtual int writeValue(const QString name, const DataArrayPath& v);
    virtual int writeValue(const QString name, const QVector<DataArrayPath>& path);

    virtual int writeValue(const QString name, const DynamicTableData& v);

  protected:
    JsonFilterParametersWriter();

  private:
    QJsonObject m_Root;
    QJsonObject m_CurrentFilterIndex;
    int         m_CurrentIndex;

    JsonFilterParametersWriter(const JsonFilterParametersWriter&); // Copy Constructor Not Implemented
    void operator=(const JsonFilterParametersWriter&); // Operator '=' Not Implemented
};

#endif /* JsonFilterParametersWriter_H_ */

