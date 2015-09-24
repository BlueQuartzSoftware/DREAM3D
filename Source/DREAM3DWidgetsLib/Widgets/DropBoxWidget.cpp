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

#include "DropBoxWidget.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QTextStream>

#include <QtWidgets/QMessageBox>

#include "QtSupportLib/DREAM3DStyles.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_DropBoxWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DropBoxWidget::DropBoxWidget(QWidget* parent) :
  QWidget(parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DropBoxWidget::~DropBoxWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DropBoxWidget::setupGui()
{
  {
    QString frameStyle;
    QTextStream ss(&frameStyle);

    ss << "QFrame#frame{"
       << "color: rgb(127, 0, 63);"
       << "border: 2px solid MediumBlue;"
       << "border-top-left-radius: 5px;"
       << "border-top-right-radius: 5px;"
       << "border-bottom-left-radius: 5px;"
       << "border-bottom-right-radius: 5px; "
       << "}";

    frame->setStyleSheet(frameStyle);
  }

  {
    QString labelStyle;
    QTextStream ss(&labelStyle);

    ss << "QLabel#label{"
       << "color: MediumBlue;"

#if defined(Q_OS_WIN)
       << "font: 9pt \"" << DREAM3DStyles::GetUIFont() << "\";"
#elif defined(Q_OS_MAC)
       << "font: 100 italic 12pt \"" << DREAM3DStyles::GetUIFont() << "\";"
#else
       << "font: 100 italic 10pt \"" << DREAM3DStyles::GetUIFont() << "\";"
#endif
       << "font-weight: bold; "
       << "}";

    label->setStyleSheet(labelStyle);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFrame* DropBoxWidget::getFrame()
{
  return frame;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DropBoxWidget::setLabel(QString text)
{
  label->setText(text);
}







