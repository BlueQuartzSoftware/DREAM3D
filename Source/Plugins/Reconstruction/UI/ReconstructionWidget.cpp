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

#include "ReconstructionWidget.h"

// HDF5 Includes
#include "hdf5.h"

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
#include <QtGui/QMessageBox>
#include <QtGui/QAbstractItemDelegate>

#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/PhaseType.h"

#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/DREAM3DQtMacros.h"
#include "QtSupport/QCheckboxDialog.h"

#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"

#include "EbsdLib/H5EbsdVolumeInfo.h"

#include "Reconstruction/EbsdSupport/H5EbsdVolumeReader.h"
#include "Reconstruction/EbsdSupport/H5AngVolumeReader.h"
#include "Reconstruction/EbsdSupport/H5CtfVolumeReader.h"

#include "Reconstruction/UI/AngFilterFields.h"
#include "Reconstruction/UI/CtfFilterFields.h"
#include "Reconstruction/UI/QualityMetricTableModel.h"
#include "Reconstruction/QualityMetricFilter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionWidget::ReconstructionWidget(QWidget *parent) :
Dream3DPluginFrame(parent),
m_Reconstruction(NULL),
m_WorkerThread(NULL),
m_phaseTypeEdited(false),
m_WritePhaseIdScalars(true),
//m_WriteImageQualityScalars(true),
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
ReconstructionWidget::~ReconstructionWidget()
{
}


// -----------------------------------------------------------------------------
//  Read the prefs from the local storage file
// -----------------------------------------------------------------------------
void ReconstructionWidget::readSettings(QSettings &prefs)
{

  QString val;
  bool ok;
  qint32 i;
  double d;

  prefs.beginGroup("Reconstruction");
  READ_FILEPATH_SETTING(prefs, m_, H5InputFile, "");
  on_m_H5InputFile_textChanged(QString(""));

  READ_FILEPATH_SETTING(prefs, m_, OutputDir, "");
  READ_STRING_SETTING(prefs, m_, OutputFilePrefix, "Reconstruction_")

  READ_SETTING(prefs, m_, ZStartIndex, ok, i, 0, Int)
  READ_SETTING(prefs, m_, ZEndIndex, ok, i, 0, Int)

  READ_CHECKBOX_SETTING(prefs, m_, MergeColonies, false);
  READ_CHECKBOX_SETTING(prefs, m_, MergeTwins, false);
  READ_CHECKBOX_SETTING(prefs, m_, FillinSample, false);
  READ_COMBO_BOX(prefs, m_, AlignMeth)
  READ_COMBO_BOX(prefs, m_, Orientation)

  READ_SETTING(prefs, m_, MisOrientationTolerance, ok, d, 5.0 , Double);
  READ_SETTING(prefs, m_, MinAllowedGrainSize, ok, i, 8 , Int);
  READ_SETTING(prefs, m_, DownSampleFactor, ok, d, 1.0 , Double);

  READ_BOOL_SETTING(prefs, m_, WritePhaseIdScalars, true);
//  READ_BOOL_SETTING(prefs, m_, WriteImageQualityScalars, true);
  READ_BOOL_SETTING(prefs, m_, WriteIPFColorScalars, true);
  READ_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true);

  READ_CHECKBOX_SETTING(prefs, m_, VisualizationVizFile, true);
  READ_CHECKBOX_SETTING(prefs, m_, DownSampledVizFile, true);
  m_HDF5GrainFile->blockSignals(true);
  READ_CHECKBOX_SETTING(prefs, m_, HDF5GrainFile, true);
  m_HDF5GrainFile->blockSignals(false);
  READ_CHECKBOX_SETTING(prefs, m_, DxFile, true);
  READ_CHECKBOX_SETTING(prefs, m_, PhFile, true);

  ok = false;
  int filterCount = prefs.value("NumQualityFilters").toInt(&ok);
  if (false == ok) {filterCount = 0;}

  // Setup the TableModel with the list of Possible Fields
  QAbstractItemModel* model = m_QualityMetricTableView->model();
  // This first time through the model will be NULL that we get from the table view. This does a
  // simple swap with our own Table Model object. Multiple times through the model will be the same
  // so we do NOT need to delete the model
  m_QualityMetricTableView->setModel(m_QualityMetricTableModel);
  if (model != m_QualityMetricTableModel && model == NULL) {
    delete model; // Clean up this memory
  }

  if (NULL != m_QualityMetricTableModel)
  {
    // Remove any filters
    m_QualityMetricTableView->model()->removeRows(0, m_QualityMetricTableModel->rowCount());
    QVector<QString> fieldNames;
    QVector<float> fieldValues;
    QVector<QString> fieldOperators;

    // Add the proper amount of rows and get the values
    for (int r = 0; r < filterCount; ++r)
    {
      on_addQualityMetric_clicked();
      QString gName = "QualityFilter-"+QString::number(r);
      prefs.beginGroup(gName);

      fieldNames.push_back(prefs.value("Field").toString());
      float fieldValue = prefs.value("Value").toFloat(&ok);
      if (false == ok) {fieldValue = 0.0f;}
      fieldValues.push_back(fieldValue);
      fieldOperators.push_back(prefs.value("Operator").toString());
      prefs.endGroup();
    }

    m_QualityMetricTableModel->setTableData(fieldNames, fieldValues, fieldOperators);

  }

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//  Write our Prefs to file
// -----------------------------------------------------------------------------
void ReconstructionWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("Reconstruction");
  WRITE_STRING_SETTING(prefs, m_, H5InputFile)
  WRITE_STRING_SETTING(prefs, m_, OutputDir)
  WRITE_STRING_SETTING(prefs, m_, OutputFilePrefix)
  WRITE_SETTING(prefs, m_, ZStartIndex)
  WRITE_SETTING(prefs, m_, ZEndIndex)


  WRITE_CHECKBOX_SETTING(prefs, m_, MergeTwins)
  WRITE_CHECKBOX_SETTING(prefs, m_, MergeColonies)
  WRITE_CHECKBOX_SETTING(prefs, m_, FillinSample)

  WRITE_SETTING(prefs, m_, MinAllowedGrainSize)
  WRITE_SETTING(prefs, m_, MisOrientationTolerance)
  WRITE_COMBO_BOX(prefs, m_, AlignMeth)
  WRITE_COMBO_BOX(prefs, m_, Orientation)


  WRITE_CHECKBOX_SETTING(prefs, m_, VisualizationVizFile)
  WRITE_BOOL_SETTING(prefs, m_, WritePhaseIdScalars, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteIPFColorScalars, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true);

  WRITE_CHECKBOX_SETTING(prefs, m_, DownSampledVizFile)
  WRITE_SETTING(prefs, m_, DownSampleFactor)
  WRITE_CHECKBOX_SETTING(prefs, m_, HDF5GrainFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, DxFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, PhFile)

  if (NULL != m_QualityMetricTableView->model())
  {
    int filterCount = m_QualityMetricTableView->model()->rowCount();
    prefs.setValue("NumQualityFilters", filterCount);
    QVector<QString> fieldNames;
    QVector<float> fieldValues;
    QVector<QString> fieldOperators;
    m_QualityMetricTableModel->getTableData(fieldNames, fieldValues, fieldOperators);

    for(int i = 0; i < filterCount; ++i)
    {
      QString gName = "QualityFilter-"+QString::number(i);
      prefs.beginGroup(gName);
      prefs.setValue("Field", fieldNames[i]);
      prefs.setValue("Value", fieldValues[i]);
      prefs.setValue("Operator", fieldOperators[i]);
      prefs.endGroup();
    }
  }
  else
  {
    prefs.setValue("NumQualityFilters", 0);
  }

  prefs.endGroup();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList) {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::setupGui()
{

  QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
  m_H5InputFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_H5InputFile_textChanged(const QString &)));

  QR3DFileCompleter* com2 = new QR3DFileCompleter(this, true);
  m_OutputDir->setCompleter(com2);
  QObject::connect( com2, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_OutputDir_textChanged(const QString &)));


  QString msg ("All files will be over written that appear in the output directory.");

  QFileInfo fi (m_OutputDir->text() + QDir::separator() +  DREAM3D::Reconstruction::VisualizationVizFile.c_str() );


  m_WidgetList << m_H5InputFile << m_OutputDir << m_OutputDirBtn << m_OutputFilePrefix;
  m_WidgetList << m_ZStartIndex << m_ZEndIndex;
  m_WidgetList << m_MergeTwins << m_MergeColonies << m_FillinSample << m_AlignMeth << m_Orientation;
  m_WidgetList << m_MinAllowedGrainSize << m_DownSampleFactor << m_MisOrientationTolerance;
  m_WidgetList << m_VisualizationVizFile << m_DownSampledVizFile;
  m_WidgetList << m_H5VoxelFile << m_VtkOptionsBtn;
  m_WidgetList << m_HDF5GrainFile << m_PhFile << m_DxFile;
  m_WidgetList << m_LoadSettingsBtn << m_SaveSettingsBtn << phaseTypeList;

  m_QualityMetricTableModel = new QualityMetricTableModel;

}


// -----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------_
void ReconstructionWidget::checkIOFiles()
{
  this->verifyPathExists(m_OutputDir->text(), this->m_OutputDir);

  CHECK_QLABEL_OUTPUT_FILE_EXISTS(DREAM3D::Reconstruction,m_, H5VoxelFile)

  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(DREAM3D::Reconstruction, m_ , VisualizationVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(DREAM3D::Reconstruction, m_ , DownSampledVizFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(DREAM3D::Reconstruction, m_ , HDF5GrainFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(DREAM3D::Reconstruction, m_ , DxFile)
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(DREAM3D::SyntheticBuilder, m_ , PhFile)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_SaveSettingsBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "ReconstructionSettings.txt";
  QString file = QFileDialog::getSaveFileName(this, tr("Save Reconstruction Settings"),
                                              proposedFile,
                                              tr("*.txt") );
  if ( true == file.isEmpty() ){ return;  }

  QSettings prefs(file, QSettings::IniFormat, this);
  writeSettings(prefs);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_LoadSettingsBtn_clicked()
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
void ReconstructionWidget::on_m_OIMH5Btn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("HDF5 EBSD Files (*.h5 *.hdf5 *.h5ang *.h5ebsd)") );
  if ( true == file.isEmpty() ){ return; }
  QFileInfo fi (file);
  m_H5InputFile->blockSignals(true);
  QString p = QDir::toNativeSeparators(fi.absoluteFilePath());
  m_H5InputFile->setText(p);
  on_m_H5InputFile_textChanged(m_H5InputFile->text() );
  m_H5InputFile->blockSignals(false);

}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string ReconstructionWidget::getEbsdManufacturer(const std::string &ebsdFile)
{
  std::string data;
  hid_t fileId = H5Utilities::openFile(ebsdFile, true);
  if (fileId < 0)
  {
    return data;
  }

  int err = H5Lite::readStringDataset(fileId, Ebsd::H5::Manufacturer, data);
  if (err < 0)
  {
    data = "";
    err = H5Utilities::closeFile(fileId);
    return data;
  }

  err = H5Utilities::closeFile(fileId);
  return data;
}
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_H5InputFile_textChanged(const QString &text)
{

  if (verifyPathExists(m_H5InputFile->text(), m_H5InputFile) )
  {
    QFileInfo fi(m_H5InputFile->text());
    QString outPath = fi.absolutePath() + QDir::separator() + fi.baseName() + "_Reconstruction";
    outPath = QDir::toNativeSeparators(outPath);
    m_OutputDir->setText(outPath);
  }

  QFileInfo fi(m_H5InputFile->text());
  if (fi.exists() && fi.isFile())
  {
    m_SetSliceInfo();
    // Set the output file Prefix based on the name of the input file
    m_OutputFilePrefix->setText(fi.baseName() + QString("_") );

    // Read the Phase information from the .h5ang file
    H5EbsdVolumeReader::Pointer h5Reader = H5EbsdVolumeReader::New();
    h5Reader->setFilename(m_H5InputFile->text().toStdString() );
    h5Reader->setSliceStart(m_ZStartIndex->value());

    int size = h5Reader->getNumPhases();

    std::vector<std::string> phaseTypeStrings;
    DREAM3D::PhaseType::getPhaseTypeStrings(phaseTypeStrings);
    std::vector<DREAM3D::Reconstruction::PhaseType> phaseTypeEnums;
    DREAM3D::PhaseType::getPhaseTypeEnums(phaseTypeEnums);

    // Remove all the items
    phaseTypeList->clear();
    // Now iterate over all the phases creating the proper UI elements
    for (int i = 0; i < size; i++)
    {

      phaseTypeList->addItem(DREAM3D::PhaseType::PrimaryStr().c_str());
      QListWidgetItem* item = phaseTypeList->item(i);
      item->setSizeHint(QSize(50, 25));
      QComboBox* cb = new QComboBox(phaseTypeList);
      for(size_t i = 0; i < phaseTypeStrings.size(); ++i)
      {
        cb->addItem(QString::fromStdString( phaseTypeStrings[i]), phaseTypeEnums[i] );
        cb->setItemData(i, phaseTypeEnums[i], Qt::UserRole);
      }
      cb->setMinimumHeight(25);
      phaseTypeList->setItemWidget(item, cb);
      connect(cb, SIGNAL(currentIndexChanged(int)),
              this, SLOT(phaseTypeEdited(int)));
    }

    int err = 0;
    int xpoints = 0;
    int ypoints = 0;
    int zpoints = 1;
    float xres = 0.0f;
    float yres = 0.0f;
    float zres = 0.0f;
    int zStart = 0;
    int zEnd = 0;

    // Read these values from the h5ebsd file in order to put something in the fields.
    zStart = h5Reader->getZStart();
    zEnd = h5Reader->getZEnd();
    err = h5Reader->getResolution(xres, yres, zres);

    h5Reader->setSliceStart(zStart);
    h5Reader->setSliceEnd(zEnd);
    zpoints = 1; // Just read the first dataset and hope the rest are the same
    err = h5Reader->getDimsAndResolution(xpoints, ypoints, zpoints, xres, yres, zres);
    if (err >= 0)
    {
      xDim->setText(QString::number(xpoints));
      yDim->setText(QString::number(ypoints));
      zDim->setText(QString::number(zpoints));
      m_XRes->setText(QString::number(xres));
      m_YRes->setText(QString::number(yres));
      m_ZRes->setText(QString::number(zres));
    }

    // Setup the TableModel with the list of Possible Fields
    QAbstractItemModel* model = m_QualityMetricTableView->model();
    // This first time through the model will be NULL that we get from the table view. This does a
    // simple swap with our own Table Model object. Multiple times through the model will be the same
    // so we do NOT need to delete the model
    m_QualityMetricTableView->setModel(m_QualityMetricTableModel);
    if (model != m_QualityMetricTableModel && model == NULL) {
      delete model; // Clean up this memory
    }

    // Compare the Manufactureres of the current file versus the one we have cached
    // If they are different then we need to remove all the quality filters
    QString fileManufact = QString::fromStdString(h5Reader->getManufacturer());
    if (m_EbsdManufacturer->text().compare(fileManufact) != 0)
    {
      m_QualityMetricTableModel->removeRows(0, m_QualityMetricTableModel->rowCount());
    }

    // Cache the Manufacturer from the File
    m_EbsdManufacturer->setText(fileManufact);


    // Get the list of Possible filter Fields based on the Manufacturer
    if (m_EbsdManufacturer->text().compare(QString(Ebsd::Ang::Manufacturer.c_str())) == 0)
    {
      AngFilterFields fields;
      m_QualityMetricTableModel->setPossibleFields(fields.getFieldNames());
    }
    else if (m_EbsdManufacturer->text().compare(QString(Ebsd::Ctf::Manufacturer.c_str())) == 0)
    {
      CtfFilterFields fields;
      m_QualityMetricTableModel->setPossibleFields(fields.getFieldNames());
    }
    // Set the ItemDelegate for the table.
    QAbstractItemDelegate* aid = m_QualityMetricTableModel->getItemDelegate();
    m_QualityMetricTableView->setItemDelegate(aid);

    // Make sure at least 1 Quality Metric is available.
    if (m_QualityMetricTableModel->rowCount() < 1)
    {
      on_addQualityMetric_clicked();
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_addQualityMetric_clicked()
{
  if (!m_QualityMetricTableModel->insertRow(m_QualityMetricTableModel->rowCount())) return;

  QModelIndex index = m_QualityMetricTableModel->index(m_QualityMetricTableModel->rowCount() - 1, 0);
  m_QualityMetricTableView->setCurrentIndex(index);
  m_QualityMetricTableView->resizeColumnsToContents();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_removeQualityMetric_clicked()
{
  std::cout << "on_removeQualityMetric_clicked" << std::endl;
  QItemSelectionModel *selectionModel = m_QualityMetricTableView->selectionModel();
  if (!selectionModel->hasSelection()) return;
  QModelIndex index = selectionModel->currentIndex();
  if (!index.isValid()) return;
  m_QualityMetricTableModel->removeRow(index.row(), index.parent());
  if (m_QualityMetricTableModel->rowCount() > 0)
  {
    m_QualityMetricTableView->resizeColumnsToContents();
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_OutputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Output Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->m_OutputDir->setText(outputFile);
    if (verifyPathExists(outputFile, m_OutputDir) == true )
    {
      checkIOFiles();
      QFileInfo fi (m_OutputDir->text() + QDir::separator() +  DREAM3D::Reconstruction::VisualizationVizFile.c_str() );
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::m_SetSliceInfo()
{
  H5EbsdVolumeInfo::Pointer reader = H5EbsdVolumeInfo::New();

  QFileInfo fi(m_H5InputFile->text());
  if (fi.isFile() == false)
  {
    return;
  }

  reader->setFilename(m_H5InputFile->text().toStdString());
  if (reader->readVolumeInfo() >= 0)
  {
    float x, y, z;
    reader->getResolution(x, y, z);
    m_ZStartIndex->setValue(reader->getZStart());
    m_ZStartIndex->setRange(reader->getZStart(), reader->getZEnd());
    m_ZEndIndex->setValue(reader->getZEnd());
    m_ZEndIndex->setRange(reader->getZStart(), reader->getZEnd());
    m_ZRes->setText(QString::number(z));
    m_ZMin->setText(QString::number(reader->getZStart()));
    m_ZMax->setText(QString::number(reader->getZEnd()));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::phaseTypeEdited(int i)
{
  m_phaseTypeEdited = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ReconstructionWidget::checkPhaseTypes()
{


  if (m_phaseTypeEdited == true)
  {
    int
        ret =
            QMessageBox::warning(this, QString("Reconstruction"), "The Phase Types were Edited. We need to save this data to the input file. Do you want to do that now. Canceling will leave all files untouched but NOT execute the Reconstruction", QMessageBox::Ok
                | QMessageBox::Default, QMessageBox::No);
    if (ret == QMessageBox::No)
    {
      return false;
    }
    else
    {
      std::cout << "We need to save the Phase Type to something?" << std::endl;
      return false;
    }

  }
  return true;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_OutputDir_textChanged(const QString & text)
{
  verifyPathExists(m_OutputDir->text(), m_OutputDir);
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_OutputFilePrefix_textChanged(const QString &text)
{
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_GoBtn_clicked()
{
  if (m_GoBtn->text().compare("Cancel") == 0)
  {
    if(m_Reconstruction != NULL)
    {
      std::cout << "canceling from GUI...." << std::endl;
      emit cancelProcess();
    }
    return;
  }

  // Make sure there is at least 1 Quality Metric Filters setup
  int qualMetricCount = m_QualityMetricTableModel->rowCount();
  if (qualMetricCount < 1) {
    QMessageBox::critical(this, tr("DREAM.3D"),
      tr("You must add at least 1 Quality Metric"),
      QMessageBox::Ok,
      QMessageBox::Ok);
      return;
  }

  // Make sure we have an output directory setup
  if (false == sanityCheckOutputDirectory(m_OutputDir, QString("Reconstruction")) )
  {
    return;
  }
  SANITY_CHECK_INPUT(m_ , OutputDir)

  std::vector<DREAM3D::Reconstruction::PhaseType> phaseTypes(1, DREAM3D::Reconstruction::UnknownPhaseType);
  int count = phaseTypeList->count();
  bool ok = false;
  for (int i = 0; i < count; ++i)
  {
    QListWidgetItem* item = phaseTypeList->item(i);
    QComboBox* cb = qobject_cast<QComboBox*> (phaseTypeList->itemWidget(item));
    DREAM3D::Reconstruction::PhaseType enPtValue = static_cast<DREAM3D::Reconstruction::PhaseType>(cb->itemData(cb->currentIndex(), Qt::UserRole).toUInt(&ok));
    if (enPtValue >= DREAM3D::Reconstruction::UnknownPhaseType)
    {
      QString msg("The Phase Type for phase ");
//      msg.append(QString::number(i)).append(" is not set correctly. Please set the phase to Primary, Precipitate or Transformation.");
      msg.append(QString::number(i)).append(" is not set correctly. Please set the phase to Primary or Precipitate.");
      QMessageBox::critical(this, QString("Grain Generator"), msg, QMessageBox::Ok | QMessageBox::Default);
      return;
    }
    phaseTypes.push_back(enPtValue);
  }

  ok = checkPhaseTypes();

  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource

  m_Reconstruction = new QReconstruction(NULL);

  // Move the Reconstruction object into the thread that we just created.
  m_Reconstruction->moveToThread(m_WorkerThread);

  m_Reconstruction->setH5AngFile( QDir::toNativeSeparators(m_H5InputFile->text()).toStdString());

  m_Reconstruction->setZStartIndex(m_ZStartIndex->value());
  m_Reconstruction->setZEndIndex(m_ZEndIndex->value());
  m_Reconstruction->setPhaseTypes(phaseTypes);

  m_Reconstruction->setMergeColonies(m_MergeColonies->isChecked() );
  m_Reconstruction->setMergeTwins(m_MergeTwins->isChecked() );
  m_Reconstruction->setFillinSample(m_FillinSample->isChecked() );

  DREAM3D::Reconstruction::AlignmentMethod alignmeth = static_cast<DREAM3D::Reconstruction::AlignmentMethod>(m_AlignMeth->currentIndex() );
  m_Reconstruction->setAlignmentMethod(alignmeth);

  Ebsd::Orientation orientation = static_cast<Ebsd::Orientation>(m_Orientation->currentIndex());
  m_Reconstruction->setOrientation(orientation);

  m_Reconstruction->setMinAllowedGrainSize(m_MinAllowedGrainSize->value());
  m_Reconstruction->setMisorientationTolerance(m_MisOrientationTolerance->value());
  m_Reconstruction->setDownSampleFactor(m_DownSampleFactor->value());

  m_Reconstruction->setOutputDirectory(QDir::toNativeSeparators(m_OutputDir->text()).toStdString());
  m_Reconstruction->setOutputFilePrefix(m_OutputFilePrefix->text().toStdString());

  m_Reconstruction->setWriteVtkFile(m_VisualizationVizFile->isChecked());
  m_Reconstruction->setWritePhaseId(m_WritePhaseIdScalars);
//  m_Reconstruction->setWriteImageQuality(m_WriteImageQualityScalars);
  m_Reconstruction->setWriteIPFColor(m_WriteIPFColorScalars);
  m_Reconstruction->setWriteBinaryVTKFiles(m_WriteBinaryVTKFile);

  m_Reconstruction->setWriteDownSampledFile(m_DownSampledVizFile->isChecked());
  m_Reconstruction->setWriteHDF5GrainFile(m_HDF5GrainFile->isChecked());
  m_Reconstruction->setWriteDxFile(m_DxFile->isChecked());
  m_Reconstruction->setWritePhFile(m_PhFile->isChecked());

  setupQualityMetricFilters(m_Reconstruction);



  // m_Reconstruction->printSettings(std::cout);

  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the Reconstruction going
  connect(m_WorkerThread, SIGNAL(started()),
          m_Reconstruction, SLOT(run()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_Reconstruction, SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( threadFinished() ) );

  // Send Progress from the Reconstruction to this object for display
  connect(m_Reconstruction, SIGNAL (updateProgress(int)),
    this, SLOT(threadProgressed(int) ) );

  // Send progress messages from Reconstruction to this object for display
  connect(m_Reconstruction, SIGNAL (updateMessage(QString)),
          this, SLOT(threadHasMessage(QString) ) );

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelProcess() ),
          m_Reconstruction, SLOT (on_CancelWorker() ) , Qt::DirectConnection);


  setWidgetListEnabled(false);
  emit processStarted();
  m_WorkerThread->start();
  m_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::setupQualityMetricFilters(QReconstruction* r)
{
  int filterCount = m_QualityMetricTableModel->rowCount();
  std::vector<QualityMetricFilter::Pointer> filters;
  QVector<QString> fieldNames;
  QVector<float> fieldValues;
  QVector<QString> fieldOperators;
  m_QualityMetricTableModel->getTableData(fieldNames, fieldValues, fieldOperators);

  for(int i = 0; i < filterCount; ++i)
  {
    QualityMetricFilter::Pointer filter = QualityMetricFilter::New();
    filter->setFieldName(fieldNames[i].toStdString());
    filter->setFieldValue(fieldValues[i]);
    filter->setFieldOperator(fieldOperators[i].toStdString());
    filters.push_back(filter);
  }

  r->setQualityMetricFilters(filters);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::threadFinished()
{
 // std::cout << "ReconstructionWidget::threadFinished()" << std::endl;
  m_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->progressBar->setValue(0);
  emit processEnded();
  checkIOFiles();
  m_Reconstruction->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::threadProgressed(int val)
{
  this->progressBar->setValue( val );
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::threadHasMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_VtkOptionsBtn_clicked()
{
  QVector<QString> options;
  options.push_back("Write Phase Ids Scalars");
//  options.push_back("Write Image Quality Scalars");
  options.push_back("Write IPF Color Scalars");
  options.push_back("Write Binary VTK File");
  QCheckboxDialog d(options, this);
  d.setWindowTitle(QString("VTK Output Options"));

  d.setValue("Write Phase Ids Scalars", m_WritePhaseIdScalars);
//  d.setValue("Write Image Quality Scalars", m_WriteImageQualityScalars);
  d.setValue("Write IPF Color Scalars", m_WriteIPFColorScalars);
  d.setValue("Write Binary VTK File", m_WriteBinaryVTKFile);

  int ret = d.exec();
  if (ret == QDialog::Accepted)
  {
    m_WritePhaseIdScalars = d.getValue("Write Phase Ids Scalars");
//    m_WriteImageQualityScalars = d.getValue("Write Image Quality Scalars");
    m_WriteIPFColorScalars = d.getValue("Write IPF Color Scalars");
    m_WriteBinaryVTKFile = d.getValue("Write Binary VTK File");
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_HDF5GrainFile_stateChanged(int state)
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

