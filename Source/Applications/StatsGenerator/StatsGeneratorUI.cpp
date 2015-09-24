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


#include "StatsGeneratorUI.h"


#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QProgressDialog>


#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"
#include "H5Support/HDF5ScopedFileSentinel.h"

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/TransformationStatsData.h"
#include "SIMPLib/StatsData/BoundaryStatsData.h"
#include "SIMPLib/StatsData/MatrixStatsData.h"

#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/CoreFilters/DataContainerWriter.h"
#include "SIMPLib/CoreFilters/DataContainerReader.h"

#include "QtSupportLib/ApplicationAboutBoxDialog.h"
#include "QtSupportLib/QRecentFileList.h"
#include "QtSupportLib/HelpDialog.h"

#include "Applications/DREAM3D/DREAM3DApplication.h"

#include "DREAM3D/License/StatsGeneratorLicenseFiles.h"

#include "SGApplication.h"
#include "EditPhaseDialog.h"


// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_StatsGeneratorUI.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGeneratorUI::StatsGeneratorUI(QWidget* parent) :
  QMainWindow(parent),
  m_FileSelected(false),
  m_HelpDialog(NULL)
{
  m_OpenDialogLastDirectory = QDir::homePath();
  m_FilePath = m_OpenDialogLastDirectory + QDir::separator() + "Untitled.dream3d";
  setupUi(this);
  setupGui();

  QRecentFileList* recentFileList = QRecentFileList::instance();
  connect(recentFileList, SIGNAL (fileListChanged(const QString&)), this, SLOT(updateRecentFileList(const QString&)));
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
void StatsGeneratorUI::closeEvent(QCloseEvent* event)
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
  DREAM3DSettings prefs;

  // Read preferences here
}

// -----------------------------------------------------------------------------
//  Write our prefs to file
// -----------------------------------------------------------------------------
void StatsGeneratorUI::writeSettings()
{
  DREAM3DSettings prefs;

  // Write preferences here
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::setupGui()
{
  phaseTabs->clear();

  PrimaryPhaseWidget* ppw = new PrimaryPhaseWidget;
  ppw->setPhaseIndex(1);
  ppw->setPhaseType(DREAM3D::PhaseType::PrimaryPhase);
  ppw->setCrystalStructure(Ebsd::CrystalStructure::Cubic_High);
  ppw->setPhaseFraction(1.0);
  ppw->setTotalPhaseFraction(1.0);
  phaseTabs->addTab(ppw, "Primary Phase");

  setWindowTitle("[*] - Untitled.dream3d");
  setWindowModified(true);

  m_HelpDialog = new HelpDialog(this);
  m_HelpDialog->setWindowModality(Qt::NonModal);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_addPhase_clicked()
{
  // Ensure the Current SGWidget has generated its data first:
  SGWidget* sgwidget = qobject_cast<SGWidget*>(phaseTabs->currentWidget());
  if (false == sgwidget->getDataHasBeenGenerated())
  {
    int r = QMessageBox::warning(this, tr("StatsGenerator"),
                                 tr("Data for the current phase has NOT been generated.\nDo you want to generate it now?"),
                                 QMessageBox::Ok | QMessageBox::Cancel);
    if (r == QMessageBox::Ok)
    {
      // The user wants to generate the data. Generate it and move on
      sgwidget->on_m_GenerateDefaultData_clicked();
    }
    else if (r == QMessageBox::Cancel)
    {
      return;
    }
  }


  double phaseFractionTotal = 0.0;
  for(int p = 0; p < phaseTabs->count(); ++p)
  {
    SGWidget* sgwidget = qobject_cast<SGWidget*>(phaseTabs->widget(p));
    phaseFractionTotal += sgwidget->getPhaseFraction();
  }

  EditPhaseDialog dialog;
  dialog.setEditFlag(true);
  dialog.setOtherPhaseFractionTotal(phaseFractionTotal);
  int r = dialog.exec();
  if (r == QDialog::Accepted)
  {
    if(dialog.getPhaseType() == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryPhaseWidget* ppw = new PrimaryPhaseWidget(centralwidget);
      phaseTabs->addTab(ppw, "Primary");
      ppw->setPhaseIndex(phaseTabs->count());
      ppw->setPhaseType(DREAM3D::PhaseType::PrimaryPhase);
      ppw->setCrystalStructure(dialog.getCrystalStructure());
      ppw->setPhaseFraction(dialog.getPhaseFraction());
      ppw->setPhaseType(dialog.getPhaseType());
      QString cName = ppw->getComboString();
      ppw->setObjectName(cName);
      ppw->updatePlots();
      setWindowModified(true);
    }
    else if(dialog.getPhaseType() == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitatePhaseWidget* ppw = new PrecipitatePhaseWidget(centralwidget);
      phaseTabs->addTab(ppw, "Precipitate");
      ppw->setPhaseIndex(phaseTabs->count());
      ppw->setPhaseType(DREAM3D::PhaseType::PrecipitatePhase);
      ppw->setCrystalStructure(dialog.getCrystalStructure());
      ppw->setPhaseFraction(dialog.getPhaseFraction());
      ppw->setPhaseType(dialog.getPhaseType());
      ppw->setPptFraction(dialog.getPptFraction());
      QString cName = ppw->getComboString();
      ppw->setObjectName(cName);
      ppw->updatePlots();
      setWindowModified(true);
    }
    else if(dialog.getPhaseType() == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationPhaseWidget* tpw = new TransformationPhaseWidget(centralwidget);
      phaseTabs->addTab(tpw, "Transformation");
      tpw->setPhaseIndex(phaseTabs->count());
      tpw->setPhaseType(DREAM3D::PhaseType::TransformationPhase);
      tpw->setCrystalStructure(dialog.getCrystalStructure());
      tpw->setPhaseFraction(dialog.getPhaseFraction());
      tpw->setPhaseType(dialog.getPhaseType());
      tpw->setParentPhase(dialog.getParentPhase());
      QString cName = tpw->getComboString();
      tpw->setObjectName(cName);
      tpw->updatePlots();
      setWindowModified(true);
    }
    else if(dialog.getPhaseType() == DREAM3D::PhaseType::MatrixPhase)
    {
      MatrixPhaseWidget* mpw = new MatrixPhaseWidget(centralwidget);
      phaseTabs->addTab(mpw, "Matrix");
      mpw->setPhaseIndex(phaseTabs->count());
      mpw->setPhaseType(DREAM3D::PhaseType::MatrixPhase);
      mpw->setCrystalStructure(dialog.getCrystalStructure());
      mpw->setPhaseFraction(dialog.getPhaseFraction());
      mpw->setPhaseType(dialog.getPhaseType());
      QString cName = mpw->getComboString();
      mpw->setObjectName(cName);
      setWindowModified(true);
    }
    else if(dialog.getPhaseType() == DREAM3D::PhaseType::BoundaryPhase)
    {
      BoundaryPhaseWidget* bpw = new BoundaryPhaseWidget(centralwidget);
      phaseTabs->addTab(bpw, "Boundary");
      bpw->setPhaseIndex(phaseTabs->count());
      bpw->setPhaseType(DREAM3D::PhaseType::BoundaryPhase);
      bpw->setCrystalStructure(dialog.getCrystalStructure());
      bpw->setPhaseFraction(dialog.getPhaseFraction());
      bpw->setPhaseType(dialog.getPhaseType());
      QString cName = bpw->getComboString();
      bpw->setObjectName(cName);
      setWindowModified(true);
    }

    // Make sure the new tab is the active tab
    phaseTabs->setCurrentIndex(phaseTabs->count() - 1);


  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_editPhase_clicked()
{
  double phaseFractionTotal = 0.0;
  EditPhaseDialog dialog;
  dialog.setEditFlag(false);

  SGWidget* sgwidget = qobject_cast<SGWidget*>(phaseTabs->currentWidget());

  for (int p = 0; p < phaseTabs->count(); ++p)
  {
    SGWidget* currentWidget = qobject_cast<SGWidget*>(phaseTabs->widget(p));
    if(sgwidget != currentWidget )
    {
      phaseFractionTotal += currentWidget->getPhaseFraction();
    }
    else
    {
      dialog.setPhaseFraction(currentWidget->getPhaseFraction());
    }
  }
  dialog.setPhaseType(sgwidget->getPhaseType());
  dialog.setOtherPhaseFractionTotal(phaseFractionTotal);
  dialog.setCrystalStructure(sgwidget->getCrystalStructure());

  if(dialog.getPhaseType() == DREAM3D::PhaseType::PrimaryPhase)
  {
    //    PrimaryPhaseWidget* ppw = qobject_cast<PrimaryPhaseWidget*>(sgwidget);
  }
  else if(dialog.getPhaseType() == DREAM3D::PhaseType::PrecipitatePhase)
  {
    PrecipitatePhaseWidget* ppw = qobject_cast<PrecipitatePhaseWidget*>(sgwidget);
    if (ppw) { dialog.setPptFraction(ppw->getPptFraction()); }
  }
  else if(dialog.getPhaseType() == DREAM3D::PhaseType::TransformationPhase)
  {
    TransformationPhaseWidget* tpw = qobject_cast<TransformationPhaseWidget*>(sgwidget);
    if (tpw) { dialog.setParentPhase(tpw->getParentPhase()); }
  }
  else if(dialog.getPhaseType() == DREAM3D::PhaseType::MatrixPhase)
  {
    //    MatrixPhaseWidget* mpw = qobject_cast<MatrixPhaseWidget*>(sgwidget);
  }
  else if(dialog.getPhaseType() == DREAM3D::PhaseType::BoundaryPhase)
  {
    //    BoundaryPhaseWidget* bpw = qobject_cast<BoundaryPhaseWidget*>(sgwidget);
  }
  int r = dialog.exec();
  if(r == QDialog::Accepted)
  {
    if(dialog.getPhaseType() == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryPhaseWidget* ppw = qobject_cast<PrimaryPhaseWidget*>(sgwidget);
      ppw->setCrystalStructure(dialog.getCrystalStructure());
      ppw->setPhaseFraction(dialog.getPhaseFraction());
      ppw->setPhaseType(dialog.getPhaseType());
      QString cName = ppw->getComboString();
      setWindowModified(true);
      ppw->updatePlots();
    }
    if(dialog.getPhaseType() == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitatePhaseWidget* ppw = qobject_cast<PrecipitatePhaseWidget*>(sgwidget);
      ppw->setCrystalStructure(dialog.getCrystalStructure());
      ppw->setPhaseFraction(dialog.getPhaseFraction());
      ppw->setPhaseType(dialog.getPhaseType());
      ppw->setPptFraction(dialog.getPptFraction());
      QString cName = ppw->getComboString();
      setWindowModified(true);
      ppw->updatePlots();
    }
    if(dialog.getPhaseType() == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationPhaseWidget* tpw = qobject_cast<TransformationPhaseWidget*>(sgwidget);
      tpw->setCrystalStructure(dialog.getCrystalStructure());
      tpw->setPhaseFraction(dialog.getPhaseFraction());
      tpw->setPhaseType(dialog.getPhaseType());
      tpw->setParentPhase(dialog.getParentPhase());
      QString cName = tpw->getComboString();
      setWindowModified(true);
      tpw->updatePlots();
    }
    if(dialog.getPhaseType() == DREAM3D::PhaseType::MatrixPhase)
    {
      MatrixPhaseWidget* mpw = qobject_cast<MatrixPhaseWidget*>(sgwidget);
      mpw->setCrystalStructure(dialog.getCrystalStructure());
      mpw->setPhaseFraction(dialog.getPhaseFraction());
      mpw->setPhaseType(dialog.getPhaseType());
      QString cName = mpw->getComboString();
      setWindowModified(true);
    }
    if(dialog.getPhaseType() == DREAM3D::PhaseType::BoundaryPhase)
    {
      BoundaryPhaseWidget* bpw = qobject_cast<BoundaryPhaseWidget*>(sgwidget);
      bpw->setCrystalStructure(dialog.getCrystalStructure());
      bpw->setPhaseFraction(dialog.getPhaseFraction());
      bpw->setPhaseType(dialog.getPhaseType());
      QString cName = bpw->getComboString();
      setWindowModified(true);
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_phaseTabs_tabCloseRequested ( int index )
{
  if(phaseTabs->count() > 1)
  {
    // Remove the SGPhase object from the vector
    SGWidget* currentWidget = qobject_cast<SGWidget*>(phaseTabs->widget(index));
    phaseTabs->removeTab(index);
    currentWidget->deleteLater(); // Actually delete it

    // Reset the phase index for each SGPhase object
    for (int p = 0; p < phaseTabs->count(); ++p)
    {
      SGWidget* sgwidget = qobject_cast<SGWidget*>(phaseTabs->widget(p));
      sgwidget->setPhaseIndex(p + 1);
      sgwidget->setObjectName(sgwidget->getComboString());
    }

#if 0
    SGWidget* widget = m_SGWidgets[0];

    verticalLayout_2->removeWidget(m_SGWidget);
    m_SGWidget->hide();
    m_SGWidget->deleteLater();
    m_SGWidget = widget;
    verticalLayout_2->addWidget(m_SGWidget);
    m_SGWidget->show();
#endif
  }
  setWindowModified(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_deletePhase_clicked()
{
  on_phaseTabs_tabCloseRequested(phaseTabs->currentIndex());
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
  // qDebug() << "RepresentationUI::on_actionClose_triggered" << "\n";
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
void StatsGeneratorUI::updateRecentFileList(const QString& file)
{
  // qDebug() << "IPHelperMainWindow::updateRecentFileList" << "\n";

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
  //qDebug() << "QRecentFileList::openRecentFile()" << "\n";

  QAction* action = qobject_cast<QAction* > (sender());
  if (action)
  {
    //qDebug() << "Opening Recent file: " << action->data().toString() << "\n";
    QString file = action->data().toString();
    openFile(file);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_actionSaveAs_triggered()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "Untitled";
  QString h5file = QFileDialog::getSaveFileName(this, tr("Save DREAM.3D File"),
                                                proposedFile,
                                                tr("DREAM.3D Files (*.dream3d)") );
  if ( true == h5file.isEmpty() ) { return;  }
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
void StatsGeneratorUI::on_action_OpenDREAM3D_triggered()
{
  QString appPath = dream3dApp->applicationDirPath();

  QDir appDir = QDir(appPath);
  QString s("file://");

#if defined(Q_OS_WIN)
  s = s + "/"; // Need the third slash on windows because file paths start with a drive letter
#elif defined(Q_OS_MAC)
  if (appDir.dirName() == "MacOS")
  {
    appDir.cdUp();
    appDir.cdUp();
    appDir.cdUp();
  }
#else
  // We are on Linux - I think
  appDir.cdUp();
#endif

  QString appName = "DREAM3D";
#ifdef QT_DEBUG
  appName.append("_debug");
#endif


#if defined(Q_OS_WIN)
  s = s + appDir.absolutePath() + QDir::separator() + appName + ".exe";
#elif defined(Q_OS_MAC)
  s = s + appDir.absolutePath() + QDir::separator() + appName  + ".app";
#else
  s = s + appDir.absolutePath() + QDir::separator() + appName  + ".sh";
#endif
  QDesktopServices::openUrl(QUrl(s));
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_actionSave_triggered()
{
  int err = 0;
  //qDebug() << "StatsGeneratorUI::on_actionSave_triggered()" << "\n";
  if (m_FileSelected == false)
  {
    //QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + m_FileName;
    QString h5file = QFileDialog::getSaveFileName(this, tr("Save DREAM.3D File"),
                                                  m_FilePath,
                                                  tr("DREAM.3D Files (*.dream3d)") );

    if ( true == h5file.isEmpty() ) { return;  }
    m_FilePath = h5file;
    QFileInfo fi (m_FilePath);
    QString ext = fi.suffix();
    m_OpenDialogLastDirectory = fi.path();
    m_FileSelected = true;
  }
  double phaseFractionTotal = 0.0;
  for(int p = 0; p < phaseTabs->count(); ++p)
  {
    SGWidget* sgwidget = qobject_cast<SGWidget*>(phaseTabs->widget(p));
    phaseFractionTotal += sgwidget->getPhaseFraction();
  }

  int nPhases = phaseTabs->count() + 1;
  DataContainerArray::Pointer dca = DataContainerArray::New();
  DataContainer::Pointer m = DataContainer::New(DREAM3D::Defaults::StatsGenerator);
  dca->addDataContainer(m);
  QVector<size_t> tDims(1, nPhases);
  AttributeMatrix::Pointer cellEnsembleAttrMat = AttributeMatrix::New(tDims, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::AttributeMatrixType::CellEnsemble);
  m->addAttributeMatrix(DREAM3D::Defaults::CellEnsembleAttributeMatrixName, cellEnsembleAttrMat);

  StatsDataArray::Pointer statsDataArray = StatsDataArray::New();
  statsDataArray->resize(nPhases);
  cellEnsembleAttrMat->addAttributeArray(DREAM3D::EnsembleData::Statistics, statsDataArray);

  QVector<size_t> cDims(1, 1);
  UInt32ArrayType::Pointer crystalStructures = UInt32ArrayType::CreateArray(tDims, cDims, DREAM3D::EnsembleData::CrystalStructures);
  crystalStructures->setValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
  cellEnsembleAttrMat->addAttributeArray(DREAM3D::EnsembleData::CrystalStructures, crystalStructures);

  UInt32ArrayType::Pointer phaseTypes = UInt32ArrayType::CreateArray(tDims, cDims, DREAM3D::EnsembleData::PhaseTypes);
  phaseTypes->setValue(0, DREAM3D::PhaseType::UnknownPhaseType);
  cellEnsembleAttrMat->addAttributeArray(DREAM3D::EnsembleData::PhaseTypes, phaseTypes);

  // Loop on all the phases

  for(int i = 0; i < phaseTabs->count(); ++i)
  {
    SGWidget* sgwidget = qobject_cast<SGWidget*>(phaseTabs->widget(i));
    sgwidget->setTotalPhaseFraction(phaseFractionTotal);
    if(sgwidget->getPhaseType() == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData::Pointer data = PrimaryStatsData::New();
      statsDataArray->setStatsData(i + 1, data);
    }
    if(sgwidget->getPhaseType() == DREAM3D::PhaseType::PrecipitatePhase)
    {
      PrecipitateStatsData::Pointer data = PrecipitateStatsData::New();
      statsDataArray->setStatsData(i + 1, data);
    }
    if(sgwidget->getPhaseType() == DREAM3D::PhaseType::TransformationPhase)
    {
      TransformationStatsData::Pointer data = TransformationStatsData::New();
      statsDataArray->setStatsData(i + 1, data);
    }
    if(sgwidget->getPhaseType() == DREAM3D::PhaseType::MatrixPhase)
    {
      MatrixStatsData::Pointer data = MatrixStatsData::New();
      statsDataArray->setStatsData(i + 1, data);
    }
    if(sgwidget->getPhaseType() == DREAM3D::PhaseType::BoundaryPhase)
    {
      BoundaryStatsData::Pointer data = BoundaryStatsData::New();
      statsDataArray->setStatsData(i + 1, data);
    }
    err = sgwidget->gatherStatsData(cellEnsembleAttrMat);
    if(err < 0)
    {
      QString  msg("Could not save file due to an internal error gathering statistics.\nError code ");
      msg.append(QString::number(err));
      QMessageBox::critical(this, QString("File Save Error"), msg , QMessageBox::Ok);
      return;
    }
  }

  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setDataContainerArray(dca);
  writer->setOutputFile(m_FilePath);
  writer->setWriteXdmfFile(false);
  writer->setWritePipeline(false);
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
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "Untitled.dream3d";
  QString h5file = QFileDialog::getOpenFileName(this,
                                                tr("Open Statistics File"),
                                                proposedFile,
                                                tr("DREAM3D Files (*.dream3d);;H5Stats Files(*.h5stats);;HDF5 Files(*.h5 *.hdf5);;All Files(*.*)"));
  if(true == h5file.isEmpty())
  {
    return;
  }

  SGApplication* app = qobject_cast<SGApplication*>(SGApplication::instance());
  // Create a new Window
  StatsGeneratorUI* window = app->createNewStatsGenerator();
  int err = window->openFile(h5file);
  if (err < 0)
  {
    delete window;
    window = NULL;
    return;
  }

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
int StatsGeneratorUI::openFile(QString h5file)
{
  // Make sure the file path is not empty and does exist on the system
  if (true == h5file.isEmpty())
  {
    QString ss = QObject::tr("Input file was empty").arg(h5file);
    QMessageBox::critical(this, QString("File Open Error"), ss , QMessageBox::Ok);
    return -1;
  }

  QFileInfo fi(h5file);
  if (fi.exists() == false)
  {
    QString ss = QObject::tr("Input file does not exist").arg(fi.absoluteFilePath());
    QMessageBox::critical(this, QString("File Open Error"), ss , QMessageBox::Ok);
    return -2;
  }

  // Tell the RecentFileList to update itself then broadcast those changes.
  QRecentFileList::instance()->addFile(h5file);
  updateRecentFileList(h5file);

  // Set the last directory that contains our file
  m_OpenDialogLastDirectory = fi.path();
  m_FilePath = h5file;
  m_FileSelected = true;

  // Delete any existing phases from the GUI
  phaseTabs->clear();

  DataContainerArray::Pointer dca = DataContainerArray::New();
  DataContainer::Pointer m = DataContainer::New(DREAM3D::Defaults::StatsGenerator);
  dca->addDataContainer(m);
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer cellEnsembleAttrMat = AttributeMatrix::New(tDims, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::AttributeMatrixType::CellEnsemble);
  m->addAttributeMatrix(DREAM3D::Defaults::CellEnsembleAttributeMatrixName, cellEnsembleAttrMat);

  hid_t fileId = QH5Utilities::openFile(h5file, true); // Open the file Read Only
  if(fileId < 0)
  {
    QString ss = QObject::tr(": Error opening input file '%1'").arg(h5file);
    QMessageBox::critical(this, QString("File Open Error"), ss , QMessageBox::Ok);
    return -3;
  }
  // This will make sure if we return early from this method that the HDF5 File is properly closed.
  HDF5ScopedFileSentinel scopedFileSentinel(&fileId, true);
  hid_t dcaGid = H5Gopen(fileId, DREAM3D::StringConstants::DataContainerGroupName.toLatin1().constData(), 0);
  scopedFileSentinel.addGroupId(&dcaGid);

  hid_t dcGid = H5Gopen(dcaGid, DREAM3D::Defaults::StatsGenerator.toLatin1().constData(), 0);
  if(dcGid < 0)
  {
    QString title = QObject::tr("Error Opening Data Container");
    QString msg = QObject::tr("Error opening default Data Container with name '%1'").arg(DREAM3D::Defaults::StatsGenerator);
    QMessageBox::critical(this, title, msg, QMessageBox::Ok, QMessageBox::Ok);
    return -4;
  }
  scopedFileSentinel.addGroupId(&dcGid);
  hid_t amGid = H5Gopen(dcGid, DREAM3D::Defaults::CellEnsembleAttributeMatrixName.toLatin1().constData(), 0);
  scopedFileSentinel.addGroupId(&amGid);


  // We need to read one of the arrays to get the number of phases so that we can resize our attributeMatrix
  UInt32ArrayType::Pointer phases = UInt32ArrayType::CreateArray(1, DREAM3D::EnsembleData::PhaseTypes);
  int err = phases->readH5Data(amGid);
  if (err < 0)
  {

    QString title = QObject::tr("Error Opening DataArray");
    QString msg = QObject::tr("Error opening 'PhaseTypes' data array at location '%1/%2/%3/%4'").arg(DREAM3D::StringConstants::DataContainerGroupName)
                  .arg(DREAM3D::Defaults::StatsGenerator).arg(DREAM3D::Defaults::CellEnsembleAttributeMatrixName).arg(DREAM3D::EnsembleData::PhaseTypes);
    QMessageBox::critical(this, title, msg, QMessageBox::Ok, QMessageBox::Ok);
    return -4;
  }
  tDims[0] = phases->getNumberOfTuples();
  cellEnsembleAttrMat->resizeAttributeArrays(tDims);

  cellEnsembleAttrMat->addAttributeArrayFromHDF5Path(amGid, "Statistics", false);
  cellEnsembleAttrMat->addAttributeArrayFromHDF5Path(amGid, "CrystalStructures", false);
  cellEnsembleAttrMat->addAttributeArrayFromHDF5Path(amGid, "PhaseTypes", false);

  // Get the number of Phases
  size_t ensembles = cellEnsembleAttrMat->getNumTuples();

  QProgressDialog progress("Opening Stats File....", "Cancel", 0, ensembles, this);
  progress.setWindowModality(Qt::WindowModal);


  IDataArray::Pointer iDataArray = cellEnsembleAttrMat->getAttributeArray(DREAM3D::EnsembleData::PhaseTypes);
  unsigned int* phaseTypes = boost::dynamic_pointer_cast< UInt32ArrayType >(iDataArray)->getPointer(0);

  // We should iterate on all the phases here to start setting data and creating
  // all of the StatsGenPhase Objects
  for (size_t phase = 1; phase < ensembles; ++phase)
  {
    progress.setValue(phase);

    if (progress.wasCanceled())
    {
      return -5;
    }

    if(phaseTypes[phase] == DREAM3D::PhaseType::BoundaryPhase)
    {
      progress.setLabelText("Opening Boundaray Phase...");
      BoundaryPhaseWidget* w = new BoundaryPhaseWidget(this);
      phaseTabs->addTab(w, w->getTabTitle());
      w->extractStatsData(cellEnsembleAttrMat, static_cast<int>(phase));
    }
    else if(phaseTypes[phase] == DREAM3D::PhaseType::MatrixPhase)
    {
      progress.setLabelText("Opening Matrix Phase...");
      MatrixPhaseWidget* w = new MatrixPhaseWidget(this);
      phaseTabs->addTab(w, w->getTabTitle());
      w->extractStatsData(cellEnsembleAttrMat, static_cast<int>(phase));
    }
    if(phaseTypes[phase] == DREAM3D::PhaseType::PrecipitatePhase)
    {
      progress.setLabelText("Opening Precipitate Phase...");
      PrecipitatePhaseWidget* w = new PrecipitatePhaseWidget(this);
      phaseTabs->addTab(w, w->getTabTitle());
      w->extractStatsData(cellEnsembleAttrMat, static_cast<int>(phase));
    }
    if(phaseTypes[phase] == DREAM3D::PhaseType::PrimaryPhase)
    {
      progress.setLabelText("Opening Primary Phase...");
      PrimaryPhaseWidget* w = new PrimaryPhaseWidget(this);
      phaseTabs->addTab(w, w->getTabTitle());
      w->extractStatsData(cellEnsembleAttrMat, static_cast<int>(phase));
    }
    if(phaseTypes[phase] == DREAM3D::PhaseType::TransformationPhase)
    {
      progress.setLabelText("Opening Transformation Phase...");
      TransformationPhaseWidget* w = new TransformationPhaseWidget(this);
      phaseTabs->addTab(w, w->getTabTitle());
      w->extractStatsData(cellEnsembleAttrMat, static_cast<int>(phase));
    }
    else
    {

    }
  }
  progress.setValue(ensembles);

  // Now delete the first Phase from the Combo which was left over from something else
  phaseTabs->setCurrentIndex(0);

  // Set the window title correctly
  setWindowModified(false);
  QString windowTitle = QString("");
  windowTitle = windowTitle +  m_FilePath + QString(" - StatsGenerator");
  setWindowTitle(windowTitle);
  return 0;
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
#ifdef Q_OS_MAC
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
  msg.append(SIMPLib::Version::Complete().toLatin1().data());
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
  version.append(SIMPLib::Version::PackageComplete().toLatin1().data());
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
  QString appPath = dream3dApp->applicationDirPath();

  QDir helpDir = QDir(appPath);
  QString s("file://");

#if defined(Q_OS_WIN)
  s = s + "/"; // Need the third slash on windows because file paths start with a drive letter
#elif defined(Q_OS_MAC)
  if (helpDir.dirName() == "MacOS")
  {
    helpDir.cdUp();
    helpDir.cdUp();
    helpDir.cdUp();
  }
#else
  // We are on Linux - I think
  helpDir.cdUp();
#endif



#if defined(Q_OS_WIN)
  QFileInfo fi( helpDir.absolutePath() + "/Help/DREAM3D/index.html");
  if (fi.exists() == false)
  {
    // The help file does not exist at the default location because we are probably running from visual studio.
    // Try up one more directory
    helpDir.cdUp();
  }
#endif

  s = s + helpDir.absolutePath() + "/Help/DREAM3D/index.html";
  return QUrl(s);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::displayDialogBox(QString title, QString text, QMessageBox::Icon icon)
{

  QMessageBox msgBox;
  msgBox.setText(title);
  msgBox.setInformativeText(text);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.setDefaultButton(QMessageBox::Ok);
  msgBox.setIcon(icon);
  msgBox.exec();
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGeneratorUI::on_actionStatsGenerator_Help_triggered()
{
  QUrl url = htmlHelpIndexFile();
  bool didOpen = QDesktopServices::openUrl(url);
  if(false == didOpen)
  {
    displayDialogBox(("Error Opening Help File"),
                     QString::fromLatin1("DREAM3D could not open the help file path ") + url.path(),
                     QMessageBox::Critical);
  }
}
