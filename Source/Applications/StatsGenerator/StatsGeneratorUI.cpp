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

#include "StatsGeneratorUI.h"


#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>

#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/Common/DataContainer.h"
#include "DREAM3DLib/Common/StatsDataArray.h"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/IOFilters/DataContainerWriter.h"
#include "DREAM3DLib/IOFilters/DataContainerReader.h"


#include "QtSupport/ApplicationAboutBoxDialog.h"
#include "QtSupport/QRecentFileList.h"
#include "QtSupport/HelpDialog.h"

#include "DREAM3D/License/StatsGeneratorLicenseFiles.h"

#include "SGApplication.h"
#include "EditPhaseDialog.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGeneratorUI::StatsGeneratorUI(QWidget *parent) :
  QMainWindow(parent),
  m_FileSelected(false),
  m_HelpDialog(NULL),
#if defined(Q_WS_WIN)
      m_OpenDialogLastDirectory("C:")
#else
      m_OpenDialogLastDirectory("~/Desktop")
#endif
{
  m_FilePath = m_OpenDialogLastDirectory + QDir::separator() + "Untitled.h5stats";
  setupUi(this);
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
  m_SGWidget->setPhaseIndex(1);
  m_SGWidget->setCrystalStructure(Ebsd::CrystalStructure::Cubic);
  m_SGWidget->setPhaseFraction(1.0);
  m_SGWidget->setTotalPhaseFraction(1.0);
  m_SGWidgets.push_back(m_SGWidget);
  QString cName = m_SGWidget->getComboString();
  m_SGWidget->setObjectName(cName);
  phaseCombo->blockSignals(true);
  phaseCombo->addItem(cName);
  phaseCombo->blockSignals(false);
  setWindowTitle("[*] - Untitled.h5stats");
  setWindowModified(true);

  m_HelpDialog = new HelpDialog(this);
  m_HelpDialog->setWindowModality(Qt::NonModal);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_phaseCombo_currentIndexChanged(int index)
{
  SGWidget* widget = m_SGWidgets[index];

  verticalLayout_2->removeWidget(m_SGWidget);
  m_SGWidget->hide();
  m_SGWidget = widget;
  verticalLayout_2->addWidget(m_SGWidget);
  m_SGWidget->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_addPhase_clicked()
{
 // std::cout << "on_addPhase_clicked" << std::endl;

  // Ensure the Current SGWidget has generated its data first:
  if (false == m_SGWidget->getDataHasBeenGenerated())
  {
    int r = QMessageBox::warning(this, tr("StatsGenerator"),
                                 tr("Data for the current phase has NOT been generated.\nDo you want to generate it now?"),
                                 QMessageBox::Ok | QMessageBox::Cancel);
    if (r == QMessageBox::Ok)
    {
      // The user wants to generate the data. Generate it and move on
      m_SGWidget->on_m_GenerateDefaultData_clicked();
    }
    else if (r == QMessageBox::Cancel)
    {
      return;
    }
  }


  double phaseFractionTotal = 0.0;
  for(int p = 0; p < m_SGWidgets.size(); ++p)
  {
    phaseFractionTotal += m_SGWidgets[p]->getPhaseFraction();
  }

  EditPhaseDialog dialog;
  dialog.setOtherPhaseFractionTotal(phaseFractionTotal);
  int r = dialog.exec();
  if (r == QDialog::Accepted)
  {
    SGWidget* widget = new SGWidget(centralwidget);
    m_SGWidgets.push_back(widget);
    widget->setPhaseIndex(m_SGWidgets.size());
    widget->setCrystalStructure(dialog.getCrystalStructure());
    widget->setPhaseFraction(dialog.getPhaseFraction());
    widget->setPhaseType(dialog.getPhaseType());
    widget->setPptFraction(dialog.getPptFraction());
    QString cName = widget->getComboString();

    widget->setObjectName(cName);
    widget->updatePlots();

    phaseCombo->addItem(cName);

    phaseCombo->setCurrentIndex(phaseCombo->count()-1);
    setWindowModified(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_editPhase_clicked()
{
  double phaseFractionTotal = 0.0;
  EditPhaseDialog dialog;
  for(int p = 0; p < m_SGWidgets.size(); ++p)
  {
    if (m_SGWidget != m_SGWidgets[p]) {
      phaseFractionTotal += m_SGWidgets[p]->getPhaseFraction();
    }
    else
    {
      dialog.setPhaseFraction(m_SGWidget->getPhaseFraction());
    }
  }
  dialog.setOtherPhaseFractionTotal(phaseFractionTotal);
  dialog.setCrystalStructure(m_SGWidget->getCrystalStructure());
  dialog.setPhaseType(m_SGWidget->getPhaseType());
  dialog.setPptFraction(m_SGWidget->getPptFraction());
  int r = dialog.exec();
  if (r == QDialog::Accepted)
  {
    int index = phaseCombo->currentIndex();
    m_SGWidget->setCrystalStructure(dialog.getCrystalStructure());
    m_SGWidget->setPhaseFraction(dialog.getPhaseFraction());
    m_SGWidget->setPhaseType(dialog.getPhaseType());
    m_SGWidget->setPptFraction(dialog.getPptFraction());
    QString cName = m_SGWidget->getComboString();
    phaseCombo->blockSignals(true);
    phaseCombo->clear();
    for(int p = 0; p < m_SGWidgets.size(); ++p)
    {
      phaseCombo->addItem(m_SGWidgets[p]->getComboString());
    }
    phaseCombo->setCurrentIndex(index);
    setWindowModified(true);

    phaseCombo->blockSignals(false);

    m_SGWidget->updatePlots();
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_deletePhase_clicked()
{
  if (m_SGWidgets.size() > 1)
  {
    int index = phaseCombo->currentIndex();
    phaseCombo->blockSignals(true);
    phaseCombo->clear(); // Clear the combo box of everything as we are going to rebuild it

    // Remove the SGPhase object from the vector
    m_SGWidgets.remove(index);

    // Reset the phase index for each SGPhase object
    for(int p = 0; p < m_SGWidgets.size(); ++p)
    {
      m_SGWidgets[p]->setPhaseIndex(p+1);
      phaseCombo->addItem(m_SGWidgets[p]->getComboString());
      m_SGWidgets[p]->setObjectName(m_SGWidgets[p]->getComboString());
    }

    SGWidget* widget = m_SGWidgets[0];

    verticalLayout_2->removeWidget(m_SGWidget);
    m_SGWidget->hide();
    m_SGWidget->deleteLater();
    m_SGWidget = widget;
    verticalLayout_2->addWidget(m_SGWidget);
    m_SGWidget->show();

    setWindowModified(true);

    // And now clean up/release any extra widgets that are being used

    phaseCombo->blockSignals(false);
  }
  setWindowModified(true);
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
  int err = 0;
  //std::cout << "StatsGeneratorUI::on_actionSave_triggered()" << std::endl;
  if (m_FileSelected == false)
  {
    //QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + m_FileName;
    QString h5file = QFileDialog::getSaveFileName(this, tr("Save HDF5 Statistics File"),
                                                  m_FilePath,
                                                   tr("HDF5 Files (*.h5stats)") );
    if ( true == h5file.isEmpty() ){ return;  }
    m_FilePath = h5file;
    QFileInfo fi (m_FilePath);
    QString ext = fi.suffix();
    m_OpenDialogLastDirectory = fi.path();
    m_FileSelected = true;
  }
  double phaseFractionTotal = 0.0;
  for(int p = 0; p < m_SGWidgets.size(); ++p)
  {
    phaseFractionTotal += m_SGWidgets[p]->getPhaseFraction();
  }

  int nPhases = m_SGWidgets.size();
  DataContainer::Pointer m = DataContainer::New();
  StatsDataArray::Pointer statsDataArray = StatsDataArray::New();
  m->addEnsembleData(DREAM3D::EnsembleData::Statistics, statsDataArray);

  UInt32ArrayType::Pointer crystalStructures = UInt32ArrayType::CreateArray(nPhases + 1, DREAM3D::EnsembleData::CrystalStructures);
  crystalStructures->SetValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
  m->addEnsembleData(DREAM3D::EnsembleData::CrystalStructures, crystalStructures);

  UInt32ArrayType::Pointer phaseTypes = UInt32ArrayType::CreateArray(nPhases + 1, DREAM3D::EnsembleData::PhaseTypes);
  phaseTypes->SetValue(0, DREAM3D::PhaseType::UnknownPhaseType);
  m->addEnsembleData(DREAM3D::EnsembleData::PhaseTypes, phaseTypes);

//  UInt32ArrayType::Pointer shapeTypes = UInt32ArrayType::CreateArray(nPhases + 1);
//  shapeTypes->SetName(DREAM3D::EnsembleData::ShapeTypes);
//  m->addEnsembleData(DREAM3D::EnsembleData::ShapeTypes, shapeTypes);

  // Loop on all the phases

  for(int i = 0; i < nPhases; ++i)
  {
    StatsData::Pointer data = StatsData::New();
    statsDataArray->setStatsData(i+1, data);
    SGWidget* sgwidget = m_SGWidgets[i];
    sgwidget->setTotalPhaseFraction(phaseFractionTotal);
    err = sgwidget->gatherStatsData(m);

  }

  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setDataContainer(m.get());
  writer->setOutputFile(m_FilePath.toStdString());
  writer->execute();
  // Force the clean up of the writer by assigning a NULL pointer which will
  // have the effect of executing the destructor of the H5StatsWriter Class
  writer = DataContainerWriter::NullPointer();



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
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "Untitled.h5stats";
  QString h5file = QFileDialog::getOpenFileName(this, tr("Open HDF5 Statistics File"), proposedFile, tr("HDF5 Files (*.h5stats *.h5 )"));
  if(true == h5file.isEmpty())
  {
    return;
  }

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
  int err = 0;
  if (true == h5file.isEmpty()) // User cancelled the operation
  {
    return;
  }

  // Tell the RecentFileList to update itself then broadcast those changes.
  QRecentFileList::instance()->addFile(h5file);

  updateRecentFileList(h5file);

  m_FilePath = h5file;
  QFileInfo fi (m_FilePath);
  QString ext = fi.suffix();
  m_OpenDialogLastDirectory = fi.path();
  m_FileSelected = true;


  std::string path;

 // int err = 0;
  size_t nPhases = 0;
  // Delete any existing phases from the GUI (except the first one)
  while (phaseCombo->count() != 1)
  {
    on_deletePhase_clicked();
  }

  // Instantiate a Reader object
  FilterPipeline::Pointer pipeline = FilterPipeline::New();
 // DataContainer::Pointer m = DataContainer::New();
 // pipeline->setDataContainer(m);
  DataContainerReader::Pointer reader = DataContainerReader::New();
  reader->setInputFile(m_FilePath.toStdString());
  //reader->setDataContainer(m.get());
  reader->setReadCellData(false);
  reader->setReadFieldData(false);
  reader->setReadEnsembleData(true);
//  reader->execute();
  pipeline->pushBack(reader);
  pipeline->run();
  err = pipeline->getErrorCondition();
  if (err < 0)
  {
    this->statusBar()->showMessage("Error Reading the DREAM3D Data File");
    return;
  }

  DataContainer::Pointer m = pipeline->getDataContainer();

  // Get the number of Phases
  nPhases = m->getNumEnsembleTuples();


  SGWidget* sgwidget = NULL;
  // We should iterate on all the phases here to start setting data and creating
  // all of the StatsGenPhase Objects
  for (size_t i = 1; i < nPhases; ++i)
  {
    // Create a new Default SGPhase
    sgwidget = createNewSGWidget();
    m_SGWidgets.push_back(sgwidget);
    sgwidget->extractStatsData(m, i);
    phaseCombo->addItem(sgwidget->getComboString());
  }

  // Now delete the first Phase from the Combo which was left over from something else
  phaseCombo->setCurrentIndex(0);
  on_deletePhase_clicked();

  // Set the window title correctly
  setWindowModified(false);
  QString windowTitle = QString("");
  windowTitle = windowTitle +  m_FilePath + QString(" - StatsGenerator");
  setWindowTitle(windowTitle);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGWidget* StatsGeneratorUI::createNewSGWidget()
{
  return new SGWidget(NULL);
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
void StatsGeneratorUI::on_actionAbout_triggered()
{
  QString msg ("StatsGenerator Version ");
  msg.append(DREAM3DLib::Version::Complete().c_str());
  msg.append("\n\nThe Primary Developers are:\n");
  msg.append("Dr. Michael Groeber\n  US Air Force Research Laboratory\n  michael.groeber@wpafb.af.mil\n");
  msg.append("Mr. Michael Jackson\n  BlueQuartz Software\n  mike.jackson@bluequartz.net\n\n");
  msg.append("Please send any help, bug or feature requests dream3d@bluequartz.net\n\n");
  msg.append("The latest version can always be downloaded from http://dream3d.bluequartz.net\n");
  QMessageBox::information(this, QString("About DREAM.3D"), msg, QMessageBox::Ok | QMessageBox::Default);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_actionLicense_Information_triggered()
{
  ApplicationAboutBoxDialog about(StatsGenerator::LicenseList, this);
  QString an = QCoreApplication::applicationName();
  QString version("");
  version.append(DREAM3DLib::Version::PackageComplete().c_str());
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUrl StatsGeneratorUI::htmlHelpIndexFile()
{
  QString s = "StatsGenerator/index.html";
  return QUrl(s);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_actionStatsGenerator_Help_triggered()
{
  m_HelpDialog->setContentFile(htmlHelpIndexFile());
}
