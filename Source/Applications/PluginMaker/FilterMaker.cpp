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

#include "FilterMaker.h"

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <QtCore/QTextStream>

#include "PluginMaker/FilterBundler.h"
#include "PluginMaker/PMDirGenerator.h"
#include "PluginMaker/PMFileGenerator.h"
#include "PluginMaker/PMFilterGenerator.h"
#include "PluginMaker/PMGeneratorTreeItem.h"

#include "QtSupport/ApplicationFileInfo.h"

#include <iostream>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterMaker::FilterMaker(QWidget* parent) :
QMainWindow(parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterMaker::~FilterMaker()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::setupGui()
{
  // Setup error string
  errorString->setTextFormat(Qt::PlainText);
  errorString->setTextInteractionFlags(Qt::NoTextInteraction);
  errorString->changeStyleSheet(FS_DOESNOTEXIST_STYLE);
  errorString->setText("");

  generateBtn->setEnabled(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FilterMaker::isPublic()
{
  return publicFilter->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMaker::getFilterName()
{
  return (filterName->text());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMaker::getPluginDir()
{
  return (pluginDir->text());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::on_pluginDir_textChanged(const QString& text)
{
  // Check whether or not we need to show an error and disable the Generate button.
  validityCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::on_filterName_textChanged(const QString& text)
{
  // Check whether or not we need to show an error and disable the Generate button.
  validityCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::on_selectBtn_clicked()
{
  QString pluginPath = QFileDialog::getExistingDirectory(this,
    tr("Select Plugin Folder"),
    m_OpenDialogLastDirectory,
    QFileDialog::ShowDirsOnly);

  if (pluginPath.isEmpty() == false)
  {
    pluginDir->setText(pluginPath);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::on_codeChooser_currentIndexChanged(const QString &text)
{
  
}

// -----------------------------------------------------------------------------
//  Called when the OK button is clicked.
// -----------------------------------------------------------------------------
void FilterMaker::on_generateBtn_clicked()
{
  QString filterName = this->filterName->text();

  // Generate the implementation, header, and test files
  generateFilterFiles();

  // Add to the SourceList.cmake file
  updateSourceList();

  // Add to the Test Locations file
  updateTestLocations();

  // Add to the CMakeLists.txt file in the Test folder
  updateTestList();

  statusbar->showMessage("'" + filterName + "' Generation Completed");
}

// -----------------------------------------------------------------------------
//  Called when the Cancel button is clicked.
// -----------------------------------------------------------------------------
void FilterMaker::on_cancelBtn_clicked()
{
  emit cancelBtnPressed();
  this->close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::on_addFilterParameterBtn_clicked()
{
  AddFilterParameter* dialog = new AddFilterParameter();

  connect(dialog, SIGNAL(addBtnPressed(AddFilterParameter*)), this, SLOT(addFilterParameterToTable(AddFilterParameter*)));

  dialog->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::on_removeFilterParameterBtn_clicked()
{
  int row = filterParametersTable->currentRow();
  filterParametersTable->removeRow(row);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::addFilterParameterToTable(AddFilterParameter* widget)
{
  QString varName = widget->getVariableName();
  QString humanName = widget->getHumanName();
  QString type = widget->getType();

  // Close the widget, since we have all the data that we need
  widget->close();

  QTableWidgetItem* item0 = new QTableWidgetItem(varName);
  QTableWidgetItem* item1 = new QTableWidgetItem(humanName);
  QTableWidgetItem* item2 = new QTableWidgetItem(type);

  // Insert items
  int row = filterParametersTable->rowCount();
  filterParametersTable->insertRow(row);
  filterParametersTable->setItem(row, VAR_NAME, item0);
  filterParametersTable->setItem(row, HUMAN_NAME, item1);
  filterParametersTable->setItem(row, TYPE, item2);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::on_errorString_linkActivated(const QString &link)
{
  QMessageBox msgBox;
  msgBox.setWindowTitle("Naming Restrictions");
  msgBox.setText("Names can only contain letters, numbers, underscores, and dashes.");
  msgBox.setInformativeText("No special characters or spaces allowed due to file system restrictions.");
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.setDefaultButton(QMessageBox::Ok);
  msgBox.exec();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::generateFilterFiles()
{
  QString filterName = this->filterName->text();
  QString pluginDir = this->pluginDir->text();

  if (pluginDir.isEmpty() || filterName.isEmpty())
  {
    return;
  }

  QFileInfo fi(pluginDir);

  // Filter.cpp file
  QString pathTemplate = "@PluginName@Filters/";
  QString resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Filter/Filter.cpp.in");
  PMFilterGenerator* cppgen = new PMFilterGenerator(pluginDir,
    pathTemplate,
    QString(filterName + ".cpp"),
    QString(filterName),
    resourceTemplate,
    NULL,
    this);

  connect(cppgen, SIGNAL(outputError(const QString&)),
    this, SLOT(generationError(const QString&)));
  cppgen->setDoesGenerateOutput(true);
  cppgen->setPluginName(fi.baseName());
  cppgen->generateOutput();

  // Filter.h file
  pathTemplate = "@PluginName@Filters/";
  resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Filter/Filter.h.in");
  PMFilterGenerator* hgen = new PMFilterGenerator(pluginDir,
    pathTemplate,
    QString(filterName + ".h"),
    QString(filterName),
    resourceTemplate,
    NULL,
    this);

  connect(hgen, SIGNAL(outputError(const QString&)),
    this, SLOT(generationError(const QString&)));
  hgen->setDoesGenerateOutput(true);
  hgen->setPluginName(fi.baseName());
  hgen->generateOutput();


  // Documentation.md file
  pathTemplate = "Documentation/@PluginName@Filters/";
  resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Documentation/Filter/Documentation.md.in");
  PMFilterGenerator* htmlgen = new PMFilterGenerator(pluginDir,
    pathTemplate,
    QString(filterName + ".md"),
    QString(filterName),
    resourceTemplate,
    NULL,
    this);

  connect(htmlgen, SIGNAL(outputError(const QString&)),
    this, SLOT(generationError(const QString&)));
  htmlgen->setDoesGenerateOutput(true);
  htmlgen->setPluginName(fi.baseName());
  htmlgen->generateOutput();

  // FilterTest.cpp file
  pathTemplate = "Test";
  resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Test/FilterTest.cpp.in");
  PMFilterGenerator* testgen = new PMFilterGenerator(pluginDir,
    pathTemplate,
    QString(filterName + "Test.cpp"),
    QString(filterName),
    resourceTemplate,
    NULL,
    this);

  connect(testgen, SIGNAL(outputError(const QString&)),
    this, SLOT(generationError(const QString&)));
  testgen->setDoesGenerateOutput(true);
  testgen->setPluginName(fi.baseName());
  testgen->generateOutput();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::updateSourceList()
{
  QString filterName = this->filterName->text();
  QString pluginDir = this->pluginDir->text();

  QString pluginName = QFileInfo(pluginDir).baseName();

  QString sourceListPath = pluginDir + "/" + pluginName + "Filters/SourceList.cmake";
  sourceListPath = QDir::toNativeSeparators(sourceListPath);

  QFile file(sourceListPath);
  if (file.open(QIODevice::ReadOnly))
  {
    QByteArray byteArray = file.readAll();
    file.close();

    QString fileData(byteArray);
    QString str = "";
    if (isPublic() == true)
    {
      str = "set(_PublicFilters";
    }
    else
    {
      str = "set(_PrivateFilters";
    }

    int index = fileData.indexOf(str);
    index = index + str.size();
    fileData.insert(index, "\n  " + filterName);

    file.remove();
    if (file.open(QIODevice::WriteOnly))
    {
      file.write(fileData.toStdString().c_str());
      file.close();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::updateTestLocations()
{
  QString filterName = this->filterName->text();
  QString pluginDir = this->pluginDir->text();

  QString testPath = pluginDir + "/Test/TestFileLocations.h.in";
  testPath = QDir::toNativeSeparators(testPath);

  QFile file(testPath);
  if (file.open(QIODevice::ReadOnly))
  {
    QByteArray byteArray = file.readAll();
    file.close();

    QString fileData(byteArray);
    QString str = "const QString DREAM3DProjDir(\"@DREAM3DProj_SOURCE_DIR@\");";
    int index = fileData.indexOf(str);
    index = index + str.size();
    QString namespaceStr = createNamespaceString();
    fileData.insert(index, namespaceStr);

    file.remove();
    if (file.open(QIODevice::WriteOnly))
    {
      file.write(fileData.toStdString().c_str());
      file.close();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::updateTestList()
{
  QString filterName = this->filterName->text();
  QString pluginDir = this->pluginDir->text();

  QString testPath = pluginDir + "/Test/CMakeLists.txt";
  testPath = QDir::toNativeSeparators(testPath);

  QFile file(testPath);
  if (file.open(QIODevice::ReadOnly))
  {
    QByteArray byteArray = file.readAll();
    file.close();

    QString fileData(byteArray);
    QString str = "set(${PROJECT_NAME}_Link_Libs Qt5::Core H5Support DREAM3DLib)";
    int index = fileData.indexOf(str);
    if (index == -1)
    {
      QString str2 = "configure_file(${${PROJECT_NAME}_SOURCE_DIR}/TestFileLocations.h.in\n";
      str2.append("               ");
      str2.append("${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}FileLocations.h @ONLY IMMEDIATE)");
      int index2 = fileData.indexOf(str2);
      index2 = index2 + str2.size();
      QString addition = "\n\nset(${PROJECT_NAME}_Link_Libs Qt5::Core H5Support DREAM3DLib)";
      fileData.insert(index2, addition);
    }

    str = "set(${PROJECT_NAME}_Link_Libs Qt5::Core H5Support DREAM3DLib)";
    index = fileData.indexOf(str);
    index = index + str.size();
    QString addition = "\n\nAddDREAM3DUnitTest(TESTNAME " + filterName + "Test SOURCES ";
    addition.append("${${PROJECT_NAME}_SOURCE_DIR}/" + filterName + "Test.cpp LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})");
    fileData.insert(index, addition);

    file.remove();
    if (file.open(QIODevice::WriteOnly))
    {
      file.write(fileData.toStdString().c_str());
      file.close();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMaker::createNamespaceString()
{
  QString filterName = this->filterName->text();

  QString addition = "\n\n";
  addition.append("  namespace " + filterName + "Test");
  addition.append("\n  {\n");
  addition.append("   const QString TestFile1(\"@TEST_TEMP_DIR@/TestFile1.txt\");\n");
  addition.append("   const QString TestFile2(\"@TEST_TEMP_DIR@/TestFile2.txt\");");
  addition.append("\n  }");

  return addition;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::validityCheck()
{
  QString filterName = this->filterName->text();
  QString pluginDir = this->pluginDir->text();

  QString pluginPath = QDir::toNativeSeparators(pluginDir);

  // Store the last used directory into the private instance variable
  QFileInfo pluginPathInfo(pluginPath);
  m_OpenDialogLastDirectory = pluginPathInfo.path();

  QString filtersDir = pluginPath;
  QTextStream ss(&filtersDir);
  QString lastDir = pluginPathInfo.baseName();
  ss << "/" << lastDir << "Filters";
  filtersDir = QDir::toNativeSeparators(filtersDir);

  QFileInfo filtersDirInfo(filtersDir);

  if (filterName.isEmpty() == false && filterName.contains(QRegExp("[^a-zA-Z_-/\s]")) == false
    && filterName.contains(QRegExp("(Filter|Plugin)$")) == false && pluginDir.isEmpty() == false
    && filtersDirInfo.exists() == true)
  {
    // No Errors
    generateBtn->setEnabled(true);
    errorString->setText("");
    errorString->setTextFormat(Qt::PlainText);
    errorString->setTextInteractionFlags(Qt::NoTextInteraction);
  }
  else
  {
    // There is an error, so disable the button and hide the error message until we know what the message is going to be.
    generateBtn->setEnabled(false);
    errorString->setText("");
    errorString->setTextFormat(Qt::PlainText);
    errorString->setTextInteractionFlags(Qt::NoTextInteraction);

    if (filterName.contains(QRegExp("[^a-zA-Z_-/\s]")) == true)
    {
      // Filter name has illegal characters
      QString linkText = "<a href=#openWindow>Learn More.</a>";
      errorString->setText("The name that you chose has illegal characters. " + linkText);
      errorString->setTextFormat(Qt::RichText);
      errorString->setTextInteractionFlags(Qt::TextBrowserInteraction);
    }
    else if (filterName.contains(QRegExp("(Filter|Plugin)$")) == true)
    {
      // Filter name has "Filter" or "Plugin" at the end of the name
      generateBtn->setEnabled(false);
      errorString->setText("Filter names cannot contain the words 'Filter' or 'Plugin' at the end of the name.\nPlease choose a different filter name.");
    }
    else if (filtersDirInfo.exists() == false)
    {
      // The directory is not a specified plugin directory
      generateBtn->setEnabled(false);
      errorString->setText("The specified directory is not a valid plugin directory.\nPlease select a valid plugin directory.");
    }
  }
}



