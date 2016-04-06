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

#ifndef _sgabstracttablemodel_h_
#define _sgabstracttablemodel_h_

#include <QtCore/QAbstractTableModel>
#include <QtCore/QVector>
#include <QtCore/QVariant>
#include <QtGui/QColor>

class QAbstractItemDelegate;

/**
 * @class SGAbstractTableModel SGAbstractTableModel.h StatsGenerator/SGAbstractTableModel.h
 *
 * @date Jan 6, 2011
 * @version 1.0
 *
 * @brief This is the superclass for creating TableModels for the various types of
 * curves that are avaiable. If you want to create a new TableModel for a new type
 * of curve calculation then there are several classes that you will need to implement.
 *
 * @li SGxxxxTableModel.h
 * @li SGxxxxTableModel.cpp
 *
 * This is the actual implementation of the TableModel class. It should inherit from
 * this class in order to make sure your new class has the functionality that is
 * required. You can take a look at one of the other classes in order to get an
 * idea of what all needs to be implemented. Sometimes directly copying and pasting
 * from a current class and then changing the bits that are diffferent works. Some
 * places to make sure to update are the <b>ColumnIndexes</b> enumerator which defines
 * the column indexes for each of the parameters. Each of the methods will have dependent
 * code based on these column headers so be sure to look at <b>each</b> method for
 * appropriate changes to make.
 *
 * @li SGxxxxItemDelegate.h
 *
 * This class tells Qt how to present editors for the values in the TableModel. Again
 * a copy/paste with appropriate changes to the class names may be all that is necessary
 * to implement that class.
 */
class SGAbstractTableModel : public QAbstractTableModel
{
    Q_OBJECT

  public:

    enum ColumnIndexes
    {
      BinNumber = 0
    };

    SGAbstractTableModel(QObject* parent = 0);
    virtual ~SGAbstractTableModel();

    /**
     *
     * @param index
     * @return
     */
    virtual Qt::ItemFlags flags(const QModelIndex& index) const = 0;

    /**
     *
     * @param index
     * @param role
     * @return
     */
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const = 0;

    /**
     *
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const = 0;

    /**
     *
     * @param parent
     * @return
     */
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const = 0;

    /**
     *
     * @param parent
     * @return
     */
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const = 0;

    /**
     *
     * @param index
     * @param value
     * @param role
     * @return
     */
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) = 0;

    /**
     *
     * @param col
     * @param orientation
     * @param data
     * @param role
     * @return
     */
    virtual bool setHeaderData(int col, Qt::Orientation orientation, const QVariant& data, int role = Qt::EditRole) = 0;

    /**
     *
     * @param row
     * @param count
     * @param parent
     * @return
     */
    virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) = 0;

    /**
     *
     * @param row
     * @param count
     * @param parent
     * @return
     */
    virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) = 0;

    /**
     *
     * @return
     */
    virtual QAbstractItemDelegate* getItemDelegate() = 0;

    /**
     *
     * @param binNumbers
     */
    //virtual void setBinNumbers(QVector<float> binNumbers) = 0;
    virtual void setTableData(QVector<float> bins, QVector<QVector<float> > data, QVector<QString> colors) = 0;


    /**
     *
     * @return
     */
    virtual QVector<float>& getBinNumbers() = 0;

    /**
     *
     * @param row
     * @return
     */
    virtual float getBinNumber(qint32 row) = 0;

    /**
     *
     * @return
     */
    virtual QVector<QString>& getColors() = 0;

    /**
     *
     * @param row
     * @return
     */
    virtual QString getColor(qint32 row) = 0;

    /**
     *
     * @param col
     * @return
     */
    virtual QVector<float> getData(int col) = 0;

    /**
     *
     * @param col
     * @param row
     * @return
     */
    virtual float getDataValue(int col, int row) = 0;


    virtual void setColumnData(int col, QVector<float>& data) = 0;


  private:

    SGAbstractTableModel(const SGAbstractTableModel&); // Copy Constructor Not Implemented
    void operator=(const SGAbstractTableModel&); // Operator '=' Not Implemented

};

#endif /* SGABSTRACTTABLEMODEL_H_ */

