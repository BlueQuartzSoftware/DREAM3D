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
#include "HelpWidget.h"
#include "AboutWidget.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtGui/QWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QProgressBar>
#include <QtGui/QMessageBox>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PluginMaker::PluginMaker(QWidget* parent) :
 QMainWindow(parent)
{
  setupUi(this);

  setupGui();


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::setupGui()
{
  nameLabel->setToolTip("Plugin Name");
  name->setToolTip("Enter Plugin Name Here");
  outputFileNameLabel->setToolTip("Output Directory");
  outputFileName->setToolTip("Enter Output Directory Here");
  selectButton->setToolTip("Select Directory");
  generateButton->setToolTip("Generate File Structure");
  aboutButton->setToolTip("About PluginMaker©");

  F_main = new QTreeWidgetItem(treeWidget);
  F_main->setText(0, "Unknown Plugin Name");

  F_code = new QTreeWidgetItem(F_main);
  F_code->setText(0, tr("Code"));

  QTreeWidgetItem *F_doc = new QTreeWidgetItem(F_main);
  F_doc->setText(0, tr("Documentation"));

  QTreeWidgetItem *cmake = new QTreeWidgetItem(F_main);
  cmake->setText(0, tr("CMakeLists.txt"));

  F_name = new QTreeWidgetItem(F_code);
  F_name->setText(0, "Unknown Plugin Name");

  plugincpp = new QTreeWidgetItem(F_code);
  plugincpp->setText(0, "Unknown Plugin Name");

  pluginh = new QTreeWidgetItem(F_code);
  pluginh->setText(0, "Unknown Plugin Name");

  filtercpp = new QTreeWidgetItem(F_name);
  filtercpp->setText(0, "Unknown Plugin Name");

  filterh = new QTreeWidgetItem(F_name);
  filterh->setText(0, "Unknown Plugin Name");

  sourceList = new QTreeWidgetItem(F_name);
  sourceList->setText(0, tr("SourceList.cmake"));

  F_namefilters = new QTreeWidgetItem(F_doc);
  F_namefilters->setText(0, "Unknown Plugin Name");

  pluginDocs = new QTreeWidgetItem(F_doc);
  pluginDocs->setText(0, "Unknown Plugin Name");

  htmlDoc = new QTreeWidgetItem(F_namefilters);
  htmlDoc->setText(0, "Unknown Plugin Name");

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
    outputFileName->setText(QDir::toNativeSeparators(outputFile));
    m_OpenDialogLastDirectory = outputFile;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PluginMaker::on_generateButton_clicked()
{
  /*
  if (this->sourceDirectoryLE->text().isEmpty() == true)
      {
        QMessageBox::critical(this, tr("Input Parameter Error"), tr("Source Directory must be set."), QMessageBox::Ok);
        return;
      }
  */
  QString pluginName = name->text();
  QString pluginDir = outputFileName->text();

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

  processFile(":/Template/CMakeLists.txt.in");
  processFile(":/Template/Code/Plugin.h.in");
  processFile(":/Template/Code/Plugin.cpp.in");
  processFile(":/Template/Code/Filter/Filter.cpp.in");
  processFile(":/Template/Code/Filter/Filter.h.in");
  processFile(":/Template/Code/Filter/SourceList.cmake.in");
  processFile(":/Template/Documentation/FilterDocs.qrc.in");
  processFile(":/Template/Documentation/Filter/Documentation.html.in");
  statusbar->showMessage("Generation Completed");
}

void PluginMaker::processFile(QString path) {
  //Get text field values from widget
  QString pluginName = name->text();
  QString pluginDir = outputFileName->text();

   pluginName = cleanName(pluginName);

  //Open file
  QFile rfile(path);
  if ( rfile.open(QIODevice::ReadOnly | QIODevice::Text) ) {
    QTextStream in(&rfile);
    QString text = in.readAll();
    text.replace("@PluginName@", pluginName);

    pluginDir.append("/" + pluginName);
    pluginDir.append(path.remove(0,10));
    pluginDir.remove(pluginDir.size()-3,3);
    pluginDir = QDir::toNativeSeparators(pluginDir);

    //Find the path to the parent folder that contains the file, and create it
    QFileInfo fInfo(pluginDir);
    QString parentPath = fInfo.path();
    QString file = fInfo.fileName();

    //Various necessary methods to create the correct file names and paths.
    if (file == "Filter.cpp" || file == "Filter.h" || file == "SourceList.cmake") {
      parentPath.remove(parentPath.size()-6,6);
      parentPath.append(pluginName);
    }
    else if (file == "Documentation.html") {
      parentPath.insert(parentPath.size()-6,pluginName);
      parentPath.append('s');
      file = pluginName + "Filter.html";
    }

    parentPath = QDir::toNativeSeparators(parentPath);
    QDir parentDir(parentPath);
    parentDir.mkpath(parentPath);

    //More methods to create the proper file names and paths
    if (file == "FilterDocs.qrc") {
      file.remove(file.size()-14,6);
      file.insert(file.size()-8,"Plugin");
    }

    if (file != "SourceList.cmake" && file != "CMakeLists.txt" && file != (pluginName + "Filter.html") ) {
      file.push_front(pluginName);
    }

    pluginDir = parentPath.append("/" + file);
    pluginDir = QDir::toNativeSeparators(pluginDir);

    //Write to file
    QFile f(pluginDir);
    if ( f.open(QIODevice::WriteOnly | QIODevice::Text) ) {
      QTextStream out(&f);
      out << text;
    }
  }
}

QString PluginMaker::cleanName(QString name) {
  //Remove all spaces and illegal characters from plugin name
  name = name.trimmed();
  name = name.remove(" ");
  return name;
}

void PluginMaker::on_name_textChanged(const QString & text) {
  statusbar->showMessage("Ready");
  QString pluginName = name->text();
  QString pluginDir = outputFileName->text();

  pluginName = cleanName(pluginName);

  if (pluginName == "") {
    F_main->setText(0, "Unknown Plugin Name");
    F_name->setText(0, "Unknown Plugin Name");
    plugincpp->setText(0, "Unknown Plugin Name");
    pluginh->setText(0, "Unknown Plugin Name");
    filtercpp->setText(0, "Unknown Plugin Name");
    filterh->setText(0, "Unknown Plugin Name");
    F_namefilters->setText(0, "Unknown Plugin Name");
    pluginDocs->setText(0, "Unknown Plugin Name");
    htmlDoc->setText(0, "Unknown Plugin Name");
  }
  else {
  F_main->setText(0, pluginName);
  F_name->setText(0, pluginName);
  plugincpp->setText(0, pluginName + "Plugin.cpp");
  pluginh->setText(0, pluginName + "Plugin.h");
  filtercpp->setText(0, pluginName + "Filter.cpp");
  filterh->setText(0, pluginName + "Filter.h");
  F_namefilters->setText(0, pluginName + "Filters");
  pluginDocs->setText(0, pluginName + "PluginDocs.qrc");
  htmlDoc->setText(0, pluginName + "Filter.html");
  }
}

void PluginMaker::on_outputFileName_textChanged(const QString & text) {
  statusbar->showMessage("Ready");
}
<<<<<<< HEAD

void PluginMaker::on_helpButton_clicked() {
  HelpWidget* helpDialog = new HelpWidget;
  helpDialog->show();
}

void PluginMaker::on_aboutButton_clicked() {
  AboutWidget* aboutDialog = new AboutWidget;
  aboutDialog->show();
}