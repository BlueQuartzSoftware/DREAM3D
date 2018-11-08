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

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Math/RadialDistributionFunction.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/StatsData.h"

#include "TableModels/SGRDFTableModel.h"

class SGRDFTableModel;
class QwtPlotCurve;
class QwtPlot;

namespace Ui
{
class StatsGenRDFWidget;
}
/**
 * @class StatsGenRDFWidget StatsGenRDFWidget.h StatsGenerator/StatsGenRDFWidget.h
 * @brief This class gives GUIs an interface into the RDF settings
 *
 * @date Apr 20, 2011
 * @version 1.0
 */
class StatsGenRDFWidget : public QWidget
{
  Q_OBJECT

public:
  StatsGenRDFWidget(QWidget* parent = nullptr);
  virtual ~StatsGenRDFWidget();
  /**
   * @brief setupGui
   */
  void setupGui();

  /**
   * @brief initQwtPlot
   * @param xAxisName
   * @param yAxisName
   * @param plot
   */
  void initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot);

  /**
   * @brief getMisorientationData
   * @param statsData
   * @param phaseType
   * @return
   */
  int getMisorientationData(StatsData* statsData, PhaseType::Type phaseType);

  /**
   * @brief extractStatsData
   * @param index
   * @param statsData
   * @param phaseType
   */
  void extractStatsData(int index, StatsData* statsData, PhaseType::Type phaseType);

  /**
   * @brief generateODFData
   * @return
   */
  QVector<float> generateODFData();

  /**
   * @brief updateRDFPlot
   * @param freqs
   */
  void updateRDFPlot(QVector<float>& freqs);

  /**
   * @brief updatePlots
   */
  void updatePlots();

  /**
   * @brief tableModel
   * @return
   */
  SGRDFTableModel* tableModel();

  /**
   * @brief getStatisticsData
   * @return
   */
  RdfData::Pointer getStatisticsData();

  /**
   * @brief getTabTitle
   * @return
   */
  QString getTabTitle();

protected slots:
  /**
   * @brief on_generateRDFBtn_clicked
   */
  void on_generateRDFBtn_clicked();

signals:

  /**
   * @brief rdfParametersChanged
   */
  void rdfParametersChanged();

private:
  QSharedPointer<Ui::StatsGenRDFWidget> ui = nullptr;

  SGRDFTableModel* m_RDFTableModel;
  QwtPlotCurve* m_PlotCurve;

  QString m_OpenDialogLastFilePath; // Must be last in the list

  /**
   * @brief validateInput
   * @return
   */
  bool validateInput();

public:
  StatsGenRDFWidget(const StatsGenRDFWidget&) = delete; // Copy Constructor Not Implemented
  StatsGenRDFWidget(StatsGenRDFWidget&&) = delete;      // Move Constructor Not Implemented
  StatsGenRDFWidget& operator=(const StatsGenRDFWidget&) = delete; // Copy Assignment Not Implemented
  StatsGenRDFWidget& operator=(StatsGenRDFWidget&&) = delete;      // Move Assignment Not Implemented
};

