/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _STATSGENPLOTWIDGET_H_
#define _STATSGENPLOTWIDGET_H_

#include <QtGui/QWidget>
#include "ui_StatsGenPlotWidget.h"

#include "StatsGen.h"
#include "AIM/Common/HDF5/H5ReconStatsWriter.h"

class SGAbstractTableModel;
class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotGrid;
class QwtPlotCurve;

namespace UIA
{
  const static int Alpha = 255;
}

class StatsGenPlotWidget : public QWidget, private Ui::StatsGenPlotWidget
{

  Q_OBJECT

  public:
    StatsGenPlotWidget(QWidget *parent = 0);
    virtual ~StatsGenPlotWidget();

    void setPlotTitle(QString title);

    int writeDataToHDF5(H5ReconStatsWriter::Pointer writer);

    void setCurveType(StatsGen::CurveType curveType);
    void setXAxisName(QString name);
    void setYAxisName(QString name);

    void setRowOperationEnabled(bool b);

    void setupGui();

    void createBetaCurve(int tableRow, double &xMax, double &yMax);
    void createLogNormalCurve(int tableRow, double &xMax, double &yMax);
    void createPowerCurve(int tableRow, double &xMax, double &yMax);

    void setBins(QVector<int > &binNumbers);

  protected slots:
    void updatePlotCurves();

    void on_addRowBtn_clicked();
    void on_deleteRowBtn_clicked();
    void on_curveTypeCombo_currentIndexChanged(int index);

  protected:
    void resetTableModel();


  private:
    SGAbstractTableModel* m_TableModel;

//    QwtPlotZoomer* m_zoomer;
//    QwtPlotPicker* m_picker;
//    QwtPlotPanner* m_panner;
    QwtPlotGrid* m_grid;
    StatsGen::CurveType m_CurveType;

    QVector<QwtPlotCurve* > m_PlotCurves;

    StatsGenPlotWidget(const StatsGenPlotWidget&); // Copy Constructor Not Implemented
    void operator=(const StatsGenPlotWidget&); // Operator '=' Not Implemented
};

#endif /* _STATSGENPLOTWIDGET_H_ */
