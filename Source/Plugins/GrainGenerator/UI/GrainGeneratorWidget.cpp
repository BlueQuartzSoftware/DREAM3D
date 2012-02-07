/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "GrainGeneratorWidget.h"


//-- Qt Includes
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QThread>
#include <QtCore/QFileInfoList>
#include <QtGui/QFileDialog>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QComboBox>

#include "QtSupport/DREAM3DQtMacros.h"
#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/QCheckboxDialog.h"

#include "MXA/Utilities/MXADir.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/ShapeType.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/HDF5/H5StatsReader.h"
#include "DREAM3DLib/HDF5/H5StatsWriter.h"
#include "DREAM3DLib/IOFilters/DataContainerWriter.h"
#include "DREAM3DLib/IOFilters/VtkRectilinearGridWriter.h"
#include "DREAM3DLib/IOFilters/FieldDataCSVWriter.h"
#include "DREAM3DLib/SyntheticBuilderFilters/MatchCrystallography.h"
#include "DREAM3DLib/SyntheticBuilderFilters/PlacePrecipitates.h"
#include "DREAM3DLib/SyntheticBuilderFilters/PackGrainsGen2.h"
#include "DREAM3DLib/SyntheticBuilderFilters/AdjustVolume.h"

#include "GrainGeneratorPlugin.h"

const static float m_pi = (float)M_PI;

#define PACK_GRAINS_ERROR_TXT_OUT 1
#define PACK_GRAINS_VTK_FILE_OUT 1

#define MAKE_OUTPUT_FILE_PATH(outpath, filename, outdir, prefix)\
    std::string outpath = outdir + MXADir::Separator + prefix + filename;



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGeneratorWidget::GrainGeneratorWidget(QWidget *parent) :
DREAM3DPluginFrame(parent),
m_FilterPipeline(NULL),
m_WorkerThread(NULL),
m_WriteSurfaceVoxelScalars(true),
m_WritePhaseIdScalars(true),
m_WriteIPFColorScalars(true),
m_WriteBinaryVTKFile(true),
#if defined(Q_WS_WIN)
m_OpenDialogLastDirectory("C:\\")
#else
m_OpenDialogLastDirectory("~/")
#endif
{
  setupUi(this);
  setupGui();
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGeneratorWidget::~GrainGeneratorWidget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::readSettings(QSettings &prefs)
{
  QString val;
  bool ok;
  qint32 i;
  double d;

  prefs.beginGroup("GrainGenerator");
  READ_FILEPATH_SETTING(prefs, m_, H5InputStatisticsFile, "");
  READ_FILEPATH_SETTING(prefs, m_, OutputDir, "");
  READ_STRING_SETTING(prefs, m_, OutputFilePrefix, "GrainGenerator_")
  READ_SETTING(prefs, m_, XResolution, ok, d, 0.25 , Double);
  READ_SETTING(prefs, m_, YResolution, ok, d, 0.25 , Double);
  READ_SETTING(prefs, m_, ZResolution, ok, d, 0.25 , Double);
  READ_SETTING(prefs, m_, NeighborhoodErrorWeight, ok, d, 1.0 , Double);
  m_XPoints->blockSignals(true);
  READ_SETTING(prefs, m_, XPoints, ok, i, 100 , Int);
  m_XPoints->blockSignals(false);
  m_YPoints->blockSignals(true);
  READ_SETTING(prefs, m_, YPoints, ok, i, 100 , Int);
  m_YPoints->blockSignals(false);
  READ_SETTING(prefs, m_, ZPoints, ok, i, 100 , Int);

  READ_CHECKBOX_SETTING(prefs, m_, PeriodicBoundaryConditions, false);
  m_AlreadyFormed->blockSignals(true);
  READ_CHECKBOX_SETTING(prefs, m_, AlreadyFormed, false);
  READ_FILEPATH_SETTING(prefs, m_, StructureFile, "");
  m_AlreadyFormed->blockSignals(false);

  READ_BOOL_SETTING(prefs, m_, WriteSurfaceVoxelScalars, true);
  READ_BOOL_SETTING(prefs, m_, WritePhaseIdScalars, true);
  READ_BOOL_SETTING(prefs, m_, WriteIPFColorScalars, true);
  READ_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true);

  READ_CHECKBOX_SETTING(prefs, m_, VisualizationVizFile, true);
  READ_CHECKBOX_SETTING(prefs, m_, HDF5GrainFile, true);

  prefs.endGroup();

  on_m_H5InputStatisticsFile_textChanged(QString(""));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("GrainGenerator");
  WRITE_STRING_SETTING(prefs, m_, H5InputStatisticsFile)
  WRITE_STRING_SETTING(prefs, m_, OutputDir)
  WRITE_STRING_SETTING(prefs, m_, OutputFilePrefix)

  WRITE_SETTING(prefs, m_, XResolution )
  WRITE_SETTING(prefs, m_, YResolution )
  WRITE_SETTING(prefs, m_, ZResolution )
  WRITE_SETTING(prefs, m_, NeighborhoodErrorWeight )
  WRITE_SETTING(prefs, m_, XPoints )
  WRITE_SETTING(prefs, m_, YPoints )
  WRITE_SETTING(prefs, m_, ZPoints )

  WRITE_BOOL_SETTING(prefs, m_, PeriodicBoundaryConditions, m_PeriodicBoundaryConditions->isChecked())
  WRITE_BOOL_SETTING(prefs, m_, AlreadyFormed, m_AlreadyFormed->isChecked())
  WRITE_STRING_SETTING(prefs, m_, StructureFile)

  WRITE_BOOL_SETTING(prefs, m_, WriteSurfaceVoxelScalars, true);
  WRITE_BOOL_SETTING(prefs, m_, WritePhaseIdScalars, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteIPFColorScalars, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true);

  WRITE_CHECKBOX_SETTING(prefs, m_, VisualizationVizFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, HDF5GrainFile)

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_StructureFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Structure File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Any File (*.*)") );
  if ( true == file.isEmpty() ){return;  }
  QFileInfo fi (file);
  QString ext = fi.suffix();
  m_StructureFile->setText(fi.absoluteFilePath());
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_StructureFile_textChanged(const QString &text)
{
  verifyPathExists(text, m_StructureFile);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::setupGui()
{
  QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
  m_H5InputStatisticsFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_H5InputStatisticsFile_textChanged(const QString &)));


  QR3DFileCompleter* com2 = new QR3DFileCompleter(this, true);
  m_OutputDir->setCompleter(com2);
  QObject::connect( com2, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_OutputDir_textChanged(const QString &)));

  QR3DFileCompleter* com3 = new QR3DFileCompleter(this, false);
  m_StructureFile->setCompleter(com3);
  QObject::connect( com3, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_StructureFile_textChanged(const QString &)));

  QString msg ("All files will be over written that appear in the output directory.");

  QFileInfo fi (m_OutputDir->text() + QDir::separator() +  DREAM3D::SyntheticBuilder::VisualizationVizFile.c_str() );
  if (m_AlreadyFormed->isChecked() == true && fi.exists() == false)
  {
    m_AlreadyFormed->setChecked(false);
  }

  m_WidgetList << m_H5InputStatisticsFile << m_InputH5StatisticsFileBtn << m_OutputDir << m_OutputDirBtn;
  m_WidgetList << m_XPoints << m_YPoints << m_ZPoints << m_XResolution << m_YResolution << m_ZResolution;
  m_WidgetList << m_NeighborhoodErrorWeight;

  m_WidgetList << m_HDF5GrainFile << m_VisualizationVizFile << m_VtkOptionsBtn;
  m_WidgetList << m_PeriodicBoundaryConditions << m_OutputFilePrefix;
  m_WidgetList << m_H5VoxelFile << m_GrainDataFile;

  m_StructureFile->setVisible(false);
  m_StructureFileBtn->setVisible(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_LoadSettingsBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Settings File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("Settings File (*.txt)") );
  if ( true == file.isEmpty() ){return;  }
  QSettings prefs(file, QSettings::IniFormat, this);
  readSettings(prefs);
  if (verifyPathExists(m_OutputDir->text(), m_OutputDir) )
  {
    checkIOFiles();
  }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_SaveSettingsBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "GrainGeneratorSettings.txt";
  QString file = QFileDialog::getSaveFileName(this, tr("Save Grain Generator Settings"),
                                              proposedFile,
                                              tr("*.txt") );
  if ( true == file.isEmpty() ){ return;  }

  QSettings prefs(file, QSettings::IniFormat, this);
  writeSettings(prefs);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::checkIOFiles()
{
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(DREAM3D::SyntheticBuilder, m_, GrainDataFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(DREAM3D::SyntheticBuilder, m_, H5VoxelFile)

  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(DREAM3D::SyntheticBuilder, m_ , VisualizationVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(DREAM3D::SyntheticBuilder, m_ , HDF5GrainFile)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void GrainGeneratorWidget::on_m_AlreadyFormed_stateChanged(int currentState)
//{
//
//  QString absPath = m_OutputDir->text() + QDir::separator() + AIM::SyntheticBuilder::VisualizationVizFile.c_str();
//  absPath = QDir::toNativeSeparators(absPath);
//  QFileInfo fi (absPath);
//  QString msg ("All files will be over written that appear in the output directory.");
//  if (m_AlreadyFormed->isChecked() == true && fi.exists() == false)
//  {
//    QMessageBox::critical(this, tr("DREAM.3D"),
//      tr("You have selected the 'Already Formed' check box \nbut the correct output file does not exist.\n"
//      "The checkbox will revert to an unchecked state.?"),
//      QMessageBox::Ok,
//      QMessageBox::Ok);
//      m_AlreadyFormed->setChecked(false);
//      CHECK_QLABEL_OUTPUT_FILE_EXISTS(AIM::SyntheticBuilder, m_, VisualizationVizFile)
//  }
//
//  if (m_AlreadyFormed->isChecked())
//  {
//    msg += QString("\nThe 'reconstructed_data.txt' file will be used as an import and NOT over written with new data");
//  }
//  messageLabel->setText(msg);
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_InputH5StatisticsFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("HDF5 Stats Files (*.h5stats *.h5 *.hdf5)") );
  if ( true == file.isEmpty() ){ return; }
    QFileInfo fi (file);
    m_H5InputStatisticsFile->blockSignals(true);
    QString p = QDir::toNativeSeparators(fi.absoluteFilePath());
    m_H5InputStatisticsFile->setText(p);
    on_m_H5InputStatisticsFile_textChanged(m_H5InputStatisticsFile->text() );
    m_H5InputStatisticsFile->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_H5InputStatisticsFile_textChanged(const QString &text)
{

  if (verifyPathExists(m_H5InputStatisticsFile->text(), m_H5InputStatisticsFile))
  {
    QFileInfo fi(m_H5InputStatisticsFile->text());
    QString outPath = fi.absolutePath() + QDir::separator() + fi.baseName() + "_GrainGenerator";
    outPath = QDir::toNativeSeparators(outPath);
    m_OutputDir->setText(outPath);
  }

  QFileInfo fi(m_H5InputStatisticsFile->text());
  if (fi.exists() && fi.isFile())
  {
    // Set the output file Prefix based on the name of the input file
    m_OutputFilePrefix->setText(fi.baseName() + QString("_"));

    // Open the HDF5 Stats file
    H5StatsReader::Pointer h5reader = H5StatsReader::New(m_H5InputStatisticsFile->text().toStdString());
    if(h5reader.get() == NULL)
    {
      return;
    }
    // Read the Phase and Crystal Structure information from the Stats File
    std::vector<int> phases;
    std::vector<unsigned int> structures;
    int err = h5reader->getPhaseAndCrystalStructures(phases, structures);
    if(err < 0)
    {
      return;
    }

    int size = static_cast<int>(phases.size());
    std::vector<std::string> shapeTypeStrings;
    ShapeType::getShapeTypeStrings(shapeTypeStrings);
    std::vector<unsigned int> shapeTypeEnums;
    ShapeType::getShapeTypeEnums(shapeTypeEnums);

    // Remove all the items from the GUI and from the internal tracking Lists
    QLayoutItem *child;
    while ((formLayout_2->count() > 0) && (child = formLayout_2->takeAt(0)) != 0)
    {
      delete child;
    }
    m_ShapeTypeLabels.clear();
    m_ShapeTypeCombos.clear();

    // Create a whole new QWidget to hold everything
    m_ShapeTypeScrollContents = new QWidget();
    m_ShapeTypeScrollContents->setObjectName(QString::fromUtf8("m_ShapeTypeScrollContents"));
    formLayout_2 = new QFormLayout(m_ShapeTypeScrollContents);
    formLayout_2->setContentsMargins(4, 4, 4, 4);
    formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
    formLayout_2->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    formLayout_2->setHorizontalSpacing(6);
    formLayout_2->setVerticalSpacing(6);
    m_ShapeTypeScrollArea->setWidget(m_ShapeTypeScrollContents);

    for (int i = 0; i < size; i++)
    {
      QLabel* shapeTypeLabel = new QLabel(m_ShapeTypeScrollContents);
      QString str("Phase ");
      str.append(QString::number(i+1, 10));
      str.append(":");
      shapeTypeLabel->setText(str);
      shapeTypeLabel->setObjectName(str);
      m_ShapeTypeLabels << shapeTypeLabel;

      formLayout_2->setWidget(i, QFormLayout::LabelRole, shapeTypeLabel);

      QComboBox* cb = new QComboBox(m_ShapeTypeScrollContents);
      str.append(" ComboBox");
      cb->setObjectName(str);
      for (size_t s = 0; s < shapeTypeStrings.size(); ++s)
      {
        cb->addItem(QString::fromStdString(shapeTypeStrings[s]), shapeTypeEnums[s]);
        cb->setItemData(s, shapeTypeEnums[s], Qt::UserRole);
      }
      m_ShapeTypeCombos << cb;
      formLayout_2->setWidget(i, QFormLayout::FieldRole, cb);
    }

    // Estimate the number of grains
    estimateNumGrainsSetup();

  }



}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_OutputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Grain Generator Output Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->m_OutputDir->setText(outputFile);
    if (verifyPathExists(outputFile, m_OutputDir) == true )
    {
      checkIOFiles();
      QFileInfo fi (m_OutputDir->text() + QDir::separator() +  DREAM3D::SyntheticBuilder::VisualizationVizFile.c_str() );
      if (m_AlreadyFormed->isChecked() == true && fi.exists() == false)
      {
        m_AlreadyFormed->setChecked(false);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_OutputDir_textChanged(const QString &text)
{
  if (verifyPathExists(m_OutputDir->text(), m_OutputDir) )
  {
    checkIOFiles();
  }
}

void GrainGeneratorWidget::on_m_GoBtn_clicked()
{

  if (m_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_FilterPipeline!= NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit cancelPipeline();
    }
    return;
  }


  if (false == sanityCheckOutputDirectory(m_OutputDir, QString("Grain Generator")) )
  {
    return;
  }
  SANITY_CHECK_INPUT(m_, OutputDir)

  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource

  if (NULL != m_FilterPipeline)
  {
    delete m_FilterPipeline;
    m_FilterPipeline = NULL;
  }

  // This method will create all the needed filters and push them into the
  // pipeline in the correct order
  setupPipeline();

  // instantiate a subclass of the FilterPipeline Wrapper which adds Qt Signal/Slots
  // to the FilterPipeline Class
  m_FilterPipeline->moveToThread(m_WorkerThread);


  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the FilterPipeline object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the Reconstruction going
  connect(m_WorkerThread, SIGNAL(started()),
          m_FilterPipeline, SLOT(run()));

  // When the FilterPipeline ends then tell the QThread to stop its event loop
  connect(m_FilterPipeline, SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( pipelineComplete() ) );

  // Send Progress from the Reconstruction to this object for display
  connect(m_FilterPipeline, SIGNAL (updateProgress(int)),
    this, SLOT(pipelineProgress(int) ) );

  // Send progress messages from FilterPipeline to this object for display
  connect(m_FilterPipeline, SIGNAL (progressMessage(QString)), this, SLOT(addProgressMessage(QString) ));

  // Send progress messages from FilterPipeline to this object for display
  connect(m_FilterPipeline, SIGNAL (warningMessage(QString)), this, SLOT(addWarningMessage(QString) ));

  // Send progress messages from FilterPipeline to this object for display
  connect(m_FilterPipeline, SIGNAL (errorMessage(QString)), this, SLOT(addErrorMessage(QString) ));

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelPipeline() ),
          m_FilterPipeline, SLOT (on_CancelWorker() ) , Qt::DirectConnection);

  // Now preflight this pipeline to make sure we can actually run it
  int err = m_FilterPipeline->preflightPipeline();
  // If any error occured during the preflight a dialog has been presented to the
  // user so simply exit
  if(err < 0)
  {
    delete m_FilterPipeline;
    m_FilterPipeline = NULL;
    // Show a Dialog with the error from the Preflight
    return;
  }
  DataContainer::Pointer m = DataContainer::New();

  m->setDimensions(m_XPoints->value(), m_YPoints->value(), m_ZPoints->value());
  m->setResolution(m_XResolution->value(), m_YResolution->value(), m_ZResolution->value());

  int count = m_ShapeTypeCombos.count();
  DataArray<unsigned int>::Pointer shapeTypes =
                   DataArray<unsigned int>::CreateArray(count+1);
  shapeTypes->SetValue(0, DREAM3D::ShapeType::UnknownShapeType);
  bool ok = false;
  for (int i = 0; i < count; ++i)
  {
    QComboBox* cb = m_ShapeTypeCombos.at(i);
    unsigned int enPtValue = static_cast<unsigned int>(cb->itemData(cb->currentIndex(), Qt::UserRole).toUInt(&ok));
    if (enPtValue >= DREAM3D::ShapeType::UnknownShapeType)
    {
      QString msg("The Shape Type for phase ");
//      msg.append(QString::number(i)).append(" is not set correctly. Please set the shape to Primary, Precipitate or Transformation.");
      msg.append(QString::number(i)).append(" is not set correctly. Please select a shape type from the combo box");
      QMessageBox::critical(this, QString("Grain Generator"), msg, QMessageBox::Ok | QMessageBox::Default);
      return;
    }
    shapeTypes->SetValue(i+1, enPtValue);
  }
  m->addEnsembleData(DREAM3D::EnsembleData::ShapeTypes, shapeTypes);
  m_FilterPipeline->setDataContainer(m);

  // Now actually execute the pipeline
  setWidgetListEnabled(false);
  emit pipelineStarted();
  m_WorkerThread->start();
  m_GoBtn->setText("Cancel");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::setupPipeline()
{
 // Create a FilterPipeline Object to hold all the filters. Later on we will execute all the filters
  m_FilterPipeline = new QFilterPipeline(NULL);

  std::string outDir = QDir::toNativeSeparators(m_OutputDir->text()).toStdString();
  std::string prefix = m_OutputFilePrefix->text().toStdString();

  if(m_AlreadyFormed->isChecked() == false)
  {
    PackGrainsGen2::Pointer pack_grains = PackGrainsGen2::New();
    pack_grains->setH5StatsInputFile(QDir::toNativeSeparators(m_H5InputStatisticsFile->text()).toStdString() );
    pack_grains->setPeriodicBoundaries(m_PeriodicBoundaryConditions->isChecked());
    pack_grains->setNeighborhoodErrorWeight(m_NeighborhoodErrorWeight->value());
#if PACK_GRAINS_ERROR_TXT_OUT
    MAKE_OUTPUT_FILE_PATH( errorFile, DREAM3D::SyntheticBuilder::ErrorFile, outDir, prefix)
    pack_grains->setErrorOutputFile(errorFile);
#endif
#if PACK_GRAINS_VTK_FILE_OUT
    MAKE_OUTPUT_FILE_PATH( vtkFile, DREAM3D::SyntheticBuilder::VtkFile, outDir, prefix)
    pack_grains->setVtkOutputFile(vtkFile);
#endif
    m_FilterPipeline->pushBack(pack_grains);

    AdjustVolume::Pointer adjust_grains = AdjustVolume::New();
    m_FilterPipeline->pushBack(adjust_grains);
  }
  else if(m_AlreadyFormed->isChecked() == true)
  {
    assert(false);
#if 0
    updateProgressAndMessage(("Reading Structure"), 40);
    std::string ext = MXAFileInfo::extension(m_StructureFile);
    if(ext.compare("vtk") == 0)
    {
      VTKStructureReader::Pointer reader = VTKStructureReader::New();
      reader->setInputFileName(m_StructureFile);
      reader->setGrainIdScalarName(DREAM3D::VTK::GrainIdScalarName);
      reader->setPhaseIdScalarName(DREAM3D::VTK::PhaseIdScalarName);
      err = reader->readStructure(m.get());
      CHECK_FOR_ERROR(DataContainer, "GrainGenerator Error getting size and resolution from VTK Voxel File", err);
    }
    else if(ext.compare("h5") == 0 || ext.compare("h5voxel") == 0)
    {
      // Load up the voxel data
      H5VoxelReader::Pointer h5Reader = H5VoxelReader::New();
      h5Reader->setFileName(m_StructureFile);
      int dims[3];
      float spacing[3];
      float origin[3];
      err = h5Reader->getSizeResolutionOrigin(dims, spacing, origin);

      CHECK_FOR_ERROR(DataContainer, "GrainGenerator Error getting size and resolution from HDF5 Voxel File", err);
      m->setDimensions(dims);
      //      m->getXPoints() = dims[0];
      //      m->getYPoints() = dims[1];
      //      m->getZPoints() = dims[2];
      //      m->totalpoints = dims[0] * dims[1] * dims[2];
      m->setResolution(spacing);
      m->getXRes() = spacing[0];
      m->getYRes() = spacing[1];
      m->getZRes() = spacing[2];

      updateProgressAndMessage(("Reading the Voxel Data from the HDF5 File"), 10);
      err = h5Reader->readVoxelData(m->m_GrainIndicies, m->m_Phases, m->m_Euler1s, m->m_Euler2s, m->m_Euler3s, m->crystruct, m->phaseType, m->totalPoints());
      CHECK_FOR_ERROR(DataContainer, "GrainGenerator Error reading voxel data from HDF5 Voxel File", err);
    }
    else
    {
      err = -1;
      CHECK_FOR_ERROR(DataContainer, "GrainGenerator Error No suitable Voxel Structure Reader found", err);
    }
#endif
  }

  if(m_AlreadyFormed->isChecked() == false)
  {
    PlacePrecipitates::Pointer place_precipitates = PlacePrecipitates::New();
    place_precipitates->setH5StatsInputFile(QDir::toNativeSeparators(m_H5InputStatisticsFile->text()).toStdString() );
    place_precipitates->setPeriodicBoundaries(m_PeriodicBoundaryConditions->isChecked());
    m_FilterPipeline->pushBack(place_precipitates);
  }

  MatchCrystallography::Pointer match_crystallography = MatchCrystallography::New();
  match_crystallography->setH5StatsInputFile(QDir::toNativeSeparators(m_H5InputStatisticsFile->text()).toStdString() );
  m_FilterPipeline->pushBack(match_crystallography);

  MAKE_OUTPUT_FILE_PATH( FieldDataFile, DREAM3D::SyntheticBuilder::GrainDataFile, outDir, prefix)
  FieldDataCSVWriter::Pointer write_fielddata = FieldDataCSVWriter::New();
  write_fielddata->setFieldDataFile(FieldDataFile);
  m_FilterPipeline->pushBack(write_fielddata);

  MAKE_OUTPUT_FILE_PATH( h5VoxelFile, DREAM3D::SyntheticBuilder::H5VoxelFile, outDir, prefix)
  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(h5VoxelFile);
  m_FilterPipeline->pushBack(writer);

  if(m_VisualizationVizFile->isChecked())
  {
    MAKE_OUTPUT_FILE_PATH( vtkVizFile, DREAM3D::Reconstruction::VisualizationVizFile, outDir, prefix);

    VtkRectilinearGridWriter::Pointer vtkWriter = VtkRectilinearGridWriter::New();
    vtkWriter->setOutputFile(vtkVizFile);
    vtkWriter->setWriteGrainIds(true);
    vtkWriter->setWritePhaseIds(m_WritePhaseIdScalars);
   // vtkWriter->setWriteGoodVoxels(m_WriteGoodVoxels);
    vtkWriter->setWriteIPFColors(m_WriteIPFColorScalars);
    vtkWriter->setWriteBinaryFile(m_WriteBinaryVTKFile);
    m_FilterPipeline->pushBack(vtkWriter);
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_XPoints_valueChanged(int v)
{
  estimateNumGrainsSetup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_YPoints_valueChanged(int v)
{
  estimateNumGrainsSetup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_ZPoints_valueChanged(int v)
{
  estimateNumGrainsSetup();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_ZResolution_valueChanged(double v)
{
  estimateNumGrainsSetup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_YResolution_valueChanged(double v)
{
  estimateNumGrainsSetup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_XResolution_valueChanged(double v)
{
  estimateNumGrainsSetup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::estimateNumGrainsSetup()
{
  int xpoints, ypoints, zpoints;
  float xres, yres, zres;
  xpoints = m_XPoints->value();
  ypoints = m_YPoints->value();
  zpoints = m_ZPoints->value();
  xres = m_XResolution->value();
  yres = m_YResolution->value();
  zres = m_ZResolution->value();
  int est_ngrains = estimate_numgrains(xpoints, ypoints, zpoints, xres, yres, zres);
  m_EstimatedGrains->setText(QString::number(est_ngrains));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GrainGeneratorWidget::estimate_numgrains(int xpoints, int ypoints, int zpoints, float xres, float yres, float zres)
{
  //TODO: We should read the file once and cache the results instead of reading every time

  float totalvol;
  int phase;
  std::vector<int> phases;
  std::vector<unsigned int> structures;
  std::vector<unsigned int> phaseType;
  std::vector<float> phasefraction;
  std::vector<float> double_data;
  std::vector<float> avgdiam;
  std::vector<float> sddiam;
  std::vector<float> grainDiamInfo;
  std::vector<float> maxdiameter;
  std::vector<float> mindiameter;

  totalvol = (xpoints * xres) * (ypoints * yres) * (zpoints * zres);
  H5StatsReader::Pointer h5Reader = H5StatsReader::New();
  h5Reader->setFileName(m_H5InputStatisticsFile->text().toStdString());
  int err = h5Reader->getPhaseAndCrystalStructures(phases, structures);
  if (err < 0 || phases.size() == 0)
  {
    return -1;
  }

  phasefraction.resize(phases.size()+1);
  phaseType.resize(phases.size()+1);
  avgdiam.resize(phases.size()+1);
  sddiam.resize(phases.size()+1);
  maxdiameter.resize(phases.size()+1);
  mindiameter.resize(phases.size()+1);
  for (size_t i = 0; i < phases.size(); i++)
  {
    phase = phases[i];

    std::vector<float> pFraction;
    err = h5Reader->readStatsDataset(phase, DREAM3D::HDF5::PhaseFraction, pFraction);
    if (err < 0) {break;}
    phasefraction[phase] = pFraction.front();

    std::vector<unsigned int> phasetypes;
    err = h5Reader->readStatsDataset(phase, DREAM3D::HDF5::PhaseType, phasetypes);
    if (err < 0) {break;}
    phaseType[phase] = static_cast<unsigned int> (phasetypes[0]);

	  err = h5Reader->readStatsDataset(phase, DREAM3D::HDF5::Grain_Size_Distribution, double_data);
    if (err < 0) {break;}
	  avgdiam[phase] = double_data[0];
	  sddiam[phase] = double_data[1];

	  err = h5Reader->readStatsDataset(phase, DREAM3D::HDF5::Grain_Diameter_Info, grainDiamInfo);
    if (err < 0) {break;}
	  maxdiameter[phase]  = grainDiamInfo[1];
	  mindiameter[phase] = grainDiamInfo[2];
  }
  // If we errored out trying to read the data from teh HDF5 file then bail out now with a Zero Grains found
  if (err < 0)
  {
    return 0;
  }
  DREAM3D_RANDOMNG_NEW()

  std::vector<int> primaryphases;
  std::vector<double> primaryphasefractions;
  double totalprimaryfractions = 0.0;
  // find which phases are primary phases
  for (size_t i = 1; i < phaseType.size(); ++i)
  {
    if (phaseType[i] == DREAM3D::PhaseType::PrimaryPhase)
    {
      primaryphases.push_back(i);
      primaryphasefractions.push_back(phasefraction[i]);
      totalprimaryfractions = totalprimaryfractions + phasefraction[i];
    }
  }
  // scale the primary phase fractions to total to 1
  for (size_t i = 0; i < primaryphasefractions.size(); i++)
  {
    primaryphasefractions[i] = primaryphasefractions[i] / totalprimaryfractions;
    if (i > 0) primaryphasefractions[i] = primaryphasefractions[i] + primaryphasefractions[i - 1];
   // if (i == 0) primaryphasefractions[i] = primaryphasefractions[i];
  }
  // generate the grains
  int gid = 1;

  float currentvol = 0.0;
  float vol, random;
  float diam;
  int volgood = 0;
  while (currentvol < totalvol)
  {
    volgood = 0;
    random = rg.genrand_res53();
    for (size_t j = 0; j < primaryphases.size(); ++j)
    {
      if(random < primaryphasefractions[j])
      {
        phase = primaryphases[j];
        break;
      }
    }
    while (volgood == 0)
    {
      volgood = 1;
     // u = rg.genrand_res53();
      diam = rg.genrand_norm(avgdiam[phase], sddiam[phase]);
      diam = exp(diam);
      if(diam >= maxdiameter[phase]) volgood = 0;
      if(diam < mindiameter[phase]) volgood = 0;
      vol = (4.0 / 3.0) * (m_pi) * ((diam * 0.5f) * (diam * 0.5f) * (diam * 0.5f));
    }
    currentvol = currentvol + vol;
    gid++;
  }
  return gid;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::pipelineComplete()
{
 // std::cout << "GrainGeneratorWidget::grainGenerator_Finished()" << std::endl;
  m_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->m_progressBar->setValue(0);
  emit pipelineEnded();
  checkIOFiles();
  m_FilterPipeline->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::pipelineProgress(int val)
{
  this->m_progressBar->setValue( val );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_VtkOptionsBtn_clicked()
{
  QVector<QString> options;
  options.push_back("Write Surface Voxel Scalars");
  options.push_back("Write Phase Ids Scalars");
  options.push_back("Write IPF Color Scalars");
  options.push_back("Write Binary VTK File");
  QCheckboxDialog d(options, this);
  d.setWindowTitle(QString("VTK Output Options"));

  d.setValue("Write Surface Voxel Scalars", m_WriteSurfaceVoxelScalars);
  d.setValue("Write Phase Ids Scalars", m_WritePhaseIdScalars);
  d.setValue("Write IPF Color Scalars", m_WriteIPFColorScalars);
  d.setValue("Write Binary VTK File", m_WriteBinaryVTKFile);

  int ret = d.exec();
  if (ret == QDialog::Accepted)
  {
    m_WriteSurfaceVoxelScalars = d.getValue("Write Surface Voxel Scalars");
    m_WritePhaseIdScalars = d.getValue("Write Phase Ids Scalars");
    m_WriteIPFColorScalars = d.getValue("Write IPF Color Scalars");
    m_WriteBinaryVTKFile = d.getValue("Write Binary VTK File");
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_HDF5GrainFile_stateChanged(int state)
{
  if (m_HDF5GrainFile->isChecked())
  {
    if (verifyPathExists(m_OutputDir->text(), m_OutputDir) == false) {
      QMessageBox::warning(this, tr("DREAM.3D"),
      tr("In order to render the data contained in the H5Grain file you will need a plugin for ParaView which is available separately."
          "\n  The raw data in the file can be viewed with any HDF5 data viewer. See http://www.hdfgroup.org for more details."),
      QMessageBox::Ok,
      QMessageBox::Ok);
      return;
      }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::on_m_OutputFilePrefix_textChanged(const QString &text)
{
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::addErrorMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::GrainGeneratorDisplayName).append(" Error");
  displayDialogBox(title, message, QMessageBox::Critical);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::addWarningMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::GrainGeneratorDisplayName).append(" Warning");
  displayDialogBox(title, message, QMessageBox::Warning);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::addProgressMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
  }
}
