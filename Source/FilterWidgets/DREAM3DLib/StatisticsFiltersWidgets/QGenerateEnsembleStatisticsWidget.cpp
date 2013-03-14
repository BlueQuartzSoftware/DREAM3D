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

#include "QtSupport/DistributionTypeWidget.h"

#include "DREAM3DLib/Common/PhaseType.h"

#include "DREAM3DLib/StatisticsFiltersWidgets/moc_QGenerateEnsembleStatisticsWidget.cxx"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QGenerateEnsembleStatisticsWidget::QGenerateEnsembleStatisticsWidget(QWidget* parent) :
  QFilterWidget(parent),
  m_PhaseTypeEdited(false)
{
  this->setupUi(this);

  GenerateEnsembleStatistics::Pointer filter = GenerateEnsembleStatistics::New();
  m_FilterGroup = QString::fromStdString(filter->getGroupName());


  //Get the defaults from the filter and assign to the internal variables

  setupGui();
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
AbstractFilter::Pointer QGenerateEnsembleStatisticsWidget::getFilter()
{
  GenerateEnsembleStatistics::Pointer filter = GenerateEnsembleStatistics::New();
/* Copy all the settings from this instance into the new instance */

  filter->setSizeDistribution(m_SizeDistributionWidget->isChecked());
  filter->setSizeDistributionFitType(m_SizeDistributionWidget->getFitType());
  filter->setAspectRatioDistribution(m_AspectRatioDistributionWidget->isChecked());
  filter->setAspectRatioDistributionFitType(m_AspectRatioDistributionWidget->getFitType());
  filter->setOmega3Distribution(m_Omega3DistributionWidget->isChecked());
  filter->setOmega3DistributionFitType(m_Omega3DistributionWidget->getFitType());
  filter->setNeighborhoodDistribution(m_NeighborhoodDistributionWidget->isChecked());
  filter->setNeighborhoodDistributionFitType(m_NeighborhoodDistributionWidget->getFitType());
  filter->setODF(m_ODFWidget->isChecked());
  filter->setMDF(m_MDFWidget->isChecked());
  filter->setAxisODF(m_AxisODFWidget->isChecked());

  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QGenerateEnsembleStatisticsWidget::writeOptions(QSettings &prefs)
{
  prefs.setValue("Filter_Name", "GenerateEnsembleStatistics" );
  // Need to figure out how to write out the values for the DistributionTypeWidgets

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QGenerateEnsembleStatisticsWidget::readOptions(QSettings &prefs)
{
  // Read the values back from the prefs and set them into the DistributionTypeWidgets
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget* QGenerateEnsembleStatisticsWidget::createDeepCopy()
{
  QGenerateEnsembleStatisticsWidget* w = new QGenerateEnsembleStatisticsWidget(NULL);
  /* Copy all the settings from this instance into the new instance */
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


  int numPhases = vdc->getNumEnsembleTuples();

  std::vector<std::string> phaseTypeStrings;
  PhaseType::getPhaseTypeStrings(phaseTypeStrings);
  std::vector<unsigned int> phaseTypeEnums;
  PhaseType::getPhaseTypeEnums(phaseTypeEnums);

  // Remove all the items
  phaseTypeList->clear();
  // Now iterate over all the phases creating the proper UI elements
  for (int i = 0; i < numPhases; i++)
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



  m_NeighborhoodDistributionWidget = new DistributionTypeWidget( QString::fromAscii("Neighborhood Distribution"), distributionTypeFrame);
  m_NeighborhoodDistributionWidget->setStyleSheet("");
  distributionTypeLayout->insertWidget(0, m_NeighborhoodDistributionWidget);
  m_Omega3DistributionWidget = new DistributionTypeWidget( QString::fromAscii("Omega3 Distribution"), distributionTypeFrame);
  m_Omega3DistributionWidget->setStyleSheet("");
  distributionTypeLayout->insertWidget(0, m_Omega3DistributionWidget);
  m_AspectRatioDistributionWidget = new DistributionTypeWidget( QString::fromAscii("Aspect Ratio Distribution"), distributionTypeFrame);
  m_AspectRatioDistributionWidget->setStyleSheet("");
  distributionTypeLayout->insertWidget(0, m_AspectRatioDistributionWidget);
  m_SizeDistributionWidget = new DistributionTypeWidget( QString::fromAscii("Size Distribution"), distributionTypeFrame);
  m_SizeDistributionWidget->setStyleSheet("");
  distributionTypeLayout->insertWidget(0, m_SizeDistributionWidget);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QGenerateEnsembleStatisticsWidget::phaseTypeEdited(int i)
{
  m_PhaseTypeEdited = true;
  emit parametersChanged();
}

