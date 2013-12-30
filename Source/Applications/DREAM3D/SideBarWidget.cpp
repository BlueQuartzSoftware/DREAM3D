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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "SideBarWidget.h"


#include "QFilterLibraryDockWidget.h"
#include "QDream3DDocumentsDockWidget.h"
#include "QPrebuiltPipelinesDockWidget.h"
#include "FilterListWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SideBarWidget::SideBarWidget(QWidget* parent) :
  QWidget(parent),
  m_CurrentWidget(NULL)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SideBarWidget::~SideBarWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SideBarWidget::setupGui()
{

  m_FilterLibrary = new QFilterLibraryDockWidget(this);
  m_FilterLibrary->setVisible(false);
  m_FavoritePipelines = new QDream3DDocumentsDockWidget(this);
  m_FavoritePipelines->setVisible(false);
  m_PrebuiltPipelines = new QPrebuiltPipelinesDockWidget(this);
  m_PrebuiltPipelines->setVisible(false);
  m_FilterListWidget = new FilterListWidget(this);
  m_FilterListWidget->setVisible(false);


  // Stick in the Filter Library Widget
  m_FilterLibrary->setVisible(true);
  gridLayout->addWidget(m_FilterLibrary, 2, 0, 1, 2);
  m_CurrentWidget = m_FilterLibrary;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SideBarWidget::setComboboxSelectedIndex(int index)
{
  widgetSelection->setCurrentIndex(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SideBarWidget::on_widgetSelection_currentIndexChanged(int index)
{
  int idx = gridLayout->indexOf(m_CurrentWidget);
  QLayoutItem* item = gridLayout->takeAt(idx);
  QWidget* w = item->widget();
  if(w) { w->setVisible(false); }


  if(index == FilterLibraryIndex)
  {
    m_FilterLibrary->setVisible(true);
    gridLayout->addWidget(m_FilterLibrary, 2, 0, 1, 2);
    m_CurrentWidget = m_FilterLibrary;

  }
  else if (index == FavoritePipelines)
  {
    m_FavoritePipelines->setVisible(true);
    gridLayout->addWidget(m_FavoritePipelines, 2, 0, 1, 2);
    m_CurrentWidget = m_FavoritePipelines;
  }
  else if (index == PrebuiltPipelines)
  {
    m_PrebuiltPipelines->setVisible(true);
    gridLayout->addWidget(m_PrebuiltPipelines, 2, 0, 1, 2);
    m_CurrentWidget = m_PrebuiltPipelines;
  }
  else if (index == FilterList)
  {
    m_FilterListWidget->setVisible(true);
    gridLayout->addWidget(m_FilterListWidget, 2, 0, 1, 2);
    m_CurrentWidget = m_FilterListWidget;
  }
}
