/*
 * MatrixPhaseWidget.h
 *
 *  Created on: Jun 14, 2012
 *      Author: mjackson
 */

#ifndef _BoundaryPhaseWidget_H_
#define _BoundaryPhaseWidget_H_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Common/Constants.h"

#include "StatsGenerator/SGWidget.h"
#include "StatsGenerator/Presets/AbstractMicrostructurePreset.h"

#include "ui_BoundaryPhaseWidget.h"


class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotGrid;
class QwtPlotCurve;
class QwtPlotMarker;

/*
 *
 */
class BoundaryPhaseWidget : public SGWidget, private Ui::BoundaryPhaseWidget
{

    Q_OBJECT

  public:
    BoundaryPhaseWidget(QWidget* parent = 0);
    virtual ~BoundaryPhaseWidget();

    void updatePlots();

    SIMPL_INSTANCE_PROPERTY(unsigned int, PhaseType)
    SIMPL_INSTANCE_PROPERTY(unsigned int, CrystalStructure)
    SIMPL_INSTANCE_PROPERTY(int, PhaseIndex)
    SIMPL_INSTANCE_PROPERTY(float, PhaseFraction)
    SIMPL_INSTANCE_PROPERTY(float, TotalPhaseFraction)
    //    SIMPL_INSTANCE_PROPERTY(bool, DataHasBeenGenerated)

    void extractStatsData(AttributeMatrix::Pointer attrMat, int index);

    QString getComboString();
    QString getTabTitle();

    int gatherStatsData(AttributeMatrix::Pointer attrMat);

    // public slots:
    //   void on_m_GenerateDefaultData_clicked();

  protected slots:

    void dataWasEdited();
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
    QList<QWidget*>      m_WidgetList;
    QwtPlotGrid*         m_grid;

    BoundaryPhaseWidget(const BoundaryPhaseWidget&); // Copy Constructor Not Implemented
    void operator=(const BoundaryPhaseWidget&); // Operator '=' Not Implemented
};

#endif /* BoundaryPhaseWidget_H_ */

