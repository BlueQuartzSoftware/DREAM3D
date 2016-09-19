/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "EMMPMFilterParameter.h"

#include "EMMPM/EMMPMFilters/MultiEmmpmFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMFilterParameter::EMMPMFilterParameter() :
  FilterParameter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMFilterParameter::~EMMPMFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMFilterParameter::Pointer EMMPMFilterParameter::New(const QString& humanLabel, const QString& propertyName,
    const QString& defaultValue, Category category, EMMPMFilter* filter, int groupIndex)
{
  EMMPMFilterParameter::Pointer ptr = EMMPMFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setDefaultValue(defaultValue);
  ptr->setCategory(category);
  ptr->setGroupIndex(groupIndex);
  ptr->setFilter(filter);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMFilterParameter::getWidgetType()
{
  return QString("EMMPMWidget");
}

// -----------------------------------------------------------------------------
// THIS IS A SPECIAL CASE AND IS NOT STANDARD.  DO NOT COPY THIS CODE.
// -----------------------------------------------------------------------------
void EMMPMFilterParameter::readJson(const QJsonObject &json)
{
  m_Filter->setNumClasses(json["NumClasses"].toInt());
  m_Filter->setExchangeEnergy(static_cast<float>(json["ExchangeEnergy"].toDouble()));
  m_Filter->setHistogramLoops(json["HistogramLoops"].toInt());
  m_Filter->setSegmentationLoops(json["SegmentationLoops"].toInt());
  m_Filter->setUseSimulatedAnnealing(json["UseSimulatedAnnealing"].toBool());
  m_Filter->setEmmpmInitType(static_cast<EMMPM_InitializationType>(json["EmmpmInitType"].toInt()));

  DynamicTableData tableData;
  QJsonObject tableDataObj = json["EMMPMTableData"].toObject();
  tableData.readJson(tableDataObj);
  m_Filter->setEMMPMTableData(tableData);

  MultiEmmpmFilter* multiFilter = dynamic_cast<MultiEmmpmFilter*>(m_Filter);
  if (multiFilter != nullptr)
  {
    {
      QJsonArray arrayObj = json["InputDataArrayVector"].toArray();
      QVector<DataArrayPath> pathVector;
      for (int i=0; i<arrayObj.size(); i++)
      {
        QJsonObject obj = arrayObj.at(i).toObject();
        DataArrayPath dap;
        dap.readJson(obj);
        pathVector.push_back(dap);
      }

      multiFilter->setInputDataArrayVector(pathVector);
    }


    multiFilter->setOutputAttributeMatrixName(json["OutputAttributeMatrixName"].toString());
    multiFilter->setOutputArrayPrefix(json["OutputArrayPrefix"].toString());
    multiFilter->setUsePreviousMuSigma(json["UsePreviousMuSigma"].toBool());
  }
}

// -----------------------------------------------------------------------------
// THIS IS A SPECIAL CASE AND IS NOT STANDARD.  DO NOT COPY THIS CODE.
// -----------------------------------------------------------------------------
void EMMPMFilterParameter::writeJson(QJsonObject &json)
{
  json["NumClasses"] = m_Filter->getNumClasses();
  json["ExchangeEnergy"] = static_cast<double>(m_Filter->getExchangeEnergy());
  json["HistogramLoops"] = m_Filter->getHistogramLoops();
  json["SegmentationLoops"] = m_Filter->getSegmentationLoops();
  json["UseSimulatedAnnealing"] = m_Filter->getUseSimulatedAnnealing();
  json["EmmpmInitType"] = static_cast<int>(m_Filter->getEmmpmInitType());

  DynamicTableData tableData = m_Filter->getEMMPMTableData();
  QJsonObject tableDataObj;
  tableData.writeJson(tableDataObj);
  json["EMMPMTableData"] = tableDataObj;

  MultiEmmpmFilter* multiFilter = dynamic_cast<MultiEmmpmFilter*>(m_Filter);
  if (multiFilter != nullptr)
  {
    {
      QVector<DataArrayPath> pathVector = multiFilter->getInputDataArrayVector();
      QJsonArray arrayObj;

      for (int i=0; i<pathVector.size(); i++)
      {
        DataArrayPath dap = pathVector[i];
        QJsonObject obj;
        dap.writeJson(obj);
        arrayObj.push_back(obj);
      }
      json["InputDataArrayVector"] = arrayObj;
    }

    json["OutputAttributeMatrixName"] = multiFilter->getOutputAttributeMatrixName();
    json["OutputArrayPrefix"] = multiFilter->getOutputArrayPrefix();
    json["UsePreviousMuSigma"] = multiFilter->getUsePreviousMuSigma();
  }
}

