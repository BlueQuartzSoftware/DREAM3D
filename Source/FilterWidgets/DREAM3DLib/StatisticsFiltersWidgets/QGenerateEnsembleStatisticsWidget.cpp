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

#include "QGenerateEnsembleStatisticsWidget.h"

#include <QtCore/QDir>

#include "QtSupport/DREAM3DQtMacros.h"
#include "QtSupport/DistributionTypeWidget.h"
#include "QtSupport/DREAM3DHelpUrlGenerator.h"

#include "DREAM3DLib/Common/PhaseType.h"

#include "DREAM3DLib/StatisticsFiltersWidgets/moc_QGenerateEnsembleStatisticsWidget.cxx"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QGenerateEnsembleStatisticsWidget::QGenerateEnsembleStatisticsWidget(QWidget* parent) :
  QFilterWidget(parent)
{
  this->setupUi(this);

  GenerateEnsembleStatistics::Pointer filter = GenerateEnsembleStatistics::New();
  m_FilterGroup = QString::fromStdString(filter->getGroupName());
  setupGui();
  getGuiParametersFromFilter( filter.get() );
  setTitle(QString::fromStdString(filter->getHumanLabel()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QGenerateEnsembleStatisticsWidget::~QGenerateEnsembleStatisticsWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QGenerateEnsembleStatisticsWidget::getGuiParametersFromFilter(AbstractFilter* filt)
{
  GenerateEnsembleStatistics* filter = GenerateEnsembleStatistics::SafeObjectDownCast<AbstractFilter*, GenerateEnsembleStatistics*>(filt);

  calcSizeDistribution->setChecked(filter->getSizeDistribution());
  m_SizeDistributionFitType->setCurrentIndex(filter->getSizeDistributionFitType());
  calcAspectRatioDistribution->setChecked(filter->getAspectRatioDistribution());
  m_AspectRatioDistributionFitType->setCurrentIndex(filter->getAspectRatioDistributionFitType());
  calcOmega3Distribution->setChecked(filter->getOmega3Distribution());
  m_Omega3DistributionFitType->setCurrentIndex(filter->getOmega3DistributionFitType());
  calcNeighborhoodDistribution->setChecked(filter->getNeighborhoodDistribution());
  m_NeighborhoodDistributionFitType->setCurrentIndex(filter->getNeighborhoodDistributionFitType());
  calcODF->setChecked(filter->getCalculateODF());
  calcMDF->setChecked(filter->getCalculateMDF());
  calcAODF->setChecked(filter->getCalculateAxisODF());
  m_SizeCorrRes->setValue( filter->getSizeCorrelationResolution() );

  std::vector<unsigned int> phaseTypes = filter->getPhaseTypeArray();
  if (phaseTypes.empty() == true)
  {
    m_DefinePhaseTypes->setChecked(false);
  }
  else
  {
    int count = phaseTypes.size();
    for (int i=0; i < count; ++i)
    {
      QComboBox* cb = new QComboBox(this);
      cb->setCurrentIndex(phaseTypes[i]);
      phaseTypeTableWidget->setCellWidget(i, 0, cb);
    }
    m_DefinePhaseTypes->setChecked(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QGenerateEnsembleStatisticsWidget::getFilter(bool defaultValues)
{
  GenerateEnsembleStatistics::Pointer filter = GenerateEnsembleStatistics::New();
  if (defaultValues == true) { return filter; }
/* Copy all the settings from this instance into the new instance */

  filter->setSizeDistribution(calcSizeDistribution->isChecked());
  filter->setSizeDistributionFitType(m_SizeDistributionFitType->currentIndex());
  filter->setAspectRatioDistribution(calcAspectRatioDistribution->isChecked());
  filter->setAspectRatioDistributionFitType(m_AspectRatioDistributionFitType->currentIndex());
  filter->setOmega3Distribution(calcOmega3Distribution->isChecked());
  filter->setOmega3DistributionFitType(m_Omega3DistributionFitType->currentIndex());
  filter->setNeighborhoodDistribution(calcNeighborhoodDistribution->isChecked());
  filter->setNeighborhoodDistributionFitType(m_NeighborhoodDistributionFitType->currentIndex());
  filter->setCalculateODF(calcODF->isChecked());
  filter->setCalculateMDF(calcMDF->isChecked());
  filter->setCalculateAxisODF(calcAODF->isChecked());
  filter->setSizeCorrelationResolution(m_SizeCorrRes->value());

  // Add the PhaseTypes Array into the VoxelDataContainer if the user has set them up.
  if (m_DefinePhaseTypes->isChecked() == true)
  {

    int count = phaseTypeTableWidget->rowCount();
    std::vector<unsigned int> phaseTypes;
  phaseTypes.push_back(DREAM3D::PhaseType::UnknownPhaseType);
    for(int r = 0; r < count; ++r)
    {
      QWidget* w = phaseTypeTableWidget->cellWidget(r, 0);
      QComboBox* cb = qobject_cast<QComboBox*>(w);
      if(NULL != cb)
      {
        phaseTypes.push_back(cb->currentIndex());
      }
    }
    filter->setPhaseTypeArray(phaseTypes);
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QGenerateEnsembleStatisticsWidget::writeOptions(QSettings &prefs)
{
  prefs.setValue("Filter_Name", "GenerateEnsembleStatistics" );
  // Need to figure out how to write out the values for the DistributionTypeWidgets
  WRITE_CHECKBOX_SETTING(prefs, calc, SizeDistribution)
  WRITE_COMBO_BOX(prefs, m_, SizeDistributionFitType)

  WRITE_CHECKBOX_SETTING(prefs, calc, AspectRatioDistribution)
  WRITE_COMBO_BOX(prefs, m_, AspectRatioDistributionFitType)

  WRITE_CHECKBOX_SETTING(prefs, calc, Omega3Distribution)
  WRITE_COMBO_BOX(prefs, m_, Omega3DistributionFitType)

  WRITE_CHECKBOX_SETTING(prefs, calc, NeighborhoodDistribution)
  WRITE_COMBO_BOX(prefs, m_, NeighborhoodDistributionFitType)

  WRITE_CHECKBOX_SETTING(prefs, calc, ODF)
  WRITE_CHECKBOX_SETTING(prefs, calc, MDF)
  WRITE_CHECKBOX_SETTING(prefs, calc, AODF)

  WRITE_SETTING(prefs, m_, SizeCorrRes)

  WRITE_CHECKBOX_SETTING(prefs, m_, DefinePhaseTypes)

  if (m_DefinePhaseTypes->isChecked() == true)
  {
    prefs.beginWriteArray("PhaseTypes");
    int count = phaseTypeTableWidget->rowCount();
    for(int r = 0; r < count; ++r)
    {
      QWidget* w = phaseTypeTableWidget->cellWidget(r, 0);
      QComboBox* cb = qobject_cast<QComboBox*>(w);
      if(NULL != cb)
      {
        prefs.setArrayIndex(r);
        prefs.setValue("PhaseType", cb->currentIndex());
      }
    }
    prefs.endArray();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QGenerateEnsembleStatisticsWidget::readOptions(QSettings &prefs)
{
  bool ok;
  qint32 i;

  // Read the values back from the prefs and set them into the DistributionTypeWidgets
  READ_CHECKBOX_SETTING(prefs, calc, SizeDistribution, false)
  READ_COMBO_BOX(prefs, m_, SizeDistributionFitType)

  READ_CHECKBOX_SETTING(prefs, calc, AspectRatioDistribution, false)
  READ_COMBO_BOX(prefs, m_, AspectRatioDistributionFitType)

  READ_CHECKBOX_SETTING(prefs, calc, Omega3Distribution, false)
  READ_COMBO_BOX(prefs, m_, Omega3DistributionFitType)

  READ_CHECKBOX_SETTING(prefs, calc, NeighborhoodDistribution, false)
  READ_COMBO_BOX(prefs, m_, NeighborhoodDistributionFitType)

  READ_CHECKBOX_SETTING(prefs, calc, ODF, false)
  READ_CHECKBOX_SETTING(prefs, calc, MDF, false)
  READ_CHECKBOX_SETTING(prefs, calc, AODF, false)

  READ_SETTING(prefs, m_, SizeCorrRes, ok, i, 1 , Int);

  READ_CHECKBOX_SETTING(prefs, m_, DefinePhaseTypes, false)

  ok = false;
  int size = prefs.beginReadArray("PhaseTypes");
  for (int r = 0; r < size; ++r) {
    prefs.setArrayIndex(r);
    on_addPhaseType_clicked();
    unsigned int value = prefs.value("PhaseType").toUInt(&ok);
    QWidget* w = phaseTypeTableWidget->cellWidget(r, 0);
    QComboBox* cb = qobject_cast<QComboBox*>(w);
    if(NULL != cb)
    {
      cb->setCurrentIndex(value);
    }
  }
  prefs.endArray();


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget* QGenerateEnsembleStatisticsWidget::createDeepCopy()
{
  QGenerateEnsembleStatisticsWidget* w = new QGenerateEnsembleStatisticsWidget(NULL);
  /* Copy all the settings from this instance into the new instance */
  w->calcSizeDistribution->setChecked(calcSizeDistribution->isChecked());
  w->m_SizeDistributionFitType->setCurrentIndex(m_SizeDistributionFitType->currentIndex());
  w->calcAspectRatioDistribution->setChecked(calcAspectRatioDistribution->isChecked());
  w->m_AspectRatioDistributionFitType->setCurrentIndex(m_AspectRatioDistributionFitType->currentIndex());
  w->calcOmega3Distribution->setChecked(calcOmega3Distribution->isChecked());
  w->m_Omega3DistributionFitType->setCurrentIndex(m_Omega3DistributionFitType->currentIndex());
  w->calcNeighborhoodDistribution->setChecked(calcNeighborhoodDistribution->isChecked());
  w->m_NeighborhoodDistributionFitType->setCurrentIndex(m_NeighborhoodDistributionFitType->currentIndex());
  w->calcODF->setChecked(calcODF->isChecked());
  w->calcMDF->setChecked(calcMDF->isChecked());
  w->calcAODF->setChecked(calcAODF->isChecked());
  w->m_SizeCorrRes->setValue(m_SizeCorrRes->value());
  w->m_DefinePhaseTypes->setChecked(m_DefinePhaseTypes->isChecked());


  return w;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QGenerateEnsembleStatisticsWidget::getFilterGroup()
{
  return m_FilterGroup;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QGenerateEnsembleStatisticsWidget::preflightAboutToExecute(VoxelDataContainer::Pointer vdc,
                                                                SurfaceMeshDataContainer::Pointer smdc,
                                                                SolidMeshDataContainer::Pointer sdc)
{
  typedef DataArray<unsigned int> PhaseTypeArrayType;
  // Add the PhaseTypes Array into the VoxelDataContainer if the user has set them up.
  if (m_DefinePhaseTypes->isChecked() == true)
  {
    int count = phaseTypeTableWidget->rowCount();
    if (count > 1) { count = 1; } // Since this is preflight we only need to size the array to 1 otherwise errors from the preflight engine
    PhaseTypeArrayType::Pointer phaseTypes = PhaseTypeArrayType::CreateArray(count, DREAM3D::EnsembleData::PhaseTypes);
    for(int r = 0; r < count; ++r)
    {
      QWidget* w = phaseTypeTableWidget->cellWidget(r, 0);
      QComboBox* cb = qobject_cast<QComboBox*>(w);
      if(NULL != cb)
      {
        phaseTypes->SetValue(r, cb->currentIndex());
      }
    }

    vdc->addEnsembleData(phaseTypes->GetName(), phaseTypes);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QGenerateEnsembleStatisticsWidget::preflightDoneExecuting(VoxelDataContainer::Pointer vdc,
                                                               SurfaceMeshDataContainer::Pointer smdc,
                                                               SolidMeshDataContainer::Pointer sdc)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QGenerateEnsembleStatisticsWidget::setupGui()
{
  setCheckable(true);
  setIsSelected(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QGenerateEnsembleStatisticsWidget::inputChanged()
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QGenerateEnsembleStatisticsWidget::on_addPhaseType_clicked()
{
  std::vector<std::string> phaseTypeStrings;
  PhaseType::getPhaseTypeStrings(phaseTypeStrings);
  std::vector<unsigned int> phaseTypeEnums;
  PhaseType::getPhaseTypeEnums(phaseTypeEnums);

  phaseTypeTableWidget->insertRow(phaseTypeTableWidget->rowCount());

  QComboBox* cb = new QComboBox(phaseTypeTableWidget);
  for (size_t i = 0; i < phaseTypeStrings.size() - 1; ++i)
  {
    cb->addItem(QString::fromStdString(phaseTypeStrings[i]), phaseTypeEnums[i]);
    cb->setItemData(i, phaseTypeEnums[i], Qt::UserRole);
  }
  cb->setMinimumHeight(25);
  phaseTypeTableWidget->setCellWidget(phaseTypeTableWidget->rowCount()-1, 0, cb);
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QGenerateEnsembleStatisticsWidget::on_removePhaseType_clicked()
{
  QItemSelectionModel *selectionModel = phaseTypeTableWidget->selectionModel();
  if (!selectionModel->hasSelection()) return;
  QModelIndex index = selectionModel->currentIndex();
  if (!index.isValid()) return;
  phaseTypeTableWidget->model()->removeRow(index.row(), index.parent());
  emit parametersChanged();
}



// -----------------------------------------------------------------------------
void QGenerateEnsembleStatisticsWidget::openHtmlHelpFile()
{
  QString lowerFilter = QString("GenerateEnsembleStatistics").toLower();
  DREAM3DHelpUrlGenerator::generateAndOpenHTMLUrl(lowerFilter, this);
}

