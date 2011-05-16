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

#include "EditPhaseDialog.h"

#include <QtGui/QDoubleValidator>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EditPhaseDialog::EditPhaseDialog(QWidget* parent) :
QDialog(parent),
m_OtherPhaseFractions(0.0)
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
void EditPhaseDialog::setCrystalStructure(AIM::Reconstruction::CrystalStructure xtal)
{
  xtalCombo->setCurrentIndex(xtal);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIM::Reconstruction::CrystalStructure EditPhaseDialog::getCrystalStructure()
{
  int index = xtalCombo->currentIndex();
  return static_cast<AIM::Reconstruction::CrystalStructure>(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double EditPhaseDialog::getPhaseFraction()
{
  bool ok = false;
  double d = phaseFraction->text().toFloat(&ok);
  if (ok) return d;
  return -1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::setPhaseFraction(double d)
{
  phaseFraction->setText(QString::number(d));
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::setPhaseType(AIM::Reconstruction::PhaseType pt)
{
  phaseTypeCombo->setCurrentIndex(pt);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIM::Reconstruction::PhaseType EditPhaseDialog::getPhaseType()
{
  int index = phaseTypeCombo->currentIndex();
  return static_cast<AIM::Reconstruction::PhaseType>(index);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::setupGui()
{
  QDoubleValidator* phaseFractionValidator = new QDoubleValidator(phaseFraction);
  phaseFractionValidator = phaseFractionValidator;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::on_phaseFraction_textChanged(const QString &string)
{
  bool ok = false;
  double d = phaseFraction->text().toFloat(&ok);
  if (ok)
  {
     double total = d + m_OtherPhaseFractions;
     total = d / total;
     calcPhaseFraction->setText(QString::number(total));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::setOtherPhaseFractionTotal(double t)
{
  m_OtherPhaseFractions = t;
}
