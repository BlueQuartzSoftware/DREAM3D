/*
 * PrimaryPhaseWidget.h
 *
 *  Created on: Jun 14, 2012
 *      Author: mjackson
 */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>

#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/SIMPLib.h"

#include "SyntheticBuilding/Gui/Widgets/StatsGenWidget.h"
#include "SyntheticBuilding/SyntheticBuildingFilters/Presets/AbstractMicrostructurePreset.h"
#include "SyntheticBuilding/ui_PrimaryPhaseWidget.h"

#ifndef QwtArray
#define QwtArray QVector
#endif

class QwtPlotGrid;
class QwtPlotCurve;
class QwtPlotMarker;
class QDoubleValidator;
class QComboBox;
class StatsGenPlotWidget;
class StatsGenODFWidget;
class StatsGenAxisODFWidget;
class StatsGenMDFWidget;

/**
 * @brief The PrimaryPhaseWidget class
 */
class PrimaryPhaseWidget : public StatsGenWidget, private Ui::PrimaryPhaseWidget
{
  Q_OBJECT

public:
  PrimaryPhaseWidget(QWidget* parent = nullptr);
  ~PrimaryPhaseWidget() override;

  void updatePlots();

  SIMPL_INSTANCE_PROPERTY(PhaseType::Type, PhaseType)

  void setCrystalStructure(const unsigned int& xtal) override;
  void setPhaseIndex(const int& index) override;

  SIMPL_INSTANCE_PROPERTY(QList<StatsGenPlotWidget*>, SGPlotWidgets)

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

  QString getComboString() override;

  int gatherStatsData(AttributeMatrix::Pointer attrMat, bool preflight = false) override;

  QIcon getPhaseIcon();

  void generateDefaultData() override;

protected slots:

  void on_m_GenerateDefaultData_clicked();

  void on_m_ResetDataBtn_clicked();

  void on_microstructurePresetCombo_currentIndexChanged(int index);

  void dataWasEdited();

  void bulkLoadEvent(bool fail);

signals:
  void dataChanged();
  void progressText(const QString& text);

protected:
  /**
   * @brief Enables or Disables all the widgets in a list
   * @param b
   */
  void setWidgetListEnabled(bool b);

  /**
   * @brief setupGui
   */
  void setupGui();

  /**
   * @brief Enables or disables the various PlotWidgetTabs
   * @param b Enable or disable the plotwidgets
   */
  void setTabsPlotTabsEnabled(bool b);

private:
  QList<QWidget*> m_WidgetList;
  bool m_ResetData = false;

public:
  PrimaryPhaseWidget(const PrimaryPhaseWidget&) = delete; // Copy Constructor Not Implemented
  PrimaryPhaseWidget(PrimaryPhaseWidget&&) = delete;      // Move Constructor Not Implemented
  PrimaryPhaseWidget& operator=(const PrimaryPhaseWidget&) = delete; // Copy Assignment Not Implemented
  PrimaryPhaseWidget& operator=(PrimaryPhaseWidget&&) = delete;      // Move Assignment Not Implemented
};

