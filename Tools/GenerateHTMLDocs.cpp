#include <iostream>
#include <sstream>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMetaProperty>
#include <QtCore/QString>
#include <QtCore/QTextStream>

#include "discount/mkdio.h"

#include "DREAM3DToolsConfiguration.h"



#define OVERWRITE_SOURCE_FILE 1


static bool CssLoaded = false;
static QString Css;

static std::string IndexMarkDown;
static std::stringstream indexStream;

static int depth = 0;

QStringList dirPaths;

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
      source.open(QFile::ReadOnly);
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
void writeHtmlHeader(QTextStream& stream, const QString &title)
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
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeHtmlFooter(QTextStream &stream)
{
  stream <<  "</body>\n</html>";
  
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenerateHTML(const QFileInfo& fi)
{
  
  QString CssFilePath = D3DTools::GetDREAM3DProjDir();
  QString markDown;
  {
    // Read the markdown file
    QFile source(fi.absoluteFilePath());
    source.open(QFile::ReadOnly);
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
  mkd_flag_t flag = MKD_1_COMPAT;
  MMIOT* doc = mkd_string(text.c_str(), text.size(), flag);
  if(!mkd_compile(doc, flag))
  {
    std::cerr << "Markdown Compilation Error!" << std::endl;
    mkd_cleanup(doc);
    return -1;
  }
  char* res = nullptr;
  int size = mkd_document(doc, &res);
  //  if(size)
  //  {
  //    std::string html(res, res + size);
  //    std::cout << "HTML: " << html << std::endl;
  //  }
  
  QString sourcePath = fi.absoluteFilePath();
  QString binDirPrefix = getDocSourceDir();
  
  if( !sourcePath.contains(binDirPrefix) )
  {
    mkd_cleanup(doc);
    std::cout << "Source file " << fi.absoluteFilePath().toStdString() << " is outside the binary directory. That is not right" << std::endl;
    return -2;
  }
  
  QString destPath = D3DTools::GetDREAM3DGeneratedHelpRoot();
  QString htmlPath = sourcePath.replace(binDirPrefix, destPath);
  
  htmlPath = htmlPath.replace(".md", ".html");
  
  
  QFileInfo fi2(htmlPath);
  
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
  QFile hOut(htmlPath);
#else
  QString tmpPath = "/tmp/" + fi2.fileName();
  QFile hOut(tmpPath);
#endif
  hOut.open(QFile::WriteOnly);
  QTextStream stream(&hOut);
  
  // Write the HTML Header
  writeHtmlHeader(stream, firstLine);
  
  // Write the generated HTML 
  QString html(res);
  stream << html;
  
  // Write the HTML footer
  writeHtmlFooter(stream);
  
  hOut.close();
  
  // std::cout<< "Saved File " << fi2.absoluteFilePath().toStdString() << std::endl;
  
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
      GenerateHTML(itemInfo);
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
      source.open(QFile::ReadOnly);
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
      source.open(QFile::ReadOnly);
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
    
    GenerateHTML(fi);
    
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
  
  ReplaceGrepSearchesRecursively(getDocSourceDir());
  
  GenerateIndexFile();
  
  return 0;
}
