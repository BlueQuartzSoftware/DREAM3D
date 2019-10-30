

#include <iostream>
#include <sstream>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <QtCore/QTextStream>

#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"

#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/PluginManager.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"

#include "DREAM3DToolsConfiguration.h"

#define OVERWRITE_SOURCE_FILE 0

class AnalyzePrebuiltPipelines
{
public:
  AnalyzePrebuiltPipelines()
  {
  }
  ~AnalyzePrebuiltPipelines() = default;

  /**
   * @brief operator ()
   */
  void operator()()
  {
    execute();
  }

  /**
   * @brief dumpUuids
   */
  void dumpUuids()
  {
    QFile outFile(D3DTools::GetDREAM3DBinaryDir() + "/Filter_UUID.json");
    outFile.open(QFile::WriteOnly);

    QTextStream stream(&outFile);
    stream << "{\n";

    FilterManager* fm = FilterManager::Instance();
    FilterManager::Collection factories = fm->getFactories();

    QMapIterator<QString, IFilterFactory::Pointer> iter(m_FilterFactories);
    for(auto iter : m_FilterFactories)
    {
      IFilterFactory* fact = iter.get();
      AbstractFilter::Pointer filter = fact->create();
      stream << "\"" << filter->getNameOfClass() << "\" : ";
      stream << "\"" << filter->getUuid().toString() << "\","
             << "\n";
    }

    stream << "}\n";

    outFile.close();
  }

  /**
   * @brief execute
   */
  void execute()
  {

    QString docBinDir = D3DTools::GetDREAM3DBinaryRefManualDir();
    QString jsonFilePath = docBinDir + QDir::separator() + "PluginIndex.json";
    QFileInfo fi(jsonFilePath);

    if(!fi.exists())
    {
      std::cout << "Input Json file not found at " << jsonFilePath.toStdString() << std::endl;
      return;
    }

    QByteArray json;
    {
      // Read the json file
      QFile source(fi.absoluteFilePath());
      bool isOpen = source.open(QFile::ReadOnly);
      Q_ASSERT(isOpen);
      json = source.readAll();
      source.close();
    }

    QJsonParseError parseError;
    m_JsonDoc = QJsonDocument::fromJson(json, &parseError);
    if(parseError.error != QJsonParseError::NoError)
    {
      std::cout << "JSON Parse Error: " << parseError.errorString().toStdString() << std::endl;
      return;
    }
    QJsonObject jsonDocObject = m_JsonDoc.object();
    QJsonObject pluginsObj = jsonDocObject["Plugins"].toObject();

    // Register all the filters including trying to load those from Plugins
    FilterManager* fm = FilterManager::Instance();
    SIMPLibPluginLoader::LoadPluginFilters(fm, false);

    QMetaObjectUtilities::RegisterMetaTypes();

    m_FilterFactories = fm->getFactories();
    std::cout << "Total Filters: " << m_FilterFactories.size() << std::endl;
    dumpUuids();

    recursiveDirFind(QDir(D3DTools::GetDREAM3DRuntimeDir() + "/PrebuiltPipelines"));

#if 0
      QMapIterator<QString, QSet<QString>> iter(m_UsedFilters);
      
      std::cout << "# Filters **with** Examples: " << m_UsedFilters.size() << " #\n" << std::endl;
      
      std::cout << "|  Filter Name | Pipelines |" << std::endl;
      std::cout << "|--------------|-----------|" << std::endl;
      
      while(iter.hasNext())
      {
        iter.next();
        QString key = iter.key();
        QSet<QString> value = iter.value();
        QString humanLabel = fm->getFactoryFromClassName(key)->create()->getHumanLabel();
        
        std::cout << "| " << humanLabel.toStdString() << " | ";
        
        for(auto name : value)
        {
          if(value.find(name) != value.end()-1)
          {
            std::cout << name.toStdString() << ", ";
          }
          else
          {
            std::cout << name.toStdString();
          }
        }
        std::cout << " |" << std::endl;
      }
#endif
    std::cout << "# Filters **with** Examples: " << m_UsedFilters.size() << " #\n" << std::endl;

    PluginManager* pm = PluginManager::Instance();
    QVector<ISIMPLibPlugin*> plugins = pm->getPluginsVector();
    for(auto plugin : plugins) // Loop over all plugins
    {
      std::cout << "## " << plugin->getPluginDisplayName().toStdString() << " ##\n" << std::endl;
      std::cout << "| Filter Name | Doc Path Exists | Doc Path |" << std::endl;
      std::cout << "|-------------|-----------------|----------|" << std::endl;
      QList<QString> filters = plugin->getFilters();
      for(auto filterName : filters) // Loop over all filters in the plugin
      {
        if(m_UsedFilters.contains(filterName)) // Make sure the filter is still in the QMap<>
        {
          IFilterFactory::Pointer fact = fm->getFactoryFromClassName(filterName); // Grap the IFilterFactory
          AbstractFilter::Pointer filter = fact->create();

          QJsonObject pluginObj = pluginsObj[filter->getCompiledLibraryName()].toObject();

          QString docPath = QString("%1/%2/%3.md").arg(pluginObj["SOURCE_DIR"].toString()).arg(pluginObj["DOC_DIR"].toString()).arg(filterName);
          QFileInfo fi(docPath);

          QSet<QString> value = m_UsedFilters[filterName];
          QString humanLabel = fm->getFactoryFromClassName(filterName)->create()->getHumanLabel();

          std::cout << "| " << humanLabel.toStdString() << " | ";

          for(auto name : value)
          {
            if(value.find(name) != value.end() - 1)
            {
              std::cout << name.toStdString() << ", ";
            }
            else
            {
              std::cout << name.toStdString();
            }
          }
          std::cout << " |" << std::endl;

          if(false)
          {
            adjustMarkdownFile(docPath, value);
          }
        }
      }
      std::cout << "\n" << std::endl;
    }

    std::cout << "# Filters **without** Examples: " << m_FilterFactories.size() << " #\n" << std::endl;
    for(auto plugin : plugins) // Loop over all plugins
    {
      std::cout << "## " << plugin->getPluginDisplayName().toStdString() << " ##\n" << std::endl;
      std::cout << "| Filter Name | Doc Path Exists | Doc Path |" << std::endl;
      std::cout << "|-------------|-----------------|----------|" << std::endl;
      QList<QString> filters = plugin->getFilters();
      for(auto filterName : filters) // Loop over all filters in the plugin
      {
        if(m_FilterFactories.contains(filterName)) // Make sure the filter is still in the QMap<>
        {
          IFilterFactory::Pointer fact = m_FilterFactories[filterName]; // Grap the IFilterFactory
          AbstractFilter::Pointer filter = fact->create();

          QJsonObject pluginObj = pluginsObj[filter->getCompiledLibraryName()].toObject();

          QString docPath = QString("%1/%2/%3.md").arg(pluginObj["SOURCE_DIR"].toString()).arg(pluginObj["DOC_DIR"].toString()).arg(filterName);
          QFileInfo fi(docPath);
          //            if(!fi.exists()) {
          //              std::cout << "Here" << std::endl;
          //            }

          std::cout << "| " << filter->getNameOfClass().toStdString() << " | " << (fi.exists() ? "TRUE" : "FALSE") << " | " << docPath.toStdString() << " |" << std::endl;
        }
      }
      std::cout << "\n" << std::endl;
    }

#if 0
      std::cout << "\n\n\n" << std::endl;
      std::cout << "Filters Missing Example: " << m_FilterFactories.size() << std::endl;
      std::cout << "| Filter Name | Plugin |" << std::endl;
      std::cout << "|-------------|--------|" << std::endl;
      for(auto iter : m_FilterFactories)
      {
        IFilterFactory* fact = iter.get();
        AbstractFilter::Pointer filter = fact->create();
        std::cout << "| " << filter->getNameOfClass().toStdString() << " | " << filter->getCompiledLibraryName().toStdString() << " |"<< std::endl;
      }
#endif
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
      // std::cout << "PIPELINE FILE: " << itemFilePath.toStdString() << std::endl;
      extractFilters(itemFilePath);
    }
  }

  /**
   * @brief adjustMarkdownFile
   * @param docPath
   * @param pipelines
   */
  bool adjustMarkdownFile(const QString& hFile, QSet<QString>& pipelines)
  {
    QString contents;
    QFileInfo fi(hFile);
    {
      // Read the Source File
      if(fi.suffix().compare("md") != 0)
      {
        return false;
      }

      QFile source(hFile);
      source.open(QFile::ReadOnly);
      contents = source.readAll();
      source.close();
    }

    QStringList names;
    bool didReplace = false;

    QString searchString = "## Example Pipelines ##";
    QString replaceString = "";
    QStringList outLines;
    QStringList list = contents.split(QRegExp("\\n"));
    QStringListIterator sourceLines(list);
    QString body;

    int index = 0;
    while(sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(searchString))
      {
        outLines.push_back(searchString);
        outLines.push_back("");
        for(auto pipeline : pipelines)
        {
          outLines.push_back(QString("+ %1").arg(pipeline));
        }
        didReplace = true;
        line = sourceLines.next();
        line = sourceLines.next();
      }
      else
      {
        outLines.push_back(line);
      }
    }

    writeOutput(didReplace, outLines, hFile);
    index++;
    return didReplace;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void writeOutput(bool didReplace, QStringList& outLines, QString filename)
  {
    if(didReplace == true)
    {
      QFileInfo fi2(filename);
#if OVERWRITE_SOURCE_FILE
      QFile hOut(filename);
#else
      QString tmpPath = "/tmp/" + fi2.fileName();
      QFile hOut(tmpPath);
#endif
      hOut.open(QFile::WriteOnly);
      QTextStream stream(&hOut);
      for(qint32 i = 0; i < outLines.size() - 1; i++)
      {
        stream << outLines[i] << "\n";
      }
      hOut.close();

      qDebug() << "Saved File " << fi2.absoluteFilePath();
    }
  }

  /**
   * @brief extractFilters
   * @param pipelineFilePath
   */
  void extractFilters(const QString& pipelineFile)
  {
    // FilterManager* fm = FilterManager::Instance();

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
    // std::cout << "  Filter Count: " << filters.size() << std::endl;
    for(auto filter : filters)
    {
      QString filterClassName = filter->getNameOfClass();
      m_FilterFactories.remove(filterClassName);

      if(m_UsedFilters.find(filterClassName) == m_UsedFilters.end())
      {
        QSet<QString> theSet = {pipeline->getName()};
        m_UsedFilters.insert(filterClassName, theSet);
      }
      else
      {
        QSet<QString> pipelineNameList = m_UsedFilters[filterClassName];
        pipelineNameList.insert(pipeline->getName());
        m_UsedFilters[filterClassName] = pipelineNameList;
      }
    }
  }

private:
  FilterManager::Collection m_FilterFactories;
  QMap<QString, QSet<QString>> m_UsedFilters;
  QJsonDocument m_JsonDoc;

  AnalyzePrebuiltPipelines(const AnalyzePrebuiltPipelines&) = delete; // Copy Constructor Not Implemented
  void operator=(const AnalyzePrebuiltPipelines&) = delete;           // Move assignment Not Implemented
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
