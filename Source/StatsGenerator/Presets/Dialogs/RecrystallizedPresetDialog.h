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

#ifndef RECRYSTALLIZEDPRESETDIALOG_H_
#define RECRYSTALLIZEDPRESETDIALOG_H_

#include <QtGui/QDialog>

class QLineEdit;

/**
 * @class RecrystallizedPresetDialog RecrystallizedPresetDialog.h StatsGenerator/Presets/Dialogs/RecrystallizedPresetDialog.h
 * @brief This class is a QDialog subclass that allows the user to input values
 * that effect the generation of a default set of statistics.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 12, 2011
 * @version 1.0
 */
class RecrystallizedPresetDialog : public QDialog
{
    Q_OBJECT    ;
  public:
    RecrystallizedPresetDialog(QWidget* parent = NULL);
    virtual ~RecrystallizedPresetDialog();

    float getPercentRecrystallized();

  protected:
    void setupGui();

  private:
    QLineEdit* percentRecystallized;

    RecrystallizedPresetDialog(const RecrystallizedPresetDialog&); // Copy Constructor Not Implemented
    void operator=(const RecrystallizedPresetDialog&); // Operator '=' Not Implemented
};

#endif /* RECRYSTALLIZEDPRESETDIALOG_H_ */
