/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
#include "StatsGenMDFWidget.h"
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

#include "AIM/Common/Texture.h"
#include "AIM/Common/OrientationMath.h"
#include "AIM/Common/OrientationOps/CubicOps.h"
#include "AIM/Common/OrientationOps/HexagonalOps.h"
#include "AIM/Common/OrientationOps/OrthoRhombicOps.h"

#include "StatsGenerator/TableModels/SGMDFTableModel.h"
#include "StatsGen.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenMDFWidget::StatsGenMDFWidget(QWidget *parent) :
QWidget(parent),
m_PhaseIndex(-1),
m_CrystalStructure(AIM::Reconstruction::Cubic),
m_MDFTableModel(NULL)
{
  this->setupUi(this);
  this->setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenMDFWidget::~StatsGenMDFWidget()
{
  if (NULL != m_MDFTableModel)
  {
    m_MDFTableModel->deleteLater();
  }
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
  QAbstractItemDelegate* aid = m_MDFTableModel->getItemDelegate();
  m_MDFTableView->setItemDelegate(aid);
  m_PlotCurve = new QwtPlotCurve;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot)
{
  plot->setAxisTitle(QwtPlot::xBottom, xAxisName);
  plot->setAxisTitle(QwtPlot::yLeft, yAxisName);
  //plot->setCanvasBackground(QColor(Qt::white));
  plot->canvas()->setFrameShape(QFrame::NoFrame);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::on_m_MDFUpdateBtn_clicked()
{
  // Generate the ODF Data from the current values in the ODFTableModel
  QwtArray<float> odf = generateODFData();

  updateMDFPlot(odf);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::updateMDFPlot(QwtArray<float> odf)
{
  int err = 0;
  StatsGen sg;
  int size = 1000;

  // These are the output vectors
  QwtArray<float> x;
  QwtArray<float> y;

  // These are the input vectors
  QwtArray<float> angles;
  QwtArray<float> axes;
  QwtArray<float> weights;

  angles = m_MDFTableModel->getData(SGMDFTableModel::Angle);
  weights = m_MDFTableModel->getData(SGMDFTableModel::Weight);
  axes = m_MDFTableModel->getData(SGMDFTableModel::Axis);

  if (m_CrystalStructure == AIM::Reconstruction::Cubic)
  {
    // Allocate a new vector to hold the mdf data
    QwtArray<float> mdf(5832);
    // Calculate the MDF Data using the ODF data and the rows from the MDF Table model
    Texture::calculateMDFData<QwtArray<float>, CubicOps>(angles, axes, weights, odf, mdf);
    // Now generate the actual XY point data that gets plotted.
    err = sg.GenCubicMDFPlotData(mdf, x, y, size);
  }
  else if (m_CrystalStructure == AIM::Reconstruction::Hexagonal)
  {
    // Allocate a new vector to hold the mdf data
    QwtArray<float> mdf(15552);
    // Calculate the MDF Data using the ODF data and the rows from the MDF Table model
    Texture::calculateMDFData<QwtArray<float>, HexagonalOps>(angles, axes, weights, odf, mdf);
    // Now generate the actual XY point data that gets plotted.
    err = sg.GenHexMDFPlotData(mdf, x, y, size);
  }

  QwtArray<double> xD(x.size());
  QwtArray<double> yD(x.size());
  for (int i = 0; i < x.size(); ++i)
  {
    xD[i] = static_cast<double>(x[i]);
    yD[i] = static_cast<double>(y[i]);
  }


  // This will actually plot the XY data in the Qwt plot widget
  QwtPlotCurve* curve = m_PlotCurve;
  curve->setData(xD, yD);
  curve->setStyle(QwtPlotCurve::Lines);
  curve->attach(m_MDFPlot);
  m_MDFPlot->replot();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QwtArray<float> StatsGenMDFWidget::generateODFData()
{
  float totalWeight = 0.0;

  QwtArray<float> e1s;
  QwtArray<float> e2s;
  QwtArray<float> e3s;
  QwtArray<float> weights;
  QwtArray<float> sigmas;
  QwtArray<float> odf;

  // Initialize xMax and yMax....
  e1s = m_ODFTableModel->getData(SGODFTableModel::Euler1);
  e2s = m_ODFTableModel->getData(SGODFTableModel::Euler2);
  e3s = m_ODFTableModel->getData(SGODFTableModel::Euler3);
  weights = m_ODFTableModel->getData(SGODFTableModel::Weight);
  sigmas = m_ODFTableModel->getData(SGODFTableModel::Sigma);

  if (m_CrystalStructure == AIM::Reconstruction::Cubic)
  {
    Texture::calculateCubicODFData(e1s, e2s, e3s, weights, sigmas, true, odf, totalWeight);
  }
  else if (m_CrystalStructure == AIM::Reconstruction::Hexagonal)
  {
    Texture::calculateHexODFData(e1s, e2s, e3s, weights, sigmas, true, odf, totalWeight);
  }
  return odf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::on_addMDFRowBtn_clicked()
{
  if (!m_MDFTableModel->insertRow(m_MDFTableModel->rowCount())) return;
  m_MDFTableView->resizeColumnsToContents();
  m_MDFTableView->scrollToBottom();
  m_MDFTableView->setFocus();
  QModelIndex index = m_MDFTableModel->index(m_MDFTableModel->rowCount() - 1, 0);
  m_MDFTableView->setCurrentIndex(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::on_deleteMDFRowBtn_clicked()
{
  QItemSelectionModel *selectionModel = m_MDFTableView->selectionModel();
  if (!selectionModel->hasSelection()) return;
  QModelIndex index = selectionModel->currentIndex();
  if (!index.isValid()) return;
  m_MDFTableModel->removeRow(index.row(), index.parent());
  if (m_MDFTableModel->rowCount() > 0)
  {
    m_MDFTableView->resizeColumnsToContents();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenMDFWidget::readDataFromHDF5(H5ReconStatsReader::Pointer reader,
                                         QVector<float>  &bins,
                                         const std::string &hdf5GroupName)
{
  int err = -1;

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenMDFWidget::writeDataToHDF5(H5ReconStatsWriter::Pointer writer)
{
  int err = 0;

  QwtArray<float> x;
  QwtArray<float> y;

  QwtArray<float> angles;
  QwtArray<float> axes;
  QwtArray<float> weights;

  angles = m_MDFTableModel->getData(SGMDFTableModel::Angle);
  weights = m_MDFTableModel->getData(SGMDFTableModel::Weight);
  axes = m_MDFTableModel->getData(SGMDFTableModel::Axis);

  // Generate the ODF Data from the current values in the ODFTableModel
  QwtArray<float> odf = generateODFData();
  QwtArray<float> mdf;

  if (m_CrystalStructure == AIM::Reconstruction::Cubic) {
	  Texture::calculateMDFData<QwtArray<float>, CubicOps>(angles, axes, weights, odf, mdf);
  }
  else if (m_CrystalStructure == AIM::Reconstruction::Hexagonal) {
	  Texture::calculateMDFData<QwtArray<float>, HexagonalOps>(angles, axes, weights, odf, mdf);
  }
  if (mdf.size() > 0)
  {
    float* mdfPtr = &(mdf.front());
    err = -1;
    if (mdfPtr != NULL)
    {
      err = writer->writeMisorientationBinsData(m_PhaseIndex, m_CrystalStructure, mdfPtr);
    }
  }

  return err;
}


