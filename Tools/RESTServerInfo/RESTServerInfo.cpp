

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

#include "SIMPLib/Plugin/SIMPLPluginConstants.h"

class RESTServerInfo
{
    
  public:
    RESTServerInfo(const QString filePath)
      : m_FilePath(filePath)
    {
    }
    
    ~RESTServerInfo() = default;
    RESTServerInfo(const RESTServerInfo&) = delete;            // Copy Constructor Not Implemented
    RESTServerInfo(RESTServerInfo&&) = delete;                 // Move Constructor Not Implemented
    RESTServerInfo& operator=(const RESTServerInfo&) = delete; // Copy Assignment Not Implemented
    RESTServerInfo& operator=(RESTServerInfo&&) = delete;      // Move Assignment Not Implemented
    
    /**
   * @brief execute
   */
    void execute()
    {

      
      // Write our File Version and DREAM3D Version strings
      QJsonObject dream3dMeta;
      
      dream3dMeta[SIMPL::JSON::ReleaseDate] = QDate::currentDate().toString();
      dream3dMeta[SIMPL::JSON::ReleaseType] = SIMPLProj_RELEASE_TYPE;
      dream3dMeta[SIMPL::JSON::MajorVersion] = SIMPLView::Version::Major();
      dream3dMeta[SIMPL::JSON::MinorVersion] = SIMPLView::Version::Minor();
      dream3dMeta[SIMPL::JSON::PatchVersion] = SIMPLView::Version::Patch();
      dream3dMeta[SIMPL::JSON::Revision] = SIMPLView::Version::Revision();
      dream3dMeta[SIMPL::JSON::DownloadURL] = "http://dream3d.bluequartz.net/?page_id=32";
      QJsonObject m_Root;
      m_Root[SIMPL::JSON::DREAM3D] = dream3dMeta;
      
      QJsonObject simplMeta;
      simplMeta[SIMPL::JSON::MajorVersion] = SIMPLib::Version::Major();
      simplMeta[SIMPL::JSON::MinorVersion] = SIMPLib::Version::Minor();
      simplMeta[SIMPL::JSON::PatchVersion] = SIMPLib::Version::Patch();
      simplMeta[SIMPL::JSON::Revision] = SIMPLib::Version::Revision();
      m_Root[SIMPL::JSON::SIMPL] = simplMeta;
      
      // Loop over all the plugins
      PluginManager* pm = PluginManager::Instance();
      m_Root[SIMPL::JSON::Plugins] = pm->toJsonArray();
      
      // Loop over all the filters
      FilterManager* fm = FilterManager::Instance();
      m_Root[SIMPL::JSON::Filters] = fm->toJsonArray();
      
      
      QJsonDocument doc(m_Root);
      
      // Make sure any directory path is also available as the user may have just typed
      // in a path without actually creating the full path
      QFileInfo fi(m_FilePath);
      QString parentPath = fi.path();
      QDir dir;
      if(!dir.mkpath(parentPath))
      {
        QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
        qDebug() << ss;
        return;
      }
      
      
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
  QCoreApplication::setApplicationName("RESTServerInfo");
  
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
