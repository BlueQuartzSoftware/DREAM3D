



#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QSet>
#include <QtCore/QDate>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"

#include "Applications/SIMPLView/SIMPLViewVersion.h"

#include "ToolConfiguration.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteWebServerJSON(const QString& filePath)
{


  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);
  PluginManager* pm = PluginManager::Instance();
  QVector<ISIMPLibPlugin*> plugins = pm->getPluginsVector();

  // Send progress messages from PipelineBuilder to this object for display
  QMetaObjectUtilities::RegisterMetaTypes();


  // Write our File Version and DREAM3D Version strings
  QJsonObject meta;

  meta["Release Date"] = QDate::currentDate().toString();
  meta["Release Type"] = SIMPLProj_RELEASE_TYPE;
  meta["Major"] = SIMPLView::Version::Major();
  meta["Minor"] = SIMPLView::Version::Minor();
  meta["Patch"] = SIMPLView::Version::Patch();
  meta["Revision"] = SIMPLView::Version::Revision();
  meta["Download WebSite"] = "http://dream3d.bluequartz.net/?page_id=32";
  QJsonObject m_Root;
  m_Root["DREAM3D"] = meta;


  QJsonArray plugArray;

  for(int i = 0; i < plugins.size(); i++)
  {
    ISIMPLibPlugin* plug = plugins[i];
    QJsonObject jobj;
    jobj["Plugin Name"] = plug->getPluginName();
    jobj["Plugin Version"] = plug->getVersion();
    jobj["Plugin Vendor"] = plug->getVendor();
    plugArray.append(jobj);
  }
  m_Root["Plugins"] = plugArray;

  QJsonDocument doc(m_Root);
  QFile outputFile(filePath);
  if (outputFile.open(QIODevice::WriteOnly))
  {
    outputFile.write(doc.toJson());
    outputFile.close();
  }

}




// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("CreateWebServerJson");

  QString filePath = D3DTools::GetDREAM3DBinaryDir() + QString("/dream3d_version.json");
  if(argc == 2)
  {
    filePath = QString::fromLatin1(argv[1]);
  }

  WriteWebServerJSON(filePath);


  int err = EXIT_SUCCESS;


  return err;
}



