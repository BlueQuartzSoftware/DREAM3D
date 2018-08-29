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

#include <QtCore/QFutureWatcher>
#include <QtGui/QImage>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QWidget>

#include "ui_StatsGenAxisODFWidget.h"

#ifndef QwtArray
#define QwtArray QVector
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/StatsData.h"
#include "SIMPLib/StatsData/TransformationStatsData.h"

#include "SyntheticBuilding/Gui/Utilities/PoleFigureImageUtilities.h"

class SGODFTableModel;
class StatsGenMDFWidget;
class QwtPlot;
class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotGrid;
class QwtPlotCurve;
class QwtPlotMarker;

/**
 * @class StatsGenODFWidget StatsGenODFWidget.h StatsGenerator/StatsGenODFWidget.h
 * @brief Calculates and displays ODF data
 *
 * @date Jan 27, 2011
 * @version 1.0
 */
class StatsGenODFWidget : public QWidget, private Ui::StatsGenAxisODFWidget
{
  Q_OBJECT

public:
  StatsGenODFWidget(QWidget* parent = nullptr);
  virtual ~StatsGenODFWidget();

  void setupGui();

  void initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot);
  void enableMDFTab(bool b);

  SIMPL_INSTANCE_PROPERTY(bool, EnableAxisDecorations)

  void setPhaseIndex(int value);
  int getPhaseIndex();

  void setCrystalStructure(unsigned int value);
  unsigned int getCrystalStructure();

  int getOrientationData(StatsData* statsData, PhaseType::Type phaseType, bool preflight = false);
  void extractStatsData(int index, StatsData* statsData, PhaseType::Type phaseType);

  void updatePlots();

  SGODFTableModel* tableModel();

  StatsGenMDFWidget* getMDFWidget();

  void drawODFPlotGrid(QwtPlot* plot);

  SIMPL_INSTANCE_PROPERTY(bool, Initializing)

protected slots:
  void on_m_CalculateODFBtn_clicked();
  void on_addODFTextureBtn_clicked();
  void on_deleteODFTextureBtn_clicked();
  void on_loadODFTextureBtn_clicked();
  void on_selectAnglesFile_clicked();

  void on_savePoleFigureImage_clicked();
  void on_angleFilePath_textChanged();

  void tableDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

  void on_m_ODFParametersBtn_clicked(bool b);
  void on_m_MDFParametersBtn_clicked(bool b);

  void on_m_WeightSpreads_clicked(bool b);
  void on_m_WeightSpreadsBulkLoad_clicked(bool b);
  
  void on_m_PFImageSize_valueChanged(int i);
  void on_m_ImageLayoutCB_currentIndexChanged(int index);
  void on_m_PFTypeCB_currentIndexChanged(int index);
  void on_m_PFLambertSize_valueChanged(int i);
  void updatePFStatus(const QString &msg);

signals:
  void dataChanged();
  void bulkLoadEvent(bool fail);

protected:
  /**
   * @brief calculateODF
   */
  void calculateODF();

private:
  int m_PhaseIndex;
  unsigned int m_CrystalStructure;
  SGODFTableModel* m_ODFTableModel = nullptr;
  SGODFTableModel* m_OdfBulkTableModel = nullptr;
  QVector<QwtPlotCurve*> m_PlotCurves;
  QwtPlotMarker* m_PlotGrid = nullptr;
  QwtPlotCurve* m_CircleGrid = nullptr;
  QwtPlotCurve* m_RotCross0 = nullptr;
  QwtPlotCurve* m_RotCross1 = nullptr;
  QButtonGroup m_ButtonGroup;
  QButtonGroup m_ODFGroup;
  bool m_AbortUpdate = true;

  QString m_OpenDialogLastFilePath; // Must be last in the list

  StatsGenODFWidget(const StatsGenODFWidget&) = delete; // Copy Constructor Not Implemented
  void operator=(const StatsGenODFWidget&) = delete;    // Move assignment Not Implemented
};

