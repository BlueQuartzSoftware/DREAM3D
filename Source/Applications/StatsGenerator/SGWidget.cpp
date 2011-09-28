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
#include "SGWidget.h"


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
#include <qwt_interval_data.h>
#include <qwt_painter.h>
#include <qwt_scale_map.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/HDF5/H5StatsWriter.h"

#include "StatsGenerator/Presets/MicrostructurePresetManager.h"
#include "StatsGenerator/Presets/DefaultStatsPreset.h"
#include "StatsGenerator/Presets/EquiaxedPreset.h"
#include "StatsGenerator/Presets/RolledPreset.h"
#include "StatsGenerator/Presets/RecrystallizedPreset.h"
#include "StatsGen.h"

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
  std::cout << "SGWidget::on_actionOpen_triggered Error: Could not read '" << group << "' data set '" << dataset << "'" << std::endl;\
  std::cout << "  File: " << __FILE__ << std::endl;\
  std::cout << "  Line: " << __LINE__ << std::endl;\
  return err;\
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGWidget::SGWidget(QWidget *parent) :
QWidget(parent),
m_PhaseType(DREAM3D::Reconstruction::PrimaryPhase),
m_PhaseFraction(1.0),
m_TotalPhaseFraction(1.0),
m_PptFraction(-1.0f),
m_DataHasBeenGenerated(false),
m_PhaseIndex(0),
m_CrystalStructure(Ebsd::Cubic),
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
SGWidget::~SGWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGWidget::on_microstructurePresetCombo_currentIndexChanged(int index)
{
  //std::cout << "on_microstructurePresetCombo_currentIndexChanged" << std::endl;
  QString presetName = microstructurePresetCombo->currentText();

  //Factory Method to get an instantiated object of the correct type?
  MicrostructurePresetManager::Pointer manager = MicrostructurePresetManager::instance();
  m_MicroPreset = manager->createNewPreset(presetName.toStdString());
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
  QString displayString = QString::fromStdString(presetFactory->displayName());
  microstructurePresetCombo->addItem(displayString);
  return presetFactory;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGWidget::setupGui()
{
  distributionTypeCombo->addItem(DREAM3D::HDF5::BetaDistribution.c_str());
  distributionTypeCombo->addItem(DREAM3D::HDF5::LogNormalDistribution.c_str());
  distributionTypeCombo->addItem(DREAM3D::HDF5::PowerLawDistribution.c_str());
  distributionTypeCombo->setCurrentIndex(DREAM3D::Reconstruction::LogNormal);
  // Turn off all the plot widgets
  setTabsPlotTabsEnabled(false);

  microstructurePresetCombo->blockSignals(true);
  // Register all of our Microstructure Preset Factories
  AbstractMicrostructurePresetFactory::Pointer presetFactory = RegisterPresetFactory<DefaultStatsPresetFactory>(microstructurePresetCombo);
  QString presetName = QString::fromStdString(presetFactory->displayName());
  MicrostructurePresetManager::Pointer manager = MicrostructurePresetManager::instance();
  m_MicroPreset = manager->createNewPreset(presetName.toStdString());

  //Register the Equiaxed Preset
  presetFactory = RegisterPresetFactory<EquiaxedPresetFactory>(microstructurePresetCombo);
  // Register the Rolled Preset
  presetFactory = RegisterPresetFactory<RolledPresetFactory>(microstructurePresetCombo);
  // Register the Recrystallized Preset
//  presetFactory = RegisterPresetFactory<RecrystallizedPresetFactory>(microstructurePresetCombo);

  // Select the first Preset in the list
  microstructurePresetCombo->setCurrentIndex(0);
  microstructurePresetCombo->blockSignals(false);


  float mu = 1.0;
  float sigma = 0.1;
  float minCutOff = 5;
  float maxCutOff = 5;
  float binStepSize = 0.5;

  StatsGenPlotWidget* w = m_Omega3Plot;

   w->setPlotTitle(QString("Size Vs. Omega 3"));
   w->setXAxisName(QString("Omega 3"));
   w->setYAxisName(QString("Frequency"));
   w->setDistributionType(DREAM3D::Reconstruction::Beta);
   w->setStatisticsType(DREAM3D::Reconstruction::Grain_SizeVOmega3);
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
   w->setDistributionType(DREAM3D::Reconstruction::Beta);
   w->setStatisticsType(DREAM3D::Reconstruction::Grain_SizeVBoverA);
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
   w->setDistributionType(DREAM3D::Reconstruction::Beta);
   w->setStatisticsType(DREAM3D::Reconstruction::Grain_SizeVCoverA);
   w->blockDistributionTypeChanges(true);
   w->setRowOperationEnabled(false);
   w->setMu(mu);
   w->setSigma(sigma);
   w->setMinCutOff(minCutOff);
   w->setMaxCutOff(maxCutOff);
   w->setBinStep(binStepSize);
   connect(m_COverAPlot, SIGNAL(userEditedData()),
           this, SLOT(dataWasEdited()));

   w = m_COverBPlot;
   w->setPlotTitle(QString("C/B Shape Distribution"));
   w->setXAxisName(QString("C/B"));
   w->setYAxisName(QString("Frequency"));
   w->setDistributionType(DREAM3D::Reconstruction::Beta);
   w->setStatisticsType(DREAM3D::Reconstruction::Grain_SizeVCoverB);
   w->blockDistributionTypeChanges(true);
   w->setRowOperationEnabled(false);
   w->setMu(mu);
   w->setSigma(sigma);
   w->setMinCutOff(minCutOff);
   w->setMaxCutOff(maxCutOff);
   w->setBinStep(binStepSize);
   connect(m_COverBPlot, SIGNAL(userEditedData()),
           this, SLOT(dataWasEdited()));

   w = m_NeighborPlot;
   w->setPlotTitle(QString("Neighbors Distributions"));
   w->setXAxisName(QString("Distance (Multiples of Diameter)"));
   w->setYAxisName(QString("Number of Grains"));
   w->setDistributionType(DREAM3D::Reconstruction::Power);
   w->setStatisticsType(DREAM3D::Reconstruction::Grain_SizeVNeighbors);
   w->blockDistributionTypeChanges(true);
   w->setRowOperationEnabled(false);
   w->setMu(mu);
   w->setSigma(sigma);
   w->setMinCutOff(minCutOff);
   w->setMaxCutOff(maxCutOff);
   w->setBinStep(binStepSize);
   connect(m_NeighborPlot, SIGNAL(userEditedData()),
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
void SGWidget::setPhaseIndex(int index)
{
  m_PhaseIndex = index;
  m_Omega3Plot->setPhaseIndex(m_PhaseIndex);
  m_BOverAPlot->setPhaseIndex(m_PhaseIndex);
  m_COverAPlot->setPhaseIndex(m_PhaseIndex);
  m_COverBPlot->setPhaseIndex(m_PhaseIndex);
  m_NeighborPlot->setPhaseIndex(m_PhaseIndex);
  m_ODFWidget->setPhaseIndex(m_PhaseIndex);
  m_AxisODFWidget->setPhaseIndex(m_PhaseIndex);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGWidget::getPhaseIndex()
{
  return m_PhaseIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGWidget::setCrystalStructure(Ebsd::CrystalStructure xtal)
{
  m_CrystalStructure = xtal;
  m_Omega3Plot->setCrystalStructure(xtal);
  m_BOverAPlot->setCrystalStructure(xtal);
  m_COverAPlot->setCrystalStructure(xtal);
  m_COverBPlot->setCrystalStructure(xtal);
  m_NeighborPlot->setCrystalStructure(xtal);
  m_ODFWidget->setCrystalStructure(xtal);
  /* Note that we do NOT want to set the crystal structure for the AxisODF widget
   * because we need that crystal structure to be OrthoRhombic in order for those
   * calculations to be performed correctly */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::CrystalStructure SGWidget::getCrystalStructure()
{
  return m_CrystalStructure;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SGWidget::getComboString()
{
  QString s = QString::number(m_PhaseIndex);
  s.append(" - ");
  if (m_CrystalStructure == Ebsd::Cubic)
  {
    s.append("Cubic");
  }
  else if (m_CrystalStructure == Ebsd::Hexagonal)
  {
    s.append("Hexagonal");
  }
  return s;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGWidget::gatherSizeDistributionFromGui(float &mu, float &sigma, float &minCutOff, float &maxCutOff, float &stepSize)
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
void SGWidget::setTabsPlotTabsEnabled(bool b)
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
void SGWidget::dataWasEdited()
{
  setTabsPlotTabsEnabled(true);
  this->tabWidget->setTabEnabled(0, false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList)
    {
      w->setEnabled(b);
    }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGWidget::updatePlots()
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
void SGWidget::on_m_GenerateDefaultData_clicked()
{
  m_DataHasBeenGenerated = true;
  updatePlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGWidget::on_m_Mu_SizeDistribution_textChanged(const QString &text)
{
  updateSizeDistributionPlot();
  m_Mu_SizeDistribution->setFocus();
  calculateNumberOfBins();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGWidget::on_m_Sigma_SizeDistribution_textChanged(const QString &text)
{
  updateSizeDistributionPlot();
  m_Sigma_SizeDistribution->setFocus();
  calculateNumberOfBins();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGWidget::on_m_MinSigmaCutOff_textChanged(const QString &text)
{
  updateSizeDistributionPlot();
  m_MinSigmaCutOff->setFocus();
  calculateNumberOfBins();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGWidget::on_m_MaxSigmaCutOff_textChanged(const QString &text)
{
  updateSizeDistributionPlot();
  m_MaxSigmaCutOff->setFocus();
  calculateNumberOfBins();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGWidget::on_m_BinStepSize_valueChanged(double v)
{
  calculateNumberOfBins();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGWidget::calculateNumberOfBins()
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

  StatsGen sg;
  int n = sg.computeNumberOfBins(mu, sigma, minCutOff, maxCutOff, stepSize, max, min);
  m_NumberBinsGenerated->setText(QString::number(n));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGWidget::calculateNumberOfBins(float mu, float sigma, float minCutOff, float maxCutOff, float stepSize)
{
  float max, min; // Only needed for the method. Not used otherwise.
  StatsGen sg;
  return sg.computeNumberOfBins(mu, sigma, minCutOff, maxCutOff, stepSize, max, min);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGWidget::computeBinsAndCutOffs( float mu, float sigma,
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

  StatsGen sg;
  err = sg.GenLogNormalPlotData<QwtArray<float> > (mu, sigma, x, y, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return -1;
  }

//  float xMax = std::numeric_limits<float >::min();
//  float yMax = std::numeric_limits<float >::min();
  for (int i = 0; i < size; ++i)
  {
    //   std::cout << x[i] << "  " << y[i] << std::endl;
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
  // QwtArray<int> numgrains;
  err = sg.GenCutOff<float, QwtArray<float> > (mu, sigma, minCutOff, maxCutOff, binStepSize, xCo, yCo, yMax, numsizebins, binsizes);

  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGWidget::updateSizeDistributionPlot()
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
  m_CutOffMin->setLabel(QString::fromLatin1("Cut Off Min Grain Diameter"));
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
  m_CutOffMax->setLabel(QString::fromLatin1("Cut Off Max Grain Diameter"));
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

  m_SizeDistributionCurve->setData(xD, yD);

  m_SizeDistributionPlot->setAxisScale(QwtPlot::xBottom, xCo[0] - (xCo[0] * 0.1), xMax * 1.10);
  m_SizeDistributionPlot->setAxisScale(QwtPlot::yLeft, 0.0, yMax);

  m_SizeDistributionPlot->replot();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGWidget::plotSizeDistribution()
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

  // Now that we have bins and grain sizes, push those to the other plot widgets
  // Setup Each Plot Widget
  // The MicroPreset class will set the distribution for each of the plots
  m_Omega3Plot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, stepSize);
  m_MicroPreset->initializeOmega3TableModel(m_Omega3Plot, binsizes);

  m_BOverAPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, stepSize);
  m_MicroPreset->initializeBOverATableModel(m_BOverAPlot, binsizes);

  m_COverAPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, stepSize);
  m_MicroPreset->initializeCOverATableModel(m_COverAPlot, binsizes);

  m_COverBPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, stepSize);
  m_MicroPreset->initializeCOverBTableModel(m_COverBPlot, binsizes);

  m_NeighborPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, stepSize);
  m_MicroPreset->initializeNeighborTableModel(m_NeighborPlot, binsizes);

  // Get any presets for the ODF/AxisODF/MDF also
  m_MicroPreset->initializeODFTableModel(m_ODFWidget);
  m_MicroPreset->initializeAxisODFTableModel(m_AxisODFWidget);
  m_MicroPreset->initializeMDFTableModel(m_ODFWidget->getMDFWidget());

}


#define SGWIGET_WRITE_ERROR_CHECK(var)\
    if (err < 0)  {\
      QString msg ("Error Writing Data ");\
      msg.append(QString::fromStdString(var));\
      msg.append(" to the HDF5 file");\
      QMessageBox::critical(this, tr("StatsGenerator"),\
                                    msg,\
                                    QMessageBox::Default);\
      retErr = -1;\
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGWidget::writeDataToHDF5(H5StatsWriter::Pointer writer)
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

  size_t nBins = 0;

  err = writer->writePhaseInformation(m_PhaseIndex, m_PhaseType, m_CrystalStructure, calcPhaseFraction, m_PptFraction);
  CHECK_ERROR_ON_WRITE(err, "PhaseInformation")
  err = writer->writeSizeDistribution(m_PhaseIndex, maxdiameter, mindiameter, stepSize, avglogdiam, sdlogdiam, nBins);
  CHECK_ERROR_ON_WRITE(err, "Size Distribution")

  // Now that we have bins and grain sizes, push those to the other plot widgets
  // Setup Each Plot Widget
  err = m_Omega3Plot->writeDataToHDF5(writer, DREAM3D::HDF5::Grain_SizeVOmega3_Distributions);
  SGWIGET_WRITE_ERROR_CHECK(DREAM3D::HDF5::Grain_SizeVOmega3_Distributions)
  err = m_BOverAPlot->writeDataToHDF5(writer, DREAM3D::HDF5::Grain_SizeVBoverA_Distributions);
  SGWIGET_WRITE_ERROR_CHECK(DREAM3D::HDF5::Grain_SizeVBoverA_Distributions)
  err = m_COverAPlot->writeDataToHDF5(writer, DREAM3D::HDF5::Grain_SizeVCoverA_Distributions);
  SGWIGET_WRITE_ERROR_CHECK(DREAM3D::HDF5::Grain_SizeVCoverA_Distributions)
  err = m_COverBPlot->writeDataToHDF5(writer, DREAM3D::HDF5::Grain_SizeVCoverB_Distributions);
  SGWIGET_WRITE_ERROR_CHECK(DREAM3D::HDF5::Grain_SizeVCoverB_Distributions)
  err = m_NeighborPlot->writeDataToHDF5(writer, DREAM3D::HDF5::Grain_SizeVNeighbors_Distributions);
  SGWIGET_WRITE_ERROR_CHECK(DREAM3D::HDF5::Grain_SizeVNeighbors_Distributions)
  err = m_ODFWidget->writeDataToHDF5(writer);
  SGWIGET_WRITE_ERROR_CHECK(std::string("ODF Data"));
  err = m_AxisODFWidget->writeDataToHDF5(writer);
  SGWIGET_WRITE_ERROR_CHECK(std::string("Axis ODF Data"));
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGWidget::readDataFromHDF5(H5StatsReader::Pointer reader, int phase)
{
  setWidgetListEnabled(true);
  float mu = 1.0f;
  float sigma = 1.0f;
  float minCutOff = 5.0f;
  float maxCutOff = 5.0f;
  float binStepSize, maxGrainSize, minGrainSize;
  std::vector<float> grainDiamInfo;
  std::vector<float> double_data;
  int err = 0;

  setPhaseIndex(phase);

  std::vector<unsigned int> xtal;
  err = reader->readStatsDataset(phase, DREAM3D::HDF5::CrystalStructure, xtal);
  CHECK_STATS_READ_ERROR(err, DREAM3D::HDF5::Statistics, DREAM3D::HDF5::CrystalStructure)
  m_CrystalStructure = static_cast<Ebsd::CrystalStructure>(xtal[0]);

  std::vector<unsigned int> pt;
  err = reader->readStatsDataset(phase, DREAM3D::HDF5::PhaseType, pt);
  CHECK_STATS_READ_ERROR(err, DREAM3D::HDF5::Statistics, DREAM3D::HDF5::PhaseType)
  m_PhaseType = static_cast<DREAM3D::Reconstruction::PhaseType>(pt[0]);

  std::vector<float> phaseFraction;
  err = reader->readStatsDataset(phase, DREAM3D::HDF5::PhaseFraction, phaseFraction);
  CHECK_STATS_READ_ERROR(err, DREAM3D::HDF5::Statistics, DREAM3D::HDF5::PhaseFraction)
  m_PhaseFraction = phaseFraction[0];

  m_PptFraction = -1.0;
  if (DREAM3D::Reconstruction::PrecipitatePhase == m_PhaseType) {
    err = reader->readScalarAttribute<float>(phase, DREAM3D::HDF5::PhaseType, DREAM3D::HDF5::PrecipitateBoundaryFraction, m_PptFraction);
    CHECK_STATS_READ_ERROR(err, DREAM3D::HDF5::Statistics, DREAM3D::HDF5::PrecipitateBoundaryFraction)
  }

  m_Omega3Plot->setCrystalStructure(m_CrystalStructure);
  m_BOverAPlot->setCrystalStructure(m_CrystalStructure);
  m_COverAPlot->setCrystalStructure(m_CrystalStructure);
  m_COverBPlot->setCrystalStructure(m_CrystalStructure);
  m_NeighborPlot->setCrystalStructure(m_CrystalStructure);
  m_ODFWidget->setCrystalStructure(m_CrystalStructure);
 // m_AxisODFWidget->setCrystalStructure(m_CrystalStructure);


  /* Read the BinNumbers data set */
  std::vector<float> bins;
  err = reader->readStatsDataset(phase, DREAM3D::HDF5::BinNumber, bins);
  CHECK_STATS_READ_ERROR(err, DREAM3D::HDF5::Statistics, DREAM3D::HDF5::BinNumber)

  /* Read the Grain_Diameter_Info Data */
  err = reader->readStatsDataset(phase, DREAM3D::HDF5::Grain_Diameter_Info, grainDiamInfo);
  CHECK_STATS_READ_ERROR(err, DREAM3D::HDF5::Statistics, DREAM3D::HDF5::Grain_Diameter_Info)

  binStepSize = grainDiamInfo[0];
  m_BinStepSize->blockSignals(true);
  m_BinStepSize->setValue(grainDiamInfo[0]);
  m_BinStepSize->blockSignals(false);
  maxGrainSize = grainDiamInfo[1];
  minGrainSize = grainDiamInfo[2];

  /* Read the Grain_Size_Distribution Data */
  err = reader->readStatsDataset(phase, DREAM3D::HDF5::Grain_Size_Distribution, double_data);
  CHECK_STATS_READ_ERROR(err, DREAM3D::HDF5::Statistics, DREAM3D::HDF5::Grain_Size_Distribution)

  mu = double_data[0];
  sigma = double_data[1];
  m_Mu_SizeDistribution->blockSignals(true);
  m_Sigma_SizeDistribution->blockSignals(true);

  m_Mu_SizeDistribution->setText(QString::number(mu));
  m_Sigma_SizeDistribution->setText(QString::number(sigma));

  m_Mu_SizeDistribution->blockSignals(false);
  m_Sigma_SizeDistribution->blockSignals(false);

  minCutOff = (mu - log(minGrainSize))/sigma;
  maxCutOff = (log(maxGrainSize) - mu)/sigma;

  m_MinSigmaCutOff->blockSignals(true);
  m_MinSigmaCutOff->setText(QString::number(minCutOff));
  m_MinSigmaCutOff->blockSignals(false);

  m_MaxSigmaCutOff->blockSignals(true);
  m_MaxSigmaCutOff->setText(QString::number(maxCutOff));
  m_MaxSigmaCutOff->blockSignals(false);

  // Update the Size/Weights Plot
  updateSizeDistributionPlot();
  m_NumberBinsGenerated->setText(QString::number(bins.size()));

  // Now have each of the plots read it's own data
  QVector<float> qbins = QVector<float>::fromStdVector(bins);
  m_Omega3Plot->readDataFromHDF5(reader, qbins, DREAM3D::HDF5::Grain_SizeVOmega3_Distributions);
  m_Omega3Plot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  m_BOverAPlot->readDataFromHDF5(reader, qbins, DREAM3D::HDF5::Grain_SizeVBoverA_Distributions);
  m_BOverAPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  m_COverAPlot->readDataFromHDF5(reader, qbins, DREAM3D::HDF5::Grain_SizeVCoverA_Distributions);
  m_COverAPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  m_COverBPlot->readDataFromHDF5(reader, qbins, DREAM3D::HDF5::Grain_SizeVCoverB_Distributions);
  m_COverBPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  m_NeighborPlot->readDataFromHDF5(reader, qbins, DREAM3D::HDF5::Grain_SizeVNeighbors_Distributions);
  m_NeighborPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);


  // Read the ODF Data
  err = m_ODFWidget->readDataFromHDF5(reader, m_PhaseIndex);

  // Read the Axis ODF Data
  err = m_AxisODFWidget->readDataFromHDF5(reader, m_PhaseIndex);

  // Enable all the tabs
  setTabsPlotTabsEnabled(true);
  m_DataHasBeenGenerated = true;
  return err;
}


