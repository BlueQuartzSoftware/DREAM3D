/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "StatsGenMDFWidget.h"

//-- C++ Includes
#include <iostream>

//-- Qt Includes
#include <QtWidgets/QAbstractItemDelegate>
#include <QtConcurrent/QtConcurrentMap>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>


#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_abstract_scale_draw.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <qwt_series_data.h>
#include <qwt_interval.h>
#include <qwt_point_3d.h>
#include <qwt_compat.h>


#include "EbsdLib/EbsdConstants.h"

#include "SIMPLib/Math/SIMPLibMath.h"

#include "OrientationLib/Texture/Texture.hpp"
#include "OrientationLib/Texture/StatsGen.hpp"
#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"
#include "OrientationLib/SpaceGroupOps/CubicOps.h"
#include "OrientationLib/SpaceGroupOps/HexagonalOps.h"
#include "OrientationLib/SpaceGroupOps/OrthoRhombicOps.h"

#include "StatsGenerator/TableModels/SGMDFTableModel.h"


// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_StatsGenMDFWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenMDFWidget::StatsGenMDFWidget(QWidget* parent) :
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::on_m_MDFUpdateBtn_clicked()
{
  // Generate the ODF Data from the current values in the ODFTableModel
  QVector<float> odf = generateODFData();

  updateMDFPlot(odf);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::updateMDFPlot(QVector<float>& odf)
{
  int err = 0;

  int size = 100000;

  // These are the input vectors
  QVector<float> angles;
  QVector<float> axes;
  QVector<float> weights;

  angles = m_MDFTableModel->getData(SGMDFTableModel::Angle);
  weights = m_MDFTableModel->getData(SGMDFTableModel::Weight);
  axes = m_MDFTableModel->getData(SGMDFTableModel::Axis);


  // These are the output vectors
  QVector<float> x;
  QVector<float> y;
  if ( Ebsd::CrystalStructure::Cubic_High == m_CrystalStructure )
  {
    // Allocate a new vector to hold the mdf data
    QVector<float> mdf(CubicOps::k_MdfSize);
    // Calculate the MDF Data using the ODF data and the rows from the MDF Table model
    Texture::CalculateMDFData<float, CubicOps>(angles.data(), axes.data(), weights.data(), odf.data(), mdf.data(), static_cast<size_t>(angles.size()));
    // Now generate the actual XY point data that gets plotted.
    int npoints = 13;
    x.resize(npoints);
    y.resize(npoints);
    err = StatsGen::GenCubicMDFPlotData(mdf.data(), x.data(), y.data(), npoints, size);
    if (err < 0)
    {
      return;
    }
  }
  else if ( Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructure )
  {
    // Allocate a new vector to hold the mdf data
    QVector<float> mdf(HexagonalOps::k_MdfSize);
    // Calculate the MDF Data using the ODF data and the rows from the MDF Table model
    Texture::CalculateMDFData<float, HexagonalOps>(angles.data(), axes.data(), weights.data(), odf.data(), mdf.data(), static_cast<size_t>(angles.size()));
    // Now generate the actual XY point data that gets plotted.
    int npoints = 20;
    x.resize(npoints);
    y.resize(npoints);
    err = StatsGen::GenHexMDFPlotData(mdf.data(), x.data(), y.data(), npoints, size);
    if (err < 0)
    {
      return;
    }
  }

  QwtArray<double> xD(static_cast<int>(x.size()));
  QwtArray<double> yD(static_cast<int>(x.size()));
  for (qint32 i = 0; i < x.size(); ++i)
  {
    xD[i] = static_cast<double>(x.at(i));
    yD[i] = static_cast<double>(y.at(i));
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
QVector<float> StatsGenMDFWidget::generateODFData()
{
  QVector<float> e1s;
  QVector<float> e2s;
  QVector<float> e3s;
  QVector<float> weights;
  QVector<float> sigmas;
  QVector<float> odf;

  // Initialize xMax and yMax....
  e1s = m_ODFTableModel->getData(SGODFTableModel::Euler1);
  e2s = m_ODFTableModel->getData(SGODFTableModel::Euler2);
  e3s = m_ODFTableModel->getData(SGODFTableModel::Euler3);
  weights = m_ODFTableModel->getData(SGODFTableModel::Weight);
  sigmas = m_ODFTableModel->getData(SGODFTableModel::Sigma);

  for(qint32 i = 0; i < e1s.size(); i++)
  {
    e1s[i] = e1s[i] * static_cast<float>(SIMPLib::Constants::k_PiOver180);
    e2s[i] = e2s[i] * static_cast<float>(SIMPLib::Constants::k_PiOver180);
    e3s[i] = e3s[i] * static_cast<float>(SIMPLib::Constants::k_PiOver180);
  }
  size_t numEntries = static_cast<size_t>(e1s.size());

  if ( Ebsd::CrystalStructure::Cubic_High == m_CrystalStructure)
  {

    odf.resize(CubicOps::k_OdfSize);
    Texture::CalculateCubicODFData(e1s.data(), e2s.data(), e3s.data(),
                                   weights.data(), sigmas.data(), true,
                                   odf.data(), numEntries);
  }
  else if ( Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructure)
  {
    odf.resize(HexagonalOps::k_OdfSize);
    Texture::CalculateHexODFData(e1s.data(), e2s.data(), e3s.data(),
                                 weights.data(), sigmas.data(), true,
                                 odf.data(), numEntries);
  }
  return odf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenMDFWidget::on_addMDFRowBtn_clicked()
{
  if (!m_MDFTableModel->insertRow(m_MDFTableModel->rowCount())) { return; }
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
  QItemSelectionModel* selectionModel = m_MDFTableView->selectionModel();
  if (!selectionModel->hasSelection()) { return; }
  QModelIndex index = selectionModel->currentIndex();
  if (!index.isValid()) { return; }
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
  QString file = QFileDialog::getOpenFileName(this, tr("Open MDF File"), proposedFile, tr("Text Document (*.txt)"));
  if(true == file.isEmpty())
  {
    return;
  }
  else
  {
    size_t numMisorients = 0;
    QString filename = file;
    std::ifstream inFile;
    inFile.open(filename.toLatin1().data());

    inFile >> numMisorients;

    float angle, weight;
    std::string n1, n2, n3;
    for(size_t i = 0; i < numMisorients; i++)
    {
      inFile >> angle >> n1 >> n2 >> n3 >> weight;

      QString axis = QString("<" + QString::fromStdString(n1) + "," + QString::fromStdString(n2) + "," + QString::fromStdString(n3) + ">");

      if (!m_MDFTableModel->insertRow(m_MDFTableModel->rowCount())) { return; }
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
void StatsGenMDFWidget::extractStatsData(int index, StatsData* statsData, unsigned int phaseType)
{
  Q_UNUSED(index)
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
  if (arrays.size() > 0 )
  {
    QVector<float> angle(static_cast<int>(arrays[0]->getNumberOfTuples()));
    ::memcpy( &(angle.front()), arrays[0]->getVoidPointer(0), sizeof(float)*angle.size() );

    QVector<float> weights(static_cast<int>(arrays[0]->getNumberOfTuples()));
    ::memcpy( &(weights.front()), arrays[0]->getVoidPointer(0), sizeof(float)*weights.size() );

    QVector<float> axis(static_cast<int>(arrays[0]->getSize())); // This one is 3xn in size
    ::memcpy( &(axis.front()), arrays[0]->getVoidPointer(0), sizeof(float)*axis.size() );

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
  QVector<float> x;
  QVector<float> y;

  QVector<float> angles;
  QVector<float> axes;
  QVector<float> weights;

  angles = m_MDFTableModel->getData(SGMDFTableModel::Angle);
  weights = m_MDFTableModel->getData(SGMDFTableModel::Weight);
  axes = m_MDFTableModel->getData(SGMDFTableModel::Axis);

  // Generate the ODF Data from the current values in the ODFTableModel
  QVector<float> odf = generateODFData();
  QVector<float> mdf;

  //unsigned long long int nElements = 0;

  if ( Ebsd::CrystalStructure::Cubic_High == m_CrystalStructure)
  {
    mdf.resize(CubicOps::k_MdfSize);
    Texture::CalculateMDFData<float, CubicOps>(angles.data(), axes.data(), weights.data(), odf.data(), mdf.data(), angles.size());
    //nElements = 18 * 18 * 18;
  }
  else if ( Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructure)
  {
    mdf.resize(HexagonalOps::k_MdfSize);
    Texture::CalculateMDFData<float, HexagonalOps>(angles.data(), axes.data(), weights.data(), odf.data(), mdf.data(), angles.size());
    //nElements = 36 * 36 * 12;
  }
  if (mdf.size() > 0)
  {
    FloatArrayType::Pointer p = FloatArrayType::FromPointer(mdf.data(), mdf.size(), DREAM3D::StringConstants::MisorientationBins);
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
      FloatArrayType::Pointer anglesArray = FloatArrayType::FromPointer(angles.data(), angles.size(), DREAM3D::StringConstants::Angle);
      FloatArrayType::Pointer axisArray = FloatArrayType::FromPointer(axes.data(), axes.size(), DREAM3D::StringConstants::Axis);
      FloatArrayType::Pointer weightArray = FloatArrayType::FromPointer(weights.data(), weights.size(), DREAM3D::StringConstants::Weight);

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


