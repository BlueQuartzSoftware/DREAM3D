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

#ifndef _STATSGENODFWIDGET_H_
#define _STATSGENODFWIDGET_H_

#include <QtGui/QWidget>

#include "ui_SGAxisODFWidget.h"

#include <MXA/Common/MXASetGetMacros.h>

#include "AIM/Common/Constants.h"
#include "AIM/Common/HDF5/H5ReconStatsWriter.h"
#include "AIM/Common/HDF5/H5ReconStatsReader.h"


class SGODFTableModel;
class StatsGenMDFWidget;
class QwtPlot;
class QwtPlotCurve;

/**
 * @class StatsGenODFWidget StatsGenODFWidget.h AIM/StatsGenerator/StatsGenODFWidget.h
 * @brief Calculates and displays ODF data
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 27, 2011
 * @version 1.0
 */
class StatsGenODFWidget : public QWidget, private Ui::SGAxisODFWidget
{
  Q_OBJECT;

  public:
    StatsGenODFWidget(QWidget *parent = 0);
    virtual ~StatsGenODFWidget();

    void setupGui();

    void initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot);
    void enableMDFTab(bool b);

    MXA_INSTANCE_PROPERTY(bool, EnableAxisDecorations)

    void setPhaseIndex(int value);
    int getPhaseIndex();

    void setCrystalStructure(AIM::Reconstruction::CrystalStructure value);
    AIM::Reconstruction::CrystalStructure getCrystalStructure();

    int writeDataToHDF5(H5ReconStatsWriter::Pointer writer);
    int readDataFromHDF5(H5ReconStatsReader::Pointer reader,
                        QVector<float>  &bins,
                        const std::string &hdf5GroupName);

    void setPlotTabTitles(QString t1, QString t2, QString t3);

    protected slots:
      void on_m_CalculateODFBtn_clicked();
      void on_addODFTextureBtn_clicked();
      void on_deleteODFTextureBtn_clicked();

    protected:
      MXA_INSTANCE_PROPERTY(bool, Initializing)

    private:
      int      m_PhaseIndex;
      AIM::Reconstruction::CrystalStructure      m_CrystalStructure;
      SGODFTableModel*        m_ODFTableModel;
      StatsGenMDFWidget*      m_MDFWidget;
      QVector<QwtPlotCurve*>  m_PlotCurves;

      StatsGenODFWidget(const StatsGenODFWidget&); // Copy Constructor Not Implemented
      void operator=(const StatsGenODFWidget&); // Operator '=' Not Implemented

};

#endif /* _STATSGENODFWIDGET_H_ */
