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


#include "TransformationPhaseWidget.h"


#include <iostream>
#include <limits>

#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QRunnable>
#include <QtCore/QThreadPool>
#include <QtConcurrent/QtConcurrentRun>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>


// Needed for AxisAngle_t and Crystal Symmetry constants
#include "EbsdLib/EbsdConstants.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/StatsData/StatsData.h"

#include "OrientationLib/Texture/StatsGen.hpp"

#include "StatsGenerator/Widgets/Presets/MicrostructurePresetManager.h"
#include "StatsGenerator/Widgets/Presets/PrimaryEquiaxedPreset.h"
#include "StatsGenerator/Widgets/Presets/PrimaryRolledPreset.h"
#include "StatsGenerator/Widgets/Presets/PrimaryRecrystallizedPreset.h"


//-- Qwt Includes AFTER SIMPLib Math due to improper defines in qwt_plot_curve.h
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>

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
    qDebug() << "TransformationPhaseWidget::on_actionOpen_triggered Error: Could not read '" << group << "' data set '" << dataset << "'" << "\n";\
    qDebug() << "  File: " << __FILE__ << "\n";\
    qDebug() << "  Line: " << __LINE__ << "\n";\
    return err;\
  }

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_TransformationPhaseWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TransformationPhaseWidget::TransformationPhaseWidget(QWidget* parent) :
  SGWidget(parent),
  m_PhaseType(SIMPL::PhaseType::PrimaryPhase),
  m_PhaseFraction(1.0),
  m_TotalPhaseFraction(1.0),
  m_ParentPhase(0),
  m_DataHasBeenGenerated(false),
  m_BulkLoadFailure(false),
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
TransformationPhaseWidget::~TransformationPhaseWidget()
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::on_microstructurePresetCombo_currentIndexChanged(int index)
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
  QString displayString = presetFactory->displayName();
  microstructurePresetCombo->addItem(displayString);
  return presetFactory;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::setupGui()
{
  distributionTypeCombo->addItem(SIMPL::StringConstants::BetaDistribution.toLatin1().data());
  distributionTypeCombo->addItem(SIMPL::StringConstants::LogNormalDistribution.toLatin1().data());
  distributionTypeCombo->addItem(SIMPL::StringConstants::PowerLawDistribution.toLatin1().data());
  distributionTypeCombo->setCurrentIndex(SIMPL::DistributionType::LogNormal);
  // Turn off all the plot widgets
  setTabsPlotTabsEnabled(false);

  microstructurePresetCombo->blockSignals(true);
  // Register all of our Microstructure Preset Factories
  AbstractMicrostructurePresetFactory::Pointer presetFactory = AbstractMicrostructurePresetFactory::NullPointer();

  //Register the Equiaxed Preset
  presetFactory = RegisterPresetFactory<PrimaryEquiaxedPresetFactory>(microstructurePresetCombo);
  QString presetName = presetFactory->displayName();
  MicrostructurePresetManager::Pointer manager = MicrostructurePresetManager::instance();
  m_MicroPreset = manager->createNewPreset(presetName);

  // Register the Rolled Preset
  presetFactory = RegisterPresetFactory<PrimaryRolledPresetFactory>(microstructurePresetCombo);

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
  w->setDistributionType(SIMPL::DistributionType::Beta);
  w->setStatisticsType(SIMPL::StatisticsType::Feature_SizeVOmega3);
  w->blockDistributionTypeChanges(true);
  w->setRowOperationEnabled(false);
  w->setMu(mu);
  w->setSigma(sigma);
  w->setMinCutOff(minCutOff);
  w->setMaxCutOff(maxCutOff);
  w->setBinStep(binStepSize);
  connect(m_Omega3Plot, SIGNAL(userEditedData()),
          this, SLOT(dataWasEdited()));
  connect(m_Omega3Plot, SIGNAL(userEditedData()),
          this, SIGNAL(phaseParametersChanged()));

  w = m_BOverAPlot;
  w->setPlotTitle(QString("B/A Shape Distribution"));
  w->setXAxisName(QString("B/A"));
  w->setYAxisName(QString("Frequency"));
  w->setDistributionType(SIMPL::DistributionType::Beta);
  w->setStatisticsType(SIMPL::StatisticsType::Feature_SizeVBoverA);
  w->blockDistributionTypeChanges(true);
  w->setRowOperationEnabled(false);
  w->setMu(mu);
  w->setSigma(sigma);
  w->setMinCutOff(minCutOff);
  w->setMaxCutOff(maxCutOff);
  w->setBinStep(binStepSize);
  connect(m_BOverAPlot, SIGNAL(userEditedData()),
          this, SLOT(dataWasEdited()));
  connect(m_BOverAPlot, SIGNAL(userEditedData()),
          this, SIGNAL(phaseParametersChanged()));

  w = m_COverAPlot;
  w->setPlotTitle(QString("C/A Shape Distribution"));
  w->setXAxisName(QString("C/A"));
  w->setYAxisName(QString("Frequency"));
  w->setDistributionType(SIMPL::DistributionType::Beta);
  w->setStatisticsType(SIMPL::StatisticsType::Feature_SizeVCoverA);
  w->blockDistributionTypeChanges(true);
  w->setRowOperationEnabled(false);
  w->setMu(mu);
  w->setSigma(sigma);
  w->setMinCutOff(minCutOff);
  w->setMaxCutOff(maxCutOff);
  w->setBinStep(binStepSize);
  connect(m_COverAPlot, SIGNAL(userEditedData()),
          this, SLOT(dataWasEdited()));
  connect(m_COverAPlot, SIGNAL(userEditedData()),
          this, SIGNAL(phaseParametersChanged()));

  m_SizeDistributionPlot->setCanvasBackground(QColor(Qt::white));
  m_SizeDistributionPlot->setTitle("Size Distribution");

  m_grid = new QwtPlotGrid;
  m_grid->enableXMin(true);
  m_grid->enableYMin(true);
#if (QWT_VERSION > 0x060000)
  m_grid->setMajorPen(QPen(Qt::gray, 0, Qt::SolidLine));
  m_grid->setMinorPen(QPen(Qt::lightGray, 0, Qt::DotLine));
#else
  m_grid->setMajPen(QPen(Qt::gray, 0, Qt::SolidLine));
  m_grid->setMinPen(QPen(Qt::lightGray, 0, Qt::DotLine));
#endif
  m_grid->attach(m_SizeDistributionPlot);

  // For the ODF Tab we want the MDF functionality
  m_ODFWidget->enableMDFTab(true);

  // Remove any Axis Decorations. The plots are explicitly know to have a -1 to 1 axis min/max
  m_ODFWidget->setEnableAxisDecorations(false);

  // Remove any Axis Decorations. The plots are explicitly know to have a -1 to 1 axis min/max
  m_AxisODFWidget->setEnableAxisDecorations(false);

  connect(m_ODFWidget, SIGNAL(odfParametersChanged()),
          this, SIGNAL(phaseParametersChanged()));
  connect(m_ODFWidget, SIGNAL(bulkLoadEvent(bool)),
          this, SLOT(bulkLoadEvent(bool)));
  connect(m_AxisODFWidget, SIGNAL(axisODFParametersChanged()),
          this, SIGNAL(phaseParametersChanged()));

  updateSizeDistributionPlot();
  calculateNumberOfBins();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::setPhaseIndex(int index)
{
  m_PhaseIndex = index;
  m_Omega3Plot->setPhaseIndex(m_PhaseIndex);
  m_BOverAPlot->setPhaseIndex(m_PhaseIndex);
  m_COverAPlot->setPhaseIndex(m_PhaseIndex);
  m_ODFWidget->setPhaseIndex(m_PhaseIndex);
  m_AxisODFWidget->setPhaseIndex(m_PhaseIndex);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TransformationPhaseWidget::getPhaseIndex() const
{
  return m_PhaseIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::setCrystalStructure(unsigned int xtal)
{
  m_CrystalStructure = xtal;
  m_Omega3Plot->setCrystalStructure(xtal);
  m_BOverAPlot->setCrystalStructure(xtal);
  m_COverAPlot->setCrystalStructure(xtal);
  m_ODFWidget->setCrystalStructure(xtal);
  /* Note that we do NOT want to set the crystal structure for the AxisODF widget
   * because we need that crystal structure to be OrthoRhombic in order for those
   * calculations to be performed correctly */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int TransformationPhaseWidget::getCrystalStructure() const
{
  return m_CrystalStructure;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString TransformationPhaseWidget::getComboString()
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
int TransformationPhaseWidget::gatherSizeDistributionFromGui(float& mu, float& sigma, float& minCutOff, float& maxCutOff, float& stepSize)
{
  QLocale loc = QLocale::system();

  bool ok = false;
  mu = loc.toFloat(m_Mu_SizeDistribution->text(), &ok);
  if (ok == false)
  {
    return 0;
  }
  sigma = loc.toFloat(m_Sigma_SizeDistribution->text(), &ok);
  if (ok == false)
  {
    return 0;
  }
  minCutOff = loc.toFloat(m_MinSigmaCutOff->text(), &ok);
  if (ok == false)
  {
    return 0;
  }
  maxCutOff = loc.toFloat(m_MaxSigmaCutOff->text(), &ok);
  if (ok == false)
  {
    return 0;
  }
  stepSize = loc.toFloat(m_BinStepSize->text(), &ok);
  if (ok == false)
  {
    return 0;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::setTabsPlotTabsEnabled(bool b)
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
void TransformationPhaseWidget::dataWasEdited()
{
  setTabsPlotTabsEnabled(true);
  this->tabWidget->setTabEnabled(0, false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::updatePlots()
{
  if (m_DataHasBeenGenerated == true)
  {
    QProgressDialog progress("Generating Data ....", "Cancel", 0, 4, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(2000);

    progress.setValue(1);
    progress.setLabelText("[1/3] Calculating Size Distributions ...");
    plotSizeDistribution();

    progress.setValue(2);
    progress.setLabelText("[2/3] Calculating ODF Data ...");
    m_ODFWidget->updatePlots();

    progress.setValue(3);
    progress.setLabelText("[3/3] Calculating Axis ODF Data ...");
    m_AxisODFWidget->updatePlots();

    progress.setValue(4);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::on_m_GenerateDefaultData_clicked()
{
  m_DataHasBeenGenerated = true;
  updatePlots();
  emit phaseParametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::bulkLoadEvent(bool fail)
{
  m_BulkLoadFailure = fail;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::on_m_Mu_SizeDistribution_textChanged(const QString& text)
{
  updateSizeDistributionPlot();
  m_Mu_SizeDistribution->setFocus();
  calculateNumberOfBins();
  emit phaseParametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::on_m_Sigma_SizeDistribution_textChanged(const QString& text)
{
  updateSizeDistributionPlot();
  m_Sigma_SizeDistribution->setFocus();
  calculateNumberOfBins();
  emit phaseParametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::on_m_MinSigmaCutOff_textChanged(const QString& text)
{
  updateSizeDistributionPlot();
  m_MinSigmaCutOff->setFocus();
  calculateNumberOfBins();
  emit phaseParametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::on_m_MaxSigmaCutOff_textChanged(const QString& text)
{
  updateSizeDistributionPlot();
  m_MaxSigmaCutOff->setFocus();
  calculateNumberOfBins();
  emit phaseParametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::on_m_BinStepSize_valueChanged(double v)
{
  calculateNumberOfBins();
  emit phaseParametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::calculateNumberOfBins()
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
int TransformationPhaseWidget::calculateNumberOfBins(float mu, float sigma, float minCutOff, float maxCutOff, float stepSize)
{
  float max, min; // Only needed for the method. Not used otherwise.

  return StatsGen::ComputeNumberOfBins(mu, sigma, minCutOff, maxCutOff, stepSize, max, min);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TransformationPhaseWidget::computeBinsAndCutOffs( float mu, float sigma,
                                                      float minCutOff, float maxCutOff,
                                                      float binStepSize,
                                                      QwtArray<float>& binsizes,
                                                      QwtArray<float>& xCo,
                                                      QwtArray<float>& yCo,
                                                      float& xMax, float& yMax,
                                                      QwtArray<float>& x,
                                                      QwtArray<float>& y)
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
void TransformationPhaseWidget::updateSizeDistributionPlot()
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
void TransformationPhaseWidget::plotSizeDistribution()
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
int TransformationPhaseWidget::gatherStatsData(AttributeMatrix::Pointer attrMat, bool preflight)
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

  // Get pointers
  IDataArray::Pointer iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::CrystalStructures);
  unsigned int* crystalStructures = std::dynamic_pointer_cast< UInt32ArrayType >(iDataArray)->getPointer(0);
  iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::PhaseTypes);
  unsigned int* phaseTypes = std::dynamic_pointer_cast< UInt32ArrayType >(iDataArray)->getPointer(0);

  crystalStructures[m_PhaseIndex] = m_CrystalStructure;
  phaseTypes[m_PhaseIndex] = m_PhaseType;

  StatsDataArray* statsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(attrMat->getAttributeArray(SIMPL::EnsembleData::Statistics).get());
  if (NULL != statsDataArray)
  {
    StatsData::Pointer statsData = statsDataArray->getStatsData(m_PhaseIndex);
    TransformationStatsData* transformationStatsData = TransformationStatsData::SafePointerDownCast(statsData.get());

    transformationStatsData->setPhaseFraction(calcPhaseFraction);
    transformationStatsData->setParentPhase(m_ParentPhase);
    // Feature Diameter Info
    transformationStatsData->setBinStepSize(stepSize);
    transformationStatsData->setMaxFeatureDiameter(maxdiameter);
    transformationStatsData->setMinFeatureDiameter(mindiameter);
    // Feature Size Distribution
    {
      VectorOfFloatArray data;
      FloatArrayType::Pointer d1 = FloatArrayType::CreateArray(1, SIMPL::StringConstants::Average);
      FloatArrayType::Pointer d2 = FloatArrayType::CreateArray(1, SIMPL::StringConstants::StandardDeviation);
      data.push_back(d1);
      data.push_back(d2);
      d1->setValue(0, avglogdiam);
      d2->setValue(0, sdlogdiam);
      transformationStatsData->setFeatureSizeDistribution(data);
      transformationStatsData->setFeatureSize_DistType(SIMPL::DistributionType::LogNormal);
    }

    // Now that we have bins and feature sizes, push those to the other plot widgets
    {
      VectorOfFloatArray data = m_Omega3Plot->getStatisticsData();
      transformationStatsData->setFeatureSize_Omegas(data);
      transformationStatsData->setOmegas_DistType(m_Omega3Plot->getDistributionType());
    }
    {
      VectorOfFloatArray data = m_BOverAPlot->getStatisticsData();
      transformationStatsData->setFeatureSize_BOverA(data);
      transformationStatsData->setBOverA_DistType(m_BOverAPlot->getDistributionType());
    }
    {
      VectorOfFloatArray data = m_COverAPlot->getStatisticsData();
      transformationStatsData->setFeatureSize_COverA(data);
      transformationStatsData->setCOverA_DistType(m_COverAPlot->getDistributionType());
    }
    m_ODFWidget->getOrientationData(transformationStatsData, SIMPL::PhaseType::TransformationPhase, preflight);

    err = m_AxisODFWidget->getOrientationData(transformationStatsData, SIMPL::PhaseType::TransformationPhase, preflight);
  }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TransformationPhaseWidget::extractStatsData(AttributeMatrix::Pointer attrMat, int index)
{
  setWidgetListEnabled(true);
  float mu = 1.0f;
  float sigma = 1.0f;
  float minCutOff = 5.0f;
  float maxCutOff = 5.0f;
  float binStepSize, maxFeatureSize, minFeatureSize;

  setPhaseIndex(index);

  IDataArray::Pointer iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::CrystalStructures);
  unsigned int* attributeArray = std::dynamic_pointer_cast< UInt32ArrayType >(iDataArray)->getPointer(0);
  m_CrystalStructure = attributeArray[index];

  iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::PhaseTypes);
  attributeArray = std::dynamic_pointer_cast< UInt32ArrayType >(iDataArray)->getPointer(0);
  m_PhaseType = attributeArray[index];

  iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::Statistics);
  StatsDataArray* statsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(iDataArray.get());

  if (statsDataArray == NULL)
  {
    return;
  }
  StatsData::Pointer statsData = statsDataArray->getStatsData(index);
  TransformationStatsData* transformationStatsData = TransformationStatsData::SafePointerDownCast(statsData.get());

  m_PhaseFraction = transformationStatsData->getPhaseFraction();

  m_ParentPhase = transformationStatsData->getParentPhase();

  m_Omega3Plot->setCrystalStructure(m_CrystalStructure);
  m_BOverAPlot->setCrystalStructure(m_CrystalStructure);
  m_COverAPlot->setCrystalStructure(m_CrystalStructure);
  m_ODFWidget->setCrystalStructure(m_CrystalStructure);
// m_AxisODFWidget->setCrystalStructure(m_CrystalStructure);


  /* SEt the BinNumbers data set */
  FloatArrayType::Pointer bins = transformationStatsData->getBinNumbers();


  /* Set the Feature_Diameter_Info Data */

  binStepSize = transformationStatsData->getBinStepSize();
  m_BinStepSize->blockSignals(true);
  m_BinStepSize->setValue(binStepSize);
  m_BinStepSize->blockSignals(false);
  maxFeatureSize = transformationStatsData->getMaxFeatureDiameter();
  minFeatureSize = transformationStatsData->getMinFeatureDiameter();

  /* Set the Feature_Size_Distribution Data */
  VectorOfFloatArray distData = transformationStatsData->getFeatureSizeDistribution();
  mu = distData[0]->getValue(0);
  sigma = distData[1]->getValue(0);
  m_Mu_SizeDistribution->blockSignals(true);
  m_Sigma_SizeDistribution->blockSignals(true);

  m_Mu_SizeDistribution->setText(QString::number(mu));
  m_Sigma_SizeDistribution->setText(QString::number(sigma));

  m_Mu_SizeDistribution->blockSignals(false);
  m_Sigma_SizeDistribution->blockSignals(false);

  minCutOff = (mu - log(minFeatureSize)) / sigma;
  maxCutOff = (log(maxFeatureSize) - mu) / sigma;

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
    qbins[i] = bins->getValue(i);
  }

  m_Omega3Plot->setDistributionType(transformationStatsData->getOmegas_DistType(), false);
  m_Omega3Plot->extractStatsData(index, qbins, transformationStatsData->getFeatureSize_Omegas());
  m_Omega3Plot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  m_BOverAPlot->setDistributionType(transformationStatsData->getBOverA_DistType(), false);
  m_BOverAPlot->extractStatsData(index, qbins, transformationStatsData->getFeatureSize_BOverA());
  m_BOverAPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  m_COverAPlot->setDistributionType(transformationStatsData->getCOverA_DistType(), false);
  m_COverAPlot->extractStatsData(index, qbins, transformationStatsData->getFeatureSize_COverA());
  m_COverAPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  // Set the ODF Data
  m_ODFWidget->extractStatsData(index, transformationStatsData, SIMPL::PhaseType::TransformationPhase);

  // Set the Axis ODF Data
  m_AxisODFWidget->extractStatsData(index, transformationStatsData, SIMPL::PhaseType::TransformationPhase);

  // Enable all the tabs
  setTabsPlotTabsEnabled(true);
  m_DataHasBeenGenerated = true;

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString TransformationPhaseWidget::getTabTitle()
{
  return QString("Transformation");
}

