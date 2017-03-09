#ifndef _statsGenFeatureSizeWidget_H_
#define _statsGenFeatureSizeWidget_H_

#include <cmath>

#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"

#include "StatsGenerator/Widgets/StatsGenWidget.h"
#include "StatsGenerator/ui_StatsGenFeatureSizeWidget.h"

#ifndef QwtArray
#define QwtArray QVector
#endif

/*
 * LaTeX to generate the equations

x^{\left( \mu + max*\sigma \right) }
x^{\left( \mu - min*\sigma \right) }

*/

class QwtPlotGrid;
class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotShapeItem;
class QDoubleValidator;
class QTreeWidget;
class QLineEdit;
class StatsGenFeatureSizeWidget;
class StatsGenPlotWidget;
class StatsGenODFWidget;
class PrimaryStatsData;
class PrecipitateStatsData;

/**
 * @brief The StatsGenFeatureSizeWidget class
 */
class StatsGenFeatureSizeWidget : public QWidget, private Ui::StatsGenFeatureSizeWidget
{
  Q_OBJECT
public:
  StatsGenFeatureSizeWidget(QWidget* parent = 0);
  virtual ~StatsGenFeatureSizeWidget();

  SIMPL_INSTANCE_PROPERTY(float, Mu)
  SIMPL_INSTANCE_PROPERTY(float, Sigma)
  SIMPL_INSTANCE_PROPERTY(float, MinCutOff)
  SIMPL_INSTANCE_PROPERTY(float, MaxCutOff)
  SIMPL_INSTANCE_PROPERTY(float, BinStep)

  SIMPL_INSTANCE_PROPERTY(int, PhaseIndex)
  SIMPL_INSTANCE_PROPERTY(unsigned int, CrystalStructure)

  SIMPL_INSTANCE_PROPERTY(QwtArray<float>, BinSizes);

  /**
   * @brief plotSizeDistribution
   */
  void plotSizeDistribution();

  /**
   * @brief updateSizeDistributionPlot
   * @return
   */
  int updateSizeDistributionPlot();

  /**
   * @brief computeBinsAndCutOffs
   * @param mu
   * @param sigma
   * @param minCutOff
   * @param maxCutOff
   * @param binStepSize
   * @param binsizes
   * @param xCo
   * @param yCo
   * @param xMax
   * @param yMax
   * @param x
   * @param y
   * @return
   */
  int computeBinsAndCutOffs(float mu, float sigma, float minCutOff, float maxCutOff, float binStepSize, QwtArray<float>& binsizes, QwtArray<float>& xCo, QwtArray<float>& yCo, float& xMax, float& yMax,
                            QwtArray<float>& x, QwtArray<float>& y);

  /**
   * @brief calculateNumberOfBins
   * @return
   */
  int calculateNumberOfBins();

  /**
   * @brief calculateNumberOfBins
   * @param mu
   * @param sigma
   * @param minCutOff
   * @param maxCutOff
   * @param stepSize
   * @return
   */
  int calculateNumberOfBins(float mu, float sigma, float minCutOff, float maxCutOff, float stepSize);

  /**
   * @brief gatherSizeDistributionFromGui
   * @param mu
   * @param sigma
   * @param minCutOff
   * @param maxCutOff
   * @param stepSize
   * @return
   */
  int gatherSizeDistributionFromGui(float& mu, float& sigma, float& minCutOff, float& maxCutOff, float& stepSize);

  /**
   * @brief getStatisticsData
   * @param primaryStatsData
   * @return
   */
  virtual int getStatisticsData(PrimaryStatsData* primaryStatsData);

  /**
   * @brief resetUI This method will reset the UI back to its default state.
   */
  virtual void resetUI();

  /**
   * @brief StatsGenFeatureSizeWidget::extractStatsData
   * @param statsData
   * @param index
   */
  template<typename StatsData>
  void extractStatsData(StatsData* statsData, int index)
  {
    Q_UNUSED(index)
    float binStepSize = 0.0f;
    float maxFeatureSize = 0.0f, minFeatureSize = 0.0f;
    float mu = 0.0f;
    float sigma = 0.0f;
    float minCutOff = 0.0f;
    float maxCutOff = 0.0f;

    QLocale loc = QLocale::system();

    /* Set the Feature_Diameter_Info Data */
    binStepSize = statsData->getBinStepSize();
    m_BinStepSize->blockSignals(true);
    m_BinStepSize->setValue(binStepSize);
    m_BinStepSize->blockSignals(false);
    maxFeatureSize = statsData->getMaxFeatureDiameter();
    minFeatureSize = statsData->getMinFeatureDiameter();

    /* Set the Feature_Size_Distribution Data */
    VectorOfFloatArray distData = statsData->getFeatureSizeDistribution();
    mu = distData[0]->getValue(0);
    sigma = distData[1]->getValue(0);

    m_Sigma_SizeDistribution->blockSignals(true);
    m_FeatureESD->blockSignals(true);

    m_Sigma_SizeDistribution->setText(loc.toString(sigma));
    float esd = std::exp(mu);
    m_FeatureESD->setText(loc.toString(esd));

    m_FeatureESD->blockSignals(false);
    m_Sigma_SizeDistribution->blockSignals(false);

    minCutOff = (mu - log(minFeatureSize)) / sigma;
    maxCutOff = (log(maxFeatureSize) - mu) / sigma;

    m_MinSigmaCutOff->blockSignals(true);
    m_MinSigmaCutOff->setText(QString::number(minCutOff));
    m_MinSigmaCutOff->blockSignals(false);

    m_MaxSigmaCutOff->blockSignals(true);
    m_MaxSigmaCutOff->setText(QString::number(maxCutOff));
    m_MaxSigmaCutOff->blockSignals(false);

    // Set the Text of the Mu QLineEdit which will cause all the plots to update
    // because we are NOT blocking signals
    m_Mu_SizeDistribution->setText(loc.toString(mu));

    int err = calculateNumberOfBins();
    if(err < 0)
    {

    }

  }

signals:

  void userEnteredValidData(bool);

  void dataChanged();

  void binSelected(int index);

protected:
  /**
  * @brief setupGui
  */
  void setupGui();

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

  /**
  * @brief mousePressEvent
  * @param event
  */
  void mousePressEvent(QMouseEvent* event);

protected slots:

  void on_m_Mu_SizeDistribution_textChanged(const QString& text);
  void on_m_Sigma_SizeDistribution_textChanged(const QString& text);
  void on_m_MinSigmaCutOff_textChanged(const QString& text);
  void on_m_MaxSigmaCutOff_textChanged(const QString& text);
  void on_m_BinStepSize_valueChanged(double v);
  void on_m_FeatureESD_editingFinished();

  /**
   * @brief plotPointSelected Slot to pick up signal from Qwt when a user clicks on the plot.
   * @param point
   */
  void plotPointSelected(const QPointF& point);

  /**
   * @brief userEditedPlotData
   */
  void userEditedPlotData();

private:
  QwtPlotCurve* m_SizeDistributionCurve = nullptr;
  QwtPlotCurve* m_SizeDistCenterPointCurve = nullptr;
  QwtPlotMarker* m_CutOffMin = nullptr;
  QwtPlotMarker* m_CutOffMax = nullptr;
  QVector<QwtPlotShapeItem*> m_BarChartItems;

  QDoubleValidator* m_MuValidator = nullptr;
  QDoubleValidator* m_SigmaValidator = nullptr;
  QDoubleValidator* m_MinCutoffValidator = nullptr;
  QDoubleValidator* m_MaxCutoffValidator = nullptr;
  bool m_EsdUpdated = false;
};

#endif /* _statsGenFeatureSizeWidget_H_ */
