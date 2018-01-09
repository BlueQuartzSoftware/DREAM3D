

#include <iostream>
#include <sstream>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"


#include "DREAM3DToolsConfiguration.h"


class AnalyzePrebuiltPipelines
{
  public:
    AnalyzePrebuiltPipelines(){};
    ~AnalyzePrebuiltPipelines() = default;
    
    /**
     * @brief operator ()
     */
    void operator()()
    {
      execute();
    }
    
    /**
     * @brief execute
     */
    void execute()
    {
      // Register all the filters including trying to load those from Plugins
      FilterManager* fm = FilterManager::Instance();
      SIMPLibPluginLoader::LoadPluginFilters(fm, false);
      
      QMetaObjectUtilities::RegisterMetaTypes();
      
      m_FilterFactories = fm->getFactories();
      std::cout << "Total Filters: " << m_FilterFactories.size() << std::endl;
      
      recursiveDirFind(QDir(D3DTools::GetDREAM3DRuntimeDir() + "/PrebuiltPipelines"));
      
      std::cout << "Filters Missing Example: " << m_FilterFactories.size() << std::endl;
    }
    
    /**
     * @brief recursiveDirFind
     * @param dir
     */
    void recursiveDirFind(const QDir& currentDir)
    {
      QStringList fileFilters;
      fileFilters.append("*.json");
      
      // Get a list of all the directories
      QFileInfoList dirList = currentDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
      if(dirList.size() > 0)
      {
        foreach(QFileInfo fi, dirList)
        {
          recursiveDirFind(QDir(fi.absoluteFilePath())); // Recursive call
        }
      }
      
      QFileInfoList itemList = currentDir.entryInfoList(fileFilters);
      foreach(QFileInfo itemInfo, itemList)
      {
        QString itemFilePath = itemInfo.absoluteFilePath();
        //std::cout << "PIPELINE FILE: " << itemFilePath.toStdString() << std::endl;
        extractFilters(itemFilePath);
      }
    }
    
    /**
     * @brief extractFilters
     * @param pipelineFilePath
     */
    void extractFilters(const QString &pipelineFile)
    {
      // Sanity Check the filepath to make sure it exists, Report an error and bail if it does not
      QFileInfo fi(pipelineFile);
      if(fi.exists() == false)
      {
        std::cout << "Error Message:The Test Pipeline file '" << pipelineFile.toStdString() << "' does not exist," << std::endl;
        return;      
      }
      
      QString ext = fi.completeSuffix();
      
      // Use the static method to read the Pipeline file and return a Filter Pipeline
      FilterPipeline::Pointer pipeline;
      if(ext == "json")
      {
        JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
        pipeline = jsonReader->readPipelineFromFile(pipelineFile);
      }
      
      if(nullptr == pipeline.get())
      {
        std::cout << "An error occurred trying to read the pipeline file. Exiting now." << std::endl;
        return;
      }
      
      FilterPipeline::FilterContainerType filters = pipeline->getFilterContainer();
      //std::cout << "  Filter Count: " << filters.size() << std::endl;
      for(auto filter : filters)
      {
        m_FilterFactories.remove(filter->getNameOfClass());
      }
    }
    
    
  private:
    FilterManager::Collection m_FilterFactories;
    
    AnalyzePrebuiltPipelines(const AnalyzePrebuiltPipelines&) = delete; // Copy Constructor Not Implemented
    void operator=(const AnalyzePrebuiltPipelines&) = delete; // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("ExamplePipelineCoverage");
  
  std::cout << "Starting " << QCoreApplication::applicationName().toStdString() << std::endl;
  
  AnalyzePrebuiltPipelines()();
  
  std::cout << QCoreApplication::applicationName().toStdString() << " Complete." << std::endl;
  
  return 0;
}
