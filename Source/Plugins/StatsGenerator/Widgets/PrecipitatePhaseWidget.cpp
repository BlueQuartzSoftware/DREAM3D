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

#include "PrecipitatePhaseWidget.h"

#include <iostream>
#include <limits>

#include <QtCore/QSettings>
#include <QtGui/QDoubleValidator>
#include <QtGui/QIntValidator>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>

// Needed for AxisAngle_t and Crystal Symmetry constants
#include "EbsdLib/EbsdConstants.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/DataArrays/StringDataArray.hpp"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/StatsData.h"

#include "OrientationLib/Texture/StatsGen.hpp"

#include "StatsGenerator/StatsGeneratorConstants.h"
#include "StatsGenerator/Widgets/Presets/MicrostructurePresetManager.h"
#include "StatsGenerator/Widgets/Presets/PrecipitateEquiaxedPreset.h"
#include "StatsGenerator/Widgets/Presets/PrecipitateRolledPreset.h"
#include "StatsGenerator/Widgets/StatsGenRDFWidget.h"

//-- Qwt Includes AFTER SIMPLib Math due to improper defines in qwt_plot_curve.h
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_PrecipitatePhaseWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PrecipitatePhaseWidget::PrecipitatePhaseWidget(QWidget* parent)
: PrimaryPhaseWidget(parent)
, m_PptFraction(-1.0f)
{
  setTabTitle("Precipitate");
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
void PrecipitatePhaseWidget::setupGui()
{
  QLocale loc = QLocale::system();

  getMicrostructurePresetCombo()->blockSignals(true);
  // Register all of our Microstructure Preset Factories
  AbstractMicrostructurePresetFactory::Pointer presetFactory = AbstractMicrostructurePresetFactory::NullPointer();

  // Register the Equiaxed Preset
  presetFactory = RegisterPresetFactory<PrecipitateEquiaxedPresetFactory>(getMicrostructurePresetCombo());
  QString presetName = (presetFactory->displayName());
  MicrostructurePresetManager::Pointer manager = MicrostructurePresetManager::instance();
  setMicroPreset(manager->createNewPreset(presetName));

  // Register the Rolled Preset
  presetFactory = RegisterPresetFactory<PrecipitateRolledPresetFactory>(getMicrostructurePresetCombo());

  // Select the first Preset in the list
  getMicrostructurePresetCombo()->setCurrentIndex(0);
  getMicrostructurePresetCombo()->blockSignals(false);

#if 0
  float mu = 1.0f;
  float sigma = 0.1f;
  float minCutOff = 5.0f;
  float maxCutOff = 5.0f;
  float binStepSize = 0.5f;

  StatsGenPlotWidget* w = getOmega3PlotWidget();
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
  connect(w, SIGNAL(userEditedData()),
          this, SLOT(dataWasEdited()));
  connect(w, SIGNAL(userEditedData()),
          this, SIGNAL(phaseParametersChanged()));
  connect(getFeatureSizeWidget(), SIGNAL(binSelected(int)),
          w, SLOT(highlightCurve(int)));

  w = getBOverAPlotPlotWidget();
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
  connect(w, SIGNAL(userEditedData()),
          this, SLOT(dataWasEdited()));
  connect(w, SIGNAL(userEditedData()),
          this, SIGNAL(phaseParametersChanged()));
  connect(getFeatureSizeWidget(), SIGNAL(binSelected(int)),
          w, SLOT(highlightCurve(int)));

  w = getCOverAPlotWidget();
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
  connect(w, SIGNAL(userEditedData()),
          this, SLOT(dataWasEdited()));
  connect(w, SIGNAL(userEditedData()),
          this, SIGNAL(phaseParametersChanged()));
  connect(getFeatureSizeWidget(), SIGNAL(binSelected(int)),
          w, SLOT(highlightCurve(int)));


  // For the ODF Tab we want the MDF functionality
  getODFWidgetWidget()->enableMDFTab(true);

  // Remove any Axis Decorations. The plots are explicitly know to have a -1 to 1 axis min/max
  getODFWidgetWidget()->setEnableAxisDecorations(false);

  // Remove any Axis Decorations. The plots are explicitly know to have a -1 to 1 axis min/max
  getAxisODFWidget()->setEnableAxisDecorations(false);

  connect(getODFWidgetWidget(), SIGNAL(odfParametersChanged()),
          this, SIGNAL(phaseParametersChanged()));
  connect(getODFWidgetWidget(), SIGNAL(bulkLoadEvent(bool)),
          this, SLOT(bulkLoadEvent(bool)));
  connect(getAxisODFWidget(), SIGNAL(axisODFParametersChanged()),
          this, SIGNAL(phaseParametersChanged()));

#endif

  removeNeighborsPlotWidget();

  QWidget* rdfTab;
  QHBoxLayout* horizontalLayout_2;

  rdfTab = new QWidget();
  rdfTab->setObjectName(QStringLiteral("rdfTab"));
  horizontalLayout_2 = new QHBoxLayout(rdfTab);
  horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
  horizontalLayout_2->setContentsMargins(4, 4, 4, 4);
  m_RdfPlot = new StatsGenRDFWidget(rdfTab);
  m_RdfPlot->setObjectName(QStringLiteral("m_RdfPlot"));
  horizontalLayout_2->addWidget(m_RdfPlot);
  getTabWidget()->addTab(rdfTab, m_RdfPlot->getTabTitle());

  connect(m_RdfPlot, SIGNAL(rdfParametersChanged()), this, SIGNAL(dataChanged()));

  connect(getFeatureSizeWidget(), SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));

  connect(getFeatureSizeWidget(), SIGNAL(userEnteredValidData(bool)), getGenerateDefaultDataBtn(), SLOT(setEnabled(bool)));

  // Turn off all the plot widgets
  setTabsPlotTabsEnabled(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::setPhaseIndex(int index)
{
  StatsGenWidget::setPhaseIndex(index);
  getFeatureSizeWidget()->setPhaseIndex(index);
  getOmega3PlotWidget()->setPhaseIndex(index);
  getBOverAPlotPlotWidget()->setPhaseIndex(index);
  getCOverAPlotWidget()->setPhaseIndex(index);
  getODFWidgetWidget()->setPhaseIndex(index);
  getAxisODFWidget()->setPhaseIndex(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::setCrystalStructure(unsigned int xtal)
{
  StatsGenWidget::setCrystalStructure(xtal);
  getFeatureSizeWidget()->setCrystalStructure(xtal);
  getOmega3PlotWidget()->setCrystalStructure(xtal);
  getBOverAPlotPlotWidget()->setCrystalStructure(xtal);
  getCOverAPlotWidget()->setCrystalStructure(xtal);
  getODFWidgetWidget()->setCrystalStructure(xtal);
  /* Note that we do NOT want to set the crystal structure for the AxisODF widget
   * because we need that crystal structure to be OrthoRhombic in order for those
   * calculations to be performed correctly */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::updatePlots()
{
  if(getDataHasBeenGenerated() == true)
  {
    QProgressDialog progress("Generating Data ....", "Cancel", 0, 4, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(2000);

    progress.setValue(1);
    progress.setLabelText("[1/4] Calculating Size Distributions ...");
    getFeatureSizeWidget()->plotSizeDistribution();

    // Now that we have bins and feature sizes, push those to the other plot widgets
    // Setup Each Plot Widget
    // The MicroPreset class will set the distribution for each of the plots

    float mu = getFeatureSizeWidget()->getMu();
    float sigma = getFeatureSizeWidget()->getSigma();
    float minCutOff = getFeatureSizeWidget()->getMinCutOff();
    float maxCutOff = getFeatureSizeWidget()->getMaxCutOff();
    float binStepSize = getFeatureSizeWidget()->getBinStep();
    QwtArray<float> binSizes = getFeatureSizeWidget()->getBinSizes();

    getOmega3PlotWidget()->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);
    getMicroPreset()->initializeOmega3TableModel(getOmega3PlotWidget(), binSizes);

    getBOverAPlotPlotWidget()->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);
    getMicroPreset()->initializeBOverATableModel(getBOverAPlotPlotWidget(), binSizes);

    getCOverAPlotWidget()->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);
    getMicroPreset()->initializeCOverATableModel(getCOverAPlotWidget(), binSizes);

    // Get any presets for the ODF/AxisODF/MDF also
    getMicroPreset()->initializeODFTableModel(getODFWidgetWidget());
    getMicroPreset()->initializeAxisODFTableModel(getAxisODFWidget());
    getMicroPreset()->initializeMDFTableModel(getODFWidgetWidget()->getMDFWidget());

    progress.setValue(2);
    progress.setLabelText("[2/4] Calculating ODF Data ...");
    getODFWidgetWidget()->updatePlots();

    progress.setValue(3);
    progress.setLabelText("[3/4] Calculating Axis ODF Data ...");
    getAxisODFWidget()->updatePlots();

    progress.setValue(4);
    progress.setLabelText("[4/4] Calculating RDF Data ...");
    m_RdfPlot->updatePlots();

    setTabsPlotTabsEnabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::generateDefaultData()
{
  on_m_GenerateDefaultData_clicked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::on_m_GenerateDefaultData_clicked()
{
  setDataHasBeenGenerated(true);
  updatePlots();
  emit dataChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PrecipitatePhaseWidget::gatherStatsData(AttributeMatrix::Pointer attrMat, bool preflight)
{
  if(getPhaseIndex() < 1)
  {
    QMessageBox::critical(this, tr("StatsGenerator"), tr("The Phase Index is Less than 1. This is not allowed."), QMessageBox::Default);
    return -1;
  }
  int retErr = 0;
  int err = 0;
  float mu = getFeatureSizeWidget()->getMu();
  float sigma = getFeatureSizeWidget()->getSigma();
  float minCutOff = getFeatureSizeWidget()->getMinCutOff();
  float maxCutOff = getFeatureSizeWidget()->getMaxCutOff();
  float binStep = getFeatureSizeWidget()->getBinStep();

  float calcPhaseFraction = getPhaseFraction() / getTotalPhaseFraction();

  QwtArray<float> xCo;
  QwtArray<float> yCo;
  QwtArray<float> binsizes;
  float xMax = std::numeric_limits<float>::min();
  float yMax = std::numeric_limits<float>::min();
  QwtArray<float> x;
  QwtArray<float> y;
  err = getFeatureSizeWidget()->computeBinsAndCutOffs(mu, sigma, minCutOff, maxCutOff, binStep, binsizes, xCo, yCo, xMax, yMax, x, y);
  if(err < 0)
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
  unsigned int* crystalStructures = std::dynamic_pointer_cast<UInt32ArrayType>(iDataArray)->getPointer(0);
  iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::PhaseTypes);
  unsigned int* phaseTypes = std::dynamic_pointer_cast<UInt32ArrayType>(iDataArray)->getPointer(0);

  crystalStructures[getPhaseIndex()] = getCrystalStructure();
  phaseTypes[getPhaseIndex()] = getPhaseType();

  iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::PhaseName);
  StringDataArray::Pointer phaseNameArray = std::dynamic_pointer_cast<StringDataArray>(iDataArray);
  phaseNameArray->setValue(getPhaseIndex(), getPhaseName());

  StatsDataArray* statsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(attrMat->getAttributeArray(SIMPL::EnsembleData::Statistics).get());
  if(nullptr != statsDataArray)
  {
    StatsData::Pointer statsData = statsDataArray->getStatsData(getPhaseIndex());
    PrecipitateStatsData* precipitateStatsData = PrecipitateStatsData::SafePointerDownCast(statsData.get());

    precipitateStatsData->setPhaseFraction(calcPhaseFraction);
    precipitateStatsData->setPrecipBoundaryFraction(m_PptFraction);
    // Feature Diameter Info
    precipitateStatsData->setBinStepSize(stepSize);
    precipitateStatsData->setMaxFeatureDiameter(maxdiameter);
    precipitateStatsData->setMinFeatureDiameter(mindiameter);
    statsData->setName(getPhaseName());
    // Feature Size Distribution
    {
      VectorOfFloatArray data;
      FloatArrayType::Pointer d1 = FloatArrayType::CreateArray(1, SIMPL::StringConstants::Average);
      FloatArrayType::Pointer d2 = FloatArrayType::CreateArray(1, SIMPL::StringConstants::StandardDeviation);
      data.push_back(d1);
      data.push_back(d2);
      d1->setValue(0, avglogdiam);
      d2->setValue(0, sdlogdiam);
      precipitateStatsData->setFeatureSizeDistribution(data);
      precipitateStatsData->setFeatureSize_DistType(SIMPL::DistributionType::LogNormal);
    }

    // Now that we have bins and feature sizes, push those to the other plot widgets
    {
      VectorOfFloatArray data = getOmega3PlotWidget()->getStatisticsData();
      precipitateStatsData->setFeatureSize_Omegas(data);
      precipitateStatsData->setOmegas_DistType(getOmega3PlotWidget()->getDistributionType());
    }
    {
      VectorOfFloatArray data = getBOverAPlotPlotWidget()->getStatisticsData();
      precipitateStatsData->setFeatureSize_BOverA(data);
      precipitateStatsData->setBOverA_DistType(getBOverAPlotPlotWidget()->getDistributionType());
    }
    {
      VectorOfFloatArray data = getCOverAPlotWidget()->getStatisticsData();
      precipitateStatsData->setFeatureSize_COverA(data);
      precipitateStatsData->setCOverA_DistType(getCOverAPlotWidget()->getDistributionType());
    }
    {
      RdfData::Pointer data = m_RdfPlot->getStatisticsData();
      precipitateStatsData->setRadialDistFunction(data);
    }

    getODFWidgetWidget()->getOrientationData(precipitateStatsData, SIMPL::PhaseType::PrecipitatePhase, preflight);

    err = getAxisODFWidget()->getOrientationData(precipitateStatsData, SIMPL::PhaseType::PrecipitatePhase, preflight);
  }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::extractStatsData(AttributeMatrix::Pointer attrMat, int index)
{
  setWidgetListEnabled(true);
  setPhaseIndex(index);

  IDataArray::Pointer iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::CrystalStructures);
  unsigned int* attributeArray = std::dynamic_pointer_cast<UInt32ArrayType>(iDataArray)->getPointer(0);
  setCrystalStructure(attributeArray[index]);

  iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::PhaseTypes);
  attributeArray = std::dynamic_pointer_cast<UInt32ArrayType>(iDataArray)->getPointer(0);
  setPhaseType(attributeArray[index]);

  iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::Statistics);
  StatsDataArray* statsDataArray = StatsDataArray::SafeObjectDownCast<IDataArray*, StatsDataArray*>(iDataArray.get());
  if(statsDataArray == nullptr)
  {
    return;
  }
  StatsData::Pointer statsData = statsDataArray->getStatsData(index);
  PrecipitateStatsData* precipitateStatsData = PrecipitateStatsData::SafePointerDownCast(statsData.get());


  QString phaseName = statsData->getName();
  if(phaseName.isEmpty())
  {
    phaseName = QString("Precipitate Phase (%1)").arg(index);
  }
  setPhaseName(phaseName);  setPhaseFraction(precipitateStatsData->getPhaseFraction());
  m_PptFraction = precipitateStatsData->getPrecipBoundaryFraction();

  getFeatureSizeWidget()->setCrystalStructure(getCrystalStructure());
  getOmega3PlotWidget()->setCrystalStructure(getCrystalStructure());
  getBOverAPlotPlotWidget()->setCrystalStructure(getCrystalStructure());
  getCOverAPlotWidget()->setCrystalStructure(getCrystalStructure());
  getODFWidgetWidget()->setCrystalStructure(getCrystalStructure());

  /* Set the BinNumbers data set */
  FloatArrayType::Pointer bins = precipitateStatsData->getBinNumbers();

  // Now have each of the plots set it's own data
  QVector<float> qbins(static_cast<int>(bins->getNumberOfTuples()));
  for(int i = 0; i < qbins.size(); ++i)
  {
    qbins[i] = bins->getValue(i);
  }

  getFeatureSizeWidget()->extractStatsData(precipitateStatsData, index);

  float mu = getFeatureSizeWidget()->getMu();
  float sigma = getFeatureSizeWidget()->getSigma();
  float minCutOff = getFeatureSizeWidget()->getMinCutOff();
  float maxCutOff = getFeatureSizeWidget()->getMaxCutOff();
  float binStepSize = getFeatureSizeWidget()->getBinStep();

  getOmega3PlotWidget()->setDistributionType(precipitateStatsData->getOmegas_DistType(), false);
  getOmega3PlotWidget()->extractStatsData(index, qbins, precipitateStatsData->getFeatureSize_Omegas());
  getOmega3PlotWidget()->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  getBOverAPlotPlotWidget()->setDistributionType(precipitateStatsData->getBOverA_DistType(), false);
  getBOverAPlotPlotWidget()->extractStatsData(index, qbins, precipitateStatsData->getFeatureSize_BOverA());
  getBOverAPlotPlotWidget()->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  getCOverAPlotWidget()->setDistributionType(precipitateStatsData->getCOverA_DistType(), false);
  getCOverAPlotWidget()->extractStatsData(index, qbins, precipitateStatsData->getFeatureSize_COverA());
  getCOverAPlotWidget()->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  m_RdfPlot->extractStatsData(index, precipitateStatsData, SIMPL::PhaseType::PrecipitatePhase);

  // Set the ODF Data
  getODFWidgetWidget()->extractStatsData(index, precipitateStatsData, SIMPL::PhaseType::PrecipitatePhase);

  // Set the Axis ODF Data
  getAxisODFWidget()->extractStatsData(index, precipitateStatsData, SIMPL::PhaseType::PrecipitatePhase);

  // Enable all the tabs
  setTabsPlotTabsEnabled(true);
  setDataHasBeenGenerated(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon PrecipitatePhaseWidget::getPhaseIcon()
{
  QIcon icon;
  icon.addFile(QStringLiteral(":/StatsGenerator/icons/Precipitate.png"), QSize(), QIcon::Normal, QIcon::Off);
  icon.addFile(QStringLiteral(":/StatsGenerator/icons/Precipitate_Selected.png"), QSize(), QIcon::Normal, QIcon::On);
  return icon;
}
