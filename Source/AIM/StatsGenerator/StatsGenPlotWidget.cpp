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

//-- C++ Includes
#include <iostream>
#include <limits>

//-- Qt Includes
#include <QtGui/QMessageBox>

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

#include "AIM/Common/Constants.h"
#include "SGBetaTableModel.h"
#include "SGLogNormalTableModel.h"
#include "SGPowerLawTableModel.h"

#define SG_ERROR_CHECK(name)\
    std::cout << "Error writing HDF5 data to " << name << std::endl;\
    std::cout << "  File: " << __FILE__ << std::endl;\
    std::cout << "  Line: " << __LINE__ << std::endl;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenPlotWidget::StatsGenPlotWidget(QWidget *parent) :
QWidget(parent), m_TableModel(NULL),
//m_zoomer(NULL), m_picker(NULL), m_panner(NULL),
m_grid(NULL),
m_DistributionType(AIM::Reconstruction::UnknownDistributionType),
m_StatsType(AIM::Reconstruction::UnknownStatisticsGroup)
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
void StatsGenPlotWidget::setStatisticsType(AIM::Reconstruction::StatisticsType distributionType)
{
  m_StatsType = distributionType;
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
int StatsGenPlotWidget::writeDataToHDF5(H5ReconStatsWriter::Pointer writer, const std::string &hdf5GroupName)
{
  int err = 0;
  if (m_StatsType == AIM::Reconstruction::UnknownStatisticsGroup)
  {
    QMessageBox::critical(this, tr("AIM Representation"),
    tr("This Plot has not been assigned a Statistics Group. This should be happening from within the program. Contact the developer."),
    QMessageBox::Ok,
    QMessageBox::Ok);
    return -1;
  }

  if (m_DistributionType == AIM::Reconstruction::UnknownDistributionType)
  {
    QMessageBox::critical(this, tr("AIM Representation"),
    tr("This Plot has not been assigned a known Distribution Type. This should be happening from within the program. Contact the developer."),
    QMessageBox::Ok,
    QMessageBox::Ok);
    return -1;
  }


//  QVector<int> binNumbers = m_TableModel->getBinNumbers();
//  int mindiameter = std::numeric_limits<int >::max();
//  int maxdiameter = std::numeric_limits<int >::min();
// // std::cout << "Bin#" << std::endl;
//  size_t numsizebins = binNumbers.size();
//  for (size_t i = 0; i < numsizebins; ++i)
//  {
//    if (binNumbers[i] < mindiameter) { mindiameter = binNumbers[i]; }
//    if (binNumbers[i] > maxdiameter) { maxdiameter = binNumbers[i]; }
//  }
  std::vector<double> col0;
  std::vector<double> col1;
  std::vector<double> col2;

  QVector<double> qcol0;
  QVector<double> qcol1;
  QVector<double> qcol2;

  // Create a new Table Model
  switch(m_DistributionType)
  {
  case AIM::Reconstruction::Beta:
    col0 = m_TableModel->getData(SGBetaTableModel::Alpha).toStdVector();
    col1 = m_TableModel->getData(SGBetaTableModel::Beta).toStdVector();
    err = writer->writeBetaDistribution(hdf5GroupName, col0, col1);
    if (err < 0) { SG_ERROR_CHECK(hdf5GroupName) }
    break;
  case AIM::Reconstruction::LogNormal:
    col0 = m_TableModel->getData(SGLogNormalTableModel::Average).toStdVector();
    col1 = m_TableModel->getData(SGLogNormalTableModel::StdDev).toStdVector();
    err = writer->writeLogNormalDistribution(hdf5GroupName, col0, col1);
    if (err < 0) { SG_ERROR_CHECK(hdf5GroupName) }
    break;
  case AIM::Reconstruction::Power:
    col0 = m_TableModel->getData(SGPowerLawTableModel::Alpha).toStdVector();
    col1 = m_TableModel->getData(SGPowerLawTableModel::K).toStdVector();
    col2 = m_TableModel->getData(SGPowerLawTableModel::Beta).toStdVector();
    err = writer->writePowerDistribution(hdf5GroupName, col0, col1, col2);
    if (err < 0) { SG_ERROR_CHECK(hdf5GroupName) }
    break;

  default:
    return -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::resetTableModel()
{
  QVector<qint32> bins;
  // Get a copy of the bins from the current TableModel if available
  if (NULL != m_TableModel)
  {
    bins = m_TableModel->getBinNumbers();
  }


  // Create a new Table Model
  switch(m_DistributionType)
  {
  case AIM::Reconstruction::Beta:
    m_TableModel = new SGBetaTableModel;
    break;
  case AIM::Reconstruction::LogNormal:
    m_TableModel = new SGLogNormalTableModel;
    break;
  case AIM::Reconstruction::Power:
    m_TableModel = new SGPowerLawTableModel;
    break;

  default:
    return;
  }


  m_TableView->setModel(m_TableModel);
  m_TableView->setItemDelegate(m_TableModel->getItemDelegate());
  setBins(bins);

  connect(m_TableModel, SIGNAL(layoutChanged()),
    this, SLOT(updatePlotCurves()));
  connect(m_TableModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
    this, SLOT(updatePlotCurves()));

  // Update the plots
  updatePlotCurves();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::setDistributionType(AIM::Reconstruction::DistributionType curveType)
{
  m_DistributionType = curveType;
  distributionTypeCombo->setCurrentIndex(m_DistributionType);
  resetTableModel();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::on_distributionTypeCombo_currentIndexChanged(int index)
{
  m_DistributionType = static_cast<AIM::Reconstruction::DistributionType>(distributionTypeCombo->currentIndex());
  resetTableModel();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::blockDistributionTypeChanges(bool block)
{
  distributionTypeCombo->setEnabled(!block);
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

  distributionTypeCombo->addItem(QString("Beta"));
  distributionTypeCombo->addItem(QString("Log Normal"));
  distributionTypeCombo->addItem(QString("Power Law"));

  // Setup the TableView and Table Models
  QHeaderView* headerView = new QHeaderView(Qt::Horizontal, m_TableView);
  headerView->setResizeMode(QHeaderView::Interactive);
  m_TableView->setHorizontalHeader(headerView);
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
//  m_panner = new QwtPlotPanner(m_PlotView->canvas());
//  m_panner->setMouseButton(Qt::MidButton);

  resetTableModel();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::updatePlotCurves()
{
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

  for (qint32 r = 0; r < nRows; ++r)
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

    switch(m_DistributionType)
    {
      case AIM::Reconstruction::Beta:
        createBetaCurve(r, xMax, yMax);
        break;
      case AIM::Reconstruction::LogNormal:
        createLogNormalCurve(r, xMax, yMax);
        break;
      case AIM::Reconstruction::Power:
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
  double alpha = m_TableModel->getDataValue(SGBetaTableModel::Alpha, tableRow);
  double beta = m_TableModel->getDataValue(SGBetaTableModel::Beta, tableRow);
  int size = 256;
  QwtArray<double > x;
  QwtArray<double > y;
  StatsGen sg;
  err = sg.GenBeta<QwtArray<double > > (alpha, beta, x, y, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return;
  }

  for (int i = 0; i < size; ++i)
  {
    //   std::cout << x[i] << "  " << y[i] << std::endl;
    if (x[i] > xMax)
    {
      xMax = x[i];
    }
    if (y[i] > yMax)
    {
      yMax = y[i];
    }
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
  double avg = m_TableModel->getDataValue(SGLogNormalTableModel::Average, tableRow);
  double stdDev = m_TableModel->getDataValue(SGLogNormalTableModel::StdDev, tableRow);
  int size = 256;
  QwtArray<double > x;
  QwtArray<double > y;
  StatsGen sg;
  err = sg.GenLogNormal<QwtArray<double > > (avg, stdDev, x, y, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return;
  }

  for (int i = 0; i < size; ++i)
  {
    //   std::cout << x[i] << "  " << y[i] << std::endl;
    if (x[i] > xMax)
    {
      xMax = x[i];
    }
    if (y[i] > yMax)
    {
      yMax = y[i];
    }
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
  double alpha = m_TableModel->getDataValue(SGPowerLawTableModel::Alpha, tableRow);
  double k = m_TableModel->getDataValue(SGPowerLawTableModel::K, tableRow);
  double beta = m_TableModel->getDataValue(SGPowerLawTableModel::Beta, tableRow);
  int size = 256;
  QwtArray<double > x;
  QwtArray<double > y;
  StatsGen sg;
  err = sg.GenPowerLaw<QwtArray<double > > (alpha, k, beta, x, y, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return;
  }

  for (int i = 0; i < size; ++i)
  {
    //   std::cout << x[i] << "  " << y[i] << std::endl;
    if (x[i] > xMax)
    {
      xMax = x[i];
    }
    if (y[i] > yMax)
    {
      yMax = y[i];
    }
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
void StatsGenPlotWidget::setBins(QVector<int > &binNumbers)
{
  m_TableModel->setBinNumbers(binNumbers);
  m_TableView->resizeColumnsToContents();
  m_TableView->scrollToBottom();
  m_TableView->setFocus();
  updatePlotCurves();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::on_addRowBtn_clicked()
{
  if (!m_TableModel->insertRow(m_TableModel->rowCount())) return;
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
  if (!selectionModel->hasSelection()) return;
  QModelIndex index = selectionModel->currentIndex();
  if (!index.isValid()) return;
  m_TableModel->removeRow(index.row(), index.parent());
  if (m_TableModel->rowCount() > 0)
  {
    m_TableView->resizeColumnsToContents();
  }
}
