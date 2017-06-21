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
#include <QtConcurrent/QtConcurrentMap>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QModelIndex>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtGui/QCloseEvent>
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

#include "EbsdLib/EbsdConstants.h"

#include "SIMPLib/Math/SIMPLibMath.h"

#include "OrientationLib/LaueOps/CubicOps.h"
#include "OrientationLib/LaueOps/HexagonalOps.h"
#include "OrientationLib/LaueOps/OrthoRhombicOps.h"
#include "OrientationLib/LaueOps/LaueOps.h"
#include "OrientationLib/Texture/StatsGen.hpp"
#include "OrientationLib/Texture/Texture.hpp"

#include "StatsGenerator/StatsGeneratorFilters/StatsGeneratorUtilities.h"
#include "StatsGenerator/Widgets/TableModels/SGMDFTableModel.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_StatsGenMDFWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenMDFWidget::StatsGenMDFWidget(QWidget* parent)
: QWidget(parent)
, m_PhaseIndex(-1)
, m_CrystalStructure(Ebsd::CrystalStructure::Cubic_High)
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

  on_m_MDFUpdateBtn_clicked();
  emit dataChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot)
{

  QPalette pal;
  pal.setColor(QPalette::Text, Qt::white);
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

  QFont font;
  font.setBold(true);

  QwtText xAxis(xAxisName);
  xAxis.setColor(Qt::white);
  xAxis.setRenderFlags(Qt::AlignHCenter | Qt::AlignTop);
  xAxis.setFont(font);

  QwtText yAxis(yAxisName);
  yAxis.setColor(Qt::white);
  yAxis.setRenderFlags(Qt::AlignHCenter | Qt::AlignTop);
  yAxis.setFont(font);

  const int margin = 5;
  plot->setContentsMargins(margin, margin, margin, margin);

  plot->setAxisTitle(QwtPlot::xBottom, xAxis);
  plot->setAxisTitle(QwtPlot::yLeft, yAxis);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::on_m_MDFUpdateBtn_clicked()
{
  // Generate the ODF Data from the current values in the ODFTableModel
  QVector<float> e1s;
  QVector<float> e2s;
  QVector<float> e3s;
  QVector<float> weights;
  QVector<float> sigmas;

  // Initialize xMax and yMax....
  e1s = m_ODFTableModel->getData(SGODFTableModel::Euler1);
  e2s = m_ODFTableModel->getData(SGODFTableModel::Euler2);
  e3s = m_ODFTableModel->getData(SGODFTableModel::Euler3);
  weights = m_ODFTableModel->getData(SGODFTableModel::Weight);
  sigmas = m_ODFTableModel->getData(SGODFTableModel::Sigma);

  for(qint32 i = 0; i < e1s.size(); i++)
  {
    e1s[i] = e1s[i] * static_cast<float>(SIMPLib::Constants::k_PiOver180);
    e2s[i] = e2s[i] * static_cast<float>(SIMPLib::Constants::k_PiOver180);
    e3s[i] = e3s[i] * static_cast<float>(SIMPLib::Constants::k_PiOver180);
  }

  QVector<float> odf = StatsGeneratorUtilities::GenerateODFData(m_CrystalStructure, e1s, e2s, e3s, weights, sigmas);

  updateMDFPlot(odf);

  emit dataChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::updateMDFPlot(QVector<float>& odf)
{
  int err = 0;
  int size = 100000;

  // These are the input vectors
  QVector<float> angles;
  QVector<float> axes;
  QVector<float> weights;

  angles = m_MDFTableModel->getData(SGMDFTableModel::Angle);
  weights = m_MDFTableModel->getData(SGMDFTableModel::Weight);
  axes = m_MDFTableModel->getData(SGMDFTableModel::Axis);

  // These are the output vectors
  QVector<float> x;
  QVector<float> y;
  if(Ebsd::CrystalStructure::Cubic_High == m_CrystalStructure)
  {
    // Allocate a new vector to hold the mdf data
    QVector<float> mdf(CubicOps::k_MdfSize);
    // Calculate the MDF Data using the ODF data and the rows from the MDF Table model
    Texture::CalculateMDFData<float, CubicOps>(angles.data(), axes.data(), weights.data(), odf.data(), mdf.data(), static_cast<size_t>(angles.size()));
    // Now generate the actual XY point data that gets plotted.
    int npoints = 13;
    x.resize(npoints);
    y.resize(npoints);
    err = StatsGen::GenCubicMDFPlotData(mdf.data(), x.data(), y.data(), npoints, size);
    if(err < 0)
    {
      return;
    }
  }
  else if(Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructure)
  {
    // Allocate a new vector to hold the mdf data
    QVector<float> mdf(HexagonalOps::k_MdfSize);
    // Calculate the MDF Data using the ODF data and the rows from the MDF Table model
    Texture::CalculateMDFData<float, HexagonalOps>(angles.data(), axes.data(), weights.data(), odf.data(), mdf.data(), static_cast<size_t>(angles.size()));
    // Now generate the actual XY point data that gets plotted.
    int npoints = 20;
    x.resize(npoints);
    y.resize(npoints);
    err = StatsGen::GenHexMDFPlotData(mdf.data(), x.data(), y.data(), npoints, size);
    if(err < 0)
    {
      return;
    }
  }
  else
  {
    err = 1;
    QString ss("Only Cubic_High or Hexagonal_High are allowed for the Laue group.");
    QMessageBox::StandardButton reply;
    reply = QMessageBox::critical(nullptr, QString("MDF Generation Error"), ss, QMessageBox::Ok);
    Q_UNUSED(reply);
  }

  QwtArray<double> xD(static_cast<int>(x.size()));
  QwtArray<double> yD(static_cast<int>(x.size()));
  for(qint32 i = 0; i < x.size(); ++i)
  {
    xD[i] = static_cast<double>(x.at(i));
    yD[i] = static_cast<double>(y.at(i));
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
  m_MDFTableView->resizeColumnsToContents();
  m_MDFTableView->scrollToBottom();
  m_MDFTableView->setFocus();
  QModelIndex index = m_MDFTableModel->index(m_MDFTableModel->rowCount() - 1, 0);
  m_MDFTableView->setCurrentIndex(index);
  emit dataChanged();
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
  emit dataChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::on_loadMDFBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastFilePath;
  QString file = QFileDialog::getOpenFileName(this, tr("Open MDF File"), proposedFile, tr("Text Document (*.txt)"));
  if(true == file.isEmpty())
  {
    return;
  }
  else
  {
    QFileInfo fi(file);
    m_OpenDialogLastFilePath = fi.filePath();

    size_t numMisorients = 0;
    QString filename = file;
    std::ifstream inFile;
    inFile.open(filename.toLatin1().data());

    inFile >> numMisorients;

    float angle, weight;
    std::string n1, n2, n3;
    for(size_t i = 0; i < numMisorients; i++)
    {
      inFile >> angle >> n1 >> n2 >> n3 >> weight;

      QString axis = QString("<" + QString::fromStdString(n1) + "," + QString::fromStdString(n2) + "," + QString::fromStdString(n3) + ">");

      if(!m_MDFTableModel->insertRow(m_MDFTableModel->rowCount()))
      {
        return;
      }
      int row = m_MDFTableModel->rowCount() - 1;
      m_MDFTableModel->setRowData(row, angle, axis, weight);

      m_MDFTableView->resizeColumnsToContents();
      m_MDFTableView->scrollToBottom();
      m_MDFTableView->setFocus();
      QModelIndex index = m_MDFTableModel->index(m_MDFTableModel->rowCount() - 1, 0);
      m_MDFTableView->setCurrentIndex(index);
    }
  }
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
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
    arrays = pp->getMDF_Weights();
  }
  if(phaseType == PhaseType::Type::Precipitate)
  {
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
    arrays = pp->getMDF_Weights();
  }
  if(phaseType == PhaseType::Type::Transformation)
  {
    TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
    arrays = tp->getMDF_Weights();
  }

  QVector<float> axis;
  QVector<float> angle;
  QVector<float> weights;

  for(int i = 0; i < arrays.size(); i++)
  {
    if(arrays[i]->getName().compare(SIMPL::StringConstants::Axis) == 0)
    {
      axis = QVector<float>(static_cast<int>(arrays[i]->getSize())); // This one is 3xn in size
      ::memcpy(&(axis.front()), arrays[i]->getVoidPointer(0), sizeof(float) * axis.size());
    }

    if(arrays[i]->getName().compare(SIMPL::StringConstants::Angle) == 0)
    {
      angle = QVector<float>(static_cast<int>(arrays[i]->getNumberOfTuples()));
      ::memcpy(&(angle.front()), arrays[i]->getVoidPointer(0), sizeof(float) * angle.size());
    }

    if(arrays[i]->getName().compare(SIMPL::StringConstants::Weight) == 0)
    {
      weights = QVector<float>(static_cast<int>(arrays[i]->getNumberOfTuples()));
      ::memcpy(&(weights.front()), arrays[i]->getVoidPointer(0), sizeof(float) * weights.size());
    }
  }
  if(arrays.size() > 0)
  {
    // Load the data into the table model
    m_MDFTableModel->setTableData(angle, axis, weights);
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
  QVector<float> e1s;
  QVector<float> e2s;
  QVector<float> e3s;
  QVector<float> odf_weights;
  QVector<float> sigmas;

  e1s = m_ODFTableModel->getData(SGODFTableModel::Euler1);
  e2s = m_ODFTableModel->getData(SGODFTableModel::Euler2);
  e3s = m_ODFTableModel->getData(SGODFTableModel::Euler3);
  odf_weights = m_ODFTableModel->getData(SGODFTableModel::Weight);
  sigmas = m_ODFTableModel->getData(SGODFTableModel::Sigma);

  for(qint32 i = 0; i < e1s.size(); i++)
  {
    e1s[i] = e1s[i] * static_cast<float>(SIMPLib::Constants::k_PiOver180);
    e2s[i] = e2s[i] * static_cast<float>(SIMPLib::Constants::k_PiOver180);
    e3s[i] = e3s[i] * static_cast<float>(SIMPLib::Constants::k_PiOver180);
  }

  QVector<float> odf = StatsGeneratorUtilities::GenerateODFData(m_CrystalStructure, e1s, e2s, e3s, odf_weights, sigmas, !preflight);

  // Now use the ODF data to generate the MDF data ************************************************
  QVector<float> angles = m_MDFTableModel->getData(SGMDFTableModel::Angle);
  QVector<float> weights = m_MDFTableModel->getData(SGMDFTableModel::Weight);
  QVector<float> axes = m_MDFTableModel->getData(SGMDFTableModel::Axis);

  StatsGeneratorUtilities::GenerateMisorientationBinData(statsData, phaseType, m_CrystalStructure, odf, angles, axes, weights, !preflight);
  return retErr;
}
