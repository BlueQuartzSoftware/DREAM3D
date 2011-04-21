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
#include "SGWidget.h"


#include <iostream>
#include <limits>

#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QVector>
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


#include "AIM/Common/HDF5/H5ReconStatsWriter.h"

#include "StatsGen.h"

#define CHECK_ERROR_ON_WRITE(var, msg)\
    if (err < 0) {\
      QMessageBox::critical(this, tr("AIM Representation"),\
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
m_PhaseIndex(0),
m_CrystalStructure(AIM::Reconstruction::Cubic),
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
void SGWidget::setupGui()
{
  distributionTypeCombo->addItem(AIM::HDF5::BetaDistribution.c_str());
  distributionTypeCombo->addItem(AIM::HDF5::LogNormalDistribution.c_str());
  distributionTypeCombo->addItem(AIM::HDF5::PowerLawDistribution.c_str());
  distributionTypeCombo->setCurrentIndex(AIM::Reconstruction::LogNormal);
  // Turn off all the plot widgets
  setTabsPlotTabsEnabled(false);

  double mu = 1.0;
  double sigma = 0.1;
  double cutOff = 5;
  double binStepSize = 0.5;

  StatsGenPlotWidget* w = m_Omega3Plot;

   w->setPlotTitle(QString("Size Vs. Omega 3"));
   w->setXAxisName(QString("Omega 3"));
   w->setYAxisName(QString("Frequency"));
   w->setDistributionType(AIM::Reconstruction::Beta);
   w->setStatisticsType(AIM::Reconstruction::Grain_SizeVOmega3);
   w->blockDistributionTypeChanges(true);
   w->setRowOperationEnabled(false);
   w->setMu(mu);
   w->setSigma(sigma);
   w->setCutoff(cutOff);
   w->setBinStep(binStepSize);
   connect(m_Omega3Plot, SIGNAL(userEditedData()),
           this, SLOT(dataWasEdited()));


   w = m_BOverAPlot;
   w->setPlotTitle(QString("B/A Shape Distribution"));
   w->setXAxisName(QString("B/A"));
   w->setYAxisName(QString("Frequency"));
   w->setDistributionType(AIM::Reconstruction::Beta);
   w->setStatisticsType(AIM::Reconstruction::Grain_SizeVBoverA);
   w->blockDistributionTypeChanges(true);
   w->setRowOperationEnabled(false);
   w->setMu(mu);
   w->setSigma(sigma);
   w->setCutoff(cutOff);
   w->setBinStep(binStepSize);
   connect(m_BOverAPlot, SIGNAL(userEditedData()),
           this, SLOT(dataWasEdited()));

   w = m_COverAPlot;
   w->setPlotTitle(QString("C/A Shape Distribution"));
   w->setXAxisName(QString("C/A"));
   w->setYAxisName(QString("Frequency"));
   w->setDistributionType(AIM::Reconstruction::Beta);
   w->setStatisticsType(AIM::Reconstruction::Grain_SizeVCoverA);
   w->blockDistributionTypeChanges(true);
   w->setRowOperationEnabled(false);
   w->setMu(mu);
   w->setSigma(sigma);
   w->setCutoff(cutOff);
   w->setBinStep(binStepSize);
   connect(m_COverAPlot, SIGNAL(userEditedData()),
           this, SLOT(dataWasEdited()));

   w = m_COverBPlot;
   w->setPlotTitle(QString("C/B Shape Distribution"));
   w->setXAxisName(QString("C/B"));
   w->setYAxisName(QString("Frequency"));
   w->setDistributionType(AIM::Reconstruction::Beta);
   w->setStatisticsType(AIM::Reconstruction::Grain_SizeVCoverB);
   w->blockDistributionTypeChanges(true);
   w->setRowOperationEnabled(false);
   w->setMu(mu);
   w->setSigma(sigma);
   w->setCutoff(cutOff);
   w->setBinStep(binStepSize);
   connect(m_COverBPlot, SIGNAL(userEditedData()),
           this, SLOT(dataWasEdited()));

   w = m_NeighborPlot;
   w->setPlotTitle(QString("Neighbors Distributions"));
   w->setXAxisName(QString("Distance (Multiples of Diameter)"));
   w->setYAxisName(QString("Number of Grains"));
   w->setDistributionType(AIM::Reconstruction::Power);
   w->setStatisticsType(AIM::Reconstruction::Grain_SizeVNeighbors);
   w->blockDistributionTypeChanges(true);
   w->setRowOperationEnabled(false);
   w->setMu(mu);
   w->setSigma(sigma);
   w->setCutoff(cutOff);
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
  m_AxisODFWidget->setCrystalStructure(AIM::Reconstruction::OrthoRhombic);

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
void SGWidget::setCrystalStructure(AIM::Reconstruction::CrystalStructure xtal)
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
AIM::Reconstruction::CrystalStructure SGWidget::getCrystalStructure()
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
  if (m_CrystalStructure == AIM::Reconstruction::Cubic)
  {
    s.append("Cubic");
  }
  else if (m_CrystalStructure == AIM::Reconstruction::Hexagonal)
  {
    s.append("Hexagonal");
  }
  return s;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGWidget::gatherSizeDistributionFromGui(double &mu, double &sigma, double &cutOff, double &stepSize)
{
  bool ok = false;
  mu = m_Mu_SizeDistribution->text().toDouble(&ok);
  if (ok == false)
  {
    return 0;
  }
  sigma = m_Sigma_SizeDistribution->text().toDouble(&ok);
  if (ok == false)
  {
    return 0;
  }
  cutOff = m_SigmaCutOff_SizeDistribution->text().toDouble(&ok);
  if (ok == false)
  {
    return 0;
  }
  stepSize = m_BinStepSize->text().toDouble(&ok);
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
void SGWidget::on_m_GenerateDefaultData_clicked()
{
  plotSizeDistribution();
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
void SGWidget::on_m_SigmaCutOff_SizeDistribution_textChanged(const QString &text)
{
  updateSizeDistributionPlot();
  m_SigmaCutOff_SizeDistribution->setFocus();
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
  double mu = 1.0;
  double sigma = 1.0;
  double cutOff = 1.0;
  double stepSize = 1.0;
  double max, min;
  int err = gatherSizeDistributionFromGui(mu, sigma, cutOff, stepSize);
  if (err == 0)
  {
    return;
  }

  StatsGen sg;
  int n = sg.computeNumberOfBins(mu, sigma, cutOff, stepSize, max, min);
  m_NumberBinsGenerated->setText(QString::number(n));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGWidget::calculateNumberOfBins(double mu, double sigma, double cutOff, double stepSize)
{
  double max, min; // Only needed for the method. Not used otherwise.
  StatsGen sg;
  return sg.computeNumberOfBins(mu, sigma, cutOff, stepSize, max, min);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGWidget::computeBinsAndCutOffs( double mu, double sigma, double cutOff, double binStepSize,
                                             QwtArray<double> &binsizes,
                                             QwtArray<double> &xCo,
                                             QwtArray<double> &yCo,
                                             double &xMax, double &yMax,
                                             QwtArray<double> &x,
                                             QwtArray<double> &y)
{
  int err = 0;
  int size = 100;

  StatsGen sg;
  err = sg.GenLogNormal<QwtArray<double> > (mu, sigma, x, y, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return -1;
  }

//  double xMax = std::numeric_limits<double >::min();
//  double yMax = std::numeric_limits<double >::min();
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
  err = sg.GenCutOff<double, QwtArray<double> > (mu, sigma, cutOff, binStepSize, xCo, yCo, yMax, numsizebins, binsizes);

  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGWidget::updateSizeDistributionPlot()
{

  double mu = 1.0;
  double sigma = 1.0;
  double cutOff = 1.0;
  double stepSize = 1.0;
  int err = gatherSizeDistributionFromGui(mu, sigma, cutOff, stepSize);
  if (err == 0)
  {
    return;
  }

  QwtArray<double> xCo;
  QwtArray<double> yCo;
  QwtArray<double> binsizes;
  double xMax = std::numeric_limits<double >::min();
  double yMax = std::numeric_limits<double >::min();
  QwtArray<double> x;
  QwtArray<double> y;
  err = computeBinsAndCutOffs(mu, sigma, cutOff, stepSize, binsizes, xCo, yCo, xMax, yMax, x, y);
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

  m_SizeDistributionCurve->setData(x, y);
//  std::cout << "----------------" << std::endl;
//  std::cout << "yMax: " << yMax << std::endl;
//  std::cout << "xMax: " << xMax << std::endl;

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

  double mu = 1.0;
  double sigma = 1.0;
  double cutOff = 1.0;
  double stepSize = 1.0;
  int err = gatherSizeDistributionFromGui(mu, sigma, cutOff, stepSize);
  if (err == 0)
  {
    return;
  }

  QwtArray<double> xCo;
  QwtArray<double> yCo;
  QwtArray<double> binsizes;
  double xMax = std::numeric_limits<double >::min();
  double yMax = std::numeric_limits<double >::min();
  QwtArray<double> x;
  QwtArray<double> y;
  err = computeBinsAndCutOffs(mu, sigma, cutOff, stepSize, binsizes, xCo, yCo, xMax, yMax, x, y);
  if (err < 0) { return; }


  // Now that we have bins and grain sizes, push those to the other plot widgets
  // Setup Each Plot Widget
  m_Omega3Plot->setSizeDistributionValues(mu, sigma, cutOff, stepSize);
  m_Omega3Plot->setBins(binsizes);

  m_BOverAPlot->setSizeDistributionValues(mu, sigma, cutOff, stepSize);
  m_BOverAPlot->setBins(binsizes);

  m_COverAPlot->setSizeDistributionValues(mu, sigma, cutOff, stepSize);
  m_COverAPlot->setBins(binsizes);

  m_COverBPlot->setSizeDistributionValues(mu, sigma, cutOff, stepSize);
  m_COverBPlot->setBins(binsizes);

  m_NeighborPlot->setSizeDistributionValues(mu, sigma, cutOff, stepSize);
  m_NeighborPlot->setBins(binsizes);
}


#define SGWIGET_WRITE_ERROR_CHECK(var)\
    if (err < 1)  {\
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
int SGWidget::writeDataToHDF5(H5ReconStatsWriter::Pointer writer)
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
  double mu, sigma, cutOff, binStep;
  gatherSizeDistributionFromGui(mu, sigma, cutOff, binStep);
  double calcPhaseFraction = m_PhaseFraction / m_TotalPhaseFraction;

  QwtArray<double> xCo;
  QwtArray<double> yCo;
  QwtArray<double> binsizes;
  double xMax = std::numeric_limits<double>::min();
  double yMax = std::numeric_limits<double>::min();
  QwtArray<double> x;
  QwtArray<double> y;
  err = computeBinsAndCutOffs(mu, sigma, cutOff, binStep, binsizes, xCo, yCo, xMax, yMax, x, y);
  if (err < 0)
  {
    return err;
  }

  // We need to compute the Max and Min Diameter Bin Values
  double mindiameter = xCo[0];
  double maxdiameter = xCo[1];
  double avglogdiam = mu;
  double sdlogdiam = sigma;
  double stepSize = binStep;

  size_t nBins = 0;
  err = writer->writeSizeDistribution(m_PhaseIndex, calcPhaseFraction, maxdiameter, mindiameter, stepSize, avglogdiam, sdlogdiam, nBins);
  CHECK_ERROR_ON_WRITE(err, "Size Distribution")

  // Now that we have bins and grain sizes, push those to the other plot widgets
  // Setup Each Plot Widget
  err = m_Omega3Plot->writeDataToHDF5(writer, AIM::HDF5::Grain_SizeVOmega3_Distributions);
  SGWIGET_WRITE_ERROR_CHECK(AIM::HDF5::Grain_SizeVOmega3_Distributions)
  err = m_BOverAPlot->writeDataToHDF5(writer, AIM::HDF5::Grain_SizeVBoverA_Distributions);
  SGWIGET_WRITE_ERROR_CHECK(AIM::HDF5::Grain_SizeVBoverA_Distributions)
  err = m_COverAPlot->writeDataToHDF5(writer, AIM::HDF5::Grain_SizeVCoverA_Distributions);
  SGWIGET_WRITE_ERROR_CHECK(AIM::HDF5::Grain_SizeVCoverA_Distributions)
  err = m_COverBPlot->writeDataToHDF5(writer, AIM::HDF5::Grain_SizeVCoverB_Distributions);
  SGWIGET_WRITE_ERROR_CHECK(AIM::HDF5::Grain_SizeVCoverB_Distributions)
  err = m_NeighborPlot->writeDataToHDF5(writer, AIM::HDF5::Grain_SizeVNeighbors_Distributions);
  SGWIGET_WRITE_ERROR_CHECK(AIM::HDF5::Grain_SizeVNeighbors_Distributions)
  err = m_ODFWidget->writeDataToHDF5(writer);
  err = m_AxisODFWidget->writeDataToHDF5(writer);
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGWidget::readDataFromHDF5(H5ReconStatsReader::Pointer reader,int phase)
{
  setWidgetListEnabled(true);
  double mu, sigma, binStepSize, xMax, yMax, cutoff;
  std::vector<double> grainDiamInfo;
  std::vector<double> double_data;
  int err = 0;

  setPhaseIndex(phase);
#ifndef _WIN32
#warning We need to set the crystal structure here also
#endif


  /* Read the BinNumbers data set */
  std::vector<double> bins;
  err = reader->readStatsDataset(phase, AIM::HDF5::BinNumber, bins);
  CHECK_STATS_READ_ERROR(err, AIM::HDF5::Reconstruction, AIM::HDF5::BinNumber)

  /* Read the Grain_Diameter_Info Data */
  err = reader->readStatsDataset(phase, AIM::HDF5::Grain_Diameter_Info, grainDiamInfo);
  CHECK_STATS_READ_ERROR(err, AIM::HDF5::Reconstruction, AIM::HDF5::Grain_Diameter_Info)

  binStepSize = grainDiamInfo[0];
  m_BinStepSize->setValue(grainDiamInfo[0]);

  /* Read the Grain_Size_Distribution Data */
  err = reader->readStatsDataset(phase, AIM::HDF5::Grain_Size_Distribution, double_data);
  CHECK_STATS_READ_ERROR(err, AIM::HDF5::Reconstruction, AIM::HDF5::Grain_Size_Distribution)

  mu = double_data[0];
  sigma = double_data[1];
  m_Mu_SizeDistribution->setText(QString::number(mu));
  m_Sigma_SizeDistribution->setText(QString::number(sigma));



  int nBins = (int)(bins.size());

  StatsGen sg;
  int computedNBins = 0;
  cutoff = 0.0;
  while (computedNBins != nBins)
  {
    computedNBins = sg.computeNumberOfBins(mu, sigma, cutoff, binStepSize, xMax, yMax);
    //    std::cout << "nBins: " << nBins << std::endl;
    //    std::cout << "computedNBins: " << computedNBins << std::endl;
    //    std::cout << "cutoff: " << cutoff << std::endl;
    cutoff++;
  }

#ifndef _WIN32
#warning We need to set the Distribution Type here also
#endif
  QVector<double> qbins = QVector<double>::fromStdVector(bins);

  m_SigmaCutOff_SizeDistribution->setText(QString::number(cutoff - 1.0));


  m_Omega3Plot->readDataFromHDF5(reader, qbins, AIM::HDF5::Grain_SizeVOmega3_Distributions);
  m_Omega3Plot->setSizeDistributionValues(mu, sigma, cutoff - 1.0, binStepSize);

  m_BOverAPlot->readDataFromHDF5(reader, qbins, AIM::HDF5::Grain_SizeVBoverA_Distributions);
  m_BOverAPlot->setSizeDistributionValues(mu, sigma, cutoff - 1.0, binStepSize);

  m_COverAPlot->readDataFromHDF5(reader, qbins, AIM::HDF5::Grain_SizeVCoverA_Distributions);
  m_COverAPlot->setSizeDistributionValues(mu, sigma, cutoff - 1.0, binStepSize);

  m_COverBPlot->readDataFromHDF5(reader, qbins, AIM::HDF5::Grain_SizeVCoverB_Distributions);
  m_COverBPlot->setSizeDistributionValues(mu, sigma, cutoff - 1.0, binStepSize);

  m_NeighborPlot->readDataFromHDF5(reader, qbins, AIM::HDF5::Grain_SizeVNeighbors_Distributions);
  m_NeighborPlot->setSizeDistributionValues(mu, sigma, cutoff - 1.0, binStepSize);


#ifndef _WIN32
#warning do We need to read the ODF data????
#endif

  // Enable all the tabs
    setTabsPlotTabsEnabled(true);


    dataWasEdited();

    return err;
}



