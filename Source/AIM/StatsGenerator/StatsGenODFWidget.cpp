/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "StatsGenODFWidget.h"

//-- C++ Includes
#include <iostream>

//-- Qt Includes
#include <QtGui/QAbstractItemDelegate>

#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>


#include "SGODFTableModel.h"
#include "AIM/Common/Texture.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenODFWidget::StatsGenODFWidget(QWidget *parent) :
QWidget(parent),
m_TableModel(NULL)
{
  this->setupUi(this);
  this->setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenODFWidget::~StatsGenODFWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::setPlotTitle(QString title)
{
  // this->m_PlotTitle->setText(title);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenODFWidget::readDataFromHDF5(H5ReconStatsReader::Pointer reader,
                                         QVector<double>  &bins,
                                         const std::string &hdf5GroupName)
{
  int err = 0;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenODFWidget::writeDataToHDF5(AIM::Reconstruction::CrystalStructure crystruct,
                                       H5ReconStatsWriter::Pointer writer)
{
  int err = 0;
  double totalWeight = 0.0;

  QwtArray<double> weights;
  QwtArray<double> odf;

  // Initialize xMax and yMax....
  weights = m_TableModel->getData(SGODFTableModel::Weight);

  double randomWeight = weights.front();
  //pop off the random number
  weights.pop_front();

  Texture::calculateCubicODFData(weights, randomWeight, false, odf, totalWeight);
  double* odfPtr = &(odf.front());
  err = writer->writeODFData(crystruct, odfPtr, totalWeight);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::setupGui()
{

  // Setup the TableView and Table Models
  QHeaderView* headerView = new QHeaderView(Qt::Horizontal, m_TableView);
  headerView->setResizeMode(QHeaderView::Interactive);
  m_TableView->setHorizontalHeader(headerView);
  headerView->show();

  resetTableModel();

  initQwtPlot("x axis", "y axis", m_Plot1);
  initQwtPlot("x axis", "y axis", m_Plot2);
  initQwtPlot("x axis", "y axis", m_Plot3);

  m_PlotCurves.push_back(new QwtPlotCurve);
  m_PlotCurves.push_back(new QwtPlotCurve);
  m_PlotCurves.push_back(new QwtPlotCurve);


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot)
{
  plot->setAxisTitle(QwtPlot::xBottom, xAxisName);
  plot->setAxisTitle(QwtPlot::yLeft, yAxisName);
  plot->setCanvasBackground(QColor(Qt::white));


#if 0
  m_grid = new QwtPlotGrid;
  m_grid->enableXMin(true);
  m_grid->enableYMin(true);
  m_grid->setMajPen(QPen(Qt::gray, 0, Qt::SolidLine));
  m_grid->setMinPen(QPen(Qt::lightGray, 0, Qt::DotLine));
  m_grid->attach(m_PlotView);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::resetTableModel()
{

  if (NULL != m_TableModel)
  {
    m_TableModel->deleteLater();
  }

  m_TableModel = new SGODFTableModel;
  m_TableModel->setInitialValues();

  m_TableView->setModel(m_TableModel);
  QAbstractItemDelegate* aid = m_TableModel->getItemDelegate();

  m_TableView->setItemDelegate(aid);

//
//  connect(m_TableModel, SIGNAL(layoutChanged()),
//    this, SLOT(updatePlotCurves()));
//  connect(m_TableModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
//    this, SLOT(updatePlotCurves()));

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::setXAxisName(QString name)
{
//  m_PlotView->setAxisTitle(QwtPlot::xBottom, name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::setYAxisName(QString name)
{
 // m_PlotView->setAxisTitle(QwtPlot::yLeft, name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_m_CalculateODFBtn_clicked()
{

  int err = 0;


  QwtArray<double > x001;
  QwtArray<double > y001;
  QwtArray<double > x011;
  QwtArray<double > y011;
  QwtArray<double > x111;
  QwtArray<double > y111;
  QwtArray<double > weights;
  QwtArray<double > sigmas;
  StatsGen sg;
  int size = 1000;

  // Initialize xMax and yMax....
  weights = m_TableModel->getData(SGODFTableModel::Weight);
  sigmas = m_TableModel->getData(SGODFTableModel::Sigma);

  double randomWeight = weights.front();
  weights.pop_front();
  sigmas.pop_front();
  //pop off the random number
  err = sg.GenCubicODFPlotData(weights, sigmas, x001, y001, x011, y011, x111, y111, size, randomWeight);
  if (err == 1)
  {
    //TODO: Present Error Message
    return;
  }

  QwtPlotCurve* curve = m_PlotCurves[0];
  curve->setData(x001, y001);
  curve->setStyle(QwtPlotCurve::Dots);
  curve->attach(m_Plot1);
  m_Plot1->setAxisScale(QwtPlot::yLeft, -1.0, 1.0);
  m_Plot1->setAxisScale(QwtPlot::xBottom, -1.0, 1.0);
  m_Plot1->replot();

  curve = m_PlotCurves[1];
  curve->setData(x011, y011);
  curve->setStyle(QwtPlotCurve::Dots);
  curve->attach(m_Plot2);
  m_Plot2->setAxisScale(QwtPlot::yLeft, -1.0, 1.0);
  m_Plot2->setAxisScale(QwtPlot::xBottom, -1.0, 1.0);
  m_Plot2->replot();

  curve = m_PlotCurves[2];
  curve->setData(x111, y111);
  curve->setStyle(QwtPlotCurve::Dots);
  curve->attach(m_Plot3);
  m_Plot3->setAxisScale(QwtPlot::yLeft, -1.0, 1.0);
  m_Plot3->setAxisScale(QwtPlot::xBottom, -1.0, 1.0);
  m_Plot3->replot();

}
