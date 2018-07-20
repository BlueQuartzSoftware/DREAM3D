

#include <QtCore/QCoreApplication>
#include <QtCore/QDate>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSet>

#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"

#include "SIMPLView/SIMPLViewVersion.h"

#include "DREAM3DToolsConfiguration.h"

class RESTServerInfo
{

public:
  RESTServerInfo(const QString filePath)
  : m_FilePath(filePath)
  {
  }

  ~RESTServerInfo() = default;
  RESTServerInfo(const RESTServerInfo&) = delete;            // Copy Constructor Not Implemented
  RESTServerInfo(RESTServerInfo&&) = delete;                 // Move Constructor
  RESTServerInfo& operator=(const RESTServerInfo&) = delete; // Copy Assignment Not Implemented
  RESTServerInfo& operator=(RESTServerInfo&&) = delete;      // Move Assignment Not Implemented

  /**
   * @brief execute
   */
  void execute()
  {
    PluginManager* pm = PluginManager::Instance();
    QVector<ISIMPLibPlugin*> plugins = pm->getPluginsVector();

    // Write our File Version and DREAM3D Version strings
    QJsonObject dream3dMeta;

    dream3dMeta["Release Date"] = QDate::currentDate().toString();
    dream3dMeta["Release Type"] = SIMPLProj_RELEASE_TYPE;
    dream3dMeta["Major"] = SIMPLView::Version::Major();
    dream3dMeta["Minor"] = SIMPLView::Version::Minor();
    dream3dMeta["Patch"] = SIMPLView::Version::Patch();
    dream3dMeta["Revision"] = SIMPLView::Version::Revision();
    dream3dMeta["Download WebSite"] = "http://dream3d.bluequartz.net/?page_id=32";
    QJsonObject m_Root;
    m_Root["DREAM3D"] = dream3dMeta;

    QJsonObject simplMeta;
    simplMeta["Major"] = SIMPLib::Version::Major();
    simplMeta["Minor"] = SIMPLib::Version::Minor();
    simplMeta["Patch"] = SIMPLib::Version::Patch();
    simplMeta["Revision"] = SIMPLib::Version::Revision();
    m_Root["SIMPL"] = simplMeta;

    QJsonArray plugArray;

    for(int i = 0; i < plugins.size(); i++)
    {
      ISIMPLibPlugin* plug = plugins[i];
      QJsonObject jobj;
      jobj["Base Name"] = plug->getPluginBaseName();
      jobj["Version"] = plug->getVersion();
      jobj["Vendor"] = plug->getVendor();
      jobj["Compatibility Version"] = plug->getCompatibilityVersion();
      jobj["Display Name"] = plug->getPluginDisplayName();
      jobj["URL"] = plug->getURL();
      plugArray.append(jobj);
    }
    m_Root["Plugins"] = plugArray;

    // Loop over all the filters
    FilterManager* fm = FilterManager::Instance();
    QJsonArray filterArray;

    FilterManager::Collection factories = fm->getFactories();
    for(auto factory : factories)
    {
      AbstractFilter::Pointer filter = factory->create();
      QJsonObject filtJson;

      filtJson["Name"] = filter->getHumanLabel();
      filtJson["ClassName"] = filter->getNameOfClass();
      filtJson["Uuid"] = filter->getUuid().toString();
      filtJson["Plugin Name"] = filter->getCompiledLibraryName();
      filtJson["Version"] = filter->getFilterVersion();
      filtJson["Group Name"] = filter->getGroupName();
      filtJson["SubGroup Name"] = filter->getSubGroupName();

      filterArray.append(filtJson);
    }

    m_Root["Filters"] = filterArray;

    QJsonDocument doc(m_Root);
    QFile outputFile(m_FilePath);
    if(outputFile.open(QIODevice::WriteOnly))
    {
      outputFile.write(doc.toJson());
      outputFile.close();
    }
  }

private:
  QString m_FilePath;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Generate(const QString& filePath)
{
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
  QCoreApplication::setApplicationName("FilterInfoUtility");

  if(argc != 2)
  {
    std::cout << "Program requires 1 argument:" << std::endl;
    std::cout << "   [1] Output File: Path (relative or absolute) to the output file." << std::endl;
    return EXIT_FAILURE;
  }

  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  // Load all the plugins
  SIMPLibPluginLoader::LoadPluginFilters(fm);

  QString filePath = QString::fromLatin1(argv[1]);
  // Send progress messages from PipelineBuilder to this object for display
  RESTServerInfo rsi(filePath);
  rsi.execute();

  int err = EXIT_SUCCESS;

  return err;
}
