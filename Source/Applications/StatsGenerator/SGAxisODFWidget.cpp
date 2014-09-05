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

#include "SGAxisODFWidget.h"

#if QWT_VERSION >= 0x060000
#include "backwards.h"
#endif

//-- C++ Includes
#include <iostream>

//-- Qt Includes
#include <QtGui/QAbstractItemDelegate>
#include <QtCore/QtConcurrentMap>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>


#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/Common/Texture.hpp"

#include "StatsGenerator/TableModels/SGODFTableModel.h"
#include "StatsGenerator/StatsGenMDFWidget.h"
#include "StatsGenerator/TextureDialog.h"
#include "DREAM3DLib/Common/StatsGen.hpp"

#include "DREAM3DLib/Utilities/PoleFigureUtilities.h"
#include "QtSupport/PoleFigureImageUtilities.h"

#define SHOW_POLE_FIGURES 1
#define COLOR_POLE_FIGURES 1

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGAxisODFWidget::SGAxisODFWidget(QWidget* parent) :
  QWidget(parent),
  m_EnableAxisDecorations(false),
  m_Initializing(true),
  m_PhaseIndex(-1),
  m_CrystalStructure(Ebsd::CrystalStructure::OrthoRhombic),
  m_ODFTableModel(NULL),
  m_MDFWidget(NULL)
{
  this->setupUi(this);
  this->setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGAxisODFWidget::~SGAxisODFWidget()
{
  if (NULL != m_ODFTableModel)
  {
    m_ODFTableModel->deleteLater();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGAxisODFWidget::extractStatsData(int index, StatsData* statsData, unsigned int phaseType)
{

  VectorOfFloatArray arrays(0);
  if(phaseType == DREAM3D::PhaseType::PrimaryPhase)
  {
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
    arrays = pp->getAxisODF_Weights();
  }
  else if(phaseType == DREAM3D::PhaseType::PrecipitatePhase)
  {
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
    arrays = pp->getAxisODF_Weights();
  }
  else if(phaseType == DREAM3D::PhaseType::TransformationPhase)
  {
    TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
    arrays = tp->getAxisODF_Weights();
  }
  if (arrays.size() > 0 )
  {
    QVector<float> e1(static_cast<int>(arrays[0]->getNumberOfTuples()));
    ::memcpy( &(e1.front()), arrays[0]->getVoidPointer(0), sizeof(float)*e1.size() );

    QVector<float> e2(static_cast<int>(arrays[0]->getNumberOfTuples()));
    ::memcpy( &(e2.front()), arrays[0]->getVoidPointer(0), sizeof(float)*e2.size() );

    QVector<float> e3(static_cast<int>(arrays[0]->getNumberOfTuples()));
    ::memcpy( &(e3.front()), arrays[0]->getVoidPointer(0), sizeof(float)*e3.size() );

    QVector<float> weights(static_cast<int>(arrays[0]->getNumberOfTuples()));
    ::memcpy( &(weights.front()), arrays[0]->getVoidPointer(0), sizeof(float)*weights.size() );

    QVector<float> sigmas(static_cast<int>(arrays[0]->getNumberOfTuples()));
    ::memcpy( &(sigmas.front()), arrays[0]->getVoidPointer(0), sizeof(float)*sigmas.size() );

    if(e1.size() > 0)
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
int SGAxisODFWidget::getOrientationData(StatsData* statsData, unsigned int phaseType)
{
  int retErr = 0;

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
    e1s[i] = e1s[i] * M_PI / 180.0;
    e2s[i] = e2s[i] * M_PI / 180.0;
    e3s[i] = e3s[i] * M_PI / 180.0;
  }
  size_t numEntries = e1s.size();

  QVector<float> aodf;
  aodf.resize(OrthoRhombicOps::k_OdfSize);
  Texture::CalculateOrthoRhombicODFData(e1s.data(), e2s.data(), e3s.data(),
                                        weights.data(), sigmas.data(), true,
                                        aodf.data(), numEntries);
  if (aodf.size() > 0)
  {
    FloatArrayType::Pointer aodfData = FloatArrayType::FromPointer(aodf.data(), aodf.size(), DREAM3D::StringConstants::AxisOrientation);
    if(phaseType == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
      pp->setAxisOrientation(aodfData);
    }
    if(phaseType == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
      pp->setAxisOrientation(aodfData);
    }
    if(phaseType == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
      tp->setAxisOrientation(aodfData);
    }

    if(e1s.size() > 0)
    {
      FloatArrayType::Pointer euler1 = FloatArrayType::FromPointer(e1s.data(), e1s.size(), DREAM3D::StringConstants::Euler1);
      FloatArrayType::Pointer euler2 = FloatArrayType::FromPointer(e2s.data(), e2s.size(), DREAM3D::StringConstants::Euler2);
      FloatArrayType::Pointer euler3 = FloatArrayType::FromPointer(e3s.data(), e3s.size(), DREAM3D::StringConstants::Euler3);
      FloatArrayType::Pointer sigma = FloatArrayType::FromPointer(sigmas.data(), sigmas.size(), DREAM3D::StringConstants::Sigma);
      FloatArrayType::Pointer weight = FloatArrayType::FromPointer(weights.data(), weights.size(), DREAM3D::StringConstants::Weight);

      VectorOfFloatArray aodfWeights;
      aodfWeights.push_back(euler1);
      aodfWeights.push_back(euler2);
      aodfWeights.push_back(euler3);
      aodfWeights.push_back(sigma);
      aodfWeights.push_back(weight);
      if(phaseType == DREAM3D::PhaseType::PrimaryPhase)
      {
        PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
        pp->setAxisODF_Weights(aodfWeights);
      }
      if(phaseType == DREAM3D::PhaseType::PrecipitatePhase)
      {
        PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
        pp->setAxisODF_Weights(aodfWeights);
      }
      if(phaseType == DREAM3D::PhaseType::TransformationPhase)
      {
        TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
        tp->setAxisODF_Weights(aodfWeights);
      }
    }

  }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int SGAxisODFWidget::getCrystalStructure()
{
  return m_CrystalStructure;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGAxisODFWidget::setPhaseIndex(int value)
{
  this->m_PhaseIndex = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGAxisODFWidget::getPhaseIndex()
{
  return m_PhaseIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGAxisODFWidget::setPlotTabTitles(QString t1, QString t2, QString t3)
{
  tabWidget->setTabText(1, t1);
  tabWidget->setTabText(2, t2);
  tabWidget->setTabText(3, t3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGAxisODFWidget::setupGui()
{
  setPlotTabTitles("A-Axis PF", "B-Axis PF", "C-Axis PF");

  // Setup the TableView and Table Models
  QHeaderView* headerView = new QHeaderView(Qt::Horizontal, m_ODFTableView);
  headerView->setResizeMode(QHeaderView::Interactive);
  m_ODFTableView->setHorizontalHeader(headerView);
  headerView->show();

  m_ODFTableModel = new SGODFTableModel;
  m_ODFTableModel->setInitialValues();
  m_ODFTableView->setModel(m_ODFTableModel);
  QAbstractItemDelegate* idelegate = m_ODFTableModel->getItemDelegate();
  m_ODFTableView->setItemDelegate(idelegate);

  addODFTextureBtn->setToolTip("Add Axis ODF Weight");
  deleteODFTextureBtn->setToolTip("Delete Axis ODF Weight");

  m_PlotCurves.push_back(new QwtPlotCurve);
  m_PlotCurves.push_back(new QwtPlotCurve);
  m_PlotCurves.push_back(new QwtPlotCurve);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGAxisODFWidget::initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot)
{
  plot->setAxisTitle(QwtPlot::xBottom, xAxisName);
  plot->setAxisTitle(QwtPlot::yLeft, yAxisName);
  plot->setCanvasBackground(QColor(Qt::white));
  //plot->setCanvasLineWidth(2);
  //plot->canvas()->setFrameShape(QFrame::NoFrame);

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
void SGAxisODFWidget::drawODFPlotGrid(QwtPlot* plot)
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
    circleX[i] = 1.0f - (i * inc);
    circleX[450 + i] = -1.0f + (i * inc);

    circleY[i] = sqrt(1.0f - (circleX[i] * circleX[i]));
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
    rotCrossX[0] = 0.7071067811f;
    rotCrossY[0] = sqrt(1.0 - (rotCrossX[0] * rotCrossX[0]));
    rotCrossX[1] = -0.7071067811f;
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
    rotCrossX[0] = 0.7071067811f;
    rotCrossY[0] = -sqrt(1.0 - (rotCrossX[0] * rotCrossX[0]));
    rotCrossX[1] = -0.7071067811f;
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
void SGAxisODFWidget::updatePlots()
{
  on_m_CalculateODFBtn_clicked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGAxisODFWidget::on_m_CalculateODFBtn_clicked()
{
  int err = 0;
  // qDebug() << "SGAxisODFWidget[" << objectName() << "]::on_m_CalculateODFBtn_clicked" << "\n";
  QwtArray<float> e1s;
  QwtArray<float> e2s;
  QwtArray<float> e3s;
  QwtArray<float> weights;
  QwtArray<float> sigmas;
  QwtArray<float> odf;


  e1s = m_ODFTableModel->getData(SGODFTableModel::Euler1);
  e2s = m_ODFTableModel->getData(SGODFTableModel::Euler2);
  e3s = m_ODFTableModel->getData(SGODFTableModel::Euler3);
  weights = m_ODFTableModel->getData(SGODFTableModel::Weight);
  sigmas = m_ODFTableModel->getData(SGODFTableModel::Sigma);

  for(int i = 0; i < e1s.size(); i++)
  {
    e1s[i] = e1s[i] * M_PI / 180.0;
    e2s[i] = e2s[i] * M_PI / 180.0;
    e3s[i] = e3s[i] * M_PI / 180.0;
  }
  size_t numEntries = e1s.size();

  int imageSize = 226;
  int lamberSize = 22;
  int numColors = 16;
  int npoints = 5000;
  QVector<size_t> dims(1, 3);
  FloatArrayType::Pointer eulers = FloatArrayType::CreateArray(npoints, dims, "Eulers");

  odf.resize(OrthoRhombicOps::k_OdfSize);

  Texture::CalculateOrthoRhombicODFData(e1s.data(), e2s.data(), e3s.data(),
                                        weights.data(), sigmas.data(), true,
                                        odf.data(), numEntries);

  err = StatsGen::GenAxisODFPlotData(odf.data(), eulers->getPointer(0), npoints);

  OrthoRhombicOps ops;
  PoleFigureConfiguration_t config;
  config.eulers = eulers.get();
  config.imageDim = imageSize;
  config.lambertDim = lamberSize;
  config.numColors = numColors;

  QVector<UInt8ArrayType::Pointer> figures = ops.generatePoleFigure(config);
  if (err == 1)
  {
    //TODO: Present Error Message
    return;
  }

  QImage image = PoleFigureImageUtilities::Create3ImagePoleFigure(figures[0].get(), figures[1].get(), figures[2].get(), config);
  m_PoleFigureLabel->setPixmap(QPixmap::fromImage(image));

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGAxisODFWidget::on_addODFTextureBtn_clicked()
{
  TextureDialog t(m_CrystalStructure, NULL);
  int r = t.exec();
  if (r == QDialog::Accepted)
  {
    if (!m_ODFTableModel->insertRow(m_ODFTableModel->rowCount())) { return; }
    // Gather values from the dialog and push them to the Table Model
    float e1 = 0.0;
    float e2 = 0.0;
    float e3 = 0.0;
    float weight = 1.0;
    float sigma = 1.0;

    t.getODFEntry(e1, e2, e3, weight, sigma);
    int row = m_ODFTableModel->rowCount() - 1;
    m_ODFTableModel->setRowData(row, e1, e2, e3, weight, sigma);

    m_ODFTableView->resizeColumnsToContents();
    m_ODFTableView->scrollToBottom();
    m_ODFTableView->setFocus();
    QModelIndex index = m_ODFTableModel->index(m_ODFTableModel->rowCount() - 1, 0);
    m_ODFTableView->setCurrentIndex(index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGAxisODFWidget::on_deleteODFTextureBtn_clicked()
{
  QItemSelectionModel* selectionModel = m_ODFTableView->selectionModel();
  if (!selectionModel->hasSelection()) { return; }
  QModelIndex index = selectionModel->currentIndex();
  if (!index.isValid()) { return; }
  m_ODFTableModel->removeRow(index.row(), index.parent());
  if (m_ODFTableModel->rowCount() > 0)
  {
    m_ODFTableView->resizeColumnsToContents();
  }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGAxisODFWidget::on_loadODFTextureBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory;
  QString file = QFileDialog::getOpenFileName(this, tr("Open Axis ODF File"), proposedFile, tr("Text Document (*.txt)"));
  if(true == file.isEmpty())
  {
    return;
  }
  else
  {
    size_t numOrients = 0;
    QString filename = file;
    std::ifstream inFile;
    inFile.open(filename.toLatin1().data());

    inFile >> numOrients;

    float e1, e2, e3, weight, sigma;
    for(size_t i = 0; i < numOrients; i++)
    {
      inFile >> e1 >> e2 >> e3 >> weight >> sigma;

      if (!m_ODFTableModel->insertRow(m_ODFTableModel->rowCount())) { return; }
      int row = m_ODFTableModel->rowCount() - 1;
      m_ODFTableModel->setRowData(row, e1, e2, e3, weight, sigma);

      m_ODFTableView->resizeColumnsToContents();
      m_ODFTableView->scrollToBottom();
      m_ODFTableView->setFocus();
      QModelIndex index = m_ODFTableModel->index(m_ODFTableModel->rowCount() - 1, 0);
      m_ODFTableView->setCurrentIndex(index);
    }
  }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGODFTableModel* SGAxisODFWidget::tableModel()
{
  return m_ODFTableModel;
}
