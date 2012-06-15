/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2012 Joseph B. Kleingers (Student Research Assistant)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, Joseph B. Kleingers,
 * the US Air Force, BlueQuartz Software nor the names of its contributors may be 
 * used to endorse or promote products derived from this software without specific 
 * prior written permission.
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

#include "AddFavoriteWidget.h"

AddFavoriteWidget::AddFavoriteWidget(QWidget *parent) {
  setupUi(this);
  addfavoriteOKButton->setEnabled(false);
  BtnClicked = false;
}

// -----------------------------------------------------------------------------
//  Called when the OK button is clicked.
// -----------------------------------------------------------------------------
void AddFavoriteWidget::on_addfavoriteOKButton_clicked() {
  BtnClicked = true;
  this->close();
}

// -----------------------------------------------------------------------------
//  Called when the Cancel button is clicked.
// -----------------------------------------------------------------------------
void AddFavoriteWidget::on_addfavoriteCancelButton_clicked() {
  BtnClicked = false;
  this->close();
}

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
bool AddFavoriteWidget::getBtnClicked() {
  return BtnClicked;
}

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
QString AddFavoriteWidget::getFavoriteName() {
  return ( favoriteName->text() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AddFavoriteWidget::on_favoriteName_textChanged(const QString & text) {
  if (favoriteName->text() == "") {
    addfavoriteOKButton->setEnabled(false);
  }
  else {
    addfavoriteOKButton->setEnabled(true);
  }
}