/*
 * MatrixPhaseWidget.h
 *
 *  Created on: Jun 14, 2012
 *      Author: mjackson
 */

#ifndef _MatrixPhaseWidget_H_
#define _MatrixPhaseWidget_H_

#include "SGWidget.h"
#include "ui_MatrixPhaseWidget.h"

#include "SIMPLib/Common/Constants.h"
#include "StatsGenerator/Presets/AbstractMicrostructurePreset.h"

class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotGrid;
class QwtPlotCurve;
class QwtPlotMarker;

/*
 *
 */
class MatrixPhaseWidget : public SGWidget, private Ui::MatrixPhaseWidget
{

    Q_OBJECT

  public:
    MatrixPhaseWidget(QWidget* parent = 0);
    virtual ~MatrixPhaseWidget();

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
    //s   void on_m_GenerateDefaultData_clicked();

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

    MatrixPhaseWidget(const MatrixPhaseWidget&); // Copy Constructor Not Implemented
    void operator=(const MatrixPhaseWidget&); // Operator '=' Not Implemented
};

#endif /* MatrixPhaseWidget_H_ */

