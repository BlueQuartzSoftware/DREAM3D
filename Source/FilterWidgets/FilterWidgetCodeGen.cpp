/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdio.h>

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MD5.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/FilterParameter.h"
#include "DREAM3DLib/Common/CreatedArrayHelpIndexEntry.h"




// Enabling this will create a pair of files that can be used to update
// the "PreflightTest.cpp" unit test
#define GENERATE_PREFLIGHT_TEST_CODE_FRAGMENT 0
#define GENERATE_FILTER_PARAMTERS_READER_CODE 1



typedef std::map<std::string, std::set<std::string> >  FilterMapType;
typedef std::set<std::string>  StringSetType;

// These will be defined in an include header file below.
std::string FILTER_WIDGETS_BINARY_DIR();
std::string FILTER_WIDGETS_SOURCE_DIR();
std::string FILTER_WIDGETS_TEMP_DIR();
std::string FILTER_WIDGETS_DOCS_DIR();
std::string DREAM3D_SOURCE_DIR();
std::string FILTER_INCLUDE_PREFIX();
std::string DREAM3D_SOURCE_DIR();
std::string DREAM3D_BINARY_DIR();
std::string DREAM3DLIB_SOURCE_DIR();

typedef std::map<std::string, CreatedArrayHelpIndexEntry::VectorType> IndexMap_t;

std::map<std::string, CreatedArrayHelpIndexEntry::VectorType>  helpIndex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void copyFile(const std::string &src, const std::string &dest)
{
  size_t tempFileSize = MXAFileInfo::fileSize(src);

  unsigned char* contents = reinterpret_cast<unsigned char*>(malloc(tempFileSize));
  FILE* f = fopen(src.c_str(), "rb");
  size_t itemsRead = fread(contents, tempFileSize, 1, f);
  if(itemsRead != 1)
  {

  }
  fclose(f);
  f = NULL;

  f = fopen(dest.c_str(), "wb");
  fwrite(contents, tempFileSize, 1, f);
  fclose(f);

  free(contents);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void extractHelpIndexEntries(AbstractFilter* filter)
{
  VoxelDataContainer::Pointer vdc = VoxelDataContainer::New();
  SurfaceMeshDataContainer::Pointer surf = SurfaceMeshDataContainer::New();
  SolidMeshDataContainer::Pointer sol = SolidMeshDataContainer::New();
  filter->setVoxelDataContainer(vdc.get());
  filter->setSurfaceMeshDataContainer(surf.get());
  filter->setSolidMeshDataContainer(sol.get());
  filter->preflight();
  CreatedArrayHelpIndexEntry::VectorType entries = filter->getCreatedArrayHelpIndexEntries();

  for(CreatedArrayHelpIndexEntry::VectorType::iterator entry = entries.begin(); entry != entries.end(); ++entry)
  {
    std::string entryName = (*entry)->getArrayDefaultName();
    entryName = entryName + " (" + (*entry)->getArrayGroup() + ")";
    CreatedArrayHelpIndexEntry::VectorType& vec = helpIndex[entryName]; // Will Create one if not there already
    vec.push_back((*entry));
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void createMarkdownCreatedArrayIndex()
{

  std::string path = DREAM3D_BINARY_DIR();
  path = path + MXADir::Separator + "createdarrayindex.md";

  FILE* f = fopen(path.c_str(), "wb");

  fprintf(f, "Created Array Index {#createdarrayindex}\n======\n");

  for(IndexMap_t::iterator entry = helpIndex.begin(); entry != helpIndex.end(); ++entry)
  {
    std::string name = (*entry).first;
    fprintf (f, "## %s ##\n\n", (*entry).first.c_str());

    CreatedArrayHelpIndexEntry::VectorType& filters = (*entry).second;


    for(CreatedArrayHelpIndexEntry::VectorType::iterator indexEntry = filters.begin(); indexEntry != filters.end(); ++indexEntry)
    {
      std::string lower = (*indexEntry)->getFilterName();
      std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
      fprintf(f, "+ [%s](%s.html) (%s->%s)\n", (*indexEntry)->getFilterHumanLabel().c_str(), lower.c_str(), (*indexEntry)->getFilterGroup().c_str(), (*indexEntry)->getFilterSubGroup().c_str());
    }

    fprintf(f, "\n");
  }
  fclose(f);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void createHeaderFile(const std::string &group, const std::string &filterName, AbstractFilter* filterPtr, const std::string &outputPath)
{
  std::stringstream ss;

  extractHelpIndexEntries(filterPtr);

  std::string completePath = MXADir::toNativeSeparators(outputPath);
  // Make sure the output path exists
  std::string parentPath = MXADir::parentPath(completePath);
  if (MXADir::exists(parentPath) == false)
  {
    MXADir::mkdir(parentPath, true);
  }

  std::vector<FilterParameter::Pointer> options = filterPtr->getFilterParameters();

  ss.str("");
  ss << FILTER_WIDGETS_TEMP_DIR() << "/TEMP_WIDGET.h";
  std::string tempPath = ss.str();

  FILE* f = fopen(tempPath.c_str(), "wb");
  if (NULL == f)
  {
    std::cout << "Could not open file '" << tempPath << "' for writing." << std::endl;
     return;
  }

  fprintf(f, "/*\n");
  fprintf(f, "  This file was auto-generated from the program FilterWidgetCodeGen.cpp which is\n  itself generated during cmake time\n");
  fprintf(f, "  If you need to make changes to the code that is generated you will need to make\n  them in the original file. \n");
  fprintf(f, "  The code generated is based off values from the filter located at\n");
  if (FILTER_INCLUDE_PREFIX().empty() == true) {
    fprintf(f, "  %s/%s.h\n*/\n", group.c_str(), filterName.c_str());
  }
  else
  {
    fprintf(f, "  %s/%s/%s.h\n*/\n", FILTER_INCLUDE_PREFIX().c_str(), group.c_str(), filterName.c_str());
  }

  fprintf(f, "#ifndef _Q%sWidget_H_\n", filterName.c_str());
  fprintf(f, "#define _Q%sWidget_H_\n\n", filterName.c_str());

  fprintf(f, "#include <QtCore/QObject>\n");
  fprintf(f, "#include <QtCore/QSettings>\n");
  fprintf(f, "#include <QtCore/QUrl>\n\n");

  fprintf(f, "#include \"PipelineBuilder/QFilterWidget.h\"\n");
  fprintf(f, "#include \"DREAM3DLib/Common/DREAM3DSetGetMacros.h\"\n");
  fprintf(f, "#include \"DREAM3DLib/Common/FilterParameter.h\"\n\n");
  if (FILTER_INCLUDE_PREFIX().empty() == true) {
    fprintf(f, "#include \"%s/%s.h\"\n", group.c_str(), filterName.c_str());
  }
  else
  {
    fprintf(f, "#include \"%s/%s/%s.h\"\n", FILTER_INCLUDE_PREFIX().c_str(), group.c_str(), filterName.c_str());
  }
  fprintf(f, "\n\nclass Q%sWidget : public QFilterWidget \n{\n", filterName.c_str());
  fprintf(f, "   Q_OBJECT\n");
  fprintf(f, "  public:\n");
  fprintf(f, "    Q%sWidget(QWidget* parent = NULL);\n", filterName.c_str());
  fprintf(f, "    virtual ~Q%sWidget();\n", filterName.c_str());
  fprintf(f, "    virtual AbstractFilter::Pointer getFilter();\n");
  fprintf(f, "    void writeOptions(QSettings &prefs);\n");
  fprintf(f, "    void readOptions(QSettings &prefs);\n\n");
  fprintf(f, "    QFilterWidget* createDeepCopy();\n\n");
  fprintf(f, "    QString getFilterGroup();\n\n");
  fprintf(f, "    QString getFilterSubGroup();\n\n");
  fprintf(f, "    virtual void openHtmlHelpFile();\n\n");

  bool implementArrayNameComboBoxUpdated = false;
  bool implementPreflightAboutToExecute = true;
  int axisAngleWidgetCount = 0;
  // Loop on all the filter options
  for(size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    std::string prop = opt->getPropertyName();
    std::string typ = opt->getValueType();

    if (opt->getCastableValueType().empty() == false)
    {
      std::string cType = opt->getCastableValueType();
      fprintf(f, "  private:\n");
      fprintf(f, "    %s m_%s;\n", cType.c_str(), prop.c_str());
      fprintf(f, "  public:\n");
      fprintf(f, "    Q_PROPERTY(%s %s READ get%s WRITE set%s)\n", cType.c_str(), prop.c_str(), prop.c_str(), prop.c_str());
      fprintf(f, "  public slots:\n");
      fprintf(f, "    void set%s(%s v, bool emitChanged = true);\n", prop.c_str(), cType.c_str());
      fprintf(f, "  public:\n");
      fprintf(f, "    %s  get%s();\n", cType.c_str(), prop.c_str());
    }
    else if (opt->getValueType().compare("string") == 0)
    {
      std::string cType = "QString";
      fprintf(f, "  private:\n");
      fprintf(f, "    QString m_%s;\n", prop.c_str());
      fprintf(f, "  public:\n");
      fprintf(f, "    Q_PROPERTY(%s %s READ get%s WRITE set%s)\n", cType.c_str(), prop.c_str(), prop.c_str(), prop.c_str());
      fprintf(f, "  public slots:\n");
      fprintf(f, "    void set%s(const %s &v, bool emitChanged = true);\n", prop.c_str(), cType.c_str());
      fprintf(f, "  public:\n");
      fprintf(f, "    %s  get%s();\n\n", cType.c_str(), prop.c_str());
    }
    else if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget && implementPreflightAboutToExecute == true)
    {
      fprintf(f, "  public:\n");
      fprintf(f, "    virtual void preflightAboutToExecute(VoxelDataContainer::Pointer vdc, SurfaceMeshDataContainer::Pointer smdc, SolidMeshDataContainer::Pointer sdc);\n");
      fprintf(f, "\n\n");
      implementPreflightAboutToExecute = false;
    }
    else if (opt->getWidgetType() == FilterParameter::IntVec3Widget)
    {
      fprintf(f, "    Q_PROPERTY(IntVec3Widget_t %s READ get%s WRITE set%s)\n", prop.c_str(), prop.c_str(), prop.c_str());
      fprintf(f, "    QFILTERWIDGET_INSTANCE_PROPERTY(IntVec3Widget_t, %s)\n\n", prop.c_str());
    }
    else if (opt->getWidgetType() == FilterParameter::FloatVec3Widget)
    {
      fprintf(f, "    Q_PROPERTY(FloatVec3Widget_t %s READ get%s WRITE set%s)\n", prop.c_str(), prop.c_str(), prop.c_str());
      fprintf(f, "    QFILTERWIDGET_INSTANCE_PROPERTY(FloatVec3Widget_t, %s)\n\n", prop.c_str());
    }
    else if (opt->getWidgetType() == FilterParameter::AxisAngleWidget)
    {
      fprintf(f, "// AxisAngleWidget: Nothing in the header for %s \n", prop.c_str());
      axisAngleWidgetCount++;
      if (axisAngleWidgetCount > 1)
      {
        fprintf(f, "#error You can have only 1 AxisAngleWidget per filter.\n");
      }
    }
    else if (opt->getWidgetType() >= FilterParameter::CellArrayComparisonSelectionWidget
             && opt->getWidgetType() <= FilterParameter::EdgeArrayComparisonSelectionWidget
             && implementPreflightAboutToExecute == true)
    {
      fprintf(f, "  public:\n");
      fprintf(f, "    virtual void preflightAboutToExecute(VoxelDataContainer::Pointer vdc, SurfaceMeshDataContainer::Pointer smdc, SolidMeshDataContainer::Pointer sdc);\n");
      fprintf(f, "\n\n");
      implementPreflightAboutToExecute = false;
    }
    else
    {
      fprintf(f, "    Q_PROPERTY(%s %s READ get%s WRITE set%s)\n", typ.c_str(), prop.c_str(), prop.c_str(), prop.c_str());
      fprintf(f, "    QFILTERWIDGET_INSTANCE_PROPERTY(%s, %s)\n\n", typ.c_str(), prop.c_str());
    }

    if (opt->getWidgetType() >= FilterParameter::VoxelCellArrayNameSelectionWidget
        && opt->getWidgetType() <= FilterParameter::SurfaceMeshEdgeArrayNameSelectionWidget )
    { implementArrayNameComboBoxUpdated = true; }
  }

  if (true == implementArrayNameComboBoxUpdated)
  {
    fprintf(f, "  public:\n    virtual void arrayNameComboBoxUpdated(QComboBox* cb);\n\n");
  }


  fprintf(f, "  private:\n");
  fprintf(f, "    QString m_FilterGroup;\n\n");
  fprintf(f, "    QString m_FilterSubGroup;\n\n");
  fprintf(f, "    Q%sWidget(const Q%sWidget&);\n", filterName.c_str(), filterName.c_str());
  fprintf(f, "    void operator=(const Q%sWidget&);\n", filterName.c_str());
  fprintf(f, "};\n");
  fprintf(f, "#endif /* Q%sWidget_H_ */\n", filterName.c_str());

  fclose(f);

  // Now compare the file just generated with any possible existing file
  size_t currentFileSize = MXAFileInfo::fileSize(completePath);
  size_t tempFileSize = MXAFileInfo::fileSize(tempPath);
  // If the file sizes are different then copy the file
  if (currentFileSize != tempFileSize)
  {
    std::cout << "0-Creating Header File: " <<completePath << std::endl;
    copyFile(tempPath, completePath);
  }
  else // Just because the files are the same size does not mean they are the same.
  {
    FILE* c = fopen(completePath.c_str(), "rb");
    unsigned char* currentContents = reinterpret_cast<unsigned char*>(malloc(currentFileSize));
    size_t itemsRead = fread(currentContents, currentFileSize, 1, c);
    if(itemsRead != 1)
    {

    }
    fclose(c);

    MD5 md5_current;
    md5_current.update(currentContents, currentFileSize);
    md5_current.finalize();
    std::string currentHexDigest = md5_current.hexdigest();


    FILE* t = fopen(tempPath.c_str(), "rb");
    unsigned char* tempContents = reinterpret_cast<unsigned char*>(malloc(tempFileSize));
    itemsRead = fread(tempContents, tempFileSize, 1, t);
    if(itemsRead != 1)
    {

    }
    fclose(t);

    MD5 md5;
    md5.update(tempContents, tempFileSize);
    md5.finalize();
    std::string tempHexDigest = md5.hexdigest();

    // Use MD5 Checksums to figure out if the files are different
    if (tempHexDigest.compare(currentHexDigest) != 0)
    {
      std::cout << "0-Creating Header File: " << completePath << std::endl;
      copyFile(tempPath, completePath);
    }
  }

}

#if GENERATE_FILTER_PARAMTERS_READER_CODE
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t nonPrintables(char* buf, size_t bufSize)
{
  size_t n = 0;
  for (size_t i = 0; i < bufSize; ++i)
  {
    if (buf[i] < 33 && buf[i] > 0) { n++; }
  }
  return n;
}


int readLine(std::istream &in, char* buf, int bufSize)
{

  bool readAnotherLine = true;
  size_t gcount = 0;
  while ( readAnotherLine == true && in.gcount() != 0 && in) {
    // Zero out the buffer
    ::memset(buf, 0, bufSize);
    // Read a line up to a '\n' which will catch windows and unix line endings but
    // will leave a trailing '\r' at the end of the string
    in.getline(buf, bufSize, '\n');
    gcount = in.gcount();
    if (gcount > 1 && buf[in.gcount()-2] == '\r')
    {
      buf[in.gcount()-2] = 0;
    }
    size_t len = strlen(buf);
    size_t np = nonPrintables(buf, bufSize);
    if (len != np)
    {
      readAnotherLine = false;
    }

  }
  return static_cast<int>(in.gcount());
}



#define kBufferSize 1024
void parseSouceFileForMarker(const std::string filename, const std::string marker)
{

  std::string tempfile = filename + "_tmp";
  std::ofstream out(tempfile.c_str(), std::ios_base::binary);

  std::cout << filename << std::endl;
  std::ifstream instream;
  instream.open(filename.c_str(), std::ios_base::binary);
  if (!instream.is_open())
  {
    std::stringstream ss;
    std::cout << " file could not be opened: " << filename << std::endl;
    return;
  }

  char buf[kBufferSize];
  ::memset(buf, 0, kBufferSize);
  size_t gcount = 0;
  while( instream.getline(buf, kBufferSize, '\n') )
  {
    gcount = instream.gcount();
    if (gcount > 1 && buf[instream.gcount()-2] == '\r')
    {
      buf[instream.gcount()-2] = 0;
    }
    if (marker.compare(buf) == 0)
    {
      out << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
    }
    else
    {
      out << std::string(buf) << std::endl;
    }
  }

  MXADir::remove(tempfile);

}


#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void createSourceFile( const std::string &group,
                       const std::string &filter,
                       std::vector<FilterParameter::Pointer> options,
                       const std::string &outputPath)
{
  std::stringstream ss;

  ss << DREAM3DLIB_SOURCE_DIR() << "/" << group << "/" << filter << ".cpp";
  parseSouceFileForMarker(ss.str(), "////!!##");



  std::string completePath = MXADir::toNativeSeparators(outputPath);
  // Make sure the output path exists
  std::string parentPath = MXADir::parentPath(completePath);
  if (MXADir::exists(parentPath) == false)
  {
    MXADir::mkdir(ss.str(), true);
  }

  //std::string headerFile = parentPath + MXADir::Separator + MXAFileInfo::fileNameWithOutExtension(completePath) + ".h";

  ss.str("");
  ss << FILTER_WIDGETS_TEMP_DIR() << "/TEMP_WIDGET.cpp";
  std::string tempPath = ss.str();

  FILE* f = fopen(tempPath.c_str(), "wb");

  bool implementArrayNameComboBoxUpdated = false;
  bool implementArrayNameSelectionWidget = false;
  bool implementComparisonSelectionWidget = false;
  bool implementAxisAngleWidget = false;
  fprintf(f, "/*\n");
  fprintf(f, "* This file was auto-generated from the program FilterWidgetCodeGen.cpp which is\n  itself generated during cmake time\n");
  fprintf(f, "* If you need to make changes to the code that is generated you will need to make\n  them in the original file. \n");
  fprintf(f, "* The code generated is based off values from the filter located at\n");
  if(FILTER_INCLUDE_PREFIX().empty() == true)
  {
    fprintf(f, "* %s/%s.h\n*/\n", group.c_str(), filter.c_str());
  }
  else
  {
    fprintf(f, "* %s/%s/%s.h\n*/\n", FILTER_INCLUDE_PREFIX().c_str(), group.c_str(), filter.c_str());
  }
  fprintf(f, "#include \"%s/%sWidgets/Q%sWidget.h\"\n", FILTER_INCLUDE_PREFIX().c_str(), group.c_str(), filter.c_str());
  fprintf(f, "#include <QtCore/QDir>\n");
  fprintf(f, "#include <QtGui/QApplication>\n");
  fprintf(f, "#include <QtGui/QLineEdit>\n");
  fprintf(f, "#include <QtGui/QCheckBox>\n");
  fprintf(f, "#include <QtGui/QComboBox>\n\n");
  fprintf(f, "#include \"QtSupport/DREAM3DHelpUrlGenerator.h\"\n\n");
  for (size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget)
    {
      fprintf(f, "#include \"ArraySelectionWidget.h\"\n");
    }
    if (opt->getWidgetType() >= FilterParameter::CellArrayComparisonSelectionWidget
             && opt->getWidgetType() <= FilterParameter::EdgeArrayComparisonSelectionWidget)
    {
      fprintf(f, "#include \"ComparisonSelectionWidget.h\"\n");
    }
    if (opt->getWidgetType() == FilterParameter::AxisAngleWidget)
    {
      fprintf(f, "#include \"AxisAngleWidget.h\"\n");
    }
  }
  fprintf(f, "\n\n\n");
  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "// This file is generated by Qt's moc program which sets up the signals/slots\n");
  fprintf(f, "// for this class.\n");
  fprintf(f, "#include \"moc_Q%sWidget.cxx\"\n\n", filter.c_str());

  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "Q%sWidget::Q%sWidget(QWidget* parent):\nQFilterWidget(parent)\n", filter.c_str(), filter.c_str());
  fprintf(f, "{\n");
  fprintf(f, "     %s::Pointer filter = %s::New();\n", filter.c_str(), filter.c_str());
  // Loop on all the options getting the defaults from a fresh instance of the filter class
  for (size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    std::string prop = opt->getPropertyName();
    std::string typ = opt->getValueType();
    if(opt->getValueType().compare("string") == 0)
    {
      fprintf(f, "     set%s( QString::fromStdString(filter->get%s() ) );\n", prop.c_str(), prop.c_str());
    }
    else if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget)
    {
      fprintf(f, "    //ArraySelectionWidget: Do we need to preset something from the filter maybe?\n");
      implementArrayNameSelectionWidget = true;
    }
    else if (opt->getWidgetType() == FilterParameter::AxisAngleWidget)
    {
      fprintf(f, "    //AxisAngleWidget: Do we need to preset something from the filter maybe?\n");
      implementAxisAngleWidget = true;
    }
    else if (opt->getWidgetType() >= FilterParameter::CellArrayComparisonSelectionWidget
             && opt->getWidgetType() <= FilterParameter::EdgeArrayComparisonSelectionWidget)
    {
      fprintf(f, "    //ComparisonSelectionWidget: Do we need to preset something from the filter maybe?\n");
      implementComparisonSelectionWidget = true;
    }
    else
    {
      fprintf(f, "     set%s( filter->get%s() );\n", prop.c_str(), prop.c_str());
    }
  }

  // Generate code to get all the array names and set the local variables that hold those names
  //appendArrayNameConstructorCode<T>(t, f);

  // Finish Writing the remainder of the constructor code
  fprintf(f, "     m_FilterGroup = QString::fromStdString(filter->getGroupName());\n");
  fprintf(f, "     m_FilterSubGroup = QString::fromStdString(filter->getSubGroupName());\n");
  fprintf(f, "     setupGui();\n");
  fprintf(f, "     setTitle(QString::fromStdString(filter->getHumanLabel()));\n");
  fprintf(f, "}\n\n");

  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "Q%sWidget::~Q%sWidget(){}\n\n", filter.c_str(), filter.c_str());

  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "QString Q%sWidget::getFilterGroup() {\n  return m_FilterGroup;\n}\n\n", filter.c_str() );

  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "QString Q%sWidget::getFilterSubGroup() {\n  return m_FilterSubGroup;\n}\n\n", filter.c_str() );

  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "AbstractFilter::Pointer Q%sWidget::getFilter() \n{\n", filter.c_str());
  fprintf(f, "  %s::Pointer filter = %s::New();\n", filter.c_str(), filter.c_str());
  for (size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    std::string prop = opt->getPropertyName();
    std::string typ = opt->getValueType();
    if(opt->getValueType().compare("string") == 0)
    {
      fprintf(f, "  filter->set%s( get%s().toStdString() );\n", prop.c_str(), prop.c_str());
    }
    else if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget)
    {
      fprintf(f, "  {\n    ArraySelectionWidget* w = qFindChild<ArraySelectionWidget*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "    if (NULL != w) {\n");
      fprintf(f, "      w->getArraySelections(filter.get());\n    }\n  }\n");
    }
    else if (opt->getWidgetType() == FilterParameter::AxisAngleWidget)
    {
      fprintf(f, "  {\n    AxisAngleWidget* w = qFindChild<AxisAngleWidget*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "    if (NULL != w) {\n");
      fprintf(f, "      w->setAxisAnglesIntoFilter<%s>(filter.get());\n    }\n  }\n", filter.c_str());
    }
    else if (opt->getWidgetType() >= FilterParameter::CellArrayComparisonSelectionWidget
             && opt->getWidgetType() <= FilterParameter::EdgeArrayComparisonSelectionWidget)
    {
      fprintf(f, "  {\n    ComparisonSelectionWidget* w = qFindChild<ComparisonSelectionWidget*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "    if (NULL != w) {\n");
      fprintf(f, "      w->setComparisonsIntoFilter<%s>(filter.get());\n    }\n  }\n", filter.c_str());
    }
    else
    {
      fprintf(f, "  filter->set%s( get%s() );\n", prop.c_str(), prop.c_str());
    }
  }
  // Generate all the source code to set the various array names into the filter
  //  appendArrayNameGetFilterCode<T>(t, f);

  fprintf(f, "  return filter;\n");
  fprintf(f, "}\n");

  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "QFilterWidget* Q%sWidget::createDeepCopy() \n{\n", filter.c_str());
  fprintf(f, "  Q%sWidget* w = new Q%sWidget(NULL);\n", filter.c_str(), filter.c_str());
  fprintf(f, "  return w;\n");
  fprintf(f, "}\n");


  // Loop on all the filter options
  for (size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    std::string prop = opt->getPropertyName();
    std::string typ = opt->getValueType();

    if(opt->getCastableValueType().empty() == false)
    {
      std::string cType = opt->getCastableValueType();

      fprintf(f, "\n// -----------------------------------------------------------------------------\n");
      fprintf(f, "void Q%sWidget::set%s(%s v, bool emitChanged)\n{\n", filter.c_str(), prop.c_str(), cType.c_str() );
      fprintf(f, "  m_%s = v;\n", prop.c_str());
      fprintf(f, "  if (true == emitChanged) { emit parametersChanged();}\n}\n");
      fprintf(f, "\n// -----------------------------------------------------------------------------\n");
      fprintf(f, "%s  Q%sWidget::get%s() { \n  return m_%s; \n}\n", cType.c_str(), filter.c_str(), prop.c_str(),  prop.c_str());
    }
    else if(opt->getValueType().compare("string") == 0)
    {
      std::string cType = "QString";

      fprintf(f, "\n// -----------------------------------------------------------------------------\n");
      fprintf(f, "void Q%sWidget::set%s(const %s &v, bool emitChanged)\n{\n  m_%s = v;\n", filter.c_str(), prop.c_str(), cType.c_str(), prop.c_str());
      if (opt->getWidgetType() == FilterParameter::OutputFileWidget ||
          opt->getWidgetType() == FilterParameter::OutputPathWidget ||
          opt->getWidgetType() == FilterParameter::InputFileWidget ||
          opt->getWidgetType() == FilterParameter::InputPathWidget)
      {
        fprintf(f, "  m_%s = QDir::toNativeSeparators(m_%s);\n", prop.c_str(), prop.c_str());
      }
      fprintf(f, "  if (true == emitChanged) { emit parametersChanged();}\n}\n");

      fprintf(f, "\n// -----------------------------------------------------------------------------\n");
      fprintf(f, "%s  Q%sWidget::get%s()\n{ \n  return m_%s; \n}\n", cType.c_str(), filter.c_str(), prop.c_str(), prop.c_str());
    }
    else
    {
      // If we fall in here then the methods should have been generated in the header file
      //    fprintf(f, "Q_PROPERTY(%s %s READ get%s WRITE set%s)\n", typ.c_str(), prop.c_str(), prop.c_str(), prop.c_str());
      //    fprintf(f, "FILTER_PROPERTY_WRAPPER(%s, %s, m_Filter);\n", typ.c_str(), prop.c_str());
    }
  }

  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "void Q%sWidget::writeOptions(QSettings &prefs)\n{\n", filter.c_str());
  fprintf(f, "  prefs.setValue(\"Filter_Name\", \"%s\" );\n", filter.c_str());
  for (size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    std::string prop = opt->getPropertyName();
    std::string typ = opt->getValueType();
    if(opt->getWidgetType() == FilterParameter::InputFileWidget
       || opt->getWidgetType() == FilterParameter::InputPathWidget
       || opt->getWidgetType() == FilterParameter::OutputFileWidget
       || opt->getWidgetType() == FilterParameter::OutputPathWidget)
    {
      fprintf(f, "  prefs.setValue(\"%s\", QDir::toNativeSeparators(get%s()) );\n", prop.c_str(), prop.c_str());
    }
    else if (opt->getWidgetType() >= FilterParameter::VoxelCellArrayNameSelectionWidget
             && opt->getWidgetType() <= FilterParameter::SolidMeshEdgeArrayNameSelectionWidget )
    {
      implementArrayNameComboBoxUpdated = true;
      fprintf(f, "  prefs.setValue(\"%s\", get%s() );\n", prop.c_str(), prop.c_str());
    }
    else if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget)
    {
      fprintf(f, "  // ------------- %s ----------------------------------\n", prop.c_str());
      fprintf(f, "  {\n    ArraySelectionWidget* w = qFindChild<ArraySelectionWidget*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "    if (NULL != w) {\n");
      fprintf(f, "      w->writeOptions(prefs, QString::fromUtf8(\"%s\"));\n", prop.c_str());
      fprintf(f, "    }\n  }\n");
    }
    else if (opt->getWidgetType() == FilterParameter::IntVec3Widget)
    {
      fprintf(f, "// ------------- %s ----------------------------------\n", prop.c_str());
      fprintf(f, "  IntVec3Widget_t v_%s = get%s();\n", prop.c_str(), prop.c_str());
      fprintf(f, "  prefs.beginWriteArray(\"%s\", 3);\n", prop.c_str());
      fprintf(f, "  prefs.setArrayIndex(0);\n");
      fprintf(f, "  prefs.setValue(\"x\", v_%s.x);\n", prop.c_str());
      fprintf(f, "  prefs.setArrayIndex(1);\n");
      fprintf(f, "  prefs.setValue(\"y\", v_%s.y);\n", prop.c_str());
      fprintf(f, "  prefs.setArrayIndex(2);\n");
      fprintf(f, "  prefs.setValue(\"z\", v_%s.z);\n", prop.c_str());
      fprintf(f, "  prefs.endArray();\n");
    }
    else if (opt->getWidgetType() == FilterParameter::FloatVec3Widget)
    {
      fprintf(f, "// ------------- %s ----------------------------------\n", prop.c_str());
      fprintf(f, "  FloatVec3Widget_t v_%s = get%s();\n", prop.c_str(), prop.c_str());
      fprintf(f, "  prefs.beginWriteArray(\"%s\", 3);\n", prop.c_str());
      fprintf(f, "  prefs.setArrayIndex(0);\n");
      fprintf(f, "  prefs.setValue(\"x\", static_cast<double>(v_%s.x));\n", prop.c_str());
      fprintf(f, "  prefs.setArrayIndex(1);\n");
      fprintf(f, "  prefs.setValue(\"y\", static_cast<double>(v_%s.y));\n", prop.c_str());
      fprintf(f, "  prefs.setArrayIndex(2);\n");
      fprintf(f, "  prefs.setValue(\"z\", static_cast<double>(v_%s.z));\n", prop.c_str());
      fprintf(f, "  prefs.endArray();\n");
    }
    else if (opt->getWidgetType() == FilterParameter::AxisAngleWidget)
    {
      fprintf(f, "  // ------------- %s ----------------------------------\n", prop.c_str());
      fprintf(f, "  {\n    AxisAngleWidget* w = qFindChild<AxisAngleWidget*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "    if (NULL != w) {\n");
      fprintf(f, "      w->writeOptions(prefs, QString::fromUtf8(\"%s\"));\n", prop.c_str());
      fprintf(f, "    }\n  }\n");
    }
    else if (opt->getWidgetType() >= FilterParameter::CellArrayComparisonSelectionWidget
             && opt->getWidgetType() <= FilterParameter::EdgeArrayComparisonSelectionWidget)
    {
      fprintf(f, "  // ------------- %s ----------------------------------\n", prop.c_str());
      fprintf(f, "  {\n    ComparisonSelectionWidget* w = qFindChild<ComparisonSelectionWidget*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "    if (NULL != w) {\n");
      fprintf(f, "      w->writeOptions(prefs, QString::fromUtf8(\"%s\"));\n", prop.c_str());
      fprintf(f, "    }\n  }\n");
    }
    else
    {
      fprintf(f, "  prefs.setValue(\"%s\", get%s() );\n", prop.c_str(), prop.c_str());
    }

  }
  fprintf(f, "}\n");

  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "void Q%sWidget::readOptions(QSettings &prefs)\n{\n", filter.c_str());
  // fprintf(f, "  std::cout << \"Reading Prefs for Filter  %s \" << std::endl;\n", filter.c_str());

  std::stringstream replaceStream;
  for (size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    std::string prop = opt->getPropertyName();
    std::string typ = opt->getValueType();
    std::string hl = opt->getHumanLabel();
    fprintf(f, "  // ------------- %s ----------------------------------\n", prop.c_str());
    fprintf(f, "  {\n   QVariant p_%s = prefs.value(\"%s\");\n", prop.c_str(), prop.c_str());

    if(opt->getWidgetType() == FilterParameter::StringWidget)
    {
      fprintf(f, "   QLineEdit* le = findChild<QLineEdit*>(\"%s\");\n", prop.c_str());
      fprintf(f, "   if (le) { le->setText(p_%s.toString()); }\n", prop.c_str());
      replaceStream << "setStlFilePrefix( QString::fromStdString( reader->readValue(H5FilterParameter::StlFilePrefixConstant, \"\") ) );" << std::endl;
    }
    else if(opt->getWidgetType() == FilterParameter::IntWidget)
    {
      fprintf(f, "   QLineEdit* le = findChild<QLineEdit*>(\"%s\");\n", prop.c_str());
      fprintf(f, "   if (le) { le->setText(p_%s.toString()); }\n", prop.c_str());
      replaceStream << "setMaxIterations( reader->readValue(H5FilterParameter::MaxIterationsConstant, 0) );" << std::endl;
    }
    else if(opt->getWidgetType() == FilterParameter::DoubleWidget)
    {
      fprintf(f, "   QLineEdit* le = findChild<QLineEdit*>(\"%s\");\n", prop.c_str());
      fprintf(f, "   if (le) { le->setText(p_%s.toString());}\n", prop.c_str());
      replaceStream << "setMisorientationTolerance( reader->readValue(H5FilterParameter::MisorientationToleranceConstant, 0) );" << std::endl;
    }
    else if(opt->getWidgetType() == FilterParameter::InputFileWidget)
    {
      fprintf(f, "   QString path = QDir::toNativeSeparators(p_%s.toString());\n", prop.c_str());
      fprintf(f, "   QLineEdit* lb = qFindChild<QLineEdit*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "   if (lb) { lb->setText(path); }\n");
      fprintf(f, "   set%s(path);\n", prop.c_str());
      replaceStream << "setInputFile( QString::fromStdString( reader->readValue(H5FilterParameter::InputFileConstant, \"\") ) );" << std::endl;
    }
    else if (opt->getWidgetType() == FilterParameter::InputPathWidget)
    {
      fprintf(f, "   QString path = QDir::toNativeSeparators(p_%s.toString());\n", prop.c_str());
      fprintf(f, "   QLineEdit* lb = qFindChild<QLineEdit*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "   if (lb) { lb->setText(path); }\n");
      fprintf(f, "   set%s(path);\n", prop.c_str());
      replaceStream << "setInputPath( QString::fromStdString( reader->readValue(H5FilterParameter::InputPathConstant, \"\") ) );" << std::endl;
    }
    else if (opt->getWidgetType() == FilterParameter::OutputFileWidget)
    {
      fprintf(f, "   QString path = QDir::toNativeSeparators(p_%s.toString());\n", prop.c_str());
      fprintf(f, "   QLineEdit* lb = qFindChild<QLineEdit*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "   if (lb) { lb->setText(path); }\n");
      fprintf(f, "   set%s(path);\n", prop.c_str());
      replaceStream << "setOutputFile( QString::fromStdString( reader->readValue(H5FilterParameter::OutputFileConstant, \"\") ) );" << std::endl;
    }
    else if (opt->getWidgetType() == FilterParameter::OutputPathWidget)
    {
      fprintf(f, "   QString path = QDir::toNativeSeparators(p_%s.toString());\n", prop.c_str());
      fprintf(f, "   QLineEdit* lb = qFindChild<QLineEdit*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "   if (lb) { lb->setText(path); }\n");
      fprintf(f, "   set%s(path);\n", prop.c_str());
      replaceStream << "setOutputPath( QString::fromStdString( reader->readValue(H5FilterParameter::OutputPathConstant, \"\") ) );" << std::endl;
    }
    else if(opt->getWidgetType() == FilterParameter::BooleanWidget)
    {
      fprintf(f, "   QCheckBox* le = findChild<QCheckBox*>(\"%s\");\n", prop.c_str());
      fprintf(f, "   if (le) { le->setChecked(p_%s.toBool()); }\n", prop.c_str());
      replaceStream << "setWriteAlignmentShifts( reader->readValue(H5FilterParameter::WriteAlignmentShiftsConstant, false) );" << std::endl;
    }
    else if(opt->getWidgetType() == FilterParameter::ChoiceWidget)
    {
      fprintf(f, "   QComboBox* cb = findChild<QComboBox*>(\"%s\");\n", prop.c_str());
      fprintf(f, "   if (cb) {\n");
      if (opt->getValueType().compare("string") == 0)
      {
        fprintf(f, "    QString str_%s = p_%s.toString();\n", prop.c_str(), prop.c_str());
        fprintf(f, "    int index = cb->findText(str_%s);\n", prop.c_str() );
        fprintf(f, "    if (index != -1) {\n");
        fprintf(f, "      cb->setCurrentIndex(index);\n");
        fprintf(f, "    }\n");
        fprintf(f, "    else {\n");
        fprintf(f, "      cb->addItem(str_%s);\n", prop.c_str() );
        fprintf(f, "      cb->setCurrentIndex(cb->count() -1 );\n");
        fprintf(f, "    }\n");
      }
      else
      {
        fprintf(f, "     bool ok = false;\n");
        fprintf(f, "     if (p_%s.toInt(&ok) < cb->count()) {\n", prop.c_str());
        fprintf(f, "       cb->setCurrentIndex(p_%s.toInt());\n", prop.c_str());
        fprintf(f, "     }\n");
      }
      fprintf(f, "   }\n");
      replaceStream << "setConversionType( reader->readValue(H5FilterParameter::ConversionTypeConstant, 0) );" << std::endl;
    }
    else if (opt->getWidgetType() >= FilterParameter::VoxelCellArrayNameSelectionWidget
             && opt->getWidgetType() <= FilterParameter::SolidMeshEdgeArrayNameSelectionWidget )
    {
      implementArrayNameComboBoxUpdated = true;
      fprintf(f, "   QComboBox* cb = findChild<QComboBox*>(\"%s\");\n", prop.c_str());
      fprintf(f, "   if (cb) {\n");
      fprintf(f, "     qint32 count = cb->count();\n");
      fprintf(f, "     for(qint32 i = 0; i < count; ++i) {\n");
      fprintf(f, "       if (cb->itemText(i).compare(p_%s.toString()) == 0) {\n", prop.c_str());
      fprintf(f, "         cb->setCurrentIndex(i);\n");
      fprintf(f, "         break;\n");
      fprintf(f, "       }\n");
      fprintf(f, "     }\n");
      fprintf(f, "   }\n");

      if (opt->getWidgetType() == FilterParameter::VoxelCellArrayNameSelectionWidget)
      {
        replaceStream << "setSelectedCellArrayName( QString::fromStdString( reader->readValue(H5FilterParameter::SelectedCellArrayNameConstant, \"\") ) );" << std::endl;
      }
      else if (opt->getWidgetType() == FilterParameter::VoxelFieldArrayNameSelectionWidget)
      {
        replaceStream << "setSelectedFieldArrayName( QString::fromStdString( reader->readValue(H5FilterParameter::SelectedFieldArrayNameConstant, \"\") ) );" << std::endl;
      }
      else if (opt->getWidgetType() == FilterParameter::VoxelEnsembleArrayNameSelectionWidget)
      {
        replaceStream << "setSelectedEnsembleArrayName( QString::fromStdString( reader->readValue(H5FilterParameter::SelectedEnsembleArrayNameConstant, \"\") ) );" << std::endl;
      }
      else if (opt->getWidgetType() == FilterParameter::SurfaceMeshVertexArrayNameSelectionWidget)
      {
        replaceStream << "setSurfaceMeshPointArrayName( QString::fromStdString( reader->readValue(H5FilterParameter::SurfaceMeshPointArrayNameConstant, \"\") ) );" << std::endl;
      }
      else if (opt->getWidgetType() == FilterParameter::SurfaceMeshFaceArrayNameSelectionWidget)
      {
        replaceStream << "setSurfaceMeshFaceArrayName( QString::fromStdString( reader->readValue(H5FilterParameter::SurfaceMeshFaceArrayNameConstant, \"\") ) );" << std::endl;
      }
      else if (opt->getWidgetType() == FilterParameter::SurfaceMeshEdgeArrayNameSelectionWidget)
      {
        replaceStream << "setSurfaceMeshEdgeArrayName( QString::fromStdString( reader->readValue(H5FilterParameter::SurfaceMeshEdgeArrayNameConstant, \"\") ) );" << std::endl;
      }
      else if (opt->getWidgetType() == FilterParameter::SolidMeshVertexArrayNameSelectionWidget)
      {
        replaceStream << "setSolidMeshPointArrayName( QString::fromStdString( reader->readValue(H5FilterParameter::SolidMeshPointArrayNameConstant, \"\") ) );" << std::endl;
      }
      else if (opt->getWidgetType() == FilterParameter::SolidMeshFaceArrayNameSelectionWidget)
      {
        replaceStream << "setSolidMeshFaceArrayName( QString::fromStdString( reader->readValue(H5FilterParameter::SolidMeshFaceArrayNameConstant, \"\") ) );" << std::endl;
      }
      else if (opt->getWidgetType() == FilterParameter::SolidMeshEdgeArrayNameSelectionWidget)
      {
        replaceStream << "setSolidMeshEdgeArrayName( QString::fromStdString( reader->readValue(H5FilterParameter::SolidMeshEdgeArrayNameConstant, \"\") ) );" << std::endl;
      }
    }
    else if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget)
    {
      fprintf(f, "    ArraySelectionWidget* w = qFindChild<ArraySelectionWidget*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "    if (NULL != w) {\n");
      fprintf(f, "      w->readOptions(prefs, QString::fromUtf8(\"%s\"));\n", prop.c_str());
      fprintf(f, "    }\n");
    }
    else if (opt->getWidgetType() == FilterParameter::IntVec3Widget)
    {
      fprintf(f, "   bool ok = false;\n");
      fprintf(f, "   IntVec3Widget_t v3 = p_%s.value<IntVec3Widget_t>();\n", prop.c_str());
      fprintf(f, "   prefs.beginReadArray(\"%s\");\n", prop.c_str());

      fprintf(f, "   prefs.setArrayIndex(0);\n");
      fprintf(f, "   v3.x = prefs.value(\"x\", v3.x).toInt(&ok);\n");
      fprintf(f, "   QLineEdit* le_0 = findChild<QLineEdit*>(\"0_%s\");\n", prop.c_str());
      fprintf(f, "   if (le_0) { le_0->setText(QString::number(v3.x)); }\n");

      fprintf(f, "   prefs.setArrayIndex(1);\n");
      fprintf(f, "   v3.y = prefs.value(\"y\", v3.y).toInt(&ok);\n");
      fprintf(f, "   QLineEdit* le_1 = findChild<QLineEdit*>(\"1_%s\");\n", prop.c_str());
      fprintf(f, "   if (le_1) { le_1->setText(QString::number(v3.y)); }\n");

      fprintf(f, "   prefs.setArrayIndex(2);\n");
      fprintf(f, "   v3.z = prefs.value(\"z\", v3.z).toInt(&ok);\n");
      fprintf(f, "   QLineEdit* le_2 = findChild<QLineEdit*>(\"2_%s\");\n", prop.c_str());
      fprintf(f, "   if (le_2) { le_2->setText(QString::number(v3.z)); }\n");

      fprintf(f, "   prefs.endArray();\n");

      replaceStream << "setDimensions( reader->readValue(H5FilterParameter::GenericTestDimensionsConstant, m_Dimensions) );" << std::endl;
    }
    else if (opt->getWidgetType() == FilterParameter::FloatVec3Widget)
    {
      fprintf(f, "   bool ok = false;\n");
      fprintf(f, "   FloatVec3Widget_t v3 = p_%s.value<FloatVec3Widget_t>();\n", prop.c_str());
      fprintf(f, "   prefs.beginReadArray(\"%s\");\n", prop.c_str());

      fprintf(f, "   prefs.setArrayIndex(0);\n");
      fprintf(f, "   v3.x = prefs.value(\"x\", v3.x).toFloat(&ok);\n");
      fprintf(f, "   QLineEdit* le_0 = findChild<QLineEdit*>(\"0_%s\");\n", prop.c_str());
      fprintf(f, "   if (le_0) { le_0->setText(QString::number(v3.x)); }\n");

      fprintf(f, "   prefs.setArrayIndex(1);\n");
      fprintf(f, "   v3.y = prefs.value(\"y\", v3.y).toFloat(&ok);\n");
      fprintf(f, "   QLineEdit* le_1 = findChild<QLineEdit*>(\"1_%s\");\n", prop.c_str());
      fprintf(f, "   if (le_1) { le_1->setText(QString::number(v3.y)); }\n");

      fprintf(f, "   prefs.setArrayIndex(2);\n");
      fprintf(f, "   v3.z = prefs.value(\"z\", v3.z).toFloat(&ok);\n");
      fprintf(f, "   QLineEdit* le_2 = findChild<QLineEdit*>(\"2_%s\");\n", prop.c_str());
      fprintf(f, "   if (le_2) { le_2->setText(QString::number(v3.z)); }\n");

      fprintf(f, "   prefs.endArray();\n");

      replaceStream << "setOrigin( reader->readValue(H5FilterParameter::GenericTestOriginConstant, m_Origin) );" << std::endl;
    }
    else if (opt->getWidgetType() == FilterParameter::AxisAngleWidget)
    {
      fprintf(f, "    AxisAngleWidget* w = qFindChild<AxisAngleWidget*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "    if (NULL != w) {\n");
      fprintf(f, "      w->readOptions(prefs, QString::fromUtf8(\"%s\"));\n", prop.c_str());
      fprintf(f, "    }\n");
      replaceStream << "setAxisAngleRotations( reader->readValue(H5FilterParameter::AxisAngleInputsConstant, m_AxisAngleRotations) );" << std::endl;
    }
    else if (opt->getWidgetType() >= FilterParameter::CellArrayComparisonSelectionWidget
             && opt->getWidgetType() <= FilterParameter::EdgeArrayComparisonSelectionWidget)
    {
      fprintf(f, "    ComparisonSelectionWidget* w = qFindChild<ComparisonSelectionWidget*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "    if (NULL != w) {\n");
      fprintf(f, "      w->readOptions(prefs, QString::fromUtf8(\"%s\"));\n", prop.c_str());
      fprintf(f, "    }\n");

      if (opt->getWidgetType() == FilterParameter::CellArrayComparisonSelectionWidget)
      {
        replaceStream << "setCellComparisonInputs( reader->readValue(H5FilterParameter::CellComparisonInputsConstant, m_CellComparisonInputs) );" << std::endl;
      }
      else if (opt->getWidgetType() == FilterParameter::FieldArrayComparisonSelectionWidget)
      {
        replaceStream << "setFieldComparisonInputs( reader->readValue(H5FilterParameter::FieldComparisonInputsConstant, m_FieldComparisonInputs) );" << std::endl;
      }
      else if (opt->getWidgetType() == FilterParameter::EnsembleArrayComparisonSelectionWidget)
      {
        replaceStream << "setEnsembleComparisonInputs( reader->readValue(H5FilterParameter::EnsembleComparisonInputsConstant, m_EnsembleComparisonInputs) );" << std::endl;
      }
      else if (opt->getWidgetType() == FilterParameter::PointArrayComparisonSelectionWidget)
      {
        replaceStream << "setPointComparisonInputs( reader->readValue(H5FilterParameter::PointComparisonInputsConstant, m_PointComparisonInputs) );" << std::endl;
      }
      else if (opt->getWidgetType() == FilterParameter::FaceArrayComparisonSelectionWidget)
      {
        replaceStream << "setFaceComparisonInputs( reader->readValue(H5FilterParameter::FaceComparisonInputsConstant, m_FaceComparisonInputs) );" << std::endl;
      }
      else if (opt->getWidgetType() == FilterParameter::EdgeArrayComparisonSelectionWidget)
      {
        replaceStream << "setEdgeComparisonInputs( reader->readValue(H5FilterParameter::EdgeComparisonInputsConstant, m_EdgeComparisonInputs) );" << std::endl;
      }
    }
    else
    {
      fprintf(f, " #error: Class %s  Property %s  NOTHING WAS GENERATED TO READ/WRITE PROPERTY\n", filter.c_str(), prop.c_str());
    }
    fprintf(f, "  }\n");
  }
  fprintf(f, "\n}\n");

  // This template function will generate all the necessary code to set the name of each
  // required and created array.
  //appendArrayNameCodeToSource<T>(t, f);

  if (true == implementArrayNameComboBoxUpdated)
  {
    fprintf(f, "\n// -----------------------------------------------------------------------------\n");
    fprintf(f, "void Q%sWidget::arrayNameComboBoxUpdated(QComboBox* cb)\n{\n", filter.c_str());
    fprintf(f, "  if (NULL == cb) { return; }\n"); // Make sure we have a non null QWidget to deal with
    for (size_t i = 0; i < options.size(); ++i)
    {
      FilterParameter::Pointer opt = options[i];
      std::string prop = opt->getPropertyName();
      std::string typ = opt->getValueType();
      std::string hl = opt->getHumanLabel();
      if (opt->getWidgetType() >= FilterParameter::VoxelCellArrayNameSelectionWidget
          && opt->getWidgetType() <= FilterParameter::SolidMeshEdgeArrayNameSelectionWidget ) {
        fprintf(f, "  if(cb->objectName().compare(\"%s\") == 0) {\n", prop.c_str());
        fprintf(f, "    m_%s = cb->currentText();\n  }\n", prop.c_str());
      }
    }
    fprintf(f, "}\n");
  }

  if (implementArrayNameSelectionWidget == true || implementComparisonSelectionWidget == true)
  {

    fprintf(f, "\n// -----------------------------------------------------------------------------\n");
    fprintf(f, "void Q%sWidget::preflightAboutToExecute(VoxelDataContainer::Pointer vdc, SurfaceMeshDataContainer::Pointer smdc, SolidMeshDataContainer::Pointer sdc)\n{\n", filter.c_str());
    for (size_t i = 0; i < options.size(); ++i)
    {
      FilterParameter::Pointer opt = options[i];
      std::string prop = opt->getPropertyName();
      std::string typ = opt->getValueType();
      std::string hl = opt->getHumanLabel();
      if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget ) {
        fprintf(f, "  {\n    ArraySelectionWidget* w = qFindChild<ArraySelectionWidget*>(this, \"%s\");\n", prop.c_str()); // Make sure we have a non null QWidget to deal with

        fprintf(f, "    if (NULL != w) {\n      w->populateArrayNames(vdc, smdc, sdc);\n    }\n  }\n");
      }
      if (opt->getWidgetType() >= FilterParameter::CellArrayComparisonSelectionWidget
             && opt->getWidgetType() <= FilterParameter::EdgeArrayComparisonSelectionWidget)
      {
        fprintf(f, "  {\n    ComparisonSelectionWidget* w = qFindChild<ComparisonSelectionWidget*>(this, \"%s\");\n", prop.c_str()); // Make sure we have a non null QWidget to deal with

        fprintf(f, "    if (NULL != w) {\n      w->populateArrayNames(vdc, smdc, sdc);\n    }\n  }\n");
      }
    }
    fprintf(f, "}\n");
  }



  /* Implement the htmlHelpIndexFile() method */
  std::string lower = filter;
  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "void Q%sWidget::openHtmlHelpFile()\n{\n", filter.c_str());
  fprintf(f, "\tDREAM3DHelpUrlGenerator::generateAndOpenHTMLUrl(\"%s\", this);\n", lower.c_str());
  fprintf(f, "}\n");




  fprintf(f, "\n\n");
  fclose(f);

  // Now compare the file just generated with any possible existing file
  size_t currentFileSize = MXAFileInfo::fileSize(completePath);
  size_t tempFileSize = MXAFileInfo::fileSize(tempPath);

  // If the file sizes are different then copy the file
  if(currentFileSize != tempFileSize)
  {
    std::cout << "0-Creating Source File: " << completePath << std::endl;
    copyFile(tempPath, completePath);
  }
  else // Just because the files are the same size does not mean they are the same.
  {
    FILE* c = fopen(completePath.c_str(), "rb");
    unsigned char* currentContents = reinterpret_cast<unsigned char*>(malloc(currentFileSize));
    size_t itemsRead = fread(currentContents, currentFileSize, 1, c);
    if(itemsRead != 1)
    {

    }
    fclose(c);

    MD5 md5_current;
    md5_current.update(currentContents, currentFileSize);
    md5_current.finalize();
    std::string currentHexDigest = md5_current.hexdigest();


    FILE* t = fopen(tempPath.c_str(), "rb");
    unsigned char* tempContents = reinterpret_cast<unsigned char*>(malloc(tempFileSize));
    itemsRead = fread(tempContents, tempFileSize, 1, t);
    if(itemsRead != 1)
    {

    }
    fclose(t);

    MD5 md5;
    md5.update(tempContents, tempFileSize);
    md5.finalize();
    std::string tempHexDigest = md5.hexdigest();

    // Use MD5 Checksums to figure out if the files are different
    if (tempHexDigest.compare(currentHexDigest) != 0)
    {
      std::cout << "0-Creating Source File: " << completePath << std::endl;
      copyFile(tempPath, completePath);
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void createPreflightTestCode( const std::string &group, const std::string &filter)
{
  std::string s = FILTER_WIDGETS_TEMP_DIR();
  s.append("Preflight_Test_Code_Fragment_1.h");
  FILE* f = fopen(s.c_str(), "ab+"); // Clear out this file
  fprintf(f, "MAKE_FILTER_TEST(  %s, FAIL_IS_PASS)\n", filter.c_str());
  fclose(f);


  s = FILTER_WIDGETS_TEMP_DIR();
  s.append("Preflight_Test_Code_Fragment_2.h");
  f = fopen(s.c_str(), "ab+"); // Clear out this file
  fprintf(f, "DREAM3D_REGISTER_TEST( %s_PreFlightTest() )\n", filter.c_str());
  fclose(f);

}




#include "FilterWidgetCodeGen.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  // if (true) return 0;

#if (GENERATE_PREFLIGHT_TEST_CODE_FRAGMENT == 1)
  std::string s = FILTER_WIDGETS_TEMP_DIR();
  s.append("Preflight_Test_Code_Fragment_1.h");
  FILE* f = fopen(s.c_str(), "wb"); // Clear out this file
  fclose(f);

  s = FILTER_WIDGETS_TEMP_DIR();
  s.append("Preflight_Test_Code_Fragment_2.h");
  f = fopen(s.c_str(), "wb"); // Clear out this file
  fclose(f);
#endif

  setFilters();

  createMarkdownCreatedArrayIndex();

  return 0;
}


