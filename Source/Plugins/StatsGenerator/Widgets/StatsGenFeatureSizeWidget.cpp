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
#include "StatsGenFeatureSizeWidget.h"

#include <iostream>
#include <limits>

#include <QtCore/QSettings>
#include <QtGui/QDoubleValidator>
#include <QtGui/QIntValidator>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>

// Needed for AxisAngle_t and Crystal Symmetry constants
#include "EbsdLib/EbsdConstants.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"

#include "OrientationLib/Texture/StatsGen.hpp"

#include "StatsGenerator/StatsGeneratorConstants.h"
#include "StatsGenerator/Widgets/StatsGenCurveTracker.h"

//-- Qwt Includes AFTER SIMPLib Math due to improper defines in qwt_plot_curve.h
#include <qwt_picker_machine.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_shapeitem.h>
#include <qwt_scale_widget.h>
#include <qwt_symbol.h>

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_StatsGenFeatureSizeWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenFeatureSizeWidget::StatsGenFeatureSizeWidget(QWidget* parent)
: QWidget(parent)
, m_Mu(1.0f)
, m_Sigma(0.1f)
, m_MinCutOff(5.0f)
, m_MaxCutOff(5.0f)
, m_BinStep(0.5f)
, m_PhaseIndex(-1)
, m_CrystalStructure(Ebsd::CrystalStructure::Cubic_High)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenFeatureSizeWidget::~StatsGenFeatureSizeWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenFeatureSizeWidget::setupGui()
{
  QLocale loc = QLocale::system();

  distributionTypeCombo->addItem(SIMPL::StringConstants::BetaDistribution.toLatin1().data());
  distributionTypeCombo->addItem(SIMPL::StringConstants::LogNormalDistribution.toLatin1().data());
  distributionTypeCombo->addItem(SIMPL::StringConstants::PowerLawDistribution.toLatin1().data());
  distributionTypeCombo->setCurrentIndex(SIMPL::DistributionType::LogNormal);
  distributionTypeCombo->setVisible(false);

  m_MuValidator = new QDoubleValidator(m_Mu_SizeDistribution);
  m_MuValidator->setLocale(loc);
  m_MuValidator->setRange(0.0001, 10.0, 6);
  m_Mu_SizeDistribution->setValidator(m_MuValidator);

  m_SigmaValidator = new QDoubleValidator(m_Sigma_SizeDistribution);
  m_SigmaValidator->setLocale(loc);
  m_SigmaValidator->setRange(0.0000, 5.0, 6);
  m_Sigma_SizeDistribution->setValidator(m_SigmaValidator);

  m_MinCutoffValidator = new QDoubleValidator(0.000, std::numeric_limits<double>::infinity(), 6, m_MinSigmaCutOff);
  m_MinCutoffValidator->setLocale(loc);
  m_MinSigmaCutOff->setValidator(m_MinCutoffValidator);

  m_MaxCutoffValidator = new QDoubleValidator(0.000, std::numeric_limits<double>::infinity(), 6, m_MinSigmaCutOff);
  m_MaxCutoffValidator->setLocale(loc);
  m_MaxSigmaCutOff->setValidator(m_MaxCutoffValidator);

  // These must go after all the Validators are setup because when they are set
  // a signal is going to get fired
  m_Mu_SizeDistribution->setText(loc.toString(StatsGeneratorConstants::k_Mu));
  m_Sigma_SizeDistribution->setText(loc.toString(StatsGeneratorConstants::k_Sigma));

  m_SizeDistributionPlot->setCanvasBackground(QColor(Qt::white));

  QwtText qwtStr(QString("Feature ESD Probability Density Function"));
  qwtStr.setFont(QFont("Arial", SG_FONT_SIZE, QFont::Bold, false));
  m_SizeDistributionPlot->setTitle(qwtStr);

  qwtStr.setText(QString("Equivalent Sphere Diameter (ESD)"));
  m_SizeDistributionPlot->setAxisTitle(QwtPlot::xBottom, qwtStr);

  qwtStr.setText(QString("Probability of Sampling ESD \nfrom the Distribution"));
  m_SizeDistributionPlot->setAxisTitle(QwtPlot::yLeft, qwtStr);

  m_SizeDistributionPlot->plotLayout()->setAlignCanvasToScales(true);
  for(int axis = 0; axis < QwtPlot::axisCnt; axis++)
  {
    m_SizeDistributionPlot->axisWidget(axis)->setMargin(0);
  }
  QwtPlotCanvas* canvas = new QwtPlotCanvas();
  canvas->setAutoFillBackground(false);
  canvas->setFrameStyle(QFrame::NoFrame);
  // canvas->setPalette(pal);
  m_SizeDistributionPlot->setCanvas(canvas);

  QwtPlotPicker* plotPicker = new QwtPlotPicker(m_SizeDistributionPlot->xBottom, m_SizeDistributionPlot->yLeft, QwtPicker::CrossRubberBand, QwtPicker::AlwaysOn, m_SizeDistributionPlot->canvas());
  QwtPickerMachine* pickerMachine = new QwtPickerClickPointMachine();
  plotPicker->setStateMachine(pickerMachine);
  connect(plotPicker, SIGNAL(selected(const QPointF&)), this, SLOT(plotPointSelected(const QPointF&)));

#if 0
  m_grid = new QwtPlotGrid;
  m_grid->enableXMin(true);
  m_grid->enableYMin(true);
  m_grid->setMajorPen(QPen(Qt::gray, 0, Qt::SolidLine));
  m_grid->setMinorPen(QPen(Qt::lightGray, 0, Qt::DotLine));
  m_grid->attach(m_SizeDistributionPlot);
#endif

  updateSizeDistributionPlot();
  calculateNumberOfBins();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenFeatureSizeWidget::plotPointSelected(const QPointF& point)
{
  int selectedBin = -1;
  for(int i = 0; i < m_BarChartItems.size(); i++)
  {
    QwtPlotShapeItem* item = m_BarChartItems[i];

    // qDebug()  << "\t" << item->boundingRect();
    if(item->boundingRect().contains(point))
    {
      QColor fillColor = QColor("IndianRed");
      fillColor.setAlpha(200);
      item->setBrush(fillColor);
      selectedBin = i;
    }
    else
    {
      QColor fillColor = QColor("RoyalBlue");
      fillColor.setAlpha(200);
      item->setBrush(fillColor);
    }
  }
  m_SizeDistributionPlot->replot();
  emit binSelected(selectedBin);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenFeatureSizeWidget::userEditedPlotData()
{
  m_Mu_SizeDistribution->setEnabled(false);
  m_Sigma_SizeDistribution->setEnabled(false);
  m_MinSigmaCutOff->setEnabled(false);
  m_MaxSigmaCutOff->setEnabled(false);
  m_BinStepSize->setEnabled(false);
  m_FeatureESD->setEnabled(false);
  distributionTypeCombo->setEnabled(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenFeatureSizeWidget::resetUI()
{
  m_Mu_SizeDistribution->setEnabled(true);
  m_Sigma_SizeDistribution->setEnabled(true);
  m_MinSigmaCutOff->setEnabled(true);
  m_MaxSigmaCutOff->setEnabled(true);
  m_BinStepSize->setEnabled(true);
  m_FeatureESD->setEnabled(true);
  distributionTypeCombo->setEnabled(true);

  this->blockSignals(true);
  m_Mu_SizeDistribution->setText("1.0");
  m_Sigma_SizeDistribution->setText("0.1");
  m_MinSigmaCutOff->setText("5.0");
  m_MaxSigmaCutOff->setText("5.0");
  distributionTypeCombo->setCurrentIndex(0);
  this->blockSignals(false);
  m_BinStepSize->setValue(0.5f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenFeatureSizeWidget::mousePressEvent(QMouseEvent* event)
{
  //  qDebug() << "mousePressedEvent";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenFeatureSizeWidget::gatherSizeDistributionFromGui(float& mu, float& sigma, float& minCutOff, float& maxCutOff, float& stepSize)
{
  QLocale loc = QLocale::system();

  bool ok = false;
  mu = loc.toFloat(m_Mu_SizeDistribution->text(), &ok);
  if(ok == false)
  {
    return 0;
  }
  sigma = loc.toFloat(m_Sigma_SizeDistribution->text(), &ok);
  if(ok == false)
  {
    return 0;
  }
  minCutOff = loc.toFloat(m_MinSigmaCutOff->text(), &ok);
  if(ok == false)
  {
    return 0;
  }
  maxCutOff = loc.toFloat(m_MaxSigmaCutOff->text(), &ok);
  if(ok == false)
  {
    return 0;
  }
  stepSize = loc.toFloat(m_BinStepSize->text(), &ok);
  if(ok == false)
  {
    return 0;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool StatsGenFeatureSizeWidget::validateValue(QDoubleValidator* val, QLineEdit* lineEdit)
{
  QString value(lineEdit->text());
  int not_used = 0;
  QValidator::State state = val->validate(value, not_used);
  if(state != QValidator::Acceptable)
  {
    lineEdit->setStyleSheet("border: 1px solid red;");
    return false;
  }
  else
  {
    lineEdit->setStyleSheet("");
  }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool StatsGenFeatureSizeWidget::validateMuSigma()
{
  bool muValid = validateValue(m_MuValidator, m_Mu_SizeDistribution);
  if(!muValid)
  {
    m_NumberBinsGenerated->setText("Error: Mu not valid");
    emit userEnteredValidData(false);
    return false;
  }
  bool sigmaValid = validateValue(m_SigmaValidator, m_Sigma_SizeDistribution);
  if(!sigmaValid)
  {
    m_NumberBinsGenerated->setText("Error: Sigma not valid");
    emit userEnteredValidData(false);
    return false;
  }
  bool minValid = validateValue(m_MinCutoffValidator, m_MinSigmaCutOff);
  if(!minValid)
  {
    m_NumberBinsGenerated->setText("Error: Min Cutoff not valid");
    emit userEnteredValidData(false);
    return false;
  }
  bool maxValid = validateValue(m_MaxCutoffValidator, m_MaxSigmaCutOff);
  if(!maxValid)
  {
    m_NumberBinsGenerated->setText("Error: Max Cutoff not valid");
    emit userEnteredValidData(false);
    return false;
  }

  if(muValid && sigmaValid && minValid && maxValid)
  {
    emit userEnteredValidData(true);
    return true;
  }

  m_NumberBinsGenerated->setText("Error");
  emit userEnteredValidData(false);
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenFeatureSizeWidget::on_m_FeatureESD_editingFinished()
{
  QLocale loc = QLocale::system();
  bool ok = false;
  float esd = loc.toFloat(m_FeatureESD->text(), &ok);
  float mu = std::log(esd);
  m_EsdUpdated = true;
  m_Mu_SizeDistribution->setText(loc.toString(mu));
  m_EsdUpdated = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenFeatureSizeWidget::on_m_Mu_SizeDistribution_textChanged(const QString& text)
{
  Q_UNUSED(text)
  if(!validateMuSigma())
  {
    return;
  }

  if(!m_EsdUpdated)
  {
    QLocale loc = QLocale::system();
    m_FeatureESD->blockSignals(true);
    bool ok = false;
    float mu = loc.toFloat(m_Mu_SizeDistribution->text(), &ok);
    float esd = std::exp(mu);
    m_FeatureESD->setText(loc.toString(esd));
    m_FeatureESD->blockSignals(false);
  }

  if(updateSizeDistributionPlot() < 0)
  {
    return;
  }
  if(calculateNumberOfBins() < 0)
  {
    return;
  }
  emit dataChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenFeatureSizeWidget::on_m_Sigma_SizeDistribution_textChanged(const QString& text)
{
  Q_UNUSED(text)
  if(!validateMuSigma())
  {
    return;
  }
  if(updateSizeDistributionPlot() < 0)
  {
    return;
  }
  m_Sigma_SizeDistribution->setFocus();
  if(calculateNumberOfBins() < 0)
  {
    return;
  }
  emit dataChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenFeatureSizeWidget::on_m_MinSigmaCutOff_textChanged(const QString& text)
{
  Q_UNUSED(text)
  if(!validateMuSigma())
  {
    return;
  }
  if(updateSizeDistributionPlot() < 0)
  {
    return;
  }
  m_MinSigmaCutOff->setFocus();
  if(calculateNumberOfBins() < 0)
  {
    return;
  }
  emit dataChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenFeatureSizeWidget::on_m_MaxSigmaCutOff_textChanged(const QString& text)
{
  Q_UNUSED(text)
  if(!validateMuSigma())
  {
    return;
  }
  if(updateSizeDistributionPlot() < 0)
  {
    return;
  }
  m_MaxSigmaCutOff->setFocus();
  if(calculateNumberOfBins() < 0)
  {
    return;
  }
  emit dataChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenFeatureSizeWidget::on_m_BinStepSize_valueChanged(double v)
{
  Q_UNUSED(v)
  if(!validateMuSigma())
  {
    return;
  }
  if(updateSizeDistributionPlot() < 0)
  {
    return;
  }
  if(calculateNumberOfBins() < 0)
  {
    return;
  }
  emit dataChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenFeatureSizeWidget::calculateNumberOfBins()
{
  float mu = 1.0;
  float sigma = 1.0;
  float minCutOff = 1.0;
  float maxCutOff = 1.0;
  float stepSize = 1.0;
  float max, min;
  int err = gatherSizeDistributionFromGui(mu, sigma, minCutOff, maxCutOff, stepSize);
  if(err < 0)
  {
    return err;
  }

  int n = StatsGen::ComputeNumberOfBins(mu, sigma, minCutOff, maxCutOff, stepSize, max, min);
  if(n < 0)
  {
    m_NumberBinsGenerated->setText("Error");
    err = n;
  }
  else
  {
    m_NumberBinsGenerated->setText(QString::number(n));
    err = 1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenFeatureSizeWidget::calculateNumberOfBins(float mu, float sigma, float minCutOff, float maxCutOff, float stepSize)
{
  float max, min; // Only needed for the method. Not used otherwise.

  return StatsGen::ComputeNumberOfBins(mu, sigma, minCutOff, maxCutOff, stepSize, max, min);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenFeatureSizeWidget::computeBinsAndCutOffs(float mu, float sigma, float minCutOff, float maxCutOff, float binStepSize, QwtArray<float>& binsizes, QwtArray<float>& xCo, QwtArray<float>& yCo,
                                                     float& xMax, float& yMax, QwtArray<float>& x, QwtArray<float>& y)
{
  int err = 0;
  int size = 250;

  err = StatsGen::GenLogNormalPlotData<QwtArray<float>>(mu, sigma, x, y, size, minCutOff, maxCutOff);
  if(err == 1)
  {
    // TODO: Present Error Message
    return -1;
  }

  xMax = x.last();
  for(int i = 0; i < size; ++i)
  {
    if(y[i] > yMax)
    {
      yMax = y[i];
    }
  }

  xCo.clear();
  yCo.clear();
  int numsizebins = 1;
  binsizes.clear();
  err = StatsGen::GenCutOff<float, QwtArray<float>>(mu, sigma, minCutOff, maxCutOff, binStepSize, xCo, yCo, yMax, numsizebins, binsizes);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenFeatureSizeWidget::updateSizeDistributionPlot()
{
  float mu = 1.0;
  float sigma = 1.0;
  float minCutOff = 1.0;
  float maxCutOff = 1.0;
  float stepSize = 1.0;
  int err = gatherSizeDistributionFromGui(mu, sigma, minCutOff, maxCutOff, stepSize);
  if(err < 0)
  {
    return err;
  }

  QwtArray<float> xCo;
  QwtArray<float> yCo;
  QwtArray<float> binsizes;
  float xMax = std::numeric_limits<float>::min();
  float yMax = std::numeric_limits<float>::min();
  QwtArray<float> x;
  QwtArray<float> y;
  err = computeBinsAndCutOffs(mu, sigma, minCutOff, maxCutOff, stepSize, binsizes, xCo, yCo, xMax, yMax, x, y);

  // Adjust the BinStepSize so that there are at least 2x as many x points as bins.
  while(binsizes.size() > x.size() / 2)
  {
    m_BinStepSize->blockSignals(true);
    m_BinStepSize->stepUp();
    m_BinStepSize->blockSignals(false);
    int err = gatherSizeDistributionFromGui(mu, sigma, minCutOff, maxCutOff, stepSize);
    err = computeBinsAndCutOffs(mu, sigma, minCutOff, maxCutOff, stepSize, binsizes, xCo, yCo, xMax, yMax, x, y);
  }

  if(err < 0)
  {
    return err;
  }

  if(nullptr == m_SizeDistributionCurve)
  {
    m_SizeDistributionCurve = new QwtPlotCurve("Size Distribution");
    m_SizeDistributionCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    m_SizeDistributionCurve->setPen(QPen(QColor("IndianRed"), 2));
    m_SizeDistributionCurve->attach(m_SizeDistributionPlot);
  }

  if(nullptr == m_SizeDistCenterPointCurve)
  {
    m_SizeDistCenterPointCurve = new QwtPlotCurve("Size Distribution Bin Centers");
    m_SizeDistCenterPointCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    m_SizeDistCenterPointCurve->setPen(QPen(Qt::blue, 2));

    QColor color = QColor("Black");
    m_SizeDistCenterPointCurve->setPen(color, 2);
    m_SizeDistCenterPointCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    m_SizeDistCenterPointCurve->setStyle(QwtPlotCurve::NoCurve);
    QwtSymbol* symbol = new QwtSymbol(QwtSymbol::VLine, QBrush(Qt::white), QPen(color, 1), QSize(1, 16));
    m_SizeDistCenterPointCurve->setSymbol(symbol);

    m_SizeDistCenterPointCurve->attach(m_SizeDistributionPlot);
  }

  // Place a vertical Line on the plot where the Min and Max Cutoff values are
  if(nullptr == m_CutOffMin)
  {
    m_CutOffMin = new QwtPlotMarker();
    m_CutOffMin->attach(m_SizeDistributionPlot);
  }

  QString str = QString("Min Feature ESD = %1").arg(xCo[0]);
  QwtText qwtStr = QwtText(str);
  qwtStr.setFont(QFont("Arial", SG_FONT_SIZE, QFont::Bold, false));
  m_CutOffMin->setLabel(qwtStr);
  m_CutOffMin->setLabelAlignment(Qt::AlignRight | Qt::AlignTop);
  m_CutOffMin->setLabelOrientation(Qt::Vertical);
  m_CutOffMin->setLineStyle(QwtPlotMarker::VLine);
  m_CutOffMin->setLinePen(QPen(Qt::blue, 1, Qt::SolidLine));
  m_CutOffMin->setXValue(xCo[0]);

  if(nullptr == m_CutOffMax)
  {
    m_CutOffMax = new QwtPlotMarker();
    m_CutOffMax->attach(m_SizeDistributionPlot);
  }
  str = QString("Max Feature ESD = %1").arg(xCo[1]);
  qwtStr = QwtText(str);
  qwtStr.setFont(QFont("Arial", SG_FONT_SIZE, QFont::Bold, false));
  m_CutOffMax->setLabel(qwtStr);
  m_CutOffMax->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
  m_CutOffMax->setLabelOrientation(Qt::Vertical);
  m_CutOffMax->setLineStyle(QwtPlotMarker::VLine);
  m_CutOffMax->setLinePen(QPen(Qt::blue, 1, Qt::SolidLine));
  m_CutOffMax->setXValue(xCo[1]);

  QwtArray<double> xD(x.size());
  QwtArray<double> yD(x.size());
  for(int i = 0; i < x.size(); ++i)
  {
    xD[i] = static_cast<double>(x[i]);
    yD[i] = static_cast<double>(y[i]);
  }

  float xAxisMargin = (xCo[1] - xCo[0]) * .05f;
  float xAxisMin = xCo[0] - xAxisMargin;
  float xAxisMax = xCo[1] + xAxisMargin;

  m_SizeDistributionCurve->setSamples(xD, yD);
  m_SizeDistributionPlot->setAxisScale(QwtPlot::xBottom, xAxisMin, xAxisMax);
  m_SizeDistributionPlot->setAxisScale(QwtPlot::yLeft, 0.0, yMax * 1.05);

  for(int i = 0; i < m_BarChartItems.count(); i++)
  {
    m_BarChartItems[i]->detach();
    delete m_BarChartItems[i];
  }

  m_BarChartItems.clear();

  float xMin = x.first();
  xMax = x.last();

  float binWidth = (xMax - xMin) / static_cast<float>(binsizes.size());
  float xStart = xMin;
  float xEnd = xStart + binWidth;

  QwtArray<double> xBinCenter(binsizes.count());
  QwtArray<double> yBinCenter(binsizes.count());

  int j = 0;
  for(int i = 0; i < binsizes.count(); i++)
  {
    QwtPlotShapeItem* item = new QwtPlotShapeItem("");
    item->setItemAttribute(QwtPlotItem::Legend, false);
    item->setLegendMode(QwtPlotShapeItem::LegendShape);
    item->setLegendIconSize(QSize(20, 20));
    item->setRenderHint(QwtPlotItem::RenderAntialiased, true);

    int jStart = j;
    // qDebug() << indexStart << "\t" << indexMid << "\t" << indexEnd;
    while(j < x.size() && xEnd > x[j])
    {
      j++;
    }

    int iSize = j - jStart;
    if(iSize == 0)
    {
      return -1;
    }

    int midIndex = jStart + iSize / 2;

    if((iSize % 2) == 0)
    {
      iSize = jStart + iSize / 2;
      xBinCenter[i] = (x[iSize] + x[iSize - 1]) / 2.0;
      yBinCenter[i] = (y[iSize] + y[iSize - 1]) / 2.0;
    }
    else
    {
      if(midIndex < x.size())
      {
        xBinCenter[i] = x[midIndex];
        yBinCenter[i] = y[midIndex];
      }
    }

    //    qDebug() << i << "\t" << jStart << "\t" << j << "\t" << xStart << "\t" << (xStart + binWidth) << "\t" << midIndex
    //    << "(" << xStart <<  y[midIndex] << ")  (" <<  binWidth << y[midIndex] << ")";

    // The coordinates are done in traditional cartesion coordinate system and NOT in CS Graphics Coordinate system
    QRectF rect = QRectF(xStart, 0.0, binWidth, yBinCenter[i]);

    QPainterPath path;
    path.addRect(rect);

    QColor fillColor = QColor("RoyalBlue");
    fillColor.setAlpha(200);

    QPen pen(Qt::black, 1);
    pen.setJoinStyle(Qt::MiterJoin);
    item->setPen(pen);
    item->setBrush(fillColor);

    item->setShape(path);
    item->attach(m_SizeDistributionPlot);
    m_BarChartItems.push_back(item);

    xStart = xStart + binWidth;
    xEnd = xStart + binWidth;
  }

  m_SizeDistCenterPointCurve->setSamples(xBinCenter, yBinCenter);

  m_SizeDistributionPlot->replot();

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenFeatureSizeWidget::plotSizeDistribution()
{
  float mu = 1.0;
  float sigma = 1.0;
  float minCutOff = 1.0;
  float maxCutOff = 1.0;
  float stepSize = 1.0;
  int err = gatherSizeDistributionFromGui(mu, sigma, minCutOff, maxCutOff, stepSize);
  if(err == 0)
  {
    return;
  }

  QwtArray<float> xCo;
  QwtArray<float> yCo;
  QwtArray<float> binSizes;
  float xMax = std::numeric_limits<float>::min();
  float yMax = std::numeric_limits<float>::min();
  QwtArray<float> x;
  QwtArray<float> y;
  err = computeBinsAndCutOffs(mu, sigma, minCutOff, maxCutOff, stepSize, binSizes, xCo, yCo, xMax, yMax, x, y);
  if(err < 0)
  {
    return;
  }

  m_Mu = mu;
  m_Sigma = sigma;
  m_MaxCutOff = maxCutOff;
  m_MinCutOff = minCutOff;
  m_BinStep = stepSize;
  m_BinSizes = binSizes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGenFeatureSizeWidget::getStatisticsData(PrimaryStatsData* primaryStatsData)
{

  float mu = 1.0f;
  float sigma = 1.0f;
  float minCutOff = 1.0f;
  float maxCutOff = 1.0f;
  float binStep = 1.0f;
  gatherSizeDistributionFromGui(mu, sigma, minCutOff, maxCutOff, binStep);

  QwtArray<float> xCo;
  QwtArray<float> yCo;
  QwtArray<float> binsizes;
  float xMax = std::numeric_limits<float>::min();
  float yMax = std::numeric_limits<float>::min();
  QwtArray<float> x;
  QwtArray<float> y;
  int err = computeBinsAndCutOffs(mu, sigma, minCutOff, maxCutOff, binStep, binsizes, xCo, yCo, xMax, yMax, x, y);
  if(err < 0)
  {
    return err;
  }

  // We need to compute the Max and Min Diameter Bin Values
  float mindiameter = xCo[0];
  float maxdiameter = xCo[1];
  float avglogdiam = mu;
  float sdlogdiam = sigma;
  float stepSize = binStep;

  // Feature Diameter Info
  primaryStatsData->setBinStepSize(stepSize);
  primaryStatsData->setMaxFeatureDiameter(maxdiameter);
  primaryStatsData->setMinFeatureDiameter(mindiameter);
  // Feature Size Distribution
  {
    VectorOfFloatArray data;
    FloatArrayType::Pointer d1 = FloatArrayType::CreateArray(1, SIMPL::StringConstants::Average);
    FloatArrayType::Pointer d2 = FloatArrayType::CreateArray(1, SIMPL::StringConstants::StandardDeviation);
    data.push_back(d1);
    data.push_back(d2);
    d1->setValue(0, avglogdiam);
    d2->setValue(0, sdlogdiam);
    primaryStatsData->setFeatureSizeDistribution(data);
    primaryStatsData->setFeatureSize_DistType(SIMPL::DistributionType::LogNormal);
  }

  return err;
}
