/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "PrecipitatePhaseWidget.h"


#include <iostream>
#include <limits>

#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <QtCore/QRunnable>
#include <QtCore/QThreadPool>
#include <QtCore/QtConcurrentRun>
#include <QtGui/QMessageBox>


//-- Qwt Includes
#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#if QWT_VERSION >= 0x060000
#include <qwt_series_data.h>
#else
#include <qwt_interval_data.h>
#endif
#include <qwt_painter.h>
#include <qwt_scale_map.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/StatsGen.hpp"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"
#include "DREAM3DLib/StatsData/StatsData.h"

#include "StatsGenerator/Presets/MicrostructurePresetManager.h"
#include "StatsGenerator/Presets/PrecipitateEquiaxedPreset.h"
#include "StatsGenerator/Presets/PrecipitateRolledPreset.h"


#define CHECK_ERROR_ON_WRITE(var, msg)\
    if (err < 0) {\
      QMessageBox::critical(this, tr("StatsGenerator"),\
      tr("There was an error writing the " msg " to the HDF5 file"),\
      QMessageBox::Ok,\
      QMessageBox::Ok);\
      return err;\
      }


#define CHECK_STATS_READ_ERROR(err, group, dataset)\
if (err < 0) {\
  qDebug() << "PrecipitatePhaseWidget::on_actionOpen_triggered Error: Could not read '" << group << "' data set '" << dataset << "'" << "\n";\
  qDebug() << "  File: " << __FILE__ << "\n";\
  qDebug() << "  Line: " << __LINE__ << "\n";\
  return err;\
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrecipitatePhaseWidget::PrecipitatePhaseWidget(QWidget *parent) :
SGWidget(parent),
m_PhaseType(DREAM3D::PhaseType::PrimaryPhase),
m_PhaseFraction(1.0),
m_TotalPhaseFraction(1.0),
m_PptFraction(-1.0f),
m_DataHasBeenGenerated(false),
m_PhaseIndex(0),
m_CrystalStructure(Ebsd::CrystalStructure::Cubic_High),
m_SizeDistributionCurve(NULL),
m_CutOffMin(NULL),
m_CutOffMax(NULL),
m_grid(NULL)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrecipitatePhaseWidget::~PrecipitatePhaseWidget()
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::on_microstructurePresetCombo_currentIndexChanged(int index)
{
  //qDebug() << "on_microstructurePresetCombo_currentIndexChanged" << "\n";
  QString presetName = microstructurePresetCombo->currentText();

  //Factory Method to get an instantiated object of the correct type?
  MicrostructurePresetManager::Pointer manager = MicrostructurePresetManager::instance();
  m_MicroPreset = manager->createNewPreset(presetName);
  m_MicroPreset->displayUserInputDialog();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
AbstractMicrostructurePresetFactory::Pointer RegisterPresetFactory(QComboBox* microstructurePresetCombo)
{
  AbstractMicrostructurePresetFactory::Pointer presetFactory = T::New();
  MicrostructurePresetManager::registerFactory(presetFactory);
  QString displayString = (presetFactory->displayName());
  microstructurePresetCombo->addItem(displayString);
  return presetFactory;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::setupGui()
{
  distributionTypeCombo->addItem(DREAM3D::HDF5::BetaDistribution.toLatin1().data());
  distributionTypeCombo->addItem(DREAM3D::HDF5::LogNormalDistribution.toLatin1().data());
  distributionTypeCombo->addItem(DREAM3D::HDF5::PowerLawDistribution.toLatin1().data());
  distributionTypeCombo->setCurrentIndex(DREAM3D::DistributionType::LogNormal);
  // Turn off all the plot widgets
  setTabsPlotTabsEnabled(false);

  microstructurePresetCombo->blockSignals(true);
  // Register all of our Microstructure Preset Factories
  AbstractMicrostructurePresetFactory::Pointer presetFactory = AbstractMicrostructurePresetFactory::NullPointer();

  //Register the Equiaxed Preset
  presetFactory = RegisterPresetFactory<PrecipitateEquiaxedPresetFactory>(microstructurePresetCombo);
  QString presetName = (presetFactory->displayName());
  MicrostructurePresetManager::Pointer manager = MicrostructurePresetManager::instance();
  m_MicroPreset = manager->createNewPreset(presetName);

  // Register the Rolled Preset
  presetFactory = RegisterPresetFactory<PrecipitateRolledPresetFactory>(microstructurePresetCombo);

  // Select the first Preset in the list
  microstructurePresetCombo->setCurrentIndex(0);
  microstructurePresetCombo->blockSignals(false);


  float mu = 1.0f;
  float sigma = 0.1f;
  float minCutOff = 5.0f;
  float maxCutOff = 5.0f;
  float binStepSize = 0.5f;

  StatsGenPlotWidget* w = m_Omega3Plot;

   w->setPlotTitle(QString("Size Vs. Omega 3"));
   w->setXAxisName(QString("Omega 3"));
   w->setYAxisName(QString("Frequency"));
   w->setDistributionType(DREAM3D::DistributionType::Beta);
   w->setStatisticsType(DREAM3D::StatisticsType::Feature_SizeVOmega3);
   w->blockDistributionTypeChanges(true);
   w->setRowOperationEnabled(false);
   w->setMu(mu);
   w->setSigma(sigma);
   w->setMinCutOff(minCutOff);
   w->setMaxCutOff(maxCutOff);
   w->setBinStep(binStepSize);
   connect(m_Omega3Plot, SIGNAL(userEditedData()),
           this, SLOT(dataWasEdited()));


   w = m_BOverAPlot;
   w->setPlotTitle(QString("B/A Shape Distribution"));
   w->setXAxisName(QString("B/A"));
   w->setYAxisName(QString("Frequency"));
   w->setDistributionType(DREAM3D::DistributionType::Beta);
   w->setStatisticsType(DREAM3D::StatisticsType::Feature_SizeVBoverA);
   w->blockDistributionTypeChanges(true);
   w->setRowOperationEnabled(false);
   w->setMu(mu);
   w->setSigma(sigma);
   w->setMinCutOff(minCutOff);
   w->setMaxCutOff(maxCutOff);
   w->setBinStep(binStepSize);
   connect(m_BOverAPlot, SIGNAL(userEditedData()),
           this, SLOT(dataWasEdited()));

   w = m_COverAPlot;
   w->setPlotTitle(QString("C/A Shape Distribution"));
   w->setXAxisName(QString("C/A"));
   w->setYAxisName(QString("Frequency"));
   w->setDistributionType(DREAM3D::DistributionType::Beta);
   w->setStatisticsType(DREAM3D::StatisticsType::Feature_SizeVCoverA);
   w->blockDistributionTypeChanges(true);
   w->setRowOperationEnabled(false);
   w->setMu(mu);
   w->setSigma(sigma);
   w->setMinCutOff(minCutOff);
   w->setMaxCutOff(maxCutOff);
   w->setBinStep(binStepSize);
   connect(m_COverAPlot, SIGNAL(userEditedData()),
           this, SLOT(dataWasEdited()));

   w = m_ClusteringPlot;
   w->setPlotTitle(QString("Clustering Distributions"));
   w->setXAxisName(QString("Distance between Centroids"));
   w->setYAxisName(QString("Frequency"));
   w->setDistributionType(DREAM3D::DistributionType::LogNormal);
   w->setStatisticsType(DREAM3D::StatisticsType::Feature_SizeVClustering);
   w->blockDistributionTypeChanges(true);
   w->setRowOperationEnabled(false);
   w->setMu(mu);
   w->setSigma(sigma);
   w->setMinCutOff(minCutOff);
   w->setMaxCutOff(maxCutOff);
   w->setBinStep(binStepSize);
   connect(m_ClusteringPlot, SIGNAL(userEditedData()),
           this, SLOT(dataWasEdited()));


  m_SizeDistributionPlot->setCanvasBackground(QColor(Qt::white));
  m_SizeDistributionPlot->setTitle("Size Distribution");

  m_grid = new QwtPlotGrid;
  m_grid->enableXMin(true);
  m_grid->enableYMin(true);
  m_grid->setMajPen(QPen(Qt::gray, 0, Qt::SolidLine));
  m_grid->setMinPen(QPen(Qt::lightGray, 0, Qt::DotLine));
  m_grid->attach(m_SizeDistributionPlot);

  // For the ODF Tab we want the MDF functionality
  m_ODFWidget->enableMDFTab(true);
  // Remove any Axis Decorations. The plots are explicitly know to have a -1 to 1 axis min/max
  m_ODFWidget->setEnableAxisDecorations(false);

  // Remove any Axis Decorations. The plots are explicitly know to have a -1 to 1 axis min/max
  m_AxisODFWidget->setEnableAxisDecorations(false);

  updateSizeDistributionPlot();
  calculateNumberOfBins();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::setPhaseIndex(int index)
{
  m_PhaseIndex = index;
  m_Omega3Plot->setPhaseIndex(m_PhaseIndex);
  m_BOverAPlot->setPhaseIndex(m_PhaseIndex);
  m_COverAPlot->setPhaseIndex(m_PhaseIndex);
  m_ClusteringPlot->setPhaseIndex(m_PhaseIndex);
  m_ODFWidget->setPhaseIndex(m_PhaseIndex);
  m_AxisODFWidget->setPhaseIndex(m_PhaseIndex);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PrecipitatePhaseWidget::getPhaseIndex()
{
  return m_PhaseIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::setCrystalStructure(unsigned int xtal)
{
  m_CrystalStructure = xtal;
  m_Omega3Plot->setCrystalStructure(xtal);
  m_BOverAPlot->setCrystalStructure(xtal);
  m_COverAPlot->setCrystalStructure(xtal);
  m_ClusteringPlot->setCrystalStructure(xtal);
  m_ODFWidget->setCrystalStructure(xtal);
  /* Note that we do NOT want to set the crystal structure for the AxisODF widget
   * because we need that crystal structure to be OrthoRhombic in order for those
   * calculations to be performed correctly */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int PrecipitatePhaseWidget::getCrystalStructure()
{
  return m_CrystalStructure;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PrecipitatePhaseWidget::getComboString()
{
  QString s = QString::number(m_PhaseIndex);
  s.append(" - ");
  if ( Ebsd::CrystalStructure::Cubic_High == m_CrystalStructure)
  {
    s.append("Cubic");
  }
  else if ( Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructure)
  {
    s.append("Hexagonal");
  }
  return s;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PrecipitatePhaseWidget::gatherSizeDistributionFromGui(float &mu, float &sigma, float &minCutOff, float &maxCutOff, float &stepSize)
{
  bool ok = false;
  mu = m_Mu_SizeDistribution->text().toFloat(&ok);
  if (ok == false)
  {
    return 0;
  }
  sigma = m_Sigma_SizeDistribution->text().toFloat(&ok);
  if (ok == false)
  {
    return 0;
  }
  minCutOff = m_MinSigmaCutOff->text().toFloat(&ok);
  if (ok == false)
  {
    return 0;
  }

  maxCutOff = m_MaxSigmaCutOff->text().toFloat(&ok);
   if (ok == false)
   {
     return 0;
   }

  stepSize = m_BinStepSize->text().toFloat(&ok);
  if (ok == false)
  {
    return 0;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::setTabsPlotTabsEnabled(bool b)
{
  qint32 count = this->tabWidget->count();
  for (qint32 i = 1; i < count; ++i)
  {
    this->tabWidget->setTabEnabled(i, b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::dataWasEdited()
{
  setTabsPlotTabsEnabled(true);
  this->tabWidget->setTabEnabled(0, false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList)
    {
      w->setEnabled(b);
    }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::updatePlots()
{
  if (m_DataHasBeenGenerated == true)
  {
    plotSizeDistribution();
    m_ODFWidget->updatePlots();
    m_AxisODFWidget->updatePlots();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::on_m_GenerateDefaultData_clicked()
{
  m_DataHasBeenGenerated = true;
  updatePlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::on_m_Mu_SizeDistribution_textChanged(const QString &text)
{
  updateSizeDistributionPlot();
  m_Mu_SizeDistribution->setFocus();
  calculateNumberOfBins();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::on_m_Sigma_SizeDistribution_textChanged(const QString &text)
{
  updateSizeDistributionPlot();
  m_Sigma_SizeDistribution->setFocus();
  calculateNumberOfBins();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::on_m_MinSigmaCutOff_textChanged(const QString &text)
{
  updateSizeDistributionPlot();
  m_MinSigmaCutOff->setFocus();
  calculateNumberOfBins();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::on_m_MaxSigmaCutOff_textChanged(const QString &text)
{
  updateSizeDistributionPlot();
  m_MaxSigmaCutOff->setFocus();
  calculateNumberOfBins();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::on_m_BinStepSize_valueChanged(double v)
{
  calculateNumberOfBins();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::calculateNumberOfBins()
{
  float mu = 1.0;
  float sigma = 1.0;
  float minCutOff = 1.0;
  float maxCutOff = 1.0;
  float stepSize = 1.0;
  float max, min;
  int err = gatherSizeDistributionFromGui(mu, sigma, minCutOff, maxCutOff, stepSize);
  if (err == 0)
  {
    return;
  }

  
  int n = StatsGen::ComputeNumberOfBins(mu, sigma, minCutOff, maxCutOff, stepSize, max, min);
  m_NumberBinsGenerated->setText(QString::number(n));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PrecipitatePhaseWidget::calculateNumberOfBins(float mu, float sigma, float minCutOff, float maxCutOff, float stepSize)
{
  float max, min; // Only needed for the method. Not used otherwise.
  
  return StatsGen::ComputeNumberOfBins(mu, sigma, minCutOff, maxCutOff, stepSize, max, min);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PrecipitatePhaseWidget::computeBinsAndCutOffs( float mu, float sigma,
                                     float minCutOff, float maxCutOff,
                                     float binStepSize,
                                     QwtArray<float> &binsizes,
                                     QwtArray<float> &xCo,
                                     QwtArray<float> &yCo,
                                     float &xMax, float &yMax,
                                     QwtArray<float> &x,
                                     QwtArray<float> &y)
{
  int err = 0;
  int size = 250;

  
  err = StatsGen::GenLogNormalPlotData<QwtArray<float> > (mu, sigma, x, y, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return -1;
  }

//  float xMax = std::numeric_limits<float >::min();
//  float yMax = std::numeric_limits<float >::min();
  for (int i = 0; i < size; ++i)
  {
    //   qDebug() << x[i] << "  " << y[i] << "\n";
    if (x[i] > xMax)
    {
      xMax = x[i];
    }
    if (y[i] > yMax)
    {
      yMax = y[i];
    }
  }

  xCo.clear();
  yCo.clear();
  int numsizebins = 1;
  binsizes.clear();
  // QwtArray<int> numfeatures;
  err = StatsGen::GenCutOff<float, QwtArray<float> > (mu, sigma, minCutOff, maxCutOff, binStepSize, xCo, yCo, yMax, numsizebins, binsizes);

  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::updateSizeDistributionPlot()
{

  float mu = 1.0;
  float sigma = 1.0;
  float minCutOff = 1.0;
  float maxCutOff = 1.0;
  float stepSize = 1.0;
  int err = gatherSizeDistributionFromGui(mu, sigma, minCutOff, maxCutOff, stepSize);
  if (err == 0)
  {
    return;
  }

  QwtArray<float> xCo;
  QwtArray<float> yCo;
  QwtArray<float> binsizes;
  float xMax = std::numeric_limits<float >::min();
  float yMax = std::numeric_limits<float >::min();
  QwtArray<float> x;
  QwtArray<float> y;
  err = computeBinsAndCutOffs(mu, sigma, minCutOff, maxCutOff, stepSize, binsizes, xCo, yCo, xMax, yMax, x, y);
  if (err < 0) { return; }

  if (NULL == m_SizeDistributionCurve)
  {
    m_SizeDistributionCurve = new QwtPlotCurve("Size Distribution");
    m_SizeDistributionCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    m_SizeDistributionCurve->setPen(QPen(Qt::red));
    m_SizeDistributionCurve->attach(m_SizeDistributionPlot);
  }

  // Place a vertical Line on the plot where the Min and Max Cutoff values are
  if (NULL == m_CutOffMin)
  {
    m_CutOffMin = new QwtPlotMarker();
    m_CutOffMin->attach(m_SizeDistributionPlot);
  }
  m_CutOffMin->setLabel(QString::fromLatin1("Cut Off Min Feature Diameter"));
  m_CutOffMin->setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
  m_CutOffMin->setLabelOrientation(Qt::Vertical);
  m_CutOffMin->setLineStyle(QwtPlotMarker::VLine);
  m_CutOffMin->setLinePen(QPen(Qt::blue, 1, Qt::SolidLine));
  m_CutOffMin->setXValue(xCo[0]);

  if (NULL == m_CutOffMax)
  {
    m_CutOffMax = new QwtPlotMarker();
    m_CutOffMax->attach(m_SizeDistributionPlot);
  }
  m_CutOffMax->setLabel(QString::fromLatin1("Cut Off Max Feature Diameter"));
  m_CutOffMax->setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
  m_CutOffMax->setLabelOrientation(Qt::Vertical);
  m_CutOffMax->setLineStyle(QwtPlotMarker::VLine);
  m_CutOffMax->setLinePen(QPen(Qt::blue, 1, Qt::SolidLine));
  m_CutOffMax->setXValue(xCo[1]);

  QwtArray<double> xD(x.size());
  QwtArray<double> yD(x.size());
  for (int i = 0; i < x.size(); ++i)
  {
    xD[i] = static_cast<double>(x[i]);
    yD[i] = static_cast<double>(y[i]);
  }

#if QWT_VERSION >= 0x060000
  m_SizeDistributionCurve->setSamples(xD, yD);
#else
  m_SizeDistributionCurve->setData(xD, yD);
#endif

  m_SizeDistributionPlot->setAxisScale(QwtPlot::xBottom, xCo[0] - (xCo[0] * 0.1), xMax * 1.10);
  m_SizeDistributionPlot->setAxisScale(QwtPlot::yLeft, 0.0, yMax);

  m_SizeDistributionPlot->replot();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::plotSizeDistribution()
{
  // We have valid data so enable the other plot tabs
  setTabsPlotTabsEnabled(true);

  float mu = 1.0;
  float sigma = 1.0;
  float minCutOff = 1.0;
  float maxCutOff = 1.0;
  float stepSize = 1.0;
  int err = gatherSizeDistributionFromGui(mu, sigma, minCutOff, maxCutOff, stepSize);
  if (err == 0)
  {
    return;
  }

  QwtArray<float> xCo;
  QwtArray<float> yCo;
  QwtArray<float> binsizes;
  float xMax = std::numeric_limits<float >::min();
  float yMax = std::numeric_limits<float >::min();
  QwtArray<float> x;
  QwtArray<float> y;
  err = computeBinsAndCutOffs(mu, sigma, minCutOff, maxCutOff, stepSize, binsizes, xCo, yCo, xMax, yMax, x, y);
  if (err < 0) { return; }

  // Now that we have bins and feature sizes, push those to the other plot widgets
  // Setup Each Plot Widget
  // The MicroPreset class will set the distribution for each of the plots
  m_Omega3Plot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, stepSize);
  m_MicroPreset->initializeOmega3TableModel(m_Omega3Plot, binsizes);

  m_BOverAPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, stepSize);
  m_MicroPreset->initializeBOverATableModel(m_BOverAPlot, binsizes);

  m_COverAPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, stepSize);
  m_MicroPreset->initializeCOverATableModel(m_COverAPlot, binsizes);

  m_ClusteringPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, stepSize);
  m_MicroPreset->initializeClusteringTableModel(m_ClusteringPlot, binsizes);

  // Get any presets for the ODF/AxisODF/MDF also
  m_MicroPreset->initializeODFTableModel(m_ODFWidget);
  m_MicroPreset->initializeAxisODFTableModel(m_AxisODFWidget);
  m_MicroPreset->initializeMDFTableModel(m_ODFWidget->getMDFWidget());

}


#define SGWIGET_WRITE_ERROR_CHECK(var)\
    if (err < 0)  {\
      QString msg ("Error Writing Data ");\
      msg.append((var));\
      msg.append(" to the HDF5 file");\
      QMessageBox::critical(this, tr("StatsGenerator"),\
                                    msg,\
                                    QMessageBox::Default);\
      retErr = -1;\
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PrecipitatePhaseWidget::gatherStatsData(VolumeDataContainer::Pointer m)
{
  if (m_PhaseIndex < 1)
  {
    QMessageBox::critical(this, tr("StatsGenerator"),
                                  tr("The Phase Index is Less than 1. This is not allowed."),
                                  QMessageBox::Default);
    return -1;
  }
  int retErr = 0;
  int err = 0;
  float mu = 1.0f;
  float sigma = 1.0f;
  float minCutOff = 1.0f;
  float maxCutOff = 1.0f;
 // float stepSize = 1.0f;
  float binStep = 1.0f;
  gatherSizeDistributionFromGui(mu, sigma, minCutOff, maxCutOff, binStep);
  float calcPhaseFraction = m_PhaseFraction / m_TotalPhaseFraction;

  QwtArray<float> xCo;
  QwtArray<float> yCo;
  QwtArray<float> binsizes;
  float xMax = std::numeric_limits<float>::min();
  float yMax = std::numeric_limits<float>::min();
  QwtArray<float> x;
  QwtArray<float> y;
  err = computeBinsAndCutOffs(mu, sigma, minCutOff, maxCutOff, binStep, binsizes, xCo, yCo, xMax, yMax, x, y);
  if (err < 0)
  {
    return err;
  }

  // We need to compute the Max and Min Diameter Bin Values
  float mindiameter = xCo[0];
  float maxdiameter = xCo[1];
  float avglogdiam = mu;
  float sdlogdiam = sigma;
  float stepSize = binStep;

 // size_t nBins = 0;

  typedef DataArray<unsigned int> XTalStructArrayType;
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  typedef DataArray<unsigned int> ShapeTypeArrayType;
  size_t ensembles = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  // Get pointers
  unsigned int* crystalStructures = m->getCellEnsembleDataSizeCheck<unsigned int, XTalStructArrayType, AbstractFilter>(DREAM3D::EnsembleData::CrystalStructures, ensembles, 1, NULL);
  unsigned int* phaseTypes = m->getCellEnsembleDataSizeCheck<unsigned int, PhaseTypeArrayType, AbstractFilter>(DREAM3D::EnsembleData::PhaseTypes, ensembles, 1, NULL);

  //unsigned int* shapeTypes = m->getCellEnsembleDataSizeCheck<unsigned int, ShapeTypeArrayType, AbstractFilter>(DREAM3D::EnsembleData::ShapeTypes, ensembles*1, NULL);

  crystalStructures[m_PhaseIndex] = m_CrystalStructure;
  phaseTypes[m_PhaseIndex] = m_PhaseType;

  StatsDataArray* statsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(m->getCellEnsembleData(DREAM3D::EnsembleData::Statistics).get());
  StatsData::Pointer statsData = statsDataArray->getStatsData(m_PhaseIndex);
  PrecipitateStatsData* precipitateStatsData = PrecipitateStatsData::SafePointerDownCast(statsData.get());

  //H5StatsWriter::Pointer writer = H5StatsWriter::New();

  precipitateStatsData->setPhaseFraction(calcPhaseFraction);
  precipitateStatsData->setPrecipBoundaryFraction(m_PptFraction);
  // Feature Diameter Info
  precipitateStatsData->setBinStepSize(stepSize);
  precipitateStatsData->setMaxFeatureDiameter(maxdiameter);
  precipitateStatsData->setMinFeatureDiameter(mindiameter);
  // Feature Size Distribution
  {
    VectorOfFloatArray data;
    FloatArrayType::Pointer d1 = FloatArrayType::CreateArray(1, DREAM3D::HDF5::Average);
    FloatArrayType::Pointer d2 = FloatArrayType::CreateArray(1, DREAM3D::HDF5::StandardDeviation);
    data.push_back(d1);
    data.push_back(d2);
    d1->SetValue(0, avglogdiam);
    d2->SetValue(0, sdlogdiam);
    precipitateStatsData->setFeatureSizeDistribution(data);
    precipitateStatsData->setFeatureSize_DistType(DREAM3D::DistributionType::LogNormal);
  }

  // Now that we have bins and feature sizes, push those to the other plot widgets

  //err = m_Omega3Plot->writeDataToHDF5(m, DREAM3D::HDF5::Feature_SizeVOmega3_Distributions);
  {
    VectorOfFloatArray data = m_Omega3Plot->getStatisticsData();
    precipitateStatsData->setFeatureSize_Omegas(data);
    precipitateStatsData->setOmegas_DistType(m_Omega3Plot->getDistributionType());
  }

  //err = m_BOverAPlot->writeDataToHDF5(writer, DREAM3D::HDF5::Feature_SizeVBoverA_Distributions);
  {
    VectorOfFloatArray data = m_BOverAPlot->getStatisticsData();
    precipitateStatsData->setFeatureSize_BOverA(data);
    precipitateStatsData->setBOverA_DistType(m_BOverAPlot->getDistributionType());
  }

  //err = m_COverAPlot->writeDataToHDF5(writer, DREAM3D::HDF5::Feature_SizeVCoverA_Distributions);
  {
    VectorOfFloatArray data = m_COverAPlot->getStatisticsData();
    precipitateStatsData->setFeatureSize_COverA(data);
    precipitateStatsData->setCOverA_DistType(m_COverAPlot->getDistributionType());
  }

 // err = m_ClusteringPlot->writeDataToHDF5(writer, DREAM3D::HDF5::Feature_SizeVClustering_Distributions);
  {
    VectorOfFloatArray data = m_ClusteringPlot->getStatisticsData();
    precipitateStatsData->setFeatureSize_Clustering(data);
    precipitateStatsData->setClustering_DistType(m_ClusteringPlot->getDistributionType());
  }


  //err = m_ODFWidget->writeDataToHDF5(writer);
  m_ODFWidget->getOrientationData(precipitateStatsData, DREAM3D::PhaseType::PrecipitatePhase);


  err = m_AxisODFWidget->getOrientationData(precipitateStatsData, DREAM3D::PhaseType::PrecipitatePhase);

  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::extractStatsData(VolumeDataContainer::Pointer m, int index)
{
  setWidgetListEnabled(true);
  float mu = 1.0f;
  float sigma = 1.0f;
  float minCutOff = 5.0f;
  float maxCutOff = 5.0f;
  float binStepSize, maxFeatureSize, minFeatureSize;

  setPhaseIndex(index);

  IDataArray* iDataPtr = NULL;

  iDataPtr = m->getCellEnsembleData(DREAM3D::EnsembleData::CrystalStructures).get();
  UInt32ArrayType* data = UInt32ArrayType::SafeObjectDownCast<IDataArray*, UInt32ArrayType*>(iDataPtr);
  m_CrystalStructure = data->GetValue(index);

  iDataPtr = m->getCellEnsembleData(DREAM3D::EnsembleData::PhaseTypes).get();
  data = UInt32ArrayType::SafeObjectDownCast<IDataArray*, UInt32ArrayType*>(iDataPtr);
  m_PhaseType = data->GetValue(index);

  iDataPtr = m->getCellEnsembleData(DREAM3D::EnsembleData::Statistics).get();
  StatsDataArray* statsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(iDataPtr);
  if (statsDataArray == NULL)
  {
    return;
  }
  StatsData::Pointer statsData = statsDataArray->getStatsData(index);
  PrecipitateStatsData* precipitateStatsData = PrecipitateStatsData::SafePointerDownCast(statsData.get());

  m_PhaseFraction = precipitateStatsData->getPhaseFraction();

  m_PptFraction = precipitateStatsData->getPrecipBoundaryFraction();

  m_Omega3Plot->setCrystalStructure(m_CrystalStructure);
  m_BOverAPlot->setCrystalStructure(m_CrystalStructure);
  m_COverAPlot->setCrystalStructure(m_CrystalStructure);
  m_ClusteringPlot->setCrystalStructure(m_CrystalStructure);
  m_ODFWidget->setCrystalStructure(m_CrystalStructure);
 // m_AxisODFWidget->setCrystalStructure(m_CrystalStructure);


  /* SEt the BinNumbers data set */
  FloatArrayType::Pointer bins = precipitateStatsData->getBinNumbers();


  /* Set the Feature_Diameter_Info Data */

  binStepSize = precipitateStatsData->getBinStepSize();
  m_BinStepSize->blockSignals(true);
  m_BinStepSize->setValue(binStepSize);
  m_BinStepSize->blockSignals(false);
  maxFeatureSize = precipitateStatsData->getMaxFeatureDiameter();
  minFeatureSize = precipitateStatsData->getMinFeatureDiameter();

  /* Set the Feature_Size_Distribution Data */
  VectorOfFloatArray distData = precipitateStatsData->getFeatureSizeDistribution();
  mu = distData[0]->GetValue(0);
  sigma = distData[1]->GetValue(0);
  m_Mu_SizeDistribution->blockSignals(true);
  m_Sigma_SizeDistribution->blockSignals(true);

  m_Mu_SizeDistribution->setText(QString::number(mu));
  m_Sigma_SizeDistribution->setText(QString::number(sigma));

  m_Mu_SizeDistribution->blockSignals(false);
  m_Sigma_SizeDistribution->blockSignals(false);

  minCutOff = (mu - log(minFeatureSize))/sigma;
  maxCutOff = (log(maxFeatureSize) - mu)/sigma;

  m_MinSigmaCutOff->blockSignals(true);
  m_MinSigmaCutOff->setText(QString::number(minCutOff));
  m_MinSigmaCutOff->blockSignals(false);

  m_MaxSigmaCutOff->blockSignals(true);
  m_MaxSigmaCutOff->setText(QString::number(maxCutOff));
  m_MaxSigmaCutOff->blockSignals(false);

  // Update the Size/Weights Plot
  updateSizeDistributionPlot();
  m_NumberBinsGenerated->setText(QString::number(bins->getNumberOfTuples()));

  // Now have each of the plots set it's own data
  QVector<float> qbins (static_cast<int>(bins->getNumberOfTuples()));
  for(int i = 0; i < qbins.size(); ++i)
  {
    qbins[i] = bins->GetValue(i);
  }

  m_Omega3Plot->setDistributionType(precipitateStatsData->getOmegas_DistType(), false);
  m_Omega3Plot->extractStatsData(m, index, qbins, precipitateStatsData->getFeatureSize_Omegas());
  m_Omega3Plot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  m_BOverAPlot->setDistributionType(precipitateStatsData->getBOverA_DistType(), false);
  m_BOverAPlot->extractStatsData(m, index, qbins, precipitateStatsData->getFeatureSize_BOverA());
  m_BOverAPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  m_COverAPlot->setDistributionType(precipitateStatsData->getCOverA_DistType(), false);
  m_COverAPlot->extractStatsData(m, index, qbins, precipitateStatsData->getFeatureSize_COverA());
  m_COverAPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  m_ClusteringPlot->setDistributionType(precipitateStatsData->getClustering_DistType(), false);
  m_ClusteringPlot->extractStatsData(m, index, qbins, precipitateStatsData->getFeatureSize_Clustering());
  m_ClusteringPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);


  // Set the ODF Data
  m_ODFWidget->extractStatsData(m, index, precipitateStatsData, DREAM3D::PhaseType::PrecipitatePhase);

  // Set the Axis ODF Data
  m_AxisODFWidget->extractStatsData(m, index, precipitateStatsData, DREAM3D::PhaseType::PrecipitatePhase);

  // Enable all the tabs
  setTabsPlotTabsEnabled(true);
  m_DataHasBeenGenerated = true;

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PrecipitatePhaseWidget::getTabTitle()
{
  return QString("Precipitate");
}

