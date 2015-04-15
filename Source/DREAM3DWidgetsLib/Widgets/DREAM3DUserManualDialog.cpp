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

#include "DREAM3DUserManualDialog.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <QtWidgets/QMessageBox>

#include "QtSupport/DREAM3DHelpUrlGenerator.h"

#include "DREAM3DLib/Common/FilterManager.h"

#include "DREAM3DWidgetsLib/Widgets/DREAM3DUserManualDialog.h"

DREAM3DUserManualDialog* DREAM3DUserManualDialog::self = NULL;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DUserManualDialog::DREAM3DUserManualDialog()
{
  Q_ASSERT_X(!self, "DREAM3DUserManualDialog", "There should be only one DREAM3DUserManualDialog object");

  setupUi(this);
  DREAM3DUserManualDialog::self = this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DUserManualDialog::~DREAM3DUserManualDialog() {}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DUserManualDialog* DREAM3DUserManualDialog::Instance()
{
  if (self == NULL)
  {
    self = new DREAM3DUserManualDialog();
  }
  return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUserManualDialog::LaunchHelpDialog(QUrl url)
{
  DREAM3DUserManualDialog* dialog = DREAM3DUserManualDialog::Instance();
  dialog->webView->load(url);
  dialog->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUserManualDialog::LaunchHelpDialog(QString humanLabel)
{
  FilterManager* fm = FilterManager::Instance();
  if (NULL == fm)
  {
    return;
  }
  IFilterFactory::Pointer factory = fm->getFactoryForFilterHumanName(humanLabel);
  if (NULL == factory.get())
  {
    return;
  }
  AbstractFilter::Pointer filter = factory->create();
  if (NULL == filter.get())
  {
    return;
  }
  QString className = filter->getNameOfClass();

  // Generate help page
  QUrl helpURL = DREAM3DHelpUrlGenerator::generateHTMLUrl(className.toLower());
  DREAM3DUserManualDialog::LaunchHelpDialog(helpURL);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUserManualDialog::on_backBtn_pressed()
{
  webView->back();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUserManualDialog::on_forwardBtn_pressed()
{
  webView->forward();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DUserManualDialog::on_refreshBtn_pressed()
{
  webView->reload();
}







