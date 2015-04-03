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
	tableData = data;

	QStringList rHeaders, cHeaders;

	for (int i = 0; i < nRows; i++)
	{
		rHeaders << QString::number(i);
	}

	for (int i = 0; i < nCols; i++)
	{
		cHeaders << QString::number(i);
	}

	rowHeaders = rHeaders;
	colHeaders = cHeaders;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::DynamicTableData(int nRows, int nCols, QStringList rHeaders, QStringList cHeaders)
{
	std::vector<std::vector<double> > data(nRows, std::vector<double>(nCols, 0));
	tableData = data;

	rowHeaders = rHeaders;
	colHeaders = cHeaders;

	// Adjust dimensions if they are not all the same
	checkAndAdjustDimensions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::DynamicTableData(std::vector<std::vector<double> > data, QStringList rHeaders, QStringList cHeaders)
{
	tableData = data;
	rowHeaders = rHeaders;
	colHeaders = cHeaders;

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
	if (tableData.size() > 0 || rowHeaders.size() > 0 || colHeaders.size() > 0)
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
	QSize dataSize(tableData.size(), 0);
	QSize headerSize(rowHeaders.size(), colHeaders.size());
	
	if (tableData.size() > 0)
	{
		dataSize.setHeight(tableData[0].size());
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
		if (rowHeaders.size() > nRows)
		{
			while (rowHeaders.size() > nRows)
			{
				rowHeaders.pop_back();
			}
		}
		// If row header dimension is less than default row dimension, add blank headers
		else
		{
			int count = rowHeaders.size();
			while (rowHeaders.size() < nRows)
			{
				rowHeaders.push_back(QString::number(count));
				count++;
			}
		}
		// If column header dimension is greater than default column dimension, remove the extra headers
		if (colHeaders.size() > nCols)
		{
			while (colHeaders.size() > nCols)
			{
				colHeaders.pop_back();
			}
		}
		// If column header dimension is less than default column dimension, add blank headers
		else
		{
			int count = colHeaders.size();
			while (colHeaders.size() < nCols)
			{
				colHeaders.push_back(QString::number(count));
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

	for (int row = 0; row < tableData.size(); row++)
	{
		for (int col = 0; col < tableData[row].size(); col++)
		{
			ss << tableData[row][col] << delimiter;
		}
	}
	str.chop(1);	// Get rid of the last, unnecessary delimiter

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
		QString valueStr = dataStr.mid(start, tokenIndex-start);
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

	for (int i = 0; i < rowHeaders.size(); i++)
	{
		ss << rowHeaders[i];
		ss << delimiter;
	}
	str.chop(1);	// Get rid of the last, unnecessary delimiter

	return str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DynamicTableData::serializeColumnHeaders(char delimiter) const
{
	QString str = "";
	QTextStream ss(&str);

	for (int i = 0; i < colHeaders.size(); i++)
	{
		ss << colHeaders[i];
		ss << delimiter;
	}
	str.chop(1);	// Get rid of the last, unnecessary delimiter

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

	QVector<double> flat(numRows*numCols);
	for (int row = 0; row < numRows; row++)
	{
		for (int col = 0; col < numCols; col++)
		{
			flat[row*numCols + col] = tableData[row][col];
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

	if (orig.size() != nRows*nCols)
	{
		// Something went wrong
		return expand;
	}

	for (int row = 0; row < nRows; row++)
	{
		for (int col = 0; col < nCols; col++)
		{
			expand[row][col] = orig[row*nCols + col];
		}
	}

	return expand;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::vector<double> > DynamicTableData::getTableData() const
{
	return tableData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::setTableData(std::vector<std::vector<double> > data)
{
	tableData = data;

	// Adjust dimensions
	checkAndAdjustDimensions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DynamicTableData::getRowHeaders() const
{
	return rowHeaders;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::setRowHeaders(QStringList rHeaders)
{
	rowHeaders = rHeaders;

	// Adjust dimensions
	checkAndAdjustDimensions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DynamicTableData::getColHeaders() const
{
	return colHeaders;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::setColHeaders(QStringList cHeaders)
{
	colHeaders = cHeaders;

	// Adjust dimensions
	checkAndAdjustDimensions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DynamicTableData::getNumRows() const
{
	return tableData.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DynamicTableData::getNumCols() const
{
	if (tableData.size() > 0)
	{
		return tableData[0].size();
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
	tableData = rhs.tableData;
	rowHeaders = rhs.rowHeaders;
	colHeaders = rhs.colHeaders;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::operator=(const DynamicTableData& rhs)
{
	tableData = rhs.tableData;
	rowHeaders = rhs.rowHeaders;
	colHeaders = rhs.colHeaders;
}






