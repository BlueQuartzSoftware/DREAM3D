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

#ifndef TEXTUREDIALOG_H_
#define TEXTUREDIALOG_H_

#include <vector>

#include <QtGui/QDialog>

#include "ui_TextureDialog.h"

#include "AIM/Common/TexturePreset.h"

/*
 *
 */
class TextureDialog : public QDialog , private Ui::TextureDialog
{
  Q_OBJECT;

  public:
    TextureDialog(QWidget *parent = 0);
    virtual ~TextureDialog();

  protected slots:
    void on_cubicCrystalStructure_toggled();
    void on_hexCrystalStructure_toggled();

    void on_presetListWidget_itemSelectionChanged ();
    void on_presetListWidget_itemDoubleClicked ( QListWidgetItem * item );


  protected:

    void setupGui();



  private:
    std::vector<TexturePreset::Pointer> m_Cubics;
    std::vector<TexturePreset::Pointer> m_Hexs;

    TextureDialog(const TextureDialog&); // Copy Constructor Not Implemented
    void operator=(const TextureDialog&); // Operator '=' Not Implemented
};

#endif /* TEXTUREDIALOG_H_ */
