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


#include "SIMPLPluginFrame.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include "moc_SIMPLPluginFrame.cpp"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLPluginFrame::SIMPLPluginFrame(QWidget* parent) :
  QFrame(parent),
  m_StatusBar(NULL)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLPluginFrame::~SIMPLPluginFrame()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLPluginFrame::verifyOutputPathParentExists(QString outFilePath, QLineEdit* lineEdit)
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
bool SIMPLPluginFrame::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
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
void SIMPLPluginFrame::setStatusBar(QStatusBar* statusBar)
{
  this->m_StatusBar = statusBar;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStatusBar* SIMPLPluginFrame::statusBar()
{
  return this->m_StatusBar;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLPluginFrame::sanityCheckOutputDirectory(QLineEdit* le, QString msgTitle)
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
void SIMPLPluginFrame::displayDialogBox(QString title, QString text, QMessageBox::Icon icon)
{

  QMessageBox msgBox;
  msgBox.setText(title);
  msgBox.setInformativeText(text);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.setDefaultButton(QMessageBox::Ok);
  msgBox.setIcon(icon);
  msgBox.exec();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLPluginFrame::writeSettings(QSettings& prefs) {}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLPluginFrame::readSettings(QSettings& prefs, bool shouldClear) {}
