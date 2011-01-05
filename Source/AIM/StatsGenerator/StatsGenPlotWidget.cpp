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


#include "SGItemDelegate.h"

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
 // this->m_PlotTitle->setText(title);
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
  m_TableView->setItemDelegate(new SGItemDelegate(this));
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
  connect(m_TableModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
          this, SLOT(updatePlot()));

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::updatePlot()
{
 // std::cout << "StatsGenPlotWidget::updatePlot" << std::endl;
  //Loop over each entry in the table
  QwtPlotCurve* curve = NULL;

  // Delete all the plots
  qint32 nRows = m_PlotCurves.count();
  for (qint32 r = nRows - 1; r >= 0; --r)
  {
    curve = m_PlotCurves[r];
    curve->detach();
    m_PlotCurves.remove(r);
    delete curve;
  }

  nRows = m_TableModel->rowCount();
  double xMax = 0.0;
  double yMax = 0.0;

  for(qint32 r = 0; r < nRows; ++r)
  {
    if (r == m_PlotCurves.size())
    {
      curve = new QwtPlotCurve("");
      curve->setRenderHint(QwtPlotItem::RenderAntialiased);
      QColor c = QColor(m_TableModel->getColor(r));
      c.setAlpha(UIA::Alpha);
      curve->setPen(QPen(c));
      curve->attach(m_PlotView);
      m_PlotCurves.append(curve);
    }
    else
    {
      curve = m_PlotCurves[r];
      QColor c = QColor(m_TableModel->getColor(r));
      c.setAlpha(UIA::Alpha);
      curve->setPen(QPen(c));
    }

    switch(m_CurveType)
    {
      case StatsGen::Beta:
        createBetaCurve(r, xMax, yMax);
        break;
      case StatsGen::LogNormal:
        createLogNormalCurve(r, xMax, yMax);
        break;
      case StatsGen::Power:
        createPowerCurve(r, xMax, yMax);
        break;
      default:
        break;
    }
  }
  m_PlotView->replot();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::createBetaCurve(int tableRow, double &xMax, double &yMax)
{
  QwtPlotCurve* curve = m_PlotCurves[tableRow];
  int err = 0;
  double mu = m_TableModel->getMu(tableRow);
  double sigma = m_TableModel->getSigma(tableRow);
  int size = 256;
  QwtArray<double > x;
  QwtArray<double > y;
  StatsGen sg;
  err = sg.GenBeta<QwtArray<double > > (mu, sigma, x, y, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return;
  }

  for (int i = 0; i < size; ++i)
  {
 //   std::cout << x[i] << "  " << y[i] << std::endl;
    if (x[i] > xMax) { xMax = x[i]; }
    if (y[i] > yMax) { yMax = y[i]; }
  }
  curve->setData(x, y);

  m_PlotView->setAxisScale(QwtPlot::yLeft, 0.0, yMax);
  m_PlotView->setAxisScale(QwtPlot::xBottom, 0.0, xMax);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::createLogNormalCurve(int tableRow, double &xMax, double &yMax)
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
  if (err == 1)
  {
    //TODO: Present Error Message
    return;
  }

  for (int i = 0; i < size; ++i)
  {
 //   std::cout << x[i] << "  " << y[i] << std::endl;
    if (x[i] > xMax) { xMax = x[i]; }
    if (y[i] > yMax) { yMax = y[i]; }
  }
  curve->setData(x, y);

  m_PlotView->setAxisScale(QwtPlot::yLeft, 0.0, yMax);
  m_PlotView->setAxisScale(QwtPlot::xBottom, 0.0, xMax);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::createPowerCurve(int tableRow, double &xMax, double &yMax)
{
  QwtPlotCurve* curve = m_PlotCurves[tableRow];
  int err = 0;
  double mu = m_TableModel->getMu(tableRow);
  double sigma = m_TableModel->getSigma(tableRow);
  int size = 256;
  QwtArray<double > x;
  QwtArray<double > y;
  StatsGen sg;
  err = sg.GenPowerLaw<QwtArray<double > > (mu, sigma, x, y, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return;
  }

  for (int i = 0; i < size; ++i)
  {
 //   std::cout << x[i] << "  " << y[i] << std::endl;
    if (x[i] > xMax) { xMax = x[i]; }
    if (y[i] > yMax) { yMax = y[i]; }
  }
  curve->setData(x, y);

  m_PlotView->setAxisScale(QwtPlot::yLeft, 0.0, yMax);
  m_PlotView->setAxisScale(QwtPlot::xBottom, 0.0, xMax);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::setRowOperationEnabled(bool b)
{
  addRowBtn->setVisible(b);
  deleteRowBtn->setVisible(b);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::setBins(QVector<int> &binNumbers)
{
  qint32 count = binNumbers.count();

  // Remove all the current rows in the table model
  m_TableModel->removeRows(0, m_TableModel->rowCount());

  double mu = 1.0;

  QStringList colorNames = QColor::colorNames();
  qint32 colorOffset = 21;

  // Now Populate the table data with the data that was passed in
  for (qint32 i = 0; i < count; ++i)
  {
    if (!m_TableModel->insertRow(m_TableModel->rowCount()))
        return;
    m_TableView->resizeColumnsToContents();
    m_TableView->scrollToBottom();
    m_TableView->setFocus();
    QModelIndex binNumberIndex = m_TableModel->index(m_TableModel->rowCount() - 1, StatsGenTableModel::BinNumber);
    m_TableView->setCurrentIndex(binNumberIndex);
    m_TableModel->setData(binNumberIndex, QVariant(binNumbers[i]), Qt::EditRole);

    QModelIndex muIndex = m_TableModel->index(m_TableModel->rowCount() - 1, StatsGenTableModel::Mu);
    m_TableView->setCurrentIndex(muIndex);
    m_TableModel->setData(muIndex, QVariant(mu++), Qt::EditRole);

    QModelIndex colorIndex = m_TableModel->index(m_TableModel->rowCount() - 1, StatsGenTableModel::LineColor);
    m_TableView->setCurrentIndex(colorIndex);
    m_TableModel->setData(colorIndex, QVariant(colorNames[colorOffset++]), Qt::EditRole);
    if (colorOffset == colorNames.count() ) 
    {
      colorOffset = colorNames.count() - 1;
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::on_addRowBtn_clicked()
{
  if (!m_TableModel->insertRow(m_TableModel->rowCount()))
      return;
  m_TableView->resizeColumnsToContents();
  m_TableView->scrollToBottom();
  m_TableView->setFocus();
  QModelIndex index = m_TableModel->index(m_TableModel->rowCount() - 1, 0);
  m_TableView->setCurrentIndex(index);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::on_deleteRowBtn_clicked()
{
  QItemSelectionModel *selectionModel = m_TableView->selectionModel();
  if (!selectionModel->hasSelection())
      return;
  QModelIndex index =  selectionModel->currentIndex();
  if (!index.isValid())
      return;
  m_TableModel->removeRow(index.row(), index.parent());
  if (m_TableModel->rowCount() > 0)
  {
    m_TableView->resizeColumnsToContents();
  }
}
