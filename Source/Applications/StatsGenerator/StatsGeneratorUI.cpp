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

#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"

#include "DREAM3DLib/DREAM3DVersion.h"
#include "DREAM3DLib/Common/VoxelDataContainer.h"
#include "DREAM3DLib/Common/StatsDataArray.h"
#include "DREAM3DLib/Common/FilterPipeline.h"
#include "DREAM3DLib/IOFilters/VoxelDataContainerWriter.h"
#include "DREAM3DLib/IOFilters/VoxelDataContainerReader.h"

#include "QtSupport/ApplicationAboutBoxDialog.h"
#include "QtSupport/QRecentFileList.h"
#include "QtSupport/HelpDialog.h"

#include "DREAM3D/License/StatsGeneratorLicenseFiles.h"

#include "SGApplication.h"
#include "EditPhaseDialog.h"

/**
 * @brief The HDF5FileSentinel class ensures the HDF5 file that is currently open
 * is closed when the variable goes out of Scope
 */
class HDF5ScopedFileSentinel
{
  public:
    HDF5ScopedFileSentinel(hid_t fileId) : m_FileId(fileId)
    {}
    virtual ~HDF5ScopedFileSentinel()
    {
      if (m_FileId > 0) {
        H5Utilities::closeFile(m_FileId);
      }
    }
    DREAM3D_INSTANCE_PROPERTY(hid_t, FileId)
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGeneratorUI::StatsGeneratorUI(QWidget *parent) :
  QMainWindow(parent),
  m_FileSelected(false),
  m_HelpDialog(NULL)
{
  m_OpenDialogLastDirectory = QDir::homePath();
  m_FilePath = m_OpenDialogLastDirectory + QDir::separator() + "Untitled.dream3d";
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
  phaseTabs->clear();

  PrimaryPhaseWidget* ppw = new PrimaryPhaseWidget;
  ppw->setPhaseIndex(1);
  ppw->setPhaseType(DREAM3D::PhaseType::PrimaryPhase);
  ppw->setCrystalStructure(Ebsd::CrystalStructure::Cubic);
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
    dialog.setPptFraction(ppw->getPptFraction());
  }
  else if(dialog.getPhaseType() == DREAM3D::PhaseType::TransformationPhase)
  {
    TransformationPhaseWidget* tpw = qobject_cast<TransformationPhaseWidget*>(sgwidget);
    dialog.setParentPhase(tpw->getParentPhase());
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
      sgwidget->setPhaseIndex(p+1);
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
  QString h5file = QFileDialog::getSaveFileName(this, tr("Save DREAM3D File"),
    proposedFile,
    tr("DREAM3D Files (*.dream3d)") );
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
    QString h5file = QFileDialog::getSaveFileName(this, tr("Save DREAM3D File"),
                                                  m_FilePath,
                                                   tr("DREAM3D Files (*.dream3d)") );
    if ( true == h5file.isEmpty() ){ return;  }
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

  int nPhases = phaseTabs->count();
  VoxelDataContainer::Pointer m = VoxelDataContainer::New();
  StatsDataArray::Pointer statsDataArray = StatsDataArray::New();
  m->addEnsembleData(DREAM3D::EnsembleData::Statistics, statsDataArray);

  UInt32ArrayType::Pointer crystalStructures = UInt32ArrayType::CreateArray(nPhases + 1, DREAM3D::EnsembleData::CrystalStructures);
  crystalStructures->SetValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
  m->addEnsembleData(DREAM3D::EnsembleData::CrystalStructures, crystalStructures);

  UInt32ArrayType::Pointer phaseTypes = UInt32ArrayType::CreateArray(nPhases + 1, DREAM3D::EnsembleData::PhaseTypes);
  phaseTypes->SetValue(0, DREAM3D::PhaseType::UnknownPhaseType);
  m->addEnsembleData(DREAM3D::EnsembleData::PhaseTypes, phaseTypes);

  // Loop on all the phases

  for(int i = 0; i < nPhases; ++i)
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
    err = sgwidget->gatherStatsData(m);
    if(err < 0)
    {

    }
  }


  hid_t fileId = H5Utilities::createFile (m_FilePath.toStdString());
    // This will make sure if we return early from this method that the HDF5 File is properly closed.
  HDF5ScopedFileSentinel scopedFileSentinel(fileId);

  VoxelDataContainerWriter::Pointer writer = VoxelDataContainerWriter::New();
  writer->setVoxelDataContainer(m.get());
  writer->setHdfFileId(fileId);
  writer->execute();
  // Force the clean up of the writer by assigning a NULL pointer which will
  // have the effect of executing the destructor of the H5StatsWriter Class
  writer = VoxelDataContainerWriter::NullPointer();

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
  // Make sure the file path is not empty and does exist on the system
  if (true == h5file.isEmpty())
  {
    return;
  }

  QFileInfo fi(h5file);
  if (fi.exists() == false)
  {
    return;
  }

  // Tell the RecentFileList to update itself then broadcast those changes.
  QRecentFileList::instance()->addFile(h5file);
  updateRecentFileList(h5file);

  // Set the last directory that contains our file
  m_OpenDialogLastDirectory = fi.path();
  m_FilePath = h5file;
  m_FileSelected = true;

  std::string path;


  // Delete any existing phases from the GUI
  phaseTabs->clear();

  // Instantiate a Reader object
  VoxelDataContainer::Pointer m = VoxelDataContainer::New();
  {
    // We are going to scope this next section so that we make sure the HDF5 file gets closed after the reading is complete
    std::stringstream ss;
    hid_t fileId = H5Utilities::openFile(m_FilePath.toStdString(), true); // Open the file Read Only
    if(fileId < 0)
    {
      ss.str("");
      ss << ": Error opening input file '" << m_FilePath.toStdString() << "'";
      return;
    }
    // This will make sure if we return early from this method that the HDF5 File is properly closed.
    HDF5ScopedFileSentinel scopedFileSentinel(fileId);

    VoxelDataContainerReader::Pointer reader = VoxelDataContainerReader::New();
    reader->setHdfFileId(fileId);
    reader->setVoxelDataContainer(m.get());
    reader->setReadCellData(false);
    reader->setReadFieldData(false);
    reader->setReadEnsembleData(true);
    reader->setReadAllArrays(true);
    reader->execute();
    err = reader->getErrorCondition();
    if (err < 0)
    {
      this->statusBar()->showMessage("Error Reading the DREAM3D Data File");
      return;
    }
  } // The HDF5 file will get properly closed when we move to the next line because the ScopedFileMonitor will go out of scope and close the file

  // Get the number of Phases
  size_t ensembles = m->getNumEnsembleTuples();

  typedef DataArray<unsigned int> PhaseTypeArrayType;
  unsigned int* phaseTypes = m->getEnsembleDataSizeCheck<unsigned int, PhaseTypeArrayType, AbstractFilter>(DREAM3D::EnsembleData::PhaseTypes, ensembles, 1, NULL);

  // We should iterate on all the phases here to start setting data and creating
  // all of the StatsGenPhase Objects
  for (size_t phase = 1; phase < ensembles; ++phase)
  {
    if(phaseTypes[phase] == DREAM3D::PhaseType::BoundaryPhase)
    {
   //   BoundaryStatsData* bsd = BoundaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
      BoundaryPhaseWidget* w = new BoundaryPhaseWidget(this);
      phaseTabs->addTab(w, w->getTabTitle());
      w->extractStatsData(m, static_cast<int>(phase));
    }
    else if(phaseTypes[phase] == DREAM3D::PhaseType::MatrixPhase)
    {
   //   MatrixStatsData* msd = MatrixStatsData::SafePointerDownCast(statsDataArray[phase].get());
      MatrixPhaseWidget* w = new MatrixPhaseWidget(this);
      phaseTabs->addTab(w, w->getTabTitle());
      w->extractStatsData(m, static_cast<int>(phase));
    }
    if(phaseTypes[phase] == DREAM3D::PhaseType::PrecipitatePhase)
    {
  //    PrecipitateStatsData* psd = PrecipitateStatsData::SafePointerDownCast(statsDataArray[phase].get());
      PrecipitatePhaseWidget* w = new PrecipitatePhaseWidget(this);
      phaseTabs->addTab(w, w->getTabTitle());
      w->extractStatsData(m, static_cast<int>(phase));
    }
    if(phaseTypes[phase] == DREAM3D::PhaseType::PrimaryPhase)
    {
   //   PrimaryStatsData* prisd = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
      PrimaryPhaseWidget* w = new PrimaryPhaseWidget(this);
      phaseTabs->addTab(w, w->getTabTitle());
      w->extractStatsData(m, static_cast<int>(phase));
    }
    if(phaseTypes[phase] == DREAM3D::PhaseType::TransformationPhase)
    {
    //  TransformationStatsData* tsd = TransformationStatsData::SafePointerDownCast(statsDataArray[phase].get());
      TransformationPhaseWidget* w = new TransformationPhaseWidget(this);
      phaseTabs->addTab(w, w->getTabTitle());
      w->extractStatsData(m, static_cast<int>(phase));
    }
    else
    {

    }

  }

  // Now delete the first Phase from the Combo which was left over from something else
  phaseTabs->setCurrentIndex(0);
  //on_deletePhase_clicked();

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
