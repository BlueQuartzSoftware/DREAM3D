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
#include "StatsGenODFWidget.h"

#if QWT_VERSION >= 0x060000
#include "backwards.h"
#endif

//-- C++ Includes
#include <iostream>

//-- Qt Includes
#include <QtCore/QVector>
#include <QtCore/QtConcurrentMap>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QSettings>

#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QAbstractItemDelegate>

#include "DREAM3DLib/Common/Texture.hpp"
#include "DREAM3DLib/Common/StatsGen.hpp"
#include "DREAM3DLib/Utilities/AngleFileLoader.h"

#include "StatsGenerator/TableModels/SGODFTableModel.h"
#include "StatsGenerator/StatsGenMDFWidget.h"
#include "StatsGenerator/TextureDialog.h"

#include "DREAM3DLib/Utilities/PoleFigureUtilities.h"
#include "QtSupport/PoleFigureImageUtilities.h"


#define SHOW_POLE_FIGURES 1
#define COLOR_POLE_FIGURES 1


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenODFWidget::StatsGenODFWidget(QWidget *parent) :
  QWidget(parent),
  m_EnableAxisDecorations(false),
  m_Initializing(true),
  m_PhaseIndex(-1),
  m_CrystalStructure(Ebsd::CrystalStructure::Cubic_High),
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
StatsGenODFWidget::~StatsGenODFWidget()
{
  if (NULL != m_ODFTableModel)
  {
    m_ODFTableModel->deleteLater();
  }
  m_PoleFigureFuture->cancel();
  m_PoleFigureFuture->waitForFinished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::extractStatsData(VolumeDataContainer::Pointer m, int index, StatsData* statsData, unsigned int phaseType)
{
  VectorOfFloatArray arrays;
  if(phaseType == DREAM3D::PhaseType::PrimaryPhase)
  {
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
    arrays = pp->getODF_Weights();
  }
  if(phaseType == DREAM3D::PhaseType::PrecipitatePhase)
  {
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
    arrays = pp->getODF_Weights();
  }
  if(phaseType == DREAM3D::PhaseType::TransformationPhase)
  {
    TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
    arrays = tp->getODF_Weights();
  }
  if (arrays.size() > 0)
  {
    QVector<float> e1(static_cast<int>(arrays[0]->GetNumberOfTuples()));
    ::memcpy( &(e1.front()), arrays[0]->GetVoidPointer(0), sizeof(float)*e1.size() );

    QVector<float> e2(static_cast<int>(arrays[1]->GetNumberOfTuples()));
    ::memcpy( &(e2.front()), arrays[1]->GetVoidPointer(0), sizeof(float)*e2.size() );

    QVector<float> e3(static_cast<int>(arrays[2]->GetNumberOfTuples()));
    ::memcpy( &(e3.front()), arrays[2]->GetVoidPointer(0), sizeof(float)*e3.size() );

    QVector<float> weights(static_cast<int>(arrays[3]->GetNumberOfTuples()));
    ::memcpy( &(weights.front()), arrays[3]->GetVoidPointer(0), sizeof(float)*weights.size() );

    QVector<float> sigmas(static_cast<int>(arrays[4]->GetNumberOfTuples()));
    ::memcpy( &(sigmas.front()), arrays[4]->GetVoidPointer(0), sizeof(float)*sigmas.size() );

    // Convert from Radians to Degrees for the Euler Angles
    for(int i = 0; i < e1.size(); ++i)
    {
      e1[i] = e1[i] * 180.0f/M_PI;
      e2[i] = e2[i] * 180.0f/M_PI;
      e3[i] = e3[i] * 180.0f/M_PI;
    }

    if(e1.size() > 0)
    {
      // Load the data into the table model
      m_ODFTableModel->setTableData(e1, e2, e3, weights, sigmas);
    }
  }
  // Write the MDF Data if we have that functionality enabled
  if (m_MDFWidget != NULL)
  {
    m_MDFWidget->extractStatsData(m, index, statsData, phaseType);
  }
  updatePlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenODFWidget::getOrientationData(StatsData* statsData, unsigned int phaseType)
{
  int retErr = 0;

  QVector<float> e1s;
  QVector<float> e2s;
  QVector<float> e3s;
  QVector<float> weights;
  QVector<float> sigmas;
  QVector<float> odf;

  // Initialize xMax and yMax....
  e1s = m_ODFTableModel->getData(SGODFTableModel::Euler1).toStdVector();
  e2s = m_ODFTableModel->getData(SGODFTableModel::Euler2).toStdVector();
  e3s = m_ODFTableModel->getData(SGODFTableModel::Euler3).toStdVector();
  weights = m_ODFTableModel->getData(SGODFTableModel::Weight).toStdVector();
  sigmas = m_ODFTableModel->getData(SGODFTableModel::Sigma).toStdVector();

  for (QVector<float>::size_type i = 0; i < e1s.size(); i++)
  {
    e1s[i] = e1s[i] * M_PI / 180.0;
    e2s[i] = e2s[i] * M_PI / 180.0;
    e3s[i] = e3s[i] * M_PI / 180.0;
  }
  size_t numEntries = e1s.size();

  if ( Ebsd::CrystalStructure::Cubic_High == m_CrystalStructure)
  {
    odf.resize(CubicOps::k_OdfSize);
    Texture::CalculateCubicODFData(&(e1s.front()), &(e2s.front()), &(e3s.front()),
                                   &(weights.front()), &(sigmas.front()), true,
                                   &(odf.front()), numEntries);
  }
  else if ( Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructure)
  {
    odf.resize(HexagonalOps::k_OdfSize);
    Texture::CalculateHexODFData(&(e1s.front()), &(e2s.front()), &(e3s.front()),
                                 &(weights.front()), &(sigmas.front()), true,
                                 &(odf.front()), numEntries);
  }
  if (odf.size() > 0)
  {
    FloatArrayType::Pointer p = FloatArrayType::FromStdVector(odf, DREAM3D::HDF5::ODF);
    if(phaseType == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
      pp->setODF(p);
    }
    if(phaseType == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
      pp->setODF(p);
    }
    if(phaseType == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
      tp->setODF(p);
    }
    if (e1s.size() > 0)
    {
      FloatArrayType::Pointer euler1 = FloatArrayType::FromStdVector(e1s, DREAM3D::HDF5::Euler1);
      FloatArrayType::Pointer euler2 = FloatArrayType::FromStdVector(e2s, DREAM3D::HDF5::Euler2);
      FloatArrayType::Pointer euler3 = FloatArrayType::FromStdVector(e3s, DREAM3D::HDF5::Euler3);
      FloatArrayType::Pointer sigma = FloatArrayType::FromStdVector(sigmas, DREAM3D::HDF5::Sigma);
      FloatArrayType::Pointer weight = FloatArrayType::FromStdVector(weights, DREAM3D::HDF5::Weight);

      VectorOfFloatArray odfWeights;
      odfWeights.push_back(euler1);
      odfWeights.push_back(euler2);
      odfWeights.push_back(euler3);
      odfWeights.push_back(sigma);
      odfWeights.push_back(weight);
      if(phaseType == DREAM3D::PhaseType::PrimaryPhase)
      {
        PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
        pp->setODF_Weights(odfWeights);
      }
      if(phaseType == DREAM3D::PhaseType::PrecipitatePhase)
      {
        PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
        pp->setODF_Weights(odfWeights);
      }
      if(phaseType == DREAM3D::PhaseType::TransformationPhase)
      {
        TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
        tp->setODF_Weights(odfWeights);
      }
    }
  }
  // Write the MDF Data if we have that functionality enabled
  if (m_MDFWidget != NULL)
  {
    m_MDFWidget->getMisorientationData(statsData, phaseType);
  }
  return retErr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::enableMDFTab(bool b)
{
  if (NULL != m_MDFWidget)
  {
    m_MDFWidget->deleteLater();
  }
  m_MDFWidget = new StatsGenMDFWidget();
  m_MDFWidget->setODFTableModel(m_ODFTableModel);
  tabWidget->addTab(m_MDFWidget, QString("MDF"));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::setCrystalStructure(unsigned int value)
{
  if (m_CrystalStructure != value)
  {
    this->m_CrystalStructure = value;
    if (m_MDFWidget != NULL)
    {
      m_MDFWidget->setCrystalStructure(m_CrystalStructure);
    }
    m_ODFTableModel->setCrystalStructure(m_CrystalStructure);
    QAbstractItemDelegate* idelegate = m_ODFTableModel->getItemDelegate();
    m_ODFTableView->setItemDelegate(idelegate);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int StatsGenODFWidget::getCrystalStructure()
{
  return m_CrystalStructure;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::setPhaseIndex(int value)
{
  this->m_PhaseIndex = value;
  if (m_MDFWidget != NULL)
  {
    m_MDFWidget->setPhaseIndex(m_PhaseIndex);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenODFWidget::getPhaseIndex()
{
  return m_PhaseIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::setPlotTabTitles(QString t1, QString t2, QString t3)
{
  //  tabWidget->setTabText(1, t1);
  //  tabWidget->setTabText(2, t2);
  //  tabWidget->setTabText(3, t3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::setupGui()
{
  // Setup the TableView and Table Models
  QHeaderView* headerView = new QHeaderView(Qt::Horizontal, m_ODFTableView);
  headerView->setResizeMode(QHeaderView::Interactive);
  m_ODFTableView->setHorizontalHeader(headerView);
  headerView->show();

  m_ODFTableModel = new SGODFTableModel;
  m_ODFTableModel->setCrystalStructure(m_CrystalStructure);
  m_ODFTableModel->setInitialValues();
  m_ODFTableView->setModel(m_ODFTableModel);
  QAbstractItemDelegate* idelegate = m_ODFTableModel->getItemDelegate();
  m_ODFTableView->setItemDelegate(idelegate);

  addODFTextureBtn->setToolTip("Add ODF Weight");
  deleteODFTextureBtn->setToolTip("Delete ODF Weight");

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
void StatsGenODFWidget::initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot)
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
void StatsGenODFWidget::drawODFPlotGrid(QwtPlot* plot)
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
    circleX[i] = 1.0 - (i * inc);
    circleX[450+i] = -1.0 + (i * inc);

    circleY[i] = sqrt(1.0-(circleX[i]*circleX[i]));
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
void StatsGenODFWidget::updatePlots()
{
  on_m_CalculateODFBtn_clicked();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::showPoleFigure(int imageIndex)
{
  // labels[num]->setPixmap(QPixmap::fromImage(imageScaling->resultAt(num)));
  switch(imageIndex)
  {
    case 0:
      //  m_PoleFigureFuture->resultAt(imageIndex).save("/tmp/ODF_PoleFigure_001.tif");
      m_001PF->setPixmap(QPixmap::fromImage(m_PoleFigureFuture->resultAt(imageIndex)));
      break;
    case 1:
      //  m_PoleFigureFuture->resultAt(imageIndex).save("/tmp/ODF_PoleFigure_011.tif");
      m_011PF->setPixmap(QPixmap::fromImage(m_PoleFigureFuture->resultAt(imageIndex)));
      break;
    case 2:
      //  m_PoleFigureFuture->resultAt(imageIndex).save("/tmp/ODF_PoleFigure_111.tif");
      m_111PF->setPixmap(QPixmap::fromImage(m_PoleFigureFuture->resultAt(imageIndex)));
      break;
    default:
      break;
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::poleFigureGenerationComplete()
{
  //  qDebug() << "ODF Pole Figure generation complete" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage generateODFPoleFigure(const PoleFigureData &data)
{
//  PoleFigureImageUtilities colorPoleFigure;
//#if COLOR_POLE_FIGURES
//  return colorPoleFigure.generateColorPoleFigureImage(data);
//#else
//  return colorPoleFigure.generatePoleFigureImage(data);
//#endif
return QImage();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_m_CalculateODFBtn_clicked()
{
  int err = 0;

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

  // Convert from Degrees to Radians
  for(int i=0;i<e1s.size();i++)
  {
    e1s[i] = e1s[i]*M_PI/180.0;
    e2s[i] = e2s[i]*M_PI/180.0;
    e3s[i] = e3s[i]*M_PI/180.0;
  }
  size_t numEntries = e1s.size();

  int imageSize = pfImageSize->value();
  int lamberSize = pfLambertSize->value();
  int numColors = 16;
  int npoints = pfSamplePoints->value();
  FloatArrayType::Pointer eulers = FloatArrayType::CreateArray(npoints, 3, "Eulers");

  if ( Ebsd::CrystalStructure::Cubic_High == m_CrystalStructure)
  {
    // We now need to resize all the arrays here to make sure they are all allocated
    odf.resize(CubicOps::k_OdfSize);
    Texture::CalculateCubicODFData(e1s.data(), e2s.data(), e3s.data(),
                                   weights.data(), sigmas.data(), true,
                                   odf.data(), numEntries);

    err = StatsGen::GenCubicODFPlotData(odf.data(), eulers->GetPointer(0), npoints);

    CubicOps ops;
    PoleFigureConfiguration_t config;
    config.eulers = eulers.get();
    config.imageDim = imageSize;
    config.lambertDim = lamberSize;
    config.numColors = numColors;

    QVector<UInt8ArrayType::Pointer> figures = ops.generatePoleFigure(config);
    {
      // Now create a QImage that is mirrored vertically and has the Axis overlay applied to it
      QImage image = PoleFigureImageUtilities::CreateQImageFromRgbaArray(figures[0].get(), imageSize, true);
      m_001PF->setPixmap(QPixmap::fromImage(image));
    }
    {
      // Now create a QImage that is mirrored vertically and has the Axis overlay applied to it
      QImage image = PoleFigureImageUtilities::CreateQImageFromRgbaArray(figures[1].get(), imageSize, true);
      m_011PF->setPixmap(QPixmap::fromImage(image));
    }
    {
      // Now create a QImage that is mirrored vertically and has the Axis overlay applied to it
      QImage image = PoleFigureImageUtilities::CreateQImageFromRgbaArray(figures[2].get(), imageSize, true);
      m_111PF->setPixmap(QPixmap::fromImage(image));
    }

  }
  else if ( Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructure)
  {
    // We now need to resize all the arrays here to make sure they are all allocated
    odf.resize(HexagonalOps::k_OdfSize);
    Texture::CalculateHexODFData(e1s.data(), e2s.data(), e3s.data(),
                                 weights.data(), sigmas.data(), true,
                                 odf.data(), numEntries);

    err = StatsGen::GenHexODFPlotData(odf.data(), eulers->GetPointer(0), npoints);

    HexagonalOps ops;
    PoleFigureConfiguration_t config;
    config.eulers = eulers.get();
    config.imageDim = imageSize;
    config.lambertDim = lamberSize;
    config.numColors = numColors;

    QVector<UInt8ArrayType::Pointer> figures = ops.generatePoleFigure(config);
    {
      // Now create a QImage that is mirrored vertically and has the Axis overlay applied to it
      QImage image = PoleFigureImageUtilities::CreateQImageFromRgbaArray(figures[0].get(), imageSize, true);
      m_001PF->setPixmap(QPixmap::fromImage(image));
    }
    {
      // Now create a QImage that is mirrored vertically and has the Axis overlay applied to it
      QImage image = PoleFigureImageUtilities::CreateQImageFromRgbaArray(figures[1].get(), imageSize, true);
      m_011PF->setPixmap(QPixmap::fromImage(image));
    }
    {
      // Now create a QImage that is mirrored vertically and has the Axis overlay applied to it
      QImage image = PoleFigureImageUtilities::CreateQImageFromRgbaArray(figures[2].get(), imageSize, true);
      m_111PF->setPixmap(QPixmap::fromImage(image));
    }
  }
  else if ( Ebsd::CrystalStructure::OrthoRhombic == m_CrystalStructure)
  {
//    // We now need to resize all the arrays here to make sure they are all allocated
    odf.resize(OrthoRhombicOps::k_OdfSize);
    Texture::CalculateOrthoRhombicODFData(e1s.data(), e2s.data(), e3s.data(),
                                 weights.data(), sigmas.data(), true,
                                 odf.data(), numEntries);

    err = StatsGen::GenOrthoRhombicODFPlotData(odf.data(), eulers->GetPointer(0), npoints);

    OrthoRhombicOps ops;
    PoleFigureConfiguration_t config;
    config.eulers = eulers.get();
    config.imageDim = imageSize;
    config.lambertDim = lamberSize;
    config.numColors = numColors;

    QVector<UInt8ArrayType::Pointer> figures = ops.generatePoleFigure(config);
    {
      // Now create a QImage that is mirrored vertically and has the Axis overlay applied to it
      QImage image = PoleFigureImageUtilities::CreateQImageFromRgbaArray(figures[0].get(), imageSize, true);
      m_001PF->setPixmap(QPixmap::fromImage(image));
    }
    {
      // Now create a QImage that is mirrored vertically and has the Axis overlay applied to it
      QImage image = PoleFigureImageUtilities::CreateQImageFromRgbaArray(figures[0].get(), imageSize, true);
      m_011PF->setPixmap(QPixmap::fromImage(image));
    }
    {
      // Now create a QImage that is mirrored vertically and has the Axis overlay applied to it
      QImage image = PoleFigureImageUtilities::CreateQImageFromRgbaArray(figures[0].get(), imageSize, true);
      m_111PF->setPixmap(QPixmap::fromImage(image));
    }
  }
  if (err == 1)
  {
    //TODO: Present Error Message
    return;
  }

  //// This is multi-threaded on appropriate hardware.
  //qint32 kRad[2] = {4, 4};
  //qint32 pfSize[2] = {226, 226};
  //QVector<PoleFigureData> data;

  //switch(this->m_CrystalStructure)
  //{
  //  case Ebsd::CrystalStructure::Cubic_High:
  //    data.push_back(PoleFigureData(x001, y001, QString("<001>"), kRad, pfSize));
  //    data.push_back(PoleFigureData(x011, y011, QString("<011>"), kRad, pfSize));
  //    data.push_back(PoleFigureData(x111, y111, QString("<111>"), kRad, pfSize));
  //    break;
  //  case Ebsd::CrystalStructure::Hexagonal_High:
  //    data.push_back(PoleFigureData(x001, y001, QString("<0001>"), kRad, pfSize));
  //    data.push_back(PoleFigureData(x011, y011, QString("<11-20>"), kRad, pfSize));
  //    data.push_back(PoleFigureData(x111, y111, QString("<10-10>"), kRad, pfSize));
  //    break;
  //  default:
  //    return;
  //}

  //// This kicks off the threads that will generate the Pole Figure images
  //m_PoleFigureFuture->setFuture(QtConcurrent::mapped(data, generateODFPoleFigure));


  // Enable the MDF tab
  if (m_MDFWidget != NULL)
  {
    m_MDFWidget->setEnabled(true);
    m_MDFWidget->updateMDFPlot(odf);
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_addODFTextureBtn_clicked()
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
void StatsGenODFWidget::on_selectAnglesFile_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory;
  QString file = QFileDialog::getOpenFileName(this, tr("Select Angles File"), proposedFile, tr("Text Document (*.txt)"));
  angleFilePath->setText(file);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_loadODFTextureBtn_clicked()
{

  QString file = angleFilePath->text();
  if(true == file.isEmpty())
  {
    return;
  }
  else
  {
    AngleFileLoader::Pointer loader = AngleFileLoader::New();
    loader->setInputFile(angleFilePath->text().toStdString());
    loader->setAngleRepresentation(angleRepresentation->currentIndex());
    loader->setFileAnglesInDegrees(anglesInDegrees->isChecked());
    loader->setOutputAnglesInDegrees(true);
    QString delim;
    int index = delimiter->currentIndex();
    switch(index)
    {
      case 0:
        delim = " ";
        break;
      case 1:
        delim = "\t";
        break;
      case 2:
        delim = ",";
        break;
      case 3:
        delim = ";";
        break;
      default:
        delim = " ";

    }

    loader->setDelimiter(delim);
    FloatArrayType::Pointer data = loader->loadData();
    if (loader->getErrorCode() < 0)
    {
      QMessageBox::critical(this, "Error Loading Angle data", QString::fromStdString(loader->getErrorMessage()), QMessageBox::Ok);
      return;
    }

    m_ODFTableView->setDisabled(true);
    m_ODFTableModel->removeRows(0, m_ODFTableModel->rowCount());


    size_t count = data->GetNumberOfTuples();
    QVector<float> e1s(count);
    QVector<float> e2s(count);
    QVector<float> e3s(count);
    QVector<float> weights(count);
    QVector<float> sigmas(count);
    for(int i = 0; i < count; ++i)
    {
      e1s[i] = data->GetComponent(i, 0);
      e2s[i] = data->GetComponent(i, 1);
      e3s[i] = data->GetComponent(i, 2);
      weights[i] = 1.0;
      sigmas[i] = 1.0;
    }

    m_ODFTableModel->blockSignals(true);
    m_ODFTableModel->setColumnData(SGODFTableModel::Euler1, e1s);
    m_ODFTableModel->setColumnData(SGODFTableModel::Euler2, e3s);
    m_ODFTableModel->setColumnData(SGODFTableModel::Euler3, e2s);
    m_ODFTableModel->setColumnData(SGODFTableModel::Weight, weights);
    m_ODFTableModel->blockSignals(false);
    m_ODFTableModel->setColumnData(SGODFTableModel::Sigma, sigmas);

    on_m_CalculateODFBtn_clicked();
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_deleteODFTextureBtn_clicked()
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
SGODFTableModel* StatsGenODFWidget::tableModel()
{
  return m_ODFTableModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenMDFWidget* StatsGenODFWidget::getMDFWidget()
{
  return m_MDFWidget;
}
