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

#include "MicrostructureStatisticsPlugin.h"
#include "MicrostructureStatisticsWidget.h"
#include "QtSupport/DREAM3DPluginFrame.h"
#include "QtSupport/HelpDialog.h"

Q_EXPORT_PLUGIN2(MicrostructureStatisticsPlugin, MicrostructureStatisticsPlugin)
;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicrostructureStatisticsPlugin::MicrostructureStatisticsPlugin() :
m_InputWidget(NULL)
{
  m_InputWidget = new MicrostructureStatisticsWidget(NULL);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicrostructureStatisticsPlugin::~MicrostructureStatisticsPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MicrostructureStatisticsPlugin::getPluginName()
{
  return QString::fromStdString(DREAM3D::UIPlugins::MicrostructureStatisticsDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* MicrostructureStatisticsPlugin::getInputWidget(QWidget* parent)
{
  m_InputWidget->setParent(parent);
  return m_InputWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DPluginFrame* MicrostructureStatisticsPlugin::getPluginFrame(QWidget* parent)
{
  DREAM3DPluginFrame* frame = qobject_cast<DREAM3DPluginFrame*>(m_InputWidget);
  return frame;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsPlugin::writeSettings(QSettings &prefs)
{
  m_InputWidget->writeSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsPlugin::readSettings(QSettings &prefs)
{
  m_InputWidget->readSettings(prefs);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon MicrostructureStatisticsPlugin::icon()
{
  return QIcon(":/MicrostructureStatistics.png");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsPlugin::displayHelp()
{
  emit showHelp(htmlHelpIndexFile());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUrl MicrostructureStatisticsPlugin::htmlHelpIndexFile()
{
  QString s = QString::fromStdString(DREAM3D::UIPlugins::MicrostructureStatisticsBaseName);
  s.append(QString("/index.html"));
  return QUrl(s);
}

