/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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

#ifndef _STATSGENMDFWIDGET_H_
#define _STATSGENMDFWIDGET_H_

#include <QtGui/QWidget>
#include "ui_StatsGenMDFWidget.h"

#include "AIM/Common/Constants.h"
#include "AIM/Common/HDF5/H5ReconStatsWriter.h"
#include "AIM/Common/HDF5/H5ReconStatsReader.h"
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
  Q_OBJECT;

  public:
    StatsGenMDFWidget(QWidget *parent = 0);
    virtual ~StatsGenMDFWidget();

    void setupGui();
    void initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot);

    MXA_INSTANCE_PROPERTY(int, PhaseIndex);
    MXA_INSTANCE_PROPERTY(AIM::Reconstruction::CrystalStructure, CrystalStructure);
    MXA_INSTANCE_PROPERTY(SGODFTableModel*, ODFTableModel);

    int writeDataToHDF5(H5ReconStatsWriter::Pointer writer);
    int readDataFromHDF5(H5ReconStatsReader::Pointer reader,
                        QVector<double>  &bins,
                        const std::string &hdf5GroupName);
    
  QwtArray<double> generateODFData();

  void updateMDFPlot(QwtArray<double> odf);

  protected slots:
    void on_addMDFRowBtn_clicked();
    void on_deleteMDFRowBtn_clicked();
    void on_m_MDFUpdateBtn_clicked();


  private:
    SGMDFTableModel* m_MDFTableModel;
    QwtPlotCurve*    m_PlotCurve;

    StatsGenMDFWidget(const StatsGenMDFWidget&); // Copy Constructor Not Implemented
    void operator=(const StatsGenMDFWidget&); // Operator '=' Not Implemented

};

#endif /* _STATSGENMDFWIDGET_H_ */
