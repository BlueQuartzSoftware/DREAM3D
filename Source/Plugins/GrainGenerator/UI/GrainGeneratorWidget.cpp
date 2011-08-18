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

#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/ShapeType.h"
#include "DREAM3D/HDF5/H5ReconStatsReader.h"
#include "DREAM3D/HDF5/H5ReconStatsWriter.h"

#include "QtSupport/DREAM3DQtMacros.h"
#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/QCheckboxDialog.h"

#include "GrainGenerator/GrainGenerator.h"

const static float m_pi = M_PI;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrainGeneratorWidget::GrainGeneratorWidget(QWidget *parent) :
Dream3DPluginFrame(parent),
m_GrainGenerator(NULL),
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
  READ_SETTING(prefs, m_, FillingErrorWeight, ok, d, 1.0 , Double);
  READ_SETTING(prefs, m_, NeighborhoodErrorWeight, ok, d, 1.0 , Double);
  READ_SETTING(prefs, m_, SizeDistErrorWeight, ok, d, 1.0 , Double);
  m_XPoints->blockSignals(true);
  READ_SETTING(prefs, m_, XPoints, ok, i, 100 , Int);
  m_XPoints->blockSignals(false);
  m_YPoints->blockSignals(true);
  READ_SETTING(prefs, m_, YPoints, ok, i, 100 , Int);
  m_YPoints->blockSignals(false);
  READ_SETTING(prefs, m_, ZPoints, ok, i, 100 , Int);

  READ_CHECKBOX_SETTING(prefs, m_, PeriodicBoundaryConditions, false);
  READ_CHECKBOX_SETTING(prefs, m_, GrainDataFile, false);
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
  READ_CHECKBOX_SETTING(prefs, m_, PhFile, true);

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
  WRITE_SETTING(prefs, m_, FillingErrorWeight )
  WRITE_SETTING(prefs, m_, NeighborhoodErrorWeight )
  WRITE_SETTING(prefs, m_, SizeDistErrorWeight )
  WRITE_SETTING(prefs, m_, XPoints )
  WRITE_SETTING(prefs, m_, YPoints )
  WRITE_SETTING(prefs, m_, ZPoints )

  WRITE_BOOL_SETTING(prefs, m_, PeriodicBoundaryConditions, m_PeriodicBoundaryConditions->isChecked())
  WRITE_BOOL_SETTING(prefs, m_, GrainDataFile, m_GrainDataFile->isChecked())
  WRITE_BOOL_SETTING(prefs, m_, AlreadyFormed, m_AlreadyFormed->isChecked())
  WRITE_STRING_SETTING(prefs, m_, StructureFile)

  WRITE_BOOL_SETTING(prefs, m_, WriteSurfaceVoxelScalars, true);
  WRITE_BOOL_SETTING(prefs, m_, WritePhaseIdScalars, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteIPFColorScalars, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true);

  WRITE_CHECKBOX_SETTING(prefs, m_, VisualizationVizFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, HDF5GrainFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, PhFile)

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
  messageLabel->setText("");

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
  m_WidgetList << m_XPoints << m_YPoints << m_ZPoints << m_XResolution << m_YResolution << m_ZResolution << m_FillingErrorWeight;
  m_WidgetList << m_NeighborhoodErrorWeight << m_SizeDistErrorWeight;

  m_WidgetList << m_PhFile << m_HDF5GrainFile << m_VisualizationVizFile << m_VtkOptionsBtn;
  m_WidgetList << m_PeriodicBoundaryConditions << m_GrainDataFile << m_OutputFilePrefix;
  m_WidgetList << m_H5VoxelFile << m_GrainAnglesFile;
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
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(DREAM3D::SyntheticBuilder, m_, GrainAnglesFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(DREAM3D::SyntheticBuilder, m_, H5VoxelFile)

  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(DREAM3D::SyntheticBuilder, m_ , VisualizationVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(DREAM3D::SyntheticBuilder, m_ , HDF5GrainFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(DREAM3D::SyntheticBuilder, m_ , PhFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(DREAM3D::MicroStats, m_ , GrainDataFile)
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
       H5ReconStatsReader::Pointer h5reader = H5ReconStatsReader::New(m_H5InputStatisticsFile->text().toStdString());
       if (h5reader.get() == NULL)
       {
         QMessageBox::critical(this, "Grain Generator",
                               "The input HDF5 Based Stats file could not be opened for reading.",
                               QMessageBox::Ok | QMessageBox::Default);
         return;
       }
       // Read the Phase and Crystal Structure information from the Stats File
       std::vector<int> phases;
       std::vector<Ebsd::CrystalStructure> structures;
       int err = h5reader->getPhaseAndCrystalStructures(phases, structures);
       if (err < 0)
       {
         QMessageBox::critical(this, "Grain Generator",
                               "The Phase information could not be read from the input HDF5 file.",
                               QMessageBox::Ok | QMessageBox::Default);
         return;
       }

       int size = phases.size();
       std::vector<std::string> shapeTypeStrings;
       DREAM3D::ShapeType::getShapeTypeStrings(shapeTypeStrings);
       std::vector<DREAM3D::SyntheticBuilder::ShapeType> shapeTypeEnums;
       DREAM3D::ShapeType::getShapeTypeEnums(shapeTypeEnums);


       // Remove all the items from the GUI and from the internal tracking Lists
       QLayoutItem *child;
       while ( (formLayout_2->count() > 0)  && (child = formLayout_2->takeAt(0)) != 0) {
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
         QString str ("Phase ");
         str.append(QString::number(i, 10));
         str.append(":");
         shapeTypeLabel->setText(str);
         shapeTypeLabel->setObjectName(str);
         m_ShapeTypeLabels << shapeTypeLabel;

         formLayout_2->setWidget(i, QFormLayout::LabelRole, shapeTypeLabel);

         QComboBox* cb = new QComboBox(m_ShapeTypeScrollContents);
         str.append(" ComboBox");
         cb->setObjectName(str);
         for(size_t s = 0; s < shapeTypeStrings.size(); ++s)
         {
           cb->addItem(QString::fromStdString( shapeTypeStrings[s]), shapeTypeEnums[s] );
           cb->setItemData(s, shapeTypeEnums[s], Qt::UserRole);
         }
         m_ShapeTypeCombos << cb;
         formLayout_2->setWidget(i, QFormLayout::FieldRole, cb);
       }


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
    if(m_GrainGenerator!= NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit cancelProcess();
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

  if (NULL != m_GrainGenerator)
  {
    delete m_GrainGenerator;
    m_GrainGenerator = NULL;
  }
  m_GrainGenerator = new QGrainGenerator(NULL);

  // Move the GrainGenerator object into the thread that we just created.
  m_GrainGenerator->moveToThread(m_WorkerThread);


  m_GrainGenerator->setH5StatsFile(QDir::toNativeSeparators(m_H5InputStatisticsFile->text()).toStdString() );
  m_GrainGenerator->setOutputDirectory(QDir::toNativeSeparators(m_OutputDir->text()).toStdString());
  m_GrainGenerator->setOutputFilePrefix(m_OutputFilePrefix->text().toStdString());
  m_GrainGenerator->setXPoints(m_XPoints->value());
  m_GrainGenerator->setYPoints(m_YPoints->value());
  m_GrainGenerator->setZPoints(m_ZPoints->value());

  m_GrainGenerator->setXResolution(m_XResolution->value());
  m_GrainGenerator->setYResolution(m_YResolution->value());
  m_GrainGenerator->setZResolution(m_ZResolution->value());
  m_GrainGenerator->setFillingErrorWeight(m_FillingErrorWeight->value());
  m_GrainGenerator->setNeighborhoodErrorWeight(m_NeighborhoodErrorWeight->value());
  m_GrainGenerator->setSizeDistErrorWeight(m_SizeDistErrorWeight->value());

  m_GrainGenerator->setPeriodicBoundary(m_PeriodicBoundaryConditions->isChecked());
  std::vector<DREAM3D::SyntheticBuilder::ShapeType> shapeTypes(1, DREAM3D::SyntheticBuilder::UnknownShapeType);
  int count = m_ShapeTypeCombos.count();
  bool ok = false;
  for (int i = 0; i < count; ++i)
  {
    QComboBox* cb = m_ShapeTypeCombos.at(i);
    DREAM3D::SyntheticBuilder::ShapeType enPtValue = static_cast<DREAM3D::SyntheticBuilder::ShapeType>(cb->itemData(cb->currentIndex(), Qt::UserRole).toUInt(&ok));
    if (enPtValue >= DREAM3D::SyntheticBuilder::UnknownShapeType)
    {
      QString msg("The Shape Type for phase ");
//      msg.append(QString::number(i)).append(" is not set correctly. Please set the shape to Primary, Precipitate or Transformation.");
      msg.append(QString::number(i)).append(" is not set correctly. Please select a shape type from the combo box");
      QMessageBox::critical(this, QString("Grain Generator"), msg, QMessageBox::Ok | QMessageBox::Default);
      return;
    }
    shapeTypes.push_back(enPtValue);
  }
  m_GrainGenerator->setShapeTypes(shapeTypes);


  m_GrainGenerator->setAlreadyFormed(m_AlreadyFormed->isChecked() );
  m_GrainGenerator->setStructureFile(m_StructureFile->text().toStdString());

  m_GrainGenerator->setWriteVtkFile(m_VisualizationVizFile->isChecked());
  m_GrainGenerator->setWriteSurfaceVoxel(m_WriteSurfaceVoxelScalars);
  m_GrainGenerator->setWritePhaseId(m_WritePhaseIdScalars);
  m_GrainGenerator->setWriteIPFColor(m_WriteIPFColorScalars);
  m_GrainGenerator->setWriteBinaryVTKFiles(m_WriteBinaryVTKFile);

  m_GrainGenerator->setWriteHDF5GrainFile(m_HDF5GrainFile->isChecked());
  m_GrainGenerator->setWritePhFile(m_PhFile->isChecked());

  m_GrainGenerator->setWriteGrainData(m_GrainDataFile->isChecked());

  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the Reconstruction going
  connect(m_WorkerThread, SIGNAL(started()),
          m_GrainGenerator, SLOT(run()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_GrainGenerator, SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( threadFinished() ) );

  // Send Progress from the Reconstruction to this object for display
  connect(m_GrainGenerator, SIGNAL (updateProgress(int)),
    this, SLOT(threadProgressed(int) ) );

  // Send progress messages from Reconstruction to this object for display
  connect(m_GrainGenerator, SIGNAL (updateMessage(QString)),
          this, SLOT(threadHasMessage(QString) ) );

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelProcess() ),
          m_GrainGenerator, SLOT (on_CancelWorker() ) , Qt::DirectConnection);


  setWidgetListEnabled(false);
  emit processStarted();
  m_WorkerThread->start();
  m_GoBtn->setText("Cancel");
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
  est_numgrains->setText(QString::number(est_ngrains));
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
  std::vector<Ebsd::CrystalStructure> structures;
  std::vector<DREAM3D::Reconstruction::PhaseType> phaseType;
  std::vector<float> phasefraction;
  std::vector<float> double_data;
  std::vector<float> avgdiam;
  std::vector<float> sddiam;
  std::vector<float> grainDiamInfo;
  std::vector<float> maxdiameter;
  std::vector<float> mindiameter;

  totalvol = (xpoints * xres) * (ypoints * yres) * (zpoints * zres);
  H5ReconStatsReader::Pointer h5Reader = H5ReconStatsReader::New();
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
    phaseType[phase] = static_cast<DREAM3D::Reconstruction::PhaseType> (phasetypes[0]);

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
  AIM_RANDOMNG_NEW()

  std::vector<int> primaryphases;
  std::vector<double> primaryphasefractions;
  double totalprimaryfractions = 0.0;
  // find which phases are primary phases
  for (size_t i = 1; i < phaseType.size(); ++i)
  {
    if (phaseType[i] == DREAM3D::Reconstruction::PrimaryPhase)
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
  float u, diam;
  int volgood = 0;
  while (currentvol < totalvol)
  {
    volgood = 0;
    random = rg.Random();
    for (size_t j = 0; j < primaryphases.size(); ++j)
    {
      if (random < primaryphasefractions[j])
      {
        phase = primaryphases[j];
        break;
      }
    }
	while(volgood == 0)
	{
	  volgood = 1;
	  u = rg.Random();
	  diam = rg.RandNorm(avgdiam[phase],sddiam[phase]);
	  diam = exp(diam);
	  if(diam >= maxdiameter[phase]) volgood = 0;
	  if(diam < mindiameter[phase]) volgood = 0;
	  vol = (4.0/3.0)*(m_pi)*((diam * 0.5f)*(diam * 0.5f)*(diam * 0.5f));
	}
    currentvol = currentvol + vol;
    gid++;
  }
  return gid;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::threadFinished()
{
 // std::cout << "GrainGeneratorWidget::grainGenerator_Finished()" << std::endl;
  m_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->m_progressBar->setValue(0);
  emit processEnded();
  checkIOFiles();
  m_GrainGenerator->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::threadProgressed(int val)
{
  this->m_progressBar->setValue( val );
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorWidget::threadHasMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
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
