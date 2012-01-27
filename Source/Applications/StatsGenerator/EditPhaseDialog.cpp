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

#include "EditPhaseDialog.h"

#include "DREAM3DLib/Common/PhaseType.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EditPhaseDialog::EditPhaseDialog(QWidget* parent) :
QDialog(parent),
m_OtherPhaseFractions(0.0),
m_PhaseFractionValidator(NULL),
m_PptFractionValidator(NULL)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EditPhaseDialog::~EditPhaseDialog()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::setCrystalStructure(unsigned int xtal)
{
  xtalCombo->setCurrentIndex(xtal);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int EditPhaseDialog::getCrystalStructure()
{
  int index = xtalCombo->currentIndex();
  return static_cast<unsigned int>(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float EditPhaseDialog::getPhaseFraction()
{
  bool ok = false;
  float d = phaseFraction->text().toFloat(&ok);
  if (ok) return d;
  return -1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::setPhaseFraction(float d)
{
  phaseFraction->setText(QString::number(d));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float EditPhaseDialog::getPptFraction()
{
  bool ok = false;
  float d = pptFraction->text().toFloat(&ok);
  if (ok) return d;
  return -1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::setPptFraction(float d)
{
  pptFraction->setText(QString::number(d));
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::setPhaseType(unsigned int pt)
{
  phaseTypeCombo->setCurrentIndex(pt);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int EditPhaseDialog::getPhaseType()
{
  int index = phaseTypeCombo->currentIndex();
  return static_cast<unsigned int>(index);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::setupGui()
{
  m_PhaseFractionValidator = new QDoubleValidator(phaseFraction);
  m_PptFractionValidator = new QDoubleValidator(pptFraction);
  m_PptFractionValidator->setBottom(0.0);
  m_PptFractionValidator->setTop(1.0);
  m_PptFractionValidator->setDecimals(6);
  pptFraction->setEnabled(false);
  pptFractionLabel->setEnabled(false);
  on_phaseFraction_textChanged(QString());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::on_phaseFraction_textChanged(const QString &string)
{
  bool ok = false;
  float d = phaseFraction->text().toFloat(&ok);
  if (ok)
  {
     float total = d + m_OtherPhaseFractions;
     total = d / total;
     calcPhaseFraction->setText(QString::number(total));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::setOtherPhaseFractionTotal(float t)
{
  m_OtherPhaseFractions = t;
  on_phaseFraction_textChanged(QString());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::on_phaseTypeCombo_currentIndexChanged(int index)
{
  bool b = false;
  if (phaseTypeCombo->currentIndex() == DREAM3D::PhaseType::PrecipitatePhase)
  {
    b = true;
  }
  pptFraction->setEnabled(b);
  pptFractionLabel->setEnabled(b);
}


