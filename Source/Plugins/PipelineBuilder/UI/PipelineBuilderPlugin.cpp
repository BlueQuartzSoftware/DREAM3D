/*
* INSERT A LICENSE OR COPYRIGHT STATEMENT HERE.
*/

#include "PipelineBuilderPlugin.h"
#include "PipelineBuilderWidget.h"
#include "QtSupport/DREAM3DPluginFrame.h"
#include "QtSupport/HelpDialog.h"

Q_EXPORT_PLUGIN2(PipelineBuilderPlugin, PipelineBuilderPlugin);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineBuilderPlugin::PipelineBuilderPlugin() :
m_InputWidget(NULL)
{
  m_InputWidget = new PipelineBuilderWidget(NULL);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineBuilderPlugin::~PipelineBuilderPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineBuilderPlugin::getPluginName()
{
  return QString::fromStdString(DREAM3D::UIPlugins::PipelineBuilderDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* PipelineBuilderPlugin::getInputWidget(QWidget* parent)
{
  m_InputWidget->setParent(parent);
  return m_InputWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DPluginFrame* PipelineBuilderPlugin::getPluginFrame(QWidget* parent)
{
  DREAM3DPluginFrame* frame = qobject_cast<DREAM3DPluginFrame*>(m_InputWidget);
  return frame;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderPlugin::writeSettings(QSettings &prefs)
{
  m_InputWidget->writeSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderPlugin::readSettings(QSettings &prefs)
{
  m_InputWidget->readSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon PipelineBuilderPlugin::icon()
{
  return QIcon(":/PipelineBuilder.png");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineBuilderPlugin::displayHelp()
{
  emit showHelp(htmlHelpIndexFile());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUrl PipelineBuilderPlugin::htmlHelpIndexFile()
{
  QString s = QString::fromStdString(DREAM3D::UIPlugins::PipelineBuilderBaseName);
  s.append(QString("/index.html"));
  return QUrl(s);
}
