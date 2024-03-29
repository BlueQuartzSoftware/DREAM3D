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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "StatsGenODFWidget.h"

#include "SIMPLib/Math/SIMPLibMath.h"

//-- C++ Includes
#include <iostream>
#include <vector>

//-- Qt Includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QVector>

#include <QtWidgets/QAbstractItemDelegate>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <qwt.h>
#include <qwt_abstract_scale_draw.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_scale_draw.h>
#include <qwt_symbol.h>

#include "SVWidgetsLib/Widgets/ProgressDialog.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "EbsdLib/Core/EbsdMacros.h"
#include "EbsdLib/IO/AngleFileLoader.h"
#include "EbsdLib/IO/HKL/CtfReader.h"
#include "EbsdLib/IO/TSL/AngReader.h"
#include "EbsdLib/LaueOps/CubicLowOps.h"
#include "EbsdLib/LaueOps/CubicOps.h"
#include "EbsdLib/LaueOps/HexagonalLowOps.h"
#include "EbsdLib/LaueOps/HexagonalOps.h"
#include "EbsdLib/LaueOps/LaueOps.h"
#include "EbsdLib/LaueOps/MonoclinicOps.h"
#include "EbsdLib/LaueOps/OrthoRhombicOps.h"
#include "EbsdLib/LaueOps/TetragonalLowOps.h"
#include "EbsdLib/LaueOps/TetragonalOps.h"
#include "EbsdLib/LaueOps/TriclinicOps.h"
#include "EbsdLib/LaueOps/TrigonalLowOps.h"
#include "EbsdLib/LaueOps/TrigonalOps.h"
#include "EbsdLib/Texture/StatsGen.hpp"
#include "EbsdLib/Texture/Texture.hpp"
#include "EbsdLib/Utilities/PoleFigureUtilities.h"

#include "SyntheticBuilding/Gui/Utilities/PoleFigureImageUtilities.h"
#include "SyntheticBuilding/Gui/Widgets/StatsGenMDFWidget.h"
#include "SyntheticBuilding/Gui/Widgets/StatsProgressWidget.h"
#include "SyntheticBuilding/Gui/Widgets/TableModels/SGODFTableModel.h"
#include "SyntheticBuilding/Gui/Widgets/TextureDialog.h"
#include "SyntheticBuilding/SyntheticBuildingFilters/StatsGeneratorUtilities.h"

#define SHOW_POLE_FIGURES 1
#define COLOR_POLE_FIGURES 1

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenODFWidget::StatsGenODFWidget(QWidget* parent)
: QWidget(parent)
, m_EnableAxisDecorations(false)
, m_Initializing(true)
, m_PhaseIndex(-1)
, m_CrystalStructure(EbsdLib::CrystalStructure::Cubic_High)
{
  m_OpenDialogLastFilePath = QDir::homePath();
  this->setupUi(this);
  this->setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenODFWidget::~StatsGenODFWidget()
{
  if(nullptr != m_ODFTableModel)
  {
    m_ODFTableModel->deleteLater();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_m_WeightSpreads_clicked(bool /* checked */)
{
  m_WeightSpreadsStackedWidget->setCurrentIndex(0);
  m_ODFTableView->setModel(m_ODFTableModel);
  QAbstractItemDelegate* idelegate = m_ODFTableModel->getItemDelegate();
  m_ODFTableView->setItemDelegate(idelegate);
  Q_EMIT bulkLoadEvent(false);
  Q_EMIT dataChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_m_WeightSpreadsBulkLoad_clicked(bool /* checked */)
{
  m_WeightSpreadsStackedWidget->setCurrentIndex(1);
  m_ODFTableView->setModel(m_OdfBulkTableModel);
  QAbstractItemDelegate* idelegate = m_OdfBulkTableModel->getItemDelegate();
  m_ODFTableView->setItemDelegate(idelegate);
  Q_EMIT bulkLoadEvent(!(m_OdfBulkTableModel->rowCount() > 0));
  Q_EMIT dataChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::extractStatsData(int index, StatsData* statsData, PhaseType::Type phaseType)
{
  VectorOfFloatArray arrays;
  if(phaseType == PhaseType::Type::Primary)
  {
    PrimaryStatsData* pp = dynamic_cast<PrimaryStatsData*>(statsData);
    arrays = pp->getODF_Weights();
  }
  if(phaseType == PhaseType::Type::Precipitate)
  {
    PrecipitateStatsData* pp = dynamic_cast<PrecipitateStatsData*>(statsData);
    arrays = pp->getODF_Weights();
  }
  if(phaseType == PhaseType::Type::Transformation)
  {
    TransformationStatsData* tp = dynamic_cast<TransformationStatsData*>(statsData);
    arrays = tp->getODF_Weights();
  }
  if(!arrays.empty())
  {
    QVector<float> e1(static_cast<int>(arrays[0]->getNumberOfTuples()));
    ::memcpy(e1.data(), arrays[0]->getVoidPointer(0), sizeof(float) * static_cast<size_t>(e1.size()));

    QVector<float> e2(static_cast<int>(arrays[1]->getNumberOfTuples()));
    ::memcpy(e2.data(), arrays[1]->getVoidPointer(0), sizeof(float) * static_cast<size_t>(e2.size()));

    QVector<float> e3(static_cast<int>(arrays[2]->getNumberOfTuples()));
    ::memcpy(e3.data(), arrays[2]->getVoidPointer(0), sizeof(float) * static_cast<size_t>(e3.size()));

    QVector<float> weights(static_cast<int>(arrays[4]->getNumberOfTuples()));
    ::memcpy(weights.data(), arrays[4]->getVoidPointer(0), sizeof(float) * static_cast<size_t>(weights.size()));

    QVector<float> sigmas(static_cast<int>(arrays[3]->getNumberOfTuples()));
    ::memcpy(sigmas.data(), arrays[3]->getVoidPointer(0), sizeof(float) * static_cast<size_t>(sigmas.size()));

    // Convert from Radians to Degrees for the Euler Angles
    for(int i = 0; i < e1.size(); ++i)
    {
      e1[i] = e1[i] * SIMPLib::Constants::k_RadToDegF;
      e2[i] = e2[i] * SIMPLib::Constants::k_RadToDegF;
      e3[i] = e3[i] * SIMPLib::Constants::k_RadToDegF;
    }

    if(!e1.empty())
    {
      // Load the data into the table model
      m_ODFTableModel->setTableData(e1, e2, e3, weights, sigmas);
    }
  }
  updatePlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenODFWidget::getOrientationData(StatsData* statsData, PhaseType::Type phaseType, bool preflight)
{
  int retErr = 0;

  std::vector<float> e1s;
  std::vector<float> e2s;
  std::vector<float> e3s;
  std::vector<float> weights;
  std::vector<float> sigmas;

  SGODFTableModel* tableModel = nullptr;

  if(m_WeightSpreads->isChecked())
  {
    tableModel = m_ODFTableModel;
  }
  else
  {
    tableModel = m_OdfBulkTableModel;
  }

  e1s = tableModel->getData(SGODFTableModel::Euler1);
  e2s = tableModel->getData(SGODFTableModel::Euler2);
  e3s = tableModel->getData(SGODFTableModel::Euler3);
  weights = tableModel->getData(SGODFTableModel::Weight);
  sigmas = tableModel->getData(SGODFTableModel::Sigma);

  // Convert from Degrees to Radians
  for(std::vector<float>::size_type i = 0; i < e1s.size(); i++)
  {
    e1s[i] = e1s[i] * SIMPLib::Constants::k_PiOver180F;
    e2s[i] = e2s[i] * SIMPLib::Constants::k_PiOver180F;
    e3s[i] = e3s[i] * SIMPLib::Constants::k_PiOver180F;
  }

  StatsGeneratorUtilities::GenerateODFBinData(statsData, phaseType, m_CrystalStructure, e1s, e2s, e3s, weights, sigmas, !preflight);

  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::setCrystalStructure(unsigned int value)
{
  if(m_CrystalStructure != value)
  {
    this->m_CrystalStructure = value;
    m_ODFTableModel->setCrystalStructure(m_CrystalStructure);
    QAbstractItemDelegate* idelegate = m_ODFTableModel->getItemDelegate();
    m_ODFTableView->setItemDelegate(idelegate);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int StatsGenODFWidget::getCrystalStructure()
{
  return m_CrystalStructure;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::setPhaseIndex(int value)
{
  this->m_PhaseIndex = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::updatePFStatus(const QString& msg)
{
  m_PFStatusLabel->setText(msg);
  m_PFStatusLabel->setVisible(!msg.isEmpty());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenODFWidget::getPhaseIndex()
{
  return m_PhaseIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_m_PFTypeCB_currentIndexChanged(int index)
{
  m_PFLambertLabel->setVisible((index == 1));
  m_PFLambertSize->setVisible((index == 1));
  updatePFStatus(QString("Click Refresh Button"));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_m_PFImageSize_valueChanged(int i)
{
  Q_UNUSED(i)
  updatePFStatus(QString("Click Refresh Button"));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_m_ImageLayoutCB_currentIndexChanged(int index)
{
  updatePFStatus(QString("Click Refresh Button"));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_m_PFLambertSize_valueChanged(int i)
{
  updatePFStatus(QString("Click Refresh Button"));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::tableDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
  Q_UNUSED(topLeft);
  Q_UNUSED(bottomRight);
  updatePlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::setupGui()
{
  // Setup the TableView and Table Models
  QHeaderView* headerView = new QHeaderView(Qt::Horizontal, m_ODFTableView);
  headerView->sectionResizeMode(QHeaderView::Interactive);
  m_ODFTableView->setHorizontalHeader(headerView);
  headerView->show();

  m_OdfBulkTableModel = new SGODFTableModel;
  m_OdfBulkTableModel->setCrystalStructure(m_CrystalStructure);
  m_OdfBulkTableModel->setInitialValues();
  // m_ODFTableView->setModel(m_OdfBulkTableModel);

  m_ODFTableModel = new SGODFTableModel;
  m_ODFTableModel->setCrystalStructure(m_CrystalStructure);
  m_ODFTableModel->setInitialValues();
  m_ODFTableView->setModel(m_ODFTableModel);

  connect(m_ODFTableModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(tableDataChanged(const QModelIndex&, const QModelIndex&)));

  QAbstractItemDelegate* idelegate = m_ODFTableModel->getItemDelegate();
  m_ODFTableView->setItemDelegate(idelegate);

  addODFTextureBtn->setToolTip("Add ODF Weight");
  deleteODFTextureBtn->setToolTip("Delete ODF Weight");

  m_PlotCurves.push_back(new QwtPlotCurve);
  m_PlotCurves.push_back(new QwtPlotCurve);
  m_PlotCurves.push_back(new QwtPlotCurve);

  m_ButtonGroup.addButton(m_WeightSpreads);
  m_ButtonGroup.addButton(m_WeightSpreadsBulkLoad);
  on_m_WeightSpreads_clicked(true);

  // In release mode hide the Lambert Square Size.
  // QString releaseType = QString::fromLatin1(SIMPLProj_RELEASE_TYPE);
  // if(releaseType.compare("Official") == 0)
  {
    m_PFLambertSize->hide();
    m_PFLambertLabel->hide();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot)
{
  plot->setAxisTitle(QwtPlot::xBottom, xAxisName);
  plot->setAxisTitle(QwtPlot::yLeft, yAxisName);
  plot->setCanvasBackground(QColor(Qt::white));

  // Lock the Axis Min/Max to -1 to 1 effectively cropping the plot. If there are
  // data points outside of that range they will never be shown.
  plot->setAxisScale(QwtPlot::xBottom, -1.0, 1.0);
  plot->setAxisScale(QwtPlot::yLeft, -1.0, 1.0);

  // These set the plot axis to NOT show anything except the axis labels.
  plot->axisScaleDraw(QwtPlot::yLeft)->enableComponent(QwtAbstractScaleDraw::Backbone, m_EnableAxisDecorations);
  plot->axisScaleDraw(QwtPlot::yLeft)->enableComponent(QwtAbstractScaleDraw::Ticks, m_EnableAxisDecorations);
  plot->axisScaleDraw(QwtPlot::yLeft)->enableComponent(QwtAbstractScaleDraw::Labels, m_EnableAxisDecorations);
  plot->axisScaleDraw(QwtPlot::xBottom)->enableComponent(QwtAbstractScaleDraw::Backbone, m_EnableAxisDecorations);
  plot->axisScaleDraw(QwtPlot::xBottom)->enableComponent(QwtAbstractScaleDraw::Ticks, m_EnableAxisDecorations);
  plot->axisScaleDraw(QwtPlot::xBottom)->enableComponent(QwtAbstractScaleDraw::Labels, m_EnableAxisDecorations);

  drawODFPlotGrid(plot);
}

// -----------------------------------------------------------------------------
//  We should just subclass QwtPlotGrid and do all the drawing there.
// -----------------------------------------------------------------------------
void StatsGenODFWidget::drawODFPlotGrid(QwtPlot* plot)
{
  // Draw the accepted Inverse Pole Grid Style

  // Draw the Horizontal and Vertical Lines that form the central cross
  m_PlotGrid = new QwtPlotMarker();
  m_PlotGrid->attach(plot);
  m_PlotGrid->setLineStyle(QwtPlotMarker::Cross);
  m_PlotGrid->setLinePen(QPen(Qt::darkMagenta, 1, Qt::SolidLine));
  m_PlotGrid->setXValue(0);
  m_PlotGrid->setYValue(0);

  // Draw the outer Circle
  QwtArray<double> circleX(900); // 900 because our plots are hard set to 450 pixels
  QwtArray<double> circleY(900);
  float inc = 2.0f / 449.0f;

  for(int i = 0; i < 450; ++i)
  {
    circleX[i] = 1.0 - static_cast<double>(i * inc);
    circleX[450 + i] = -1.0 + static_cast<double>(i * inc);

    circleY[i] = sqrt(1.0 - (circleX[i] * circleX[i]));
    circleY[450 + i] = -circleY[i];
  }
  m_CircleGrid = new QwtPlotCurve;
#if QWT_VERSION >= 0x060000
  m_CircleGrid->setSamples(circleX, circleY);
#else
  m_CircleGrid->setData(circleX, circleY);
#endif
  QColor c = QColor(Qt::darkMagenta);
  c.setAlpha(255);
  m_CircleGrid->setPen(QPen(c));
  m_CircleGrid->setStyle(QwtPlotCurve::Lines);
  m_CircleGrid->attach(plot);

  // Draw the Rotated Central cross
  {
    QwtArray<double> rotCrossX(2);
    QwtArray<double> rotCrossY(2);
    rotCrossX[0] = 0.7071067811;
    rotCrossY[0] = sqrt(1.0 - (rotCrossX[0] * rotCrossX[0]));
    rotCrossX[1] = -0.7071067811;
    rotCrossY[1] = -sqrt(1.0 - (rotCrossX[1] * rotCrossX[1]));
    m_RotCross0 = new QwtPlotCurve;
#if QWT_VERSION >= 0x060000
    m_RotCross0->setSamples(rotCrossX, rotCrossY);
#else
    m_RotCross0->setData(rotCrossX, rotCrossY);
#endif
    QColor c = QColor(Qt::darkMagenta);
    c.setAlpha(200);
    m_RotCross0->setPen(QPen(c));
    m_RotCross0->setStyle(QwtPlotCurve::Lines);
    m_RotCross0->attach(plot);
  }

  {
    QwtArray<double> rotCrossX(2);
    QwtArray<double> rotCrossY(2);
    rotCrossX[0] = 0.7071067811;
    rotCrossY[0] = -sqrt(1.0 - (rotCrossX[0] * rotCrossX[0]));
    rotCrossX[1] = -0.7071067811;
    rotCrossY[1] = sqrt(1.0 - (rotCrossX[1] * rotCrossX[1]));
    m_RotCross1 = new QwtPlotCurve;
#if QWT_VERSION >= 0x060000
    m_RotCross1->setSamples(rotCrossX, rotCrossY);
#else
    m_RotCross1->setData(rotCrossX, rotCrossY);
#endif
    QColor c = QColor(Qt::darkMagenta);
    c.setAlpha(200);
    m_RotCross1->setPen(QPen(c));
    m_RotCross1->setStyle(QwtPlotCurve::Lines);
    m_RotCross1->attach(plot);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::updatePlots()
{
  m_AbortUpdate = false;
  calculateODF();
  m_AbortUpdate = true;
  updatePFStatus(QString(""));
  Q_EMIT odfDataChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_m_CalculateODFBtn_clicked()
{
  updatePlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::calculateODF()
{
  int err = 0;
  if(m_AbortUpdate)
  {
    return;
  }
  StatsProgressWidget progress("Calculating/Updating the ODF", "Cancel", nullptr);
  progress.setLabelText("Please Wait...");
  progress.setProgTitle("Generating ODF Plot Data...");
  progress.setVisible(true);
  progress.show();
  qApp->processEvents();

  progress.setValue(0);
  progress.setLabelText("Calculating ODF");

  using ContainerType = std::vector<float>;
  ContainerType e1s;
  ContainerType e2s;
  ContainerType e3s;
  ContainerType weights;
  ContainerType sigmas;
  ContainerType odf;

  SGODFTableModel* tableModel = nullptr;

  int npoints = 0;
  if(m_WeightSpreads->isChecked())
  {
    tableModel = m_ODFTableModel;
    npoints = pfSamplePoints->value();
  }
  else
  {
    tableModel = m_OdfBulkTableModel;
    npoints = tableModel->rowCount();
  }

  e1s = tableModel->getData(SGODFTableModel::Euler1);
  e2s = tableModel->getData(SGODFTableModel::Euler2);
  e3s = tableModel->getData(SGODFTableModel::Euler3);
  weights = tableModel->getData(SGODFTableModel::Weight);
  sigmas = tableModel->getData(SGODFTableModel::Sigma);

  // Convert from Degrees to Radians
  for(ContainerType::size_type i = 0; i < e1s.size(); i++)
  {
    e1s[i] = e1s[i] * SIMPLib::Constants::k_PiOver180F;
    e2s[i] = e2s[i] * SIMPLib::Constants::k_PiOver180F;
    e3s[i] = e3s[i] * SIMPLib::Constants::k_PiOver180F;
  }
  size_t numEntries = static_cast<size_t>(e1s.size());

  int imageSize = m_PFImageSize->value();
  int lamberSize = m_PFLambertSize->value();
  int numColors = 16;

  std::vector<size_t> dims(1, 3);
  EbsdLib::FloatArrayType::Pointer eulers = EbsdLib::FloatArrayType::CreateArray(npoints, dims, "Eulers", true);
  PoleFigureConfiguration_t config;
  config.eulers = eulers.get();
  config.imageDim = imageSize;
  config.lambertDim = lamberSize;
  config.numColors = numColors;
  config.discrete = true;
  config.discreteHeatMap = false;

  // Check if the user wants a Discreet or Lambert PoleFigure
  if(m_PFTypeCB->currentIndex() == 1)
  {
    config.discrete = false;
  }
  std::vector<EbsdLib::UInt8ArrayType::Pointer> figures;

  //// ODF/MDF Update Codes
  LaueOps::Pointer ops = LaueOps::NullPointer();
  switch(m_CrystalStructure)
  {
  case EbsdLib::CrystalStructure::Triclinic: // 4; Triclinic -1
    Texture::CalculateODFData<float, TriclinicOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
    err = StatsGen::GenODFPlotData<float, TriclinicOps, ContainerType>(odf, eulers->getPointer(0), npoints);
    ops = TriclinicOps::New();
    break;
  case EbsdLib::CrystalStructure::Monoclinic: // 5; Monoclinic 2/m
    Texture::CalculateODFData<float, MonoclinicOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
    err = StatsGen::GenODFPlotData<float, MonoclinicOps, ContainerType>(odf, eulers->getPointer(0), npoints);
    ops = MonoclinicOps::New();
    break;
  case EbsdLib::CrystalStructure::OrthoRhombic: // 6; Orthorhombic mmm
    Texture::CalculateODFData<float, OrthoRhombicOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
    err = StatsGen::GenODFPlotData<float, OrthoRhombicOps, ContainerType>(odf, eulers->getPointer(0), npoints);
    ops = OrthoRhombicOps::New();
    break;
  case EbsdLib::CrystalStructure::Tetragonal_Low: // 7; Tetragonal-Low 4/m
    Texture::CalculateODFData<float, TetragonalLowOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
    err = StatsGen::GenODFPlotData<float, TetragonalLowOps, ContainerType>(odf, eulers->getPointer(0), npoints);
    ops = TetragonalLowOps::New();
    break;
  case EbsdLib::CrystalStructure::Tetragonal_High: // 8; Tetragonal-High 4/mmm
    Texture::CalculateODFData<float, TetragonalOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
    err = StatsGen::GenODFPlotData<float, TetragonalOps, ContainerType>(odf, eulers->getPointer(0), npoints);
    ops = TetragonalOps::New();
    break;
  case EbsdLib::CrystalStructure::Trigonal_Low: // 9; Trigonal-Low -3
    Texture::CalculateODFData<float, TrigonalLowOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
    err = StatsGen::GenODFPlotData<float, TrigonalLowOps, ContainerType>(odf, eulers->getPointer(0), npoints);
    ops = TrigonalLowOps::New();
    break;
  case EbsdLib::CrystalStructure::Trigonal_High: // 10; Trigonal-High -3m
    Texture::CalculateODFData<float, TrigonalOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
    err = StatsGen::GenODFPlotData<float, TrigonalOps, ContainerType>(odf, eulers->getPointer(0), npoints);
    ops = TrigonalOps::New();
    break;
  case EbsdLib::CrystalStructure::Hexagonal_Low: // 2; Hexagonal-Low 6/m
    Texture::CalculateODFData<float, HexagonalLowOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
    err = StatsGen::GenODFPlotData<float, HexagonalLowOps, ContainerType>(odf, eulers->getPointer(0), npoints);
    ops = HexagonalLowOps::New();
    break;
  case EbsdLib::CrystalStructure::Hexagonal_High: // 0; Hexagonal-High 6/mmm
    Texture::CalculateODFData<float, HexagonalOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
    err = StatsGen::GenODFPlotData<float, HexagonalOps, ContainerType>(odf, eulers->getPointer(0), npoints);
    ops = HexagonalOps::New();
    break;
  case EbsdLib::CrystalStructure::Cubic_Low: // 3; Cubic Cubic-Low m3 (Tetrahedral)
    Texture::CalculateODFData<float, CubicLowOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
    err = StatsGen::GenODFPlotData<float, CubicLowOps, ContainerType>(odf, eulers->getPointer(0), npoints);
    ops = CubicLowOps::New();
    break;
  case EbsdLib::CrystalStructure::Cubic_High: // 1; Cubic Cubic-High m3m
    Texture::CalculateODFData<float, CubicOps, ContainerType>(e1s, e2s, e3s, weights, sigmas, true, odf, numEntries);
    err = StatsGen::GenODFPlotData<float, CubicOps, ContainerType>(odf, eulers->getPointer(0), npoints);
    ops = CubicOps::New();
    break;
  default:
    err = -1;
    break;
  }

  if(err < 0)
  {
    QString ss("Error creating the ODF Plots");
    QMessageBox::StandardButton reply;
    reply = QMessageBox::critical(nullptr, QString("ODF Generation Error"), ss, QMessageBox::Ok);
    Q_UNUSED(reply);
  }
  else
  {
    if(nullptr != ops.get())
    {
      figures = ops->generatePoleFigure(config);
    }

    std::vector<UInt8ArrayType::Pointer> convertedFigures;
    for(const auto& figure : figures)
    {
      convertedFigures.emplace_back(figure->moveToDataArrayType<UInt8ArrayType>());
    }

    QImage image = PoleFigureImageUtilities::Create3ImagePoleFigure(convertedFigures[0].get(), convertedFigures[1].get(), convertedFigures[2].get(), config, m_ImageLayoutCB->currentIndex());
    m_PoleFigureLabel->setPixmap(QPixmap::fromImage(image));
    Q_EMIT dataChanged();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_addODFTextureBtn_clicked()
{
  TextureDialog t(m_CrystalStructure, this);
  int r = t.exec();
  if(r == QDialog::Accepted)
  {
    m_AbortUpdate = true;
    if(!m_ODFTableModel->insertRow(m_ODFTableModel->rowCount()))
    {
      return;
    }
    m_AbortUpdate = false;
    // Gather values from the dialog and push them to the Table Model
    float e1 = 0.0;
    float e2 = 0.0;
    float e3 = 0.0;
    float weight = 500000.0;
    float sigma = 1.0;

    t.getODFEntry(e1, e2, e3, weight, sigma);
    int row = m_ODFTableModel->rowCount() - 1;
    m_ODFTableModel->setRowData(row, e1, e2, e3, weight, sigma);

    m_ODFTableView->resizeColumnsToContents();
    m_ODFTableView->scrollToBottom();
    m_ODFTableView->setFocus();
    QModelIndex index = m_ODFTableModel->index(m_ODFTableModel->rowCount() - 1, 0);
    m_ODFTableView->setCurrentIndex(index);
    updatePlots();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_selectAnglesFile_clicked()
{
  QString proposedFile = m_OpenDialogLastFilePath;
  if(!angleFilePath->text().isEmpty())
  {
    proposedFile = angleFilePath->text();
  }
  QString file = QFileDialog::getOpenFileName(this, tr("Select Angles File"), proposedFile, tr("Text Documents (*.txt);;CSV (*.csv);;All Files(*.*)"));
  if(!file.isEmpty())
  {
    angleFilePath->setText(file);
    m_OpenDialogLastFilePath = file;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_angleFilePath_textChanged()
{
  // If the text has changed, we don't know if the file exists/is valid,
  // so blow away the bulk table model, if it exists, and emit that
  // the user needs to select a valid file and reload
  if(m_OdfBulkTableModel != nullptr)
  {
    delete m_OdfBulkTableModel;
    m_OdfBulkTableModel = nullptr;
  }
  m_OdfBulkTableModel = new SGODFTableModel;
  m_OdfBulkTableModel->setCrystalStructure(m_CrystalStructure);
  m_OdfBulkTableModel->setInitialValues();

  Q_EMIT bulkLoadEvent(true);
  updatePlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_loadODFTextureBtn_clicked()
{
  QString file = angleFilePath->text();
  if(file.isEmpty())
  {
    Q_EMIT bulkLoadEvent(true);
    return;
  }

  QFileInfo fi(angleFilePath->text());
  if(!fi.exists())
  {
    Q_EMIT bulkLoadEvent(true);
    return;
  }

  size_t count = 0;
  QVector<float> e1s(count);
  QVector<float> e2s(count);
  QVector<float> e3s(count);
  QVector<float> weights(count);
  QVector<float> sigmas(count);

  QProgressDialog progress("Loading Data ....", "Cancel", 0, 3, this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(2000);
  progress.show();

  if(fi.suffix() == S2Q(EbsdLib::Ang::FileExt))
  {
    QMessageBox::critical(this, "ANG File Loading NOT Supported",
                          "Please use the 'Write StatsGenerator ODF Angle File' filter from DREAM.3D to generate a file. See that filter's help for the proper format.", QMessageBox::Ok);
    Q_EMIT bulkLoadEvent(true);
    return;
#if 0
    phaseOfInterest->setEnabled(true);
    anglesInDegrees->setEnabled(true);
    angleRepresentation->setEnabled(false);
    delimiter->setEnabled(false);
    progress.setValue(1);
    progress.setLabelText("[1/3] Reading File ...");
    AngReader loader;
    loader.setFileName(angleFilePath->text());
    loader.setReadHexGrid(true);
    int err = loader.readFile();
    if(err < 0)
    {
      QMessageBox::critical(this, "Error loading the ANG file", loader.getErrorMessage(), QMessageBox::Ok);
      return;
    }
    float* phi1 = loader.getPhi1Pointer();
    float* phi = loader.getPhiPointer();
    float* phi2 = loader.getPhi2Pointer();
    int32_t* phase = loader.getPhaseDataPointer();
    QVector<AngPhase::Pointer> phaseInfo = loader.getPhaseVector();
    phaseOfInterest->setRange(0, phaseInfo.size());
    int phaseSelection = phaseOfInterest->value();

    int xDim = loader.getXDimension();
    int yDim = loader.getYDimension();
    size_t totalScanPoints = xDim * yDim;


    count = 0;
    for(size_t i = 0; i < totalScanPoints; ++i)
    {
      if(phase[i] == phaseSelection)
      {
        count++;
      }
    }

    e1s.resize(count);
    e2s.resize(count);
    e3s.resize(count);
    weights.resize(count);
    sigmas.resize(count);
    count = 0;
    for(size_t i = 0; i < totalScanPoints; ++i)
    {
      if(phase[i] == phaseSelection)
      {
        // Be sure to convert the ANG Radians into degrees because this part of
        // DREAM.3D stores the angles as Degrees.
        e1s[count] = phi1[i] * SIMPLib::Constants::k_180OverPiD;
        e2s[count] = phi[i] * SIMPLib::Constants::k_180OverPiD;
        e3s[count] = phi2[i] * SIMPLib::Constants::k_180OverPiD;
        weights[count] = 1.0;
        sigmas[count] = 0.0;
        count++;
      }
    }
#endif
  }
  if(fi.suffix() == S2Q(EbsdLib::Ctf::FileExt))
  {
    QMessageBox::critical(this, "CTF File Loading not Supported",
                          "Please use the 'Write StatsGenerator ODF Angle File' filter from DREAM.3D to generate a file. See that filter's help for the proper format.", QMessageBox::Ok);
    Q_EMIT bulkLoadEvent(true);
    return;
#if 0
    phaseOfInterest->setEnabled(true);
    anglesInDegrees->setEnabled(true);
    angleRepresentation->setEnabled(false);
    delimiter->setEnabled(false);
    progress.setValue(1);
    progress.setLabelText("[1/3] Reading File ...");
    CtfReader loader;
    loader.setFileName(angleFilePath->text());
    int err = loader.readFile();
    if(err < 0)
    {
      QMessageBox::critical(this, "Error loading the CTF file", loader.getErrorMessage(), QMessageBox::Ok);
      return;
    }
    float* phi1 = loader.getEuler1Pointer();
    float* phi = loader.getEuler2Pointer();
    float* phi2 = loader.getEuler3Pointer();
    int32_t* phase = loader.getPhasePointer();
    QVector<CtfPhase::Pointer> phaseInfo = loader.getPhaseVector();
    phaseOfInterest->setRange(0, phaseInfo.size());
    int phaseSelection = phaseOfInterest->value();

    int xDim = loader.getXDimension();
    int yDim = loader.getYDimension();
    size_t totalScanPoints = xDim * yDim;

    count = 0;
    for(size_t i = 0; i < totalScanPoints; ++i)
    {
      if(phase[i] == phaseSelection)
      {
        count++;
      }
    }

    e1s.resize(count);
    e2s.resize(count);
    e3s.resize(count);
    weights.resize(count);
    sigmas.resize(count);
    count = 0;
    for(size_t i = 0; i < totalScanPoints; ++i)
    {
      if(phase[i] == phaseSelection)
      {
        e1s[count] = phi1[i];
        e2s[count] = phi[i];
        e3s[count] = phi2[i];
        weights[count] = 1.0;
        sigmas[count] = 0.0;
        count++;
      }
    }
#endif
  }

  anglesInDegrees->setEnabled(true);
  angleRepresentation->setEnabled(true);
  delimiter->setEnabled(true);
  progress.setValue(1);
  progress.setLabelText("[1/3] Reading File ...");
  AngleFileLoader::Pointer loader = AngleFileLoader::New();
  loader->setInputFile(angleFilePath->text().toStdString());
  loader->setAngleRepresentation(angleRepresentation->currentIndex());
  loader->setFileAnglesInDegrees(anglesInDegrees->isChecked());
  loader->setOutputAnglesInDegrees(true);
  QString delim;
  int index = delimiter->currentIndex();
  switch(index)
  {
  case 0:
    delim = " ";
    break;
  case 1:
    delim = "\t";
    break;
  case 2:
    delim = ",";
    break;
  case 3:
    delim = ";";
    break;
  default:
    delim = " ";
  }

  loader->setDelimiter(delim.toStdString());
  EbsdLib::FloatArrayType::Pointer data = loader->loadData();
  if(loader->getErrorCode() < 0)
  {
    QMessageBox::critical(this, "Error Loading Angle data", QString::fromStdString(loader->getErrorMessage()), QMessageBox::Ok);
    Q_EMIT bulkLoadEvent(true);
    return;
  }
  count = data->getNumberOfTuples();
  e1s.resize(count);
  e2s.resize(count);
  e3s.resize(count);
  weights.resize(count);
  sigmas.resize(count);
  for(size_t i = 0; i < count; ++i)
  {
    e1s[i] = data->getComponent(i, 0);
    e2s[i] = data->getComponent(i, 1);
    e3s[i] = data->getComponent(i, 2);
    weights[i] = data->getComponent(i, 3);
    sigmas[i] = data->getComponent(i, 4);
  }

  progress.setValue(2);
  progress.setLabelText("[2/3] Rendering Pole Figure ...");

  // Just blow away our table and create a new one. Quicker than removing all the rows
  // first, the repopulating them
  if(m_OdfBulkTableModel != nullptr)
  {
    delete m_OdfBulkTableModel;
    m_OdfBulkTableModel = nullptr;
  }
  m_OdfBulkTableModel = new SGODFTableModel;
  m_OdfBulkTableModel->setCrystalStructure(m_CrystalStructure);
  m_OdfBulkTableModel->setInitialValues();
  m_ODFTableView->setModel(m_OdfBulkTableModel);
  QAbstractItemDelegate* idelegate = m_OdfBulkTableModel->getItemDelegate();
  m_ODFTableView->setItemDelegate(idelegate);

#if 1
  m_OdfBulkTableModel->blockSignals(true);
  m_OdfBulkTableModel->setColumnData(SGODFTableModel::Euler1, e1s);
  m_OdfBulkTableModel->setColumnData(SGODFTableModel::Euler2, e2s);
  m_OdfBulkTableModel->setColumnData(SGODFTableModel::Euler3, e3s);
  m_OdfBulkTableModel->setColumnData(SGODFTableModel::Weight, weights);
  m_OdfBulkTableModel->setColumnData(SGODFTableModel::Sigma, sigmas);
  m_OdfBulkTableModel->blockSignals(false);
#endif

  Q_EMIT bulkLoadEvent(false);
  on_m_CalculateODFBtn_clicked();
  progress.setValue(3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_deleteODFTextureBtn_clicked()
{
  QItemSelectionModel* selectionModel = m_ODFTableView->selectionModel();
  if(!selectionModel->hasSelection())
  {
    return;
  }
  QModelIndex index = selectionModel->currentIndex();
  if(!index.isValid())
  {
    return;
  }
  m_ODFTableModel->removeRow(index.row(), index.parent());
  if(m_ODFTableModel->rowCount() > 0)
  {
    m_ODFTableView->resizeColumnsToContents();
  }
  updatePlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGODFTableModel* StatsGenODFWidget::tableModel()
{
  return m_ODFTableModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_savePoleFigureImage_clicked()
{
  QString Ftype = "Image Files";
  QString ext = "*.png";
  QString s = "Image Files (*.tiff *.png *.bmp);;All Files(*.*)";
  QString defaultName = m_OpenDialogLastFilePath;
  QString file = QFileDialog::getSaveFileName(this, tr("Save File As"), defaultName, s);

  if(file.isEmpty())
  {
    return;
  }
  // bool ok = false;
  file = QDir::toNativeSeparators(file);
  // Store the last used directory into the private instance variable
  m_OpenDialogLastFilePath = file;

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
  QImage image = m_PoleFigureLabel->pixmap()->toImage();
#else
  QImage image = m_PoleFigureLabel->pixmap(Qt::ReturnByValue).toImage();
#endif
  image.save(file);
}

// -----------------------------------------------------------------------------
void StatsGenODFWidget::setEnableAxisDecorations(bool value)
{
  m_EnableAxisDecorations = value;
}

// -----------------------------------------------------------------------------
bool StatsGenODFWidget::getEnableAxisDecorations() const
{
  return m_EnableAxisDecorations;
}

// -----------------------------------------------------------------------------
void StatsGenODFWidget::setInitializing(bool value)
{
  m_Initializing = value;
}

// -----------------------------------------------------------------------------
bool StatsGenODFWidget::getInitializing() const
{
  return m_Initializing;
}
