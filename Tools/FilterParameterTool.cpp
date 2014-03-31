/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <iostream>


#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMetaProperty>

// DREAM3DLib includes
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DREAM3DVersion.h"


#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "DREAM3DLib/Plugin/DREAM3DPluginInterface.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString quote(const QString& str)
{
  return QString("\"%1\"").arg(str);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool fixFile(FilterParameter::Pointer parameter, const QString &replacement, const QString &cppFile)
{
  // Read the Source File
  QFileInfo fi(cppFile);
  QFile source(cppFile);
  source.open(QFile::ReadOnly);
  QString cpp = source.readAll();
  source.close();

  std::cout << "Filter: " << fi.baseName().toStdString() << "   Parameter: " << parameter->getHumanLabel().toStdString() << std::endl;
//  if (fi.baseName().compare("EBSDSegmentFeatures") != 0)
//  {
//    return false;
//  }

  QStringList list;

  bool didReplace = false;

  QString pType = parameter->getValueType();

  QString pSearch = "FilterParameterVector parameters;";

  QString cSearch = "FilterParameter::Pointer parameter = FilterParameter::New();";
  QString searchString2 = "parameter->setHumanLabel(" + quote(parameter->getHumanLabel()) + ");";

  QString buffer;
  QTextStream ss(&buffer);

  //  QString headerStr;
  QStringList header;


  list = cpp.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();

    if(line.contains(searchString2) ) // we found the filter parameter section
    {
      line = sourceLines.previous();
      // Look for our specific human label, and then walk back up the lines until we find the constructor
      while(sourceLines.hasPrevious())
      {
        line = sourceLines.previous();
        header.pop_back();
        if(line.contains("{") == true)
        {
          break;
        }
      }

      // write in our Replacement code
      std::cout << "   Replacing FilterParameter Code" << std::endl;
      header << replacement; // This is the humanLabel we are looking for so use the replacement instead
      didReplace = true;
      // Walk until we find the } character which should be the end of the scoped section
      while(sourceLines.hasNext() )
      {
        line = sourceLines.next();
        if(line.contains("}") == true) { break; } // Keep looping and eating lines until we find the "}" character.
      }
    }
    else
    {
      header.push_back(line);
      //if(sourceLines.hasNext() == true) { ref << "\n"; }
    }
  }

  if(didReplace == true)
  {
    QFileInfo fi2(cppFile);
#if 1
    QFile hOut(cppFile);
#else
    QString tmpPath = "/tmp/" + fi2.fileName();
    QFile hOut(tmpPath);
#endif
    hOut.open(QFile::WriteOnly);
    QTextStream stream( &hOut );
    stream << header.join("\n");
    hOut.close();

    qDebug() << "Saved File " << fi2.absoluteFilePath();
  }
  return didReplace;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void appendSignal(const QString& cppFile)
{
  // Read the Source File
  QFileInfo fi(cppFile);
  QFile source(cppFile);
  source.open(QFile::ReadOnly);
  QString cpp = source.readAll();
  source.close();

  QStringList list;

  QString searchString = "protected:";


  QString headerStr;
  QTextStream header(&headerStr);

  list = cpp.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();

    if(line.contains(searchString) )
    {
      header << "  signals:\n    void parametersChanged();\n\n";
    }
    if (sourceLines.hasNext() ) { header << line << "\n"; }
  }

  QFileInfo fi2(cppFile);
#if 0
  QFile hOut(cppFile);
#else
  QString tmpPath = "/tmp/" + fi2.fileName();
  QFile hOut(tmpPath);
#endif
  hOut.open(QFile::WriteOnly);
  QTextStream stream( &hOut );
  stream << headerStr;
  hOut.close();

  qDebug() << "Saved File " << fi2.absoluteFilePath();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void generateQProperties(AbstractFilter::Pointer filter, const QString &path)
{
  QString out;
  QTextStream ss(&out);
  bool addSignal = false;
  ss << "  /* ** These are the Q_Properties for each of the input parameters. **  */\n";

  QVector<FilterParameter::Pointer> parameters = filter->getFilterParameters();

  for(int i = 0; i < parameters.size(); i++)
  {
    out = ""; // clear the string
    QString pType = parameters[i]->getValueType();
    QString pName = parameters[i]->getPropertyName();

    ss << "    Q_PROPERTY(" << pType << " " << pName << " READ get" << pName << " WRITE set" << pName << " NOTIFY parametersChanged)\n";

    addSignal = fixFile(parameters[i], out, path);
  }

  if(addSignal == true)
  {
    appendSignal(path);
  }

  std::cout << "Done" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString findPath(const QString& groupName, const QString& filtName, const QString ext)
{
  QString prefix("/Users/mjackson/Workspace/DREAM3D_Rewrite/Source/");
  {
    QString path = prefix + "DREAM3DLib/" + groupName + "Filters/" + filtName + ext;
    QFileInfo fi(path);
    if(fi.exists() == true)
    {
      return path;
    }
  }

  prefix = prefix + "Plugins/";
  QStringList libs;
  libs << "ImageImport" << "OrientationAnalysis" << "Processing" <<  "Reconstruction" << "Sampling" << "Statistics"  << "SurfaceMeshing" << "SyntheticBuilding";

  for (int i = 0; i < libs.size(); ++i)
  {
    QString path = prefix + libs.at(i) + "/" + libs.at(i) + "Filters/" + filtName + ext;
    //  std::cout << "****" << path.toStdString() << std::endl;

    QFileInfo fi(path);
    if(fi.exists() == true)
    {
      return path;
    }
  }
  return "NOT FOUND";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFilterParametersCode()
{
  qDebug() << "-------------- GenerateFilterParametersCode ------------------------------";

  FilterManager::Pointer fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories();
  QMapIterator<QString, IFilterFactory::Pointer> iter(factories);
  while(iter.hasNext())
  {
    iter.next();
    IFilterFactory::Pointer factory = iter.value();
    AbstractFilter::Pointer filter = factory->create();

    QString cpp = findPath(filter->getGroupName(), filter->getNameOfClass(), ".cpp");
    //std::cout << filter << " " << cpp.toStdString() << std::endl;

    const QMetaObject* meta = filter->metaObject();
    std::string cn = filter->getNameOfClass().toStdString();

    QStringList properties;
    for(int i = meta->propertyOffset(); i < meta->propertyCount(); ++i)
    {
      properties << QString::fromLatin1(meta->property(i).name());
    }
    QVector<FilterParameter::Pointer> options = filter->getFilterParameters();
    for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
    {
      std::stringstream ss;
      ss << "  parameters.push_back(FilterParameter::New(";
      FilterParameter* option = (*iter).get();
      if (option->getNameOfClass().compare("FilterParameter") != 0) {
        ss << "NOT UPDATING FILTER PARAMETER " << option->getNameOfClass().toStdString() << std::endl;
        continue;
      }

      ss << quote(option->getHumanLabel()).toStdString() << ", ";
      ss << quote(option->getPropertyName()).toStdString() << ", ";
      ss << "FilterParameterWidgetType::" << option->getWidgetType().toStdString() << ",";
      ss << quote(option->getValueType()).toStdString() << ", ";
      if(option->getAdvanced()) {
        ss << "true";
      } else {
        ss << "false";
      }

      QString units = option->getUnits();
      QString fileExt = option->getFileExtension();
      QString fType = option->getFileType();

      if (units.isEmpty() && fileExt.isEmpty() && fType.isEmpty() )
      {
        //     std::cout << "));" << std::endl;
      }
      else if(fType.isEmpty() == false)
      {
        ss << ", ";
        ss << quote(units).toStdString() << ", ";
        ss << quote(fileExt).toStdString() << ", ";
        ss << quote(fType).toStdString();
      }
      else if (fileExt.isEmpty() == false)
      {
        ss << ", ";
        ss << quote(units).toStdString() << ", ";
        ss << quote(fileExt).toStdString();
      }
      else if(units.isEmpty() == false)
      {
        ss << ", ";
        ss << quote(units).toStdString();
      }
      ss << "));";

      fixFile(*iter, QString::fromStdString(ss.str()), cpp);
    }
  }

}





// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoopOnFilters()
{
  FilterManager::Pointer fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories();

  FilterManager::CollectionIterator i(factories);
  int count = 0;
  while (i.hasNext())
  {
    i.next();
    std::cout << ++count << ": " << i.key().toStdString() << ": " << std::endl;

    //std::cout << "  public:" << std::endl;
    IFilterFactory::Pointer factory = i.value();
    AbstractFilter::Pointer filter = factory->create();
    //if (filter->getGroupName().compare(DREAM3D::FilterGroups::StatisticsFilters) == 0)
    // if(filter->getNameOfClass().compare("FindSchmids") == 0)
    {
      //   std::cout << "" << filter->getGroupName().toStdString() << "Filters/" << filter->getNameOfClass().toStdString() << ".cpp" << std::endl;
      QString cpp = findPath(filter->getGroupName(), filter->getNameOfClass(), ".cpp");
      std::cout << filter << " " << cpp.toStdString() << std::endl;
    }

  }

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Q_ASSERT(false); // We don't want anyone to run this program.
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("FilterParameterTool");

  std::cout << "FilterParameterTool Starting. Version " << DREAM3DLib::Version::PackageComplete().toStdString() << std::endl;


  // Register all the filters including trying to load those from Plugins
  FilterManager::Pointer fm = FilterManager::Instance();
  DREAM3DPluginLoader::LoadPluginFilters(fm.get());


  // Send progress messages from PipelineBuilder to this object for display
  qRegisterMetaType<PipelineMessage>();

  GenerateFilterParametersCode();

  return 0;
}
