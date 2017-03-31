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

#ifndef _statsgenrdfwidget_h_
#define _statsgenrdfwidget_h_

#include <QtWidgets/QWidget>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/StatsData/StatsData.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/Math/RadialDistributionFunction.h"

#include "TableModels/SGRDFTableModel.h"

#include "ui_StatsGenRDFWidget.h"

class SGRDFTableModel;
class QwtPlotCurve;

/**
 * @class StatsGenRDFWidget StatsGenRDFWidget.h StatsGenerator/StatsGenRDFWidget.h
 * @brief This class gives GUIs an interface into the RDF settings
 *
 * @date Apr 20, 2011
 * @version 1.0
 */
class StatsGenRDFWidget : public QWidget, private Ui::StatsGenRDFWidget
{
    Q_OBJECT

  public:
    StatsGenRDFWidget(QWidget* parent = 0);
    virtual ~StatsGenRDFWidget();

    void setupGui();
    void initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot);

    int getMisorientationData(StatsData* statsData, PhaseType::Type phaseType);
    void extractStatsData(int index, StatsData* statsData, PhaseType::Type phaseType);

    QVector<float> generateODFData();

    void updateRDFPlot(QVector<float>& freqs);
    void updatePlots();

    SGRDFTableModel* tableModel();

    RdfData::Pointer getStatisticsData();

    QString getTabTitle();

  protected slots:
    void on_generateRDFBtn_clicked();
    void on_minDistLE_textChanged(const QString& text);
    void on_maxDistLE_textChanged(const QString& text);
    void on_BoxSizeXLE_textChanged(const QString& text);
    void on_BoxSizeYLE_textChanged(const QString& text);
    void on_BoxSizeZLE_textChanged(const QString& text);

  signals:
    void rdfParametersChanged();

  private:
    SGRDFTableModel* m_RDFTableModel;
    QwtPlotCurve*    m_PlotCurve;

    QString m_OpenDialogLastFilePath; // Must be last in the list


    StatsGenRDFWidget(const StatsGenRDFWidget&); // Copy Constructor Not Implemented
    void operator=(const StatsGenRDFWidget&); // Operator '=' Not Implemented

};

#endif /* _StatsGenRDFWidget_H_ */

