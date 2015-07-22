



#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QSet>
#include <QtCore/QDate>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DREAM3DLibVersion.h"
#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Plugin/IDREAM3DPlugin.h"
#include "DREAM3DLib/Plugin/PluginManager.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteWebServerJSON(const QString& filePath)
{


  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  DREAM3DPluginLoader::LoadPluginFilters(fm);
  PluginManager* pm = PluginManager::Instance();
  QVector<IDREAM3DPlugin*> plugins = pm->getPluginsVector();

  // Send progress messages from PipelineBuilder to this object for display
  QMetaObjectUtilities::RegisterMetaTypes();


  // Write our File Version and DREAM3D Version strings
  QJsonObject meta;

  meta["Release Date"] = QDate::currentDate().toString();
  meta["Release Type"] = DREAM3DProj_RELEASE_TYPE;
  meta["Major"] = DREAM3DLib::Version::Major();
  meta["Minor"] = DREAM3DLib::Version::Minor();
  meta["Patch"] = DREAM3DLib::Version::Patch();
  meta["Revision"] = DREAM3DLib::Version::Revision();
  meta["Download WebSite"] = "http://dream3d.bluequartz.net/?page_id=32";
  QJsonObject m_Root;
  m_Root["DREAM3D"] = meta;


  QJsonArray plugArray;

  for(int i = 0; i < plugins.size(); i++)
  {
    IDREAM3DPlugin* plug = plugins[i];
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

  QString filePath = "/tmp/dream3d_version.json";
  if(argc == 2)
  {
    filePath = QString::fromLatin1(argv[1]);
  }

  WriteWebServerJSON(filePath);


  int err = EXIT_SUCCESS;


  return err;
}



