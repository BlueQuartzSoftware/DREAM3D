/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "QInitializeSyntheticVolumeWidget.h"

#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtGui/QMessageBox>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QComboBox>

#include "H5Support/H5Utilities.h"
#include "H5Support/H5Lite.h"
#include "H5Support/HDF5ScopedFileSentinel.h"


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/ShapeType.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"
#include "DREAM3DLib/Common/StatsData.h"
#include "DREAM3DLib/Common/PrimaryStatsData.h"
#include "DREAM3DLib/IOFilters/VoxelDataContainerReader.h"


#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/DREAM3DQtMacros.h"
#include "QtSupport/DREAM3DHelpUrlGenerator.h"

#include "DREAM3DLib/SyntheticBuildingFiltersWidgets/moc_QInitializeSyntheticVolumeWidget.cxx"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QInitializeSyntheticVolumeWidget::QInitializeSyntheticVolumeWidget(QWidget* parent) :
QFilterWidget(parent)
{
  m_OpenDialogLastDirectory = QDir::homePath();
  setupUi(this);
  InitializeSyntheticVolume::Pointer filter = InitializeSyntheticVolume::New();
  setupGui();
  setTitle(QString::fromStdString(filter->getHumanLabel()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QInitializeSyntheticVolumeWidget::~QInitializeSyntheticVolumeWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QInitializeSyntheticVolumeWidget::getFilterGroup()
{
    return QString::fromStdString(DREAM3D::FilterGroups::SyntheticBuildingFilters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QInitializeSyntheticVolumeWidget::getFilter()
{
  // Update the filter with the latest values from the GUI
  InitializeSyntheticVolume::Pointer filter = InitializeSyntheticVolume::New();

  filter->setInputFile(m_InputFile->text().toStdString());
  filter->setXVoxels(m_XPoints->value());
  filter->setYVoxels(m_YPoints->value());
  filter->setZVoxels(m_ZPoints->value());
  filter->setXRes(m_XResolution->value());
  filter->setYRes(m_YResolution->value());
  filter->setZRes(m_ZResolution->value());

  int count = m_ShapeTypeCombos.count();
  DataArray<unsigned int>::Pointer shapeTypes =
                  DataArray<unsigned int>::CreateArray(count+1, DREAM3D::EnsembleData::ShapeTypes);
  shapeTypes->SetValue(0, DREAM3D::ShapeType::UnknownShapeType);
  bool ok = false;
  for (int i = 0; i < count; ++i)
  {
   QComboBox* cb = m_ShapeTypeCombos.at(i);
   unsigned int sType = static_cast<unsigned int>(cb->itemData(cb->currentIndex(), Qt::UserRole).toUInt(&ok));
   shapeTypes->SetValue(i+1, sType);
  }
  filter->setShapeTypes(shapeTypes);

  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget* QInitializeSyntheticVolumeWidget::createDeepCopy()
{

  QInitializeSyntheticVolumeWidget* w = new QInitializeSyntheticVolumeWidget;

  // Update the filter with the latest values from the GUI
  w->blockSignals(true);
  w->on_m_InputFile_textChanged(m_InputFile->text());
  w->on_m_XPoints_valueChanged(m_XPoints->value());
  w->on_m_YPoints_valueChanged(m_YPoints->value());
  w->on_m_ZPoints_valueChanged(m_ZPoints->value());
  w->on_m_XResolution_valueChanged(m_XResolution->value());
  w->on_m_YResolution_valueChanged(m_YResolution->value());
  w->on_m_ZResolution_valueChanged(m_ZResolution->value());

//  int count = m_ShapeTypeCombos.count();
//  DataArray<unsigned int>::Pointer shapeTypes =
//    DataArray<unsigned int>::CreateArray(count+1, DREAM3D::EnsembleData::ShapeTypes);
//  shapeTypes->SetValue(0, DREAM3D::ShapeType::UnknownShapeType);
//  bool ok = false;
//  for (int i = 0; i < count; ++i)
//  {
//    QComboBox* cb = m_ShapeTypeCombos.at(i);
//    unsigned int sType = static_cast<unsigned int>(cb->itemData(cb->currentIndex(), Qt::UserRole).toUInt(&ok));
//    shapeTypes->SetValue(i+1, sType);
//  }
//  w->setShapeTypes(shapeTypes);
  w->blockSignals(false);
  return w;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::setShapeTypes(DataArray<unsigned int>::Pointer array)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QInitializeSyntheticVolumeWidget::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
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
void QInitializeSyntheticVolumeWidget::setupGui()
{
  setCheckable(true);

  setIsSelected(false);

  QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
  m_InputFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_InputFile_textChanged(const QString &)));



}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_InputFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("DREAM3D Stats Files (*.dream3d *.h5stats);;HDF5 Files(*.h5 *.hdf5);;All Files(*.*)") );
  if ( true == file.isEmpty() ){ return; }
  QFileInfo fi (file);
  m_InputFile->blockSignals(true);
  QString p = QDir::toNativeSeparators(fi.absoluteFilePath());
  m_InputFile->setText(p);
  on_m_InputFile_textChanged(m_InputFile->text() );
  m_InputFile->blockSignals(false);

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_InputFile_textChanged(const QString &text)
{
  if (m_InputFile->text().isEmpty())
  {
    return;
  }
  if(verifyPathExists(m_InputFile->text(), m_InputFile))
  {
    QFileInfo fi(m_InputFile->text());
    if(fi.exists() && fi.isFile())
    {
      m_DataContainer = VoxelDataContainer::New();
      std::stringstream ss;
      hid_t fileId = H5Utilities::openFile(m_InputFile->text().toStdString(), true); // Open the file Read Only
      if(fileId < 0)
      {
        ss.str("");
        ss << ": Error opening input file '" << m_InputFile->text().toStdString() << "'";
        return;
      }
      HDF5ScopedFileSentinel sentinel(&fileId, true);

      VoxelDataContainerReader::Pointer reader = VoxelDataContainerReader::New();
      reader->setHdfFileId(fileId);
      reader->setVoxelDataContainer(m_DataContainer.get());
      reader->setReadCellData(false);
      reader->setReadFieldData(false);
      reader->setReadEnsembleData(true);
      reader->setReadAllArrays(true); // THIS IS VERY IMPORTANT to set otherwise nothing gets read.
      reader->execute();
      int err = reader->getErrorCondition();
      if(err < 0)
      {
        m_DataContainer = VoxelDataContainer::NullPointer();
        QMessageBox::critical(this, tr("DREAM.3D"), tr("The DREAM3D Data File could not be read."), QMessageBox::Ok, QMessageBox::Ok);
        return;
      }

      IDataArray::Pointer iPtr = m_DataContainer->getEnsembleData(DREAM3D::EnsembleData::PhaseTypes);
      if (NULL == iPtr.get())
      {
        m_DataContainer = VoxelDataContainer::NullPointer();
        QMessageBox::critical(this, tr("DREAM.3D"), tr("The Ensemble Array 'PhaseTypes' was not found in the File"), QMessageBox::Ok, QMessageBox::Ok);
        return;
      }
      // Get the Phases
      DataArray<uint32_t>* phases = DataArray<uint32_t>::SafePointerDownCast(iPtr.get());

      int size = static_cast<int>(phases->GetNumberOfTuples());
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

      // We skip the first Ensemble as it is always a dummy
      for (int i = 0; i < size-1; i++)
      {
        QLabel* shapeTypeLabel = new QLabel(m_ShapeTypeScrollContents);
        QString str("Phase ");
        str.append(QString::number(i + 1, 10));
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
          cb->setItemData(static_cast<int>(s), shapeTypeEnums[s], Qt::UserRole);
        }
        m_ShapeTypeCombos << cb;
        formLayout_2->setWidget(i, QFormLayout::FieldRole, cb);
      }

      // Estimate the number of grains
      estimateNumGrainsSetup();

    }
  }
  emit parametersChanged();
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_XPoints_valueChanged(int v)
{
  estimateNumGrainsSetup();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_YPoints_valueChanged(int v)
{
  estimateNumGrainsSetup();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_ZPoints_valueChanged(int v)
{
  estimateNumGrainsSetup();
  emit parametersChanged();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_ZResolution_valueChanged(double v)
{
  estimateNumGrainsSetup();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_YResolution_valueChanged(double v)
{
  estimateNumGrainsSetup();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_XResolution_valueChanged(double v)
{
  estimateNumGrainsSetup();
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QInitializeSyntheticVolumeWidget::estimate_numgrains(int xpoints, int ypoints, int zpoints, float xres, float yres, float zres)
{
//  int err = -1;

  float totalvol;
  int phase;
//  std::vector<int> phases;
//  std::vector<unsigned int> structures;
//  std::vector<unsigned int> phaseType;
//  std::vector<float> phasefraction;
//  std::vector<float> double_data;
//  std::vector<float> avgdiam;
//  std::vector<float> sddiam;
//  std::vector<float> grainDiamInfo;
//  std::vector<float> maxdiameter;
//  std::vector<float> mindiameter;

  totalvol = (xpoints * xres) * (ypoints * yres) * (zpoints * zres);
  if (totalvol == 0.0)
  {
    return -1;
  }
  if(m_DataContainer.get() == NULL)
  {
    // This will force a read of the DataContainer from the data file
    on_m_InputFile_textChanged(QString(""));
    if(m_DataContainer.get() == NULL)
    {
      return -1;
    }
  }

  IDataArray::Pointer iPtr = m_DataContainer->getEnsembleData(DREAM3D::EnsembleData::PhaseTypes);
  // Get the PhaseTypes - Remember there is a Dummy PhaseType in the first slot of the array
  DataArray<uint32_t>* phaseType = DataArray<uint32_t>::SafePointerDownCast(iPtr.get());

  iPtr = m_DataContainer->getEnsembleData(DREAM3D::EnsembleData::Statistics);
  StatsDataArray* statsDataArrayPtr = StatsDataArray::SafePointerDownCast(iPtr.get());
  if(NULL == statsDataArrayPtr)
  {
    return -1;
  }

  // Create a Reference Variable so we can use the [] syntax
  StatsDataArray& statsDataArray = *statsDataArrayPtr;

#if 1
  DREAM3D_RANDOMNG_NEW()

  std::vector<int> primaryphases;
  std::vector<double> primaryphasefractions;
  double totalprimaryfractions = 0.0;
  StatsData::Pointer statsData = StatsData::NullPointer();
  // find which phases are primary phases
  for (size_t i = 1; i < phaseType->GetNumberOfTuples(); ++i)
  {
    if(phaseType->GetValue(i) == DREAM3D::PhaseType::PrimaryPhase)
    {
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[i].get());
      primaryphases.push_back(i);
      primaryphasefractions.push_back(pp->getPhaseFraction());
      totalprimaryfractions = totalprimaryfractions + pp->getPhaseFraction();
    }
  }
  // scale the primary phase fractions to total to 1
  for (size_t i = 0; i < primaryphasefractions.size(); i++)
  {
    primaryphasefractions[i] = primaryphasefractions[i] / totalprimaryfractions;
  }
  // generate the grains
  int gid = 1;

  float currentvol = 0.0;
  float vol;
  float diam;
  int volgood = 0;
  for (size_t j = 0; j < primaryphases.size(); ++j)
  {
    float curphasetotalvol = totalvol * primaryphasefractions[j];
    while (currentvol < (curphasetotalvol))
    {
      volgood = 0;
      phase = primaryphases[j];
      PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[phase].get());
      while (volgood == 0)
      {
        volgood = 1;
        // u = rg.genrand_res53();
        if(pp->getGrainSize_DistType() == DREAM3D::DistributionType::LogNormal)
        {
          float avgdiam = pp->getGrainSizeDistribution().at(0)->GetValue(0);
          float sddiam = pp->getGrainSizeDistribution().at(1)->GetValue(0);
          diam = rg.genrand_norm(avgdiam, sddiam);
          diam = exp(diam);
          if(diam >= pp->getMaxGrainDiameter()) volgood = 0;
          if(diam < pp->getMinGrainDiameter()) volgood = 0;
          vol = (4.0f / 3.0f) * (M_PI) * ((diam * 0.5f) * (diam * 0.5f) * (diam * 0.5f));
        }
      }
      currentvol = currentvol + vol;
      gid++;
    }
  }
#endif
  return gid;

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::estimateNumGrainsSetup()
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
void QInitializeSyntheticVolumeWidget::readOptions(QSettings &prefs)
{
  QString val;
  bool ok;
  qint32 i;
  double d;
  READ_FILEPATH_SETTING(prefs, m_, InputFile, "");
  READ_SETTING(prefs, m_, XResolution, ok, d, 0.25 , Double);
  READ_SETTING(prefs, m_, YResolution, ok, d, 0.25 , Double);
  READ_SETTING(prefs, m_, ZResolution, ok, d, 0.25 , Double);

  m_XPoints->blockSignals(true);
  READ_SETTING(prefs, m_, XPoints, ok, i, 100 , Int);
  m_XPoints->blockSignals(false);
  m_YPoints->blockSignals(true);
  READ_SETTING(prefs, m_, YPoints, ok, i, 100 , Int);
  m_YPoints->blockSignals(false);
  READ_SETTING(prefs, m_, ZPoints, ok, i, 100 , Int);

  on_m_InputFile_textChanged(QString(""));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::writeOptions(QSettings &prefs)
{
  prefs.setValue("Filter_Name", "InitializeSyntheticVolume" );
  WRITE_STRING_SETTING(prefs, m_, InputFile)


  WRITE_SETTING(prefs, m_, XResolution )
  WRITE_SETTING(prefs, m_, YResolution )
  WRITE_SETTING(prefs, m_, ZResolution )
  WRITE_SETTING(prefs, m_, XPoints )
  WRITE_SETTING(prefs, m_, YPoints )
  WRITE_SETTING(prefs, m_, ZPoints )

}

// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::openHtmlHelpFile()
{
  QString lowerFilter = QString("InitializeSyntheticVolume").toLower();
  DREAM3DHelpUrlGenerator::generateAndOpenHTMLUrl(lowerFilter, this);
}

