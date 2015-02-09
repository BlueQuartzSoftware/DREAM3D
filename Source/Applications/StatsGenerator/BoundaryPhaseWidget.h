/*
 * MatrixPhaseWidget.h
 *
 *  Created on: Jun 14, 2012
 *      Author: mjackson
 */

#ifndef _BoundaryPhaseWidget_H_
#define _BoundaryPhaseWidget_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

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

    DREAM3D_INSTANCE_PROPERTY(unsigned int, PhaseType)
    DREAM3D_INSTANCE_PROPERTY(unsigned int, CrystalStructure)
    DREAM3D_INSTANCE_PROPERTY(int, PhaseIndex)
    DREAM3D_INSTANCE_PROPERTY(float, PhaseFraction)
    DREAM3D_INSTANCE_PROPERTY(float, TotalPhaseFraction)
    //    DREAM3D_INSTANCE_PROPERTY(bool, DataHasBeenGenerated)

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

