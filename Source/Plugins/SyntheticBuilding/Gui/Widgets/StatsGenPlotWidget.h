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

#include <QtWidgets/QWidget>

#include "ui_StatsGenPlotWidget.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/StatsData/StatsData.h"

class QTableView;
class SGAbstractTableModel;
class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotGrid;
class QwtPlotCurve;

/**
 * @brief The StatsGenPlotWidget class
 */
class StatsGenPlotWidget : public QWidget, private Ui::StatsGenPlotWidget
{

  Q_OBJECT

public:
  StatsGenPlotWidget(QWidget* parent = nullptr);
  virtual ~StatsGenPlotWidget();

  VectorOfFloatArray getStatisticsData();

  int extractStatsData(int index, QVector<float>& binNumbers, VectorOfFloatArray arrays);

  void setStatisticsType(unsigned int statsType);
  void setDistributionType(unsigned int distributionType, bool updatePlots = true);
  uint32_t getDistributionType()
  {
    return m_DistributionType;
  }
  void blockDistributionTypeChanges(bool block);

  void setDataTitle(const QString& title);
  void setPlotTitle(const QString& title);
  void setXAxisName(const QString& name);
  void setYAxisName(const QString& name);

  void setRowOperationEnabled(bool b);

  void setupGui();

  void createBetaCurve(int tableRow, float& xMax, float& yMax);
  void createLogNormalCurve(int tableRow, float& xMax, float& yMax);
  void createPowerCurve(int tableRow, float& xMax, float& yMax);
  SGAbstractTableModel* tableModel();

  //    void setBins(QVector<float> &binValues);
  bool userUpdatedData();

  /**
   * @brief Setter property for Mu
   */
  void setMu(float value);
  /**
   * @brief Getter property for Mu
   * @return Value of Mu
   */
  float getMu() const;

  /**
   * @brief Setter property for Sigma
   */
  void setSigma(float value);
  /**
   * @brief Getter property for Sigma
   * @return Value of Sigma
   */
  float getSigma() const;

  /**
   * @brief Setter property for MinCutOff
   */
  void setMinCutOff(float value);
  /**
   * @brief Getter property for MinCutOff
   * @return Value of MinCutOff
   */
  float getMinCutOff() const;

  /**
   * @brief Setter property for MaxCutOff
   */
  void setMaxCutOff(float value);
  /**
   * @brief Getter property for MaxCutOff
   * @return Value of MaxCutOff
   */
  float getMaxCutOff() const;

  /**
   * @brief Setter property for BinStep
   */
  void setBinStep(float value);
  /**
   * @brief Getter property for BinStep
   * @return Value of BinStep
   */
  float getBinStep() const;

  /**
   * @brief Setter property for PhaseIndex
   */
  void setPhaseIndex(int value);
  /**
   * @brief Getter property for PhaseIndex
   * @return Value of PhaseIndex
   */
  int getPhaseIndex() const;

  /**
   * @brief Setter property for CrystalStructure
   */
  void setCrystalStructure(unsigned int value);
  /**
   * @brief Getter property for CrystalStructure
   * @return Value of CrystalStructure
   */
  unsigned int getCrystalStructure() const;

  void setSizeDistributionValues(float mu, float sigma, float minCutOff, float maxCutOff, float binStepSize);

signals:
  void dataChanged();

public slots:
  void updatePlotCurves();
  void userCommittedData(QWidget* w);
  void highlightCurve(int index);
  void requestContextMenu(const QPoint& pos);
  void showDataWindow(bool b);

protected:
  void resetTableModel();
  void initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot);

private:
  float m_Mu = {};
  float m_Sigma = {};
  float m_MinCutOff = {};
  float m_MaxCutOff = {};
  float m_BinStep = {};
  int m_PhaseIndex = {};
  unsigned int m_CrystalStructure = {};

  unsigned int m_DistributionType;
  SGAbstractTableModel* m_TableModel = nullptr;
  unsigned int m_StatsType;
  bool m_UserUpdatedData;

  QVector<QwtPlotCurve*> m_PlotCurves;
  QWidget* m_TableViewWidget = nullptr;
  QTableView* m_TableView = nullptr;
  QPoint m_ContextMenuPoint;

public:
  StatsGenPlotWidget(const StatsGenPlotWidget&) = delete; // Copy Constructor Not Implemented
  StatsGenPlotWidget(StatsGenPlotWidget&&) = delete;      // Move Constructor Not Implemented
  StatsGenPlotWidget& operator=(const StatsGenPlotWidget&) = delete; // Copy Assignment Not Implemented
  StatsGenPlotWidget& operator=(StatsGenPlotWidget&&) = delete;      // Move Assignment Not Implemented
};

