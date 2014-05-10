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
#include "StatsGenMDFWidget.h"

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


#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_abstract_scale_draw.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_canvas.h>

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/Common/Texture.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#include "StatsGenerator/TableModels/SGMDFTableModel.h"
#include "DREAM3DLib/Common/StatsGen.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenMDFWidget::StatsGenMDFWidget(QWidget *parent) :
  QWidget(parent),
  m_PhaseIndex(-1),
  m_CrystalStructure(Ebsd::CrystalStructure::Cubic_High),
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
  //plot->canvas()->setFrameShape(QFrame::NoFrame);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::on_m_MDFUpdateBtn_clicked()
{
  // Generate the ODF Data from the current values in the ODFTableModel
  std::vector<float> odf = generateODFData();

  updateMDFPlot(odf);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::updateMDFPlot(std::vector<float> &odf)
{
  int err = 0;
  StatsGen sg;
  int size = 100000;

  // These are the output vectors
  std::vector<float> x;
  std::vector<float> y;

  // These are the input vectors
  std::vector<float> angles;
  std::vector<float> axes;
  std::vector<float> weights;

  angles = m_MDFTableModel->getData(SGMDFTableModel::Angle).toStdVector();
  weights = m_MDFTableModel->getData(SGMDFTableModel::Weight).toStdVector();
  axes = m_MDFTableModel->getData(SGMDFTableModel::Axis).toStdVector();

  if ( Ebsd::CrystalStructure::Check::IsCubic(m_CrystalStructure) )
  {
    // Allocate a new vector to hold the mdf data
    std::vector<float> mdf(5832);
    // Calculate the MDF Data using the ODF data and the rows from the MDF Table model
    Texture::calculateMDFData<std::vector<float>, CubicOps>(angles, axes, weights, odf, mdf);
    // Now generate the actual XY point data that gets plotted.
    err = sg.GenCubicMDFPlotData(mdf, x, y, size);
    if (err < 0)
    {
      return;
    }
  }
  else if ( Ebsd::CrystalStructure::Check::IsHexagonal(m_CrystalStructure) )
  {
    // Allocate a new vector to hold the mdf data
    std::vector<float> mdf(15552);
    // Calculate the MDF Data using the ODF data and the rows from the MDF Table model
    Texture::calculateMDFData<std::vector<float>, HexagonalOps>(angles, axes, weights, odf, mdf);
    // Now generate the actual XY point data that gets plotted.
    err = sg.GenHexMDFPlotData(mdf, x, y, size);
    if (err < 0)
    {
      return;
    }
  }

  QwtArray<double> xD(static_cast<int>(x.size()));
  QwtArray<double> yD(static_cast<int>(x.size()));
  for (std::vector<float>::size_type i = 0; i < x.size(); ++i)
  {
    xD[i] = static_cast<double>(x[i]);
    yD[i] = static_cast<double>(y[i]);
  }


  // This will actually plot the XY data in the Qwt plot widget
  QwtPlotCurve* curve = m_PlotCurve;
#if QWT_VERSION >= 0x060000
  curve->setSamples(xD, yD);
#else
  curve->setData(xD, yD);
#endif
  curve->setStyle(QwtPlotCurve::Lines);
  curve->attach(m_MDFPlot);
  m_MDFPlot->replot();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<float> StatsGenMDFWidget::generateODFData()
{
  std::vector<float> e1s;
  std::vector<float> e2s;
  std::vector<float> e3s;
  std::vector<float> weights;
  std::vector<float> sigmas;
  std::vector<float> odf;

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

  if ( Ebsd::CrystalStructure::Check::IsCubic(m_CrystalStructure))
  {
    Texture::calculateCubicODFData(e1s, e2s, e3s, weights, sigmas, true, odf);
  }
  else if ( Ebsd::CrystalStructure::Check::IsHexagonal(m_CrystalStructure))
  {
    Texture::calculateHexODFData(e1s, e2s, e3s, weights, sigmas, true, odf);
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
void StatsGenMDFWidget::on_loadMDFBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory;
  QString file = QFileDialog::getOpenFileName(this, tr("Open Axis ODF File"), proposedFile, tr("Text Document (*.txt)"));
  if(true == file.isEmpty())
  {
    return;
  }
  else
  {
    size_t numMisorients = 0;
    std::string filename = file.toStdString();
    std::ifstream inFile;
    inFile.open(filename.c_str());

    inFile >> numMisorients;

    float angle, weight;
    std::string axis, n1, n2, n3;
    for(size_t i = 0; i < numMisorients; i++)
    {
      inFile >> angle >> n1 >> n2 >> n3 >> weight;

      axis = std::string("<" + n1 + "," + n2 + "," + n3 + ">");

      if (!m_MDFTableModel->insertRow(m_MDFTableModel->rowCount())) return;
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
void StatsGenMDFWidget::extractStatsData(VoxelDataContainer::Pointer m, int index, StatsData* statsData, unsigned int phaseType)
{
  VectorOfFloatArray arrays;
  if(phaseType == DREAM3D::PhaseType::PrimaryPhase)
  {
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
    arrays = pp->getMDF_Weights();
  }
  if(phaseType == DREAM3D::PhaseType::PrecipitatePhase)
  {
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
    arrays = pp->getMDF_Weights();
  }
  if(phaseType == DREAM3D::PhaseType::TransformationPhase)
  {
    TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
    arrays = tp->getMDF_Weights();
  }
  if (arrays.size() > 0 ) {
    QVector<float> angle(static_cast<int>(arrays[0]->GetNumberOfTuples()));
    ::memcpy( &(angle.front()), arrays[0]->GetVoidPointer(0), sizeof(float)*angle.size() );

    QVector<float> weights(static_cast<int>(arrays[0]->GetNumberOfTuples()));
    ::memcpy( &(weights.front()), arrays[0]->GetVoidPointer(0), sizeof(float)*weights.size() );

    QVector<float> axis(static_cast<int>(arrays[0]->GetSize())); // This one is 3xn in size
    ::memcpy( &(axis.front()), arrays[0]->GetVoidPointer(0), sizeof(float)*axis.size() );

    if (angle.size() > 0)
    {
      // Load the data into the table model
      m_MDFTableModel->setTableData(angle, axis, weights);
    }
  }
  on_m_MDFUpdateBtn_clicked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenMDFWidget::getMisorientationData(StatsData* statsData, unsigned int phaseType)
{
  int retErr = 0;
  std::vector<float> x;
  std::vector<float> y;

  std::vector<float> angles;
  std::vector<float> axes;
  std::vector<float> weights;

  angles = m_MDFTableModel->getData(SGMDFTableModel::Angle).toStdVector();
  weights = m_MDFTableModel->getData(SGMDFTableModel::Weight).toStdVector();
  axes = m_MDFTableModel->getData(SGMDFTableModel::Axis).toStdVector();

  // Generate the ODF Data from the current values in the ODFTableModel
  std::vector<float> odf = generateODFData();
  std::vector<float> mdf;

  unsigned long long int nElements = 0;

  if ( Ebsd::CrystalStructure::Check::IsCubic(m_CrystalStructure)) {
    Texture::calculateMDFData<std::vector<float>, CubicOps>(angles, axes, weights, odf, mdf);
    nElements = 18 * 18 * 18;
  }
  else if ( Ebsd::CrystalStructure::Check::IsHexagonal(m_CrystalStructure)) {
    Texture::calculateMDFData<std::vector<float>, HexagonalOps>(angles, axes, weights, odf, mdf);
    nElements = 36 * 36 * 12;
  }
  if (mdf.size() > 0)
  {
    FloatArrayType::Pointer p = FloatArrayType::FromStdVector(mdf, DREAM3D::HDF5::MisorientationBins);
    if(phaseType == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
      pp->setMisorientationBins(p);
    }
    if(phaseType == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
      pp->setMisorientationBins(p);
    }
    if(phaseType == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
      tp->setMisorientationBins(p);
    }

    if(angles.size() > 0)
    {
      FloatArrayType::Pointer anglesArray = FloatArrayType::FromStdVector(angles, DREAM3D::HDF5::Angle);

      FloatArrayType::Pointer axisArray = FloatArrayType::FromStdVector(axes, DREAM3D::HDF5::Axis);
      axisArray->SetNumberOfComponents(3);

      FloatArrayType::Pointer weightArray = FloatArrayType::FromStdVector(weights, DREAM3D::HDF5::Weight);

      VectorOfFloatArray mdfWeights;
      mdfWeights.push_back(anglesArray);
      mdfWeights.push_back(axisArray);
      mdfWeights.push_back(weightArray);
      if(phaseType == DREAM3D::PhaseType::PrimaryPhase)
      {
        PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
        pp->setMDF_Weights(mdfWeights);
      }
      if(phaseType == DREAM3D::PhaseType::PrecipitatePhase)
      {
        PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
        pp->setMDF_Weights(mdfWeights);
      }
      if(phaseType == DREAM3D::PhaseType::TransformationPhase)
      {
        TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
        tp->setMDF_Weights(mdfWeights);
      }
    }

  }

  return retErr;
}


