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

#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/DREAM3DQtMacros.h"
#include "QtSupport/QCheckboxDialog.h"

#include "MXA/Utilities/MXADir.h"

#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/H5EbsdVolumeInfo.h"
#include "EbsdLib/QualityMetricFilter.h"
#include "EbsdLib/H5EbsdVolumeReader.h"
#include "EbsdLib/TSL/H5AngVolumeReader.h"
#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/HKL/H5CtfVolumeReader.h"
#include "EbsdLib/HKL/CtfFields.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/PhaseType.h"
#include "DREAM3DLib/GenericFilters/DataContainerWriter.h"
#include "DREAM3DLib/GenericFilters/VtkRectilinearGridWriter.h"
#include "DREAM3DLib/ReconstructionFilters/LoadSlices.h"
#include "DREAM3DLib/ReconstructionFilters/AlignSections.h"
#include "DREAM3DLib/ReconstructionFilters/SegmentGrains.h"
#include "DREAM3DLib/ReconstructionFilters/CleanupGrains.h"
#include "DREAM3DLib/ReconstructionFilters/MergeTwins.h"
#include "DREAM3DLib/ReconstructionFilters/MergeColonies.h"


#include "Reconstruction/UI/QualityMetricTableModel.h"
#include "ReconstructionPlugin.h"

#define MAKE_OUTPUT_FILE_PATH(outpath, filename, outdir, prefix)\
    std::string outpath = outdir + MXADir::Separator + prefix + filename;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReconstructionWidget::ReconstructionWidget(QWidget *parent) :
DREAM3DPluginFrame(parent),
m_FilterPipeline(NULL),
m_WorkerThread(NULL),
m_phaseTypeEdited(false),
m_WritePhaseIdScalars(true),
m_WriteGoodVoxelsScalars(true),
m_WriteIPFColorScalars(true),
m_WriteBinaryVTKFile(true),
rotateslice(false),
reorderarray(false),
aligneulers(false),
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
  READ_FILEPATH_SETTING(prefs, m_, H5EbsdFile, "");
  on_m_H5EbsdFile_textChanged(QString(""));

  READ_FILEPATH_SETTING(prefs, m_, OutputDir, "");
  READ_STRING_SETTING(prefs, m_, OutputFilePrefix, "Reconstruction_")

  READ_SETTING(prefs, m_, ZStartIndex, ok, i, 0, Int)
  READ_SETTING(prefs, m_, ZEndIndex, ok, i, 0, Int)

  READ_CHECKBOX_SETTING(prefs, m_, MergeColonies, false);
  READ_CHECKBOX_SETTING(prefs, m_, MergeTwins, false);
  READ_COMBO_BOX(prefs, m_, AlignMeth)

  READ_SETTING(prefs, m_, MisOrientationTolerance, ok, d, 5.0 , Double);
  READ_SETTING(prefs, m_, MinAllowedGrainSize, ok, i, 8 , Int);

  READ_BOOL_SETTING(prefs, m_, WritePhaseIdScalars, true);
  READ_BOOL_SETTING(prefs, m_, WriteGoodVoxelsScalars, true);
  READ_BOOL_SETTING(prefs, m_, WriteIPFColorScalars, true);
  READ_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true);

  READ_CHECKBOX_SETTING(prefs, m_, VisualizationVizFile, true);
  m_HDF5GrainFile->blockSignals(true);
  READ_CHECKBOX_SETTING(prefs, m_, HDF5GrainFile, true);
  m_HDF5GrainFile->blockSignals(false);

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
  WRITE_STRING_SETTING(prefs, m_, H5EbsdFile)
  WRITE_STRING_SETTING(prefs, m_, OutputDir)
  WRITE_STRING_SETTING(prefs, m_, OutputFilePrefix)
  WRITE_SETTING(prefs, m_, ZStartIndex)
  WRITE_SETTING(prefs, m_, ZEndIndex)

  WRITE_CHECKBOX_SETTING(prefs, m_, MergeTwins)
  WRITE_CHECKBOX_SETTING(prefs, m_, MergeColonies)

  WRITE_SETTING(prefs, m_, MinAllowedGrainSize)
  WRITE_SETTING(prefs, m_, MisOrientationTolerance)
  WRITE_COMBO_BOX(prefs, m_, AlignMeth)

  WRITE_CHECKBOX_SETTING(prefs, m_, VisualizationVizFile)
  WRITE_BOOL_SETTING(prefs, m_, WritePhaseIdScalars, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteGoodVoxelsScalars, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteIPFColorScalars, true);
  WRITE_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true);

  WRITE_CHECKBOX_SETTING(prefs, m_, HDF5GrainFile)

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
  m_H5EbsdFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_H5EbsdFile_textChanged(const QString &)));

  QR3DFileCompleter* com2 = new QR3DFileCompleter(this, true);
  m_OutputDir->setCompleter(com2);
  QObject::connect( com2, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_OutputDir_textChanged(const QString &)));


  QString msg ("All files will be over written that appear in the output directory.");

  //QFileInfo fi (m_OutputDir->text() + QDir::separator() +  DREAM3D::Reconstruction::VisualizationVizFile.c_str() );


  m_WidgetList << m_H5EbsdFile << m_OutputDir << m_OutputDirBtn << m_OutputFilePrefix;
  m_WidgetList << m_ZStartIndex << m_ZEndIndex;
  m_WidgetList << m_MergeTwins << m_MergeColonies << m_AlignMeth;
  m_WidgetList << m_MinAllowedGrainSize << m_MisOrientationTolerance;
  m_WidgetList << m_VisualizationVizFile;
  m_WidgetList << m_H5VoxelFile << m_VtkOptionsBtn;
  m_WidgetList << m_HDF5GrainFile;
  m_WidgetList << m_LoadSettingsBtn << m_SaveSettingsBtn << phaseTypeList;
  m_WidgetList << m_QualityMetricTableView << addQualityMetric << removeQualityMetric;
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
  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(DREAM3D::Reconstruction, m_ , HDF5GrainFile)
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
void ReconstructionWidget::on_m_H5EbsdBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("HDF5 EBSD Files (*.h5 *.hdf5 *.h5ang *.h5ebsd)") );
  if ( true == file.isEmpty() ){ return; }
  QFileInfo fi (file);
  m_H5EbsdFile->blockSignals(true);
  QString p = QDir::toNativeSeparators(fi.absoluteFilePath());
  m_H5EbsdFile->setText(p);
  on_m_H5EbsdFile_textChanged(m_H5EbsdFile->text() );
  m_H5EbsdFile->blockSignals(false);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_H5EbsdFile_textChanged(const QString &text)
{

  if(verifyPathExists(m_H5EbsdFile->text(), m_H5EbsdFile))
  {
    QFileInfo fi(m_H5EbsdFile->text());
    QString outPath = fi.absolutePath() + QDir::separator() + fi.baseName() + "_Reconstruction";
    outPath = QDir::toNativeSeparators(outPath);
    m_OutputDir->setText(outPath);
  }

  QFileInfo fi(m_H5EbsdFile->text());
  if(fi.exists() && fi.isFile())
  {
    m_SetSliceInfo();
    // Set the output file Prefix based on the name of the input file
    m_OutputFilePrefix->setText(fi.baseName() + QString("_"));

    // Read the Phase information from the .h5ang file
    H5EbsdVolumeReader::Pointer h5Reader = H5EbsdVolumeReader::New();
    h5Reader->setFileName(m_H5EbsdFile->text().toStdString());
    h5Reader->setSliceStart(m_ZStartIndex->value());

    int size = h5Reader->getNumPhases();

    std::vector<std::string> phaseTypeStrings;
    PhaseType::getPhaseTypeStrings(phaseTypeStrings);
    std::vector<unsigned int> phaseTypeEnums;
    PhaseType::getPhaseTypeEnums(phaseTypeEnums);

    // Remove all the items
    phaseTypeList->clear();
    // Now iterate over all the phases creating the proper UI elements
    for (int i = 0; i < size; i++)
    {

      phaseTypeList->addItem(PhaseType::PrimaryStr().c_str());
      QListWidgetItem* item = phaseTypeList->item(i);
      item->setSizeHint(QSize(50, 25));
      QComboBox* cb = new QComboBox(phaseTypeList);
      for (size_t i = 0; i < phaseTypeStrings.size(); ++i)
      {
        cb->addItem(QString::fromStdString(phaseTypeStrings[i]), phaseTypeEnums[i]);
        cb->setItemData(i, phaseTypeEnums[i], Qt::UserRole);
      }
      cb->setMinimumHeight(25);
      phaseTypeList->setItemWidget(item, cb);
      connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(phaseTypeEdited(int)));
    }

    int err = 0;
    int64_t xpoints = 0;
    int64_t ypoints = 0;
    int64_t zpoints = 1;
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
    if(err >= 0)
    {
      m_XDim->setText(QString::number(xpoints));
      m_YDim->setText(QString::number(ypoints));
      m_ZDim->setText(QString::number(zpoints));
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
    if(model != m_QualityMetricTableModel && model == NULL)
    {
      delete model; // Clean up this memory
    }

    // Compare the Manufactureres of the current file versus the one we have cached
    // If they are different then we need to remove all the quality filters
    QString fileManufact = QString::fromStdString(h5Reader->getManufacturer());
    if(m_EbsdManufacturer->text().compare(fileManufact) != 0)
    {
      m_QualityMetricTableModel->removeRows(0, m_QualityMetricTableModel->rowCount());
    }

    // Cache the Manufacturer from the File
    m_EbsdManufacturer->setText(fileManufact);

    m_StackingOrder->setText(QString::fromStdString(Ebsd::StackingOrder::Utils::getStringForEnum(h5Reader->getStackingOrder())));

    rotateslice = h5Reader->getRotateSlice();
    reorderarray = h5Reader->getReorderArray();
    aligneulers = h5Reader->getAlignEulers();

    if(rotateslice == true) m_RotateSliceLabel->setText("true");
    else if(rotateslice == false) m_RotateSliceLabel->setText("false");

    if(reorderarray == true) m_ReorderArrayLabel->setText("true");
    else if(reorderarray == false) m_ReorderArrayLabel->setText("false");

    if(aligneulers == true) m_AlignEulersLabel->setText("true");
    else if(aligneulers == false) m_AlignEulersLabel->setText("false");

    // Get the list of Possible filter Fields based on the Manufacturer
    if(m_EbsdManufacturer->text().compare(QString(Ebsd::Ang::Manufacturer.c_str())) == 0)
    {
      AngFields fields;
      m_QualityMetricTableModel->setPossibleFields(fields.getFilterFields<QStringList>());
    }
    else if(m_EbsdManufacturer->text().compare(QString(Ebsd::Ctf::Manufacturer.c_str())) == 0)
    {
      CtfFields fields;
      m_QualityMetricTableModel->setPossibleFields(fields.getFilterFields<QStringList>());
    }
    // Set the ItemDelegate for the table.
    QAbstractItemDelegate* aid = m_QualityMetricTableModel->getItemDelegate();
    m_QualityMetricTableView->setItemDelegate(aid);

    // Make sure at least 1 Quality Metric is available.
    if(m_QualityMetricTableModel->rowCount() < 1)
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
//  std::cout << "on_removeQualityMetric_clicked" << std::endl;
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

  QFileInfo fi(m_H5EbsdFile->text());
  if (fi.isFile() == false)
  {
    return;
  }

  reader->setFileName(m_H5EbsdFile->text().toStdString());
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
    if(m_FilterPipeline != NULL)
    {
      std::cout << "canceling from GUI...." << std::endl;
      emit cancelPipeline();
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
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the Reconstruction going
  connect(m_WorkerThread, SIGNAL(started()),
          m_FilterPipeline, SLOT(run()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_FilterPipeline, SIGNAL(finished() ),
          m_WorkerThread, SLOT(quit()) );

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()),
          this, SLOT( pipelineComplete() ) );

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelPipeline() ),
          m_FilterPipeline, SLOT (on_CancelWorker() ) , Qt::DirectConnection);

  // Send Progress from the Reconstruction to this object for display
  connect(m_FilterPipeline, SIGNAL (updateProgress(int)),
          this, SLOT(pipelineProgress(int) ) );

  // Send progress messages from Reconstruction to this object for display
  connect(m_FilterPipeline, SIGNAL (progressMessage(QString)),
          this, SLOT(addProgressMessage(QString) ));

  // Send progress messages from Reconstruction to this object for display
  connect(m_FilterPipeline, SIGNAL (warningMessage(QString)),
          this, SLOT(addWarningMessage(QString) ));

  // Send progress messages from Reconstruction to this object for display
  connect(m_FilterPipeline, SIGNAL (errorMessage(QString)),
          this, SLOT(addErrorMessage(QString) ));



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
  m_FilterPipeline->setDataContainer(m);

  setWidgetListEnabled(false);
  emit pipelineStarted();
  m_WorkerThread->start();
  m_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::setupPipeline()
{
  // Create a FilterPipeline Object to hold all the filters. Later on we will execute all the filters
  m_FilterPipeline = new QFilterPipeline(NULL);
  std::string outDir = QDir::toNativeSeparators(m_OutputDir->text()).toStdString();
  std::string prefix = m_OutputFilePrefix->text().toStdString();

  int count = phaseTypeList->count();

  DataArray<unsigned int>::Pointer phaseTypes = DataArray<unsigned int>::CreateArray(count + 1);
  phaseTypes->SetName(DREAM3D::EnsembleData::PhaseType);
  phaseTypes->SetValue(0, DREAM3D::PhaseType::UnknownPhaseType);

  bool ok = false;
  for (int i = 0; i < count; ++i)
  {
    QListWidgetItem* item = phaseTypeList->item(i);
    QComboBox* cb = qobject_cast<QComboBox*>(phaseTypeList->itemWidget(item));
    unsigned int enPtValue = static_cast<unsigned int>(cb->itemData(cb->currentIndex(), Qt::UserRole).toUInt(&ok));
    if(enPtValue >= DREAM3D::PhaseType::UnknownPhaseType)
    {
      QString msg("The Phase Type for phase ");
      //      msg.append(QString::number(i)).append(" is not set correctly. Please set the phase to Primary, Precipitate or Transformation.");
      msg.append(QString::number(i)).append(" is not set correctly. Please set the phase to Primary or Precipitate.");
      QMessageBox::critical(this, QString("Grain Generator"), msg, QMessageBox::Ok | QMessageBox::Default);
      return;
    }
    phaseTypes->SetValue(i + 1, enPtValue);
  }

  ok = checkPhaseTypes();



  typedef std::vector<AbstractFilter::Pointer>  FilterContainerType;
  // Create a Vector to hold all the filters. Later on we will execute all the filters
  FilterContainerType pipeline;

  LoadSlices::Pointer load_slices = LoadSlices::New();
  load_slices->setH5EbsdFile(QDir::toNativeSeparators(m_H5EbsdFile->text()).toStdString());
  load_slices->setZStartIndex(m_ZStartIndex->value());
  load_slices->setZEndIndex(m_ZEndIndex->value());
  load_slices->setPhaseTypes(phaseTypes);
  load_slices->setQualityMetricFilters(getQualityMetricFilters());
  load_slices->setRefFrameZDir(Ebsd::StackingOrder::Utils::getEnumForString(m_StackingOrder->text().toStdString()));
  load_slices->setMisorientationTolerance(m_MisOrientationTolerance->value());
  m_FilterPipeline->pushBack(load_slices);

  AlignSections::Pointer align_sections = AlignSections::New();
  align_sections->setMisorientationTolerance(m_MisOrientationTolerance->value());
  unsigned int alignmeth = static_cast<unsigned int>(m_AlignMeth->currentIndex());
  align_sections->setAlignmentMethod(alignmeth);
  m_FilterPipeline->pushBack(align_sections);

  SegmentGrains::Pointer segment_grains = SegmentGrains::New();
  segment_grains->setMisorientationTolerance(m_MisOrientationTolerance->value());
  m_FilterPipeline->pushBack(segment_grains);

  CleanupGrains::Pointer cleanup_grains = CleanupGrains::New();
  cleanup_grains->setMinAllowedGrainSize(m_MinAllowedGrainSize->value());
  cleanup_grains->setMisorientationTolerance(m_MisOrientationTolerance->value());
  m_FilterPipeline->pushBack(cleanup_grains);

  if(m_MergeTwins->isChecked() == true)
  {
    MergeTwins::Pointer merge_twins = MergeTwins::New();
    m_FilterPipeline->pushBack(merge_twins);
  }

  if(m_MergeColonies->isChecked() == true)
  {
    MergeColonies::Pointer merge_colonies = MergeColonies::New();
    m_FilterPipeline->pushBack(merge_colonies);
  }

  // Create a new HDF5 Volume file by overwriting any HDF5 file that may be in the way
  MAKE_OUTPUT_FILE_PATH(hdf5VolumeFile, DREAM3D::Reconstruction::H5VoxelFile, outDir, prefix)
  DataContainerWriter::Pointer writer = DataContainerWriter::New();
  writer->setOutputFile(hdf5VolumeFile);
  m_FilterPipeline->pushBack(writer);

  if(m_VisualizationVizFile->isChecked() == true)
  {
    MAKE_OUTPUT_FILE_PATH(reconVisFile, DREAM3D::Reconstruction::VisualizationVizFile, outDir, prefix);
    VtkRectilinearGridWriter::Pointer vtkWriter = VtkRectilinearGridWriter::New();
    vtkWriter->setOutputFile(reconVisFile);
    vtkWriter->setWriteGrainIds(true);
    vtkWriter->setWritePhaseIds(m_WritePhaseIdScalars);
    vtkWriter->setWriteGoodVoxels(m_WriteGoodVoxelsScalars);
    vtkWriter->setWriteIPFColors(m_WriteIPFColorScalars);
    vtkWriter->setWriteBinaryFile(m_WriteBinaryVTKFile);
    m_FilterPipeline->pushBack(vtkWriter);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<QualityMetricFilter::Pointer> ReconstructionWidget::getQualityMetricFilters()
{
  int filterCount = m_QualityMetricTableModel->rowCount();
  std::vector<QualityMetricFilter::Pointer> filters;
  QVector<QString> fieldNames;
  QVector<float> fieldValues;
  QVector<QString> fieldOperators;
  m_QualityMetricTableModel->getTableData(fieldNames, fieldValues, fieldOperators);

  for (int i = 0; i < filterCount; ++i)
  {
    QualityMetricFilter::Pointer filter = QualityMetricFilter::New();
    filter->setFieldName(fieldNames[i].toStdString());
    filter->setFieldValue(fieldValues[i]);
    filter->setFieldOperator(fieldOperators[i].toStdString());
    filters.push_back(filter);
  }
  return filters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::pipelineComplete()
{
 // std::cout << "ReconstructionWidget::threadFinished()" << std::endl;
  m_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->progressBar->setValue(0);
  emit pipelineEnded();
  checkIOFiles();
  m_FilterPipeline->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::pipelineProgress(int val)
{
  this->progressBar->setValue( val );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::on_m_VtkOptionsBtn_clicked()
{
  QVector<QString> options;
  options.push_back("Write Phase Ids Scalars");
  options.push_back("Write Good Voxels Scalars");
  options.push_back("Write IPF Color Scalars");
  options.push_back("Write Binary VTK File");
  QCheckboxDialog d(options, this);
  d.setWindowTitle(QString("VTK Output Options"));

  d.setValue("Write Phase Ids Scalars", m_WritePhaseIdScalars);
  d.setValue("Write Good Voxels Scalars", m_WriteGoodVoxelsScalars);
  d.setValue("Write IPF Color Scalars", m_WriteIPFColorScalars);
  d.setValue("Write Binary VTK File", m_WriteBinaryVTKFile);

  int ret = d.exec();
  if (ret == QDialog::Accepted)
  {
    m_WritePhaseIdScalars = d.getValue("Write Phase Ids Scalars");
    m_WriteGoodVoxelsScalars = d.getValue("Write Good Voxels Scalars");
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::addErrorMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::ReconstructionDisplayName).append(" Error");
  displayDialogBox(title, message, QMessageBox::Critical);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::addWarningMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::ReconstructionDisplayName).append(" Warning");
  displayDialogBox(title, message, QMessageBox::Warning);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionWidget::addProgressMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
  }
}

