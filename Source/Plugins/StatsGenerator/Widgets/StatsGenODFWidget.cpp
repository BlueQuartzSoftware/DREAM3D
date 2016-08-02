/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "StatsGenODFWidget.h"

#include "SIMPLib/Math/SIMPLibMath.h"

//-- C++ Includes
#include <iostream>

//-- Qt Includes
#include <QtCore/QVector>
#include <QtConcurrent/QtConcurrentMap>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QSettings>

#include <QtGui/QCloseEvent>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QAbstractItemDelegate>
#include <QtWidgets/QProgressDialog>

#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_abstract_scale_draw.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>

#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/HKL/CtfReader.h"

#include "OrientationLib/Texture/Texture.hpp"
#include "OrientationLib/Texture/StatsGen.hpp"
#include "OrientationLib/IO/AngleFileLoader.h"
#include "OrientationLib/Utilities/PoleFigureUtilities.h"
#include "OrientationLib/Utilities/PoleFigureImageUtilities.h"

#include "StatsGenerator/StatsGeneratorFilters/StatsGeneratorUtilities.h"
#include "StatsGenerator/Widgets/TableModels/SGODFTableModel.h"
#include "StatsGenerator/Widgets/StatsGenMDFWidget.h"
#include "StatsGenerator/Widgets/TextureDialog.h"

#define SHOW_POLE_FIGURES 1
#define COLOR_POLE_FIGURES 1

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_StatsGenODFWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenODFWidget::StatsGenODFWidget(QWidget* parent) :
  QWidget(parent),
  m_EnableAxisDecorations(false),
  m_Initializing(true),
  m_PhaseIndex(-1),
  m_CrystalStructure(Ebsd::CrystalStructure::Cubic_High),
  m_ODFTableModel(NULL),
  m_MDFWidget(NULL)
{
  m_OpenDialogLastDirectory = QDir::homePath();
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::extractStatsData(int index, StatsData* statsData, unsigned int phaseType)
{
  VectorOfFloatArray arrays;
  if(phaseType == SIMPL::PhaseType::PrimaryPhase)
  {
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsData);
    arrays = pp->getODF_Weights();
  }
  if(phaseType == SIMPL::PhaseType::PrecipitatePhase)
  {
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
    arrays = pp->getODF_Weights();
  }
  if(phaseType == SIMPL::PhaseType::TransformationPhase)
  {
    TransformationStatsData* tp = TransformationStatsData::SafePointerDownCast(statsData);
    arrays = tp->getODF_Weights();
  }
  if (arrays.size() > 0)
  {
    QVector<float> e1(static_cast<int>(arrays[0]->getNumberOfTuples()));
    ::memcpy( e1.data(), arrays[0]->getVoidPointer(0), sizeof(float)*e1.size() );

    QVector<float> e2(static_cast<int>(arrays[1]->getNumberOfTuples()));
    ::memcpy( e2.data(), arrays[1]->getVoidPointer(0), sizeof(float)*e2.size() );

    QVector<float> e3(static_cast<int>(arrays[2]->getNumberOfTuples()));
    ::memcpy( e3.data(), arrays[2]->getVoidPointer(0), sizeof(float)*e3.size() );

    QVector<float> weights(static_cast<int>(arrays[4]->getNumberOfTuples()));
    ::memcpy( weights.data(), arrays[4]->getVoidPointer(0), sizeof(float)*weights.size() );

    QVector<float> sigmas(static_cast<int>(arrays[3]->getNumberOfTuples()));
    ::memcpy( sigmas.data(), arrays[3]->getVoidPointer(0), sizeof(float)*sigmas.size() );

    // Convert from Radians to Degrees for the Euler Angles
    for (int i = 0; i < e1.size(); ++i)
    {
      e1[i] = static_cast<float>(e1[i] * 180.0f / M_PI);
      e2[i] = static_cast<float>(e2[i] * 180.0f / M_PI);
      e3[i] = static_cast<float>(e3[i] * 180.0f / M_PI);
    }

    if (e1.size() > 0)
    {
      // Load the data into the table model
      m_ODFTableModel->setTableData(e1, e2, e3, weights, sigmas);
    }
  }
  // Write the MDF Data if we have that functionality enabled
  if (m_MDFWidget != NULL)
  {
    m_MDFWidget->extractStatsData(index, statsData, phaseType);
  }
  updatePlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenODFWidget::getOrientationData(StatsData* statsData, unsigned int phaseType, bool preflight)
{
  int retErr = 0;

  QVector<float> e1s;
  QVector<float> e2s;
  QVector<float> e3s;
  QVector<float> weights;
  QVector<float> sigmas;

  SGODFTableModel* tableModel = NULL;

  if (weightSpreadGroupBox->isChecked() )
  {
    tableModel = m_ODFTableModel;
  }
  else
  {
    tableModel = m_OdfBulkTableModel;
  }

  e1s = tableModel->getData(SGODFTableModel::Euler1);
  e2s = tableModel->getData(SGODFTableModel::Euler2);
  e3s = tableModel->getData(SGODFTableModel::Euler3);
  weights = tableModel->getData(SGODFTableModel::Weight);
  sigmas = tableModel->getData(SGODFTableModel::Sigma);

  // Convert from Degrees to Radians
  for (QVector<float>::size_type i = 0; i < e1s.size(); i++)
  {
    e1s[i] = static_cast<float>(e1s[i] * M_PI / 180.0);
    e2s[i] = static_cast<float>(e2s[i] * M_PI / 180.0);
    e3s[i] = static_cast<float>(e3s[i] * M_PI / 180.0);
  }

  StatsGeneratorUtilities::GenerateODFBinData(statsData, phaseType, m_CrystalStructure, e1s, e2s, e3s, weights, sigmas, !preflight);

  // Write the MDF Data if we have that functionality enabled
  if (m_MDFWidget != NULL)
  {
    m_MDFWidget->getMisorientationData(statsData, phaseType, !preflight);
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
  connect(m_MDFWidget, SIGNAL(mdfParametersChanged()),
          this, SIGNAL(odfParametersChanged()));
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
void StatsGenODFWidget::setupGui()
{
  // Setup the TableView and Table Models
  QHeaderView* headerView = new QHeaderView(Qt::Horizontal, m_ODFTableView);
  headerView->sectionResizeMode(QHeaderView::Interactive);
  m_ODFTableView->setHorizontalHeader(headerView);
  headerView->show();

  m_OdfBulkTableModel = new SGODFTableModel;
  m_OdfBulkTableModel->setCrystalStructure(m_CrystalStructure);
  m_OdfBulkTableModel->setInitialValues();
  //m_ODFTableView->setModel(m_OdfBulkTableModel);

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

  // In release mode hide the Lambert Square Size.
  QString releaseType = QString::fromLatin1("Official");
  if (releaseType.compare("Official") == 0)
  {
    pfLambertSize->hide();
    pfLambertLabel->hide();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot)
{
  plot->setAxisTitle(QwtPlot::xBottom, xAxisName);
  plot->setAxisTitle(QwtPlot::yLeft, yAxisName);
  plot->setCanvasBackground(QColor(Qt::white));

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
  float inc = 2.0f / 449.0f;

  for(int i = 0; i < 450; ++i)
  {
    circleX[i] = 1.0 - (i * inc);
    circleX[450 + i] = -1.0 + (i * inc);

    circleY[i] = sqrt(1.0 - (circleX[i] * circleX[i]));
    circleY[450 + i] = -circleY[i];
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
    rotCrossY[0] = sqrt(1.0 - (rotCrossX[0] * rotCrossX[0]));
    rotCrossX[1] = -0.7071067811f;
    rotCrossY[1] = -sqrt(1.0 - (rotCrossX[1] * rotCrossX[1]));
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
    rotCrossY[0] = -sqrt(1.0 - (rotCrossX[0] * rotCrossX[0]));
    rotCrossX[1] = -0.7071067811f;
    rotCrossY[1] = sqrt(1.0 - (rotCrossX[1] * rotCrossX[1]));
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
void StatsGenODFWidget::on_m_CalculateODFBtn_clicked()
{
  int err = 0;

  QwtArray<float> e1s;
  QwtArray<float> e2s;
  QwtArray<float> e3s;
  QwtArray<float> weights;
  QwtArray<float> sigmas;
  QwtArray<float> odf;
  SGODFTableModel* tableModel = NULL;

  int npoints = 0;
  if (weightSpreadGroupBox->isChecked() )
  {
    tableModel = m_ODFTableModel;
    npoints = pfSamplePoints->value();
  }
  else
  {
    tableModel = m_OdfBulkTableModel;
    npoints = tableModel->rowCount();
  }

  e1s = tableModel->getData(SGODFTableModel::Euler1);
  e2s = tableModel->getData(SGODFTableModel::Euler2);
  e3s = tableModel->getData(SGODFTableModel::Euler3);
  weights = tableModel->getData(SGODFTableModel::Weight);
  sigmas = tableModel->getData(SGODFTableModel::Sigma);

  // Convert from Degrees to Radians
  for(int i = 0; i < e1s.size(); i++)
  {
    e1s[i] = static_cast<float>(e1s[i] * M_PI / 180.0);
    e2s[i] = static_cast<float>(e2s[i] * M_PI / 180.0);
    e3s[i] = static_cast<float>(e3s[i] * M_PI / 180.0);
  }
  size_t numEntries = e1s.size();

  int imageSize = pfImageSize->value();
  int lamberSize = pfLambertSize->value();
  int numColors = 16;

  QVector<size_t> dims(1, 3);
  FloatArrayType::Pointer eulers = FloatArrayType::CreateArray(npoints, dims, "Eulers");
  PoleFigureConfiguration_t config;
  QVector<UInt8ArrayType::Pointer> figures;

  if (Ebsd::CrystalStructure::Cubic_High == m_CrystalStructure)
  {
    // We now need to resize all the arrays here to make sure they are all allocated
    odf.resize(CubicOps::k_OdfSize);
    Texture::CalculateCubicODFData(e1s.data(), e2s.data(), e3s.data(),
                                     weights.data(), sigmas.data(), true,
                                     odf.data(), numEntries);

    err = StatsGen::GenCubicODFPlotData(odf.data(), eulers->getPointer(0), npoints);

    CubicOps ops;
    config.eulers = eulers.get();
    config.imageDim = imageSize;
    config.lambertDim = lamberSize;
    config.numColors = numColors;

    figures = ops.generatePoleFigure(config);
  }
  else if (Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructure)
  {
    // We now need to resize all the arrays here to make sure they are all allocated
    odf.resize(HexagonalOps::k_OdfSize);
    Texture::CalculateHexODFData(e1s.data(), e2s.data(), e3s.data(),
                                   weights.data(), sigmas.data(), true,
                                   odf.data(), numEntries);

    err = StatsGen::GenHexODFPlotData(odf.data(), eulers->getPointer(0), npoints);

    HexagonalOps ops;
    config.eulers = eulers.get();
    config.imageDim = imageSize;
    config.lambertDim = lamberSize;
    config.numColors = numColors;

    figures = ops.generatePoleFigure(config);
  }
  else if ( Ebsd::CrystalStructure::OrthoRhombic == m_CrystalStructure)
  {
    // We now need to resize all the arrays here to make sure they are all allocated
    odf.resize(OrthoRhombicOps::k_OdfSize);
    Texture::CalculateOrthoRhombicODFData(e1s.data(), e2s.data(), e3s.data(),
                                            weights.data(), sigmas.data(), true,
                                            odf.data(), numEntries);

    err = StatsGen::GenOrthoRhombicODFPlotData(odf.data(), eulers->getPointer(0), npoints);

    OrthoRhombicOps ops;
    config.eulers = eulers.get();
    config.imageDim = imageSize;
    config.lambertDim = lamberSize;
    config.numColors = numColors;

    figures = ops.generatePoleFigure(config);
  }

  if (err == 1)
  {
    //TODO: Present Error Message
    return;
  }

  QImage image = PoleFigureImageUtilities::Create3ImagePoleFigure(figures[0].get(), figures[1].get(), figures[2].get(), config, imageLayout->currentIndex());
  m_PoleFigureLabel->setPixmap(QPixmap::fromImage(image));

  // Enable the MDF tab
  if (m_MDFWidget != NULL)
  {
    m_MDFWidget->setEnabled(true);
    m_MDFWidget->updateMDFPlot(odf);
  }

  emit odfParametersChanged();
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
    if (!m_ODFTableModel->insertRow(m_ODFTableModel->rowCount())) { return; }
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
  emit odfParametersChanged();
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
void StatsGenODFWidget::on_angleFilePath_textChanged()
{
  // If the text has changed, we don't know if the file exists/is valid,
  // so blow away the bulk table model, if it exists, and emit that
  // the user needs to select a valid file and reload
  if (m_OdfBulkTableModel) { delete m_OdfBulkTableModel; m_OdfBulkTableModel = NULL; }
  m_OdfBulkTableModel = new SGODFTableModel;
  m_OdfBulkTableModel->setCrystalStructure(m_CrystalStructure);
  m_OdfBulkTableModel->setInitialValues();

  emit bulkLoadEvent(true);
  emit odfParametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_loadODFTextureBtn_clicked()
{
  QString file = angleFilePath->text();
  if(true == file.isEmpty())
  {
    emit bulkLoadEvent(true);
    return;
  }

  QFileInfo fi(angleFilePath->text());
  if(fi.exists() == false)
  {
    emit bulkLoadEvent(true);
    return;
  }

  size_t count = 0;
  QVector<float> e1s(count);
  QVector<float> e2s(count);
  QVector<float> e3s(count);
  QVector<float> weights(count);
  QVector<float> sigmas(count);

  QProgressDialog progress("Loading Data ....", "Cancel", 0, 3, this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(2000);
  progress.show();

  if (fi.suffix().compare(Ebsd::Ang::FileExt) == 0)
  {
    QMessageBox::critical(this, "ANG File Loading NOT Supported",
                          "Please use the 'Write StatsGenerator ODF Angle File' filter from DREAM.3D to generate a file. See that filter's help for the proper format.", QMessageBox::Ok);
    emit bulkLoadEvent(true);
    return;
#if 0
    phaseOfInterest->setEnabled(true);
    anglesInDegrees->setEnabled(true);
    angleRepresentation->setEnabled(false);
    delimiter->setEnabled(false);
    progress.setValue(1);
    progress.setLabelText("[1/3] Reading File ...");
    AngReader loader;
    loader.setFileName(angleFilePath->text());
    loader.setReadHexGrid(true);
    int err = loader.readFile();
    if(err < 0)
    {
      QMessageBox::critical(this, "Error loading the ANG file", loader.getErrorMessage(), QMessageBox::Ok);
      return;
    }
    float* phi1 = loader.getPhi1Pointer();
    float* phi = loader.getPhiPointer();
    float* phi2 = loader.getPhi2Pointer();
    int32_t* phase = loader.getPhaseDataPointer();
    QVector<AngPhase::Pointer> phaseInfo = loader.getPhaseVector();
    phaseOfInterest->setRange(0, phaseInfo.size());
    int phaseSelection = phaseOfInterest->value();

    int xDim = loader.getXDimension();
    int yDim = loader.getYDimension();
    size_t totalScanPoints = xDim * yDim;


    count = 0;
    for(size_t i = 0; i < totalScanPoints; ++i)
    {
      if(phase[i] == phaseSelection)
      {
        count++;
      }
    }

    e1s.resize(count);
    e2s.resize(count);
    e3s.resize(count);
    weights.resize(count);
    sigmas.resize(count);
    count = 0;
    for(size_t i = 0; i < totalScanPoints; ++i)
    {
      if(phase[i] == phaseSelection)
      {
        // Be sure to convert the ANG Radians into degrees because this part of
        // DREAM.3D stores the angles as Degrees.
        e1s[count] = phi1[i] * SIMPLib::Constants::k_180OverPi;
        e2s[count] = phi[i] * SIMPLib::Constants::k_180OverPi;
        e3s[count] = phi2[i] * SIMPLib::Constants::k_180OverPi;
        weights[count] = 1.0;
        sigmas[count] = 0.0;
        count++;
      }
    }
#endif
  }
  else if (fi.suffix().compare(Ebsd::Ctf::FileExt) == 0)
  {
    QMessageBox::critical(this, "CTF File Loading not Supported",
                          "Please use the 'Write StatsGenerator ODF Angle File' filter from DREAM.3D to generate a file. See that filter's help for the proper format.", QMessageBox::Ok);
    emit bulkLoadEvent(true);
    return;
#if 0
    phaseOfInterest->setEnabled(true);
    anglesInDegrees->setEnabled(true);
    angleRepresentation->setEnabled(false);
    delimiter->setEnabled(false);
    progress.setValue(1);
    progress.setLabelText("[1/3] Reading File ...");
    CtfReader loader;
    loader.setFileName(angleFilePath->text());
    int err = loader.readFile();
    if(err < 0)
    {
      QMessageBox::critical(this, "Error loading the CTF file", loader.getErrorMessage(), QMessageBox::Ok);
      return;
    }
    float* phi1 = loader.getEuler1Pointer();
    float* phi = loader.getEuler2Pointer();
    float* phi2 = loader.getEuler3Pointer();
    int32_t* phase = loader.getPhasePointer();
    QVector<CtfPhase::Pointer> phaseInfo = loader.getPhaseVector();
    phaseOfInterest->setRange(0, phaseInfo.size());
    int phaseSelection = phaseOfInterest->value();

    int xDim = loader.getXDimension();
    int yDim = loader.getYDimension();
    size_t totalScanPoints = xDim * yDim;

    count = 0;
    for(size_t i = 0; i < totalScanPoints; ++i)
    {
      if(phase[i] == phaseSelection)
      {
        count++;
      }
    }

    e1s.resize(count);
    e2s.resize(count);
    e3s.resize(count);
    weights.resize(count);
    sigmas.resize(count);
    count = 0;
    for(size_t i = 0; i < totalScanPoints; ++i)
    {
      if(phase[i] == phaseSelection)
      {
        e1s[count] = phi1[i];
        e2s[count] = phi[i];
        e3s[count] = phi2[i];
        weights[count] = 1.0;
        sigmas[count] = 0.0;
        count++;
      }
    }
#endif
  }
  else
  {
    anglesInDegrees->setEnabled(true);
    angleRepresentation->setEnabled(true);
    delimiter->setEnabled(true);
    progress.setValue(1);
    progress.setLabelText("[1/3] Reading File ...");
    AngleFileLoader::Pointer loader = AngleFileLoader::New();
    loader->setInputFile(angleFilePath->text());
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
      QMessageBox::critical(this, "Error Loading Angle data", loader->getErrorMessage(), QMessageBox::Ok);
      emit bulkLoadEvent(true);
      return;
    }
    count = data->getNumberOfTuples();
    e1s.resize(count);
    e2s.resize(count);
    e3s.resize(count);
    weights.resize(count);
    sigmas.resize(count);
    for(size_t i = 0; i < count; ++i)
    {
      e1s[i] = data->getComponent(i, 0);
      e2s[i] = data->getComponent(i, 1);
      e3s[i] = data->getComponent(i, 2);
      weights[i] = data->getComponent(i, 3);
      sigmas[i] = data->getComponent(i, 4);
    }
  }

  progress.setValue(2);
  progress.setLabelText("[2/3] Rendering Pole Figure ...");

  // Just blow away our table and create a new one. Quicker than removing all the rows
  // first, the repopulating them
  if (m_OdfBulkTableModel) { delete m_OdfBulkTableModel; m_OdfBulkTableModel = NULL; }
  m_OdfBulkTableModel = new SGODFTableModel;
  m_OdfBulkTableModel->setCrystalStructure(m_CrystalStructure);
  m_OdfBulkTableModel->setInitialValues();

#if 1
  m_OdfBulkTableModel->blockSignals(true);
  m_OdfBulkTableModel->setColumnData(SGODFTableModel::Euler1, e1s);
  m_OdfBulkTableModel->setColumnData(SGODFTableModel::Euler2, e2s);
  m_OdfBulkTableModel->setColumnData(SGODFTableModel::Euler3, e3s);
  m_OdfBulkTableModel->setColumnData(SGODFTableModel::Weight, weights);
  m_OdfBulkTableModel->setColumnData(SGODFTableModel::Sigma, sigmas);
  m_OdfBulkTableModel->blockSignals(false);
#endif

  emit bulkLoadEvent(false);
  on_m_CalculateODFBtn_clicked();
  progress.setValue(3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_deleteODFTextureBtn_clicked()
{
  QItemSelectionModel* selectionModel = m_ODFTableView->selectionModel();
  if (!selectionModel->hasSelection()) { return; }
  QModelIndex index = selectionModel->currentIndex();
  if (!index.isValid()) { return; }
  m_ODFTableModel->removeRow(index.row(), index.parent());
  if (m_ODFTableModel->rowCount() > 0)
  {
    m_ODFTableView->resizeColumnsToContents();
  }
  emit odfParametersChanged();
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_bulkLoadGroupBox_clicked ( bool checked )
{
  weightSpreadGroupBox->setChecked(!checked);
  m_ODFTableView->setModel(m_OdfBulkTableModel);
  QAbstractItemDelegate* idelegate = m_OdfBulkTableModel->getItemDelegate();
  m_ODFTableView->setItemDelegate(idelegate);
  emit bulkLoadEvent(!(m_OdfBulkTableModel->rowCount() > 0));
  emit odfParametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_weightSpreadGroupBox_clicked ( bool checked )
{
  bulkLoadGroupBox->setChecked(!checked);
  m_ODFTableView->setModel(m_ODFTableModel);
  QAbstractItemDelegate* idelegate = m_ODFTableModel->getItemDelegate();
  m_ODFTableView->setItemDelegate(idelegate);
  emit bulkLoadEvent(false);
  emit odfParametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenODFWidget::on_savePoleFigureImage_clicked()
{
  QString Ftype = "Image Files";
  QString ext = "*.png";
  QString s = "Image Files (*.tiff *.png *.bmp);;All Files(*.*)";
  QString defaultName = m_OpenDialogLastDirectory + QDir::separator() + "Untitled.png";
  QString file = QFileDialog::getSaveFileName(this, tr("Save File As"), defaultName, s);

  if(true == file.isEmpty())
  {
    return;
  }
  // bool ok = false;
  file = QDir::toNativeSeparators(file);
  // Store the last used directory into the private instance variable
  QFileInfo fi(file);
  m_OpenDialogLastDirectory = fi.path();

  QImage image = m_PoleFigureLabel->pixmap()->toImage();
  image.save(file);
}
