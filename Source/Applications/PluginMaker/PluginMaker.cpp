/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2012 Joseph B. Kleingers (Student Research Assistant)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, Joseph B. Kleingers,
 * the US Air Force, BlueQuartz Software nor the names of its contributors may be
 * used to endorse or promote products derived from this software without specific
 * prior written permission.
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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "PluginMaker.h"
#include <iostream>



#include <QtCore/QtDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtGui/QWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QProgressBar>
#include <QtGui/QMessageBox>
#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>
#include <QtCore/QSize>

#include "DREAM3D/License/PluginMakerLicenseFiles.h"
#include "QtSupport/ApplicationAboutBoxDialog.h"
#include "QtSupport/QFileCompleter.h"


#include "HelpWidget.h"
#include "AddFilterWidget.h"
#include "FilterBundler.h"
#include "PMDirGenerator.h"
#include "PMFileGenerator.h"
#include "PMFilterGenerator.h"
#include "PMGeneratorTreeItem.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PluginMaker::PluginMaker(QWidget* parent) :
  QMainWindow(parent)
{
  m_OpenDialogLastDirectory = QDir::homePath();
  setupUi(this);

  setupGui();

  //Set window to open at the center of the screen
  QDesktopWidget* desktop = QApplication::desktop();

  int screenWidth, width;
  int screenHeight, height;
  int x, y;
  QSize windowSize;

  screenWidth = desktop->width(); // get width of screen
  screenHeight = desktop->height(); // get height of screen

  windowSize = size(); // size of application window
  width = windowSize.width();
  height = windowSize.height();

  x = (screenWidth - width) / 2;
  y = (screenHeight - height) / 2;
  y -= 50;

  // move window to desired coordinates
  move (x, y);

  readSettings();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::setupGui()
{
  QFileCompleter* com = new QFileCompleter(this, true);
  m_OutputDir->setCompleter(com);
  QObject::connect(com, SIGNAL(activated(const QString&)), this, SLOT(on_m_OutputDir_textChanged(const QString&)));


  QString pathTemplate;

  nameLabel->setToolTip("Plugin Name");
  m_PluginName->setToolTip("Enter Plugin Name Here");
  outputFileNameLabel->setToolTip("Output Directory");
  m_OutputDir->setToolTip("Enter Output Directory Here");
  selectButton->setToolTip("Select Directory");
  generateButton->setToolTip("Generate File Structure");


  PMGeneratorTreeItem* F_main = new PMGeneratorTreeItem(treeWidget);
  F_main->setText(0, "Unknown Plugin Name");
  {
    pathTemplate = "@PluginName@";
    QString resourceTemplate("");
    PMDirGenerator* gen = new PMDirGenerator(m_OutputDir->text(),
                                             pathTemplate,
                                             QString(""),
                                             resourceTemplate,
                                             F_main,
                                             this);
    gen->setDisplaySuffix("");
    gen->setDoesGenerateOutput(false);
    gen->setNameChangeable(true);
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            gen, SLOT(pluginNameChanged(const QString&)));
  }

  //// This is for the @PluginName@Constants File Generation
  PMGeneratorTreeItem* pluginConstants = new PMGeneratorTreeItem(F_main);
  pluginConstants->setText(0, "Unknown Plugin Name");
  pathTemplate = "@PluginName@/";
  QString resourceTemplate( generateFileSystemPath("/Template/Constants.h.in") );
  PMFileGenerator* constantsPluginGen = new PMFileGenerator(m_OutputDir->text(),
                                                            pathTemplate,
                                                            QString(""),
                                                            resourceTemplate,
                                                            pluginConstants,
                                                            this);
  pluginConstants->setFileGenPtr(constantsPluginGen);
  constantsPluginGen->setDisplaySuffix("Constants.h");
  constantsPluginGen->setDoesGenerateOutput(true);
  constantsPluginGen->setNameChangeable(true);
  connect(m_PluginName, SIGNAL(textChanged(const QString&)),
          constantsPluginGen, SLOT(pluginNameChanged(const QString&)));
  connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
          constantsPluginGen, SLOT(outputDirChanged(const QString&)));
  // For "Directories" this probably isn't needed
  connect(generateButton, SIGNAL(clicked()),
          constantsPluginGen, SLOT(generateOutput()));
  connect(constantsPluginGen, SIGNAL(outputError(const QString&)),
          this, SLOT(generationError(const QString&)));


  //// This is for the @PluginName@Plugin.cpp File Generation
  PMGeneratorTreeItem* pluginCPP = new PMGeneratorTreeItem(F_main);
  pluginCPP->setText(0, "Unknown Plugin Name");
  pathTemplate = "@PluginName@/";
  resourceTemplate = generateFileSystemPath("/Template/Plugin.cpp.in");
  PMFileGenerator* cppPluginGen = new PMFileGenerator(m_OutputDir->text(),
                                                      pathTemplate,
                                                      QString(""),
                                                      resourceTemplate,
                                                      pluginCPP,
                                                      this);
  pluginCPP->setFileGenPtr(cppPluginGen);
  cppPluginGen->setDisplaySuffix("Plugin.cpp");
  cppPluginGen->setDoesGenerateOutput(true);
  cppPluginGen->setNameChangeable(true);
  connect(m_PluginName, SIGNAL(textChanged(const QString&)),
          cppPluginGen, SLOT(pluginNameChanged(const QString&)));
  connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
          cppPluginGen, SLOT(outputDirChanged(const QString&)));
  // For "Directories" this probably isn't needed
  connect(generateButton, SIGNAL(clicked()),
          cppPluginGen, SLOT(generateOutput()));
  connect(cppPluginGen, SIGNAL(outputError(const QString&)),
          this, SLOT(generationError(const QString&)));


  //// This is for the @PluginName@Plugin.h File Generation
  PMGeneratorTreeItem* pluginH = new PMGeneratorTreeItem(F_main);
  pluginH->setText(0, "Unknown Plugin Name");
  pathTemplate = "@PluginName@/";
  resourceTemplate = generateFileSystemPath("/Template/Plugin.h.in");
  PMFileGenerator* hPluginGen = new PMFileGenerator(m_OutputDir->text(),
                                                    pathTemplate,
                                                    QString(""),
                                                    resourceTemplate,
                                                    pluginH,
                                                    this);
  pluginH->setFileGenPtr(hPluginGen);
  hPluginGen->setDisplaySuffix("Plugin.h");
  hPluginGen->setDoesGenerateOutput(true);
  hPluginGen->setNameChangeable(true);
  connect(m_PluginName, SIGNAL(textChanged(const QString&)),
          hPluginGen, SLOT(pluginNameChanged(const QString&)));
  connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
          hPluginGen, SLOT(outputDirChanged(const QString&)));
  // For "Directories" this probably isn't needed
  connect(generateButton, SIGNAL(clicked()),
          hPluginGen, SLOT(generateOutput()));
  connect(hPluginGen, SIGNAL(outputError(const QString&)),
          this, SLOT(generationError(const QString&)));


  F_res = new PMGeneratorTreeItem(F_main);
  F_res->setText(0, tr("Resources"));

  PMGeneratorTreeItem* resources_sourceList = new PMGeneratorTreeItem(F_res);
  resources_sourceList->setText(0, tr("SourceList.cmake"));
  {
    pathTemplate = "@PluginName@/Resources/";

    QString resourceTemplate( generateFileSystemPath("/Template/Resources/SourceList.cmake.in") );
    PMFileGenerator* gen = new PMFileGenerator(m_OutputDir->text(),
                                               pathTemplate,
                                               QString("SourceList.cmake"),
                                               resourceTemplate,
                                               resources_sourceList,
                                               this);

    resources_sourceList->setFileGenPtr(gen);
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            gen, SLOT(pluginNameChanged(const QString&)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
            gen, SLOT(outputDirChanged(const QString&)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            gen, SLOT(generateOutput()));
    connect(gen, SIGNAL(outputError(const QString&)),
            this, SLOT(generationError(const QString&)));
    gen->setDoesGenerateOutput(true);
  }

  PMGeneratorTreeItem* resources_qrc = new PMGeneratorTreeItem(F_res);
  resources_qrc->setText(0, tr("Unknown Plugin Name"));
  {
    pathTemplate = "@PluginName@/Resources/";

    QString resourceTemplate( generateFileSystemPath("/Template/Resources/Filter.qrc.in") );
    PMFileGenerator* gen = new PMFileGenerator(m_OutputDir->text(),
                                               pathTemplate,
                                               QString(""),
                                               resourceTemplate,
                                               resources_qrc,
                                               this);

    resources_qrc->setFileGenPtr(gen);
    gen->setNameChangeable(true);
    gen->setDisplaySuffix(".qrc");
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            gen, SLOT(pluginNameChanged(const QString&)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
            gen, SLOT(outputDirChanged(const QString&)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            gen, SLOT(generateOutput()));
    connect(gen, SIGNAL(outputError(const QString&)),
            this, SLOT(generationError(const QString&)));
    gen->setDoesGenerateOutput(true);
  }

  F_res_sub = new PMGeneratorTreeItem(F_res);
  F_res_sub->setText(0, tr("Unknown Plugin Name"));
  {
    pathTemplate = "@PluginName@/Resources/";
    QString resourceTemplate("");
    PMDirGenerator* gen = new PMDirGenerator(m_OutputDir->text(),
                                             pathTemplate,
                                             QString(""),
                                             resourceTemplate,
                                             F_res_sub,
                                             this);
    gen->setDoesGenerateOutput(false);
    gen->setNameChangeable(true);
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            gen, SLOT(pluginNameChanged(const QString&)));
  }

  PMGeneratorTreeItem* resources_license = new PMGeneratorTreeItem(F_res_sub);
  resources_license->setText(0, tr("Unknown Plugin Name"));
  {
    pathTemplate = "@PluginName@/Resources/@PluginName@/";

    QString resourceTemplate( generateFileSystemPath("/Template/Resources/FilterLicense.txt.in") );
    PMFileGenerator* gen = new PMFileGenerator(m_OutputDir->text(),
                                               pathTemplate,
                                               QString(""),
                                               resourceTemplate,
                                               resources_license,
                                               this);

    resources_license->setFileGenPtr(gen);
    gen->setNameChangeable(true);
    gen->setDoesGenerateOutput(true);
    gen->setDisplaySuffix("License.txt");
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            gen, SLOT(pluginNameChanged(const QString&)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
            gen, SLOT(outputDirChanged(const QString&)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            gen, SLOT(generateOutput()));
    connect(gen, SIGNAL(outputError(const QString&)),
            this, SLOT(generationError(const QString&)));
    gen->setDoesGenerateOutput(true);
  }

  PMGeneratorTreeItem* resources_description = new PMGeneratorTreeItem(F_res_sub);
  resources_description->setText(0, tr("Unknown Plugin Name"));
  {
    pathTemplate = "@PluginName@/Resources/@PluginName@/";

    QString resourceTemplate( generateFileSystemPath("/Template/Resources/FilterDescription.txt.in") );
    PMFileGenerator* gen = new PMFileGenerator(m_OutputDir->text(),
                                               pathTemplate,
                                               QString(""),
                                               resourceTemplate,
                                               resources_description,
                                               this);

    resources_description->setFileGenPtr(gen);
    gen->setNameChangeable(true);
    gen->setDoesGenerateOutput(true);
    gen->setDisplaySuffix("Description.txt");
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            gen, SLOT(pluginNameChanged(const QString&)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
            gen, SLOT(outputDirChanged(const QString&)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            gen, SLOT(generateOutput()));
    connect(gen, SIGNAL(outputError(const QString&)),
            this, SLOT(generationError(const QString&)));
    gen->setDoesGenerateOutput(true);
  }


  F_doc = new PMGeneratorTreeItem(F_main);
  F_doc->setText(0, tr("Documentation"));

  PMGeneratorTreeItem* cmake = new PMGeneratorTreeItem(F_main);
  cmake->setText(0, tr("CMakeLists.txt"));
  {
    pathTemplate = "@PluginName@";

    QString resourceTemplate( generateFileSystemPath("/Template/CMakeLists.txt.in") );
    PMFileGenerator* gen = new PMFileGenerator(m_OutputDir->text(),
                                               pathTemplate,
                                               QString("CMakeLists.txt"),
                                               resourceTemplate,
                                               cmake,
                                               this);

    cmake->setFileGenPtr(gen);
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            gen, SLOT(pluginNameChanged(const QString&)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
            gen, SLOT(outputDirChanged(const QString&)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            gen, SLOT(generateOutput()));
    connect(gen, SIGNAL(outputError(const QString&)),
            this, SLOT(generationError(const QString&)));
    gen->setDoesGenerateOutput(true);
  }

  // Dummy HTML file (to bundle the plugin CPP and H files in a FilterBundler)
  PMFileGenerator* htmlPluginDoc = new PMFileGenerator("", "", "", "", NULL, this);

  FilterBundler fb(cppPluginGen, hPluginGen, htmlPluginDoc, true);



//// TreeItem for the PluginNameFilters directory
  F_name = new PMGeneratorTreeItem(F_main);
  F_name->setText(0, "Unknown Plugin Name");
  {
    pathTemplate = "@PluginName@/@PluginName@Filters/";
    QString resourceTemplate("");
    PMDirGenerator* gen = new PMDirGenerator(m_OutputDir->text(),
                                             pathTemplate,
                                             QString(""),
                                             resourceTemplate,
                                             F_name,
                                             this);
    gen->setDisplaySuffix("Filters");
    gen->setDoesGenerateOutput(false);
    gen->setNameChangeable(true);
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            gen, SLOT(pluginNameChanged(const QString&)));

  }

  //// TreeItem for a Filter Class source filfe
  PMGeneratorTreeItem* filterCPP = new PMGeneratorTreeItem(F_name);
  filterCPP->setText(0, "Unknown Plugin Name");
  pathTemplate = "@PluginName@/@PluginName@Filters/";
  resourceTemplate = generateFileSystemPath("/Template/Filter/Filter.cpp.in");
  PMFileGenerator* cppFilterGen = new PMFileGenerator(m_OutputDir->text(),
                                                      pathTemplate,
                                                      QString(""),
                                                      resourceTemplate,
                                                      filterCPP,
                                                      this);
  filterCPP->setFileGenPtr(cppFilterGen);
  cppFilterGen->setDisplaySuffix("Filter.cpp");
  cppFilterGen->setDoesGenerateOutput(true);
  cppFilterGen->setNameChangeable(true);
  connect(m_PluginName, SIGNAL(textChanged(const QString&)),
          cppFilterGen, SLOT(pluginNameChanged(const QString&)));
  connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
          cppFilterGen, SLOT(outputDirChanged(const QString&)));
  // For "Directories" this probably isn't needed
  connect(generateButton, SIGNAL(clicked()),
          cppFilterGen, SLOT(generateOutput()));
  connect(cppFilterGen, SIGNAL(outputError(const QString&)),
          this, SLOT(generationError(const QString&)));

  //// TreeItem for a Filter Class Header
  PMGeneratorTreeItem* filterH = new PMGeneratorTreeItem(F_name);
  filterH->setText(0, "Unknown Plugin Name");
  pathTemplate = "@PluginName@/@PluginName@Filters/";
  resourceTemplate = generateFileSystemPath("/Template/Filter/Filter.h.in");
  PMFileGenerator* hFilterGen = new PMFileGenerator(m_OutputDir->text(),
                                                    pathTemplate,
                                                    QString(""),
                                                    resourceTemplate,
                                                    filterH,
                                                    this);
  filterH->setFileGenPtr(hFilterGen);
  hFilterGen->setDisplaySuffix("Filter.h");
  hFilterGen->setDoesGenerateOutput(true);
  hFilterGen->setNameChangeable(true);
  connect(m_PluginName, SIGNAL(textChanged(const QString&)),
          hFilterGen, SLOT(pluginNameChanged(const QString&)));
  connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
          hFilterGen, SLOT(outputDirChanged(const QString&)));
  // For "Directories" this probably isn't needed
  connect(generateButton, SIGNAL(clicked()),
          hFilterGen, SLOT(generateOutput()));
  connect(hFilterGen, SIGNAL(outputError(const QString&)),
          this, SLOT(generationError(const QString&)));

  //// TreeItem for the SourceList.cmake file that will compile all the filters
  PMGeneratorTreeItem* sourceList = new PMGeneratorTreeItem(F_name);
  sourceList->setText(0, tr("SourceList.cmake"));




  //// This adds a "Directory" TreeItem for the FilterParameterWidgets folder
  {
    QTreeWidgetItem* fpw_Name = new PMGeneratorTreeItem(F_main);
    fpw_Name->setText(0, "FilterParameterWidgets");

    pathTemplate = "@PluginName@/FilterParameterWidgets/";
    QString resourceTemplate("");
    PMDirGenerator* gen = new PMDirGenerator(m_OutputDir->text(),
                                             pathTemplate,
                                             QString(""),
                                             resourceTemplate,
                                             fpw_Name,
                                             this);
    gen->setDisplaySuffix("FilterParameterWidgets");
    gen->setDoesGenerateOutput(false);
    gen->setNameChangeable(true);

    /// Add TreeItem for the FilterParameterWidgets/SourceList.cmake file
    PMGeneratorTreeItem* fpw_SourceList = new PMGeneratorTreeItem(fpw_Name);
    fpw_SourceList->setText(0, "SourceList.cmake");
    pathTemplate = "@PluginName@/FilterParameterWidgets/";
    resourceTemplate = generateFileSystemPath("/Template/FilterParameterWidgets/SourceList.cmake.in");
    PMFileGenerator* fpwFilterGen = new PMFileGenerator(m_OutputDir->text(),
                                                        pathTemplate,
                                                        QString("SourceList.cmake"),
                                                        resourceTemplate,
                                                        fpw_SourceList,
                                                        this);
    fpw_SourceList->setFileGenPtr(fpwFilterGen);
    fpwFilterGen->setDisplaySuffix("");
    fpwFilterGen->setDoesGenerateOutput(true);
    fpwFilterGen->setNameChangeable(false);
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            fpwFilterGen, SLOT(pluginNameChanged(const QString&)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
            fpwFilterGen, SLOT(outputDirChanged(const QString&)));
    connect(generateButton, SIGNAL(clicked()),
            fpwFilterGen, SLOT(generateOutput()));
    connect(fpwFilterGen, SIGNAL(outputError(const QString&)),
            this, SLOT(generationError(const QString&)));

  }


  //// This adds a "Directory" TreeItem for the Widgets folder
  {
    QTreeWidgetItem* fpw_Name = new PMGeneratorTreeItem(F_main);
    fpw_Name->setText(0, "Widgets");

    pathTemplate = "@PluginName@/Widgets/";
    QString resourceTemplate("");
    PMDirGenerator* gen = new PMDirGenerator(m_OutputDir->text(),
                                             pathTemplate,
                                             QString(""),
                                             resourceTemplate,
                                             fpw_Name,
                                             this);
    gen->setDisplaySuffix("Widgets");
    gen->setDoesGenerateOutput(false);
    gen->setNameChangeable(true);

    /// Add TreeItem for the FilterParameterWidgets/SourceList.cmake file
    PMGeneratorTreeItem* fpw_SourceList = new PMGeneratorTreeItem(fpw_Name);
    fpw_SourceList->setText(0, "SourceList.cmake");
    pathTemplate = "@PluginName@/Widgets/";
    resourceTemplate = generateFileSystemPath("/Template/Widgets/SourceList.cmake.in");
    PMFileGenerator* fpwFilterGen = new PMFileGenerator(m_OutputDir->text(),
                                                        pathTemplate,
                                                        QString("SourceList.cmake"),
                                                        resourceTemplate,
                                                        fpw_SourceList,
                                                        this);
    fpw_SourceList->setFileGenPtr(fpwFilterGen);
    fpwFilterGen->setDisplaySuffix("");
    fpwFilterGen->setDoesGenerateOutput(true);
    fpwFilterGen->setNameChangeable(false);
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            fpwFilterGen, SLOT(pluginNameChanged(const QString&)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
            fpwFilterGen, SLOT(outputDirChanged(const QString&)));
    connect(generateButton, SIGNAL(clicked()),
            fpwFilterGen, SLOT(generateOutput()));
    connect(fpwFilterGen, SIGNAL(outputError(const QString&)),
            this, SLOT(generationError(const QString&)));

  }


  //// TreeItems for the Plugin Documentation
  F_namefilters = new PMGeneratorTreeItem(F_doc);
  F_namefilters->setText(0, "Unknown Plugin Name");
  {
    pathTemplate = "@PluginName@/Documentation/@PluginName@Filters";
    QString resourceTemplate("");
    PMDirGenerator* gen = new PMDirGenerator(m_OutputDir->text(),
                                             pathTemplate,
                                             QString(""),
                                             resourceTemplate,
                                             F_namefilters,
                                             this);
    gen->setDisplaySuffix("Filters");
    gen->setDoesGenerateOutput(false);
    gen->setNameChangeable(true);
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            gen, SLOT(pluginNameChanged(const QString&)));
  }


  //// TreeItem for the actual filter Documentation
  PMGeneratorTreeItem* filterHTML = new PMGeneratorTreeItem(F_namefilters);
  filterHTML->setText(0, "Unknown Plugin Name");
  pathTemplate = "@PluginName@/Documentation/@PluginName@Filters/";
  resourceTemplate = generateFileSystemPath("/Template/Documentation/Filter/Documentation.md.in");
  PMFileGenerator* htmlFilterDoc = new PMFileGenerator(m_OutputDir->text(),
                                                       pathTemplate,
                                                       QString(""),
                                                       resourceTemplate,
                                                       filterHTML,
                                                       this);
  filterHTML->setFileGenPtr(htmlFilterDoc);
  htmlFilterDoc->setDisplaySuffix("Filter.md");
  htmlFilterDoc->setDoesGenerateOutput(true);
  htmlFilterDoc->setNameChangeable(true);
  connect(m_PluginName, SIGNAL(textChanged(const QString&)),
          htmlFilterDoc, SLOT(pluginNameChanged(const QString&)));
  connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
          htmlFilterDoc, SLOT(outputDirChanged(const QString&)));
  // For "Directories" this probably isn't needed
  connect(generateButton, SIGNAL(clicked()),
          htmlFilterDoc, SLOT(generateOutput()));
  connect(htmlFilterDoc, SIGNAL(outputError(const QString&)),
          this, SLOT(generationError(const QString&)));


  FilterBundler fb2(cppFilterGen, hFilterGen, htmlFilterDoc, true);
  m_FilterBundles.push_back(fb2);

  m_PluginName->setText("Unknown Plugin Name");
  m_PluginName->selectAll();
  treeWidget->expandAll();
  statusbar->showMessage("Ready");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PluginMaker::generateFileSystemPath(QString pathEnding)
{
  QString appPath = qApp->applicationDirPath();

  QDir pluginMakerDir = QDir(appPath);

#if defined(Q_OS_WIN)

#elif defined(Q_OS_MAC)
  if (pluginMakerDir.dirName() == "MacOS")
  {
    pluginMakerDir.cdUp();
    pluginMakerDir.cdUp();
    pluginMakerDir.cdUp();
  }
#else
  // We are on Linux - I think
  QFileInfo fi( pluginMakerDir.absolutePath() + pathEnding);
  if (fi.exists() == false)
  {
    // The help file does not exist at the default location because we are probably running from the build tree.
    // Try up one more directory
    pluginMakerDir.cdUp();
  }
#endif

#if defined(Q_OS_WIN)
  QFileInfo fi( pluginMakerDir.absolutePath() + pathEnding);
  if (fi.exists() == false)
  {
    // The help file does not exist at the default location because we are probably running from visual studio.
    // Try up one more directory
    pluginMakerDir.cdUp();
  }
#endif

  QString filePath = pluginMakerDir.absolutePath() + pathEnding;
  filePath = QDir::toNativeSeparators(filePath);
  return filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_selectButton_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Directory"), outputFile);
  if (!outputFile.isNull())
  {
    m_OutputDir->setText(QDir::toNativeSeparators(outputFile));
    m_OpenDialogLastDirectory = outputFile;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_generateButton_clicked()
{
  QString pluginName = m_PluginName->text();
  QString pluginDir = m_OutputDir->text();

  pluginName = cleanName(pluginName);

  if (pluginName == "")
  {
    statusbar->showMessage("Generation Failed --- Please provide a plugin name");
    QMessageBox::critical(this, tr("PluginMaker"), tr("The file generation was unsuccessful.\n"
                                                      "Please enter a Plugin Name."));
    return;
  }
  else if (pluginDir == "")
  {
    statusbar->showMessage("Generation Failed --- Please provide a plugin directory");
    QMessageBox::critical(this, tr("PluginMaker"), tr("The file generation was unsuccessful.\n"
                                                      "Please enter a Plugin Directory."));
    return;
  }

  // We need to generate the SourceList.cmake file here because we possibly have
  // more than a single filter
  QString text = generateCmakeContents();

  QString pathTemplate = "@PluginName@/@PluginName@Filters/";
  QString parentPath = m_OutputDir->text() + QDir::separator() + pathTemplate.replace("@PluginName@", pluginName);
  parentPath = QDir::toNativeSeparators(parentPath);

  QDir dir(parentPath);
  dir.mkpath(parentPath);

  parentPath = parentPath + QDir::separator() + "SourceList.cmake";
  //Write to file
  QFile f(parentPath);
  if ( f.open(QIODevice::WriteOnly | QIODevice::Text) )
  {
    QTextStream out(&f);
    out << text;
  }


  pathTemplate = "@PluginName@/Documentation/";
  parentPath = m_OutputDir->text() + QDir::separator() + pathTemplate.replace("@PluginName@", pluginName);
  parentPath = QDir::toNativeSeparators(parentPath);

  QDir dir2(parentPath);
  dir2.mkpath(parentPath);

  statusbar->showMessage("Generation Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PluginMaker::cleanName(QString name)
{
  //Remove all uses of "Plugin", "plugin", "Filter", and "filter"
  QRegExp rx("Filter|filter");
  name = name.replace(rx, "");
  //Remove all spaces and illegal characters from plugin name
  name = name.trimmed();
  name = name.remove(" ");
  name = name.remove(QRegExp("[^a-zA-Z_\\d\\s]"));
  return name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PluginMaker::cleanName_filters(QString name)
{
  //Remove all spaces and illegal characters from plugin name
  name = name.trimmed();
  name = name.remove(" ");
  name = name.remove(QRegExp("[^a-zA-Z_\\d\\s]"));
  return name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_m_PluginName_textChanged(const QString& text)
{
  //  QString pluginName = cleanName(m_PluginName->text());

  statusbar->showMessage("Ready");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_m_OutputDir_textChanged(const QString& text)
{
  statusbar->showMessage("Ready");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_actionPlugin_Maker_Help_triggered()
{
  HelpWidget* helpDialog = new HelpWidget(this);
  helpDialog->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_actionAbout_triggered()
{
  ApplicationAboutBoxDialog about(PluginMakerProj::LicenseList, this);
  QString an = QCoreApplication::applicationName();
  QString version("");
  version.append("1.0.0");
  about.setApplicationInfo(an, version);
  about.exec();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::generationError(const QString& test)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_addFilterBtn_clicked()
{
  AddFilterWidget* addFilterDialog = new AddFilterWidget(this);
  addFilterDialog->exec();

  QString filterTitle = addFilterDialog->getFilterName();

  if ( addFilterDialog->getBtnClicked() )
  {
    filterTitle = cleanName_filters(filterTitle);

    /* This simulates the user clicking on the "Add Filter" button */
    PMGeneratorTreeItem* filt2cpp = new PMGeneratorTreeItem(F_name);
    filt2cpp->setText(0, filterTitle + ".cpp");
    QString pathTemplate = "@PluginName@/@PluginName@Filters/";
    QString resourceTemplate = generateFileSystemPath("/Template/Filter/Filter.cpp.in");
    PMFilterGenerator* cppgen = new PMFilterGenerator(m_OutputDir->text(),
                                                      pathTemplate,
                                                      QString(filterTitle + ".cpp"),
                                                      QString(filterTitle),
                                                      resourceTemplate,
                                                      filt2cpp,
                                                      this);
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            cppgen, SLOT(pluginNameChanged(const QString&)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
            cppgen, SLOT(outputDirChanged(const QString&)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            cppgen, SLOT(generateOutput()));
    connect(cppgen, SIGNAL(outputError(const QString&)),
            this, SLOT(generationError(const QString&)));
    cppgen->setDoesGenerateOutput(true);
    cppgen->setNameChangeable(false);
    QString tempPluginName = cppgen->cleanName(m_PluginName->text());
    cppgen->setPluginName(tempPluginName);
    filt2cpp->setFileGenPtr(cppgen);

    //m_itemMap[filt2cpp] = cppgen;


    /* This simulates the user clicking on the "Add Filter" button */
    PMGeneratorTreeItem* filt2h = new PMGeneratorTreeItem(F_name);
    filt2h->setText(0, filterTitle + ".h");
    pathTemplate = "@PluginName@/@PluginName@Filters/";
    resourceTemplate = generateFileSystemPath("/Template/Filter/Filter.h.in");
    PMFilterGenerator* hgen = new PMFilterGenerator(m_OutputDir->text(),
                                                    pathTemplate,
                                                    QString(filterTitle + ".h"),
                                                    QString(filterTitle),
                                                    resourceTemplate,
                                                    filt2h,
                                                    this);
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            hgen, SLOT(pluginNameChanged(const QString&)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
            hgen, SLOT(outputDirChanged(const QString&)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            hgen, SLOT(generateOutput()));
    connect(hgen, SIGNAL(outputError(const QString&)),
            this, SLOT(generationError(const QString&)));
    hgen->setDoesGenerateOutput(true);
    hgen->setNameChangeable(false);
    tempPluginName = hgen->cleanName(m_PluginName->text());
    hgen->setPluginName(tempPluginName);
    filt2h->setFileGenPtr(hgen);

    //m_itemMap[filt2h] = hgen;


    /* This simulates the user clicking on the "Add Filter" button */
    PMGeneratorTreeItem* filt2html = new PMGeneratorTreeItem(F_namefilters);
    filt2html->setText(0, filterTitle + ".md");
    pathTemplate = "@PluginName@/Documentation/@PluginName@Filters/";
    resourceTemplate = generateFileSystemPath("/Template/Documentation/Filter/Documentation.md.in");
    PMFilterGenerator* htmlgen = new PMFilterGenerator(m_OutputDir->text(),
                                                       pathTemplate,
                                                       QString(filterTitle + ".md"),
                                                       QString(filterTitle),
                                                       resourceTemplate,
                                                       filt2html,
                                                       this);
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            htmlgen, SLOT(pluginNameChanged(const QString&)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
            htmlgen, SLOT(outputDirChanged(const QString&)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            htmlgen, SLOT(generateOutput()));
    connect(htmlgen, SIGNAL(outputError(const QString&)),
            this, SLOT(generationError(const QString&)));
    htmlgen->setDoesGenerateOutput(true);
    htmlgen->setNameChangeable(false);
    tempPluginName = htmlgen->cleanName(m_PluginName->text());
    htmlgen->setPluginName(tempPluginName);
    filt2html->setFileGenPtr(htmlgen);
    //m_itemMap[filt2html] = htmlgen;


    FilterBundler filterpack(cppgen, hgen, htmlgen, addFilterDialog->isPublic());
    m_FilterBundles.push_back(filterpack);
  }
  //  for(int i = 0;i < m_FilterBundles.count(); ++i)
  //  {
  //    std::cout  << "CPP: " << m_FilterBundles[i].getCPPGenerator()->getTreeWidgetItem()->text(0)() << "\n";
  //  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_removeFilterBtn_clicked()
{
  QTreeWidgetItem* ptr(treeWidget->currentItem());
  if (ptr == NULL)
  {
    return;
  }
  QString namecheck = ptr->text(0);

  for (int i = 0; i < m_FilterBundles.size(); i++)
  {
    if (m_FilterBundles[i].containsTreeWidgetItem(ptr))
    {
      //Remove the TreeWidgetItems from the tree widget
      treeWidget->removeItemWidget(m_FilterBundles[i].getCPPGenerator()->getTreeWidgetItem(), 0);
      treeWidget->removeItemWidget(m_FilterBundles[i].getHGenerator()->getTreeWidgetItem(), 0);
      treeWidget->removeItemWidget(m_FilterBundles[i].getHTMLGenerator()->getTreeWidgetItem(), 0);

      //Delete the TreeWidgetItems
      delete ( m_FilterBundles[i].getCPPGenerator()->getTreeWidgetItem() );
      delete ( m_FilterBundles[i].getHGenerator()->getTreeWidgetItem() );
      delete ( m_FilterBundles[i].getHTMLGenerator()->getTreeWidgetItem() );

      //Delete all 3 instances of PMFileGenerator
      delete ( m_FilterBundles[i].getCPPGenerator() );
      delete ( m_FilterBundles[i].getHGenerator() );
      delete ( m_FilterBundles[i].getHTMLGenerator() );

      //Remove the instance of FilterBundler from the m_FilterBundles QVector
      m_FilterBundles.remove(i);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_treeWidget_itemSelectionChanged()
{
  QString pluginName = cleanName(m_PluginName->text());

  PMGeneratorTreeItem* currentFile = dynamic_cast<PMGeneratorTreeItem*> ( treeWidget->currentItem() );
  if (currentFile == NULL)
  {
    return;
  }

  QString text = "";
  QTreeWidgetItem* parent = treeWidget->currentItem()->parent();
  if(NULL != parent
      && parent->text(0).endsWith("Filters")
      && currentFile->text(0).compare("SourceList.cmake") == 0 )
  {
    text = generateCmakeContents();
  }
  else
  {
    PMFileGenerator* fileGen = qobject_cast<PMFileGenerator*> ( currentFile->getFileGenPtr() );
    if (!fileGen)
    {
      return;
    }
    text = fileGen->generateFileContents();
  }

  m_fileEditor->setPlainText(text);

  statusbar->showMessage("Currently viewing " + currentFile->text(0));
}

// -----------------------------------------------------------------------------
//  Called when the main window is closed.
// -----------------------------------------------------------------------------
void PluginMaker::closeEvent(QCloseEvent* event)
{
  qint32 err = checkDirtyDocument();
  if (err < 0)
  {
    event->ignore();
  }
  else
  {
    writeSettings();
    event->accept();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 PluginMaker::checkDirtyDocument()
{
  qint32 err = -1;

  if (this->isWindowModified() == true)
  {
    int r = QMessageBox::warning(this, tr("DREAM.3D"),
                                 tr("The Data has been modified.\nDo you want to save your changes?"),
                                 QMessageBox::Save | QMessageBox::Default,
                                 QMessageBox::Discard,
                                 QMessageBox::Cancel | QMessageBox::Escape);
    if (r == QMessageBox::Save)
    {
      //TODO: Save the current document or otherwise save the state.
    }
    else if (r == QMessageBox::Discard)
    {
      err = 1;
    }
    else if (r == QMessageBox::Cancel)
    {
      err = -1;
    }
  }
  else
  {
    err = 1;
  }

  return err;
}

// -----------------------------------------------------------------------------
//  Write our Prefs to file
// -----------------------------------------------------------------------------
void PluginMaker::writeSettings()
{
  // qDebug() << "writeSettings" << "\n";
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif

  //Save the Plugin Name and Output Directory features to the QSettings object
  prefs.setValue("Plugin Name", m_PluginName->text());
  prefs.setValue("Output Directory", m_OutputDir->text());

  writeWindowSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::writeWindowSettings(QSettings& prefs)
{
  prefs.beginGroup("WindowSettings");
  QByteArray geo_data = saveGeometry();
  QByteArray layout_data = saveState();
  prefs.setValue(QString("Geometry"), geo_data);
  prefs.setValue(QString("Layout"), layout_data);
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::readSettings()
{
  // qDebug() << "Read Settings" << "\n";
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif

  // Have the PipelineBuilder Widget read its settings
  m_PluginName->setText( prefs.value("Plugin Name").toString() );
  m_OutputDir->setText( prefs.value("Output Directory").toString() );

  readWindowSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::readWindowSettings(QSettings& prefs)
{
  bool ok = false;
  prefs.beginGroup("WindowSettings");
  if (prefs.contains(QString("Geometry")) )
  {
    QByteArray geo_data = prefs.value(QString("Geometry")).toByteArray();
    ok = restoreGeometry(geo_data);
    if (!ok)
    {
      qDebug() << "Error Restoring the Window Geometry" << "\n";
    }
  }

  if (prefs.contains(QString("Layout")))
  {
    QByteArray layout_data = prefs.value(QString("Layout")).toByteArray();
    restoreState(layout_data);
  }
  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PluginMaker::generateCmakeContents()
{
  QString pluginName = m_PluginName->text();
  pluginName = cleanName(pluginName);

  QString cmakeHdrCode("${@PluginName@Plugin_SOURCE_DIR}/@PluginName@Filters/");
  cmakeHdrCode.replace("@PluginName@", pluginName);

  QString publicFilterList;
  QString privateFilterList;
  QString hdrContents;
  for (int i = 0; i < m_FilterBundles.count(); ++i)
  {
    // PMFileGenerator* cppGen = m_FilterBundles[i].getCPPGenerator();
    PMFileGenerator* hGen = m_FilterBundles[i].getHGenerator();
    bool isPublic = m_FilterBundles[i].isPublic();

    //qDebug() << cppGen->getFileName() << "\n";
    //qDebug() << hGen->getFileName() << "\n";

    //  hdrContents.append("    ").append(cmakeHdrCode).append(hGen->getFileName()).append("\n    ");
    //  srcContents.append("    ").append(cmakeHdrCode).append(cppGen->getFileName()).append("\n    ");

    QFileInfo fi(hGen->getFileName());
    QString className = fi.baseName();
    if (isPublic == true)
    {
      publicFilterList.append("  ").append(className).append("\n");
    }
    else
    {
      privateFilterList.append("  ").append(className).append("\n");
    }
    pluginName = m_PluginName->text();
    pluginName = cleanName(pluginName);
  }

  QString text = "";

  // Create SourceList File

  QFile rfile( generateFileSystemPath("/Template/Filter/SourceList.cmake.in") );
  if ( rfile.open(QIODevice::ReadOnly | QIODevice::Text) )
  {
    QTextStream in(&rfile);
    text = in.readAll();
    text.replace("@PluginName@", pluginName);
    text.replace("@GENERATED_CMAKE_HEADERS_CODE@", hdrContents);
    text.replace("@PUBLIC_FILTERS@", publicFilterList);
    text.replace("@PRIVATE_FILTERS@", privateFilterList);
  }
  return text;
}

