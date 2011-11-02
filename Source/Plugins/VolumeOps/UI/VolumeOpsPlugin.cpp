/*
* INSERT A LICENSE OR COPYRIGHT STATEMENT HERE.
*/

#include "VolumeOpsPlugin.h"
#include "VolumeOpsWidget.h"
#include "QtSupport/DREAM3DPluginFrame.h"
#include "QtSupport/HelpDialog.h"

Q_EXPORT_PLUGIN2(VolumeOpsPlugin, VolumeOpsPlugin);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeOpsPlugin::VolumeOpsPlugin() :
m_InputWidget(NULL)
{
  m_InputWidget = new VolumeOpsWidget(NULL);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeOpsPlugin::~VolumeOpsPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VolumeOpsPlugin::getPluginName()
{
  return QString::fromStdString(DREAM3D::UIPlugins::VolumeOpsDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* VolumeOpsPlugin::getInputWidget(QWidget* parent)
{
  m_InputWidget->setParent(parent);
  return m_InputWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DPluginFrame* VolumeOpsPlugin::getPluginFrame(QWidget* parent)
{
  DREAM3DPluginFrame* frame = qobject_cast<DREAM3DPluginFrame*>(m_InputWidget);
  return frame;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsPlugin::writeSettings(QSettings &prefs)
{
  m_InputWidget->writeSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsPlugin::readSettings(QSettings &prefs)
{
  m_InputWidget->readSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon VolumeOpsPlugin::icon()
{
  return QIcon(":/VolumeOps.png");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VolumeOpsPlugin::displayHelp()
{
  emit showHelp(htmlHelpIndexFile());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUrl VolumeOpsPlugin::htmlHelpIndexFile()
{
  QString s = QString::fromStdString(DREAM3D::UIPlugins::VolumeOpsBaseName);
  s.append(QString("/index.html"));
  return QUrl(s);
}
