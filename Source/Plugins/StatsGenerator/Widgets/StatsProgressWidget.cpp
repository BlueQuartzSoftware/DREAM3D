/* ============================================================================
 * Copyright (c) 2018 BlueQuartz Software, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
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
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "StatsProgressWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsProgressWidget::StatsProgressWidget(const QString& title, const QString& buttonText, QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::StatsProgressWidget)
{
  m_Ui->setupUi(this);
  setupGui();
  setWindowTitle(title);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsProgressWidget::~StatsProgressWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsProgressWidget::setupGui()
{
  connect(m_Ui->cancel, SIGNAL(clicked()), this, SLOT(setCancel()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsProgressWidget::setValue(int i)
{
  m_Value = i;
  updateLabelText();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsProgressWidget::setLabelText(const QString& labelText)
{
  m_ProgressText = labelText;
  updateLabelText();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsProgressWidget::setProgTitle(const QString& progTitle)
{
  m_Ui->progTitle->setText(progTitle);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsProgressWidget::setCancel()
{
  m_Cancel = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool StatsProgressWidget::wasCanceled()
{
  return m_Cancel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsProgressWidget::updateLabelText()
{
  QString text = QString("%3").arg(m_ProgressText);
  m_Ui->labelText->setText(text);
}
