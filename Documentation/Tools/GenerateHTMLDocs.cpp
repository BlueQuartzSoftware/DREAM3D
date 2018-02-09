#include <iostream>
#include <sstream>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMetaProperty>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Plugin/PluginManager.h"


#include "SVWidgetsLib/SVWidgetsLib.h"

#ifdef SIMPL_USE_DISCOUNT
#include "discount/mkdio.h"
#endif


#include "DocToolsConfiguration.h"

#define OVERWRITE_SOURCE_FILE 1

static int file_count = 0;

static bool CssLoaded = false;
static QString Css;

static std::string IndexMarkDown;
static std::stringstream indexStream;

static int depth = 0;

static QStringList dirPaths;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString getCss()
{
  if(!CssLoaded)
  {
    QString CssFilePath = D3DTools::GetDREAM3DToolsDir() + "/HTMLDocs_Style.css";
    
    {
      // Read the CSS File
      QFileInfo fi(CssFilePath);
      
      QFile source(fi.absoluteFilePath());
      bool isOpen = source.open(QFile::ReadOnly);
      Q_ASSERT(isOpen);
      Css = source.readAll();
      source.close();
    }
    CssLoaded = true;
  }
  return Css;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString getDocSourceDir()
{
  return QString(D3DTools::GetDREAM3DBinaryDir() + "/Documentation/ReferenceManual");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString getNavIndex(const QFileInfo& fi, const QFileInfo& htmlFileInfo)
{
//  std::cout << "-------------------------------------------------" << std::endl;
//  std::cout << htmlFileInfo.absoluteFilePath().toStdString() << std::endl;
  QString markDown;
  PluginManager* pm = PluginManager::Instance();
  QVector<ISIMPLibPlugin*> plugins = pm->getPluginsVector();
  
  
  // This chunk of code basically sorts the plugins in Alphabetical order
  QMap<QString, ISIMPLibPlugin*> plugMap;
  for (auto plug : plugins)
  {
    plugMap[plug->getPluginBaseName()] = plug;
  }

  QString plugMarkDown;
  QTextStream plugMdOStream(&plugMarkDown);
  QDir indexDir(htmlFileInfo.absolutePath());
  
  //============================================================================
  
  QString docBinDir = D3DTools::GetDREAM3DBinaryRefManualDir();
  QString jsonFilePath = docBinDir + QDir::separator() + "PluginIndex.json";
  QFileInfo jsonFileInfo(jsonFilePath);
  
  if(!jsonFileInfo.exists())
  {
    std::cout << "Input Json file not found at " << jsonFilePath.toStdString() << std::endl;
    return QString("");
  }
  
  QByteArray json;
  {
    // Read the json file
    QFile source(jsonFileInfo.absoluteFilePath());
    bool isOpen = source.open(QFile::ReadOnly);
    Q_ASSERT(isOpen);
    json = source.readAll();
    source.close();
  }
  
  QJsonParseError parseError;
  QJsonDocument jsonDoc = QJsonDocument::fromJson(json, &parseError);
  if(parseError.error != QJsonParseError::NoError)
  {
    std::cout << "JSON Parse Error: " << parseError.errorString().toStdString() << std::endl;
    return QString("");
  }
  QJsonObject jsonDocObject = jsonDoc.object();
  /* ***************************************************************************
   * This section converts all the ReferenceManual MarkDown files using Discount
   * into HTML and also copies any "Images" folder.
   ****************************************************************************/
  {
    QJsonObject refManualObj = jsonDocObject["ReferenceManual"].toObject();
    QStringList refKeys = refManualObj.keys();
    QStringListIterator refKeyIter(refKeys);
    while(refKeyIter.hasNext())
    {
      QString refKey = refKeyIter.next();
      QJsonObject docGroupObj = refManualObj[refKey].toObject();
      
      QString sourceDir = docGroupObj["SOURCE_DIR"].toString("NOT_FOUND");
      QString docDir = docGroupObj["DOC_DIR"].toString("EMPTY");
      QJsonArray filters = docGroupObj["FILTERS"].toArray();
      
      QString header = QString(2, '#');
      plugMdOStream << "\n###### " << refKey << " ######\n\n";

      for(QJsonArray::Iterator iter = filters.begin(); iter != filters.end(); iter++)
      {
                
        QString docPath = QString("%1/%2/%3.md").arg(sourceDir).arg(docDir).arg( (*iter).toString());
        //std::cout << "DOC PATH: " << docPath.toStdString() << std::endl;
        QString outPath = QString("%1/%2/%3.html").arg(D3DTools::GetDREAM3DGeneratedHelpRoot()).arg(docDir).arg((*iter).toString());
        //std::cout << "OUT PATH: " << outPath.toStdString() << std::endl;
        QFileInfo docFileInfo(docPath);
        QFileInfo outFileInfo(outPath);
        
        QString relPath;
        QTextStream relPathStream(&relPath);
        relPathStream << outPath;
        relPath = indexDir.relativeFilePath(relPath);      
        plugMdOStream << "+ [" << (*iter).toString() << "](" << relPath << ")" << "\n";
      }  
    }
  }
  //============================================================================
  
  // This code loops through all the plugins and extracts all the filters

  FilterManager* fm = FilterManager::Instance();
  
  QMapIterator<QString, ISIMPLibPlugin*> iter(plugMap);
  while(iter.hasNext())
  {
    iter.next();
    QString plugName = iter.key();
    ISIMPLibPlugin* plug = iter.value();
    QStringList filters;
    plugMdOStream << "\n###### " << plug->getPluginDisplayName() << " ######\n\n";
    filters = plug->getFilters();
    
    for(auto filter: filters)
    {
      IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filter);
      QString humanName = filterFactory->getFilterHumanLabel();
      QString className = filterFactory->getFilterClassName();
      
      QString relPath;
      QTextStream relPathStream(&relPath);
      relPathStream << D3DTools::GetDREAM3DGeneratedHelpRoot() << "/" << "Plugins/" << plug->getPluginBaseName() << "/" << className << ".html";
      relPath = indexDir.relativeFilePath(relPath);      
      plugMdOStream << "+ [" << humanName << "](" << relPath << ")" << "\n";
    }
  }
  markDown = markDown + plugMarkDown;
  
  std::string text = markDown.toStdString();
  mkd_flag_t flag = MKD_1_COMPAT | MKD_TOC;
  MMIOT* doc = mkd_string(text.c_str(), text.size(), flag);
  if(!mkd_compile(doc, flag))
  {
    std::cerr << "Markdown Compilation Error!" << std::endl;
    mkd_cleanup(doc);
    return QString("<b>Error Generating Navigation index</b>");
  }
  char* res = nullptr;
  int size = mkd_document(doc, &res);
  QString html = QString::fromLatin1(res, size);
  mkd_cleanup(doc);
  return html;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeHtmlHeader(QTextStream& stream, const QString &title, const QFileInfo& fi, const QFileInfo& htmlFileInfo)
{
  stream << 
            
            "<!DOCTYPE html>\n\
            <html>\n\
            <head>\n\
            <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n\
      <title>" << title << "</title>\n\
      <meta name=\"generator\" content=\"discount\" />\n\
      <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
      \
      <style>\n\
      " <<
      getCss()
      << "\
         </style>\n\
         </head>\n\
         <body class=\"markdown haroopad\">\n";
  
  stream << "<div id=\"content\">" << "\n";
  stream << "<div id=\"left\">" << "\n";
//  stream << "<!--\n";
  stream << getNavIndex(fi, htmlFileInfo);
//  stream << "-->\n";
  stream << "</div>" << "\n";
  stream << "<div id=\"right\" >" << "\n";
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeHtmlFooter(QTextStream &stream)
{
  stream << "</div>" << "\n";
  stream << "<div id=\"cleared\"></div>" << "\n";
  stream << "</div>" << "\n";
  stream << "</body>\n</html>";
  
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenerateHTML(const QFileInfo& fi, const QFileInfo& htmlFileInfo)
{

  //    if (fi.baseName().compare("ArrayCalculator") != 0)
  //    {
  //      return false;
  //    }

  //  qDebug() << fi.absoluteFilePath();

  QString CssFilePath = D3DTools::GetDREAM3DProjDir();
  QString markDown;
  {
    // Read the markdown file
    QFile source(fi.absoluteFilePath());
    bool isOpen = source.open(QFile::ReadOnly);
    Q_ASSERT(isOpen);
    markDown = source.readAll();
    source.close();
  }

  // Extract the title of the file from the MarkDown which should be the first line
  QString searchString = QString("{#%1}").arg(fi.baseName());
  searchString = searchString.toLower();
  markDown = markDown.replace(searchString, "");
  
  // Remove the LaTeX image commands
  QStringList list = markDown.split(QRegExp("\\n"));
  QString firstLine = list.first();
  QStringListIterator sourceLines(list);
  QStringList outLines;
  searchString = QString("@image latex");
  while(sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    if(!line.contains(searchString))
    {
      outLines.push_back(line);
    }
  }
  markDown = outLines.join("\n");
  
  std::string text = markDown.toStdString();
  mkd_flag_t flag = MKD_1_COMPAT | MKD_TOC;

  // Sanity check for any characters that are out of the US ASCII printable range
  // and if they are replace with a ' ' [SPACE] character. Discount does not like
  // these out of range characters.
  for (std::string::size_type i = 0; i < text.size(); i++)
  {
    if(text[i] > 126 || text[i] < 0)
    {
      text[i] = ' ';
    }
  }
  
  {
    QFile hOut(fi.absoluteFilePath());
    hOut.open(QFile::WriteOnly);
    QTextStream stream(&hOut);
    QString newMD = QString::fromStdString(text);
    stream << newMD;
    hOut.close();
  }
  

  MMIOT* doc = mkd_string(text.c_str(), text.size(), flag);
  if(!mkd_compile(doc, flag))
  {
    std::cerr << "Markdown Compilation Error!" << std::endl;
    mkd_cleanup(doc);
    return -1;
  }
  char* res = nullptr;
  int size = mkd_document(doc, &res);
    if(size)
    {
//      std::string html(res, res + size);
//      std::cout << "HTML: " << html << std::endl;
    }
  

  QFileInfo fi2 = htmlFileInfo;
  
  QString parentPath = fi2.path();
  QDir dir;
  if(!dir.mkpath(parentPath))
  {
    mkd_cleanup(doc);
    
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    std::cout << ss.toStdString() << std::endl;
    return -2;
  }
#if OVERWRITE_SOURCE_FILE
  QString absFilePath = htmlFileInfo.absoluteFilePath();
  if(absFilePath.contains("SIMPLib/"))
  {
    absFilePath = absFilePath.replace("SIMPLib/", "");
  }
  QFile hOut(absFilePath);
#else
  QString tmpPath = "/tmp/" + fi2.fileName();
  QFile hOut(tmpPath);
#endif
  hOut.open(QFile::WriteOnly);
  QTextStream stream(&hOut);
  
  // Write the HTML Header
  writeHtmlHeader(stream, firstLine, fi, htmlFileInfo);
  
  // Write the generated HTML 
  QString html(res);
  stream << html;
  
  // Write the HTML footer
  writeHtmlFooter(stream);
  
  hOut.close();
  
  mkd_cleanup(doc);
  
  std::cout<< "Saved File " << fi2.absoluteFilePath().toStdString() << std::endl;
  file_count++;
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CopyImageFile(const QFileInfo& fi)
{
  QString sourcePath = fi.absoluteFilePath();
  QString binDirPrefix = getDocSourceDir();
  
  if( !sourcePath.contains(binDirPrefix) )
  {
    std::cout << "Image file " << fi.absoluteFilePath().toStdString() << " is outside the binary directory. That is not right" << std::endl;
    return -4;
  }
  
  QString destPath = D3DTools::GetDREAM3DGeneratedHelpRoot();
  QString htmlPath = sourcePath.replace(binDirPrefix, destPath);
  //  std::cout << "Image File Input: " << fi.absoluteFilePath().toStdString() << std::endl;
  //  std::cout << "Image File Output:" << htmlPath.toStdString() << std::endl;
  
  QFileInfo fi2(htmlPath);
  
  QString parentPath = fi2.path();
  QDir dir;
  if(!dir.mkpath(parentPath))
  {  
    QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
    std::cout << ss.toStdString() << std::endl;
    return -6;
  }
  
  
  QFile destImage(htmlPath);
  if(destImage.exists())
  {
    destImage.remove();
  }
  
  bool success = QFile::copy(fi.absoluteFilePath(), htmlPath);
  if(!success)
  {
    std::cout << "File was not copied: " << fi.absoluteFilePath().toStdString() << std::endl;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CopyImageFile(const QFileInfo& fi, const QFileInfo& outFileInfo)
{

  QString absFilePath = outFileInfo.absoluteFilePath();
  if(absFilePath.contains("SIMPLib/"))
  {
    absFilePath = absFilePath.replace("SIMPLib/", "");
  }

  QFile destImage(absFilePath);
  if(destImage.exists())
  {
    destImage.remove();
  }

  bool success = QFile::copy(fi.absoluteFilePath(), absFilePath);
  if(!success)
  {
    std::cout << "File was not copied: " << fi.absoluteFilePath().toStdString() << std::endl;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReplaceGrepSearchesRecursively(QDir currentDir)
{
  depth++;
  QStringList filters;
  filters << "*.md" << "*.png" << "*.tiff";
  
  if(currentDir.dirName().compare("zRel") == 0 || currentDir.dirName().compare("Build") == 0)
  {
    return;
  }
  if(currentDir.dirName().compare("ReferenceManual") != 0)
  {
    dirPaths.push_back(currentDir.dirName());
  }
  
  bool parentWritten = false;
  QFileInfoList itemList = currentDir.entryInfoList(filters);
  foreach(QFileInfo itemInfo, itemList)
  {
    QString itemFilePath = itemInfo.absoluteFilePath();
    
    //std::cout << "-----------------------------------------------" << std::endl;
    if(itemInfo.suffix().compare("md") == 0)
    {
      if(!parentWritten && !currentDir.absolutePath().contains("Plugins") )
      {
        QString header = QString(depth, '#');
        indexStream << header.toStdString() << " " <<  currentDir.dirName().toStdString() << " " << header.toStdString() << "\n\n";
        
        parentWritten = true;
      }
      //std::cout << "HTML: " << itemFilePath.toStdString() << std::endl;
      if(parentWritten) 
      {
        QString dir_path(dirPaths.join("/"));
        if(!dir_path.isEmpty()) { dir_path.append("/");}
        indexStream << "[" << itemInfo.baseName().toStdString() << "](" << dir_path.toStdString() << itemInfo.baseName().toStdString() << ".html" << ")\n";
      }
      GenerateHTML(itemInfo, QFileInfo());
    }
    else
    {
      //std::cout << "IMAGE: " << itemFilePath.toStdString() << std::endl;
      CopyImageFile(itemInfo);
    }
    if(parentWritten)
    {
      indexStream << "\n";
    }
  }
  
  // Get a list of all the directories
  QFileInfoList dirList = currentDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
  if(dirList.size() > 0)
  {
    foreach(QFileInfo fi, dirList)
    {
      
      ReplaceGrepSearchesRecursively(QDir(fi.absoluteFilePath())); // Recursive call
      depth--;
      if(!dirPaths.isEmpty()) { dirPaths.pop_back();}
    }
  }
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIndexFile()
{
  
  std::string text;
  
  {
    QString indexFile = getDocSourceDir() + "/index.md";
    QFileInfo fi(indexFile);
    QString markDown;
    {
      // Read the markdown file
      QFile source(fi.absoluteFilePath());
      bool isOpen = source.open(QFile::ReadOnly);
      Q_ASSERT(isOpen);
      markDown = source.readAll();
      source.close();
    }
    text = markDown.toStdString();
  }
  
  
  text = text + indexStream.str();
  
  {
    QString indexFile = getDocSourceDir() + "/PluginIndex.md";
    QFileInfo fi = QFileInfo(indexFile);
    QString markDown;
    {
      // Read the markdown file
      QFile source(fi.absoluteFilePath());
      bool isOpen = source.open(QFile::ReadOnly);
      Q_ASSERT(isOpen);
      markDown = source.readAll();
      source.close();
    }
    text = text + markDown.toStdString();
  }
  
  {
    QString indexFile = getDocSourceDir() + "/index.md";
    QFileInfo fi(indexFile);
    QString parentPath = fi.path();
    QDir dir;
    if(!dir.mkpath(parentPath))
    {    
      QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
      std::cout << ss.toStdString() << std::endl;
      return;
    }
#if OVERWRITE_SOURCE_FILE
    QFile hOut(indexFile);
#else
    QString tmpPath = "/tmp/" + fi2.fileName();
    QFile hOut(tmpPath);
#endif
    hOut.open(QFile::WriteOnly);
    QTextStream stream(&hOut);
    stream << QString::fromStdString(text);
    hOut.close();
    
    GenerateHTML(fi, QFileInfo());
    
  }
  
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateHtmlFromJson()
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
  QJsonDocument jsonDoc = QJsonDocument::fromJson(json, &parseError);
  if(parseError.error != QJsonParseError::NoError)
  {
    std::cout << "JSON Parse Error: " << parseError.errorString().toStdString() << std::endl;
    return;
  }
  QJsonObject jsonDocObject = jsonDoc.object();
  /* ***************************************************************************
   * This section converts all the ReferenceManual MarkDown files using Discount
   * into HTML and also copies any "Images" folder.
   ****************************************************************************/
  {
    QJsonObject refManualObj = jsonDocObject["ReferenceManual"].toObject();
    QStringList refKeys = refManualObj.keys();
    QStringListIterator refKeyIter(refKeys);
    while(refKeyIter.hasNext())
    {
      QString refKey = refKeyIter.next();
      QJsonObject docGroupObj = refManualObj[refKey].toObject();
      
      QString sourceDir = docGroupObj["SOURCE_DIR"].toString("NOT_FOUND");
      QString docDir = docGroupObj["DOC_DIR"].toString("EMPTY");
      QJsonArray filters = docGroupObj["FILTERS"].toArray();
      
      QString header = QString(2, '#');

      indexStream << "\n" << header.toStdString() << " " <<  refKey.toStdString() << " " << header.toStdString() << "\n\n";

      for(QJsonArray::Iterator iter = filters.begin(); iter != filters.end(); iter++)
      {
      
        indexStream << "+ [" << (*iter).toString().toStdString() << "](" << docDir.toStdString() << "/" << (*iter).toString().toStdString() << ".html" << ")\n";

        QString docPath = QString("%1/%2/%3.md").arg(sourceDir).arg(docDir).arg( (*iter).toString());
        //std::cout << "DOC PATH: " << docPath.toStdString() << std::endl;
        QString outPath = QString("%1/%2/%3.html").arg(D3DTools::GetDREAM3DGeneratedHelpRoot()).arg(docDir).arg((*iter).toString());
        //std::cout << "OUT PATH: " << outPath.toStdString() << std::endl;
        QFileInfo docFileInfo(docPath);
        QFileInfo outFileInfo(outPath);
        GenerateHTML(docFileInfo, outFileInfo);
      }
      // Copy any Images folder
      QString imagesDir = docGroupObj["IMAGES_DIR"].toString("NOT_FOUND");
      
      if(imagesDir.compare("NOT_FOUND") != 0)
      {
        QString imagesDestDirPath = QString("%1/%2").arg(D3DTools::GetDREAM3DGeneratedHelpRoot()).arg(imagesDir);
        if(imagesDestDirPath.contains("SIMPLib/"))
        {
          imagesDestDirPath = imagesDestDirPath.replace("SIMPLib/", "");
        }

        QDir dir;
        if(!dir.mkpath(imagesDestDirPath))
        {  
          QString ss = QObject::tr("Error creating parent path '%1'").arg(imagesDestDirPath);
          std::cout << ss.toStdString() << std::endl;
          continue;
        }
        
        QString imagesSourceDirPath = QString("%1/%2").arg(sourceDir).arg(imagesDir); 
        QStringList filters;
        filters << "*.png" << "*.tiff" << "*.tif" << "*.bmp" << "*.jpg" << "*.jpeg";
        QDir currentDir(imagesSourceDirPath);
        QFileInfoList itemList = currentDir.entryInfoList(filters);
        for(QFileInfoList::Iterator iter = itemList.begin(); iter != itemList.end(); iter++)
        { 
          QString outPath = QString("%1/%2/%3").arg(D3DTools::GetDREAM3DGeneratedHelpRoot()).arg(imagesDir).arg((*iter).fileName());
          CopyImageFile((*iter), outPath);
        }
      }
    }
  }
  /* ***************************************************************************
   * This section converts all the Plugin Documentation MarkDown files using Discount
   * into HTML and also copies any "Images" folder.
   ****************************************************************************/  
  QJsonObject pluginObj = jsonDocObject["Plugins"].toObject();
  QStringList pluginKeys = pluginObj.keys();
  QStringListIterator pluginKeyIter(pluginKeys);
  while(pluginKeyIter.hasNext())
  {
    QString refKey = pluginKeyIter.next();
    QJsonObject docGroupObj = pluginObj[refKey].toObject();
    
    QString sourceDir = docGroupObj["SOURCE_DIR"].toString("NOT_FOUND");
    QString docDir = docGroupObj["DOC_DIR"].toString("EMPTY");
    QString outDocDir = docDir;
    outDocDir.replace("Documentation", "Plugins");
    outDocDir.replace("Filters", "");
    QJsonArray filters = docGroupObj["FILTERS"].toArray();
    for(QJsonArray::Iterator iter = filters.begin(); iter != filters.end(); iter++)
    {
      QString docPath = QString("%1/%2/%3.md").arg(sourceDir).arg(docDir).arg( (*iter).toString());
      //std::cout << "DOC PATH: " << docPath.toStdString() << std::endl;
      QFileInfo docFileInfo(docPath);
      
      QString outPath = QString("%1/%2/%3.html").arg(D3DTools::GetDREAM3DGeneratedHelpRoot()).arg(outDocDir).arg((*iter).toString());
      //std::cout << "OUT PATH: " << outPath.toStdString() << std::endl;
      
      QFileInfo outFileInfo(outPath);
      GenerateHTML(docFileInfo, outFileInfo);
    }
    // Copy any Images folder
    QString imagesDir = docGroupObj["IMAGES_DIR"].toString("NOT_FOUND");
    
    if(imagesDir.compare("NOT_FOUND") != 0)
    {      
      QString imagesSourceDirPath = QString("%1/%2").arg(sourceDir).arg(imagesDir); 
      
      QString imagesDestDirPath = QString("%1/%2").arg(D3DTools::GetDREAM3DGeneratedHelpRoot()).arg(imagesDir.replace("Filters", ""));
      imagesDestDirPath = imagesDestDirPath.replace("Documentation", "Plugins");
      if(imagesDestDirPath.contains("SIMPLib/"))
      {
        imagesDestDirPath = imagesDestDirPath.replace("SIMPLib/", "");
      }

      QDir dir;
      if(!dir.mkpath(imagesDestDirPath))
      {  
        QString ss = QObject::tr("Error creating parent path '%1'").arg(imagesDestDirPath);
        std::cout << ss.toStdString() << std::endl;
        continue;
      }
      
      QStringList filters;
      filters << "*.png" << "*.tiff" << "*.tif" << "*.bmp" << "*.jpg" << "*.jpeg";
      QDir currentDir(imagesSourceDirPath);
      QFileInfoList itemList = currentDir.entryInfoList(filters);
      for(QFileInfoList::Iterator iter = itemList.begin(); iter != itemList.end(); iter++)
      { 
        QString outPath = QString("%1/%2").arg(imagesDestDirPath).arg((*iter).fileName());
        CopyImageFile((*iter), outPath);
      }
    }
  } 
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateIndexFile2()
{
  
  std::string text;
  QString indexFile = D3DTools::GetDREAM3DProjDir() + "/Documentation/ReferenceManual/index.md";
  {
    QFileInfo fi(indexFile);
    QString markDown;
    {
      // Read the markdown file
      QFile source(fi.absoluteFilePath());
      bool isOpen = source.open(QFile::ReadOnly);
      Q_ASSERT(isOpen);
      markDown = source.readAll();
      source.close();
    }
    text = markDown.toStdString();
  }
  text = text + indexStream.str();
  
  #if 0
  #if 0
  {
    QString indexFile = getDocSourceDir() + "/PluginIndex.md";
    QFileInfo fi = QFileInfo(indexFile);
    QString markDown;
    {
      // Read the markdown file
      QFile source(fi.absoluteFilePath());
      source.open(QFile::ReadOnly);
      markDown = source.readAll();
      source.close();
    }
    text = text + markDown.toStdString();
  }
  #else
    PluginManager* pm = PluginManager::Instance();
    QVector<ISIMPLibPlugin*> plugins = pm->getPluginsVector();
    
    
    // This chunk of code basically sorts the plugins in Alphabetical order
    QMap<QString, ISIMPLibPlugin*> plugMap;
    for (auto plug : plugins)
    {
      plugMap[plug->getPluginName()] = plug;
    }
    
    QDir indexDir(indexFile);
    QString plugMarkDown;
    QTextStream plugMdOStream(&plugMarkDown);
    FilterManager* fm = FilterManager::Instance();
    
    QMapIterator<QString, ISIMPLibPlugin*> iter(plugMap);
    while(iter.hasNext())
    {
      iter.next();
      QString plugName = iter.key();
      ISIMPLibPlugin* plug = iter.value();
      QStringList filters;
      plugMdOStream << "\n## " << plug->getPluginName() << " ##\n\n";
      filters = plug->getFilters();
    
      for(auto filter: filters)
      {
        IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filter);
        QString humanName = filterFactory->getFilterHumanLabel();
        QString className = filterFactory->getFilterClassName();
        
        QString relPath;
        QTextStream relPathStream(&relPath);
        relPathStream << indexDir.absolutePath() << "/" << "Plugins/" << plug->getPluginName() << "/" << className << ".html";
        
        relPath = indexDir.relativeFilePath(relPath);
        plugMdOStream << "+ [" << humanName << "](" << relPath << ")" << "\n";
        
        //std::cout << "relPath: " << relPath.toStdString() << std::endl;
      }
    }
    text = text + plugMarkDown.toStdString();
  
  #endif
  #endif
  
  {
    QString indexFile = D3DTools::GetDREAM3DBinaryRefManualDir() + "/index.md";
    QFileInfo fi(indexFile);
    QString parentPath = fi.path();
    QDir dir;
    if(!dir.mkpath(parentPath))
    {    
      QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
      std::cout << ss.toStdString() << std::endl;
      return;
    }
#if OVERWRITE_SOURCE_FILE
    QFile hOut(indexFile);
#else
    QString tmpPath = "/tmp/" + fi2.fileName();
    QFile hOut(tmpPath);
#endif
    hOut.open(QFile::WriteOnly);
    QTextStream stream(&hOut);
    stream << QString::fromStdString(text);
    hOut.close();
    
    GenerateHTML(fi, QFileInfo(D3DTools::GetDREAM3DGeneratedHelpRoot() + "/Index.html"));
    
  }
  
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("GenerateHTMLDocs");

  QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);

  std::cout << "Starting " << QCoreApplication::applicationName().toStdString() << std::endl;
  
#if 0
  
  ReplaceGrepSearchesRecursively(getDocSourceDir());
  
  GenerateIndexFile();
  
#else
    // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm, true);

  QMetaObjectUtilities::RegisterMetaTypes();


  GenerateHtmlFromJson();
  // GenerateIndexFile2();
  
#endif
  std::cout << file_count << " Doc files were generated in " << D3DTools::GetDREAM3DGeneratedHelpRoot().toStdString() <<  std::endl;
  std::cout << QCoreApplication::applicationName().toStdString() << " Complete." << std::endl;

  return 0;
}
