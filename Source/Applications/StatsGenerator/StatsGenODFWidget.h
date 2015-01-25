/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _STATSGENODFWIDGET_H_
#define _STATSGENODFWIDGET_H_


#include <QtCore/QFutureWatcher>
#include <QtGui/QImage>
#include <QtGui/QWidget>

#include "ui_SGAxisODFWidget.h"

#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_abstract_scale_draw.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/StatsData/StatsData.h"
#include "DREAM3DLib/StatsData/PrimaryStatsData.h"
#include "DREAM3DLib/StatsData/PrecipitateStatsData.h"
#include "DREAM3DLib/StatsData/TransformationStatsData.h"

#include "QtSupport/PoleFigureImageUtilities.h"

class SGODFTableModel;
class StatsGenMDFWidget;


/**
 * @class StatsGenODFWidget StatsGenODFWidget.h StatsGenerator/StatsGenODFWidget.h
 * @brief Calculates and displays ODF data
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 27, 2011
 * @version 1.0
 */
class StatsGenODFWidget : public QWidget, private Ui::SGAxisODFWidget
{
    Q_OBJECT

  public:
    StatsGenODFWidget(QWidget* parent = 0);
    virtual ~StatsGenODFWidget();

    void setupGui();

    void initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot);
    void enableMDFTab(bool b);

    DREAM3D_INSTANCE_PROPERTY(bool, EnableAxisDecorations)

    void setPhaseIndex(int value);
    int getPhaseIndex();

    void setCrystalStructure(unsigned int value);
    unsigned int getCrystalStructure();

    int getOrientationData(StatsData* statsData, unsigned int phaseType);
    void extractStatsData(int index, StatsData* statsData, unsigned int phaseType);

    void updatePlots();

    SGODFTableModel* tableModel();

    StatsGenMDFWidget* getMDFWidget();

    void drawODFPlotGrid(QwtPlot* plot);

    DREAM3D_INSTANCE_PROPERTY(bool, Initializing)

  protected slots:
    void on_m_CalculateODFBtn_clicked();
    void on_addODFTextureBtn_clicked();
    void on_deleteODFTextureBtn_clicked();
    void on_loadODFTextureBtn_clicked();
    void on_selectAnglesFile_clicked();

    void on_bulkLoadGroupBox_clicked ( bool checked = false );
    void on_weightSpreadGroupBox_clicked ( bool checked = false );
    void on_savePoleFigureImage_clicked();


  protected:

  private:
    int      m_PhaseIndex;
    unsigned int      m_CrystalStructure;
    SGODFTableModel*        m_ODFTableModel;
    SGODFTableModel*        m_OdfBulkTableModel;
    StatsGenMDFWidget*      m_MDFWidget;
    QVector<QwtPlotCurve*>  m_PlotCurves;
    QwtPlotMarker*          m_PlotGrid;
    QwtPlotCurve*           m_CircleGrid;
    QwtPlotCurve*           m_RotCross0;
    QwtPlotCurve*           m_RotCross1;

    QString m_OpenDialogLastDirectory; // Must be last in the list

    StatsGenODFWidget(const StatsGenODFWidget&); // Copy Constructor Not Implemented
    void operator=(const StatsGenODFWidget&); // Operator '=' Not Implemented

};

#endif /* _STATSGENODFWIDGET_H_ */

