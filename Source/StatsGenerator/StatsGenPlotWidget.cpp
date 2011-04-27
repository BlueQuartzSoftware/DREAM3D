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
#include <QtGui/QAbstractItemDelegate>

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
#include "StatsGenerator/TableModels/SGBetaTableModel.h"
#include "StatsGenerator/TableModels/SGLogNormalTableModel.h"
#include "StatsGenerator/TableModels/SGPowerLawTableModel.h"
#include "StatsGen.h"

#define SG_ERROR_CHECK(name)\
    std::cout << "Error writing HDF5 data to " << name << std::endl;\
    std::cout << "  File: " << __FILE__ << std::endl;\
    std::cout << "  Line: " << __LINE__ << std::endl;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenPlotWidget::StatsGenPlotWidget(QWidget *parent) :
QWidget(parent),
m_Mu(1.0),
m_Sigma(0.1),
m_Cutoff(3),
m_BinStep(1.0),
m_PhaseIndex(-1),
m_TableModel(NULL),
//m_zoomer(NULL), m_picker(NULL), m_panner(NULL),
m_grid(NULL),
m_DistributionType(AIM::Reconstruction::UnknownDistributionType),
m_StatsType(AIM::Reconstruction::UnknownStatisticsGroup),
m_UserUpdatedData(false)
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
void StatsGenPlotWidget::setSizeDistributionValues(double mu, double sigma, double cutOff, double binStepSize)
{
  m_Mu = mu;
  m_Sigma = sigma;
  m_Cutoff = cutOff;
  m_BinStep = binStepSize;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenPlotWidget::readDataFromHDF5(H5ReconStatsReader::Pointer reader,
                                         QVector<double>  &bins,
                                         const std::string &hdf5GroupName)
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

  AIM::Reconstruction::DistributionType dt;
  std::string disType = reader->getDistributionType(m_PhaseIndex, hdf5GroupName, dt);
  if (dt == AIM::Reconstruction::UnknownDistributionType)
  {
    QMessageBox::critical(this, tr("AIM Representation"),
    tr("The 'Distribution Type' attribute was either not found on the HDF5 Group or it was of an unknown type. Please verify your HDF5 file has been written correctly"),
    QMessageBox::Ok,
    QMessageBox::Ok);
    return -1;
  }
  setDistributionType(dt); // This makes sure the combo box is set correctly

  //FIXME: This is broken. We will need another way of adding the proper number of rows to the table model
 // m_TableModel->setBinNumbers(bins);

  AIM::HDF5::ColumnCount colCount = AIM::HDF5::UnknownColumCount;
  std::vector<double> col0;
  std::vector<double> col1;
  std::vector<double> col2;

  std::vector<std::vector<double> > columnsData;

  std::string datasetName;
  // Create a new Table Model
  switch(m_DistributionType)
  {
  case AIM::Reconstruction::Beta:
    datasetName = hdf5GroupName + "/" + AIM::HDF5::Alpha;
    err = reader->readStatsDataset(m_PhaseIndex, datasetName, col0);
    datasetName = hdf5GroupName + "/" + AIM::HDF5::Beta;
    err = reader->readStatsDataset(m_PhaseIndex, datasetName, col1);
    colCount = AIM::HDF5::BetaColumnCount;
    columnsData.push_back(col0);
    columnsData.push_back(col1);
    if (err < 0) { SG_ERROR_CHECK(hdf5GroupName) }
    break;
  case AIM::Reconstruction::LogNormal:
    datasetName = hdf5GroupName + "/" + AIM::HDF5::Average;
    err = reader->readStatsDataset(m_PhaseIndex, datasetName, col0);
    datasetName = hdf5GroupName + "/" + AIM::HDF5::StandardDeviation;
    err = reader->readStatsDataset(m_PhaseIndex, datasetName, col1);
    colCount = AIM::HDF5::LogNormalColumnCount;
    columnsData.push_back(col0);
    columnsData.push_back(col1);
    if (err < 0) { SG_ERROR_CHECK(hdf5GroupName) }
    break;
  case AIM::Reconstruction::Power:
    datasetName = hdf5GroupName + "/" + AIM::HDF5::Alpha;
    err = reader->readStatsDataset(m_PhaseIndex, datasetName, col0);
    datasetName = hdf5GroupName + "/" + AIM::HDF5::Exp_k;
    err = reader->readStatsDataset(m_PhaseIndex, datasetName, col1);
    datasetName = hdf5GroupName + "/" + AIM::HDF5::Beta;
    err = reader->readStatsDataset(m_PhaseIndex, datasetName, col2);
    columnsData.push_back(col0);
    columnsData.push_back(col1);
    columnsData.push_back(col2);
    colCount = AIM::HDF5::PowerLawColumnCount;
    if (err < 0) { SG_ERROR_CHECK(hdf5GroupName) }
    break;

  default:
    return -1;
  }

// Now set all the data into the various columns of data
  for (int column = 0; column < colCount; ++column)
  {
    QVector<double> colData = QVector<double>::fromStdVector(columnsData[column]);
    /* we are adding 1 to the column value because by convention Column 0 is the
     * 'bin' column.
     */
    m_TableModel->setColumnData(column + 1, colData);
  }

  m_TableView->resizeColumnsToContents();
  m_TableView->scrollToBottom();
  m_TableView->setFocus();
  updatePlotCurves();

  m_UserUpdatedData = true;
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenPlotWidget::writeDataToHDF5(H5ReconStatsWriter::Pointer writer,
                                        const std::string &hdf5GroupName)
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

  std::vector<double> col0;
  std::vector<double> col1;
  std::vector<double> col2;

  Q_ASSERT(m_PhaseIndex >= 0);

  // Create a new Table Model
  switch(m_DistributionType)
  {
  case AIM::Reconstruction::Beta:
    col0 = m_TableModel->getData(SGBetaTableModel::Alpha).toStdVector();
    col1 = m_TableModel->getData(SGBetaTableModel::Beta).toStdVector();
    err = writer->writeBetaDistribution(m_PhaseIndex, hdf5GroupName, col0, col1);
    if (err < 0) { SG_ERROR_CHECK(hdf5GroupName) }
    break;
  case AIM::Reconstruction::LogNormal:
    col0 = m_TableModel->getData(SGLogNormalTableModel::Average).toStdVector();
    col1 = m_TableModel->getData(SGLogNormalTableModel::StdDev).toStdVector();
    err = writer->writeLogNormalDistribution(m_PhaseIndex, hdf5GroupName, col0, col1);
    if (err < 0) { SG_ERROR_CHECK(hdf5GroupName) }
    break;
  case AIM::Reconstruction::Power:
    col0 = m_TableModel->getData(SGPowerLawTableModel::Alpha).toStdVector();
    col1 = m_TableModel->getData(SGPowerLawTableModel::K).toStdVector();
    col2 = m_TableModel->getData(SGPowerLawTableModel::Beta).toStdVector();
    err = writer->writePowerDistribution(m_PhaseIndex, hdf5GroupName, col0, col1, col2);
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
  QVector<double> bins;
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
  QAbstractItemDelegate* aid = m_TableModel->getItemDelegate();

  m_TableView->setItemDelegate(aid);
  setBins(bins);

  connect(m_TableModel, SIGNAL(layoutChanged()),
    this, SLOT(updatePlotCurves()));
  connect(m_TableModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
    this, SLOT(updatePlotCurves()));

  connect(aid, SIGNAL(commitData(QWidget*)),
          this, SLOT(userCommittedData(QWidget*)));

  // Update the plots
  updatePlotCurves();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenPlotWidget::userCommittedData(QWidget* w)
{
  m_UserUpdatedData = true;
  emit userEditedData();
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
  distributionTypeCombo->blockSignals(true);
  distributionTypeCombo->addItem(AIM::HDF5::BetaDistribution.c_str());
  distributionTypeCombo->addItem(AIM::HDF5::LogNormalDistribution.c_str());
  distributionTypeCombo->addItem(AIM::HDF5::PowerLawDistribution.c_str());
  distributionTypeCombo->blockSignals(false);


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
bool StatsGenPlotWidget::userUpdatedData()
{
  return m_UserUpdatedData;
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
  err = sg.GenBetaPlotData<QwtArray<double > > (alpha, beta, x, y, size);
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
  err = sg.GenLogNormalPlotData<QwtArray<double > > (avg, stdDev, x, y, size);
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
  err = sg.GenPowerLawPlotData<QwtArray<double > > (alpha, k, beta, x, y, size);
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
void StatsGenPlotWidget::setBins(QVector<double> &binNumbers)
{
#if 0

#if 1
  if (m_UserUpdatedData == true)
  {
    QString msg = "The Statistics Group '";

    switch(m_StatsType)
    {
      case AIM::Reconstruction::Grain_SizeVBoverA:
        msg.append(AIM::HDF5::Grain_SizeVBoverA_Distributions.c_str());
        break;
      case AIM::Reconstruction::Grain_SizeVCoverA:
        msg.append(AIM::HDF5::Grain_SizeVCoverA_Distributions.c_str());
        break;
      case AIM::Reconstruction::Grain_SizeVCoverB:
        msg.append(AIM::HDF5::Grain_SizeVCoverB_Distributions.c_str());
          break;
      case AIM::Reconstruction::Grain_SizeVNeighbors:
        msg.append(AIM::HDF5::Grain_SizeVNeighbors_Distributions.c_str());
        break;
      case AIM::Reconstruction::Grain_SizeVOmega3:
        msg.append(AIM::HDF5::Grain_SizeVOmega3_Distributions.c_str());
        break;
      default:
        msg.append("Unknown");
    }
    msg.append("' has user modified data. Do you want to over write the data with the new bin values?");



    int r = QMessageBox::critical(this,
                          tr("Stats Generator"),
                          msg, QMessageBox::Ok
        | QMessageBox::Cancel, QMessageBox::Ok);
    if (r != QMessageBox::Ok) {
      return;
    }
  }
#endif
  m_UserUpdatedData = false;
  m_TableModel->setBinNumbers(binNumbers);
  m_TableView->resizeColumnsToContents();
  m_TableView->scrollToBottom();
  m_TableView->setFocus();
#endif
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGAbstractTableModel* StatsGenPlotWidget::tableModel()
{
  return m_TableModel;
}
