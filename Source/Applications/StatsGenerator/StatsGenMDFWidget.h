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

#ifndef _STATSGENMDFWIDGET_H_
#define _STATSGENMDFWIDGET_H_

#include <QtGui/QWidget>
#include "ui_StatsGenMDFWidget.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/StatsData/StatsData.h"
#include "DREAM3DLib/StatsData/PrimaryStatsData.h"
#include "DREAM3DLib/StatsData/PrecipitateStatsData.h"
#include "DREAM3DLib/StatsData/TransformationStatsData.h"
#include "StatsGenerator/TableModels/SGODFTableModel.h"


class SGMDFTableModel;
class QwtPlot;
class QwtPlotCurve;

/**
 * @class StatsGenMDFWidget StatsGenMDFWidget.h StatsGenerator/StatsGenMDFWidget.h
 * @brief This class gives GUIs an interface into the MDF settings
 * @author Michael A. Jackson for BlueQuartz Software
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

    DREAM3D_INSTANCE_PROPERTY(int, PhaseIndex)
    DREAM3D_INSTANCE_PROPERTY(unsigned int, CrystalStructure)
    DREAM3D_INSTANCE_PROPERTY(SGODFTableModel*, ODFTableModel)

    int getMisorientationData(StatsData* statsData, unsigned int phaseType);
    void extractStatsData(int index, StatsData* statsData, unsigned int phaseType);

    QVector<float> generateODFData();

    void updateMDFPlot(QVector<float>& odf);

    SGMDFTableModel* tableModel();

  protected slots:
    void on_addMDFRowBtn_clicked();
    void on_deleteMDFRowBtn_clicked();
    void on_m_MDFUpdateBtn_clicked();
    void on_loadMDFBtn_clicked();


  private:
    SGMDFTableModel* m_MDFTableModel;
    QwtPlotCurve*    m_PlotCurve;

    QString m_OpenDialogLastDirectory; // Must be last in the list
    StatsGenMDFWidget(const StatsGenMDFWidget&); // Copy Constructor Not Implemented
    void operator=(const StatsGenMDFWidget&); // Operator '=' Not Implemented

};

#endif /* _STATSGENMDFWIDGET_H_ */

