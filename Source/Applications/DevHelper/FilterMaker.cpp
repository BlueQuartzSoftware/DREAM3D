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

#include <iostream>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <QtCore/QTextStream>

#include "DevHelper/FilterBundler.h"
#include "DevHelper/PMDirGenerator.h"
#include "DevHelper/PMGeneratorTreeItem.h"
#include "DevHelper/CodeGenerators/CodeGenFactory.h"
#include "DevHelper/CodeGenerators/FPCodeGenerator.h"

#include "QtSupportLib/ApplicationFileInfo.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_FilterMaker.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterMaker::FilterMaker(QWidget* parent) :
  QWidget(parent),
  cppGenerator(NULL),
  hGenerator(NULL),
  htmlGenerator(NULL),
  testGenerator(NULL)
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
  errorString->changeStyleSheet(QFSDropLabel::FS_DOESNOTEXIST_STYLE);
  errorString->setText("");

  // Stretch Factors
  splitter->setStretchFactor(0, 0);
  splitter->setStretchFactor(1, 1);

  generateBtn->setEnabled(false);

  // Populate the code viewer
  on_codeChooser_currentIndexChanged(codeChooser->currentIndex());
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
  // Update the code viewer
  on_codeChooser_currentIndexChanged(codeChooser->currentIndex());
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
void FilterMaker::on_codeChooser_currentIndexChanged(int index)
{
  if (validityCheck() == false)
  {
    codeViewer->clear();
    return;
  }

  // Update generators with new information from table
  updateFilterFileGenerators();

  if (index == H_INDEX)
  {
    codeViewer->setText(hGenerator->generateFileContents());
  }
  else if (index == CPP_INDEX)
  {
    codeViewer->setText(cppGenerator->generateFileContents());
  }
  else if (index == DOC_INDEX)
  {
    codeViewer->setText(htmlGenerator->generateFileContents());
  }
}

// -----------------------------------------------------------------------------
//  Called when the OK button is clicked.
// -----------------------------------------------------------------------------
void FilterMaker::on_generateBtn_clicked()
{
  QString filterName = this->filterName->text();

  // Update all filter file generators with information from table
  updateFilterFileGenerators();

  // Generate the implementation, header, and test files
  generateFilterFiles();

  // Add to the SourceList.cmake file
  updateSourceList();

  // Add to the Test Locations file
  updateTestLocations();

  // Add to the CMakeLists.txt file in the Test folder
  updateTestList();

  emit updateStatusBar("'" + filterName + "' Generation Completed");
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

  // Update the filter file generators with the new information
  updateFilterFileGenerators();

  // Show the new code in the code viewer
  on_codeChooser_currentIndexChanged(codeChooser->currentIndex());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::on_filterParametersTable_itemChanged(QTableWidgetItem* item)
{
  // Update the filter file generators with the new information
  updateFilterFileGenerators();

  // Show the new code in the code viewer
  on_codeChooser_currentIndexChanged(codeChooser->currentIndex());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::addFilterParameterToTable(AddFilterParameter* widget)
{
  QString varName = widget->getVariableName();
  QString humanName = widget->getHumanName();
  QString type = widget->getType();
  QString category = widget->getCategory();
  QString initValue = widget->getInitValue();

  // Close the widget, since we have all the data that we need
  widget->close();

  QTableWidgetItem* varNameItem = new QTableWidgetItem(varName);
  QTableWidgetItem* humanNameItem = new QTableWidgetItem(humanName);
  QTableWidgetItem* typeItem = new QTableWidgetItem(type);
  QTableWidgetItem* categoryItem = new QTableWidgetItem(category);
  QTableWidgetItem* initValueItem = new QTableWidgetItem(initValue);

  // Insert items
  int row = filterParametersTable->rowCount();
  filterParametersTable->insertRow(row);

  if (type == "SeparatorWidget")
  {
    varNameItem->setFlags(Qt::NoItemFlags);
    initValueItem->setFlags(Qt::NoItemFlags);
  }

  // Block type and category cells from being edited
  typeItem->setFlags(Qt::NoItemFlags);
  categoryItem->setFlags(Qt::NoItemFlags);

  filterParametersTable->blockSignals(true);
  filterParametersTable->setItem(row, VAR_NAME, varNameItem);
  filterParametersTable->setItem(row, HUMAN_NAME, humanNameItem);
  filterParametersTable->setItem(row, TYPE, typeItem);
  filterParametersTable->setItem(row, CATEGORY, categoryItem);
  filterParametersTable->setItem(row, INIT_VALUE, initValueItem);
  filterParametersTable->blockSignals(false);

  // Update the filter file generators with the new information
  updateFilterFileGenerators();

  // Show the new code in the code viewer
  on_codeChooser_currentIndexChanged(codeChooser->currentIndex());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::on_errorString_linkActivated(const QString& link)
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
void FilterMaker::updateFilterFileGenerators()
{
  QString filterName = this->filterName->text();
  QString pluginDirText = this->pluginDir->text();

  if (pluginDirText.isEmpty() || filterName.isEmpty())
  {
    return;
  }

  QFileInfo fi(pluginDirText);

  QMap<QString, QString> contentsMap = getFunctionContents();

  // Filter.cpp file
  QString pathTemplate = "@PluginName@Filters/";
  QString resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Filter/Filter.cpp.in");

  if (NULL != cppGenerator)
  {
    delete cppGenerator;
    cppGenerator = NULL;
  }
  cppGenerator = new PMFileGenerator(pluginDirText,
                                     pathTemplate,
                                     QString(filterName + ".cpp"),
                                     resourceTemplate,
                                     NULL,
                                     this);

  connect(cppGenerator, SIGNAL(outputError(const QString&)),
          this, SLOT(generationError(const QString&)));
  cppGenerator->setDoesGenerateOutput(true);
  cppGenerator->setPluginName(fi.baseName());
  cppGenerator->setFilterName(filterName);

  if (contentsMap.size() > 0)
  {
    cppGenerator->setSetupFPContents(contentsMap["Setup Filter Parameters"]);
    cppGenerator->setReadFPContents(contentsMap["Read Filter Parameters"]);
    cppGenerator->setWriteFPContents(contentsMap["Write Filter Parameters"]);
    cppGenerator->setInitListContents(contentsMap["Initialization List"]);
    cppGenerator->setFilterCPPIncludesContents(contentsMap["Filter Implementation Includes"]);
  }
  else
  {
    cppGenerator->setSetupFPContents(getDefaultSetupFPContents());
    cppGenerator->setReadFPContents(getDefaultReadFPContents());
    cppGenerator->setWriteFPContents(getDefaultWriteFPContents());
    cppGenerator->setInitListContents(getDefaultInitListContents());
    cppGenerator->setFilterCPPIncludesContents(getDefaultFilterCPPIncludesContents());
  }

  // Filter.h file
  pathTemplate = "@PluginName@Filters/";
  resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Filter/Filter.h.in");

  if (NULL != hGenerator)
  {
    delete hGenerator;
    hGenerator = NULL;
  }
  hGenerator = new PMFileGenerator(pluginDirText,
                                   pathTemplate,
                                   QString(filterName + ".h"),
                                   resourceTemplate,
                                   NULL,
                                   this);

  connect(hGenerator, SIGNAL(outputError(const QString&)),
          this, SLOT(generationError(const QString&)));
  hGenerator->setDoesGenerateOutput(true);
  hGenerator->setPluginName(fi.baseName());
  hGenerator->setFilterName(filterName);

  if (contentsMap.size() > 0)
  {
    hGenerator->setFPContents(contentsMap["Filter Parameters"]);
    hGenerator->setFilterHIncludesContents(contentsMap["Filter Header Includes"]);
  }
  else
  {
    hGenerator->setFPContents(getDefaultFPContents());
    hGenerator->setFilterHIncludesContents(getDefaultFilterHIncludesContents());
  }


  // Documentation.md file
  pathTemplate = "Documentation/@PluginName@Filters/";
  resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Documentation/Filter/Documentation.md.in");

  if (NULL != htmlGenerator)
  {
    delete htmlGenerator;
    htmlGenerator = NULL;
  }
  htmlGenerator = new PMFileGenerator(pluginDirText,
                                      pathTemplate,
                                      QString(filterName + ".md"),
                                      resourceTemplate,
                                      NULL,
                                      this);

  connect(htmlGenerator, SIGNAL(outputError(const QString&)),
          this, SLOT(generationError(const QString&)));
  htmlGenerator->setDoesGenerateOutput(true);
  htmlGenerator->setPluginName(fi.baseName());
  htmlGenerator->setFilterName(filterName);

  // FilterTest.cpp file
  pathTemplate = "Test";
  resourceTemplate = ApplicationFileInfo::GenerateFileSystemPath("/Template/Test/FilterTest.cpp.in");

  if (NULL != testGenerator)
  {
    delete testGenerator;
    testGenerator = NULL;
  }
  testGenerator = new PMFileGenerator(pluginDirText,
                                      pathTemplate,
                                      QString(filterName + "Test.cpp"),
                                      resourceTemplate,
                                      NULL,
                                      this);

  connect(testGenerator, SIGNAL(outputError(const QString&)),
          this, SLOT(generationError(const QString&)));
  testGenerator->setDoesGenerateOutput(true);
  testGenerator->setPluginName(fi.baseName());
  testGenerator->setFilterName(filterName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::generateFilterFiles()
{
  // Generate all the output
  cppGenerator->generateOutput();
  hGenerator->generateOutput();
  htmlGenerator->generateOutput();
  testGenerator->generateOutput();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, QString> FilterMaker::getFunctionContents()
{
  QMap<QString, QString> map;

  if (filterParametersTable->rowCount() <= 0)
  {
    return map;
  }

  QString setupFPContents = "";
  QString readFPContents = "";
  QString writeFPContents = "";
  QString dataCheckContents = "";
  QString FPContents = "";
  QString initListContents = "  AbstractFilter(),\n";
  QString filterHIncludes = "";
  QString filterCPPIncludes = "";

  CodeGenFactory::Pointer factory = CodeGenFactory::New();
  for (int row = 0; row < filterParametersTable->rowCount(); row++)
  {
    QString propertyName = filterParametersTable->item(row, VAR_NAME)->text();
    QString humanName = filterParametersTable->item(row, HUMAN_NAME)->text();
    QString type = filterParametersTable->item(row, TYPE)->text();
    QString category = filterParametersTable->item(row, CATEGORY)->text();
    QString initValue = filterParametersTable->item(row, INIT_VALUE)->text();

    FPCodeGenerator::Pointer generator = factory->create(humanName, propertyName, type, category, initValue);
    if (generator->generateSetupFilterParameters().isEmpty() == false)
    {
      setupFPContents.append(generator->generateSetupFilterParameters() + "\n");
    }

    if (generator->generateReadFilterParameters().isEmpty() == false)
    {
      readFPContents.append(generator->generateReadFilterParameters() + "\n");
    }

    if (generator->generateWriteFilterParameters().isEmpty() == false)
    {
      writeFPContents.append(generator->generateWriteFilterParameters() + "\n");
    }

    if (generator->generateDataCheck().isEmpty() == false)
    {
      dataCheckContents.append(generator->generateDataCheck() + "\n");
    }

    if (generator->generateFilterParameters().isEmpty() == false)
    {
      FPContents.append(generator->generateFilterParameters() + "\n\n");
    }

    if (generator->generateInitializationList().isEmpty() == false)
    {
      initListContents.append(generator->generateInitializationList() + "\n");
    }

    if (generator->generateHIncludes().isEmpty() == false)
    {
      filterHIncludes.append(generator->generateHIncludes() + "\n");
    }

    if (generator->generateCPPIncludes().isEmpty() == false)
    {
      filterCPPIncludes.append(generator->generateCPPIncludes() + "\n");
    }
  }

  // Chop off the last, un-needed new-line character from each contents
  setupFPContents.chop(1);
  readFPContents.chop(1);
  writeFPContents.chop(1);
  dataCheckContents.chop(1);
  FPContents.chop(1);
  initListContents.chop(1);
  filterHIncludes.chop(1);
  filterCPPIncludes.chop(1);

  // Chop off last comma from initialization list
  initListContents.chop(1);

  map.insert("Setup Filter Parameters", setupFPContents);
  map.insert("Read Filter Parameters", readFPContents);
  map.insert("Write Filter Parameters", writeFPContents);
  map.insert("Data Check", dataCheckContents);
  map.insert("Filter Parameters", FPContents);
  map.insert("Initialization List", initListContents);
  map.insert("Filter Header Includes", filterHIncludes);
  map.insert("Filter Implementation Includes", filterCPPIncludes);

  return map;
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

  QFile source(sourceListPath);
  source.open(QFile::ReadOnly);
  QString contents = source.readAll();
  source.close();

  QString namespaceStr = createNamespaceString();
  // Check to make sure we don't already have this filter in the namespace
  if (contents.contains(namespaceStr) == true)
  {
    return;
  }
  bool keepGoing = false;
  QStringList outLines;
  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  QString searchString = "set(_PublicFilters";
  if (!isPublic())
  {
    searchString = "set(_PrivateFilters";
  }

  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    if (line.contains(searchString) && !keepGoing)
    {
      outLines.push_back(line);
      while (sourceLines.hasNext())
      {
        line = sourceLines.next();
        if (line.contains(filterName) && !keepGoing)
        {
          outLines.push_back(line);
          keepGoing = true;
        }
        else if (line.contains(")"))
        {
          // We never found the filter name so lets add it back in
          if (!keepGoing)
          {
            outLines.push_back("  " + filterName);
          }
          outLines.push_back(line);
          keepGoing = true;
          break;
        }
        else
        {
          outLines.push_back(line);
        }
      }
    }
    else
    {
      outLines.push_back(line);
    }
  }

  source.remove();
  if (source.open(QIODevice::WriteOnly))
  {
    source.write(outLines.join("\n").toLatin1().data());
    source.close();
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

  QFile source(testPath);
  source.open(QFile::ReadOnly);
  QString contents = source.readAll();
  source.close();

  QString namespaceStr = createNamespaceString();
  // Check to make sure we don't already have this filter in the namespace
  if (contents.contains(namespaceStr) == true)
  {
    return;
  }

  QStringList outLines;
  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  QString searchString = "#endif";

  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    if (line.contains(searchString))
    {
      QString str("namespace UnitTest\n{");
      QTextStream outStream(&str);
      outStream << namespaceStr << "\n";
      outStream << "}\n";
      outLines.push_back(str);
      outLines.push_back(line);
    }
    else
    {
      outLines.push_back(line);
    }
  }

  source.remove();
  if (source.open(QIODevice::WriteOnly))
  {
    source.write(outLines.join("\n").toLatin1().data());
    source.close();
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


  QFile source(testPath);
  source.open(QFile::ReadOnly);
  QString contents = source.readAll();
  source.close();


  QString testCMakeCode;
  QTextStream out(&testCMakeCode);
  out << "AddDREAM3DUnitTest(TESTNAME " << filterName << "Test";
  // Check to make sure we don't already have this filter in the namespace
  if (contents.contains(testCMakeCode) == true)
  {
    return;
  }

  out << " SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/" << filterName << "Test.cpp LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})\n\n";

  QStringList list = contents.split(QRegExp("\\n"));
  list.push_back(testCMakeCode);

  source.remove();
  if (source.open(QIODevice::WriteOnly))
  {
    source.write(list.join("\n").toLatin1().data());
    source.close();
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMaker::createNamespaceString()
{
  QString filterName = this->filterName->text();

  QString addition = "\n";
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
bool FilterMaker::validityCheck()
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

  if (filterName.isEmpty() == false && filterName.contains(QRegExp("[^a-zA-Z_-/\\s]")) == false
      && filterName.contains(QRegExp("(Filter|Plugin)$")) == false && pluginDir.isEmpty() == false
      && filtersDirInfo.exists() == true)
  {
    // No Errors
    generateBtn->setEnabled(true);
    errorString->setText("");
    errorString->setTextFormat(Qt::PlainText);
    errorString->setTextInteractionFlags(Qt::NoTextInteraction);
    return true;
  }
  else
  {
    // There is an error, so disable the button and hide the error message until we know what the message is going to be.
    generateBtn->setEnabled(false);
    errorString->setText("");
    errorString->setTextFormat(Qt::PlainText);
    errorString->setTextInteractionFlags(Qt::NoTextInteraction);

    if (filterName.contains(QRegExp("[^a-zA-Z_-/\\s]")) == true)
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
    else if (pluginDir.isEmpty() == true)
    {
      // The directory is empty
      generateBtn->setEnabled(false);
      errorString->setText("The plugin directory cannot be empty.\nPlease select a valid plugin directory.");
    }
    else if (filtersDirInfo.exists() == false)
    {
      // The directory is not a specified plugin directory
      generateBtn->setEnabled(false);
      errorString->setText("The specified directory is not a valid plugin directory.\nPlease select a valid plugin directory.");
    }
    else if (filterName.isEmpty() == true)
    {
      // The directory is empty
      generateBtn->setEnabled(false);
      errorString->setText("The filter name cannot be empty.\nPlease select a valid filter name.");
    }

    return false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMaker::getDefaultSetupFPContents()
{
  QString contents = "";

  //Open file
  QFile file(ApplicationFileInfo::GenerateFileSystemPath("/Template/Contents/SetupFilterParameters.in"));
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream in(&file);
    contents = in.readAll();
  }

  return contents;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMaker::getDefaultReadFPContents()
{
  QString contents = "";

  //Open file
  QFile file(ApplicationFileInfo::GenerateFileSystemPath("/Template/Contents/ReadFilterParameters.in"));
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream in(&file);
    contents = in.readAll();
  }

  return contents;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMaker::getDefaultWriteFPContents()
{
  QString contents = "";

  //Open file
  QFile file(ApplicationFileInfo::GenerateFileSystemPath("/Template/Contents/WriteFilterParameters.in"));
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream in(&file);
    contents = in.readAll();
  }

  return contents;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMaker::getDefaultDataCheckContents()
{
  QString contents = "";

  //Open file
  QFile file(ApplicationFileInfo::GenerateFileSystemPath("/Template/Contents/DataCheck.in"));
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream in(&file);
    contents = in.readAll();
  }

  return contents;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMaker::getDefaultFPContents()
{
  QString contents = "";

  //Open file
  QFile file(ApplicationFileInfo::GenerateFileSystemPath("/Template/Contents/Q_PROPERTY_FILTER_PARAMETER.in"));
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream in(&file);
    contents = in.readAll();
  }

  return contents;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMaker::getDefaultInitListContents()
{
  QString contents = "";

  //Open file
  QFile file(ApplicationFileInfo::GenerateFileSystemPath("/Template/Contents/InitList.in"));
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream in(&file);
    contents = in.readAll();
  }

  return contents;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMaker::getDefaultFilterHIncludesContents()
{
  QString contents = "";

  //Open file
  QFile file(ApplicationFileInfo::GenerateFileSystemPath("/Template/Contents/FilterHIncludes.in"));
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream in(&file);
    contents = in.readAll();
  }

  return contents;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FilterMaker::getDefaultFilterCPPIncludesContents()
{
  QString contents = "";

  //Open file
  QFile file(ApplicationFileInfo::GenerateFileSystemPath("/Template/Contents/FilterCPPIncludes.in"));
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QTextStream in(&file);
    contents = in.readAll();
  }

  return contents;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterMaker::generationError(const QString& test)
{

}






