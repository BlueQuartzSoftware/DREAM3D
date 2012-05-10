/*
* INSERT A LICENSE OR COPYRIGHT STATEMENT HERE.
*/

#include "FileConversionPlugin.h"
#include "FileConversionWidget.h"
#include "QtSupport/DREAM3DPluginFrame.h"
#include "QtSupport/HelpDialog.h"

Q_EXPORT_PLUGIN2(FileConversionPlugin, FileConversionPlugin);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileConversionPlugin::FileConversionPlugin() :
m_InputWidget(NULL)
{
  m_InputWidget = new FileConversionWidget(NULL);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileConversionPlugin::~FileConversionPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FileConversionPlugin::getPluginName()
{
  return QString::fromStdString(DREAM3D::UIPlugins::FileConversionDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* FileConversionPlugin::getInputWidget(QWidget* parent)
{
  m_InputWidget->setParent(parent);
  return m_InputWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DPluginFrame* FileConversionPlugin::getPluginFrame(QWidget* parent)
{
  DREAM3DPluginFrame* frame = qobject_cast<DREAM3DPluginFrame*>(m_InputWidget);
  return frame;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionPlugin::writeSettings(QSettings &prefs)
{
  m_InputWidget->writeSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionPlugin::readSettings(QSettings &prefs)
{
  m_InputWidget->readSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon FileConversionPlugin::icon()
{
  return QIcon(":/FileConversion.png");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileConversionPlugin::displayHelp()
{
  emit showHelp(htmlHelpIndexFile());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUrl FileConversionPlugin::htmlHelpIndexFile()
{
  QString s = QString::fromStdString(DREAM3D::UIPlugins::FileConversionBaseName);
  s.append(QString("/index.html"));
  return QUrl(s);
}
