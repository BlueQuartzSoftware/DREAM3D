/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2012 Joseph B. Kleingers (Student Research Assistant)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, Joseph B. Kleingers,
 * the US Air Force, BlueQuartz Software nor the names of its contributors may be
 * used to endorse or promote products derived from this software without specific
 * prior written permission.
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

#include "DREAM3DFileDragMessageBox.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DFileDragMessageBox::DREAM3DFileDragMessageBox(QWidget* parent, int filterCount)
{
  setupUi(this);

  this->filterCount = filterCount;

  extractPipelineRadioBtn->setChecked(true);

  if (filterCount == -1)
  {
      // This should never be the case - Throw error???
      return;
  }
  else if (filterCount <= 0)
  {
      appendPipelineBtn->setHidden(true);
      prependPipelineBtn->setHidden(true);
      replacePipelineBtn->setText("Add to Pipeline");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DREAM3DFileDragMessageBox::getFilePath()
{
  return filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DFileDragMessageBox::setFilePath(QString path)
{
  filePath = path;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DFileDragMessageBox::on_replacePipelineBtn_clicked()
{
    if (addFilterRadioBtn->isChecked())
    {
        emit fireAddDREAM3DReaderFilter(filePath, Replace);
    }
    else
    {
        emit fireExtractPipelineFromFile(filePath, Replace);
    }
  // Close the dialog box
  close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DFileDragMessageBox::on_appendPipelineBtn_clicked()
{
    if (addFilterRadioBtn->isChecked())
    {
        emit fireAddDREAM3DReaderFilter(filePath, Append);
    }
    else
    {
        emit fireExtractPipelineFromFile(filePath, Append);
    }

  // Close the dialog box
  close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DFileDragMessageBox::on_prependPipelineBtn_clicked()
{
    if (addFilterRadioBtn->isChecked())
    {
        emit fireAddDREAM3DReaderFilter(filePath, Prepend);
    }
    else
    {
        emit fireExtractPipelineFromFile(filePath, Prepend);
    }

  // Close the dialog box
  close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DFileDragMessageBox::on_cancelBtn_clicked()
{
  // Close the dialog box
  close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DFileDragMessageBox::on_addFilterRadioBtn_toggled()
{
    if (addFilterRadioBtn->isChecked())
    {
        prependPipelineBtn->setHidden(true);
    }
    else
    {
        if (filterCount > 0)
        {
            prependPipelineBtn->setHidden(false);
        }
    }
}
