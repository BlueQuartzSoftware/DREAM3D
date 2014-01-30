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
bool fixFile(FilterParameter::Pointer parameter, const QString &props, const QString &cppFile)
{
  // Read the Source File
  QFileInfo fi(cppFile);
  QFile source(cppFile);
  source.open(QFile::ReadOnly);
  QString cpp = source.readAll();
  source.close();

  QStringList list;

  bool didReplace = false;

  QString pType = parameter->getValueType();

  QString searchString = "DREAM3D_INSTANCE_PROPERTY(" + parameter->getValueType() + ", " + parameter->getPropertyName() + ")";
  if(pType.compare("QString") == 0)
  {
    searchString = "DREAM3D_INSTANCE_STRING_PROPERTY("+parameter->getPropertyName() + ")";
  }

  QString headerStr;
  QTextStream header(&headerStr);

  list = cpp.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    if (sourceLines.hasNext() ) {
      if(line.contains(searchString))
      {
        line.replace("DREAM3D_INSTANCE_PROPERTY", "DREAM3D_FILTER_PARAMETER");
        line.replace("DREAM3D_INSTANCE_STRING_PROPERTY(", "DREAM3D_FILTER_PARAMETER(QString, ");
      }
      header << line << "\n";
    }
    if(line.contains(searchString) )
    {
      qDebug() << "Found a DREAM3D Filter Property " << searchString;
      line = sourceLines.next(); // Get the next line, it should start with a Q_PROPERTY
      if(line.contains("Q_PROPERTY") == true ) // It had the Q_PROPERTY line already so just write the existing line
      {
        header << line << "\n";
      }
      else // The Q_PROPERTY was missing so write that, and also the line that was read
      {
        header << props;
        qDebug() << "adding " << props;
        didReplace = true;
        header << line << "\n";
      }
    }
  }

  QFileInfo fi2(cppFile);
#if 1
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
QString findPath(const QString& groupName, const QString& filtName)
{
  QString prefix("/Users/mjackson/Workspace/DREAM3D_Rewrite/Source/");
  {
    QString path = prefix + "DREAM3DLib/" + groupName + "Filters/" + filtName + ".h";
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
    QString path = prefix + libs.at(i) + "/" + libs.at(i) + "Filters/" + filtName + ".h";
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
      std::cout << "" << filter->getGroupName().toStdString() << "Filters/" << filter->getNameOfClass().toStdString() << ".h" << std::endl;
      QString path = findPath(filter->getGroupName(), filter->getNameOfClass());
      std::cout << " " << path.toStdString() << std::endl;
      generateQProperties(filter, path);
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

  LoopOnFilters();

  return 0;
}
