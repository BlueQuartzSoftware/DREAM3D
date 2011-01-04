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

#include "StatsGeneratorUI.h"

#include <iostream>
#include <limits>

#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <QtGui/QCloseEvent>

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
#include <qwt_plot_marker.h>

#include "AIM/Common/Qt/QRecentFileList.h"
#include "StatsGen.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGeneratorUI::StatsGeneratorUI(QWidget *parent) :
  QMainWindow(parent),
  m_SizeDistributionCurve(NULL),
  m_SizeDistributionCutoffCurve(NULL),
  m_zoomer(NULL),
  m_picker(NULL),
  m_panner(NULL),
  m_grid(NULL),
#if defined(Q_WS_WIN)
      m_OpenDialogLastDirectory("C:\\")
#else
      m_OpenDialogLastDirectory("~/")
#endif
{
  setupUi(this);
  readSettings();
  setupGui();

  QRecentFileList* recentFileList = QRecentFileList::instance();
  connect(recentFileList, SIGNAL (fileListChanged(const QString &)), this, SLOT(updateRecentFileList(const QString &)));
  // Get out initial Recent File List
  this->updateRecentFileList(QString::null);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGeneratorUI::~StatsGeneratorUI()
{
  // TODO Auto-generated destructor stub
}

// -----------------------------------------------------------------------------
//  Called when the main window is closed.
// -----------------------------------------------------------------------------
void StatsGeneratorUI::closeEvent(QCloseEvent *event)
{
  qint32 err = checkDirtyDocument();
  if (err < 0)
  {
    event->ignore();
  }
  else
  {
    writeSettings();
    event->accept();
  }
}

// -----------------------------------------------------------------------------
//  Read the prefs from the local storage file
// -----------------------------------------------------------------------------
void StatsGeneratorUI::readSettings()
{
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif

}

// -----------------------------------------------------------------------------
//  Write our prefs to file
// -----------------------------------------------------------------------------
void StatsGeneratorUI::writeSettings()
{
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::setupGui()
{

  // Configure the Histogram Plot
  m_SizeDistributionPlot->setCanvasBackground(QColor(Qt::white));
  m_SizeDistributionPlot->setTitle("Size Distribution");
  //  m_HistogramPlot->setAxisTitle(QwtPlot::xBottom, "Gray Scale Value");
  m_grid = new QwtPlotGrid;
  m_grid->enableXMin(true);
  m_grid->enableYMin(true);
  m_grid->setMajPen(QPen(Qt::gray, 0, Qt::SolidLine));
  m_grid->setMinPen(QPen(Qt::lightGray, 0, Qt::DotLine));
  m_grid->attach(m_SizeDistributionPlot);

 plotSizeDistribution();

 // LeftButton for the zooming
 // MidButton for the panning
 // RightButton: zoom out by 1
 // Ctrl+RighButton: zoom out to full size

  m_panner = new QwtPlotPanner(m_SizeDistributionPlot->canvas());
  m_panner->setMouseButton(Qt::MidButton);
#if 0
  QwtPlotZoomer* zoomer = new QwtPlotZoomer(m_SizeDistributionPlot->canvas());
  zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
 // zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);

 // zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
  zoomer->setRubberBand(QwtPicker::EllipseRubberBand);
  zoomer->setRubberBandPen(QColor(Qt::green));
  zoomer->setTrackerMode(QwtPicker::AlwaysOn);
  const QColor c(Qt::red);
//  zoomer->setRubberBandPen(c);
  zoomer->setTrackerPen(c);



  m_zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, m_SizeDistributionPlot->canvas());
  m_zoomer->setRubberBand(QwtPicker::RectRubberBand);
  m_zoomer->setRubberBandPen(QColor(Qt::green));
  m_zoomer->setTrackerMode(QwtPicker::ActiveOnly);
  m_zoomer->setTrackerPen(QColor(Qt::blue));


  m_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                               QwtPicker::PointSelection | QwtPicker::DragSelection,
                               QwtPlotPicker::CrossRubberBand,
                               QwtPicker::AlwaysOn, m_SizeDistributionPlot->canvas());
  m_picker->setRubberBandPen(QColor(Qt::green));
  m_picker->setRubberBand(QwtPicker::CrossRubberBand);
  m_picker->setTrackerPen(QColor(Qt::blue));
#endif

  // Setup Each Plot Widget
  m_Omega3Plot->setPlotTitle(QString("Size Vs. Omega 3"));
  m_Omega3Plot->setXAxisName(QString("Omega 3"));
  m_Omega3Plot->setYAxisName(QString("Frequency"));
  m_Omega3Plot->setCurveType(StatsGen::LogNormal);

  m_BOverAPlot->setPlotTitle(QString("B/A Shape Distribution"));
  m_BOverAPlot->setCurveType(StatsGen::Beta);

  m_COverAPlot->setPlotTitle(QString("C/A Shape Distribution"));
  m_COverAPlot->setCurveType(StatsGen::Beta);

  m_COverBPlot->setPlotTitle(QString("C/B Shape Distribution"));
  m_COverBPlot->setCurveType(StatsGen::Beta);
  m_COverBPlot->setRowOperationEnabled(false);

  m_NeighborPlot->setPlotTitle(QString("Neighbors Distributions"));

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool StatsGeneratorUI::verifyOutputPathParentExists(QString outFilePath, QLineEdit* lineEdit)
{
  QFileInfo fileinfo(outFilePath);
  QDir parent(fileinfo.dir());
  return parent.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool StatsGeneratorUI::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
{
  QFileInfo fileinfo(outFilePath);
  if (false == fileinfo.exists())
  {
    lineEdit->setStyleSheet("border: 1px solid red;");
  }
  else
  {
    lineEdit->setStyleSheet("");
  }
  return fileinfo.exists();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 StatsGeneratorUI::checkDirtyDocument()
{
  qint32 err = -1;
  {
    err = 1;
  }
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::updateRecentFileList(const QString &file)
{
  // std::cout << "IPHelperMainWindow::updateRecentFileList" << std::endl;

  // Clear the Recent Items Menu
  this->menu_RecentFiles->clear();

  // Get the list from the static object
  QStringList files = QRecentFileList::instance()->fileList();
  foreach (QString file, files)
    {
      QAction* action = new QAction(this->menu_RecentFiles);
      action->setText(QRecentFileList::instance()->parentAndFileName(file));
      action->setData(file);
      action->setVisible(true);
      this->menu_RecentFiles->addAction(action);
      connect(action, SIGNAL(triggered()), this, SLOT(openRecentBaseImageFile()));
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::openRecentFile()
{
  //std::cout << "QRecentFileList::openRecentFile()" << std::endl;

  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
  {
    //std::cout << "Opening Recent file: " << action->data().toString().toStdString() << std::endl;
    QString file = action->data().toString();
    openFile( file );
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::openFile(QString imageFile)
{
  if ( true == imageFile.isEmpty() ) // User cancelled the operation
  {
    return;
  }


  // Tell the RecentFileList to update itself then broadcast those changes.
  QRecentFileList::instance()->addFile(imageFile);
  setWidgetListEnabled(true);
  updateRecentFileList(imageFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_m_Mu_SizeDistribution_textChanged(const QString &text)
{
  plotSizeDistribution();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_m_Sigma_SizeDistribution_textChanged(const QString &text)
{
  plotSizeDistribution();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_m_SigmaCutOff_SizeDistribution_textChanged(const QString &text)
{
  plotSizeDistribution();
}




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::plotSizeDistribution()
{
  bool ok = false;
  double mu = 1.0;
  double sigma = 1.0;
  double cutOff = 1.0;
  int err = 0;
  mu = m_Mu_SizeDistribution->text().toDouble(&ok);
  if (ok == false)
  {
    //TODO: Present Error Message
  }
  sigma = m_Sigma_SizeDistribution->text().toDouble(&ok);
  if (ok == false)
  {
    //TODO: Present Error Message
  }
  cutOff = m_SigmaCutOff_SizeDistribution->text().toDouble(&ok);
  if (ok == false)
  {
    //TODO: Present Error Message
  }

  int size = 256;
  QwtArray<double> x;
  QwtArray<double> y;
  StatsGen sg;
  err = sg.GenLogNormal<QwtArray<double> >(mu, sigma, x, y, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return;
  }

  double xMax = std::numeric_limits<double>::min();
  double yMax = std::numeric_limits<double>::min();
  for (int i = 0; i < size; ++i)
  {
 //   std::cout << x[i] << "  " << y[i] << std::endl;
    if (x[i] > xMax) { xMax = x[i]; }
    if (y[i] > yMax) { yMax = y[i]; }
  }

  QwtArray<double> xCo;
  QwtArray<double> yCo;
  int numsizebins = 1;
  QwtArray<double> binsizes;
  err = sg.GenCutOff<QwtArray<double> >(mu, sigma, cutOff, xCo, yCo, yMax, numsizebins, binsizes);

//  for (int i = 0; i < 6; ++i )
//  {
//    std::cout << xCo[i] << " "<< yCo[i] << std::endl;
//  }

  if (NULL == m_SizeDistributionCurve)
  {
    m_SizeDistributionCurve = new QwtPlotCurve("Size Distribution");
    m_SizeDistributionCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    m_SizeDistributionCurve->setPen(QPen(Qt::red));
    m_SizeDistributionCurve->attach(m_SizeDistributionPlot);
  }

  // QwtPlotMarker* marker = new QwtPlotMarker;
  if (NULL == m_SizeDistributionCutoffCurve)
  {
    m_SizeDistributionCutoffCurve = new QwtPlotCurve("Cut Off Value");
    m_SizeDistributionCutoffCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    m_SizeDistributionCutoffCurve->setPen(QPen(Qt::blue));
    m_SizeDistributionCutoffCurve->attach(m_SizeDistributionPlot);
  }

  m_SizeDistributionCurve->setData(x, y);
  m_SizeDistributionCutoffCurve->setData(xCo, yCo);

  m_SizeDistributionPlot->setAxisScale(QwtPlot::yLeft, 0.0, yMax);
  m_SizeDistributionPlot->setAxisScale(QwtPlot::xBottom, 0.0, xMax);
  m_SizeDistributionPlot->replot();
}
