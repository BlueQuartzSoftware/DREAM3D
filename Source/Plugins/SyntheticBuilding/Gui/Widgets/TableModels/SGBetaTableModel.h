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

#pragma once

#include "SGAbstractTableModel.h"
#include <QtCore/QVariant>
#include <QtCore/QVector>

class QAbstractItemDelegate;

/**
 * @class SGBetaTableModel SGBetaTableModel.h StatsGenerator/SGBetaTableModel.h
 * @brief
 *
 * @date Dec 22, 2010
 * @version 1.0
 */
class SGBetaTableModel : public SGAbstractTableModel
{

  Q_OBJECT

public:
  enum ColumnIndexes
  {
    BinNumber = 0,
    Alpha,
    Beta,
    ColumnCount
  };

  SGBetaTableModel(QObject* parent = nullptr);

  ~SGBetaTableModel();

  /**
   * @brief flags
   * @param index
   * @return
   */
  Qt::ItemFlags flags(const QModelIndex& index) const override;

  /**
   * @brief data
   * @param index
   * @param role
   * @return
   */
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  /**
   * @brief headerData
   * @param section
   * @param orientation
   * @param role
   * @return
   */
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  /**
   * @brief rowCount
   * @param parent
   * @return
   */
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

  /**
   * @brief columnCount
   * @param parent
   * @return
   */
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

  /**
   * @brief setData
   * @param index
   * @param value
   * @param role
   * @return
   */
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

  /**
   * @brief setHeaderData
   * @param col
   * @param orientation
   * @param data
   * @param role
   * @return
   */
  bool setHeaderData(int col, Qt::Orientation orientation, const QVariant& data, int role = Qt::EditRole) override;

  /**
   * @brief insertRows
   * @param row
   * @param count
   * @param parent
   * @return
   */
  bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

  /**
   * @brief removeRows
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
  QAbstractItemDelegate* getItemDelegate() override;

  /**
   * @brief setTableData
   * @param bins
   * @param data
   * @param colors
   */
  void setTableData(QVector<float> bins, QVector<QVector<float>> data, QVector<SIMPL::Rgb> colors) override;

  /**
   * @brief getBinNumbers
   * @return
   */
  const QVector<float>& getBinNumbers() const override;

  /**
   * @brief getBinNumber
   * @param row
   * @return
   */
  float getBinNumber(qint32 row) const override;

  /**
   * @brief getColors
   * @return
   */
  const QVector<SIMPL::Rgb>& getColors() const override;

  /**
   * @brief getColor
   * @param row
   * @return
   */
  SIMPL::Rgb getColor(qint32 row) const override;

  /**
   * @brief getData
   * @param col
   * @return
   */
  std::vector<float> getData(int col) const override;

  /**
   * @brief getDataValue
   * @param col
   * @param row
   * @return
   */
  float getDataValue(int col, int row) const override;

  /**
   * @brief setColumnData
   * @param col
   * @param data
   */
  void setColumnData(int col, QVector<float>& data) override;

  //------------------------------- Class Specific Data Accessors

  const QVector<float>& getAlphas() const;

  const QVector<float>& getBetas() const;

  float getAlpha(qint32 row) const;

  float getBeta(qint32 row) const;

private:
  int m_ColumnCount;
  int m_RowCount;

  QVector<float> m_BinNumbers;
  QVector<float> m_Alpha;
  QVector<float> m_Beta;
  QVector<SIMPL::Rgb> m_Colors;

public:
  SGBetaTableModel(const SGBetaTableModel&) = delete;            // Copy Constructor Not Implemented
  SGBetaTableModel(SGBetaTableModel&&) = delete;                 // Move Constructor Not Implemented
  SGBetaTableModel& operator=(const SGBetaTableModel&) = delete; // Copy Assignment Not Implemented
  SGBetaTableModel& operator=(SGBetaTableModel&&) = delete;      // Move Assignment Not Implemented
};
