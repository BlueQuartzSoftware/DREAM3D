/*
 * PrecipitatePhaseWidget.h
 *
 *  Created on: Jun 14, 2012
 *      Author: mjackson
 */

#ifndef _PrecipitatePhaseWidget_H_
#define _PrecipitatePhaseWidget_H_

#include "SGWidget.h"
#include "Applications/StatsGenerator/ui_PrecipitatePhaseWidget.h"

#include "SIMPLib/Common/Constants.h"
#include "StatsGenerator/Presets/AbstractMicrostructurePreset.h"

#ifndef QwtArray
#define QwtArray QVector
#endif

class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotGrid;
class QwtPlotCurve;
class QwtPlotMarker;

/*
 *
 */
class PrecipitatePhaseWidget : public SGWidget, private Ui::PrecipitatePhaseWidget
{

    Q_OBJECT

  public:
    PrecipitatePhaseWidget(QWidget* parent = 0);
    virtual ~PrecipitatePhaseWidget();

    void updatePlots();

    SIMPL_INSTANCE_PROPERTY(unsigned int, PhaseType)
    void setCrystalStructure(unsigned int xtal);
    unsigned int getCrystalStructure() const;
    void setPhaseIndex(int index);
    int getPhaseIndex() const;
    SIMPL_INSTANCE_PROPERTY(float, PhaseFraction)
    SIMPL_INSTANCE_PROPERTY(float, TotalPhaseFraction)
    SIMPL_INSTANCE_PROPERTY(float, PptFraction)
    SIMPL_INSTANCE_PROPERTY(bool, DataHasBeenGenerated)

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

    QString getComboString();
    QString getTabTitle();

    void calculateNumberOfBins();
    int calculateNumberOfBins(float mu, float sigma, float minCutOff, float maxCutOff, float stepSize);
    int gatherSizeDistributionFromGui(float& mu, float& sigma, float& minCutOff, float& maxCutOff, float& stepSize);

    int gatherStatsData(AttributeMatrix::Pointer attrMat);

  public slots:
    void on_m_GenerateDefaultData_clicked();

  protected slots:

    void on_m_Mu_SizeDistribution_textChanged(const QString& text);
    void on_m_Sigma_SizeDistribution_textChanged(const QString& text);
    void on_m_MinSigmaCutOff_textChanged(const QString& text);
    void on_m_MaxSigmaCutOff_textChanged(const QString& text);
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
    unsigned int  m_CrystalStructure;

    QList<QWidget*>      m_WidgetList;
    QwtPlotCurve*        m_SizeDistributionCurve;
    QwtPlotMarker*       m_CutOffMin;
    QwtPlotMarker*       m_CutOffMax;
    QwtPlotGrid*         m_grid;

    AbstractMicrostructurePreset::Pointer m_MicroPreset;

    PrecipitatePhaseWidget(const PrecipitatePhaseWidget&); // Copy Constructor Not Implemented
    void operator=(const PrecipitatePhaseWidget&); // Operator '=' Not Implemented
};

#endif /* PrecipitatePhaseWidget_H_ */

