/*
 * PrimaryPhaseWidget.h
 *
 *  Created on: Jun 14, 2012
 *      Author: mjackson
 */

#ifndef _primaryphasewidget_h_
#define _primaryphasewidget_h_

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"

#include "StatsGenerator/SGWidget.h"
#include "StatsGenerator/Presets/AbstractMicrostructurePreset.h"
#include "StatsGenerator/ui_PrimaryPhaseWidget.h"


#ifndef QwtArray
#define QwtArray QVector
#endif


class QwtPlotGrid;
class QwtPlotCurve;
class QwtPlotMarker;
class QDoubleValidator;


/*
 * LaTeX to generate the equations
 x^{\left( \mu - max*\sigma \right) }

 */

/*
 *
 */
class PrimaryPhaseWidget : public SGWidget, private Ui::PrimaryPhaseWidget
{

    Q_OBJECT

  public:
    PrimaryPhaseWidget(QWidget* parent = 0);
    virtual ~PrimaryPhaseWidget();

    void updatePlots();



    SIMPL_INSTANCE_PROPERTY(unsigned int, PhaseType)
    void setCrystalStructure(unsigned int xtal);
    unsigned int getCrystalStructure() const;
    void setPhaseIndex(int index);
    int getPhaseIndex() const;
    SIMPL_INSTANCE_PROPERTY(float, PhaseFraction)
    SIMPL_INSTANCE_PROPERTY(float, TotalPhaseFraction)
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


  protected slots:
    void on_m_GenerateDefaultData_clicked();
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

    /**
     * @brief validateValue
     * @param val
     * @param lineEdit
     * @return
     */
    bool validateValue(QDoubleValidator* val, QLineEdit* lineEdit);

    /**
     * @brief validateMuSigma
     * @return
     */
    bool validateMuSigma();

  private:
    int                  m_PhaseIndex;
    unsigned int  m_CrystalStructure;

    QList<QWidget*>      m_WidgetList;
    QwtPlotCurve*        m_SizeDistributionCurve;
    QwtPlotMarker*       m_CutOffMin;
    QwtPlotMarker*       m_CutOffMax;
    QwtPlotGrid*         m_grid;
    AbstractMicrostructurePreset::Pointer m_MicroPreset;

    QDoubleValidator* m_MuValidator;
    QDoubleValidator* m_SigmaValidator;

    PrimaryPhaseWidget(const PrimaryPhaseWidget&); // Copy Constructor Not Implemented
    void operator=(const PrimaryPhaseWidget&); // Operator '=' Not Implemented
};

#endif /* PRIMARYPHASEWIDGET_H_ */

