/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
#include "SGApplication.h"
#include "StatsGeneratorUI.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGApplication::SGApplication(int &argc, char **argv)
: QApplication(argc, argv)
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGApplication::~SGApplication()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGeneratorUI* SGApplication::createNewStatsGenerator()
{
  StatsGeneratorUI* sg = new StatsGeneratorUI;
  m_Windows << sg;
  connect(sg, SIGNAL(windowIsClosing(StatsGeneratorUI*)),
          this, SLOT(windowClosed(StatsGeneratorUI*)));
  return sg;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SGApplication::windowClosed(StatsGeneratorUI* window)
{
  m_Windows.removeAll(window);
  window->deleteLater();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SGApplication::newTempFile(QString directory)
{
  StatsGeneratorUI* sg = NULL;
  int i = 1;
  bool fileExists = false;
  QString name;
  while (true)
  {
    name = QString("Untitled-");
    name.append(QString::number(i)).append(".h5");
    name = directory + QDir::separator() + name;
    fileExists = false;
    Q_FOREACH(sg, m_Windows)
    {
      QString f = sg->getFilePath();
      if (f.compare(name) == 0)
      {
        fileExists = true;
      }
    }
    if (fileExists == false)
    {
      break;
    }
    ++i; // Increment the file number
  }
  return name;
}
