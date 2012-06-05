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

PluginMaker::PluginMaker(QWidget* parent) :
 QMainWindow(parent) 
{
  setupUi(this);

  setupGui();

  
}



void PluginMaker::setupGui()
{

}

void PluginMaker::on_selectButton_clicked() {
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Directory"), outputFile);
  if (!outputFile.isNull())
  {
    outputFileName->setText(QDir::toNativeSeparators(outputFile));
    m_OpenDialogLastDirectory = outputFile;
  }
}

void PluginMaker::on_generateButton_clicked() {
  QString pluginName = name->text();
  QString pluginDir = outputFileName->text();
  processFile(":/Template/CMakeLists.txt.in");
  processFile(":/Template/Code/Plugin.h.in");
  processFile(":/Template/Code/Plugin.cpp.in");
  processFile(":/Template/Code/Filter/Filter.cpp.in");
  processFile(":/Template/Code/Filter/Filter.h.in");
  processFile(":/Template/Code/Filter/SourceList.cmake.in");
  processFile(":/Template/Documentation/FilterDocs.qrc.in");
  processFile(":/Template/Documentation/Filter/Documentation.html.in");
}

void PluginMaker::processFile(QString path) {
  //Get text field values from widget
  QString pluginName = name->text();
  QString pluginDir = outputFileName->text();
  
  cleanName(pluginName);

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

void PluginMaker::cleanName(QString name) {
  //Remove all spaces and illegal characters from plugin name
  name = name.trimmed();
  name.remove(" ");
  name.remove( QRegExp("[" + QRegExp::escape("\\/:@#$%^&!(){}';`~.,+=*?\"<>-|") + "]") );
}

void PluginMaker::on_name_textChanged(const QString & text) {
  if ( name->text().isEmpty() ) {
    errorMessage->setText("");
    return;
  }

  //Get text field values from widget
  QString pluginName = name->text();
  QString pluginDir = outputFileName->text();

  //Remove all spaces and illegal characters from plugin name
  cleanName(pluginName);

  errorMessage->setText("Plugin Name: " + pluginName);
}

void PluginMaker::on_outputFileName_textChanged(const QString & text) {
  
}