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


#include "DREAM3DLib/DREAM3DLib.h"

class GenerateEnsembleStatisticsFilter : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(GenerateEnsembleStatisticsFilter)
    DREAM3D_STATIC_NEW_MACRO(GenerateEnsembleStatisticsFilter)
    DREAM3D_TYPE_MACRO_SUPER(GenerateEnsembleStatisticsFilter, AbstractFilter)
    virtual ~GenerateEnsembleStatisticsFilter(){}

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::StatisticsFilters; }
    virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::SpatialFilters; }
    virtual const std::string getHumanLabel() { return "Generate Ensemble Statistics"; }
    virtual void writeFilterParameters(AbstractFilterParametersWriter* writer){}
    virtual void execute() {}
    virtual void preflight() {}
  protected:
    GenerateEnsembleStatisticsFilter() : AbstractFilter(){}

  private:
    GenerateEnsembleStatisticsFilter(const GenerateEnsembleStatisticsFilter&); // Copy Constructor Not Implemented
    void operator=(const GenerateEnsembleStatisticsFilter&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QGenerateEnsembleStatisticsWidget::QGenerateEnsembleStatisticsWidget(QWidget* parent) :
  QFilterWidget(parent)
{
  this->setupUi(this);
  setupGui();

  GenerateEnsembleStatisticsFilter::Pointer filter = GenerateEnsembleStatisticsFilter::New();
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
  GenerateEnsembleStatisticsFilter::Pointer filter = GenerateEnsembleStatisticsFilter::New();
/* Copy all the settings from this instance into the new instance */

  /* Mike,
   *  work your way through all the instances of the DistributionTypeWidget getting the type of
   * distribution and the checkbox value.
   */

  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QGenerateEnsembleStatisticsWidget::writeOptions(QSettings &prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QGenerateEnsembleStatisticsWidget::readOptions(QSettings &prefs)
{

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
  m_SizeDistributionWidget = new DistributionTypeWidget( QString::fromAscii("Size Distribution"), distributionTypeGroupBox);
  distributionTypeLayout->addWidget(m_SizeDistributionWidget);
}

