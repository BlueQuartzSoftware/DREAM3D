
// C Includes
#include <stdlib.h>
#include <assert.h>

// C++ Includes
#include <iostream>

// TCLAP Includes
#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

// Qt Includes
#include <QtCore/QtDebug>
#include <QtCore/QCoreApplication>
#include <QtCore/QPluginLoader>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSettings>




#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "DREAM3DFileStructure.h"

int main(int argc, char* argv[])
{

  QString filePath;
  try
  {
    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("DREAM3DFileStructure", ' ', SIMPLib::Version::Complete().toStdString());

    TCLAP::ValueArg<std::string> fileArg( "d", "file", "DREAM3D File", true, "", "DREAM3D Input File (*.dream3d)");
    cmd.add(fileArg);

    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      qDebug() << "DREAM3DFileStructure program was not provided any arguments. Use the --help argument to show the help listing.";
      return EXIT_FAILURE;
    }
    // Extract the file path passed in by the user.
    filePath = QString::fromStdString(fileArg.getValue());
  }
  catch (TCLAP::ArgException& e) // catch any exceptions
  {
    std::cerr << " error: " << e.error() << " for arg " << e.argId();
    return EXIT_FAILURE;
  }

// Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("DREAM3DFileStructure");

  std::cout << "DREAM3DFileStructure Starting. Version " << SIMPLib::Version::PackageComplete().toStdString() << std::endl;

  // Sanity Check the filepath to make sure it exists, Report an error and bail if it does not
  QFileInfo fi(filePath);
  if(fi.exists() == false)
  {
    std::cout << "The input file '" << filePath.toStdString() << "' does not exist" << std::endl;
    return EXIT_FAILURE;
  }



  DataContainerArray::Pointer dcArray = DREAM3DFileStructure::ReadFileStructure(filePath);


  return 0;
}
