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
DynamicTableData::DynamicTableData(std::vector<std::vector<double> > data, int nRows, int nCols, QStringList rHeaders, QStringList cHeaders)
{
	// Adjust dimensions if they are not all the same
	checkAndAdjustDimensions(data, nRows, nCols, rHeaders, cHeaders);

	numRows = nRows;
	numCols = nCols;
	tableData = data;
	rowHeaders = rHeaders;
	colHeaders = cHeaders;
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
void DynamicTableData::checkAndAdjustDimensions(std::vector<std::vector<double> > &data, int nRows, int nCols, QStringList &rHeaders, QStringList &cHeaders)
{
	QSize dataSize(data.size(), 0);
	QSize defaultSize(nRows, nCols);
	QSize headerSize(rHeaders.size(), cHeaders.size());
	
	if (data.size() > 0)
	{
		dataSize.setHeight(data[0].size());
	}

	if (dataSize == defaultSize && defaultSize == headerSize)
	{
		return;
	}
	if (dataSize != defaultSize)
	{
		qDebug() << "The data dimensions do not equal the default dimensions.  The default dimensions will be used and will overwrite the current data dimensions.  This may result in data loss or garbage values.";
		data.resize(nRows);		// Resize rows
		for (int i = 0; i < nRows; i++)
		{
			data[i].resize(nCols);		// Resize columns
		}
	}
	if (headerSize != defaultSize)
	{
		qDebug() << "The header dimensions do not equal the default dimensions.  The default dimensions will be used and will overwrite the current header dimensions.  This may result in data loss.";

		// If row header dimension is greater than default row dimension, remove the extra headers
		if (rHeaders.size() > nRows)
		{
			while (rHeaders.size() > nRows)
			{
				rHeaders.pop_back();
			}
		}
		// If row header dimension is less than default row dimension, add blank headers
		else
		{
			while (rHeaders.size() < nRows)
			{
				rHeaders.push_back("");
			}
		}
		// If column header dimension is greater than default column dimension, remove the extra headers
		if (cHeaders.size() > nCols)
		{
			while (cHeaders.size() > nCols)
			{
				cHeaders.pop_back();
			}
		}
		// If column header dimension is less than default column dimension, add blank headers
		else
		{
			while (cHeaders.size() < nCols)
			{
				cHeaders.push_back("");
			}
		}
	}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAbstractItemModel* DynamicTableData::convertToModel(DynamicTableData data)
{
	/*QAbstractTableModel* tableModel = new QAbstractTableModel();
	for (int row = 0; row < data.numRows; row++)
	{
		for (int col = 0; col < data.numCols; col++)
		{

		}
	}*/
	QAbstractItemModel* model;
	return model;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<std::vector<double> > DynamicTableData::getTableData()
{
	return tableData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::setTableData(std::vector<std::vector<double> > data)
{
	tableData = data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DynamicTableData::getRowHeaders()
{
	return rowHeaders;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::setRowHeaders(QStringList rHeaders)
{
	rowHeaders = rHeaders;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DynamicTableData::getColHeaders()
{
	return colHeaders;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::setColHeaders(QStringList cHeaders)
{
	colHeaders = cHeaders;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DynamicTableData::getNumRows()
{
	return numRows;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::setNumRows(int nRows)
{
	numRows = nRows;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DynamicTableData::getNumCols()
{
	return numCols;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::setNumCols(int nCols)
{
	numCols = nCols;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::DynamicTableData(const DynamicTableData& rhs)
{
	tableData = rhs.tableData;
	rowHeaders = rhs.rowHeaders;
	colHeaders = rhs.colHeaders;
	numRows = rhs.numRows;
	numCols = rhs.numCols;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DynamicTableData::operator=(const DynamicTableData& rhs)
{
	tableData = rhs.tableData;
	rowHeaders = rhs.rowHeaders;
	colHeaders = rhs.colHeaders;
	numRows = rhs.numRows;
	numCols = rhs.numCols;
}






