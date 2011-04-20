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

#include "TextureDialog.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TextureDialog::TextureDialog(QWidget *parent) :
QDialog(parent)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TextureDialog::~TextureDialog()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TextureDialog::setupGui()
{
  m_Cubics = CubicTexturePresets::getTextures();
  m_Hexs = HexTexturePresets::getTextures();
  // Clear the list view;
  presetListWidget->clear();

  // Now push the Cubic list into the List View
  for (TexturePreset::Container::iterator iter = m_Cubics.begin(); iter != m_Cubics.end(); ++iter )
  {
    presetListWidget->addItem( QString::fromStdString((*iter)->getName() ) );
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TextureDialog::on_hexCrystalStructure_toggled()
{
  if (hexCrystalStructure->isChecked())
  {
    // Clear the list view;
    presetListWidget->clear();

    // Now push the Cubic list into the List View
    for (TexturePreset::Container::iterator iter = m_Hexs.begin(); iter != m_Hexs.end(); ++iter)
    {
      presetListWidget->addItem(QString::fromStdString((*iter)->getName()));
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TextureDialog::on_cubicCrystalStructure_toggled()
{
  if (cubicCrystalStructure->isChecked())
  {

    // Clear the list view;
    presetListWidget->clear();

    // Now push the Cubic list into the List View
    for (TexturePreset::Container::iterator iter = m_Cubics.begin(); iter != m_Cubics.end(); ++iter)
    {
      presetListWidget->addItem(QString::fromStdString((*iter)->getName()));
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TextureDialog::on_presetListWidget_itemDoubleClicked ( QListWidgetItem * item )
{
  int index = presetListWidget->currentRow();
  TexturePreset::Pointer t;
  if (cubicCrystalStructure->isChecked())
  {
    t = m_Cubics[index];
  }
  else if (hexCrystalStructure->isChecked())
  {
    t = m_Hexs[index];
  }
  euler1Preset->setText(QString::number(t->getEuler1()));
  euler2Preset->setText(QString::number(t->getEuler2()));
  euler3Preset->setText(QString::number(t->getEuler3()));

  euler1->setText(QString::number(t->getEuler1()));
  euler2->setText(QString::number(t->getEuler2()));
  euler3->setText(QString::number(t->getEuler3()));

  weight->setText("1.0");
  sigma->setText("1.0");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TextureDialog::on_presetListWidget_itemSelectionChanged()
{
  int index = presetListWidget->currentRow();
  TexturePreset::Pointer t;
  if (cubicCrystalStructure->isChecked())
  {
    t = m_Cubics[index];
  }
  else if (hexCrystalStructure->isChecked())
  {
    t = m_Hexs[index];
  }
  euler1Preset->setText(QString::number(t->getEuler1()));
  euler2Preset->setText(QString::number(t->getEuler2()));
  euler3Preset->setText(QString::number(t->getEuler3()));
}


