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
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>

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

#include "AIM/Common/AIMRepresentationVersion.h"
#include "AIM/Common/Qt/ApplicationAboutBoxDialog.h"
#include "AIM/Common/Qt/QRecentFileList.h"
#include "AIM/Common/HDF5/H5ReconStatsWriter.h"
#include "AIM/License/AIMRepresentationLicenseFiles.h"
#include "StatsGen.h"
#include "SGApplication.h"
#include "EditPhaseDialog.h"


#define CHECK_ERROR_ON_WRITE(var, msg)\
    if (err < 0) {\
      QMessageBox::critical(this, tr("AIM Representation"),\
      tr("There was an error writing the " msg " to the HDF5 file"),\
      QMessageBox::Ok,\
      QMessageBox::Ok);\
      return;\
      }


#define CHECK_STATS_READ_ERROR(err, group, dataset)\
if (err < 0) {\
  std::cout << "StatsGeneratorUI::on_actionOpen_triggered Error: Could not read '" << group << "' data set '" << dataset << "'" << std::endl;\
  std::cout << "  File: " << __FILE__ << std::endl;\
  std::cout << "  Line: " << __LINE__ << std::endl;\
  return;\
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGeneratorUI::StatsGeneratorUI(QWidget *parent) :
  QMainWindow(parent), m_SizeDistributionCurve(NULL), m_CutOffMin(NULL), m_CutOffMax(NULL),
  // m_zoomer(NULL),
      //  m_picker(NULL),
      //  m_panner(NULL),
      m_grid(NULL),
      m_FileSelected(false),
#if defined(Q_WS_WIN)
      m_OpenDialogLastDirectory("C:")
#else
      m_OpenDialogLastDirectory("~/Desktop")
#endif
{
  m_FilePath = m_OpenDialogLastDirectory + QDir::separator() + "Untitled.h5";
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_actionExit_triggered()
{
  this->close();
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
    emit windowIsClosing(this);
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
  distributionTypeCombo->addItem(AIM::HDF5::BetaDistribution.c_str());
  distributionTypeCombo->addItem(AIM::HDF5::LogNormalDistribution.c_str());
  distributionTypeCombo->addItem(AIM::HDF5::PowerLawDistribution.c_str());
  distributionTypeCombo->setCurrentIndex(AIM::Reconstruction::LogNormal);
  // Turn off all the plot widgets
  setTabsPlotTabsEnabled(false);

  // Place the name of the file in the window title
  adjustWindowTitle();


  // LeftButton for the zooming
  // MidButton for the panning
  // RightButton: zoom out by 1
  // Ctrl+RighButton: zoom out to full size
#if 0
  m_panner = new QwtPlotPanner(m_SizeDistributionPlot->canvas());
  m_panner->setMouseButton(Qt::MidButton);

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
  m_Omega3Plot->setDistributionType(AIM::Reconstruction::Beta);
  m_Omega3Plot->setStatisticsType(AIM::Reconstruction::Grain_SizeVOmega3);
  m_Omega3Plot->blockDistributionTypeChanges(true);
  m_Omega3Plot->setRowOperationEnabled(false);
  setSizeDistributionValues(m_Omega3Plot);
  connect(m_Omega3Plot, SIGNAL(userEditedData()),
          this, SLOT(dataWasEdited()));

  m_BOverAPlot->setPlotTitle(QString("B/A Shape Distribution"));
  m_BOverAPlot->setXAxisName(QString("B/A"));
  m_BOverAPlot->setYAxisName(QString("Frequency"));
  m_BOverAPlot->setDistributionType(AIM::Reconstruction::Beta);
  m_BOverAPlot->setStatisticsType(AIM::Reconstruction::Grain_SizeVBoverA);
  m_BOverAPlot->blockDistributionTypeChanges(true);
  m_BOverAPlot->setRowOperationEnabled(false);
  setSizeDistributionValues(m_BOverAPlot);
  connect(m_BOverAPlot, SIGNAL(userEditedData()),
          this, SLOT(dataWasEdited()));


  m_COverAPlot->setPlotTitle(QString("C/A Shape Distribution"));
  m_COverAPlot->setXAxisName(QString("C/A"));
  m_COverAPlot->setYAxisName(QString("Frequency"));
  m_COverAPlot->setDistributionType(AIM::Reconstruction::Beta);
  m_COverAPlot->setStatisticsType(AIM::Reconstruction::Grain_SizeVCoverA);
  m_COverAPlot->blockDistributionTypeChanges(true);
  m_COverAPlot->setRowOperationEnabled(false);
  setSizeDistributionValues(m_COverAPlot);
  connect(m_COverAPlot, SIGNAL(userEditedData()),
          this, SLOT(dataWasEdited()));

  m_COverBPlot->setPlotTitle(QString("C/B Shape Distribution"));
  m_COverBPlot->setXAxisName(QString("C/B"));
  m_COverBPlot->setYAxisName(QString("Frequency"));
  m_COverBPlot->setDistributionType(AIM::Reconstruction::Beta);
  m_COverBPlot->setStatisticsType(AIM::Reconstruction::Grain_SizeVCoverB);
  m_COverBPlot->blockDistributionTypeChanges(true);
  m_COverBPlot->setRowOperationEnabled(false);
  setSizeDistributionValues(m_COverBPlot);
  connect(m_COverBPlot, SIGNAL(userEditedData()),
          this, SLOT(dataWasEdited()));

  m_NeighborPlot->setPlotTitle(QString("Neighbors Distributions"));
  m_NeighborPlot->setXAxisName(QString("Distance (Multiples of Diameter)"));
  m_NeighborPlot->setYAxisName(QString("Number of Grains"));
  m_NeighborPlot->setDistributionType(AIM::Reconstruction::Power);
  m_NeighborPlot->setStatisticsType(AIM::Reconstruction::Grain_SizeVNeighbors);
  m_NeighborPlot->blockDistributionTypeChanges(true);
  m_NeighborPlot->setRowOperationEnabled(false);
  setSizeDistributionValues(m_NeighborPlot);
  connect(m_NeighborPlot, SIGNAL(userEditedData()),
          this, SLOT(dataWasEdited()));

  m_SizeDistributionPlot->setCanvasBackground(QColor(Qt::white));
  m_SizeDistributionPlot->setTitle("Size Distribution");

  m_grid = new QwtPlotGrid;
  m_grid->enableXMin(true);
  m_grid->enableYMin(true);
  m_grid->setMajPen(QPen(Qt::gray, 0, Qt::SolidLine));
  m_grid->setMinPen(QPen(Qt::lightGray, 0, Qt::DotLine));
  m_grid->attach(m_SizeDistributionPlot);

  updateSizeDistributionPlot();
  calculateNumberOfBins();

  /* Setup the default Phase as index 0 and Cubic Structure */
  bool ok = false;
  m_CurrentPhase = SGPhase::New(0, AIM::Reconstruction::Cubic);
  m_CurrentPhase->setMu(m_Mu_SizeDistribution->text().toDouble(&ok));
  m_CurrentPhase->setSigma(m_Sigma_SizeDistribution->text().toDouble(&ok));
  m_CurrentPhase->setSigmaCutoff(m_SigmaCutOff_SizeDistribution->text().toDouble(&ok));
  m_CurrentPhase->setBinStepSize(m_BinStepSize->text().toDouble(&ok));
  m_CurrentPhase->setDistributionType(static_cast<AIM::Reconstruction::DistributionType>(distributionTypeCombo->currentIndex()));
  m_CurrentPhase->setOmega3Plot(m_Omega3Plot);
  m_CurrentPhase->setBOverAPlot(m_BOverAPlot);
  m_CurrentPhase->setCOverAPlot(m_COverAPlot);
  m_CurrentPhase->setCOverBPlot(m_COverBPlot);
  m_CurrentPhase->setNeighborPlot(m_NeighborPlot);
  m_CurrentPhase->setODFWidget(m_ODFWidget);

  m_Phases.push_back(m_CurrentPhase);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_phaseCombo_currentIndexChanged(int index)
{
  std::cout << "on_phaseCombo_currentIndexChanged" << std::endl;
  SGPhase::Pointer phase = m_Phases[index];
  swapPhaseWidgets(phase);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_addPhase_clicked()
{
  std::cout << "on_addPhase_clicked" << std::endl;
  SGPhase::Pointer phase = SGPhase::New( m_Phases.size() , AIM::Reconstruction::Cubic);

  //Display Dialog box asking for the crystal structure
  EditPhaseDialog dialog(this);
  int ok = dialog.exec();
  if (ok == QDialog::Accepted)
  {
    m_Phases.push_back(phase);
    phase->setCrystalStructure(dialog.getCrystalStructure());
    phase->setMu(1.0);
    phase->setSigma(0.1);
    phase->setSigmaCutoff(5);
    phase->setBinStepSize(0.5);
    phase->setDistributionType(AIM::Reconstruction::LogNormal);
    distributionTypeCombo->setCurrentIndex(AIM::Reconstruction::LogNormal);
    // Create all the widgets for the User Interface
    StatsGenPlotWidget* w = new StatsGenPlotWidget(omega3DistributionTab);
    phase->setOmega3Plot(w);
    w =  new StatsGenPlotWidget(scrollAreaWidgetContents);
    phase->setBOverAPlot(w);
    w = new StatsGenPlotWidget(scrollAreaWidgetContents);
    phase->setCOverAPlot(w);
    w = new StatsGenPlotWidget(scrollAreaWidgetContents);
    phase->setCOverBPlot(w);
    w = new StatsGenPlotWidget(neighborDistributionTab);
    phase->setNeighborPlot(w);
    StatsGenODFWidget* o = new StatsGenODFWidget(tab);
    phase->setODFWidget(o);

    phaseCombo->addItem(phase->getComboString());

    // Now swap out our new widgets with those currently being displayed
    swapPhaseWidgets(phase);
 }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_editPhase_clicked()
{
  std::cout << "on_editPhase_clicked" << std::endl;
  //Display Dialog box asking for the crystal structure
  EditPhaseDialog dialog(this);
  int ok = dialog.exec();
  if (ok == QDialog::Accepted)
  {
    m_CurrentPhase->setCrystalStructure(dialog.getCrystalStructure());
#warning Do we need to recalculate anything at this point if the phase changes?
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_deletePhase_clicked()
{

  std::cout << "on_deletePhase_clicked" << std::endl;
  if (m_Phases.size() > 1)
  {
    int index = phaseCombo->currentIndex();
    phaseCombo->removeItem(index);
    std::vector<SGPhase::Pointer>::iterator iter = m_Phases.begin();
    iter = iter + index;
    SGPhase::Pointer delPhase = m_Phases[index];
    // Remove the SGPhase object from the vector
    m_Phases.erase(iter);

    // Reset the phase index for each SGPhase object
    for (std::vector<SGPhase::Pointer>::size_type p = 0; p < m_Phases.size(); ++p )
    {
      m_Phases[p]->setIndex(p);
    }
    // Get the first phase to use as the current phase
    SGPhase::Pointer phase = m_Phases[0];
    // Swap out the widgets
    swapPhaseWidgets(phase);
    // And now clean up/release any extra widgets that are being used
    delPhase->deleteWidgets();
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::swapPhaseWidgets(SGPhase::Pointer phase)
{
  verticalLayout_4->removeWidget(m_Omega3Plot);
  m_Omega3Plot = phase->getOmega3Plot();
  verticalLayout_4->addWidget(m_Omega3Plot);

  verticalLayout->removeWidget(m_BOverAPlot);
  verticalLayout->removeWidget(m_COverAPlot);
  verticalLayout->removeWidget(m_COverBPlot);

  m_BOverAPlot = phase->getBOverAPlot();
  verticalLayout->addWidget(m_BOverAPlot);

  m_COverAPlot = phase->getCOverBPlot();
  verticalLayout->addWidget(m_COverAPlot);

  m_COverBPlot = phase->getCOverBPlot();
  verticalLayout->addWidget(m_COverBPlot);

  horizontalLayout_2->removeWidget(m_NeighborPlot);
  m_NeighborPlot = phase->getNeighborPlot();
  horizontalLayout_2->addWidget(m_NeighborPlot);

  verticalLayout_6->removeWidget(m_ODFWidget);
  m_ODFWidget = phase->getODFWidget();
  verticalLayout_6->addWidget(m_ODFWidget);

  m_CurrentPhase = phase;

  //Do these swaps LAST because we have to get the other widgets swapped out first other
  // wise we screw up those widgets
  m_Mu_SizeDistribution->setText( QString::number(m_CurrentPhase->getMu()));
  m_Sigma_SizeDistribution->setText( QString::number(m_CurrentPhase->getSigma()));
  m_SigmaCutOff_SizeDistribution->setText( QString::number(m_CurrentPhase->getSigmaCutoff()));
  m_BinStepSize->setValue(m_CurrentPhase->getBinStepSize());
  distributionTypeCombo->setCurrentIndex(m_CurrentPhase->getDistributionType());

  updateSizeDistributionPlot();
  calculateNumberOfBins();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::setSizeDistributionValues(StatsGenPlotWidget* w)
{
  bool ok = false;
  double mu = 1.0;
  double sigma = 1.0;
  double cutOff = 1.0;
  double stepSize = 1.0;
  mu = m_Mu_SizeDistribution->text().toDouble(&ok);
  if (ok == false)
  {
    return;
  }
  sigma = m_Sigma_SizeDistribution->text().toDouble(&ok);
  if (ok == false)
  {
    return;
  }
  cutOff = m_SigmaCutOff_SizeDistribution->text().toDouble(&ok);
  if (ok == false)
  {
    return;
  }
  stepSize = m_BinStepSize->text().toDouble(&ok);
  if (ok == false)
  {
    return;
  }
  w->setMu(mu);
  w->setSigma(sigma);
  w->setCutoff(cutOff);
  w->setBinStep(stepSize);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 StatsGeneratorUI::checkDirtyDocument()
{
  qint32 err = -1;

  if (isWindowModified() == true)
  {
    int r = QMessageBox::warning(this, tr("StatsGenerator"), tr("The Data has been modified.\nDo you want to save your changes?"), QMessageBox::Save
        | QMessageBox::Default, QMessageBox::Discard, QMessageBox::Cancel | QMessageBox::Escape);
    if (r == QMessageBox::Save)
    {
      on_actionSave_triggered(); // Save the hdf5 file
      err = 1;
    }
    else if (r == QMessageBox::Discard)
    {
      err = 1;
    }
    else if (r == QMessageBox::Cancel)
    {
      err = -1;
    }
  }
  else
  {
    err = 1;
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_actionClose_triggered()
{
  // std::cout << "RepresentationUI::on_actionClose_triggered" << std::endl;
  qint32 err = -1;
  err = checkDirtyDocument();
  if (err >= 0)
  {
    // Close the window. Files have been saved if needed
    if (QApplication::activeWindow() == this)
    {
      this->close();
    }
    else
    {
      QApplication::activeWindow()->close();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::setTabsPlotTabsEnabled(bool b)
{
  qint32 count = this->tabWidget->count();
  for (qint32 i = 1; i < count; ++i)
  {
    this->tabWidget->setTabEnabled(i, b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::dataWasEdited()
{
  setTabsPlotTabsEnabled(true);
  this->tabWidget->setTabEnabled(0, false);
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
      connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::openRecentFile()
{
  //std::cout << "QRecentFileList::openRecentFile()" << std::endl;

  QAction *action = qobject_cast<QAction * > (sender());
  if (action)
  {
    //std::cout << "Opening Recent file: " << action->data().toString().toStdString() << std::endl;
    QString file = action->data().toString();
    openFile(file);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_actionSaveAs_triggered()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "Untitled.h5";
  QString h5file = QFileDialog::getSaveFileName(this, tr("Save HDF5 Statistics File"),
    proposedFile,
    tr("HDF5 Files (*.h5)") );
  if ( true == h5file.isEmpty() ){ return;  }
  m_FilePath = h5file;
  QFileInfo fi (m_FilePath);
  QString ext = fi.suffix();
  m_OpenDialogLastDirectory = fi.path();
  m_FileSelected = true;
  on_actionSave_triggered();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_actionSave_triggered()
{
  //std::cout << "StatsGeneratorUI::on_actionSave_triggered()" << std::endl;
  if (m_FileSelected == false)
  {
    //QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + m_FileName;
    QString h5file = QFileDialog::getSaveFileName(this, tr("Save HDF5 Statistics File"),
                                                  m_FilePath,
                                                   tr("HDF5 Files (*.h5)") );
    if ( true == h5file.isEmpty() ){ return;  }
    m_FilePath = h5file;
    QFileInfo fi (m_FilePath);
    QString ext = fi.suffix();
    m_OpenDialogLastDirectory = fi.path();
    m_FileSelected = true;
  }


  // Verify that the Mu, Sigma, Cutoff, BinStep are all set correctly by getting the
  // values from one of the PlotWidgets
  double mu, sigma, cutOff, binStep;
  mu = m_Omega3Plot->getMu();
  sigma = m_Omega3Plot->getSigma();
  cutOff = m_Omega3Plot->getCutoff();
  binStep = m_Omega3Plot->getBinStep();
  m_Sigma_SizeDistribution->setText(QString::number(sigma));
  m_Mu_SizeDistribution->setText(QString::number(mu));
  m_SigmaCutOff_SizeDistribution->setText(QString::number(cutOff));
  m_BinStepSize->setValue(binStep);



  H5ReconStatsWriter::Pointer writer = H5ReconStatsWriter::New(m_FilePath.toStdString());

  QwtArray<double> xCo;
  QwtArray<double> yCo;
  QwtArray<double> binsizes;
  double xMax = std::numeric_limits<double >::min();
  double yMax = std::numeric_limits<double >::min();
  QwtArray<double> x;
  QwtArray<double> y;
  int err = computeBinsAndCutOffs(binsizes, xCo, yCo, xMax, yMax, x, y);
  if (err < 0) { return; }


  // We need to compute the Max and Min Diameter Bin Values
  double mindiameter = xCo[0];
  double maxdiameter = xCo[1];

 // std::cout << "maxdiameter: " << maxdiameter << std::endl;
 // std::cout << "mindiameter: " << mindiameter << std::endl;
  bool ok = false;
  double avglogdiam = m_Mu_SizeDistribution->text().toDouble(&ok);
  double sdlogdiam = m_Sigma_SizeDistribution->text().toDouble(&ok);
  double stepSize = m_BinStepSize->value();

  size_t nBins = 0;
  err = writer->writeSizeDistribution(maxdiameter, mindiameter, stepSize, avglogdiam, sdlogdiam, nBins);
  CHECK_ERROR_ON_WRITE(err, "Size Distribution")

  // Now that we have bins and grain sizes, push those to the other plot widgets
  // Setup Each Plot Widget
  m_Omega3Plot->writeDataToHDF5(writer, AIM::HDF5::Grain_SizeVOmega3_Distributions);

  m_BOverAPlot->writeDataToHDF5(writer, AIM::HDF5::Grain_SizeVBoverA_Distributions);

  m_COverAPlot->writeDataToHDF5(writer, AIM::HDF5::Grain_SizeVCoverA_Distributions);

  m_COverBPlot->writeDataToHDF5(writer, AIM::HDF5::Grain_SizeVCoverB_Distributions);

  m_NeighborPlot->writeDataToHDF5(writer, AIM::HDF5::Grain_SizeVNeighbors_Distributions);

  m_ODFWidget->writeDataToHDF5(AIM::Reconstruction::Cubic, writer);

  // Force the clean up of the writer by assigning a NULL pointer which will
  // have the effect of executing the destructor of the H5ReconStatsWriter Class
  writer = H5ReconStatsWriter::NullPointer();

  setWindowTitle(m_FilePath + " - StatsGenerator");
  setWindowModified(false);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_actionNew_triggered()
{

  SGApplication* app = qobject_cast<SGApplication*>(SGApplication::instance());
  // Create a new Window
  StatsGeneratorUI* window = app->createNewStatsGenerator();

  // Get a unique File name
  QString filePath = app->newTempFile(m_OpenDialogLastDirectory);
  window->setFilePath(filePath);

  // Offset the window a bit from the current window
  QRect geometry = this->geometry();
  geometry.setX(geometry.x() + 25);
  geometry.setY(geometry.y() + 25);
  window->setGeometry(geometry);

  // Show the window
  window->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_actionOpen_triggered()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "Untitled.h5";
  QString h5file = QFileDialog::getOpenFileName(this, tr("Open HDF5 Statistics File"),
    proposedFile,
    tr("HDF5 Files (*.h5)") );
  if ( true == h5file.isEmpty() ){ return;  }

  SGApplication* app = qobject_cast<SGApplication*>(SGApplication::instance());
  // Create a new Window
  StatsGeneratorUI* window = app->createNewStatsGenerator();
  window->openFile(h5file);

  // Offset the window a bit from the current window
  QRect geometry = this->geometry();
  geometry.setX(geometry.x() + 25);
  geometry.setY(geometry.y() + 25);
  window->setGeometry(geometry);

  app->checkWindowPosition(window);

  // Show the window
  window->show();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::openFile(QString h5file)
{

  if (true == h5file.isEmpty()) // User cancelled the operation
  {
    return;
  }

  // Tell the RecentFileList to update itself then broadcast those changes.
  QRecentFileList::instance()->addFile(h5file);
  setWidgetListEnabled(true);
  updateRecentFileList(h5file);


  m_FilePath = h5file;
  QFileInfo fi (m_FilePath);
  QString ext = fi.suffix();
  m_OpenDialogLastDirectory = fi.path();
  m_FileSelected = true;

  std::vector<double> grainDiamInfo;
  std::vector<double> double_data;
  std::string path;
  double mu, sigma, binStepSize, xMax, yMax, cutoff;


  int err = 0;

  H5ReconStatsReader::Pointer reader = H5ReconStatsReader::New(m_FilePath.toStdString());


  // Get the list of Phases from the HDF5 file
  std::vector<int> phases = reader->getPhases();

  // We should iterate on all the phases here to start setting data and creating
  // all of the StatsGenPhase Objects
  for (std::vector<int>::iterator phase = phases.begin(); phase != phases.end(); ++phase )
  {

  }

  /* Read the BinNumbers data set */
  std::vector<double> bins;
  err = reader->readStatsDataset(AIM::HDF5::BinNumber, bins);
  CHECK_STATS_READ_ERROR(err, AIM::HDF5::Reconstruction, AIM::HDF5::BinNumber)

  /* Read the Grain_Diameter_Info Data */
  err = reader->readStatsDataset(AIM::HDF5::Grain_Diameter_Info, grainDiamInfo);
  CHECK_STATS_READ_ERROR(err,  AIM::HDF5::Reconstruction, AIM::HDF5::Grain_Diameter_Info)

  binStepSize = grainDiamInfo[0];
  m_BinStepSize->setValue(grainDiamInfo[0]);

  /* Read the Grain_Size_Distribution Data */
  err = reader->readStatsDataset(AIM::HDF5::Grain_Size_Distribution, double_data);
  CHECK_STATS_READ_ERROR(err,  AIM::HDF5::Reconstruction, AIM::HDF5::Grain_Size_Distribution)
  m_Mu_SizeDistribution->setText(QString::number(double_data[0]));
  m_Sigma_SizeDistribution->setText(QString::number(double_data[1]));
  mu = double_data[0];
  sigma = double_data[1];

  int nBins = (int)(bins.size());

  StatsGen sg;
  int computedNBins = 0;
  cutoff = 0.0;
  while (computedNBins != nBins)
  {
    computedNBins = sg.computeNumberOfBins(mu, sigma, cutoff, binStepSize, xMax, yMax);
//    std::cout << "nBins: " << nBins << std::endl;
//    std::cout << "computedNBins: " << computedNBins << std::endl;
//    std::cout << "cutoff: " << cutoff << std::endl;
    cutoff++;
  }
  m_SigmaCutOff_SizeDistribution->setText(QString::number(cutoff-1.0));


  QVector<double> qbins = QVector<double>::fromStdVector(bins);

  m_Omega3Plot->readDataFromHDF5(reader, qbins, AIM::HDF5::Grain_SizeVOmega3_Distributions);
  setSizeDistributionValues(m_Omega3Plot);

  m_BOverAPlot->readDataFromHDF5(reader, qbins, AIM::HDF5::Grain_SizeVBoverA_Distributions);
  setSizeDistributionValues(m_BOverAPlot);

  m_COverAPlot->readDataFromHDF5(reader, qbins, AIM::HDF5::Grain_SizeVCoverA_Distributions);
  setSizeDistributionValues(m_COverAPlot);

  m_COverBPlot->readDataFromHDF5(reader, qbins, AIM::HDF5::Grain_SizeVCoverB_Distributions);
  setSizeDistributionValues(m_COverBPlot);

  m_NeighborPlot->readDataFromHDF5(reader, qbins, AIM::HDF5::Grain_SizeVNeighbors_Distributions);
  setSizeDistributionValues(m_NeighborPlot);

  // Enable all the tabs
  setTabsPlotTabsEnabled(true);

  // Set the window title correctly
  setWindowModified(false);
  QString windowTitle = QString("");
  windowTitle = windowTitle +  m_FilePath + QString(" - StatsGenerator");
  setWindowTitle(windowTitle);

  dataWasEdited();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_m_GenerateDefaultData_clicked()
{
  plotSizeDistribution();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_m_Mu_SizeDistribution_textChanged(const QString &text)
{
  updateSizeDistributionPlot();
  m_Mu_SizeDistribution->setFocus();
  calculateNumberOfBins();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_m_Sigma_SizeDistribution_textChanged(const QString &text)
{
  updateSizeDistributionPlot();
  m_Sigma_SizeDistribution->setFocus();
  calculateNumberOfBins();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_m_SigmaCutOff_SizeDistribution_textChanged(const QString &text)
{
  updateSizeDistributionPlot();
  m_SigmaCutOff_SizeDistribution->setFocus();
  calculateNumberOfBins();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_m_BinStepSize_valueChanged(double v)
{
  calculateNumberOfBins();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::calculateNumberOfBins()
{
  bool ok = false;
  double mu = 1.0;
  double sigma = 1.0;
  double cutOff = 1.0;
  double stepSize = 1.0;
  double max, min; // Only needed for the method. Not used otherwise.
  mu = m_Mu_SizeDistribution->text().toDouble(&ok);
  if (ok == false)
  {
    return;
  }
  sigma = m_Sigma_SizeDistribution->text().toDouble(&ok);
  if (ok == false)
  {
    return;
  }
  cutOff = m_SigmaCutOff_SizeDistribution->text().toDouble(&ok);
  if (ok == false)
  {
    return;
  }
  stepSize = m_BinStepSize->text().toDouble(&ok);
  if (ok == false)
  {
    return;
  }
  StatsGen sg;
  int n = sg.computeNumberOfBins(mu, sigma, cutOff, stepSize, max, min);
  m_NumberBinsGenerated->setText(QString::number(n));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StatsGeneratorUI::computeBinsAndCutOffs( QwtArray<double> &binsizes, QwtArray<double> &xCo, QwtArray<double> &yCo,
                                             double &xMax, double &yMax,
                                             QwtArray<double> &x, QwtArray<double> &y)
{
  bool ok = false;
  double mu = 1.0;
  double sigma = 1.0;
  double cutOff = 1.0;
  double binStepSize = 1.0;
  int err = 0;
  mu = m_Mu_SizeDistribution->text().toDouble(&ok);
  if (ok == false)
  {
    return -1;
  }
  sigma = m_Sigma_SizeDistribution->text().toDouble(&ok);
  if (ok == false)
  {
    return -1;
  }
  cutOff = m_SigmaCutOff_SizeDistribution->text().toDouble(&ok);
  if (ok == false)
  {
    return -1;
  }
  binStepSize = m_BinStepSize->value();
  if (ok == false)
  {
    return -1;
  }

  int size = 100;

  StatsGen sg;
  err = sg.GenLogNormal<QwtArray<double> > (mu, sigma, x, y, size);
  if (err == 1)
  {
    //TODO: Present Error Message
    return -1;
  }

//  double xMax = std::numeric_limits<double >::min();
//  double yMax = std::numeric_limits<double >::min();
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

  xCo.clear();
  yCo.clear();
  int numsizebins = 1;
  binsizes.clear();
  // QwtArray<int> numgrains;
  err = sg.GenCutOff<double, QwtArray<double> > (mu, sigma, cutOff, binStepSize, xCo, yCo, yMax, numsizebins, binsizes);

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::adjustWindowTitle()
{
  setWindowModified(true);

  QString windowTitle("*");
#ifdef Q_WS_MAC
  windowTitle = QString("");
#endif
  windowTitle = windowTitle +  m_FilePath + QString(" - StatsGenerator");
  setWindowTitle(windowTitle);
  setWindowModified(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::updateSizeDistributionPlot()
{
  adjustWindowTitle();
  QwtArray<double> xCo;
  QwtArray<double> yCo;
  QwtArray<double> binsizes;
  double xMax = std::numeric_limits<double >::min();
  double yMax = std::numeric_limits<double >::min();
  QwtArray<double> x;
  QwtArray<double> y;
  int err = computeBinsAndCutOffs(binsizes, xCo, yCo, xMax, yMax, x, y);
  if (err < 0) { return; }

  if (NULL == m_SizeDistributionCurve)
  {
    m_SizeDistributionCurve = new QwtPlotCurve("Size Distribution");
    m_SizeDistributionCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    m_SizeDistributionCurve->setPen(QPen(Qt::red));
    m_SizeDistributionCurve->attach(m_SizeDistributionPlot);
  }

  // Place a vertical Line on the plot where the Min and Max Cutoff values are
  if (NULL == m_CutOffMin)
  {
    m_CutOffMin = new QwtPlotMarker();
    m_CutOffMin->attach(m_SizeDistributionPlot);
  }
  m_CutOffMin->setLabel(QString::fromLatin1("Cut Off Min Grain Diameter"));
  m_CutOffMin->setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
  m_CutOffMin->setLabelOrientation(Qt::Vertical);
  m_CutOffMin->setLineStyle(QwtPlotMarker::VLine);
  m_CutOffMin->setLinePen(QPen(Qt::blue, 1, Qt::SolidLine));
  m_CutOffMin->setXValue(xCo[0]);

  if (NULL == m_CutOffMax)
  {
    m_CutOffMax = new QwtPlotMarker();
    m_CutOffMax->attach(m_SizeDistributionPlot);
  }
  m_CutOffMax->setLabel(QString::fromLatin1("Cut Off Max Grain Diameter"));
  m_CutOffMax->setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
  m_CutOffMax->setLabelOrientation(Qt::Vertical);
  m_CutOffMax->setLineStyle(QwtPlotMarker::VLine);
  m_CutOffMax->setLinePen(QPen(Qt::blue, 1, Qt::SolidLine));
  m_CutOffMax->setXValue(xCo[1]);

  m_SizeDistributionCurve->setData(x, y);
//  std::cout << "----------------" << std::endl;
//  std::cout << "yMax: " << yMax << std::endl;
//  std::cout << "xMax: " << xMax << std::endl;

  m_SizeDistributionPlot->setAxisScale(QwtPlot::xBottom, xCo[0] - (xCo[0] * 0.1), xMax * 1.10);
  m_SizeDistributionPlot->setAxisScale(QwtPlot::yLeft, 0.0, yMax);

  m_SizeDistributionPlot->replot();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::plotSizeDistribution()
{
  // We have valid data so enable the other plot tabs
  setTabsPlotTabsEnabled(true);

  adjustWindowTitle();
  QwtArray<double> xCo;
  QwtArray<double> yCo;
  QwtArray<double> binsizes;
  double xMax = std::numeric_limits<double >::min();
  double yMax = std::numeric_limits<double >::min();
  QwtArray<double> x;
  QwtArray<double> y;
  int err = computeBinsAndCutOffs(binsizes, xCo, yCo, xMax, yMax, x, y);
  if (err < 0) { return; }


  // Now that we have bins and grain sizes, push those to the other plot widgets
  // Setup Each Plot Widget
  setSizeDistributionValues(m_Omega3Plot);
  m_Omega3Plot->setBins(binsizes);

  setSizeDistributionValues(m_BOverAPlot);
  m_BOverAPlot->setBins(binsizes);

  setSizeDistributionValues(m_COverAPlot);
  m_COverAPlot->setBins(binsizes);

  setSizeDistributionValues(m_COverBPlot);
  m_COverBPlot->setBins(binsizes);

  setSizeDistributionValues(m_NeighborPlot);
  m_NeighborPlot->setBins(binsizes);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_actionAbout_triggered()
{
  ApplicationAboutBoxDialog about(AIMRepresentation::LicenseList, this);
  QString an = QCoreApplication::applicationName();
  QString version("");
  version.append(AIMRepresentation::Version::PackageComplete.c_str());
  about.setApplicationInfo(an, version);
  about.exec();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::setFilePath(QString filePath)
{
  this->m_FilePath = filePath;
  adjustWindowTitle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString StatsGeneratorUI::getFilePath()
{
  return m_FilePath;
}
