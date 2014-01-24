/*
 * MatrixPhaseWidget.h
 *
 *  Created on: Jun 14, 2012
 *      Author: mjackson
 */

#ifndef MatrixPhaseWidget_H_
#define MatrixPhaseWidget_H_

#include "SGWidget.h"
#include "ui_MatrixPhaseWidget.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
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
     MatrixPhaseWidget(QWidget *parent = 0);
     virtual ~MatrixPhaseWidget();

     void updatePlots();

     void setPhaseIndex(int index);
     int getPhaseIndex();

     DREAM3D_INSTANCE_PROPERTY(unsigned int, PhaseType)
     DREAM3D_INSTANCE_PROPERTY(float, PhaseFraction)
     DREAM3D_INSTANCE_PROPERTY(float, TotalPhaseFraction)
 //    DREAM3D_INSTANCE_PROPERTY(bool, DataHasBeenGenerated)

     void extractStatsData(AttributeMatrix::Pointer attrMat, int index);

     void setCrystalStructure(unsigned int xtal);
     unsigned int getCrystalStructure();

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
     int                  m_PhaseIndex;
     unsigned int  m_CrystalStructure;

     QList<QWidget*>      m_WidgetList;
     QwtPlotGrid*         m_grid;

     MatrixPhaseWidget(const MatrixPhaseWidget&); // Copy Constructor Not Implemented
     void operator=(const MatrixPhaseWidget&); // Operator '=' Not Implemented
};

#endif /* MatrixPhaseWidget_H_ */

