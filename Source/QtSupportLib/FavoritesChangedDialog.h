/* ============================================================================
* Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#ifndef _FavoritesChangedDialog_H_
#define _FavoritesChangedDialog_H_

#include <QtWidgets/QDialog>

#include "QtSupportLib/QtSupportLib.h"

//-- UIC generated Header
#include <ui_FavoritesChangedDialog.h>


class QtSupportLib_EXPORT FavoritesChangedDialog : public QDialog, private Ui::FavoritesChangedDialog
{
  Q_OBJECT

public:
  FavoritesChangedDialog(QWidget* parent = 0);
  virtual ~FavoritesChangedDialog();

  protected slots:
    void on_selectBtn_clicked();
    void on_exportBtn_clicked();
    void on_outputDir_textChanged(const QString &text);

  signals:
    void exportBtnPressed(QString directory);

private:
  QString m_OpenDialogLastDirectory;

  FavoritesChangedDialog(const FavoritesChangedDialog&); // Copy Constructor Not Implemented
  void operator=(const FavoritesChangedDialog&); // Operator '=' Not Implemented
};

#endif /* FavoritesChangedDialog_H_ */

