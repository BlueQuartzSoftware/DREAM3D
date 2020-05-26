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

#include "StatsGenMDFWidget.h"

#include "SIMPLib/Math/SIMPLibMath.h"

//-- C++ Includes
#include <iostream>

//-- Qt Includes
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QModelIndex>
#include <QtCore/QString>
#include <QtCore/QVector>

#include <QtWidgets/QAbstractItemDelegate>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <qwt.h>
#include <qwt_abstract_scale_draw.h>
#include <qwt_compat.h>
#include <qwt_interval.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_marker.h>
#include <qwt_point_3d.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_series_data.h>
#include <qwt_symbol.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_picker.h>

#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Utilities/UTFUtilities.hpp"

#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "EbsdLib/LaueOps/CubicLowOps.h"
#include "EbsdLib/LaueOps/CubicOps.h"
#include "EbsdLib/LaueOps/HexagonalLowOps.h"
#include "EbsdLib/LaueOps/HexagonalOps.h"
#include "EbsdLib/LaueOps/MonoclinicOps.h"
#include "EbsdLib/LaueOps/OrthoRhombicOps.h"
#include "EbsdLib/LaueOps/TetragonalLowOps.h"
#include "EbsdLib/LaueOps/TetragonalOps.h"
#include "EbsdLib/LaueOps/TriclinicOps.h"
#include "EbsdLib/LaueOps/TrigonalLowOps.h"
#include "EbsdLib/LaueOps/TrigonalOps.h"
#include "EbsdLib/LaueOps/LaueOps.h"
#include "EbsdLib/Texture/StatsGen.hpp"
#include "EbsdLib/Texture/Texture.hpp"

#include "SyntheticBuilding/Gui/Widgets/TableModels/SGMDFTableModel.h"
#include "SyntheticBuilding/Gui/Widgets/StatsProgressWidget.h"
#include "SyntheticBuilding/SyntheticBuildingFilters/StatsGeneratorUtilities.h"
#include "SyntheticBuilding/SyntheticBuildingConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenMDFWidget::StatsGenMDFWidget(QWidget* parent)
: QWidget(parent)
{
  this->setupUi(this);
  this->setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenMDFWidget::~StatsGenMDFWidget()
{
  if(nullptr != m_MDFTableModel)
  {
    m_MDFTableModel->deleteLater();
  }
  if(nullptr != m_PlotPicker)
  {
    delete m_PlotPicker;
  }
  if(nullptr != m_PlotPickerMachine)
  {
    delete m_PlotPickerMachine;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGMDFTableModel* StatsGenMDFWidget::tableModel()
{
  return m_MDFTableModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::setupGui()
{
  initQwtPlot("Misorientation Angle(w)", "Freq", m_MDFPlot);
  m_MDFTableModel = new SGMDFTableModel;
  m_MDFTableModel->setInitialValues();
  m_MDFTableView->setModel(m_MDFTableModel);

  connect(m_MDFTableModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(tableDataChanged(const QModelIndex&, const QModelIndex&)));

  QAbstractItemDelegate* aid = m_MDFTableModel->getItemDelegate();
  m_MDFTableView->setItemDelegate(aid);
  m_PlotCurve = new QwtPlotCurve;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::tableDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
  Q_UNUSED(topLeft);
  Q_UNUSED(bottomRight);

  updatePlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot)
{

  QPalette pal;
  pal.setColor(QPalette::Text, SVStyle::Instance()->getQLabel_color());
  pal.setColor(QPalette::Foreground, Qt::white);
  pal.setColor(QPalette::Window, Qt::black);

  plot->setPalette(pal);

  plot->plotLayout()->setAlignCanvasToScales(true);
  for(int axis = 0; axis < QwtPlot::axisCnt; axis++)
  {
    plot->axisWidget(axis)->setMargin(0);
    plot->axisWidget(axis)->setPalette(pal);
  }
  QwtPlotCanvas* canvas = new QwtPlotCanvas();

  canvas->setAutoFillBackground(false);
  canvas->setFrameStyle(QFrame::NoFrame);
  canvas->setPalette(pal);
  plot->setCanvas(canvas);

  QFont font = SVStyle::Instance()->GetUIFont();
  font.setWeight(QFont::Bold);
  font.setPointSize(SG_FONT_SIZE);

  QwtText xAxis(xAxisName);
  xAxis.setRenderFlags(Qt::AlignHCenter | Qt::AlignTop);
  xAxis.setFont(font);
  xAxis.setColor(SVStyle::Instance()->getQLabel_color());

  plot->setAxisTitle(QwtPlot::xBottom, xAxisName);
  plot->setAxisScale(QwtPlot::xBottom, 0.0, 180.0, 10);

  QwtText yAxis(yAxisName);
  yAxis.setRenderFlags(Qt::AlignHCenter | Qt::AlignTop);
  yAxis.setFont(font);
  yAxis.setColor(SVStyle::Instance()->getQLabel_color());
  plot->setAxisTitle(QwtPlot::yLeft, yAxisName);

  const int margin = 0;
  plot->setContentsMargins(margin, margin, margin, margin);

  plot->setAxisTitle(QwtPlot::xBottom, xAxis);
  plot->setAxisTitle(QwtPlot::yLeft, yAxis);

  QwtPlotPicker* m_PlotPicker = new QwtPlotPicker(plot->xBottom, plot->yLeft, QwtPicker::CrossRubberBand, QwtPicker::AlwaysOn, plot->canvas());
  QwtPickerMachine* m_PlotPickerMachine = new QwtPickerClickPointMachine();
  m_PlotPicker->setTrackerPen(QPen(SVStyle::Instance()->getQLabel_color()));
  m_PlotPicker->setStateMachine(m_PlotPickerMachine);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::updatePlots()
{
  // Generate the ODF Data from the current values in the ODFTableModel
  std::vector<float> e1s;
  std::vector<float> e2s;
  std::vector<float> e3s;
  std::vector<float> weights;
  std::vector<float> sigmas;
  if(nullptr == m_ODFTableModel)
  {
    return;
  }
  // Initialize xMax and yMax....
  e1s = m_ODFTableModel->getData(SGODFTableModel::Euler1);
  e2s = m_ODFTableModel->getData(SGODFTableModel::Euler2);
  e3s = m_ODFTableModel->getData(SGODFTableModel::Euler3);
  weights = m_ODFTableModel->getData(SGODFTableModel::Weight);
  sigmas = m_ODFTableModel->getData(SGODFTableModel::Sigma);

  // Convert Degrees to Radians for ODF
  for(size_t i = 0; i < e1s.size(); i++)
  {
    e1s[i] = e1s[i] * static_cast<float>(SIMPLib::Constants::k_PiOver180);
    e2s[i] = e2s[i] * static_cast<float>(SIMPLib::Constants::k_PiOver180);
    e3s[i] = e3s[i] * static_cast<float>(SIMPLib::Constants::k_PiOver180);
  }

  StatsProgressWidget progress("Calculating/Updating the ODF", "Cancel", nullptr);
  progress.setLabelText("Please Wait...");
  progress.setProgTitle("Generating ODF Plot Data...");
  progress.setVisible(true);
  progress.show();
  qApp->processEvents();

  progress.setValue(0);
  progress.setLabelText("Calculating ODF");

  std::vector<float> odf = StatsGeneratorUtilities::GenerateODFData(m_CrystalStructure, e1s, e2s, e3s, weights, sigmas);

  updateMDFPlot(odf);

  emit dataChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::on_m_MDFUpdateBtn_clicked()
{
  updatePlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::updateMDFPlot(std::vector<float>& odf)
{
  int err = 0;
  int size = 100000;

  StatsProgressWidget progress("Calculating/Updating the MDF", "Cancel", nullptr);
  progress.setLabelText("Please Wait...");
  progress.setProgTitle("Generating MDF Plot Data...");
  progress.setVisible(true);
  progress.show();
  qApp->processEvents();

  // These are the input vectors
  using ContainerType = std::vector<float>;

  ContainerType angles = m_MDFTableModel->getData(SGMDFTableModel::Angle);
  ContainerType axes = m_MDFTableModel->getData(SGMDFTableModel::Axis);
  ContainerType weights = m_MDFTableModel->getData(SGMDFTableModel::Weight);

  // Ensure that the angle is in range of [0,pi] and Convert Angles to Radians
  for(auto& angle : angles)
  {
    while(angle > 180.0)
    {
      angle -= 180.0;
    }
    while(angle < 0.0)
    {
      angle += 180.0;
    }
    angle *= SIMPLib::Constants::k_DegToRad;
  }

  // Normalize the axis to unit norm
  for(size_t i = 0; i < axes.size(); i = i + 3)
  {
    float length = std::sqrt(axes[i] * axes[i] + axes[i + 1] * axes[i + 1] + axes[i + 2] * axes[i + 2]);
    axes[i] /= length;
    axes[i + 1] /= length;
    axes[i + 2] /= length;
  }

  // Sanity check the Axis_Angle inputs;
  for(size_t i = 0; i < angles.size(); i++)
  {
    OrientationF ax(axes[i * 3], axes[i * 3 + 1], axes[i * 3 + 2], angles[i]);
    OrientationTransformation::ResultType result = OrientationTransformation::ax_check<OrientationF>(ax);
    if(result.result < 0)
    {
      QMessageBox msgBox;
      msgBox.setText("The Axis Angle was not normalized and/or the angle was not within the range of [0, Pi]");
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      std::ignore = msgBox.exec();

      return;
    }
  }

  // These are the output vectors
  int npoints = 36;
  ContainerType x(npoints);
  ContainerType y(npoints);
  ContainerType mdf;

  progress.setValue(1);
  progress.setLabelText("Calculating and Generating MDF.....");

  //// ODF/MDF Update Codes
  switch(m_CrystalStructure)
  {
  case EbsdLib::CrystalStructure::Triclinic: // 4; Triclinic -1
    Texture::CalculateMDFData<float, TriclinicOps, ContainerType>(angles, axes, weights, odf, mdf, static_cast<size_t>(angles.size()));
    err = StatsGen::GenMDFPlotData<float, TriclinicOps, ContainerType>(mdf, x, y, size);
    break;
  case EbsdLib::CrystalStructure::Monoclinic: // 5; Monoclinic 2/m
    Texture::CalculateMDFData<float, MonoclinicOps, ContainerType>(angles, axes, weights, odf, mdf, static_cast<size_t>(angles.size()));
    err = StatsGen::GenMDFPlotData<float, MonoclinicOps, ContainerType>(mdf, x, y, size);
    break;
  case EbsdLib::CrystalStructure::OrthoRhombic: // 6; Orthorhombic mmm
    Texture::CalculateMDFData<float, OrthoRhombicOps, ContainerType>(angles, axes, weights, odf, mdf, static_cast<size_t>(angles.size()));
    err = StatsGen::GenMDFPlotData<float, OrthoRhombicOps, ContainerType>(mdf, x, y, size);
    break;
  case EbsdLib::CrystalStructure::Tetragonal_Low: // 7; Tetragonal-Low 4/m
    Texture::CalculateMDFData<float, TetragonalLowOps, ContainerType>(angles, axes, weights, odf, mdf, static_cast<size_t>(angles.size()));
    err = StatsGen::GenMDFPlotData<float, TetragonalLowOps, ContainerType>(mdf, x, y, size);
    break;
  case EbsdLib::CrystalStructure::Tetragonal_High: // 8; Tetragonal-High 4/mmm
    Texture::CalculateMDFData<float, TetragonalOps, ContainerType>(angles, axes, weights, odf, mdf, static_cast<size_t>(angles.size()));
    err = StatsGen::GenMDFPlotData<float, TetragonalOps, ContainerType>(mdf, x, y, size);
    break;
  case EbsdLib::CrystalStructure::Trigonal_Low: // 9; Trigonal-Low -3
    Texture::CalculateMDFData<float, TrigonalLowOps, ContainerType>(angles, axes, weights, odf, mdf, static_cast<size_t>(angles.size()));
    err = StatsGen::GenMDFPlotData<float, TrigonalLowOps, ContainerType>(mdf, x, y, size);
    break;
  case EbsdLib::CrystalStructure::Trigonal_High: // 10; Trigonal-High -3m
    Texture::CalculateMDFData<float, TrigonalOps, ContainerType>(angles, axes, weights, odf, mdf, static_cast<size_t>(angles.size()));
    err = StatsGen::GenMDFPlotData<float, TrigonalOps, ContainerType>(mdf, x, y, size);
    break;
  case EbsdLib::CrystalStructure::Hexagonal_Low: // 2; Hexagonal-Low 6/m
    Texture::CalculateMDFData<float, HexagonalLowOps, ContainerType>(angles, axes, weights, odf, mdf, static_cast<size_t>(angles.size()));
    err = StatsGen::GenMDFPlotData<float, HexagonalLowOps, ContainerType>(mdf, x, y, size);
    break;
  case EbsdLib::CrystalStructure::Hexagonal_High: // 0; Hexagonal-High 6/mmm
    Texture::CalculateMDFData<float, HexagonalOps, ContainerType>(angles, axes, weights, odf, mdf, static_cast<size_t>(angles.size()));
    err = StatsGen::GenMDFPlotData<float, HexagonalOps, ContainerType>(mdf, x, y, size);
    break;
  case EbsdLib::CrystalStructure::Cubic_Low: // 3; Cubic Cubic-Low m3 (Tetrahedral)
    Texture::CalculateMDFData<float, CubicLowOps, ContainerType>(angles, axes, weights, odf, mdf, static_cast<size_t>(angles.size()));
    err = StatsGen::GenMDFPlotData<float, CubicLowOps, ContainerType>(mdf, x, y, size);
    break;
  case EbsdLib::CrystalStructure::Cubic_High: // 1; Cubic Cubic-High m3m
    Texture::CalculateMDFData<float, CubicOps, ContainerType>(angles, axes, weights, odf, mdf, static_cast<size_t>(angles.size()));
    err = StatsGen::GenMDFPlotData<float, CubicOps, ContainerType>(mdf, x, y, size);
    break;
  default:
    err = -1;
    break;
  }

  if(err < 0)
  {
    return;
  }

  QwtArray<double> xD(static_cast<int>(x.size()));
  QwtArray<double> yD(static_cast<int>(x.size()));
  for(ContainerType::size_type i = 0; i < x.size(); ++i)
  {
    xD[i] = static_cast<double>(x[i]);
    yD[i] = static_cast<double>(y[i]);
  }

  // This will actually plot the XY data in the Qwt plot widget
  QwtPlotCurve* curve = m_PlotCurve;
#if QWT_VERSION >= 0x060000
  curve->setSamples(xD, yD);
#else
  curve->setData(xD, yD);
#endif
  QColor color = QColor("DodgerBlue");
  curve->setPen(color, 2);
  curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
  curve->setStyle(QwtPlotCurve::Lines);
  QwtSymbol* symbol = new QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::white), QPen(color, 2), QSize(8, 8));
  curve->setSymbol(symbol);
  curve->attach(m_MDFPlot);
  m_MDFPlot->replot();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::on_addMDFRowBtn_clicked()
{
  if(!m_MDFTableModel->insertRow(m_MDFTableModel->rowCount()))
  {
    return;
  }
  QModelIndex index = m_MDFTableModel->index(m_MDFTableModel->rowCount() - 1, 0);
  m_MDFTableView->setCurrentIndex(index);
  m_MDFTableView->resizeColumnsToContents();
  m_MDFTableView->scrollToBottom();
  m_MDFTableView->setFocus();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::on_deleteMDFRowBtn_clicked()
{
  QItemSelectionModel* selectionModel = m_MDFTableView->selectionModel();
  if(!selectionModel->hasSelection())
  {
    return;
  }
  QModelIndex index = selectionModel->currentIndex();
  if(!index.isValid())
  {
    return;
  }
  m_MDFTableModel->removeRow(index.row(), index.parent());
  if(m_MDFTableModel->rowCount() > 0)
  {
    m_MDFTableView->resizeColumnsToContents();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::on_loadMDFBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastFilePath;
  QString file = QFileDialog::getOpenFileName(this, tr("Open MDF File"), proposedFile, tr("Text Document (*.txt)"));
  if(file.isEmpty())
  {
    return;
  }

  QFileInfo fi(file);
  m_OpenDialogLastFilePath = fi.filePath();

  std::pair<bool, int32_t> isUtf8 = UTFUtilities::IsUtf8(fi.filePath().toStdString());

  QString filename = file;
  std::ifstream inFile;
  inFile.open(filename.toLatin1().data(), std::ios::in);
  if(!inFile.is_open())
  {
    return;
  }

  int32_t numMisorients = 0;

  // If the file is UTF8 with a BOM marker then read the first 3 bytes and dump them.
  if(isUtf8.first)
  {
    char a = '\0';
    char b = '\0';
    char c = '\0';
    inFile >> a >> b >> c;
  }

  inFile >> numMisorients;

  QVector<float> angles(numMisorients);
  QVector<float> axis(numMisorients * 3);
  QVector<float> weights(numMisorients);

  float angle, weight, n1, n2, n3;
  for(int32_t i = 0; i < numMisorients - 1; i++)
  {
    inFile >> angle >> n1 >> n2 >> n3 >> weight;
    angles[i] = angle;
    axis[i * 3] = n1;
    axis[i * 3 + 1] = n2;
    axis[i * 3 + 2] = n3;
    weights[i] = weight;
  }

  m_MDFTableModel->setTableData(angles, axis, weights);

  m_MDFTableView->resizeColumnsToContents();
  m_MDFTableView->scrollToBottom();
  m_MDFTableView->setFocus();
  QModelIndex index = m_MDFTableModel->index(m_MDFTableModel->rowCount() - 1, 0);
  m_MDFTableView->setCurrentIndex(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::extractStatsData(int index, StatsData* statsData, PhaseType::Type phaseType)
{
  Q_UNUSED(index)
  VectorOfFloatArray arrays;
  if(phaseType == PhaseType::Type::Primary)
  {
    PrimaryStatsData* pp = dynamic_cast<PrimaryStatsData*>(statsData);
    arrays = pp->getMDF_Weights();
  }
  if(phaseType == PhaseType::Type::Precipitate)
  {
    PrecipitateStatsData* pp = dynamic_cast<PrecipitateStatsData*>(statsData);
    arrays = pp->getMDF_Weights();
  }
  if(phaseType == PhaseType::Type::Transformation)
  {
    TransformationStatsData* tp = dynamic_cast<TransformationStatsData*>(statsData);
    arrays = tp->getMDF_Weights();
  }

  QVector<float> axis;
  QVector<float> angles;
  QVector<float> weights;

  for(int i = 0; i < arrays.size(); i++)
  {
    if(arrays[i]->getName().compare(SIMPL::StringConstants::Axis) == 0)
    {
      axis = QVector<float>(static_cast<int>(arrays[i]->getSize())); // This one is 3xn in size
      std::copy_n(arrays[i]->cbegin(), axis.size(), axis.begin());
    }

    if(arrays[i]->getName().compare(SIMPL::StringConstants::Angle) == 0)
    {
      angles = QVector<float>(static_cast<int>(arrays[i]->getNumberOfTuples()));
      std::copy_n(arrays[i]->cbegin(), angles.size(), angles.begin());
    }

    if(arrays[i]->getName().compare(SIMPL::StringConstants::Weight) == 0)
    {
      weights = QVector<float>(static_cast<int>(arrays[i]->getNumberOfTuples()));
      std::copy_n(arrays[i]->cbegin(), weights.size(), weights.begin());
    }
  }
  // Convert from Radians to Degrees
  for(float& a : angles)
  {
    a *= static_cast<float>(SIMPLib::Constants::k_RadToDeg);
  }
  if(!arrays.empty())
  {
    // Load the data into the table model
    m_MDFTableModel->setTableData(angles, axis, weights);
  }

  on_m_MDFUpdateBtn_clicked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenMDFWidget::getMisorientationData(StatsData* statsData, PhaseType::Type phaseType, bool preflight)
{
  int retErr = 0;

  // Generate the ODF Data from the current values in the ODFTableModel
  std::vector<float> e1s;
  std::vector<float> e2s;
  std::vector<float> e3s;
  std::vector<float> odf_weights;
  std::vector<float> sigmas;

  e1s = m_ODFTableModel->getData(SGODFTableModel::Euler1);
  e2s = m_ODFTableModel->getData(SGODFTableModel::Euler2);
  e3s = m_ODFTableModel->getData(SGODFTableModel::Euler3);
  odf_weights = m_ODFTableModel->getData(SGODFTableModel::Weight);
  sigmas = m_ODFTableModel->getData(SGODFTableModel::Sigma);

  // Convert from Degrees to Radians
  for(size_t i = 0; i < e1s.size(); i++)
  {
    e1s[i] *= static_cast<float>(SIMPLib::Constants::k_PiOver180);
    e2s[i] *= static_cast<float>(SIMPLib::Constants::k_PiOver180);
    e3s[i] *= static_cast<float>(SIMPLib::Constants::k_PiOver180);
  }

  std::vector<float> odf = StatsGeneratorUtilities::GenerateODFData(m_CrystalStructure, e1s, e2s, e3s, odf_weights, sigmas, !preflight);

  // Now use the ODF data to generate the MDF data ************************************************
  std::vector<float> angles = m_MDFTableModel->getData(SGMDFTableModel::Angle);
  // Convert from Degrees to Radians
  for(float& a : angles)
  {
    a *= static_cast<float>(SIMPLib::Constants::k_DegToRad);
  }

  std::vector<float> weights = m_MDFTableModel->getData(SGMDFTableModel::Weight);
  std::vector<float> axes = m_MDFTableModel->getData(SGMDFTableModel::Axis);

  StatsGeneratorUtilities::GenerateMisorientationBinData(statsData, phaseType, m_CrystalStructure, odf, angles, axes, weights, !preflight);
  return retErr;
}

// -----------------------------------------------------------------------------
void StatsGenMDFWidget::setPhaseIndex(int value)
{
  m_PhaseIndex = value;
}

// -----------------------------------------------------------------------------
int StatsGenMDFWidget::getPhaseIndex() const
{
  return m_PhaseIndex;
}

// -----------------------------------------------------------------------------
void StatsGenMDFWidget::setCrystalStructure(unsigned int value)
{
  m_CrystalStructure = value;
}

// -----------------------------------------------------------------------------
unsigned int StatsGenMDFWidget::getCrystalStructure() const
{
  return m_CrystalStructure;
}

// -----------------------------------------------------------------------------
void StatsGenMDFWidget::setODFTableModel(SGODFTableModel* value)
{
  m_ODFTableModel = value;
}

// -----------------------------------------------------------------------------
SGODFTableModel* StatsGenMDFWidget::getODFTableModel() const
{
  return m_ODFTableModel;
}
