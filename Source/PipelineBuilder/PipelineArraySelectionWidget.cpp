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

#include "PipelineArraySelectionWidget.h"

#include "DREAM3DLib/Common/AbstractFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineArraySelectionWidget::PipelineArraySelectionWidget(QWidget *parent) :
QWidget(parent)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineArraySelectionWidget::~PipelineArraySelectionWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineArraySelectionWidget::setupGui()
{

}


void PipelineArraySelectionWidget::setPossibleCellArrayNames(QStringList names)
{
  m_PossibleCellArrayNames = names;
}

void PipelineArraySelectionWidget::setPossibleFieldArrayNames(QStringList names)
{
  m_PossibleFieldArrayNames = names;
}

void PipelineArraySelectionWidget::setPossibleEnsembleArrayNames(QStringList names)
{
  m_PossibleEnsembleArrayNames = names;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineArraySelectionWidget::updatePipelineArrayNames(AbstractFilter* filter)
{
  StringSet_t cellNameSet = filter->getCreatedCellData();
  StringSet_t fieldNameSet = filter->getCreatedFieldData();
  StringSet_t ensembleNameSet = filter->getCreatedEnsembleData();

  StringSet_t cellReqNameSet = filter->getRequiredCellData();
  StringSet_t fieldReqNameSet = filter->getRequiredFieldData();
  StringSet_t ensembleReqNameSet = filter->getRequiredEnsembleData();

  createdArraysTreeWidget->clear();
  updateCreatedTree(cellNameSet, "Cell Data");
  updateCreatedTree(fieldNameSet, "Field Data");
  updateCreatedTree(ensembleNameSet, "Ensemble Data");

  requiredArraysTreeWidget->clear();
  updateRequiredTree(cellReqNameSet, "Cell Data");
  updateRequiredTree(fieldReqNameSet, "Field Data");
  updateRequiredTree(ensembleReqNameSet, "Ensemble Data");


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineArraySelectionWidget::updateCreatedTree(StringSet_t &names, QString groupName)
{
  if (names.size() > 0)
  {
    QTreeWidgetItem* item = new QTreeWidgetItem(createdArraysTreeWidget);
    item->setText(0, groupName);
    for(StringSet_t::iterator iter = names.begin(); iter != names.end(); ++iter)
    {
       QTreeWidgetItem* createdArrayName = new QTreeWidgetItem(item);
       createdArrayName->setText(0, QString::fromStdString(*iter));
    }
    item->setExpanded(true);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineArraySelectionWidget::updateRequiredTree(StringSet_t &names, QString groupName)
{
  if (names.size() > 0)
  {
    QTreeWidgetItem* item = new QTreeWidgetItem(requiredArraysTreeWidget);
    item->setText(0, groupName);
    for(StringSet_t::iterator iter = names.begin(); iter != names.end(); ++iter)
    {
       QTreeWidgetItem* createdArrayName = new QTreeWidgetItem(item);
       createdArrayName->setText(0, QString::fromStdString(*iter));
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineArraySelectionWidget::setCreatedCellArrayNames(QStringList names)
{

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineArraySelectionWidget::setCreatedFieldArrayNames(QStringList names)
{

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineArraySelectionWidget::setCreatedEnsembleArrayNames(QStringList names)
{

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineArraySelectionWidget::setRequiredCellArrayNames(QStringList names)
{

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineArraySelectionWidget::setRequiredFieldArrayNames(QStringList names)
{

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineArraySelectionWidget::setRequiredEnsembleArrayNames(QStringList names)
{

}


