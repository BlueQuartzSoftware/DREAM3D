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
#ifndef SGABSTRACTTABLEMODEL_H_
#define SGABSTRACTTABLEMODEL_H_

#include <QtCore/QAbstractTableModel>
#include <QtCore/QVector>
#include <QtCore/QVariant>
#include <QtGui/QColor>

class QAbstractItemDelegate;


class SGAbstractTableModel : public QAbstractTableModel
{
  Q_OBJECT;

public:

  enum ColumnIndexes {
    BinNumber = 0
  };


  SGAbstractTableModel(QObject* parent = 0);
  virtual ~SGAbstractTableModel();

  virtual Qt::ItemFlags flags(const QModelIndex &index) const = 0;
  virtual QVariant data(const QModelIndex &index,  int role=Qt::DisplayRole) const = 0;
  virtual QVariant headerData(int section, Qt::Orientation orientation,  int role=Qt::DisplayRole) const = 0;
  virtual int rowCount(const QModelIndex &parent=QModelIndex()) const = 0;
 
  virtual int columnCount(const QModelIndex &parent=QModelIndex()) const = 0;

  virtual bool setData(const QModelIndex &index, const QVariant &value,  int role=Qt::EditRole) = 0;
  virtual bool setHeaderData(int, Qt::Orientation, const QVariant&, int=Qt::EditRole) = 0; // { return false; }

  virtual bool insertRows(int row, int count, const QModelIndex &parent=QModelIndex()) = 0;
  virtual bool removeRows(int row, int count, const QModelIndex &parent=QModelIndex()) = 0;

  virtual QAbstractItemDelegate* getItemDelegate() = 0;

  virtual void setBinNumbers(QVector<int> binNumbers) = 0;

  virtual QVector<qint32>& getBinNumbers() = 0; //  { return m_BinNumbers; }
  virtual qint32 getBinNumber(qint32 row) = 0; //   { return m_BinNumbers[row]; }
  
  virtual QVector<QString>& getColors() = 0; //   { return m_Colors; }
  virtual QString getColor(qint32 row) = 0; //   { return m_Colors[row]; }

  virtual QVector<double> getData(int col) = 0;
  virtual double getDataValue(int col, int row) = 0;

private:

  SGAbstractTableModel(const SGAbstractTableModel&); // Copy Constructor Not Implemented
  void operator=(const SGAbstractTableModel&); // Operator '=' Not Implemented
 
};

#endif /* SGABSTRACTTABLEMODEL_H_ */
