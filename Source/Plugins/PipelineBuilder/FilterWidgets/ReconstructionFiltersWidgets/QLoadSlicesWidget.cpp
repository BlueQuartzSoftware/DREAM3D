/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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

#include "QLoadSlicesWidget.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/H5EbsdVolumeInfo.h"
#include "EbsdLib/H5EbsdVolumeReader.h"
#include "EbsdLib/TSL/H5AngVolumeReader.h"
#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/HKL/H5CtfVolumeReader.h"
#include "EbsdLib/HKL/CtfFields.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/PhaseType.h"
#include "DREAM3DLib/Common/QualityMetricFilter.h"

#include <QtGui/QLabel>
#include <QtGui/QMessageBox>
#include <QtGui/QComboBox>

#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/DREAM3DQtMacros.h"

#include "PipelineBuilder/FilterWidgets/ReconstructionFiltersWidgets/QualityMetricTableModel.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QLoadSlicesWidget::QLoadSlicesWidget(QWidget* parent) :
    QFilterWidget(parent),
    m_QualityMetricTableModel(NULL),
    m_phaseTypeEdited(false),
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
  m_Filter = LoadSlices::New();
  setupGui();
  setTitle(QString::fromStdString(m_Filter->getHumanLabel()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QLoadSlicesWidget::~QLoadSlicesWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QLoadSlicesWidget::getFilter()
{
  int count = phaseTypeList->count();

  DataArray<unsigned int>::Pointer phaseTypes = DataArray<unsigned int>::CreateArray(count + 1, DREAM3D::EnsembleData::PhaseTypes);
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
      msg.append(QString::number(i)).append(" is not set correctly. Please set the phase to Primary or Precipitate.");
      QMessageBox::critical(this, QString("Grain Generator"), msg, QMessageBox::Ok | QMessageBox::Default);
      return AbstractFilter::NullPointer();
    }
    phaseTypes->SetValue(i + 1, enPtValue);
  }

  m_Filter->setPhaseTypes(phaseTypes);
  setupQualityMetricFilters();

  // Update the Filter with all of these values;
  m_Filter->setH5EbsdFile(m_H5EbsdFile->text().toStdString());
  m_Filter->setZStartIndex(m_ZStartIndex->value());
  m_Filter->setZEndIndex(m_ZEndIndex->value());

  m_Filter->setRefFrameZDir(Ebsd::StackingOrder::Utils::getEnumForString(m_StackingOrder->text().toStdString()));
  m_Filter->setMisorientationTolerance(m_MisorientationTolerance->value());
  return m_Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QLoadSlicesWidget::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
{
//  std::cout << "outFilePath: " << outFilePath.toStdString() << std::endl;
  QFileInfo fileinfo(outFilePath);
  if (false == fileinfo.exists() )
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
void QLoadSlicesWidget::setupGui()
{
  setCheckable(true);

  changeStyle(false);

  QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
  m_H5EbsdFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_H5EbsdFile_textChanged(const QString &)));

  m_QualityMetricTableModel = new QualityMetricTableModel;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QLoadSlicesWidget::on_m_H5EbsdBtn_clicked()
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
void QLoadSlicesWidget::on_m_H5EbsdFile_textChanged(const QString &text)
{

  if(verifyPathExists(m_H5EbsdFile->text(), m_H5EbsdFile))
  {

    QFileInfo fi(m_H5EbsdFile->text());
    if(fi.exists() && fi.isFile())
    {
      m_SetSliceInfo();

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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QLoadSlicesWidget::on_addQualityMetric_clicked()
{
  if (!m_QualityMetricTableModel->insertRow(m_QualityMetricTableModel->rowCount())) return;

  QModelIndex index = m_QualityMetricTableModel->index(m_QualityMetricTableModel->rowCount() - 1, 0);
  m_QualityMetricTableView->setCurrentIndex(index);
  m_QualityMetricTableView->resizeColumnsToContents();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QLoadSlicesWidget::on_removeQualityMetric_clicked()
{
  //std::cout << "on_removeQualityMetric_clicked" << std::endl;
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
void QLoadSlicesWidget::m_SetSliceInfo()
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
    m_ZStartIndex->setRange(reader->getZStart(), reader->getZEnd());
    m_ZStartIndex->setValue(reader->getZStart());
    m_ZEndIndex->setRange(reader->getZStart(), reader->getZEnd());
    m_ZEndIndex->setValue(reader->getZEnd());
    m_ZRes->setText(QString::number(z));
    m_ZMin->setText(QString::number(reader->getZStart()));
    m_ZMax->setText(QString::number(reader->getZEnd()));
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QLoadSlicesWidget::phaseTypeEdited(int i)
{
  m_phaseTypeEdited = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QLoadSlicesWidget::checkPhaseTypes()
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
void QLoadSlicesWidget::setupQualityMetricFilters()
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

  m_Filter->setQualityMetricFilters(filters);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QLoadSlicesWidget::readOptions(QSettings &prefs)
{
  QString val;
  bool ok;
  qint32 i;
  double d;

  READ_FILEPATH_SETTING(prefs, m_, H5EbsdFile, "");
  on_m_H5EbsdFile_textChanged(QString(""));
  READ_SETTING(prefs, m_, ZStartIndex, ok, i, 0, Int)
  READ_SETTING(prefs, m_, ZEndIndex, ok, i, 0, Int)
  READ_SETTING(prefs, m_, MisorientationTolerance, ok, d, 5.0 , Double);

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


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QLoadSlicesWidget::writeOptions(QSettings &prefs)
{
  prefs.setValue("Filter_Name", "LoadSlices" );
  prefs.setValue("H5EbsdFile", m_H5EbsdFile->text());
  prefs.setValue("ZStartIndex", m_ZStartIndex->value());
  prefs.setValue("ZEndIndex", m_ZEndIndex->value() );
  prefs.setValue("MisorientationTolerance", m_MisorientationTolerance->value() );


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


}


