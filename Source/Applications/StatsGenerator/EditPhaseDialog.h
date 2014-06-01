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

#ifndef _EDITPHASEDIALOG_H_
#define _EDITPHASEDIALOG_H_

#include <QtGui/QDialog>
#include <QtGui/QDoubleValidator>
#include "ui_EditPhaseDialog.h"

#include "EbsdLib/EbsdConstants.h"

#include "DREAM3DLib/Common/Constants.h"

/**
 * @class EditPhaseDialog EditPhaseDialog.h StatsGenerator/EditPhaseDialog.h
 * @brief Simple dialog that is presented to the user when a new phase is added.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Apr 14, 2011
 * @version 1.0
 */
class EditPhaseDialog : public QDialog, private Ui::EditPhaseDialog
{
    Q_OBJECT

  public:
    EditPhaseDialog(QWidget* parent = 0);
    virtual ~EditPhaseDialog();

    unsigned int getCrystalStructure();
    void setCrystalStructure(unsigned int xtal);

    float getPhaseFraction();
    void setPhaseFraction(float d);

    void setPhaseType(unsigned int pt);
    unsigned int getPhaseType();

    void setOtherPhaseFractionTotal(float t);

    void setEditFlag(bool flag);

    float getPptFraction();
    void setPptFraction(float d);

    unsigned int getParentPhase();
    void setParentPhase(unsigned int d);

  protected slots:
    void on_phaseFraction_textChanged(const QString& string);
    void on_phaseTypeCombo_currentIndexChanged(int index);


  protected:
    void setupGui();

  private:
    float m_OtherPhaseFractions;
    bool m_EditFlag;
    QDoubleValidator* m_PhaseFractionValidator;
    QDoubleValidator* m_ParentPhaseValidator;
    QDoubleValidator* m_PptFractionValidator;
    EditPhaseDialog(const EditPhaseDialog&); // Copy Constructor Not Implemented
    void operator=(const EditPhaseDialog&); // Operator '=' Not Implemented
};

#endif /* EDITPHASEDIALOG_H_ */

