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

#include "StatsGenPlotWidget.h"
#include "StatsGenTableModel.h"

#include <iostream>

//-- Qwt Includes
#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_interval_data.h>
#include <qwt_painter.h>
#include <qwt_scale_map.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_curve.h>




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenPlotWidget::StatsGenPlotWidget(QWidget *parent) :
QWidget(parent),
m_TableModel(NULL),
m_zoomer(NULL),
m_picker(NULL),
m_panner(NULL),
m_grid(NULL),
m_CurveType(StatsGen::LogNormal)
{
  this->setupUi(this);
  this->setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenPlotWidget::~StatsGenPlotWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::setPlotTitle(QString title)
{
  this->m_PlotTitle->setText(title);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenPlotWidget::writeDataToHDF5(QString hdf5File)
{
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::setCurveType(StatsGen::CurveType curveType)
{
  m_CurveType = curveType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::setXAxisName(QString name)
{
  m_PlotView->setAxisTitle(QwtPlot::xBottom, name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::setYAxisName(QString name)
{
  m_PlotView->setAxisTitle(QwtPlot::yLeft, name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::setupGui()
{
  // Setup the TableView and Table Models
  QHeaderView* headerView = new QHeaderView(Qt::Horizontal, m_TableView);
  headerView->setResizeMode(QHeaderView::Interactive);
  m_TableView->setHorizontalHeader(headerView);
  m_TableModel = new StatsGenTableModel;
  m_TableView->setModel(m_TableModel);
  headerView->show();

  // Setup the Qwt Plot Wigets
  // Configure the Histogram Plot
  m_PlotView->setCanvasBackground(QColor(Qt::white));
//  m_PlotView->setTitle(m_PlotTitle->text());

  m_grid = new QwtPlotGrid;
  m_grid->enableXMin(true);
  m_grid->enableYMin(true);
  m_grid->setMajPen(QPen(Qt::gray, 0, Qt::SolidLine));
  m_grid->setMinPen(QPen(Qt::lightGray, 0, Qt::DotLine));
  m_grid->attach(m_PlotView);

  // Add the ability to pan the plots
  m_panner = new QwtPlotPanner(m_PlotView->canvas());
  m_panner->setMouseButton(Qt::MidButton);


  connect(m_TableModel, SIGNAL(layoutChanged()),
          this, SLOT(updatePlot()));

//  m_TableModel->addRow(0, 10, 1.0, 0.5, Qt::red);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::updatePlot()
{
  std::cout << "update Plot" << std::endl;
  //Loop over each entry in the table
  QwtPlotCurve* curve = NULL;
  qint32 nRows = m_TableModel->rowCount();
  for(qint32 r = 0; r < nRows; ++r)
  {
    if (r == m_PlotCurves.size())
    {
      curve = new QwtPlotCurve("");
      curve->setRenderHint(QwtPlotItem::RenderAntialiased);
      curve->setPen(QPen(m_TableModel->getColors().at(r)));
      curve->attach(m_PlotView);
      m_PlotCurves.append(curve);
    }
    switch(m_CurveType)
    {
      case StatsGen::Beta:
        createBetaCurve(r);
        break;
      case StatsGen::LogNormal:
        createLogNormalCurve(r);
        break;
      case StatsGen::Power:
        createPowerCurve(r);
        break;
      default:
        break;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::createBetaCurve(int tableRow)
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::createLogNormalCurve(int tableRow)
{
  QwtPlotCurve* curve = m_PlotCurves[tableRow];
  int err = 0;
  double mu = m_TableModel->getMu(tableRow);
  double sigma = m_TableModel->getSigma(tableRow);
  int size = 256;
  QwtArray<double > x;
  QwtArray<double > y;
  StatsGen sg;
  err = sg.GenLogNormal<QwtArray<double > > (mu, sigma, x, y, size);
  err = sg.GenLogNormal<QwtArray<double > > (mu, sigma, x, y, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return;
  }
//  double xMax = std::numeric_limits<double>::min();
//  double yMax = std::numeric_limits<double>::min();
  double xMax = 0.0;
  double yMax = 0.0;
  for (int i = 0; i < size; ++i)
  {
 //   std::cout << x[i] << "  " << y[i] << std::endl;
    if (x[i] > xMax) { xMax = x[i]; }
    if (y[i] > yMax) { yMax = y[i]; }
  }
  curve->setData(x, y);
  m_PlotView->setAxisScale(QwtPlot::yLeft, 0.0, yMax);
  m_PlotView->setAxisScale(QwtPlot::xBottom, 0.0, xMax);
  m_PlotView->replot();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::createPowerCurve(int tableRow)
{

}


void StatsGenPlotWidget::setRowOperationEnabled(bool b)
{
  addRowBtn->setVisible(b);
  deleteRowBtn->setVisible(b);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::on_deleteRowBtn_clicked()
{
QModelIndexList selections = m_TableView->selectionModel()->selectedRows(0);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::on_addRowBtn_clicked()
{
  m_TableModel->addRow(0, 1, 1.0, 0.5);
}
