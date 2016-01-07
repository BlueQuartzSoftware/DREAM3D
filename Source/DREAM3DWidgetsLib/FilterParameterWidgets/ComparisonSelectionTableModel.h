/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


#ifndef _QUALITYMETRICTABLEMODEL_H_
#define _QUALITYMETRICTABLEMODEL_H_

#include <QtCore/QAbstractTableModel>
#include <QtCore/QVector>
#include <QtCore/QVariant>
#include <QtCore/QStringList>

#include "SIMPLib/Common/ComparisonInputs.h"

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"


class QAbstractItemDelegate;

/*
 *
 */
class DREAM3DWidgetsLib_EXPORT ComparisonSelectionTableModel : public QAbstractTableModel
{
    Q_OBJECT

  public:
    enum ColumnIndexes
    {
      FeatureName = 0,
      FeatureOperator,
      FeatureValue,
      ColumnCount
    };


    ComparisonSelectionTableModel(bool showOperators, QObject* parent = 0);

    virtual ~ComparisonSelectionTableModel();

    /**
     *
     * @param index
     * @return
     */
    virtual Qt::ItemFlags flags(const QModelIndex& index) const ;

    /**
     *
     * @param index
     * @param role
     * @return
     */
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const ;

    /**
     *
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const ;

    /**
     *
     * @param parent
     * @return
     */
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const ;

    /**
     *
     * @param parent
     * @return
     */
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const ;

    /**
     *
     * @param index
     * @param value
     * @param role
     * @return
     */
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) ;

    /**
     *
     * @param col
     * @param orientation
     * @param data
     * @param role
     * @return
     */
    virtual bool setHeaderData(int col, Qt::Orientation orientation, const QVariant& data, int role = Qt::EditRole) ;

    /**
     *
     * @param row
     * @param count
     * @param parent
     * @return
     */
    virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) ;

    /**
     *
     * @param row
     * @param count
     * @param parent
     * @return
     */
    virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) ;

    /**
     *
     * @return
     */
    virtual QAbstractItemDelegate* getItemDelegate() ;


    void setPossibleFeatures(QStringList features);

    void setPossibleFeatures(QList<QString>& features);

    QStringList getPossibleFeatures();

    virtual void setTableData(QVector<QString> featureNames, QVector<float> featureValues,  QVector<int> featureOperators);

    virtual void setTableData(ComparisonInputs& comps);

    void getTableData( QVector<QString>& featureNames, QVector<float>& featureValues,  QVector<int>& featureOperators);

    void setNumberOfPhases(int n);


  private:
    int m_ColumnCount;
    int m_RowCount;
    int m_NumberOfPhases;

    QStringList   m_PossibleFeatures;
    QVector<QString>   m_FeatureNames;
    QVector<float>     m_FeatureValues;
    QVector<QString> m_FeatureOperators;


    ComparisonSelectionTableModel(const ComparisonSelectionTableModel&); // Copy Constructor Not Implemented
    void operator=(const ComparisonSelectionTableModel&); // Operator '=' Not Implemented
};

#endif /* QUALITYMETRICTABLEMODEL_H_ */

