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

#include "DREAM3DLib/Common/Texture.h"

#include "StatsGenerator/TableModels/SGODFTableModel.h"
#include "StatsGenerator/StatsGenMDFWidget.h"
#include "StatsGenerator/TextureDialog.h"
#include "DREAM3DLib/Common/StatsGen.h"

#define SHOW_POLE_FIGURES 1
#define COLOR_POLE_FIGURES 1

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGAxisODFWidget::SGAxisODFWidget(QWidget *parent) :
QWidget(parent),
m_EnableAxisDecorations(false),
m_Initializing(true),
m_PhaseIndex(-1),
m_CrystalStructure(Ebsd::CrystalStructure::OrthoRhombic),
m_ODFTableModel(NULL),
m_MDFWidget(NULL),
m_PoleFigureFuture(NULL)
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
void SGAxisODFWidget::extractStatsData(VoxelDataContainer::Pointer m, int index, StatsData* statsData, unsigned int phaseType)
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
  if (arrays.size() > 0 ) {
    QVector<float> e1(static_cast<int>(arrays[0]->GetNumberOfTuples()));
    ::memcpy( &(e1.front()), arrays[0]->GetVoidPointer(0), sizeof(float)*e1.size() );

    QVector<float> e2(static_cast<int>(arrays[0]->GetNumberOfTuples()));
    ::memcpy( &(e2.front()), arrays[0]->GetVoidPointer(0), sizeof(float)*e2.size() );

    QVector<float> e3(static_cast<int>(arrays[0]->GetNumberOfTuples()));
    ::memcpy( &(e3.front()), arrays[0]->GetVoidPointer(0), sizeof(float)*e3.size() );

    QVector<float> weights(static_cast<int>(arrays[0]->GetNumberOfTuples()));
    ::memcpy( &(weights.front()), arrays[0]->GetVoidPointer(0), sizeof(float)*weights.size() );

    QVector<float> sigmas(static_cast<int>(arrays[0]->GetNumberOfTuples()));
    ::memcpy( &(sigmas.front()), arrays[0]->GetVoidPointer(0), sizeof(float)*sigmas.size() );

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

  std::vector<float> e1s;
  std::vector<float> e2s;
  std::vector<float> e3s;
  std::vector<float> weights;
  std::vector<float> sigmas;
  std::vector<float> aodf;

  // Initialize xMax and yMax....
  e1s = m_ODFTableModel->getData(SGODFTableModel::Euler1).toStdVector();
  e2s = m_ODFTableModel->getData(SGODFTableModel::Euler2).toStdVector();
  e3s = m_ODFTableModel->getData(SGODFTableModel::Euler3).toStdVector();
  weights = m_ODFTableModel->getData(SGODFTableModel::Weight).toStdVector();
  sigmas = m_ODFTableModel->getData(SGODFTableModel::Sigma).toStdVector();

  for(std::vector<float>::size_type i=0;i<e1s.size();i++)
  {
    e1s[i] = e1s[i]*M_PI/180.0;
    e2s[i] = e2s[i]*M_PI/180.0;
    e3s[i] = e3s[i]*M_PI/180.0;
  }

  Texture::calculateOrthoRhombicODFData(e1s, e2s, e3s, weights, sigmas, true, aodf);
  if (aodf.size() > 0)
  {
    FloatArrayType::Pointer aodfData = FloatArrayType::FromStdVector(aodf, DREAM3D::HDF5::AxisOrientation);
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
      FloatArrayType::Pointer euler1 = FloatArrayType::FromStdVector(e1s, DREAM3D::HDF5::Euler1);
      FloatArrayType::Pointer euler2 = FloatArrayType::FromStdVector(e2s, DREAM3D::HDF5::Euler2);
      FloatArrayType::Pointer euler3 = FloatArrayType::FromStdVector(e3s, DREAM3D::HDF5::Euler3);
      FloatArrayType::Pointer sigma = FloatArrayType::FromStdVector(sigmas, DREAM3D::HDF5::Sigma);
      FloatArrayType::Pointer weight = FloatArrayType::FromStdVector(weights, DREAM3D::HDF5::Weight);

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

#if SHOW_POLE_FIGURES
  m_PoleFigureFuture = new QFutureWatcher<QImage>(this);
  connect(m_PoleFigureFuture, SIGNAL(resultReadyAt(int)),
          this, SLOT(showPoleFigure(int)));
  connect(m_PoleFigureFuture, SIGNAL(finished()),
          this, SLOT(poleFigureGenerationComplete()));
#else
  // Hide the color Pole Figures in this version
  m_PFScrollArea->hide();
#endif

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGAxisODFWidget::initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot)
{
  plot->setAxisTitle(QwtPlot::xBottom, xAxisName);
  plot->setAxisTitle(QwtPlot::yLeft, yAxisName);
  plot->setCanvasBackground(QColor(Qt::white));
  plot->setCanvasLineWidth(2);
  plot->canvas()->setFrameShape(QFrame::NoFrame);

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
  float inc = 2.0f/449.0f;

  for(int i = 0; i < 450; ++i)
  {
    circleX[i] = 1.0f - (i * inc);
    circleX[450+i] = -1.0f + (i * inc);

    circleY[i] = sqrt(1.0f-(circleX[i]*circleX[i]));
    circleY[450+i] = -circleY[i];
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
    rotCrossY[0] = sqrt(1.0-(rotCrossX[0]*rotCrossX[0]));
    rotCrossX[1] = -0.7071067811f;
    rotCrossY[1] = -sqrt(1.0-(rotCrossX[1]*rotCrossX[1]));
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
    rotCrossY[0] = -sqrt(1.0-(rotCrossX[0]*rotCrossX[0]));
    rotCrossX[1] = -0.7071067811f;
    rotCrossY[1] = sqrt(1.0-(rotCrossX[1]*rotCrossX[1]));
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
void SGAxisODFWidget::showPoleFigure(int imageIndex)
{
 // labels[num]->setPixmap(QPixmap::fromImage(imageScaling->resultAt(num)));
  switch(imageIndex)
  {
    case 0:
    //  m_PoleFigureFuture->resultAt(imageIndex).save("/tmp/AxisODF_PoleFigure_001.tif");
      m_001PF->setPixmap(QPixmap::fromImage(m_PoleFigureFuture->resultAt(imageIndex)));
      break;
    case 1:
    //  m_PoleFigureFuture->resultAt(imageIndex).save("/tmp/AxisODF_PoleFigure_011.tif");
      m_011PF->setPixmap(QPixmap::fromImage(m_PoleFigureFuture->resultAt(imageIndex)));
      break;
    case 2:
    //  m_PoleFigureFuture->resultAt(imageIndex).save("/tmp/AxisODF_PoleFigure_111.tif");
      m_111PF->setPixmap(QPixmap::fromImage(m_PoleFigureFuture->resultAt(imageIndex)));
      break;
    default:
      break;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGAxisODFWidget::poleFigureGenerationComplete()
{
//  std::cout << "ODF Pole Figure generation complete" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage generateAxisODFPoleFigure(const PoleFigureData &data)
{
  PoleFigureMaker colorPoleFigure;
#if COLOR_POLE_FIGURES
  return colorPoleFigure.generateColorPoleFigureImage(data);
#else
    return colorPoleFigure.generatePoleFigureImage(data);
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGAxisODFWidget::on_m_CalculateODFBtn_clicked()
{
  int err = 0;
 // std::cout << "SGAxisODFWidget[" << objectName().toStdString() << "]::on_m_CalculateODFBtn_clicked" << std::endl;
  QwtArray<float> x001;
  QwtArray<float> y001;
  QwtArray<float> x011;
  QwtArray<float> y011;
  QwtArray<float> x111;
  QwtArray<float> y111;

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

  for(int i=0;i<e1s.size();i++)
  {
    e1s[i] = e1s[i] * M_PI / 180.0;
    e2s[i] = e2s[i] * M_PI / 180.0;
    e3s[i] = e3s[i] * M_PI / 180.0;
  }

  StatsGen sg;
  int size = 2500;

  static const size_t odfsize = 46656;
  odf.resize(odfsize);
  Texture::calculateOrthoRhombicODFData(e1s, e2s, e3s, weights, sigmas, true, odf);

  err = sg.GenAxisODFPlotData(odf, x001, y001, x011, y011, x111, y111, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return;
  }

#if SHOW_POLE_FIGURES
  // This is multi-threaded on appropriate hardware.
  qint32 kRad[2] = {5, 5};
  qint32 pfSize[2] = {226, 226};
  QVector<PoleFigureData> data;
  data.push_back(PoleFigureData(x001, y001, QString("A Axis"), kRad, pfSize));
  data.push_back(PoleFigureData(x011, y011, QString("B Axis"), kRad, pfSize));
  data.push_back(PoleFigureData(x111, y111, QString("C Axis"), kRad, pfSize));
  // This kicks off the threads
  m_PoleFigureFuture->setFuture(QtConcurrent::mapped(data, generateAxisODFPoleFigure));

#else
  QwtArray<double> x001d(x001.size());
  QwtArray<double> y001d(y001.size());
  QwtArray<double> x011d(x011.size());
  QwtArray<double> y011d(y011.size());
  QwtArray<double> x111d(x111.size());
  QwtArray<double> y111d(y111.size());
  float minX = 1.0f;
  float maxX = -1.0f;
  float minY = 1.0f;
  float maxY = -1.0f;
  for(int i =0; i < x001.size(); ++i)
  {
    x001d[i] = x001[i];
    y001d[i] = y001[i];
    if (x001[i] < minX) { minX = x001[i];}
    if (x001[i] > maxX) { maxX = x001[i];}
    if (y001[i] < minY) { minY = y001[i];}
    if (y001[i] > maxY) { maxY = y001[i];}
  }
  if (minX < -1 || minY < -1) {
    std::cout << "Minx: " << minX << "  MinY: " << minY << std::endl;
  }
  if (maxX > 1.0 || maxY > 1.0f) {
    std::cout << "maxX: " << maxX << "  maxY: " << maxY << std::endl;
  }
  for(int i =0; i < x011.size(); ++i)
  {
    x011d[i] = x011[i];
    y011d[i] = y011[i];
    if (x011[i] < minX) { minX = x011[i];}
    if (x011[i] > maxX) { maxX = x011[i];}
    if (y011[i] < minY) { minY = y011[i];}
    if (y011[i] > maxY) { maxY = y011[i];}
  }

  if (minX < -1 || minY < -1) {
    std::cout << "Minx: " << minX << "  MinY: " << minY << std::endl;
  }
  if (maxX > 1.0 || maxY > 1.0f) {
    std::cout << "maxX: " << maxX << "  maxY: " << maxY << std::endl;
  }

  for(int i =0; i < x111.size(); ++i)
  {
    x111d[i] = x111[i];
    y111d[i] = y111[i];
    if (x111[i] < minX) { minX = x111[i];}
    if (x111[i] > maxX) { maxX = x111[i];}
    if (y111[i] < minY) { minY = y111[i];}
    if (y111[i] > maxY) { maxY = y111[i];}
  }
  if (minX < -1 || minY < -1) {
    std::cout << "Minx: " << minX << "  MinY: " << minY << std::endl;
  }
  if (maxX > 1.0 || maxY > 1.0f) {
    std::cout << "maxX: " << maxX << "  maxY: " << maxY << std::endl;
  }

//  QwtSymbol symbol;
//  symbol.setStyle(QwtSymbol::Ellipse);
//  symbol.setSize(1,1);

  QwtPlotCurve* curve = m_PlotCurves[0];
#if QWT_VERSION >= 0x060000
  curve->setSamples(x001d, y001d);
#else
  curve->setData(x001d, y001d);
#endif
  curve->setStyle(QwtPlotCurve::Dots);
  curve->attach(m_ODF_001Plot);
//  curve->setSymbol(symbol);
  m_ODF_001Plot->replot();

  curve = m_PlotCurves[1];
#if QWT_VERSION >= 0x060000
  curve->setSamples(x011d, y011d);
#else
  curve->setData(x011d, y011d);
#endif
  curve->setStyle(QwtPlotCurve::Dots);
 // curve->setSymbol(symbol);
  curve->attach(m_ODF_011Plot);
  m_ODF_011Plot->replot();

  curve = m_PlotCurves[2];
#if QWT_VERSION >= 0x060000
  curve->setSamples(x111d, y111d);
#else
  curve->setData(x111d, y111d);
#endif
  curve->setStyle(QwtPlotCurve::Dots);
 // curve->setSymbol(symbol);
  curve->attach(m_ODF_111Plot);
  m_ODF_111Plot->replot();
#endif
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
    if (!m_ODFTableModel->insertRow(m_ODFTableModel->rowCount())) return;
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
  QItemSelectionModel *selectionModel = m_ODFTableView->selectionModel();
  if (!selectionModel->hasSelection()) return;
  QModelIndex index = selectionModel->currentIndex();
  if (!index.isValid()) return;
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
  std::string filename = file.toStdString();
  std::ifstream inFile;
  inFile.open(filename.c_str());

  inFile >> numOrients;

  float e1, e2, e3, weight, sigma;
  for(size_t i = 0; i < numOrients; i++)
  {
    inFile >> e1 >> e2 >> e3 >> weight >> sigma;

    if (!m_ODFTableModel->insertRow(m_ODFTableModel->rowCount())) return;
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
