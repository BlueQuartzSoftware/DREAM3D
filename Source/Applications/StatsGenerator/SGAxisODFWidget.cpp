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

//-- C++ Includes
#include <iostream>

//-- Qt Includes
#include <QtGui/QAbstractItemDelegate>

#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_abstract_scale_draw.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3D/Common/Texture.h"

#include "StatsGenerator/TableModels/SGODFTableModel.h"
#include "StatsGenerator/StatsGenMDFWidget.h"
#include "StatsGenerator/TextureDialog.h"
#include "StatsGen.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGAxisODFWidget::SGAxisODFWidget(QWidget *parent) :
QWidget(parent),
m_EnableAxisDecorations(false),
m_Initializing(true),
m_PhaseIndex(-1),
m_CrystalStructure(Ebsd::AxisOrthoRhombic),
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
int SGAxisODFWidget::readDataFromHDF5(H5ReconStatsReader::Pointer reader, int phase)
{
  int err = -1;
  std::string index = StringUtils::numToString(phase);
  std::string path = "/" + DREAM3D::HDF5::Reconstruction + "/" + index  + "/" + DREAM3D::HDF5::AxisODFWeights;

  //FIXME: Do we load the ODF data array at all or generate a new one?


  // Load the ODF Weights and Spreads Table data
  HDF_ERROR_HANDLER_OFF;
  std::vector<float> e1;
  err = reader->readVectorDataset(path, DREAM3D::HDF5::Euler1, e1);
  if (e1.size() > 0)
  {
    std::vector<float> e2;
    err = reader->readVectorDataset(path, DREAM3D::HDF5::Euler2, e2);
    std::vector<float> e3;
    err = reader->readVectorDataset(path, DREAM3D::HDF5::Euler3, e3);
    std::vector<float> weights;
    err = reader->readVectorDataset(path, DREAM3D::HDF5::Weight, weights);
    std::vector<float> sigmas;
    err = reader->readVectorDataset(path, DREAM3D::HDF5::Sigma, sigmas);

    // Load the data into the table model
    m_ODFTableModel->setTableData(QVector<float>::fromStdVector(e1),
                                  QVector<float>::fromStdVector(e2),
                                  QVector<float>::fromStdVector(e3),
                                  QVector<float>::fromStdVector(weights),
                                  QVector<float>::fromStdVector(sigmas));
  }

  HDF_ERROR_HANDLER_ON

  updatePlots();

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SGAxisODFWidget::writeDataToHDF5(H5ReconStatsWriter::Pointer writer)
{
  int err = -1;
  int retErr = 0;
  float totalWeight = 0.0;

  QwtArray<float> e1s;
  QwtArray<float> e2s;
  QwtArray<float> e3s;
  QwtArray<float> weights;
  QwtArray<float> sigmas;
  QwtArray<float> aodf;

  // Initialize xMax and yMax....
  e1s = m_ODFTableModel->getData(SGODFTableModel::Euler1);
  e2s = m_ODFTableModel->getData(SGODFTableModel::Euler2);
  e3s = m_ODFTableModel->getData(SGODFTableModel::Euler3);
  weights = m_ODFTableModel->getData(SGODFTableModel::Weight);
  sigmas = m_ODFTableModel->getData(SGODFTableModel::Sigma);

  for(int i=0;i<e1s.size();i++)
  {
    e1s[i] = e1s[i]*M_PI/180.0;
    e2s[i] = e2s[i]*M_PI/180.0;
    e3s[i] = e3s[i]*M_PI/180.0;
  }

  Texture::calculateOrthoRhombicODFData(e1s, e2s, e3s, weights, sigmas, true, aodf, totalWeight);
  if (aodf.size() > 0)
  {
    float* aodfPtr = &(aodf.front());
    err = -1;
    if (aodfPtr != NULL)
    {
      uint64_t dims = 36 * 36 * 36;
      err = writer->writeAxisOrientationData(m_PhaseIndex, aodfPtr, 1.0);
      if (err < 0)
      {
        //FIXME: Display an error message
        retErr = err;
      }
      dims = e1s.size();
      if (dims > 0)
      {
        err = writer->writeAxisODFWeights(m_PhaseIndex, &dims, &(e1s.front()),  &(e2s.front()),  &(e3s.front()),  &(weights.front()),  &(sigmas.front()));
        if (err < 0)
        {
          //FIXME: Display an error message
          retErr = err;
        }
      }
    }
  }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::CrystalStructure SGAxisODFWidget::getCrystalStructure()
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

  initQwtPlot("RD", "TD", m_ODF_001Plot);
  initQwtPlot("RD", "TD", m_ODF_011Plot);
  initQwtPlot("RD", "TD", m_ODF_111Plot);

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
  float inc = 2.0/449.0;

  for(int i = 0; i < 450; ++i)
  {
    circleX[i] = 1.0 - (i * inc);
    circleX[450+i] = -1.0 + (i * inc);

    circleY[i] = sqrt(1.0-(circleX[i]*circleX[i]));
    circleY[450+i] = -circleY[i];
  }
  m_CircleGrid = new QwtPlotCurve;
  m_CircleGrid->setData(circleX, circleY);
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
    m_RotCross0->setData(rotCrossX, rotCrossY);
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
    m_RotCross1->setData(rotCrossX, rotCrossY);
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
	e1s[i] = e1s[i]*M_PI/180.0;
	e2s[i] = e2s[i]*M_PI/180.0;
	e3s[i] = e3s[i]*M_PI/180.0;
  }

  StatsGen sg;
  int size = 2500;

  static const size_t odfsize = 46656;
  float totalweight = 0;
  odf.resize(odfsize);
  Texture::calculateOrthoRhombicODFData(e1s, e2s, e3s, weights, sigmas, true, odf, totalweight);

  err = sg.GenAxisODFPlotData(odf, x001, y001, x011, y011, x111, y111, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return;
  }

  QwtArray<double> x001d(x001.size());
  QwtArray<double> y001d(y001.size());
  QwtArray<double> x011d(x011.size());
  QwtArray<double> y011d(y011.size());
  QwtArray<double> x111d(x111.size());
  QwtArray<double> y111d(y111.size());
  for(int i =0; i < x001.size(); ++i)
  {
    x001d[i] = x001[i];
    y001d[i] = y001[i];
  }
  for(int i =0; i < x011.size(); ++i)
  {
    x011d[i] = x011[i];
    y011d[i] = y011[i];
  }
  for(int i =0; i < x111.size(); ++i)
  {
    x111d[i] = x111[i];
    y111d[i] = y111[i];
  }

//  QwtSymbol symbol;
//  symbol.setStyle(QwtSymbol::Ellipse);
//  symbol.setSize(1,1);

  QwtPlotCurve* curve = m_PlotCurves[0];
  curve->setData(x001d, y001d);
  curve->setStyle(QwtPlotCurve::Dots);
  curve->attach(m_ODF_001Plot);
//  curve->setSymbol(symbol);
  m_ODF_001Plot->replot();

  curve = m_PlotCurves[1];
  curve->setData(x011d, y011d);
  curve->setStyle(QwtPlotCurve::Dots);
 // curve->setSymbol(symbol);
  curve->attach(m_ODF_011Plot);
  m_ODF_011Plot->replot();

  curve = m_PlotCurves[2];
  curve->setData(x111d, y111d);
  curve->setStyle(QwtPlotCurve::Dots);
 // curve->setSymbol(symbol);
  curve->attach(m_ODF_111Plot);
  m_ODF_111Plot->replot();
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
SGODFTableModel* SGAxisODFWidget::tableModel()
{
  return m_ODFTableModel;
}
