/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "PluginMaker.h"
#include <iostream>



#include <QtCore/QtDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtWidgets/QWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QApplication>
#include <QtCore/QSize>

#include "QtSupportLib/QFileCompleter.h"
#include "QtSupportLib/ApplicationFileInfo.h"

#include "AddFilterWidget.h"
#include "FilterBundler.h"
#include "PMDirGenerator.h"
#include "PMFileGenerator.h"
#include "PMGeneratorTreeItem.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PluginMaker::PluginMaker(QWidget* parent) :
  QWidget(parent)
{
  m_OpenDialogLastDirectory = QDir::homePath();
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::setupGui()
{
  // Setup error string
  errorString->setTextFormat(Qt::PlainText);
  errorString->setTextInteractionFlags(Qt::NoTextInteraction);
  errorString->changeStyleSheet(QFSDropLabel::FS_DOESNOTEXIST_STYLE);
  errorString->setText("");

  // Stretch Factors
  splitter->setStretchFactor(0, 0);
  splitter->setStretchFactor(1, 1);

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
  pathTemplate = "@PluginName@";
  QString resourceTemplate( ApplicationFileInfo::GenerateFileSystemPath("/Template/Constants.h.in") );
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
  pathTemplate = "@PluginName@";
  resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Plugin.cpp.in");
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
  pathTemplate = "@PluginName@";
  resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Plugin.h.in");
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
    pathTemplate = "@PluginName@/Resources";

    QString resourceTemplate(ApplicationFileInfo::GenerateFileSystemPath("/Template/Resources/SourceList.cmake.in"));
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
    pathTemplate = "@PluginName@/Resources";

    QString resourceTemplate(ApplicationFileInfo::GenerateFileSystemPath("/Template/Resources/Filter.qrc.in"));
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
    pathTemplate = "@PluginName@/Resources";
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
    pathTemplate = "@PluginName@/Resources/@PluginName@";

    QString resourceTemplate(ApplicationFileInfo::GenerateFileSystemPath("/Template/Resources/FilterLicense.txt.in"));
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
    pathTemplate = "@PluginName@/Resources/@PluginName@";

    QString resourceTemplate(ApplicationFileInfo::GenerateFileSystemPath("/Template/Resources/FilterDescription.txt.in"));
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
  }

  F_test = new PMGeneratorTreeItem(F_main);
  F_test->setText(0, tr("Test"));

  PMGeneratorTreeItem* F_test_cmake = new PMGeneratorTreeItem(F_test);
  F_test_cmake->setText(0, tr("CMakeLists.txt"));
  {
    pathTemplate = "@PluginName@/Test";
    QString resourceTemplate(ApplicationFileInfo::GenerateFileSystemPath("/Template/Test/CMakeLists.txt.in"));
    PMFileGenerator* gen = new PMFileGenerator(m_OutputDir->text(),
                                               pathTemplate,
                                               QString("CMakeLists.txt"),
                                               resourceTemplate,
                                               F_test_cmake,
                                               this);

    F_test_cmake->setFileGenPtr(gen);
    gen->setDoesGenerateOutput(true);
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            gen, SLOT(pluginNameChanged(const QString&)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
            gen, SLOT(outputDirChanged(const QString&)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            this, SLOT(testFileLocationsHandler()));
    connect(this, SIGNAL(clicked(QSet<QString>)),
            gen, SLOT(generateOutputWithFilterNames(QSet<QString>)));
    connect(gen, SIGNAL(outputError(const QString&)),
            this, SLOT(generationError(const QString&)));
  }

  PMGeneratorTreeItem* F_test_fileLoc = new PMGeneratorTreeItem(F_test);
  F_test_fileLoc->setText(0, tr("TestFileLocations.h.in"));
  {
    pathTemplate = "@PluginName@/Test";
    QString resourceTemplate(ApplicationFileInfo::GenerateFileSystemPath("/Template/Test/TestFileLocations.h.in.in"));
    PMFileGenerator* gen = new PMFileGenerator(m_OutputDir->text(),
                                               pathTemplate,
                                               QString("TestFileLocations.h.in"),
                                               resourceTemplate,
                                               F_test_fileLoc,
                                               this);

    m_TestFileLocationNames.insert("@PluginName@Filter");   // This name needs to be used in the TestFileLocations.h.in file
    F_test_fileLoc->setFileGenPtr(gen);
    gen->setDoesGenerateOutput(true);
    gen->setNameChangeable(false);
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            gen, SLOT(pluginNameChanged(const QString&)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
            gen, SLOT(outputDirChanged(const QString&)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            this, SLOT(testFileLocationsHandler()));
    connect(this, SIGNAL(clicked(QSet<QString>)),
            gen, SLOT(generateOutputWithFilterNames(QSet<QString>)));
    connect(gen, SIGNAL(outputError(const QString&)),
            this, SLOT(generationError(const QString&)));
  }

  PMGeneratorTreeItem* F_test_filterTest = new PMGeneratorTreeItem(F_test);
  F_test_filterTest->setText(0, tr("Unknown Plugin Name"));
  pathTemplate = "@PluginName@/Test";
  resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Test/FilterTest.cpp.in");
  PMFileGenerator* testgen = new PMFileGenerator(m_OutputDir->text(),
                                                 pathTemplate,
                                                 QString("@PluginName@"),
                                                 resourceTemplate,
                                                 F_test_filterTest,
                                                 this);

  F_test_filterTest->setFileGenPtr(testgen);
  testgen->setDoesGenerateOutput(true);
  testgen->setNameChangeable(true);
  testgen->setDisplaySuffix("FilterTest.cpp");
  //testgen->setDisplaySuffix("FilterTest.cpp");
  connect(m_PluginName, SIGNAL(textChanged(const QString&)),
          testgen, SLOT(pluginNameChanged(const QString&)));
  connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
          testgen, SLOT(outputDirChanged(const QString&)));
  // For "Directories" this probably isn't needed
  connect(generateButton, SIGNAL(clicked()),
          testgen, SLOT(generateOutput()));
  connect(testgen, SIGNAL(outputError(const QString&)),
          this, SLOT(generationError(const QString&)));


  F_doc = new PMGeneratorTreeItem(F_main);
  F_doc->setText(0, tr("Documentation"));

  PMGeneratorTreeItem* cmake = new PMGeneratorTreeItem(F_main);
  cmake->setText(0, tr("CMakeLists.txt"));
  {
    pathTemplate = "@PluginName@";

    QString resourceTemplate(ApplicationFileInfo::GenerateFileSystemPath("/Template/CMakeLists.txt.in"));
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

  // Dummy test file (to bundle the plugin CPP and H files in a FilterBundler)
  PMFileGenerator* testPluginFile = new PMFileGenerator("", "", "", "", NULL, this);

  FilterBundler fb(cppPluginGen, hPluginGen, htmlPluginDoc, testPluginFile, true);



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
  resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Filter/Filter.cpp.in");
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
  cppFilterGen->setInitListContents("  AbstractFilter()");
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
  resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Filter/Filter.h.in");
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
    resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/FilterParameterWidgets/SourceList.cmake.in");
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
    resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Widgets/SourceList.cmake.in");
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
  resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Documentation/Filter/Documentation.md.in");
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

  FilterBundler fb2(cppFilterGen, hFilterGen, htmlFilterDoc, testgen, true);
  m_FilterBundles.push_back(fb2);

  m_PluginName->setText("Unknown Plugin Name");
  m_PluginName->selectAll();
  treeWidget->expandAll();
  emit updateStatusBar("Ready");

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

  // Delete the old plugin folder, if it is there
  QString pluginPath = pluginDir + "/" + pluginName;
  pluginPath = QDir::toNativeSeparators(pluginPath);
  QFileInfo fi(pluginPath);
  if (fi.exists())
  {
    QDir dir(pluginPath);
    dir.removeRecursively();
  }

  pluginName = cleanName(pluginName);

  if (pluginName == "")
  {
    emit updateStatusBar("Generation Failed --- Please provide a plugin name");
    QMessageBox::critical(this, tr("PluginMaker"), tr("The file generation was unsuccessful.\n"
                                                      "Please enter a Plugin Name."));
    return;
  }
  else if (pluginDir == "")
  {
    emit updateStatusBar("Generation Failed --- Please provide a plugin directory");
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

  emit updateStatusBar("Generation Completed");
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
  validityCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_m_OutputDir_textChanged(const QString& text)
{
  validityCheck();
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
    QString resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Filter/Filter.cpp.in");
    PMFileGenerator* cppgen = new PMFileGenerator(m_OutputDir->text(),
                                                  pathTemplate,
                                                  QString(filterTitle + ".cpp"),
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
    cppgen->setInitListContents("  AbstractFilter()");
    filt2cpp->setFileGenPtr(cppgen);

    //m_itemMap[filt2cpp] = cppgen;


    /* This simulates the user clicking on the "Add Filter" button */
    PMGeneratorTreeItem* filt2h = new PMGeneratorTreeItem(F_name);
    filt2h->setText(0, filterTitle + ".h");
    pathTemplate = "@PluginName@/@PluginName@Filters/";
    resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Filter/Filter.h.in");
    PMFileGenerator* hgen = new PMFileGenerator(m_OutputDir->text(),
                                                pathTemplate,
                                                QString(filterTitle + ".h"),
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
    resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Documentation/Filter/Documentation.md.in");
    PMFileGenerator* htmlgen = new PMFileGenerator(m_OutputDir->text(),
                                                   pathTemplate,
                                                   QString(filterTitle + ".md"),
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


    PMGeneratorTreeItem* filt2test = new PMGeneratorTreeItem(F_test);
    filt2test->setText(0, tr("Unknown Plugin Name"));
    pathTemplate = "@PluginName@/Test";
    resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Test/FilterTest.cpp.in");
    PMFileGenerator* testgen = new PMFileGenerator(m_OutputDir->text(),
                                                   pathTemplate,
                                                   QString(filterTitle + "Test.cpp"),
                                                   resourceTemplate,
                                                   filt2test,
                                                   this);

    m_TestFileLocationNames.insert(filterTitle);    // This name needs to be used in the TestFileLocations.h.in file
    filt2test->setFileGenPtr(testgen);
    connect(m_PluginName, SIGNAL(textChanged(const QString&)),
            testgen, SLOT(pluginNameChanged(const QString&)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString&)),
            testgen, SLOT(outputDirChanged(const QString&)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            testgen, SLOT(generateOutput()));
    connect(testgen, SIGNAL(outputError(const QString&)),
            this, SLOT(generationError(const QString&)));
    htmlgen->setNameChangeable(false);
    testgen->setDoesGenerateOutput(true);
    tempPluginName = testgen->cleanName(m_PluginName->text());
    testgen->setPluginName(tempPluginName);
    filt2test->setFileGenPtr(testgen);


    FilterBundler filterpack(cppgen, hgen, htmlgen, testgen, addFilterDialog->isPublic());
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
  QString filePath = ptr->text(0);
  QFileInfo fi(filePath);
  QString namecheck = fi.baseName();

  for (int i = 0; i < m_FilterBundles.size(); i++)
  {
    if (m_FilterBundles[i].containsTreeWidgetItem(ptr))
    {
      // Remove the entry from the TestFileLocations.h.in cache
      QString pluginName = m_FilterBundles[i].getTestGenerator()->getPluginName();
      if (namecheck == pluginName + "Filter")
      {
        namecheck = "@PluginName@Filter";
      }

      m_TestFileLocationNames.remove(namecheck);

      //Remove the TreeWidgetItems from the tree widget
      treeWidget->removeItemWidget(m_FilterBundles[i].getCPPGenerator()->getTreeWidgetItem(), 0);
      treeWidget->removeItemWidget(m_FilterBundles[i].getHGenerator()->getTreeWidgetItem(), 0);
      treeWidget->removeItemWidget(m_FilterBundles[i].getHTMLGenerator()->getTreeWidgetItem(), 0);
      treeWidget->removeItemWidget(m_FilterBundles[i].getTestGenerator()->getTreeWidgetItem(), 0);

      //Delete the TreeWidgetItems
      delete ( m_FilterBundles[i].getCPPGenerator()->getTreeWidgetItem() );
      delete ( m_FilterBundles[i].getHGenerator()->getTreeWidgetItem() );
      delete ( m_FilterBundles[i].getHTMLGenerator()->getTreeWidgetItem() );
      delete (m_FilterBundles[i].getTestGenerator()->getTreeWidgetItem() );

      //Delete all 3 instances of PMFileGenerator
      delete ( m_FilterBundles[i].getCPPGenerator() );
      delete ( m_FilterBundles[i].getHGenerator() );
      delete ( m_FilterBundles[i].getHTMLGenerator() );
      delete (m_FilterBundles[i].getTestGenerator() );

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
  else if ( NULL != parent
            && parent->text(0) == "Test"
            && currentFile->text(0) == "TestFileLocations.h.in")
  {
    PMFileGenerator* fileGen = qobject_cast<PMFileGenerator*> (currentFile->getFileGenPtr());
    if (!fileGen)
    {
      return;
    }

    QString replaceStr = fileGen->createReplacementString(TESTFILELOCATIONS, m_TestFileLocationNames);
    text = fileGen->generateFileContents(replaceStr);
  }
  else if (NULL != parent
           && parent->text(0) == "Test"
           && currentFile->text(0) == "CMakeLists.txt")
  {
    PMFileGenerator* fileGen = qobject_cast<PMFileGenerator*> (currentFile->getFileGenPtr());
    if (!fileGen)
    {
      return;
    }

    QString replaceStr = fileGen->createReplacementString(CMAKELISTS, m_TestFileLocationNames);
    text = fileGen->generateFileContents(replaceStr);
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

  emit updateStatusBar("Currently viewing " + currentFile->text(0));
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

  QFile rfile(ApplicationFileInfo::GenerateFileSystemPath("/Template/Filter/SourceList.cmake.in"));
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::testFileLocationsHandler()
{
  emit clicked(m_TestFileLocationNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PluginMaker::validityCheck()
{
  QString filePath = m_OutputDir->text();
  QFileInfo fi(filePath);
  if (m_PluginName->text().isEmpty() || m_OutputDir->text().isEmpty() || fi.exists() == false)
  {
    generateButton->setDisabled(true);
    if (m_PluginName->text().isEmpty())
    {
      errorString->setText("The plugin name has not been chosen.");
    }
    else if (m_OutputDir->text().isEmpty())
    {
      errorString->setText("The output directory has not been selected.");
    }
    else
    {
      errorString->setText("The currently selected output directory does not exist.");
    }

    return false;
  }
  else
  {
    generateButton->setEnabled(true);
    errorString->setText("");
    emit updateStatusBar("Ready");
    return true;
  }
}



