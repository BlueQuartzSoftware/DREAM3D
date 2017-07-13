/*
 * PrimaryPhaseWidget.h
 *
 *  Created on: Jun 14, 2012
 *      Author: mjackson
 */

#ifndef _primaryphasewidget_h_
#define _primaryphasewidget_h_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>

#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/SIMPLib.h"

#include "StatsGenerator/Widgets/Presets/AbstractMicrostructurePreset.h"
#include "StatsGenerator/Widgets/StatsGenWidget.h"
#include "StatsGenerator/ui_PrimaryPhaseWidget.h"

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


/**
 * @brief The PrimaryPhaseWidget class
 */
class PrimaryPhaseWidget : public StatsGenWidget, private Ui::PrimaryPhaseWidget
{
  Q_OBJECT

public:
  PrimaryPhaseWidget(QWidget* parent = 0);
  virtual ~PrimaryPhaseWidget();

  void updatePlots();

  SIMPL_INSTANCE_PROPERTY(PhaseType::Type, PhaseType)

  virtual void setCrystalStructure(unsigned int xtal);
  virtual void setPhaseIndex(int index);

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

  void setFeatureSizeWidget(StatsGenFeatureSizeWidget* w);
  StatsGenFeatureSizeWidget* getFeatureSizeWidget();

  QTabWidget* getTabWidget();

  QPushButton* getGenerateDefaultDataBtn();

  QComboBox* getMicrostructurePresetCombo();

  virtual void extractStatsData(AttributeMatrix::Pointer attrMat, int index);

  virtual QString getComboString();

  virtual int gatherStatsData(AttributeMatrix::Pointer attrMat, bool preflight = false);

  virtual QIcon getPhaseIcon();

  virtual void removeNeighborsPlotWidget();

  virtual void generateDefaultData();

protected slots:

  virtual void on_m_GenerateDefaultData_clicked();

  virtual void on_m_ResetDataBtn_clicked();

  void on_microstructurePresetCombo_currentIndexChanged(int index);

  void dataWasEdited();

  void bulkLoadEvent(bool fail);

  virtual void on_m_Omega3Btn_clicked(bool b);
  virtual void on_m_BOverABtn_clicked(bool b);
  virtual void on_m_COverABtn_clicked(bool b);
  virtual void on_m_NeighborBtn_clicked(bool b);

 signals:
  void dataChanged();
  void progressText(const QString &text);

protected:
  /**
    * @brief Enables or Disables all the widgets in a list
    * @param b
    */
  virtual void setWidgetListEnabled(bool b);

  /**
   * @brief setupGui
   */
  virtual void setupGui();

  /**
    * @brief Enables or disables the various PlotWidgetTabs
    * @param b Enable or disable the plotwidgets
    */
  virtual void setTabsPlotTabsEnabled(bool b);

private:
  QList<QWidget*> m_WidgetList;
  QButtonGroup            m_DistButtonGroup;
  bool m_ResetData = false;

  PrimaryPhaseWidget(const PrimaryPhaseWidget&); // Copy Constructor Not Implemented
  void operator=(const PrimaryPhaseWidget&);     // Operator '=' Not Implemented
};

#endif /* PRIMARYPHASEWIDGET_H_ */
