/*
* INSERT A LICENSE OR COPYRIGHT STATEMENT HERE.
*/

#include "Extrapolation2Dto3DPlugin.h"
#include "Extrapolation2Dto3DWidget.h"
#include "QtSupport/DREAM3DPluginFrame.h"
#include "QtSupport/HelpDialog.h"

Q_EXPORT_PLUGIN2(Extrapolation2Dto3DPlugin, Extrapolation2Dto3DPlugin);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Extrapolation2Dto3DPlugin::Extrapolation2Dto3DPlugin() :
m_InputWidget(NULL)
{
  m_InputWidget = new Extrapolation2Dto3DWidget(NULL);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Extrapolation2Dto3DPlugin::~Extrapolation2Dto3DPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Extrapolation2Dto3DPlugin::getPluginName()
{
  return QString::fromStdString(DREAM3D::UIPlugins::Extrapolation2Dto3DDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* Extrapolation2Dto3DPlugin::getInputWidget(QWidget* parent)
{
  m_InputWidget->setParent(parent);
  return m_InputWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DPluginFrame* Extrapolation2Dto3DPlugin::getPluginFrame(QWidget* parent)
{
  DREAM3DPluginFrame* frame = qobject_cast<DREAM3DPluginFrame*>(m_InputWidget);
  return frame;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3DPlugin::writeSettings(QSettings &prefs)
{
  m_InputWidget->writeSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3DPlugin::readSettings(QSettings &prefs)
{
  m_InputWidget->readSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon Extrapolation2Dto3DPlugin::icon()
{
  return QIcon(":/Extrapolation2Dto3D.png");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3DPlugin::displayHelp()
{
  emit showHelp(htmlHelpIndexFile());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUrl Extrapolation2Dto3DPlugin::htmlHelpIndexFile()
{
  QString s = QString::fromStdString(DREAM3D::UIPlugins::Extrapolation2Dto3DBaseName);
  s.append(QString("/index.html"));
  return QUrl(s);
}
