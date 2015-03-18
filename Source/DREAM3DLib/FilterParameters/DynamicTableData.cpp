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
	if (nRows != data.size())
	{
		qDebug() << "The default row count does not equal the row dimension of the default data.  The default row count will be used and will overwrite the default data dimension.  This may result in data loss.";
	}

	if (rHeaders.size() != data.size())
	{
		qDebug() << "The number of default row headers does not equal the row dimension of the default data.  The default row count will be used and will overwrite the default data dimension.  This may result in data loss.";
	}

	if (data.size() > 0)
	{
		if (nCols != data[0].size())
		{
			qDebug() << "The default column count does not equal the column dimension of the default data.  The default column count will be used and will overwrite the default data dimension.  This may result in data loss.";
		}

		if (cHeaders.size() != data[0].size())
		{
			qDebug() << "The number of default column headers does not equal the column dimension of the default data.  The default column count will be used and will overwrite the default data dimension.  This may result in data loss.";
		}
	}

	// Resize the data with the correct dimensions
	tableData.resize(nRows);
	numRows = nRows;

	for (int i = 0; i < tableData.size(); i++)
	{
		tableData[i].resize(nCols);
	}
	numCols = nCols;

	// Store data in the object
	for (int row = 0; row < nRows; row++)
	{
		for (int col = 0; col < nCols; col++)
		{
			tableData[row][col] = data[row][col];
		}
	}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableData::DynamicTableData(QAbstractItemModel* model)
{
	QAbstractTableModel* tableModel = qobject_cast<QAbstractTableModel*>(model);

	if (NULL == tableModel)
	{
		qDebug() << "Could not convert QAbstractItemModel* to QAbstractTableModel*.";
		return;
	}

	// Resize the data with the correct dimensions
	tableData.resize(tableModel->rowCount());
	numRows = tableModel->rowCount();

	for (int i = 0; i < tableData.size(); i++)
	{
		tableData[i].resize(tableModel->columnCount());
	}
	numCols = tableModel->columnCount();

	// Store data from the model in the object
	for (int row = 0; row < tableModel->rowCount(); row++)
	{
		// Store row header
		rowHeaders.push_back(tableModel->headerData(row, Qt::Vertical, Qt::DisplayRole).toString());

		for (int col = 0; col < tableModel->columnCount(); col++)
		{
			bool ok = false;
			tableData[row][col] = tableModel->index(row, col).data(Qt::DisplayRole).toDouble(&ok);

			if (ok == false)
			{
				qDebug() << "Could not set the model data into the DynamicTableData object.";
				return;
			}
		}
	}

	// Store column headers
	for (int col = 0; col < tableModel->columnCount(); col++)
	{
		colHeaders.push_back(tableModel->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString());
	}
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






