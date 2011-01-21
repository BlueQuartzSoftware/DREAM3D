/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _SG_BETA_TABLEMODEL_H_
#define _SG_BETA_TABLEMODEL_H_

#include <QtCore/QVector>
#include <QtCore/QVariant>
#include <QtGui/QColor>
#include "SGAbstractTableModel.h"

class QAbstractItemDelegate;

/**
 * @class SGBetaTableModel SGBetaTableModel.h AIM/StatsGenerator/SGBetaTableModel.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Dec 22, 2010
 * @version 1.0
 */
class SGBetaTableModel : public SGAbstractTableModel
{

Q_OBJECT    ;

    public:

    enum ColumnIndexes
    {
      BinNumber = 0,
      Alpha,
      Beta,
      LineColor,
      ColumnCount
    };

    SGBetaTableModel(QObject* parent = 0);
    virtual ~SGBetaTableModel();

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index,
        int role=Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
        int role=Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;

    bool setData(const QModelIndex &index, const QVariant &value,
        int role=Qt::EditRole);
    bool setHeaderData(int, Qt::Orientation, const QVariant&,
        int=Qt::EditRole)
    { return false;}

    bool insertRows(int row, int count,
        const QModelIndex &parent=QModelIndex());
    bool removeRows(int row, int count,
        const QModelIndex &parent=QModelIndex());

    QAbstractItemDelegate* getItemDelegate();

    void setBinNumbers(QVector<double> binNumbers);

    QVector<double>& getBinNumbers()
    { return m_BinNumbers;}
    double getBinNumber(qint32 row)
    { return m_BinNumbers[row];}

    QVector<QString>& getColors()
    { return m_Colors;}
    QString getColor(qint32 row)
    { return m_Colors[row];}

    virtual QVector<double> getData(int col);
    virtual double getDataValue(int col, int row);

    QVector<double>& getAlphas()
    { return m_Alpha;}
    QVector<double>& getBetas()
    { return m_Beta;}

    double getAlpha(qint32 row)
    { return m_Alpha[row];}
    double getBeta(qint32 row)
    { return m_Beta[row];}

    private:
    int m_ColumnCount;
    int m_RowCount;

    QVector<double> m_BinNumbers;
    QVector<double> m_Alpha;
    QVector<double> m_Beta;
    QVector<QString> m_Colors;

    SGBetaTableModel(const SGBetaTableModel&); // Copy Constructor Not Implemented
    void operator=(const SGBetaTableModel&); // Operator '=' Not Implemented
  };

#endif /* _SG_BETA_TABLEMODEL_H_ */
