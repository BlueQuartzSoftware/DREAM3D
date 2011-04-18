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
#ifndef SGWIDGET_H_
#define SGWIDGET_H_

#include <QtGui/QWidget>
#include "ui_SGWidget.h"

#include "AIM/Common/Constants.h"
#include "AIM/Common/HDF5/H5ReconStatsWriter.h"
#include "AIM/Common/HDF5/H5ReconStatsReader.h"

class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotGrid;
class QwtPlotCurve;
class QwtPlotMarker;

/*
 *
 */
class SGWidget : public QWidget, private Ui::SGWidget
{

  Q_OBJECT ;

  public:
    SGWidget(QWidget *parent = 0);
    virtual ~SGWidget();

    MXA_INSTANCE_PROPERTY(int, PhaseIndex);
    MXA_INSTANCE_PROPERTY(AIM::Reconstruction::CrystalStructure, CrystalStructure);

    void plotSizeDistribution();
    void updateSizeDistributionPlot();
    int computeBinsAndCutOffs(double mu,
                              double sigma,
                              double cutOff,
                              double binStepSize,
                              QwtArray<double> &binsizes,
                              QwtArray<double> &xCo,
                              QwtArray<double> &yCo,
                              double &xMax,
                              double &yMax,
                              QwtArray<double> &x,
                              QwtArray<double> &y);

    QString getComboString();

    void calculateNumberOfBins();
    int calculateNumberOfBins(double mu, double sigma, double cutOff, double stepSize);
    int gatherSizeDistributionFromGui(double &mu, double &sigma, double &cutOff, double &binStepSize);

    int writeDataToHDF5(H5ReconStatsWriter::Pointer writer);
    int readDataFromHDF5(H5ReconStatsReader::Pointer reader, int phase);
  protected slots:

    void on_m_Mu_SizeDistribution_textChanged(const QString &text);
    void on_m_Sigma_SizeDistribution_textChanged(const QString &text);
    void on_m_SigmaCutOff_SizeDistribution_textChanged(const QString &text);
    void on_m_BinStepSize_valueChanged(double v);

    void on_m_GenerateDefaultData_clicked();

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
    QwtPlotCurve*        m_SizeDistributionCurve;
    QwtPlotMarker*       m_CutOffMin;
    QwtPlotMarker*       m_CutOffMax;
    QwtPlotGrid*         m_grid;
    AIM::Reconstruction::CrystalStructure  m_Xtal;


    SGWidget(const SGWidget&); // Copy Constructor Not Implemented
    void operator=(const SGWidget&); // Operator '=' Not Implemented

};

#endif /* SGWIDGET_H_ */
