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

#include "StatsGenRDFWidget.h"

//-- C++ Includes
#include <iostream>

//-- Qt Includes
#include <QtConcurrent/QtConcurrentMap>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QAbstractItemDelegate>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <qwt.h>
#include <qwt_abstract_scale_draw.h>
#include <qwt_compat.h>
#include <qwt_interval.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_point_3d.h>
#include <qwt_scale_draw.h>
#include <qwt_series_data.h>
#include <qwt_symbol.h>

#include "SIMPLib/Math/RadialDistributionFunction.h"

#include "StatsGenerator/Widgets/TableModels/SGMDFTableModel.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_StatsGenRDFWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenRDFWidget::StatsGenRDFWidget(QWidget* parent)
: QWidget(parent)
, m_RDFTableModel(nullptr)
{
  this->setupUi(this);
  this->setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenRDFWidget::~StatsGenRDFWidget()
{
  if(nullptr != m_RDFTableModel)
  {
    m_RDFTableModel->deleteLater();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGRDFTableModel* StatsGenRDFWidget::tableModel()
{
  return m_RDFTableModel;
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateTestData(QwtPlot* plotPtr)
{
  QwtPlot& plot = *plotPtr;
  plot.setCanvasBackground(QColor(Qt::white));
  plot.setTitle("Histogram");

  QwtPlotGrid* grid = new QwtPlotGrid;
  grid->enableXMin(true);
  grid->enableYMin(true);
  grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
  grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
  grid->attach(&plot);

  QwtHistogramItem* histogram = new QwtHistogramItem();
  histogram->setColor(Qt::darkCyan);

  const int numValues = 20;

  QwtArray<QwtDoubleInterval> intervals(numValues);
  QwtArray<double> values(numValues);

  double pos = 0.0;
  for ( int i = 0; i < (int)intervals.size(); i++ )
  {
    const int width = 5 + rand() % 15;
    const int value = rand() % 100;

    intervals[i] = QwtDoubleInterval(pos, pos + double(width));
    values[i] = value;

    pos += width;
  }

  histogram->setData(QwtIntervalData(intervals, values));
  histogram->attach(&plot);

  plot.setAxisScale(QwtPlot::yLeft, 0.0, 100.0);
  plot.setAxisScale(QwtPlot::xBottom, 0.0, pos);
  plot.replot();

#if QT_VERSION < 0x040000
  a.setMainWidget(&plot);
#endif

  plot.resize(600, 400);
  plot.show();
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::setupGui()
{
  initQwtPlot("Radial Distribution Plot", "Freq", m_RDFPlot);
  m_RDFTableModel = new SGRDFTableModel;
  m_RDFTableModel->setInitialValues();
  m_RDFTableView->setModel(m_RDFTableModel);
  QAbstractItemDelegate* aid = m_RDFTableModel->getItemDelegate();
  m_RDFTableView->setItemDelegate(aid);
  m_PlotCurve = new QwtPlotCurve;
  // GenerateTestData(m_RDFPlot);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot)
{
  plot->setAxisTitle(QwtPlot::xBottom, xAxisName);
  plot->setAxisTitle(QwtPlot::yLeft, yAxisName);
  plot->setCanvasBackground(Qt::black); // Set the Background colour
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::on_generateRDFBtn_clicked()
{
  QLocale loc = QLocale::system();
  // Generate the ODF Data from the current values in the ODFTableModel
  bool ok = false;

  float minDist = loc.toFloat(minDistLE->text(), &ok);
  float maxDist = loc.toFloat(maxDistLE->text(), &ok);
  int numBins = numBinsLE->text().toInt(&ok);

  std::vector<float> boxDims(3);
  boxDims[0] = loc.toFloat(BoxSizeXLE->text(), &ok);
  boxDims[1] = loc.toFloat(BoxSizeYLE->text(), &ok);
  boxDims[2] = loc.toFloat(BoxSizeZLE->text(), &ok);
  std::vector<float> boxRes(3);
  boxRes[0] = 0.1f;
  boxRes[1] = 0.1f;
  boxRes[2] = 0.1f;

  // Generate the RDF Frequencies
  std::vector<float> rdfFrequencies = RadialDistributionFunction::GenerateRandomDistribution(minDist, maxDist, numBins, boxDims, boxRes);
  QVector<float> qFreq = QVector<float>::fromStdVector(rdfFrequencies);

  // Update the Table model with the latest values
  m_RDFTableModel->setTableData(qFreq);

  // Update the Qwt plot with the correct values
  updateRDFPlot(qFreq);

  emit rdfParametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::on_minDistLE_textChanged(const QString& text)
{
  Q_UNUSED(text)
  minDistLE->setFocus();
  emit rdfParametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::on_maxDistLE_textChanged(const QString& text)
{
  Q_UNUSED(text)
  maxDistLE->setFocus();
  emit rdfParametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::on_BoxSizeXLE_textChanged(const QString& text)
{
  Q_UNUSED(text)
  BoxSizeXLE->setFocus();
  emit rdfParametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::on_BoxSizeYLE_textChanged(const QString& text)
{
  Q_UNUSED(text)
  BoxSizeYLE->setFocus();
  emit rdfParametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::on_BoxSizeZLE_textChanged(const QString& text)
{
  Q_UNUSED(text)
  BoxSizeZLE->setFocus();
  emit rdfParametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::updateRDFPlot(QVector<float>& freqs)
{
  // These are the output vectors
  QwtArray<double> xD(static_cast<int>(freqs.size()));
  QwtArray<double> yD(static_cast<int>(freqs.size()));

  QLocale loc = QLocale::system();

  bool ok = false;
  float minDist = loc.toFloat(minDistLE->text(), &ok);
  float maxDist = loc.toFloat(maxDistLE->text(), &ok);

  const int numValues = freqs.size();
  float increment = (maxDist - minDist) / numValues;

  double pos = minDist;

  for(qint32 i = 0; i < numValues; ++i)
  {
    xD[i] = pos;
    yD[i] = static_cast<double>(freqs.at(i));
    pos = pos + increment;
  }

  // This will actually plot the XY data in the Qwt plot widget
  QwtPlotCurve* curve = m_PlotCurve;
#if QWT_VERSION >= 0x060000
  curve->setSamples(xD, yD);
#else
  curve->setData(xD, yD);
#endif
  curve->setStyle(QwtPlotCurve::Lines);
  // Use Antialiasing to improve plot render quality
  curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
  QPen pen;
  pen.setColor(Qt::white);
  pen.setWidth(2);
  curve->setPen(pen); // Set colour and thickness for drawing the curve
  curve->attach(m_RDFPlot);
  m_RDFPlot->replot();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::updatePlots()
{
  on_generateRDFBtn_clicked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::extractStatsData(int index, StatsData* statsData, unsigned int phaseType)
{
  Q_UNUSED(index)
  if(phaseType == SIMPL::PhaseType::PrecipitatePhase)
  {
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
    //    arrays = pp->getMDF_Weights();
    RdfData::Pointer rdf = pp->getRadialDistFunction();
    if(nullptr != rdf.get())
    {
      float boxDim[3];
      rdf->getBoxSize(boxDim);
      BoxSizeXLE->setText(QString::number(boxDim[0]));
      BoxSizeYLE->setText(QString::number(boxDim[1]));
      BoxSizeZLE->setText(QString::number(boxDim[2]));

      float boxRes[3];
      rdf->getBoxResolution(boxRes);

      minDistLE->setText(QString::number(rdf->getMinDistance()));
      maxDistLE->setText(QString::number(rdf->getMaxDistance()));
      numBinsLE->setText(QString::number(rdf->getNumberOfBins()));

      // Generate a new RDF for these
      on_generateRDFBtn_clicked();
    }
  }

  // Generate the RDF table Plot? which adds value to the Table?
  on_generateRDFBtn_clicked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RdfData::Pointer StatsGenRDFWidget::getStatisticsData()
{
  bool ok = false;
  RdfData::Pointer rdf = RdfData::New();
  QVector<float> qRdfDataFinal(numBinsLE->text().toInt(&ok));
  rdf->setNumberOfBins(numBinsLE->text().toInt());

  QLocale loc = QLocale::system();
  std::vector<float> boxDims(3);
  boxDims[0] = loc.toFloat(BoxSizeXLE->text(), &ok);
  boxDims[1] = loc.toFloat(BoxSizeYLE->text(), &ok);
  boxDims[2] = loc.toFloat(BoxSizeZLE->text(), &ok);
  rdf->setBoxSize(boxDims.data());

  std::vector<float> boxRes(3);
  boxRes[0] = 0.1f;
  boxRes[1] = 0.1f;
  boxRes[2] = 0.1f;
  rdf->setBoxResolution(boxRes.data());

  float minDist = loc.toFloat(minDistLE->text(), &ok);
  float maxDist = loc.toFloat(maxDistLE->text(), &ok);

  rdf->setMinDistance(minDist);
  rdf->setMaxDistance(maxDist);

  // Here we want to take whatever the user entered in and normalize
  // it by what it would look like if a large number of particles were
  // randomly thrown in the same size box.
  // So, for example, if the user chooses a random distribution to start with
  // the actual values they would store would be very close to 1.0, signifying that
  // in each bin, it is about 1.0x above a random distribution.
  // The user will not see the normalized by random values in the StatsGen Widget table,
  // but they will be stored normalized by random in the HDF5 file

  QVector<float> qRdfData = m_RDFTableModel->getData(SGRDFTableModel::Frequency);
  std::vector<float> randomFreq = RadialDistributionFunction::GenerateRandomDistribution(minDistLE->text().toFloat(&ok), maxDistLE->text().toFloat(&ok), numBinsLE->text().toInt(&ok), boxDims, boxRes);

  for(int i = 0; i < qRdfDataFinal.size(); i++)
  {
    if(i + 1 < qRdfData.size() && i + 1 < randomFreq.size())
    {
      qRdfDataFinal[i] = qRdfData[i + 1] / randomFreq[i + 1];
    }
  }

  rdf->setFrequencies(qRdfDataFinal.toStdVector());

  return rdf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsGenRDFWidget::getTabTitle()
{
  return QString("Radial Distribution Function");
}
