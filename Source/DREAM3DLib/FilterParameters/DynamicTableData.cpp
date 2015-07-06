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
*                           FA8650-10-D-5210
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DynamicTableData.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::DynamicTableData()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::DynamicTableData(int nRows, int nCols)
{
  std::vector<std::vector<double> > data(nRows, std::vector<double>(nCols, 0));
  m_TableData = data;

  QStringList rHeaders, cHeaders;

  for (int i = 0; i < nRows; i++)
  {
    rHeaders << QString::number(i);
  }

  for (int i = 0; i < nCols; i++)
  {
    cHeaders << QString::number(i);
  }

  m_RowHeaders = rHeaders;
  m_ColHeaders = cHeaders;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::DynamicTableData(int nRows, int nCols, QStringList rHeaders, QStringList cHeaders)
{
  std::vector<std::vector<double> > data(nRows, std::vector<double>(nCols, 0));
  m_TableData = data;

  m_RowHeaders = rHeaders;
  m_ColHeaders = cHeaders;

  // Adjust dimensions if they are not all the same
  checkAndAdjustDimensions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::DynamicTableData(std::vector<std::vector<double> > data, QStringList rHeaders, QStringList cHeaders)
{
  m_TableData = data;
  m_RowHeaders = rHeaders;
  m_ColHeaders = cHeaders;

  // Adjust dimensions if they are not all the same
  checkAndAdjustDimensions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::~DynamicTableData()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DynamicTableData::isEmpty()
{
  if (m_TableData.size() > 0 || m_RowHeaders.size() > 0 || m_ColHeaders.size() > 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::checkAndAdjustDimensions()
{
  QSize dataSize(m_TableData.size(), 0);
  QSize headerSize(m_RowHeaders.size(), m_ColHeaders.size());

  if (m_TableData.size() > 0)
  {
    dataSize.setHeight(m_TableData[0].size());
  }

  if (dataSize == headerSize)
  {
    return;
  }
  else
  {
    /* The header dimensions do not equal the data dimensions.
       The data dimensions will be used and will overwrite the current header dimensions.
       This may result in data loss.
    */
    int nRows = dataSize.width();
    int nCols = dataSize.height();

    // If row header dimension is greater than default row dimension, remove the extra headers
    if (m_RowHeaders.size() > nRows)
    {
      while (m_RowHeaders.size() > nRows)
      {
        m_RowHeaders.pop_back();
      }
    }
    // If row header dimension is less than default row dimension, add blank headers
    else
    {
      int count = m_RowHeaders.size();
      while (m_RowHeaders.size() < nRows)
      {
        m_RowHeaders.push_back(QString::number(count));
        count++;
      }
    }
    // If column header dimension is greater than default column dimension, remove the extra headers
    if (m_ColHeaders.size() > nCols)
    {
      while (m_ColHeaders.size() > nCols)
      {
        m_ColHeaders.pop_back();
      }
    }
    // If column header dimension is less than default column dimension, add blank headers
    else
    {
      int count = m_ColHeaders.size();
      while (m_ColHeaders.size() < nCols)
      {
        m_ColHeaders.push_back(QString::number(count));
        count++;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DynamicTableData::serializeData(char delimiter) const
{
  QString str;
  QTextStream ss(&str);

  for (int row = 0; row < m_TableData.size(); row++)
  {
    for (int col = 0; col < m_TableData[row].size(); col++)
    {
      ss << m_TableData[row][col] << delimiter;
    }
  }
  str.chop(1);  // Get rid of the last, unnecessary delimiter

  return str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::vector<double> > DynamicTableData::DeserializeData(QString dataStr, int nRows, int nCols, char delimiter)
{
  std::vector<std::vector<double> > data(nRows, std::vector<double>(nCols));
  int row = 0, col = 0;

  if (dataStr.isEmpty())
  {
    return data;
  }

  int start = 0;
  int tokenIndex = 0;

  while (tokenIndex >= 0)
  {
    tokenIndex = dataStr.indexOf(delimiter, start);
    QString valueStr = dataStr.mid(start, tokenIndex - start);
    double value = valueStr.toDouble();
    data[row][col] = value;
    start = tokenIndex + 1;

    col++;
    if (col == nCols)
    {
      row++;
      col = 0;
    }
  }

  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DynamicTableData::serializeRowHeaders(char delimiter) const
{
  QString str = "";
  QTextStream ss(&str);

  for (int i = 0; i < m_RowHeaders.size(); i++)
  {
    ss << m_RowHeaders[i];
    ss << delimiter;
  }
  str.chop(1);  // Get rid of the last, unnecessary delimiter

  return str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DynamicTableData::serializeColumnHeaders(char delimiter) const
{
  QString str = "";
  QTextStream ss(&str);

  for (int i = 0; i < m_ColHeaders.size(); i++)
  {
    ss << m_ColHeaders[i];
    ss << delimiter;
  }
  str.chop(1);  // Get rid of the last, unnecessary delimiter

  return str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DynamicTableData::DeserializeHeaders(QString headersStr, char delimiter)
{
  QStringList headers;

  if (headersStr.isEmpty())
  {
    return headers;
  }

  int start = 0;
  int tokenIndex = 0;

  while (tokenIndex >= 0)
  {
    tokenIndex = headersStr.indexOf(delimiter, start);
    QString header = headersStr.mid(start, tokenIndex);
    headers.push_back(header);
    start = tokenIndex + 1;
  }

  return headers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<double> DynamicTableData::flattenData() const
{
  int numRows = getNumRows();
  int numCols = getNumCols();

  QVector<double> flat(numRows * numCols);
  for (int row = 0; row < numRows; row++)
  {
    for (int col = 0; col < numCols; col++)
    {
      flat[row * numCols + col] = m_TableData[row][col];
    }
  }

  return flat;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::vector<double> > DynamicTableData::ExpandData(std::vector<double> orig, int nRows, int nCols)
{
  std::vector<std::vector<double> > expand(nRows, std::vector<double>(nCols));

  if (orig.size() != nRows * nCols)
  {
    // Something went wrong
    return expand;
  }

  for (int row = 0; row < nRows; row++)
  {
    for (int col = 0; col < nCols; col++)
    {
      expand[row][col] = orig[row * nCols + col];
    }
  }

  return expand;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::writeJson(QJsonObject& json) const
{
  json["Dynamic Table Data"] = writeData();

  QJsonArray rHeaders;
  foreach(QString header, m_RowHeaders)
  {
    rHeaders.push_back(header);
  }
  json["Row Headers"] = rHeaders;

  QJsonArray cHeaders;
  foreach(QString header, m_ColHeaders)
  {
    cHeaders.push_back(header);
  }
  json["Column Headers"] = cHeaders;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DynamicTableData::readJson(QJsonObject& json)
{
  if (json["Dynamic Table Data"].isObject())
  {
    m_TableData = readData(json["Dynamic Table Data"].toObject());

    QJsonArray rHeaders = json["Row Headers"].toArray();
    foreach(QJsonValue val, rHeaders)
    {
      if (val.isString())
      {
        m_RowHeaders.push_back(val.toString());
      }
    }

    QJsonArray cHeaders = json["Column Headers"].toArray();
    foreach(QJsonValue val, cHeaders)
    {
      if (val.isString())
      {
        m_ColHeaders.push_back(val.toString());
      }
    }
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject DynamicTableData::writeData() const
{
  QJsonObject obj;

  QJsonArray rows;
  foreach(std::vector<double> vector, m_TableData)
  {
    QJsonArray cols;
    foreach(double val, vector)
    {
      cols.push_back(val);
    }
    rows.push_back(cols);
  }

  obj["Table Data"] = rows;

  return obj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::vector<double> > DynamicTableData::readData(QJsonObject object)
{
  std::vector<std::vector<double> > data;
  if (object["Table Data"].isArray())
  {
    QJsonArray rowArray = object["Table Data"].toArray();
    data.resize(rowArray.size());

    for (int row = 0; row < rowArray.size(); row++)
    {
      QJsonValue rowObj = rowArray.at(row);
      if (rowObj.isArray())
      {
        QJsonArray colArray = rowObj.toArray();
        data[row].resize(colArray.size());

        for (int col = 0; col < colArray.size(); col++)
        {
          QJsonValue colObj = colArray.at(col);

          if (colObj.isDouble())
          {
            data[row][col] = colObj.toDouble();
          }
        }
      }
    }
    return data;
  }
  return std::vector<std::vector<double> >();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::vector<double> > DynamicTableData::getTableData()
{
  return m_TableData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::setTableData(std::vector<std::vector<double> > data)
{
  m_TableData = data;

  // Adjust dimensions
  checkAndAdjustDimensions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DynamicTableData::getRowHeaders() const
{
  return m_RowHeaders;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::setRowHeaders(const QStringList& rHeaders)
{
  m_RowHeaders = rHeaders;

  // Adjust dimensions
  checkAndAdjustDimensions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DynamicTableData::getColHeaders() const
{
  return m_ColHeaders;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::setColHeaders(const QStringList& cHeaders)
{
  m_ColHeaders = cHeaders;

  // Adjust dimensions
  checkAndAdjustDimensions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DynamicTableData::getNumRows() const
{
  return m_TableData.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DynamicTableData::getNumCols() const
{
  if (m_TableData.size() > 0)
  {
    return m_TableData[0].size();
  }
  else
  {
    return 0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::DynamicTableData(const DynamicTableData& rhs)
{
  m_TableData = rhs.m_TableData;
  m_RowHeaders = rhs.m_RowHeaders;
  m_ColHeaders = rhs.m_ColHeaders;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::operator=(const DynamicTableData& rhs)
{
  m_TableData = rhs.m_TableData;
  m_RowHeaders = rhs.m_RowHeaders;
  m_ColHeaders = rhs.m_ColHeaders;
}






