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

#include <QtCore/QList>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"

#include "SyntheticBuilding/Gui/Widgets/StatsGenWidget.h"
#include "SyntheticBuilding/SyntheticBuildingFilters/Presets/AbstractMicrostructurePreset.h"
#include "ui_TransformationPhaseWidget.h"

class QwtPlotGrid;
class QwtPlotCurve;
class QwtPlotMarker;

/*
 *
 */
class TransformationPhaseWidget : public StatsGenWidget, private Ui::TransformationPhaseWidget
{

  Q_OBJECT

public:
  TransformationPhaseWidget(QWidget* parent = nullptr);
  ~TransformationPhaseWidget() override;

  void updatePlots();

  void setCrystalStructure(unsigned int xtal) override;
  unsigned int getCrystalStructure() const override;
  void setPhaseIndex(int index) override;
  int getPhaseIndex() const override;

  /**
   * @brief Setter property for ParentPhase
   */
  void setParentPhase(unsigned int value);
  /**
   * @brief Getter property for ParentPhase
   * @return Value of ParentPhase
   */
  unsigned int getParentPhase() const;

  /**
   * @brief Setter property for PhaseType
   */
  void setPhaseType(PhaseType::Type value) override;
  /**
   * @brief Getter property for PhaseType
   * @return Value of PhaseType
   */
  PhaseType::Type getPhaseType() const override;

  /**
   * @brief Setter property for SGPlotWidgets
   */
  void setSGPlotWidgets(const QList<StatsGenPlotWidget*>& value);
  /**
   * @brief Getter property for SGPlotWidgets
   * @return Value of SGPlotWidgets
   */
  QList<StatsGenPlotWidget*> getSGPlotWidgets() const;

  void setOmega3PlotWidget(StatsGenPlotWidget* w);
  StatsGenPlotWidget* getOmega3PlotWidget();

  void setBOverAPlotPlotWidget(StatsGenPlotWidget* w);
  StatsGenPlotWidget* getBOverAPlotPlotWidget();

  void setCOverAPlotWidget(StatsGenPlotWidget* w);
  StatsGenPlotWidget* getCOverAPlotWidget();

  void setODFWidget(StatsGenODFWidget* w);
  StatsGenODFWidget* getODFWidget();

  void setAxisODFWidget(StatsGenAxisODFWidget* w);
  StatsGenAxisODFWidget* getAxisODFWidget();

  void setMDFWidget(StatsGenMDFWidget* w);
  StatsGenMDFWidget* getMDFWidget();

  void setFeatureSizeWidget(StatsGenFeatureSizeWidget* w);
  StatsGenFeatureSizeWidget* getFeatureSizeWidget();

  QTabWidget* getTabWidget();

  QPushButton* getGenerateDefaultDataBtn();

  QComboBox* getMicrostructurePresetCombo();

  void removeNeighborsPlotWidget();

  void extractStatsData(AttributeMatrix::Pointer attrMat, int index) override;

  int gatherStatsData(AttributeMatrix::Pointer attrMat, bool preflight = false) override;

  void generateDefaultData() override;

protected Q_SLOTS:

  void on_m_GenerateDefaultData_clicked();

  void on_m_ResetDataBtn_clicked();

  void on_microstructurePresetCombo_currentIndexChanged(int index);

  void dataWasEdited();
  void bulkLoadEvent(bool fail);

Q_SIGNALS:
  void dataChanged();
  void progressText(const QString& text);

protected:
  /**
   * @brief Enables or Disables all the widgets in a list
   * @param b
   */
  void setWidgetListEnabled(bool b);

  void setupGui();

  /**
   * @brief Enables or disables the various PlotWidgetTabs
   * @param b Enable or disable the plotwidgets
   */
  void setTabsPlotTabsEnabled(bool b);

private:
  unsigned int m_ParentPhase = {};
  PhaseType::Type m_PhaseType = {};
  QList<StatsGenPlotWidget*> m_SGPlotWidgets = {};

  int m_PhaseIndex;
  unsigned int m_CrystalStructure;

  QList<QWidget*> m_WidgetList;
  bool m_ResetData = false;

  AbstractMicrostructurePreset::Pointer m_MicroPreset;

public:
  TransformationPhaseWidget(const TransformationPhaseWidget&) = delete;            // Copy Constructor Not Implemented
  TransformationPhaseWidget(TransformationPhaseWidget&&) = delete;                 // Move Constructor Not Implemented
  TransformationPhaseWidget& operator=(const TransformationPhaseWidget&) = delete; // Copy Assignment Not Implemented
  TransformationPhaseWidget& operator=(TransformationPhaseWidget&&) = delete;      // Move Assignment Not Implemented
};
