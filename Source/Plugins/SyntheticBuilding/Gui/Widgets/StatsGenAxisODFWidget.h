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

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QWidget>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/PhaseType.h"

#include "ui_StatsGenAxisODFWidget.h"

class SGODFTableModel;
class StatsGenMDFWidget;
class QwtPlot;
class QwtPlotCurve;
class QwtPlotMarker;
class StatsData;

/**
 * @class StatsGenAxisODFWidget StatsGenAxisODFWidget.h StatsGenerator/StatsGenAxisODFWidget.h
 * @brief Calculates and displays ODF data
 *
 * @date Jan 27, 2011
 * @version 1.0
 */
class StatsGenAxisODFWidget : public QWidget, private Ui::StatsGenAxisODFWidget
{
  Q_OBJECT

public:
  StatsGenAxisODFWidget(QWidget* parent = nullptr);
  virtual ~StatsGenAxisODFWidget();

  void setupGui();

  void initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot);
  void enableMDFTab(bool b);

  /**
   * @brief Setter property for EnableAxisDecorations
   */
  void setEnableAxisDecorations(bool value);
  /**
   * @brief Getter property for EnableAxisDecorations
   * @return Value of EnableAxisDecorations
   */
  bool getEnableAxisDecorations() const;

  void setPhaseIndex(int value);
  int getPhaseIndex();

  unsigned int getCrystalStructure();

  int getOrientationData(StatsData* statsData, PhaseType::Type phaseType, bool preflight = false);
  void extractStatsData(int index, StatsData* statsData, PhaseType::Type phaseType);

  void setPlotTabTitles(QString t1, QString t2, QString t3);

  void updatePlots();

  SGODFTableModel* tableModel();

  void drawODFPlotGrid(QwtPlot* plot);

protected Q_SLOTS:
  void on_m_CalculateODFBtn_clicked();
  void on_addODFTextureBtn_clicked();
  void on_deleteODFTextureBtn_clicked();
  void on_loadODFTextureBtn_clicked();
  void on_savePoleFigureImage_clicked();
  void tableDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

  virtual void on_m_WeightSpreads_clicked(bool b);
  virtual void on_m_WeightSpreadsBulkLoad_clicked(bool b);

Q_SIGNALS:
  void dataChanged();

protected:
  /**
   * @brief Setter property for Initializing
   */
  void setInitializing(bool value);
  /**
   * @brief Getter property for Initializing
   * @return Value of Initializing
   */
  bool getInitializing() const;

  /**
   * @brief calculateAxisODF
   */
  void calculateAxisODF();

private:
  bool m_EnableAxisDecorations = {};
  bool m_Initializing = {};

  int m_PhaseIndex;
  unsigned int m_CrystalStructure;
  SGODFTableModel* m_ODFTableModel;
  QVector<QwtPlotCurve*> m_PlotCurves;
  QwtPlotMarker* m_PlotGrid;
  QwtPlotCurve* m_CircleGrid;
  QwtPlotCurve* m_RotCross0;
  QwtPlotCurve* m_RotCross1;
  QButtonGroup m_ButtonGroup;
  QButtonGroup m_ODFGroup;
  QString m_OpenDialogLastDirectory; // Must be last in the list

public:
  StatsGenAxisODFWidget(const StatsGenAxisODFWidget&) = delete;            // Copy Constructor Not Implemented
  StatsGenAxisODFWidget(StatsGenAxisODFWidget&&) = delete;                 // Move Constructor Not Implemented
  StatsGenAxisODFWidget& operator=(const StatsGenAxisODFWidget&) = delete; // Copy Assignment Not Implemented
  StatsGenAxisODFWidget& operator=(StatsGenAxisODFWidget&&) = delete;      // Move Assignment Not Implemented
};
