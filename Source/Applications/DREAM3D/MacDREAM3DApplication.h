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

#ifndef _MacDREAM3DApplication_H_
#define _MacDREAM3DApplication_H_

#include "Applications/DREAM3D/DREAM3DApplication.h"

#define macApp (static_cast<MacDREAM3DApplication*>(qApp))

class MacDREAM3DApplication : public DREAM3DApplication
{
  Q_OBJECT

  public:
    MacDREAM3DApplication(int& argc, char** argv);
    virtual ~MacDREAM3DApplication();

    virtual void unregisterDREAM3DWindow(DREAM3D_UI* window);

    void toEmptyMenuState();

  protected slots:

  /**
  * @brief Updates the QMenu 'Recent Files' with the latest list of files. This
  * should be connected to the Signal QRecentFileList->fileListChanged
  * @param file The newly added file.
  */
  virtual void updateRecentFileList(const QString& file);

  /**
  * @brief activeWindowChanged
  */
  virtual void dream3dWindowChanged(DREAM3D_UI* instance);

  virtual void toolboxWindowChanged();

  // DREAM3D_UI slots
  virtual void on_actionClearRecentFiles_triggered();

  void toToolboxMenuState();
  void toDREAM3DMenuState(DREAM3D_UI *instance);

private:
  // The global menu
  QMenuBar*                            m_GlobalMenu;

  void createGlobalMenu();

  MacDREAM3DApplication(const MacDREAM3DApplication&); // Copy Constructor Not Implemented
  void operator=(const MacDREAM3DApplication&); // Operator '=' Not Implemented
};

#endif /* _MacDREAM3DApplication_H */
