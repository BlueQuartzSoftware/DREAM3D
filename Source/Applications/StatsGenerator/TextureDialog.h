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

#ifndef _TEXTUREDIALOG_H_
#define _TEXTUREDIALOG_H_

#include <vector>

#include <QtWidgets/QDialog>

#include "ui_TextureDialog.h"



#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/TexturePreset.h"

/**
 * @class TextureDialog TextureDialog.h StatsGenerator/TextureDialog.h
 * @brief  This class presents the User a GUI to adjust Texture values.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Apr 20, 2011
 * @version 1.0
 */
class TextureDialog : public QDialog , private Ui::TextureDialog
{
    Q_OBJECT

  public:
    TextureDialog(unsigned int xtal, QWidget* parent = 0);
    virtual ~TextureDialog();

    DREAM3D_INSTANCE_PROPERTY(unsigned int, CrystalStructure)

    void getODFEntry(float& e1, float& e2, float& e3, float& weight, float& sigma);

  protected slots:
    void on_presetListWidget_itemSelectionChanged ();
    void on_presetListWidget_itemDoubleClicked ( QListWidgetItem* item );


  protected:

    void setupGui();



  private:
    QVector<TexturePreset::Pointer> m_Presets;

    TextureDialog(const TextureDialog&); // Copy Constructor Not Implemented
    void operator=(const TextureDialog&); // Operator '=' Not Implemented
};

#endif /* TEXTUREDIALOG_H_ */

