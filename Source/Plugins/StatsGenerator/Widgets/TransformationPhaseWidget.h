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

#ifndef _transformationphasewidget_h_
#define _transformationphasewidget_h_

#include <QtCore/QList>
#include <QtCore/QVector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"

#include "StatsGenerator/Widgets/StatsGenWidget.h"
#include "StatsGenerator/Widgets/Presets/AbstractMicrostructurePreset.h"
#include "ui_TransformationPhaseWidget.h"


class QwtPlotGrid;
class QwtPlotCurve;
class QwtPlotMarker;

/*
 *
 */
class TransformationPhaseWidget : public StatsGenWidget, private Ui::TransformationPhaseWidget
{

    Q_OBJECT

  public:
    TransformationPhaseWidget(QWidget* parent = 0);
    virtual ~TransformationPhaseWidget();

    void updatePlots();

    SIMPL_INSTANCE_PROPERTY(unsigned int, PhaseType)
    void setCrystalStructure(unsigned int xtal);
    unsigned int getCrystalStructure() const;
    void setPhaseIndex(int index);
    int getPhaseIndex() const;
    SIMPL_INSTANCE_PROPERTY(float, PhaseFraction)
    SIMPL_INSTANCE_PROPERTY(float, TotalPhaseFraction)
    SIMPL_INSTANCE_PROPERTY(unsigned int, ParentPhase)
    SIMPL_INSTANCE_PROPERTY(bool, DataHasBeenGenerated)
    SIMPL_INSTANCE_PROPERTY(bool, BulkLoadFailure)

    void extractStatsData(AttributeMatrix::Pointer attrMat, int index);

    void plotSizeDistribution();
    void updateSizeDistributionPlot();
    int computeBinsAndCutOffs( float mu, float sigma,
                               float minCutOff, float maxCutOff,
                               float binStepSize,
                               QwtArray<float>& binsizes,
                               QwtArray<float>& xCo,
                               QwtArray<float>& yCo,
                               float& xMax, float& yMax,
                               QwtArray<float>& x,
                               QwtArray<float>& y);


    void calculateNumberOfBins();
    int calculateNumberOfBins(float mu, float sigma, float minCutOff, float maxCutOff, float stepSize);
    int gatherSizeDistributionFromGui(float& mu, float& sigma, float& minCutOff, float& maxCutOff, float& stepSize);

    int gatherStatsData(AttributeMatrix::Pointer attrMat, bool preflight = false);

    virtual void generateDefaultData();

  protected slots:
    void on_m_GenerateDefaultData_clicked();
    void on_m_Mu_SizeDistribution_textChanged(const QString& text);
    void on_m_Sigma_SizeDistribution_textChanged(const QString& text);
    void on_m_MinSigmaCutOff_textChanged(const QString& text);
    void on_m_MaxSigmaCutOff_textChanged(const QString& text);
    void on_m_BinStepSize_valueChanged(double v);

    void dataWasEdited();
    void bulkLoadEvent(bool fail);

  signals:
    void phaseParametersChanged();

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
    QwtPlotCurve*        m_SizeDistributionCurve;
    QwtPlotMarker*       m_CutOffMin;
    QwtPlotMarker*       m_CutOffMax;
    QwtPlotGrid*         m_grid;
    AbstractMicrostructurePreset::Pointer m_MicroPreset;

    TransformationPhaseWidget(const TransformationPhaseWidget&); // Copy Constructor Not Implemented
    void operator=(const TransformationPhaseWidget&); // Operator '=' Not Implemented
};

#endif /* TransformationPhaseWidget_H_ */

