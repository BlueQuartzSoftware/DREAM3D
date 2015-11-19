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


#include "QFSDropLineEdit.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtCore/QTextStream>
#include <QtCore/QMimeData>
#include <QtWidgets/QWidget>
#include <QtGui/QDragEnterEvent>

#include "QtSupportLib/DREAM3DStyles.h"

#include "moc_QFSDropLineEdit.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFSDropLineEdit::QFSDropLineEdit(QWidget* parent)
  : QLineEdit(parent)
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFSDropLineEdit::changeStyleSheet(int style)
{
  QString styleSheet;
  QTextStream ss(&styleSheet);

  ss << "QLabel {";
#if defined(Q_OS_WIN)
  ss << "font: italic 9 pt \"" << DREAM3DStyles::GetUIFont() << "\";";
#elif defined(Q_OS_MAC)
  ss << "font: italic 12 pt \"" << DREAM3DStyles::GetUIFont() << "\";";
#else
  ss << "font: italic 10 pt \"" << DREAM3DStyles::GetUIFont() << "\";";
#endif

  if(style == FS_STANDARD_STYLE)
  {

  }
  else if(style == FS_DRAGGING_STYLE)
  {
    ss << "border: 2px solid rgb(34, 170, 46);";
    ss << "border-radius: 5px;";
  }
  else if(style == FS_DOESNOTEXIST_STYLE)
  {
    ss << "color: rgb(200, 50, 50); font: bold;";
  }
  else if(style == FS_WARNING_STYLE)
  {
    ss << "color: rgb(255, 140, 0); font: bold;";
  }


  ss << "}";

  setStyleSheet(styleSheet);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFSDropLineEdit::dragEnterEvent(QDragEnterEvent* event)
{
  // accept just text/uri-list mime format
  if (event->mimeData()->hasFormat("text/uri-list"))
  {
    event->acceptProposedAction();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFSDropLineEdit::dropEvent(QDropEvent* event)
{
  QList<QUrl> urlList;
  QString fName;
  QFileInfo info;

  if (event->mimeData()->hasUrls())
  {
    urlList = event->mimeData()->urls(); // returns list of QUrls
    // if just text was dropped, urlList is empty (size == 0)

    if ( urlList.size() > 0) // if at least one QUrl is present in list
    {
      fName = urlList[0].toLocalFile(); // convert first QUrl to local path
      fName = QDir::toNativeSeparators(fName);
      info.setFile( fName ); // information about file
      setText( fName ); // if is file, setText
      emit fileDropped(fName);
    }
  }

  event->acceptProposedAction();
}


