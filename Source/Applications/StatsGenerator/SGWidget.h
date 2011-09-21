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
#ifndef SGWIDGET_H_
#define SGWIDGET_H_

#include <QtGui/QWidget>
#include "ui_SGWidget.h"

#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/HDF5/H5StatsWriter.h"
#include "DREAM3D/HDF5/H5StatsReader.h"
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
class SGWidget : public QWidget, private Ui::SGWidget
{

  Q_OBJECT ;

  public:
    SGWidget(QWidget *parent = 0);
    virtual ~SGWidget();

    void updatePlots();

    void setPhaseIndex(int index);
    int getPhaseIndex();

    MXA_INSTANCE_PROPERTY(DREAM3D::Reconstruction::PhaseType, PhaseType);
    MXA_INSTANCE_PROPERTY(float, PhaseFraction);
    MXA_INSTANCE_PROPERTY(float, TotalPhaseFraction);
    MXA_INSTANCE_PROPERTY(float, PptFraction);
    MXA_INSTANCE_PROPERTY(bool, DataHasBeenGenerated);

    void setCrystalStructure(Ebsd::CrystalStructure xtal);
    Ebsd::CrystalStructure getCrystalStructure();

    void plotSizeDistribution();
    void updateSizeDistributionPlot();
    int computeBinsAndCutOffs( float mu, float sigma,
                               float minCutOff, float maxCutOff,
                               float binStepSize,
                               QwtArray<float> &binsizes,
                               QwtArray<float> &xCo,
                               QwtArray<float> &yCo,
                               float &xMax, float &yMax,
                               QwtArray<float> &x,
                               QwtArray<float> &y);

    QString getComboString();

    void calculateNumberOfBins();
    int calculateNumberOfBins(float mu, float sigma, float minCutOff, float maxCutOff, float stepSize);
    int gatherSizeDistributionFromGui(float &mu, float &sigma, float &minCutOff, float &maxCutOff, float &stepSize);

    int writeDataToHDF5(H5StatsWriter::Pointer writer);
    int readDataFromHDF5(H5StatsReader::Pointer reader, int phase);

  public slots:
    void on_m_GenerateDefaultData_clicked();

  protected slots:

    void on_m_Mu_SizeDistribution_textChanged(const QString &text);
    void on_m_Sigma_SizeDistribution_textChanged(const QString &text);
    void on_m_MinSigmaCutOff_textChanged(const QString &text);
    void on_m_MaxSigmaCutOff_textChanged(const QString &text);
    void on_m_BinStepSize_valueChanged(double v);

    void on_microstructurePresetCombo_currentIndexChanged(int index);


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
    Ebsd::CrystalStructure  m_CrystalStructure;

    QList<QWidget*>      m_WidgetList;
    QwtPlotCurve*        m_SizeDistributionCurve;
    QwtPlotMarker*       m_CutOffMin;
    QwtPlotMarker*       m_CutOffMax;
    QwtPlotGrid*         m_grid;
    AbstractMicrostructurePreset::Pointer m_MicroPreset;

    SGWidget(const SGWidget&); // Copy Constructor Not Implemented
    void operator=(const SGWidget&); // Operator '=' Not Implemented

};

#endif /* SGWIDGET_H_ */
