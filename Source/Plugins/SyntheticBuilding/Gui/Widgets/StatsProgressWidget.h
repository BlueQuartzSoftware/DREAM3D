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
#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtWidgets/QWidget>

#include "ui_StatsProgressWidget.h"

/**
 * @brief The StatsProgressWidget class
 */
class StatsProgressWidget : public QWidget
{
  Q_OBJECT

public:
  /**
   * @brief StatsProgressWidget
   * @param title
   * @param buttonText
   * @param parent
   */
  StatsProgressWidget(const QString& title, const QString& buttonText, QWidget* parent = nullptr);

  ~StatsProgressWidget() override;

public slots:

  /**
   * @brief setValue
   * @param i
   */
  void setValue(int i);

  /**
   * @brief setLabelText
   * @param labelText
   */
  void setLabelText(const QString& labelText);

  /**
   * @brief setProgTitle
   * @param progTitle
   */
  void setProgTitle(const QString& progTitle);

  /**
   * @brief wasCanceled
   * @return
   */
  bool wasCanceled();

  /**
   * @brief setCancel
   */
  void setCancel();

protected:
  /**
   * @brief updateLabelText
   */
  void updateLabelText();

  /**
   * @brief setupGui
   */
  void setupGui();

private:
  QSharedPointer<Ui::StatsProgressWidget> m_Ui;
  int m_Value = 0;
  QString m_ProgressText;
  bool m_Cancel = false;

public:
  StatsProgressWidget(const StatsProgressWidget&) = delete;            // Copy Constructor Not Implemented
  StatsProgressWidget(StatsProgressWidget&&) = delete;                 // Move Constructor Not Implemented
  StatsProgressWidget& operator=(const StatsProgressWidget&) = delete; // Copy Assignment Not Implemented
  StatsProgressWidget& operator=(StatsProgressWidget&&) = delete;      // Move Assignment Not Implemented
};

