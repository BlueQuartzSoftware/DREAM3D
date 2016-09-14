/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "EMMPMWidget.h"

#include <assert.h>

#include <QtCore/QMetaProperty>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QDateTime>

#include <QtWidgets/QLabel>
#include <QtWidgets/QFileDialog>
#include <QtGui/QStandardItemModel>
#include <QtCore/QItemSelectionModel>
#include <QtWidgets/QListWidget>

#include "EMMPM/EMMPMFilters/EMMPMFilter.h"
#include "EMMPM/FilterParameters/EMMPMFilterParameter.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

// Include the MOC generated file for this class
#include "moc_EMMPMWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMWidget::EMMPMWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<EMMPMFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "nullptr Pointer", "EMMPMWidget can ONLY be used with a EMMPMFilterParameter object");
  m_Filter = dynamic_cast<EMMPMFilter*>(filter);
  Q_ASSERT_X(getFilter() != nullptr, "nullptr Pointer", "EMMPMWidget can ONLY be used with a EMMPMFilter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMWidget::EMMPMWidget(QWidget* parent) :
  FilterParameterWidget(nullptr, nullptr, parent),
  m_Filter(nullptr),
  m_FilterParameter(nullptr),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMWidget::~EMMPMWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMWidget::setFilter(AbstractFilter* value)
{
  m_Filter = dynamic_cast<EMMPMFilter*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter* EMMPMWidget::getFilter() const
{
  return m_Filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMWidget::setFilterParameter(FilterParameter* value)
{
  m_FilterParameter = dynamic_cast<EMMPMFilterParameter*>(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter* EMMPMWidget::getFilterParameter() const
{
  return m_FilterParameter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMWidget::setupGui()
{

  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

//  if (getFilterParameter() != nullptr)
//  {
//    label->setText(getFilterParameter()->getHumanLabel() );
//  }

//  if(getFilter() != nullptr)
//  {
//    QString path = m_Filter->getInputFile();

//    filePath->setText(path);
//    on_filePath_fileDropped(path);
//  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMWidget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMWidget::afterPreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  Q_UNUSED(filter)

//  m_Filter->setInputFile(filePath->text());
//  updateProxyFromModel(); // Will update m_DcaProxy with the latest selections from the Model
//  m_Filter->setInputFileDataContainerArrayProxy(m_DcaProxy);
}
