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

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/DataArrays/StringDataArray.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/StatsData.h"

#include "OrientationLib/Texture/StatsGen.hpp"

#include "StatsGenerator/StatsGeneratorConstants.h"
#include "StatsGenerator/Widgets/Presets/MicrostructurePresetManager.h"
#include "StatsGenerator/Widgets/Presets/PrecipitateEquiaxedPreset.h"
#include "StatsGenerator/Widgets/Presets/PrecipitateRolledPreset.h"
#include "StatsGenerator/Widgets/StatsGenMDFWidget.h"
#include "StatsGenerator/Widgets/StatsGenRDFWidget.h"
#include "StatsGenerator/Widgets/TableModels/SGAbstractTableModel.h"
#include "StatsGenerator/Widgets/TableModels/SGMDFTableModel.h"
#include "StatsGenerator/Widgets/TableModels/SGODFTableModel.h"

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
  setPhaseType(PhaseType::Type::Precipitate);
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

  removeNeighborsPlotWidget();

  if(!m_RdfPlot)
  {
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
  }
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
  getODFWidget()->setPhaseIndex(index);
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
  getODFWidget()->setCrystalStructure(xtal);
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
    QwtArray<float> binSizes = getFeatureSizeWidget()->getBinSizes();
    QMap<QString, QVector<float>> data;
    data[AbstractMicrostructurePreset::kBinNumbers] = binSizes;
    QVector<QColor> colors;

    getMicroPreset()->initializeOmega3TableModel(data, colors);
    getOmega3PlotWidget()->setDistributionType(getMicroPreset()->getDistributionType(AbstractMicrostructurePreset::kOmega3Distribution), false);
    SGAbstractTableModel* tmodel = getOmega3PlotWidget()->tableModel();
    if(tmodel)
    {
      QVector<QVector<float>> colData;
      colData.push_back(data[AbstractMicrostructurePreset::kAlpha]);
      colData.push_back(data[AbstractMicrostructurePreset::kBeta]);
      tmodel->setTableData(binSizes, colData, colors);
    }

    getMicroPreset()->initializeBOverATableModel(data, colors);
    getBOverAPlotPlotWidget()->setDistributionType(getMicroPreset()->getDistributionType(AbstractMicrostructurePreset::kBOverADistribution), false);
    tmodel = getBOverAPlotPlotWidget()->tableModel();
    if(tmodel)
    {
      QVector<QVector<float>> colData;
      colData.push_back(data[AbstractMicrostructurePreset::kAlpha]);
      colData.push_back(data[AbstractMicrostructurePreset::kBeta]);
      tmodel->setTableData(binSizes, colData, colors);
    }

    getMicroPreset()->initializeCOverATableModel(data, colors);
    getCOverAPlotWidget()->setDistributionType(getMicroPreset()->getDistributionType(AbstractMicrostructurePreset::kCOverADistribution), false);
    tmodel = getCOverAPlotWidget()->tableModel();
    if(tmodel)
    {
      QVector<QVector<float>> colData;
      colData.push_back(data[AbstractMicrostructurePreset::kAlpha]);
      colData.push_back(data[AbstractMicrostructurePreset::kBeta]);
      tmodel->setTableData(binSizes, colData, colors);
    }
#if 0
       // Get any presets for the ODF/AxisODF/MDF also
       getMicroPreset()->initializeODFTableModel(getODFWidget());
       getMicroPreset()->initializeAxisODFTableModel(getAxisODFWidget());
       getMicroPreset()->initializeMDFTableModel(getODFWidget()->getMDFWidget());
#else
    // Get any presets for the ODF/AxisODF/MDF also
    getMicroPreset()->initializeODFTableModel(data);
    SGODFTableModel* model = getODFWidget()->tableModel();
    if(model)
    {
      model->setTableData(data[AbstractMicrostructurePreset::kEuler1], data[AbstractMicrostructurePreset::kEuler2], data[AbstractMicrostructurePreset::kEuler3],
                          data[AbstractMicrostructurePreset::kWeight], data[AbstractMicrostructurePreset::kSigma]);
    }

    getMicroPreset()->initializeAxisODFTableModel(data);
    model = getAxisODFWidget()->tableModel();
    if(model)
    {
      model->setTableData(data[AbstractMicrostructurePreset::kEuler1], data[AbstractMicrostructurePreset::kEuler2], data[AbstractMicrostructurePreset::kEuler3],
                          data[AbstractMicrostructurePreset::kWeight], data[AbstractMicrostructurePreset::kSigma]);
    }

    // getMicroPreset()->initializeMDFTableModel(m_ODFWidget->getMDFWidget());
    getMicroPreset()->initializeMDFTableModel(data);
    SGMDFTableModel* mdfModel = (getODFWidget()->getMDFWidget()->tableModel());
    if(mdfModel)
    {
      mdfModel->setTableData(data[AbstractMicrostructurePreset::kAngles], data[AbstractMicrostructurePreset::kAxis], data[AbstractMicrostructurePreset::kWeight]);
    }
#endif
    progress.setValue(2);
    progress.setLabelText("[2/4] Calculating ODF Data ...");
    getODFWidget()->updatePlots();

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
  phaseTypes[getPhaseIndex()] = static_cast<PhaseType::EnumType>(getPhaseType());

  iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::PhaseName);
  StringDataArray::Pointer phaseNameArray = std::dynamic_pointer_cast<StringDataArray>(iDataArray);
  phaseNameArray->setValue(getPhaseIndex(), getPhaseName());

  StatsDataArray::Pointer statsDataArray = std::dynamic_pointer_cast<StatsDataArray>(attrMat->getAttributeArray(SIMPL::EnsembleData::Statistics));
  if(nullptr != statsDataArray)
  {
    StatsData::Pointer statsData = statsDataArray->getStatsData(getPhaseIndex());
    PrecipitateStatsData::Pointer precipitateStatsData = std::dynamic_pointer_cast<PrecipitateStatsData>(statsData);

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

    getODFWidget()->getOrientationData(precipitateStatsData.get(), PhaseType::Type::Precipitate, preflight);

    err = getAxisODFWidget()->getOrientationData(precipitateStatsData.get(), PhaseType::Type::Precipitate, preflight);
  }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PrecipitatePhaseWidget::extractStatsData(AttributeMatrix::Pointer attrMat, int index)
{
  emit progressText(QString("Precipitate Phase extracting statistics..."));

  setWidgetListEnabled(true);
  setPhaseIndex(index);

  IDataArray::Pointer iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::CrystalStructures);
  unsigned int* attributeArray = std::dynamic_pointer_cast<UInt32ArrayType>(iDataArray)->getPointer(0);
  setCrystalStructure(attributeArray[index]);

  iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::PhaseTypes);
  attributeArray = std::dynamic_pointer_cast<UInt32ArrayType>(iDataArray)->getPointer(0);
  setPhaseType(static_cast<PhaseType::Type>(attributeArray[index]));

  iDataArray = attrMat->getAttributeArray(SIMPL::EnsembleData::Statistics);
  StatsDataArray::Pointer statsDataArray = std::dynamic_pointer_cast<StatsDataArray>(iDataArray);
  if(statsDataArray == nullptr)
  {
    return;
  }
  StatsData::Pointer statsData = statsDataArray->getStatsData(index);
  PrecipitateStatsData::Pointer precipitateStatsData = std::dynamic_pointer_cast<PrecipitateStatsData>(statsData);

  QString phaseName = statsData->getName();
  if(phaseName.isEmpty())
  {
    phaseName = QString("Precipitate Phase (%1)").arg(index);
  }
  setPhaseName(phaseName);
  setTabTitle(phaseName);
  setPhaseFraction(precipitateStatsData->getPhaseFraction());
  m_PptFraction = precipitateStatsData->getPrecipBoundaryFraction();

  getFeatureSizeWidget()->setCrystalStructure(getCrystalStructure());
  getOmega3PlotWidget()->setCrystalStructure(getCrystalStructure());
  getBOverAPlotPlotWidget()->setCrystalStructure(getCrystalStructure());
  getCOverAPlotWidget()->setCrystalStructure(getCrystalStructure());
  getODFWidget()->setCrystalStructure(getCrystalStructure());

  /* Set the BinNumbers data set */
  FloatArrayType::Pointer bins = precipitateStatsData->getBinNumbers();

  // Now have each of the plots set it's own data
  QVector<float> qbins(static_cast<int>(bins->getNumberOfTuples()));
  for(int i = 0; i < qbins.size(); ++i)
  {
    qbins[i] = bins->getValue(i);
  }
  emit progressText(QString("Extracting Size Distribution Values"));
  qApp->processEvents();
  getFeatureSizeWidget()->extractStatsData(precipitateStatsData.get(), index);

  float mu = getFeatureSizeWidget()->getMu();
  float sigma = getFeatureSizeWidget()->getSigma();
  float minCutOff = getFeatureSizeWidget()->getMinCutOff();
  float maxCutOff = getFeatureSizeWidget()->getMaxCutOff();
  float binStepSize = getFeatureSizeWidget()->getBinStep();
  emit progressText(QString("Extracting Omega 3 Distribution Values"));
  qApp->processEvents();
  getOmega3PlotWidget()->setDistributionType(precipitateStatsData->getOmegas_DistType(), false);
  getOmega3PlotWidget()->extractStatsData(index, qbins, precipitateStatsData->getFeatureSize_Omegas());
  getOmega3PlotWidget()->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);
  emit progressText(QString("Extracting B Over a Distribution Values"));
  qApp->processEvents();
  getBOverAPlotPlotWidget()->setDistributionType(precipitateStatsData->getBOverA_DistType(), false);
  getBOverAPlotPlotWidget()->extractStatsData(index, qbins, precipitateStatsData->getFeatureSize_BOverA());
  getBOverAPlotPlotWidget()->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);
  emit progressText(QString("Extracting C Over A Distribution Values"));
  qApp->processEvents();
  getCOverAPlotWidget()->setDistributionType(precipitateStatsData->getCOverA_DistType(), false);
  getCOverAPlotWidget()->extractStatsData(index, qbins, precipitateStatsData->getFeatureSize_COverA());
  getCOverAPlotWidget()->setSizeDistributionValues(mu, sigma, minCutOff, maxCutOff, binStepSize);

  m_RdfPlot->extractStatsData(index, precipitateStatsData.get(), PhaseType::Type::Precipitate);

  emit progressText(QString("Extracting ODF Distribution Values"));
  qApp->processEvents();
  // Set the ODF Data
  getODFWidget()->extractStatsData(index, precipitateStatsData.get(), PhaseType::Type::Precipitate);

  emit progressText(QString("Extracting Axis ODF Distribution Values"));
  qApp->processEvents();
  // Set the Axis ODF Data
  getAxisODFWidget()->extractStatsData(index, precipitateStatsData.get(), PhaseType::Type::Precipitate);

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
