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
#ifndef _DynamicTableData_H_
#define _DynamicTableData_H_


#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QAbstractTableModel>

#include <QtWidgets/QWidget>

#include "SIMPLib/Common/AbstractFilter.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"

/**
* @brief
* @author
* @version
*/
class SIMPLib_EXPORT DynamicTableData : public QObject
{
    Q_OBJECT

  public:
    DynamicTableData();
    DynamicTableData(std::vector<std::vector<double> > data, int nRows, int nCols, QStringList rHeaders = QStringList(), QStringList cHeaders = QStringList());
    DynamicTableData(QAbstractItemModel* model);

    virtual ~DynamicTableData();

    std::vector<std::vector<double> > getTableData();
    void setTableData(std::vector<std::vector<double> > data);

    QStringList getRowHeaders();
    void setRowHeaders(QStringList rHeaders);

    QStringList getColHeaders();
    void setColHeaders(QStringList cHeaders);

    int getNumRows();
    void setNumRows(int nRows);

    int getNumCols();
    void setNumCols(int nCols);

    static QAbstractItemModel* convertToModel(DynamicTableData data);

    DynamicTableData(const DynamicTableData& rhs);
    void operator=(const DynamicTableData& rhs);

  private:
    std::vector<std::vector<double> > tableData;
    QStringList rowHeaders;
    QStringList colHeaders;
    int numRows;
    int numCols;
};

Q_DECLARE_METATYPE(DynamicTableData)

#endif /* _DynamicTableWidget_H_ */