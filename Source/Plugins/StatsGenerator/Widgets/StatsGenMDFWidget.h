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


#ifndef _statsgenmdfwidget_h_
#define _statsgenmdfwidget_h_

#include <QtWidgets/QWidget>
#include "ui_StatsGenMDFWidget.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/StatsData/StatsData.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/TransformationStatsData.h"
#include "StatsGenerator/Widgets/TableModels/SGODFTableModel.h"


class SGMDFTableModel;
class QwtPlot;
class QwtPlotCurve;

/**
 * @class StatsGenMDFWidget StatsGenMDFWidget.h StatsGenerator/StatsGenMDFWidget.h
 * @brief This class gives GUIs an interface into the MDF settings
 *
 * @date Apr 20, 2011
 * @version 1.0
 */
class StatsGenMDFWidget : public QWidget, private Ui::StatsGenMDFWidget
{
    Q_OBJECT

  public:
    StatsGenMDFWidget(QWidget* parent = 0);
    virtual ~StatsGenMDFWidget();

    void setupGui();
    void initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot);

    SIMPL_INSTANCE_PROPERTY(int, PhaseIndex)
    SIMPL_INSTANCE_PROPERTY(unsigned int, CrystalStructure)
    SIMPL_INSTANCE_PROPERTY(SGODFTableModel*, ODFTableModel)

    int getMisorientationData(StatsData* statsData, PhaseType::Type phaseType, bool preflight = false);
    void extractStatsData(int index, StatsData* statsData, PhaseType::Type phaseType);

    void updateMDFPlot(QVector<float>& odf);

    SGMDFTableModel* tableModel();

  protected slots:
    void on_addMDFRowBtn_clicked();
    void on_deleteMDFRowBtn_clicked();
    void on_m_MDFUpdateBtn_clicked();
    void on_loadMDFBtn_clicked();

    void tableDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

  signals:
    void dataChanged();

  private:
    SGMDFTableModel* m_MDFTableModel = nullptr;
    QwtPlotCurve*    m_PlotCurve = nullptr;
    QString          m_OpenDialogLastDirectory; // Must be last in the list

    StatsGenMDFWidget(const StatsGenMDFWidget&); // Copy Constructor Not Implemented
    void operator=(const StatsGenMDFWidget&); // Operator '=' Not Implemented

};

#endif /* _STATSGENMDFWIDGET_H_ */

