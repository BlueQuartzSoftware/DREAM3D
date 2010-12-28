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

#ifndef STATSGENTABLEMODEL_H_
#define STATSGENTABLEMODEL_H_

#include <QtCore/QAbstractTableModel>
#include <QtCore/QVector>
#include <QtCore/QVariant>
#include <QtGui/QColor>


/**
 * @class StatsGenTableModel StatsGenTableModel.h AIM/StatsGenerator/StatsGenTableModel.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Dec 22, 2010
 * @version 1.0
 */
class StatsGenTableModel : public QAbstractTableModel
{

    Q_OBJECT;

  public:

    enum ColumnIndexes {
      BinNumber = 0,
      NumGrains,
      Mu,
      Sigma,
      LineColor
    };

    StatsGenTableModel(QObject* parent = 0);
    virtual ~StatsGenTableModel();

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
                       int=Qt::EditRole) { return false; }

    bool insertRows(int row, int count,
                    const QModelIndex &parent=QModelIndex());
    bool removeRows(int row, int count,
                    const QModelIndex &parent=QModelIndex());


    QVector<qint32>& getBinNumbers() { return m_BinNumbers; }
    QVector<qint32>& getNumGrains() { return m_NumGrains; }
    QVector<double>& getMu() { return m_Mu; }
    QVector<double>& getSigma() { return m_Sigma; }
    QVector<QString>& getColors() { return m_Colors; }

    qint32 getBinNumber(qint32 row) { return m_BinNumbers[row]; }
    qint32 getNumGrains(qint32 row) { return m_NumGrains[row]; }
    double getMu(qint32 row) { return m_Mu[row]; }
    double getSigma(qint32 row) { return m_Sigma[row]; }
    QString getColor(qint32 row) { return m_Colors[row]; }


  private:
    int m_ColumnCount;
    int m_RowCount;

    QVector<qint32> m_BinNumbers;
    QVector<qint32> m_NumGrains;
    QVector<double> m_Mu;
    QVector<double> m_Sigma;
    QVector<QString> m_Colors;


    StatsGenTableModel(const StatsGenTableModel&); // Copy Constructor Not Implemented
    void operator=(const StatsGenTableModel&); // Operator '=' Not Implemented
};

#endif /* STATSGENTABLEMODEL_H_ */
