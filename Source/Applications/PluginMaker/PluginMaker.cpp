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

#include "HelpWidget.h"
#include "AddFilterWidget.h"
#include "FilterBundler.h"
#include "DREAM3D/License/PluginMakerLicenseFiles.h"
#include "QtSupport/ApplicationAboutBoxDialog.h"
#include "PMDirGenerator.h"
#include "PMFileGenerator.h"
#include "PMFilterGenerator.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PluginMaker::PluginMaker(QWidget* parent) :
 QMainWindow(parent)
{
  setupUi(this);

  setupGui();

  //Set window to open at the center of the screen
  QDesktopWidget *desktop = QApplication::desktop();
 
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

  QString pathTemplate;

  nameLabel->setToolTip("Plugin Name");
  m_PluginName->setToolTip("Enter Plugin Name Here");
  outputFileNameLabel->setToolTip("Output Directory");
  m_OutputDir->setToolTip("Enter Output Directory Here");
  selectButton->setToolTip("Select Directory");
  generateButton->setToolTip("Generate File Structure");


  QTreeWidgetItem* F_main = new QTreeWidgetItem(treeWidget);
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
     m_GenObjects.push_back(gen);
     connect(m_PluginName, SIGNAL(textChanged(const QString &)),
             gen, SLOT(pluginNameChanged(const QString &)));

   }


  QTreeWidgetItem* F_code = new QTreeWidgetItem(F_main);
  F_code->setText(0, tr("Code"));


  F_doc = new QTreeWidgetItem(F_main);
  F_doc->setText(0, tr("Documentation"));

  QTreeWidgetItem* cmake = new QTreeWidgetItem(F_main);
  cmake->setText(0, tr("CMakeLists.txt"));
  {
    pathTemplate = "@PluginName@";
    QString resourceTemplate(":/Template/CMakeLists.txt.in");
    PMFileGenerator* gen = new PMFileGenerator(m_OutputDir->text(),
                                                    pathTemplate,
                                                    QString("CMakeLists.txt"),
                                                    resourceTemplate,
                                                    cmake,
                                                    this);
    connect(m_PluginName, SIGNAL(textChanged(const QString &)),
            gen, SLOT(pluginNameChanged(const QString &)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString &)),
            gen, SLOT(outputDirChanged(const QString &)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            gen, SLOT(generateOutput()));
    connect(gen, SIGNAL(outputError(const QString &)),
            this, SLOT(generationError(const QString &)));
    gen->setDoesGenerateOutput(true);
    m_GenObjects.push_back(gen);
  }




  F_name = new QTreeWidgetItem(F_code);
  F_name->setText(0, "Unknown Plugin Name");
  {
     pathTemplate = "@PluginName@/Code/@PluginName@Filters/";
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
     m_GenObjects.push_back(gen);
     connect(m_PluginName, SIGNAL(textChanged(const QString &)),
             gen, SLOT(pluginNameChanged(const QString &)));

   }



  QTreeWidgetItem* plugincpp = new QTreeWidgetItem(F_code);
  plugincpp->setText(0, "Unknown Plugin Name");
  {
    pathTemplate = "@PluginName@/Code/";
    QString resourceTemplate(":/Template/Code/Plugin.cpp.in");
    PMFileGenerator* gen = new PMFileGenerator(m_OutputDir->text(),
                                                    pathTemplate,
                                                    QString(""),
                                                    resourceTemplate,
                                                    plugincpp,
                                                    this);
    gen->setDisplaySuffix("Plugin.cpp");
    gen->setDoesGenerateOutput(true);
    gen->setNameChangeable(true);
    m_GenObjects.push_back(gen);
    connect(m_PluginName, SIGNAL(textChanged(const QString &)),
            gen, SLOT(pluginNameChanged(const QString &)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString &)),
            gen, SLOT(outputDirChanged(const QString &)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            gen, SLOT(generateOutput()));
    connect(gen, SIGNAL(outputError(const QString &)),
            this, SLOT(generationError(const QString &)));
  }

  QTreeWidgetItem* pluginh = new QTreeWidgetItem(F_code);
  pluginh->setText(0, "Unknown Plugin Name");
  {
    pathTemplate = "@PluginName@/Code/";
    QString resourceTemplate(":/Template/Code/Plugin.h.in");
    PMFileGenerator* gen = new PMFileGenerator(m_OutputDir->text(),
                                                    pathTemplate,
                                                    QString(""),
                                                    resourceTemplate,
                                                    pluginh,
                                                    this);
    gen->setDisplaySuffix("Plugin.h");
    gen->setDoesGenerateOutput(true);
    gen->setNameChangeable(true);
    m_GenObjects.push_back(gen);
    connect(m_PluginName, SIGNAL(textChanged(const QString &)),
            gen, SLOT(pluginNameChanged(const QString &)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString &)),
            gen, SLOT(outputDirChanged(const QString &)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            gen, SLOT(generateOutput()));
    connect(gen, SIGNAL(outputError(const QString &)),
            this, SLOT(generationError(const QString &)));
  }




  QTreeWidgetItem* filtercpp = new QTreeWidgetItem(F_name);
  filtercpp->setText(0, "Unknown Plugin Name");
    pathTemplate = "@PluginName@/Code/@PluginName@Filters/";
    QString resourceTemplate(":/Template/Code/Filter/Filter.cpp.in");
    PMFileGenerator* cppgen = new PMFileGenerator(m_OutputDir->text(),
                                                    pathTemplate,
                                                    QString(""),
                                                    resourceTemplate,
                                                    filtercpp,
                                                    this);
    cppgen->setDisplaySuffix("Filter.cpp");
    cppgen->setDoesGenerateOutput(true);
    cppgen->setNameChangeable(true);
    connect(m_PluginName, SIGNAL(textChanged(const QString &)),
            cppgen, SLOT(pluginNameChanged(const QString &)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString &)),
            cppgen, SLOT(outputDirChanged(const QString &)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            cppgen, SLOT(generateOutput()));
    connect(cppgen, SIGNAL(outputError(const QString &)),
            this, SLOT(generationError(const QString &)));


  QTreeWidgetItem* filterh = new QTreeWidgetItem(F_name);
  filterh->setText(0, "Unknown Plugin Name");
    pathTemplate = "@PluginName@/Code/@PluginName@Filters/";
    resourceTemplate = ":/Template/Code/Filter/Filter.h.in";
    PMFileGenerator* hgen = new PMFileGenerator(m_OutputDir->text(),
                                                    pathTemplate,
                                                    QString(""),
                                                    resourceTemplate,
                                                    filterh,
                                                    this);
    hgen->setDisplaySuffix("Filter.h");
    hgen->setDoesGenerateOutput(true);
    hgen->setNameChangeable(true);
    connect(m_PluginName, SIGNAL(textChanged(const QString &)),
            hgen, SLOT(pluginNameChanged(const QString &)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString &)),
            hgen, SLOT(outputDirChanged(const QString &)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            hgen, SLOT(generateOutput()));
    connect(hgen, SIGNAL(outputError(const QString &)),
            this, SLOT(generationError(const QString &)));




  QTreeWidgetItem* sourceList = new QTreeWidgetItem(F_name);
  sourceList->setText(0, tr("SourceList.cmake"));



  F_namefilters = new QTreeWidgetItem(F_doc);
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
       m_GenObjects.push_back(gen);
       connect(m_PluginName, SIGNAL(textChanged(const QString &)),
               gen, SLOT(pluginNameChanged(const QString &)));
     }


  QTreeWidgetItem* pluginDocs = new QTreeWidgetItem(F_doc);
  pluginDocs->setText(0, "PluginDocumentation.qrc");


  QTreeWidgetItem* htmlDoc = new QTreeWidgetItem(F_namefilters);
  htmlDoc->setText(0, "Unknown Plugin Name");
    pathTemplate = "@PluginName@/Documentation/@PluginName@Filters/";
    resourceTemplate = ":/Template/Documentation/Filter/Documentation.html.in";
    PMFileGenerator* htmlgen = new PMFileGenerator(m_OutputDir->text(),
                                                    pathTemplate,
                                                    QString(""),
                                                    resourceTemplate,
                                                    htmlDoc,
                                                    this);
    htmlgen->setDisplaySuffix("Filter.html");
    htmlgen->setDoesGenerateOutput(true);
    htmlgen->setNameChangeable(true);
    connect(m_PluginName, SIGNAL(textChanged(const QString &)),
            htmlgen, SLOT(pluginNameChanged(const QString &)));
    connect(m_OutputDir, SIGNAL(textChanged(const QString &)),
            htmlgen, SLOT(outputDirChanged(const QString &)));
    // For "Directories" this probably isn't needed
    connect(generateButton, SIGNAL(clicked()),
            htmlgen, SLOT(generateOutput()));
    connect(htmlgen, SIGNAL(outputError(const QString &)),
            this, SLOT(generationError(const QString &)));

    FilterBundler fb(cppgen, hgen, htmlgen);
    m_FilterBundles.push_back(fb);

  m_PluginName->setText("Unknown Plugin Name");
  m_PluginName->selectAll();
  treeWidget->expandAll();
  statusbar->showMessage("Ready");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_selectButton_clicked() {
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

  if (pluginName == "") {
    statusbar->showMessage("Generation Failed --- Please provide a plugin name");
    QMessageBox::critical(this, tr("PluginMaker"), tr("The file generation was unsuccessful.\n"
      "Please enter a Plugin Name."));
    return;
  }
  else if (pluginDir == "") {
    statusbar->showMessage("Generation Failed --- Please provide a plugin directory");
    QMessageBox::critical(this, tr("PluginMaker"), tr("The file generation was unsuccessful.\n"
      "Please enter a Plugin Directory."));
    return;
  }

  // WE need to generate the SourceList.cmake file here because we possibly have
  // more than a single filter
  QString cmakeHdrCode("${@PluginName@Plugin_SOURCE_DIR}/Code/@PluginName@Filters/");
  cmakeHdrCode.replace("@PluginName@", m_PluginName->text());

  QString srcContents;
  QString hdrContents;
  for (int i = 0; i < m_FilterBundles.count(); ++i)
  {
    PMFileGenerator* cppGen = m_FilterBundles[i].getCPPGenerator();
    PMFileGenerator* hGen = m_FilterBundles[i].getHGenerator();

      hdrContents.append("    ").append(cmakeHdrCode).append(hGen->getFileName()).append("\n    ");
      srcContents.append("    ").append(cmakeHdrCode).append(cppGen->getFileName()).append("\n    ");
  
    pluginName = m_GenObjects[i]->getPluginName();
  }

  // Create SourceList File
  QFile rfile(":/Template/Code/Filter/SourceList.cmake.in");
  if ( rfile.open(QIODevice::ReadOnly | QIODevice::Text) )
  {
    QTextStream in(&rfile);
    QString text = in.readAll();
    text.replace("@PluginName@", pluginName);
    text.replace("@GENERATED_CMAKE_HEADERS_CODE@", hdrContents);
    text.replace("@GENERATED_CMAKE_SOURCE_CODE@", srcContents);
    QString pathTemplate = "@PluginName@/Code/@PluginName@Filters/";
    QString parentPath = m_OutputDir->text() + QDir::separator()
                        + pathTemplate.replace("@PluginName@", pluginName);
    parentPath = QDir::toNativeSeparators(parentPath);

    QDir dir(parentPath);
    dir.mkpath(parentPath);

    parentPath = parentPath + QDir::separator() + "SourceList.cmake";
    //Write to file
    QFile f(parentPath);
    if ( f.open(QIODevice::WriteOnly | QIODevice::Text) ) {
      QTextStream out(&f);
      out << text;
    }
  }

  // WE need to generate the QRC file here because we possibly have
  // more than a single filter
  QString htmlPathCode("@PluginName@Filters/");
  htmlPathCode.replace("@PluginName@", m_PluginName->text());

  QString htmlContents = "";
  for (int i = 0; i < m_FilterBundles.count(); ++i)
  {
    PMFileGenerator* htmlGen = m_FilterBundles[i].getHTMLGenerator();

    htmlContents.append("<file>").append(htmlPathCode).append(htmlGen->getFileName()).append("</file>\n");

    pluginName = m_GenObjects[i]->getPluginName();
  }

  // Create QRC File
  QFile rfile2(":/Template/Documentation/FilterDocs.qrc.in");
  if ( rfile2.open(QIODevice::ReadOnly | QIODevice::Text) )
  {
    QTextStream in(&rfile2);
    QString text = in.readAll();
    text.replace("@PluginName@", pluginName);
    text.replace("@GENERATED_HTML_FILTERS_CODE@", htmlContents);
    QString pathTemplate = "@PluginName@/Documentation/";
    QString parentPath = m_OutputDir->text() + QDir::separator()
                        + pathTemplate.replace("@PluginName@", pluginName);
    parentPath = QDir::toNativeSeparators(parentPath);

    QDir dir(parentPath);
    dir.mkpath(parentPath);

    parentPath = parentPath + QDir::separator() + "PluginDocumentation.qrc";
    //Write to file
    QFile f(parentPath);
    if ( f.open(QIODevice::WriteOnly | QIODevice::Text) ) {
      QTextStream out(&f);
      out << text;
    }
  }

  statusbar->showMessage("Generation Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PluginMaker::cleanName(QString name) {
  //Remove all uses of "Plugin", "plugin", "Filter", and "filter"
  QRegExp rx("Plugin|plugin|Filter|filter");
  name.replace(rx, "");
  //Remove all spaces and illegal characters from plugin name
  name.trimmed();
  name.remove(" ");
  name.remove(QRegExp("[^a-zA-Z_\\d\\s]"));
  return name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PluginMaker::cleanName_filters(QString name) {
  //Remove all spaces and illegal characters from plugin name
  name.trimmed();
  name.remove(" ");
  name.remove(QRegExp("[^a-zA-Z_\\d\\s]"));
  return name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_m_PluginName_textChanged(const QString & text) {
  statusbar->showMessage("Ready");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_m_OutputDir_textChanged(const QString & text) {
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
    QTreeWidgetItem* filt2cpp = new QTreeWidgetItem(F_name);
    filt2cpp->setText(0, filterTitle + ".cpp");
      QString pathTemplate = "@PluginName@/Code/@PluginName@Filters/";
      QString resourceTemplate(":/Template/Code/Filter/Filter.cpp.in");
      PMFilterGenerator* cppgen = new PMFilterGenerator(m_OutputDir->text(),
                                                    pathTemplate,
                                                    QString(filterTitle + ".cpp"),
                                                    QString(filterTitle),
                                                    resourceTemplate,
                                                    filt2cpp,
                                                    this);
      connect(m_PluginName, SIGNAL(textChanged(const QString &)),
            cppgen, SLOT(pluginNameChanged(const QString &)));
      connect(m_OutputDir, SIGNAL(textChanged(const QString &)),
            cppgen, SLOT(outputDirChanged(const QString &)));
      // For "Directories" this probably isn't needed
      connect(generateButton, SIGNAL(clicked()),
            cppgen, SLOT(generateOutput()));
      connect(cppgen, SIGNAL(outputError(const QString &)),
            this, SLOT(generationError(const QString &)));
      cppgen->setDoesGenerateOutput(true);
      cppgen->setNameChangeable(false);
      QString tempPluginName = cppgen->cleanName(m_PluginName->text());
      cppgen->setPluginName(tempPluginName);
      

    /* This simulates the user clicking on the "Add Filter" button */
    QTreeWidgetItem* filt2h = new QTreeWidgetItem(F_name);
    filt2h->setText(0, filterTitle + ".h");
      pathTemplate = "@PluginName@/Code/@PluginName@Filters/";
      resourceTemplate = ":/Template/Code/Filter/Filter.h.in";
      PMFilterGenerator* hgen = new PMFilterGenerator(m_OutputDir->text(),
                                                    pathTemplate,
                                                    QString(filterTitle + ".h"),
                                                    QString(filterTitle),
                                                    resourceTemplate,
                                                    filt2h,
                                                    this);
      connect(m_PluginName, SIGNAL(textChanged(const QString &)),
            hgen, SLOT(pluginNameChanged(const QString &)));
      connect(m_OutputDir, SIGNAL(textChanged(const QString &)),
            hgen, SLOT(outputDirChanged(const QString &)));
      // For "Directories" this probably isn't needed
      connect(generateButton, SIGNAL(clicked()),
            hgen, SLOT(generateOutput()));
      connect(hgen, SIGNAL(outputError(const QString &)),
            this, SLOT(generationError(const QString &)));
      hgen->setDoesGenerateOutput(true);
      hgen->setNameChangeable(false);
      tempPluginName = hgen->cleanName(m_PluginName->text());
      hgen->setPluginName(tempPluginName);



    /* This simulates the user clicking on the "Add Filter" button */
    QTreeWidgetItem* filt2html = new QTreeWidgetItem(F_namefilters);
    filt2html->setText(0, filterTitle + ".html");
      pathTemplate = "@PluginName@/Documentation/@PluginName@Filters/";
      resourceTemplate = ":/Template/Documentation/Filter/Documentation.html.in";
      PMFilterGenerator* htmlgen = new PMFilterGenerator(m_OutputDir->text(),
                                                    pathTemplate,
                                                    QString(filterTitle + ".html"),
                                                    QString(filterTitle),
                                                    resourceTemplate,
                                                    filt2html,
                                                    this);
      connect(m_PluginName, SIGNAL(textChanged(const QString &)),
            htmlgen, SLOT(pluginNameChanged(const QString &)));
      connect(m_OutputDir, SIGNAL(textChanged(const QString &)),
            htmlgen, SLOT(outputDirChanged(const QString &)));
      // For "Directories" this probably isn't needed
      connect(generateButton, SIGNAL(clicked()),
            htmlgen, SLOT(generateOutput()));
      connect(htmlgen, SIGNAL(outputError(const QString &)),
            this, SLOT(generationError(const QString &)));
      htmlgen->setDoesGenerateOutput(true);
      htmlgen->setNameChangeable(false);
      tempPluginName = htmlgen->cleanName(m_PluginName->text());
      htmlgen->setPluginName(tempPluginName);


      FilterBundler filterpack(cppgen, hgen, htmlgen);
      m_FilterBundles.push_back(filterpack);
  }
  for(int i = 0;i < m_FilterBundles.count(); ++i)
  {
    std::cout  << "CPP: " << m_FilterBundles[i].getCPPGenerator()->getTreeWidgetItem()->text(0).toStdString() << std::endl;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_removeFilterBtn_clicked() {
  QTreeWidgetItem* ptr(treeWidget->currentItem());
  if (ptr == NULL) {
    return;
  }
  QString namecheck = ptr->text(0);
  
  for (int i=0; i<m_FilterBundles.size(); i++)
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
void PluginMaker::on_treeWidget_itemSelectionChanged() {
  #if 0
  QTreeWidgetItem* currentFile = treeWidget->currentItem();

  for (int i = 0; i < m_FilterBundles.count(); i++) {
    if (m_FilterBundles[i].containsTreeWidgetItem(currentFile) ) {
      if (m_FilterBundles[i].getCPPGenerator()->getTreeWidgetItem() == currentFile) {
        previewFile(":/Template/Code/Filter/Filter.cpp.in", currentFile->text(0));
      }
      else if (m_FilterBundles[i].getHGenerator()->getTreeWidgetItem() == currentFile) {
        previewFile(":/Template/Code/Filter/Filter.h.in", currentFile->text(0));
      }
      else if (m_FilterBundles[i].getHTMLGenerator()->getTreeWidgetItem() == currentFile) {
        previewFile(":/Template/Documentation/Filter/Documentation.html.in", currentFile->text(0));
      }
    }
    else {
      filePreview->setText("");
    }
  }
  #endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::previewFile(QString rTemplate, QString fileName) {
    #if 0
    QString pluginName = m_GenObjects[0]->getPluginName();

    //Open file
    QFile rfile(rTemplate);
    if ( rfile.open(QIODevice::ReadOnly | QIODevice::Text) ) {
      QTextStream in(&rfile);
      QString text = in.readAll();
      text.replace("@PluginName@", pluginName);
      QFileInfo fi(fileName);
      QString className = fi.baseName();
      text.replace("@ClassName@", className);
      filePreview->setText(text);
    }
    #endif
}

// -----------------------------------------------------------------------------
//  Called when the main window is closed.
// -----------------------------------------------------------------------------
void PluginMaker::closeEvent(QCloseEvent *event)
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
  // std::cout << "writeSettings" << std::endl;
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif

  //Save the Plugin Name and Output Directory fields to the QSettings object
  prefs.setValue("Plugin Name", m_PluginName->text());
  prefs.setValue("Output Directory", m_OutputDir->text());

  writeWindowSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::writeWindowSettings(QSettings &prefs)
{
  prefs.beginGroup("WindowSettings");
  QByteArray geo_data = saveGeometry();
  QByteArray layout_data = saveState();
  prefs.setValue(QString("Geometry"), geo_data);
  prefs.setValue(QString("Layout"), layout_data);
  prefs.endGroup();
}

void PluginMaker::readSettings()
{
  // std::cout << "Read Settings" << std::endl;
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

void PluginMaker::readWindowSettings(QSettings &prefs)
{
  bool ok = false;
  prefs.beginGroup("WindowSettings");
  if (prefs.contains(QString("Geometry")) )
  {
    QByteArray geo_data = prefs.value(QString("Geometry")).toByteArray();
    ok = restoreGeometry(geo_data);
    if (!ok)
    {
    	std::cout << "Error Restoring the Window Geometry" << std::endl;
    }
  }

  if (prefs.contains(QString("Layout")))
  {
    QByteArray layout_data = prefs.value(QString("Layout")).toByteArray();
    restoreState(layout_data);
  }
  prefs.endGroup();
}