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

#include "PrimaryPhaseWidget.h"

#include <iostream>
#include <limits>

#include <QtCore/QSettings>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QLineEdit>
#include <QtGui/QIntValidator>
#include <QtGui/QDoubleValidator>


// Needed for AxisAngle_t and Crystal Symmetry constants
#include "EbsdLib/EbsdConstants.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/DataArrays/StringDataArray.hpp"
#include "SIMPLib/StatsData/StatsData.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"

#include "OrientationLib/Texture/StatsGen.hpp"

#include "StatsGenerator/StatsGeneratorConstants.h"
#include "StatsGenerator/Widgets/Presets/PrimaryEquiaxedPreset.h"
#include "StatsGenerator/Widgets/Presets/PrimaryRolledPreset.h"
#include "StatsGenerator/Widgets/Presets/PrimaryRecrystallizedPreset.h"

//-- Qwt Includes AFTER SIMPLib Math due to improper defines in qwt_plot_curve.h
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>


// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_PrimaryPhaseWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrimaryPhaseWidget::PrimaryPhaseWidget(QWidget* parent) :
  SGWidget(parent)
{
  setTabTitle("Primary");
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrimaryPhaseWidget::~PrimaryPhaseWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QComboBox* PrimaryPhaseWidget::getMicrostructurePresetCombo()
{
  return microstructurePresetCombo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::on_microstructurePresetCombo_currentIndexChanged(int index)
{
  Q_UNUSED(index);
  //qDebug() << "on_microstructurePresetCombo_currentIndexChanged" << "\n";
  QString presetName = microstructurePresetCombo->currentText();

  //Factory Method to get an instantiated object of the correct type?
  MicrostructurePresetManager::Pointer manager = MicrostructurePresetManager::instance();
  getMicroPreset() = manager->createNewPreset(presetName);
  getMicroPreset()->displayUserInputDialog();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::setOmega3PlotWidget(StatsGenPlotWidget* w)
{
  m_Omega3Plot = w;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenPlotWidget* PrimaryPhaseWidget::getOmega3PlotWidget()
{
  return m_Omega3Plot;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::setBOverAPlotPlotWidget(StatsGenPlotWidget* w)
{
  m_BOverAPlot = w;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenPlotWidget* PrimaryPhaseWidget::getBOverAPlotPlotWidget()
{
  return m_BOverAPlot;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::setCOverAPlotWidget(StatsGenPlotWidget* w)
{
  m_COverAPlot = w;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenPlotWidget* PrimaryPhaseWidget::getCOverAPlotWidget()
{
  return m_COverAPlot;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::setODFWidgetWidget(StatsGenODFWidget* w)
{
  m_ODFWidget = w;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenODFWidget *PrimaryPhaseWidget::getODFWidgetWidget()
{
  return m_ODFWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::setAxisODFWidget(SGAxisODFWidget* w)
{
  m_AxisODFWidget = w;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGAxisODFWidget* PrimaryPhaseWidget::getAxisODFWidget()
{
  return m_AxisODFWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::setFeatureSizeWidget(StatsGenFeatureSizeWidget* w)
{
  m_FeatureSizeDistWidget = w;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenFeatureSizeWidget* PrimaryPhaseWidget::getFeatureSizeWidget()
{
  return m_FeatureSizeDistWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QTabWidget* PrimaryPhaseWidget::getTabWidget()
{
  return tabWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPushButton* PrimaryPhaseWidget::getGenerateDefaultDataBtn()
{
  return m_GenerateDefaultData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::removeNeighborsPlotWidget()
{
  int index = tabWidget->indexOf(neighborDistributionTab);
  if(index >= 0)
  {
    tabWidget->removeTab(index);
    m_NeighborPlot->setParent(nullptr);
    delete neighborDistributionTab;
    neighborDistributionTab = nullptr;
    delete m_NeighborPlot;
    m_NeighborPlot = nullptr;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::setupGui()
{
  // Turn off all the plot widgets
  setTabsPlotTabsEnabled(false);

  QLocale loc = QLocale::system();

  getMicrostructurePresetCombo()->blockSignals(true);
  // Register all of our Microstructure Preset Factories
  AbstractMicrostructurePresetFactory::Pointer presetFactory = AbstractMicrostructurePresetFactory::NullPointer();

  //Register the Equiaxed Preset
  presetFactory = RegisterPresetFactory<PrimaryEquiaxedPresetFactory>(getMicrostructurePresetCombo());
  QString presetName = (presetFactory->displayName());
  MicrostructurePresetManager::Pointer manager = MicrostructurePresetManager::instance();
  setMicroPreset(manager->createNewPreset(presetName));

  // Register the Rolled Preset
  presetFactory = RegisterPresetFactory<PrimaryRolledPresetFactory>(getMicrostructurePresetCombo());

  // Select the first Preset in the list
  microstructurePresetCombo->setCurrentIndex(0);
  microstructurePresetCombo->blockSignals(false);

  float mu = 1.0f;
  float sigma = 0.1f;
  float minCutOff = 5.0f;
  float maxCutOff = 5.0f;
  float binStepSize = 0.5f;

  QList<StatsGenPlotWidget*> plotWidgets;

  plotWidgets.push_back(m_Omega3Plot);
  StatsGenPlotWidget* w = m_Omega3Plot;
  w->setPlotTitle(QString("Omega 3 Probabiity Density Functions"));
  w->setXAxisName(QString("Omega 3"));
  w->setYAxisName(QString("Frequency"));
  w->setDataTitle(QString("Edit Distribution Values"));
  w->setDistributionType(SIMPL::DistributionType::Beta);
  w->setStatisticsType(SIMPL::StatisticsType::Feature_SizeVOmega3);
  w->blockDistributionTypeChanges(true);
  w->setRowOperationEnabled(false);
  w->setMu(mu);
  w->setSigma(sigma);
  w->setMinCutOff(minCutOff);
  w->setMaxCutOff(maxCutOff);
  w->setBinStep(binStepSize);
  connect(w, SIGNAL(dataChanged()),
          this, SLOT(dataWasEdited()));
  connect(w, SIGNAL(dataChanged()),
          this, SIGNAL(dataChanged()));
  connect(m_FeatureSizeDistWidget, SIGNAL(binSelected(int)),
          w, SLOT(highlightCurve(int)));

  plotWidgets.push_back(m_BOverAPlot);
  w = m_BOverAPlot;
  w->setPlotTitle(QString("B/A Shape Distribution"));
  w->setXAxisName(QString("B/A"));
  w->setYAxisName(QString("Frequency"));
  w->setDataTitle(QString("Edit Distribution Values"));
  w->setDistributionType(SIMPL::DistributionType::Beta);
  w->setStatisticsType(SIMPL::StatisticsType::Feature_SizeVBoverA);
  w->blockDistributionTypeChanges(true);
  w->setRowOperationEnabled(false);
  w->setMu(mu);
  w->setSigma(sigma);
  w->setMinCutOff(minCutOff);
  w->setMaxCutOff(maxCutOff);
  w->setBinStep(binStepSize);
  connect(w, SIGNAL(dataChanged()),
          this, SLOT(dataWasEdited()));
  connect(w, SIGNAL(dataChanged()),
          this, SIGNAL(dataChanged()));
  connect(m_FeatureSizeDistWidget, SIGNAL(binSelected(int)),
          w, SLOT(highlightCurve(int)));

  plotWidgets.push_back(m_COverAPlot);
  w = m_COverAPlot;
  w->setPlotTitle(QString("C/A Shape Distribution"));
  w->setXAxisName(QString("C/A"));
  w->setYAxisName(QString("Frequency"));
  w->setDataTitle(QString("Edit Distribution Values"));
  w->setDistributionType(SIMPL::DistributionType::Beta);
  w->setStatisticsType(SIMPL::StatisticsType::Feature_SizeVCoverA);
  w->blockDistributionTypeChanges(true);
  w->setRowOperationEnabled(false);
  w->setMu(mu);
  w->setSigma(sigma);
  w->setMinCutOff(minCutOff);
  w->setMaxCutOff(maxCutOff);
  w->setBinStep(binStepSize);
  connect(w, SIGNAL(dataChanged()),
          this, SLOT(dataWasEdited()));
  connect(w, SIGNAL(dataChanged()),
          this, SIGNAL(dataChanged()));
  connect(m_FeatureSizeDistWidget, SIGNAL(binSelected(int)),
          w, SLOT(highlightCurve(int)));

  plotWidgets.push_back(m_NeighborPlot);
  w = m_NeighborPlot;
  w->setPlotTitle(QString("Neighbors Distributions"));
  w->setXAxisName(QString("Number of Features (within 1 diameter)"));
  w->setYAxisName(QString("Frequency"));
  w->setDataTitle(QString("Edit Distribution Values"));
  w->setDistributionType(SIMPL::DistributionType::LogNormal);
  w->setStatisticsType(SIMPL::StatisticsType::Feature_SizeVNeighbors);
  w->blockDistributionTypeChanges(true);
  w->setRowOperationEnabled(false);
  w->setMu(mu);
  w->setSigma(sigma);
  w->setMinCutOff(minCutOff);
  w->setMaxCutOff(maxCutOff);
  w->setBinStep(binStepSize);
  connect(w, SIGNAL(dataChanged()),
          this, SLOT(dataWasEdited()));
  connect(w, SIGNAL(dataChanged()),
          this, SIGNAL(dataChanged()));
  connect(m_FeatureSizeDistWidget, SIGNAL(binSelected(int)),
          w, SLOT(highlightCurve(int)));


  setSGPlotWidgets(plotWidgets);

  // For the ODF Tab we want the MDF functionality
  m_ODFWidget->enableMDFTab(true);

  // Remove any Axis Decorations. The plots are explicitly know to have a -1 to 1 axis min/max
  m_ODFWidget->setEnableAxisDecorations(false);

  // Remove any Axis Decorations. The plots are explicitly know to have a -1 to 1 axis min/max
  m_AxisODFWidget->setEnableAxisDecorations(false);

  connect(m_ODFWidget, SIGNAL(dataChanged()),
          this, SIGNAL(dataChanged()));
  connect(m_ODFWidget, SIGNAL(bulkLoadEvent(bool)),
          this, SLOT(bulkLoadEvent(bool)));
  connect(m_AxisODFWidget, SIGNAL(dataChanged()),
          this, SIGNAL(dataChanged()));

  connect(m_FeatureSizeDistWidget, SIGNAL(dataChanged()),
          this, SIGNAL(dataChanged()));

  connect(m_FeatureSizeDistWidget, SIGNAL(userEnteredValidData(bool)),
          m_GenerateDefaultData, SLOT(setEnabled(bool)));

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::setPhaseIndex(int index)
{
  SGWidget::setPhaseIndex(index);
  m_Omega3Plot->setPhaseIndex(index);
  m_BOverAPlot->setPhaseIndex(index);
  m_COverAPlot->setPhaseIndex(index);
  if(m_NeighborPlot) { m_NeighborPlot->setPhaseIndex(index); }
  m_ODFWidget->setPhaseIndex(index);
  m_AxisODFWidget->setPhaseIndex(index);
  m_FeatureSizeDistWidget->setPhaseIndex(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::setCrystalStructure(unsigned int xtal)
{
  SGWidget::setCrystalStructure(xtal);
  m_Omega3Plot->setCrystalStructure(xtal);
  m_BOverAPlot->setCrystalStructure(xtal);
  m_COverAPlot->setCrystalStructure(xtal);
  if(m_NeighborPlot) { m_NeighborPlot->setCrystalStructure(xtal);}
  m_ODFWidget->setCrystalStructure(xtal);
  m_FeatureSizeDistWidget->setCrystalStructure(xtal);
  /* Note that we do NOT want to set the crystal structure for the AxisODF widget
   * because we need that crystal structure to be OrthoRhombic in order for those
   * calculations to be performed correctly */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PrimaryPhaseWidget::getComboString()
{
  QString s = QString::number(getPhaseIndex());
  s.append(" - ");
  if ( Ebsd::CrystalStructure::Cubic_High == getCrystalStructure())
  {
    s.append("Cubic");
  }
  else if ( Ebsd::CrystalStructure::Hexagonal_High == getCrystalStructure())
  {
    s.append("Hexagonal");
  }
  return s;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::setTabsPlotTabsEnabled(bool b)
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
void PrimaryPhaseWidget::dataWasEdited()
{
  setTabsPlotTabsEnabled(true);
  this->tabWidget->setTabEnabled(0, false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::updatePlots()
{
  if (getDataHasBeenGenerated() == true)
  {
    QProgressDialog progress("Generating Data ....", "Cancel", 0, 4, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(2000);

    progress.setValue(1);
    progress.setLabelText("[1/3] Calculating Size Distributions ...");
    m_FeatureSizeDistWidget->plotSizeDistribution();

    // Now that we have bins and feature sizes, push those to the other plot widgets
    // Setup Each Plot Widget
    // The MicroPreset class will set the distribution for each of the plots

    float mu = m_FeatureSizeDistWidget->getMu();
    float sigma = m_FeatureSizeDistWidget->getSigma();
    float minCutOff = m_FeatureSizeDistWidget->getMinCutOff();
    float maxCutOff = m_FeatureSizeDistWidget->getMaxCutOff();
    float binStepSize = m_FeatureSizeDistWidget->getBinStep();
    QwtArray<float> binSizes = m_FeatureSizeDistWidget->getBinSizes();

    m_Omega3Plot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);
    getMicroPreset()->initializeOmega3TableModel(m_Omega3Plot, binSizes);

    m_BOverAPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);
    getMicroPreset()->initializeBOverATableModel(m_BOverAPlot, binSizes);

    m_COverAPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);
    getMicroPreset()->initializeCOverATableModel(m_COverAPlot, binSizes);
    if(m_NeighborPlot) {
      m_NeighborPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);
      getMicroPreset()->initializeNeighborTableModel(m_NeighborPlot, binSizes);
    }

    // Get any presets for the ODF/AxisODF/MDF also
    getMicroPreset()->initializeODFTableModel(m_ODFWidget);
    getMicroPreset()->initializeAxisODFTableModel(m_AxisODFWidget);
    getMicroPreset()->initializeMDFTableModel(m_ODFWidget->getMDFWidget());

    progress.setValue(2);
    progress.setLabelText("[2/3] Calculating ODF Data ...");
    m_ODFWidget->updatePlots();

    progress.setValue(3);
    progress.setLabelText("[3/3] Calculating Axis ODF Data ...");
    m_AxisODFWidget->updatePlots();

    progress.setValue(4);

    setTabsPlotTabsEnabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::generateDefaultData()
{
  on_m_GenerateDefaultData_clicked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::on_m_GenerateDefaultData_clicked()
{
  setDataHasBeenGenerated(true);
  updatePlots();
  emit dataChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::bulkLoadEvent(bool fail)
{
  setBulkLoadFailure(fail);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PrimaryPhaseWidget::gatherStatsData(AttributeMatrix::Pointer attrMat, bool preflight)
{
  if (getPhaseIndex() < 1)
  {
    QMessageBox::critical(this, tr("StatsGenerator"),
                          tr("The Phase Index is Less than 1. This is not allowed."),
                          QMessageBox::Default);
    return -1;
  }
  int retErr = 0;
  int err = 0;

  // Get pointers
  IDataArray::Pointer iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::CrystalStructures);
  unsigned int* crystalStructures = std::dynamic_pointer_cast< UInt32ArrayType >(iDataArray)->getPointer(0);
  iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::PhaseTypes);
  unsigned int* phaseTypes = std::dynamic_pointer_cast< UInt32ArrayType >(iDataArray)->getPointer(0);

  iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::PhaseName);
  StringDataArray::Pointer strArray = std::dynamic_pointer_cast<StringDataArray>(iDataArray);

  crystalStructures[getPhaseIndex()] = getCrystalStructure();
  phaseTypes[getPhaseIndex()] = getPhaseType();
  strArray->setValue(getPhaseIndex(), getPhaseName());

  StatsDataArray* statsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(attrMat->getAttributeArray(SIMPL::EnsembleData::Statistics).get());
  if (nullptr != statsDataArray)
  {
    StatsData::Pointer statsData = statsDataArray->getStatsData(getPhaseIndex());
    PrimaryStatsData* primaryStatsData = PrimaryStatsData::SafePointerDownCast(statsData.get());

    float calcPhaseFraction = getPhaseFraction() / getTotalPhaseFraction();

    primaryStatsData->setPhaseFraction(calcPhaseFraction);
    statsData->setName(getPhaseName());

    err = m_FeatureSizeDistWidget->getStatisticsData(primaryStatsData);

    // Now that we have bins and feature sizes, push those to the other plot widgets
    {
      VectorOfFloatArray data = m_Omega3Plot->getStatisticsData();
      primaryStatsData->setFeatureSize_Omegas(data);
      primaryStatsData->setOmegas_DistType(m_Omega3Plot->getDistributionType());
    }
    {
      VectorOfFloatArray data = m_BOverAPlot->getStatisticsData();
      primaryStatsData->setFeatureSize_BOverA(data);
      primaryStatsData->setBOverA_DistType(m_BOverAPlot->getDistributionType());
    }
    {
      VectorOfFloatArray data = m_COverAPlot->getStatisticsData();
      primaryStatsData->setFeatureSize_COverA(data);
      primaryStatsData->setCOverA_DistType(m_COverAPlot->getDistributionType());
    }
    if(m_NeighborPlot) {
      VectorOfFloatArray data = m_NeighborPlot->getStatisticsData();
      primaryStatsData->setFeatureSize_Neighbors(data);
      primaryStatsData->setNeighbors_DistType(m_NeighborPlot->getDistributionType());
    }

    m_ODFWidget->getOrientationData(primaryStatsData, SIMPL::PhaseType::PrimaryPhase, preflight);

    err = m_AxisODFWidget->getOrientationData(primaryStatsData, SIMPL::PhaseType::PrimaryPhase, preflight);
  }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrimaryPhaseWidget::extractStatsData(AttributeMatrix::Pointer attrMat, int index)
{
  setWidgetListEnabled(true);
  setPhaseIndex(index);

  IDataArray::Pointer iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::CrystalStructures);
  unsigned int* attributeArray = std::dynamic_pointer_cast< UInt32ArrayType >(iDataArray)->getPointer(0);
  setCrystalStructure(attributeArray[index]);

  iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::PhaseTypes);
  attributeArray = std::dynamic_pointer_cast< UInt32ArrayType >(iDataArray)->getPointer(0);
  setPhaseType(attributeArray[index]);

  iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::Statistics);
  StatsDataArray* statsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(iDataArray.get());
  if (statsDataArray == nullptr)
  {
    return;
  }
  StatsData::Pointer statsData = statsDataArray->getStatsData(index);
  PrimaryStatsData* primaryStatsData = PrimaryStatsData::SafePointerDownCast(statsData.get());

  setPhaseFraction(primaryStatsData->getPhaseFraction());
  setPhaseName(statsData->getName());

  m_FeatureSizeDistWidget->setCrystalStructure(getCrystalStructure());
  foreach(StatsGenPlotWidget* w, m_SGPlotWidgets)
  {
    w->setCrystalStructure(getCrystalStructure());
  }
  m_ODFWidget->setCrystalStructure(getCrystalStructure());

  /* SEt the BinNumbers data set */
  FloatArrayType::Pointer bins = primaryStatsData->getBinNumbers();

#if 0
  /* Set the Feature_Diameter_Info Data */

  binStepSize = primaryStatsData->getBinStepSize();
  m_BinStepSize->blockSignals(true);
  m_BinStepSize->setValue(binStepSize);
  m_BinStepSize->blockSignals(false);
  maxFeatureSize = primaryStatsData->getMaxFeatureDiameter();
  minFeatureSize = primaryStatsData->getMinFeatureDiameter();

  /* Set the Feature_Size_Distribution Data */
  VectorOfFloatArray distData = primaryStatsData->getFeatureSizeDistribution();
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
#endif

  // Now have each of the plots set it's own data
  QVector<float> qbins (bins->getNumberOfTuples());
  for(int i = 0; i < qbins.size(); ++i)
  {
    qbins[i] = bins->getValue(i);
  }

  m_FeatureSizeDistWidget->extractStatsData(primaryStatsData, index);

  float mu = m_FeatureSizeDistWidget->getMu();
  float sigma = m_FeatureSizeDistWidget->getSigma();
  float minCutOff = m_FeatureSizeDistWidget->getMinCutOff();
  float maxCutOff = m_FeatureSizeDistWidget->getMaxCutOff();
  float binStepSize = m_FeatureSizeDistWidget->getBinStep();

  m_Omega3Plot->setDistributionType(primaryStatsData->getOmegas_DistType(), false);
  m_Omega3Plot->extractStatsData(index, qbins, primaryStatsData->getFeatureSize_Omegas());
  m_Omega3Plot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  m_BOverAPlot->setDistributionType(primaryStatsData->getBOverA_DistType(), false);
  m_BOverAPlot->extractStatsData(index, qbins, primaryStatsData->getFeatureSize_BOverA());
  m_BOverAPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  m_COverAPlot->setDistributionType(primaryStatsData->getCOverA_DistType(), false);
  m_COverAPlot->extractStatsData(index, qbins, primaryStatsData->getFeatureSize_COverA());
  m_COverAPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  if(m_NeighborPlot) {
    m_NeighborPlot->setDistributionType(primaryStatsData->getNeighbors_DistType(), false);
    m_NeighborPlot->extractStatsData(index, qbins, primaryStatsData->getFeatureSize_Neighbors());
    m_NeighborPlot->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);
  }
  // Set the ODF Data
  m_ODFWidget->extractStatsData(index, primaryStatsData, SIMPL::PhaseType::PrimaryPhase);

  // Set the Axis ODF Data
  m_AxisODFWidget->extractStatsData(index, primaryStatsData, SIMPL::PhaseType::PrimaryPhase);

  // Enable all the tabs
  setTabsPlotTabsEnabled(true);
  setDataHasBeenGenerated(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon PrimaryPhaseWidget::getPhaseIcon()
{
  QIcon icon;
  icon.addFile(QStringLiteral(":/StatsGenerator/icons/Primary.png"), QSize(), QIcon::Normal, QIcon::Off);
  icon.addFile(QStringLiteral(":/StatsGenerator/icons/Primary_Selected.png"), QSize(), QIcon::Normal, QIcon::On);
  return icon;
}
