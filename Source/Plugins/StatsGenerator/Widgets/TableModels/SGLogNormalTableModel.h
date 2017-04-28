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


#ifndef _sglognormaltablemodel_h_
#define _sglognormaltablemodel_h_

#include <QtCore/QVector>
#include <QtCore/QVariant>
#include <QtGui/QColor>
#include "SGAbstractTableModel.h"

class QAbstractItemDelegate;

/**
 * @class SGLogNormalTableModel SGLogNormalTableModel.h StatsGenerator/SGLogNormalTableModel.h
 * @brief
 *
 * @date Dec 22, 2010
 * @version 1.0
 */
class SGLogNormalTableModel : public SGAbstractTableModel
{

    Q_OBJECT

  public:

    enum ColumnIndexes
    {
      BinNumber = 0,
      Average,
      StdDev,
      ColumnCount
    };

    SGLogNormalTableModel(QObject* parent = 0);
    virtual ~SGLogNormalTableModel();

    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    bool setData(const QModelIndex& index, const QVariant& value,
                 int role = Qt::EditRole);
    /**
     *
     * @param col
     * @param orientation
     * @param data
     * @param role
     * @return
     */
    bool setHeaderData(int col, Qt::Orientation orientation, const QVariant& data, int role = Qt::EditRole)
    { return false;}

    bool insertRows(int row, int count,
                    const QModelIndex& parent = QModelIndex());
    bool removeRows(int row, int count,
                    const QModelIndex& parent = QModelIndex());

    QAbstractItemDelegate* getItemDelegate();

    void setTableData(QVector<float> bins, QVector<QVector<float> > data, QVector<QColor> colors);

    QVector<float>& getBinNumbers()
    { return m_BinNumbers;}
    float getBinNumber(qint32 row)
    { return m_BinNumbers[row];}

    QVector<QColor>& getColors()
    { return m_Colors;}
    QColor getColor(qint32 row)
    { return m_Colors[row];}

    virtual QVector<float> getData(int col);
    virtual float getDataValue(int col, int row);
    virtual void setColumnData(int col, QVector<float>& data);

    QVector<float>& getAvergaes()
    { return m_Average;}
    QVector<float>& getStdDevs()
    { return m_StdDev;}

    float getAverage(qint32 row)
    { return m_Average[row];}
    float getStdDev(qint32 row)
    { return m_StdDev[row];}

  private:
    int m_ColumnCount;
    int m_RowCount;

    QVector<float> m_BinNumbers;
    QVector<float> m_Average;
    QVector<float> m_StdDev;
    QVector<QColor> m_Colors;

    SGLogNormalTableModel(const SGLogNormalTableModel&); // Copy Constructor Not Implemented
    void operator=(const SGLogNormalTableModel&); // Operator '=' Not Implemented
};

#endif /* _SG_LOG_NORMAL_TABLEMODEL_H_ */

