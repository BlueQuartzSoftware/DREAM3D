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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtCore/QAbstractTableModel>
#include <QtCore/QVariant>
#include <QtCore/QVector>

class QAbstractItemDelegate;

/*
 *
 */
class SGMDFTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  enum ColumnIndexes
  {
    Angle = 0,
    Axis,
    Weight,
    ColumnCount
  };

  SGMDFTableModel(QObject* parent = nullptr);
  virtual ~SGMDFTableModel();

  /**
   *
   * @param index
   * @return
   */
  Qt::ItemFlags flags(const QModelIndex& index) const override;

  /**
   *
   * @param index
   * @param role
   * @return
   */
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  /**
   *
   * @param section
   * @param orientation
   * @param role
   * @return
   */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  /**
   *
   * @param parent
   * @return
   */
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

  /**
   *
   * @param parent
   * @return
   */
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

  /**
   *
   * @param index
   * @param value
   * @param role
   * @return
   */
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

  /**
   *
   * @param col
   * @param orientation
   * @param data
   * @param role
   * @return
   */
  bool setHeaderData(int col, Qt::Orientation orientation, const QVariant& data, int role = Qt::EditRole) override;

  /**
   *
   * @param row
   * @param count
   * @param parent
   * @return
   */
  bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

  /**
   *
   * @param row
   * @param count
   * @param parent
   * @return
   */
  bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

  /**
   * @brief getItemDelegate
   * @return
   */
  QAbstractItemDelegate* getItemDelegate();

  /**
   *
   * @param col
   * @return
   */
  std::vector<float> getData(int col);

  /**
   * @brief Parses an "Axes" value
   * @param row The row that is going to be parsed.
   * @param h
   * @param k
   * @param l
   * @return
   */
  int parseHKLRow(int row, float& h, float& k, float& l) const;

  /**
   * @brief Sets the data for a column of data
   * @param col
   * @param data
   */
  void setColumnData(int col, const QVector<float>& data);

  /**
   * @brief Sets the data for a row in the table
   * @param row
   * @param angle
   * @param axis
   * @param weight
   */
  void setRowData(int row, float angle, float* const axis, float weight);

  /**
   * @brief setInitialValues
   */
  void setInitialValues();

  /**
   * @brief Sets all the table data in one function
   * @param angles The Angles
   * @param axis The Axis
   * @param weights The weight values
   */
  void setTableData(const QVector<float>& angles, const QVector<float>& axis, const QVector<float>& weights);

private:
  int m_ColumnCount = {ColumnCount};
  int m_RowCount = {0};

  QVector<float> m_Angles;
  QVector<float> m_Axis;
  QVector<float> m_Weights;

public:
  SGMDFTableModel(const SGMDFTableModel&) = delete;            // Copy Constructor Not Implemented
  SGMDFTableModel(SGMDFTableModel&&) = delete;                 // Move Constructor Not Implemented
  SGMDFTableModel& operator=(const SGMDFTableModel&) = delete; // Copy Assignment Not Implemented
  SGMDFTableModel& operator=(SGMDFTableModel&&) = delete;      // Move Assignment Not Implemented
};
