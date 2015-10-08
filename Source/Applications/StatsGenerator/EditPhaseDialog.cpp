/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "EditPhaseDialog.h"

#include "SIMPLib/Common/PhaseType.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_EditPhaseDialog.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EditPhaseDialog::EditPhaseDialog(QWidget* parent) :
  QDialog(parent),
  m_OtherPhaseFractions(0.0),
  m_PhaseFractionValidator(NULL),
  m_ParentPhaseValidator(NULL),
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
  if (ok) { return d; }
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
  if (ok) { return d; }
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
unsigned int EditPhaseDialog::getParentPhase()
{
  bool ok = false;
  unsigned int d = ParentPhase->text().toInt(&ok);
  if (ok) { return d; }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::setParentPhase(unsigned int d)
{
  ParentPhase->setText(QString::number(d));
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

  if (index == 2)
  {
    return DREAM3D::PhaseType::MatrixPhase;
  }
  return static_cast<unsigned int>(index);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::setupGui()
{
  m_PhaseFractionValidator = new QDoubleValidator(phaseFraction);
  m_ParentPhaseValidator = new QDoubleValidator(ParentPhase);
  m_PptFractionValidator = new QDoubleValidator(pptFraction);
  m_PptFractionValidator->setBottom(0.0);
  m_PptFractionValidator->setTop(1.0);
  m_PptFractionValidator->setDecimals(6);
  if(m_EditFlag == true) { phaseTypeCombo->setEnabled(false); }
  if(m_EditFlag == false) { phaseTypeCombo->setEnabled(true); }
  if(m_EditFlag == true) { phaseTypeComboLabel->setEnabled(false); }
  if(m_EditFlag == false) { phaseTypeComboLabel->setEnabled(true); }
  pptFraction->setEnabled(false);
  ParentPhase->setEnabled(false);
  pptFractionLabel->setEnabled(false);
  ParentPhaseLabel->setEnabled(false);
  on_phaseFraction_textChanged(QString());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::on_phaseFraction_textChanged(const QString& string)
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
void EditPhaseDialog::setEditFlag(bool flag)
{
  m_EditFlag = flag;
  phaseTypeCombo->setEnabled(flag);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EditPhaseDialog::on_phaseTypeCombo_currentIndexChanged(int index)
{
  bool b = false;
  if (static_cast<unsigned int>(phaseTypeCombo->currentIndex()) == DREAM3D::PhaseType::PrecipitatePhase)
  {
    b = true;
  }
  pptFraction->setEnabled(b);
  pptFractionLabel->setEnabled(b);

  b = false;
  if (static_cast<unsigned int>(phaseTypeCombo->currentIndex()) == DREAM3D::PhaseType::TransformationPhase)
  {
    b = true;
  }
  ParentPhase->setEnabled(b);
  ParentPhaseLabel->setEnabled(b);
}


