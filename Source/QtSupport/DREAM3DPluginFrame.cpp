/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "DREAM3DPluginFrame.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DPluginFrame::DREAM3DPluginFrame(QWidget *parent) :
QFrame(parent),
m_StatusBar(NULL)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DPluginFrame::~DREAM3DPluginFrame()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DREAM3DPluginFrame::verifyOutputPathParentExists(QString outFilePath, QLineEdit* lineEdit)
{
  QFileInfo fileinfo(outFilePath);
  QDir parent (fileinfo.dir() );
//  if (false == parent.exists() )
//  {
//    lineEdit->setStyleSheet("border: 1px solid red;");
//  }
//  else
//  {
//    lineEdit->setStyleSheet("");
//  }
  return parent.exists();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DREAM3DPluginFrame::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
{
//  qDebug() << "outFilePath: " << outFilePath() << "\n";
  QFileInfo fileinfo(outFilePath);
  if (false == fileinfo.exists() )
  {
    lineEdit->setStyleSheet("border: 1px solid red;");
  }
  else
  {
    lineEdit->setStyleSheet("");
  }
  return fileinfo.exists();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DPluginFrame::setStatusBar(QStatusBar* statusBar)
{
  this->m_StatusBar = statusBar;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStatusBar* DREAM3DPluginFrame::statusBar()
{
  return this->m_StatusBar;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DREAM3DPluginFrame::sanityCheckOutputDirectory(QLineEdit* le, QString msgTitle)
{

  if (le->text().isEmpty() == true)
  {
    QMessageBox::critical(this, msgTitle,
                          "The output directory has NOT been set. Please set a directory path and try again.",
                          QMessageBox::Ok | QMessageBox::Default);
    return false;
  }

  if (verifyPathExists(le->text(), le) == false)
  {
    QString msg("The Output Directory '");
    msg.append(le->text()).append("'\ndoes not exist. Would you like to create it?");
    int ret = QMessageBox::warning(this, msgTitle,
                                   msg,
                                   QMessageBox::Yes | QMessageBox::Default,
                                   QMessageBox::No);
    if (ret == QMessageBox::No)
    {
      return false;
    }
    else if (ret == QMessageBox::Yes)
    {
      QDir outputDir(le->text());
      if (outputDir.exists() == false)
      {
        bool ok = outputDir.mkpath(".");
        if (ok == false)
        {
          QMessageBox::critical(this,
                                tr("Output Directory Creation"),
                                tr("The output directory could not be created."),
                                QMessageBox::Ok);
          return false;
        }
        else
        {
          return true;
        }
      }
    }
  }
  return true;

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DPluginFrame::displayDialogBox(QString title, QString text, QMessageBox::Icon icon)
{

  QMessageBox msgBox;
  msgBox.setText(title);
  msgBox.setInformativeText(text);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.setDefaultButton(QMessageBox::Ok);
  msgBox.setIcon(icon);
  msgBox.exec();
}


