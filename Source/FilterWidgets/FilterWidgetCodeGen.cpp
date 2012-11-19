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

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/FilterParameter.h"


// Enable this to generate a text file that has each filter and all of its
// parameters, required array and created array listed.
#define GENERATE_FILTER_TEXT_LIST 0

// Enable this to generate a fragment of HTML code that was used to update
// the documentation files
#define GENERATE_HTML_FRAGMENT 0

// Enable this to generate an HTML template file for each and every
// filter. If you are NOT careful you can over write any existing documentation
// file. You have been warned.
#define GENERATE_HTML_FILE 1
#define OVERWRITE_HTML_DOCS 1

// Just some experimental code that generates a bunch of update code to
// insert into each and every filter. Completely deprecated now.
#define GENERATE_OPTIONS_WRITER_CODE 0

// Enabling this will create a pair of files that can be used to update
// the "PreflightTest.cpp" unit test
#define GENERATE_PREFLIGHT_TEST_CODE_FRAGMENT 0

typedef std::map<std::string, std::set<std::string> >  FilterMapType;
typedef std::set<std::string>  StringSetType;

// These will be defined in an include header file below.
std::string FILTER_WIDGETS_BINARY_DIR();
std::string FILTER_WIDGETS_SOURCE_DIR();
std::string FILTER_WIDGETS_TEMP_DIR();
std::string FILTER_WIDGETS_DOCS_DIR();
std::string DREAM3D_SOURCE_DIR();
std::string FILTER_INCLUDE_PREFIX();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void copyFile(const std::string &src, const std::string &dest)
{
  // Now compare the file just generated with any possible existing file
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
void writeArrayNameHeaderCode(FILE* f, std::set<std::string> &list, const std::string &filterName, const std::string &title)
{
    if(list.size() == 0) { return; }
    fprintf(f, "  //------ %s ----------------\n", title.c_str());
    const char* cType = "QString";
    for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
    {
        std::string arrayname = std::string((*iter) + "ArrayName");

        fprintf(f, "  // Get/Set the Array name for %s\n", (*iter).c_str());
        fprintf(f, "  private:\n");
        fprintf(f, "    QString m_%sArrayName;\n", (*iter).c_str());
        fprintf(f, "  public:\n");
        fprintf(f, "    Q_PROPERTY(%s %s READ get%s WRITE set%s)\n", cType, arrayname.c_str(), arrayname.c_str(), arrayname.c_str());
        fprintf(f, "    %s  get%s();\n", cType, arrayname.c_str());
        fprintf(f, "  public slots:\n");
        fprintf(f, "    void set%s(const %s &v);\n", arrayname.c_str(), cType);
        fprintf(f, "  // Get/Set the Array name for %s Complete ------------\n\n", (*iter).c_str());
    }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeArrayNameSourceCode(FILE* f, std::set<std::string> &list, const std::string &filter, const std::string &title)
{
    if(list.size() == 0) { return; }
  //  fprintf(f, "//------ %s ----------------\n", title.c_str());
    const char* cType = "QString";
    for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
    {
        std::string arrayname = std::string((*iter) + "ArrayName");

        fprintf(f, "// -----------------------------------------------------------------------------\n");
        fprintf(f, "// Get/Set the Array name for %s '%s'\n", title.c_str(), (*iter).c_str());
        fprintf(f, "void Q%sWidget::set%s(const %s &v)\n{\n  m_%s = v;\n}\n", filter.c_str(), arrayname.c_str(), cType, arrayname.c_str());
        fprintf(f, "%s  Q%sWidget::get%s()\n{\n  return m_%s; \n}\n\n", cType, filter.c_str(), arrayname.c_str(), arrayname.c_str());
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeArrayNameConstructorCode(FILE* f, std::set<std::string> &list, const std::string &filter, const std::string &title)
{
    if(list.size() == 0) { return; }
    for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
    {
        std::string arrayname = std::string((*iter) + "ArrayName");
        fprintf(f, "     set%s( QString::fromStdString(filter->get%s() ) );\n", arrayname.c_str(), arrayname.c_str());
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeArrayNameGetFilterCode(FILE* f, std::set<std::string> &list, const std::string &filter, const std::string &title)
{
    if(list.size() == 0) { return; }
    for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
    {
        std::string arrayname = std::string((*iter) + "ArrayName");
        fprintf(f, "  filter->set%s( get%s().toStdString() );\n", arrayname.c_str(), arrayname.c_str());
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeArrayNameDeepCopyCode(FILE* f, std::set<std::string> &list, const std::string &filter, const std::string &title)
{
    if(list.size() == 0) { return; }
    for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
    {
        std::string arrayname = std::string((*iter) + "ArrayName");
        fprintf(f, "  w->set%s( get%s() );\n", arrayname.c_str(), arrayname.c_str() );
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#define ARRAY_NAME_CODE_GEN_METHODS(methodName, writerName)\
template<typename T>\
void methodName(typename T::Pointer t, FILE* f){\
    VoxelDataContainer::Pointer m = VoxelDataContainer::New();\
    t->setVoxelDataContainer(m.get());\
    SurfaceMeshDataContainer::Pointer sm = SurfaceMeshDataContainer::New();\
    t->setSurfaceMeshDataContainer(sm.get());\
    SolidMeshDataContainer::Pointer solid = SolidMeshDataContainer::New();\
    t->setSolidMeshDataContainer(solid.get());\
    t->preflight();\
    {\
      std::set<std::string> list = t->getRequiredCellData();\
      writerName(f, list, t->getNameOfClass(), "Required Cell Data");\
    }\
    {\
      std::set<std::string> list = t->getCreatedCellData();\
      writerName(f, list, t->getNameOfClass(), "Created Cell Data");\
    }\
    {\
      std::set<std::string> list = t->getRequiredFieldData();\
      writerName(f, list, t->getNameOfClass(), "Required Field Data");\
    }\
    {\
      std::set<std::string> list = t->getCreatedFieldData();\
      writerName(f, list, t->getNameOfClass(), "Created Field Data");\
    }\
    {\
      std::set<std::string> list = t->getRequiredEnsembleData();\
      writerName(f, list, t->getNameOfClass(), "Required Ensemble Data");\
    }\
    {\
      std::set<std::string> list = t->getCreatedEnsembleData();\
      writerName(f, list, t->getNameOfClass(), "Created Ensemble Data");\
    }\
}

ARRAY_NAME_CODE_GEN_METHODS(appendArrayNameCodeToHeader, writeArrayNameHeaderCode)
ARRAY_NAME_CODE_GEN_METHODS(appendArrayNameCodeToSource, writeArrayNameSourceCode)
ARRAY_NAME_CODE_GEN_METHODS(appendArrayNameConstructorCode, writeArrayNameConstructorCode)
ARRAY_NAME_CODE_GEN_METHODS(appendArrayNameGetFilterCode, writeArrayNameGetFilterCode)
ARRAY_NAME_CODE_GEN_METHODS(appendArrayNameDeepCopyCode, writeArrayNameDeepCopyCode)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void createHeaderFile( const std::string &group, const std::string &filter)
{
  typename T::Pointer t = T::New();
  std::vector<FilterParameter::Pointer> options = t->getFilterParameters();


  std::stringstream ss;
  ss << FILTER_WIDGETS_SOURCE_DIR() << "/" << group << "Widgets/Q" << filter << "Widget.h";
  std::string completePath = MXADir::toNativeSeparators(ss.str());
  if (MXADir::exists(completePath) == true)
  {
    std::cout << filter << ": Header file already exists in source directory. NOT generating generic widget." << std::endl;
    return;
  }

  ss.str("");
  ss << FILTER_WIDGETS_BINARY_DIR() << "/" << group << "Widgets/Q" << filter << "Widget.h";

  completePath = MXADir::toNativeSeparators(ss.str());

  ss.str("");
  ss << FILTER_WIDGETS_TEMP_DIR() << "/TEMP_WIDGET.h";
  std::string tempPath = ss.str();

  FILE* f = fopen(tempPath.c_str(), "wb");

  fprintf(f, "/*\n");
  fprintf(f, "  This file was auto-generated from the program FilterWidgetCodeGen.cpp which is\n  itself generated during cmake time\n");
  fprintf(f, "  If you need to make changes to the code that is generated you will need to make\n  them in the original file. \n");
  fprintf(f, "  The code generated is based off values from the filter located at\n");
  if (FILTER_INCLUDE_PREFIX().empty() == true) {
    fprintf(f, "  %s/%s.h\n*/\n", group.c_str(), filter.c_str());
  }
  else
  {
    fprintf(f, "  %s/%s/%s.h\n*/\n", FILTER_INCLUDE_PREFIX().c_str(), group.c_str(), filter.c_str());
  }

  fprintf(f, "#ifndef _Q%s_H_\n", filter.c_str());
  fprintf(f, "#define _Q%s_H_\n\n", filter.c_str());

  fprintf(f, "#include <QtCore/QObject>\n");
  fprintf(f, "#include <QtCore/QSettings>\n\n");

  fprintf(f, "#include \"PipelineBuilder/QFilterWidget.h\"\n");
  fprintf(f, "#include \"DREAM3DLib/Common/DREAM3DSetGetMacros.h\"\n\n");
  if (FILTER_INCLUDE_PREFIX().empty() == true) {
    fprintf(f, "#include \"%s/%s.h\"\n", group.c_str(), filter.c_str());
  }
  else
  {
    fprintf(f, "#include \"%s/%s/%s.h\"\n", FILTER_INCLUDE_PREFIX().c_str(), group.c_str(), filter.c_str());
  }
  fprintf(f, "class Q%sWidget : public QFilterWidget \n{\n", filter.c_str());
  fprintf(f, "   Q_OBJECT\n");
  fprintf(f, " public:\n");
  fprintf(f, "    Q%sWidget(QWidget* parent = NULL);\n", filter.c_str());
  fprintf(f, "    virtual ~Q%sWidget();\n", filter.c_str());
  fprintf(f, "    virtual AbstractFilter::Pointer getFilter();\n");
  fprintf(f, "    void writeOptions(QSettings &prefs);\n");
  fprintf(f, "    void readOptions(QSettings &prefs);\n\n");
  fprintf(f, "    QFilterWidget* createDeepCopy();\n\n");
  fprintf(f, "    QString getFilterGroup();\n\n");


 // Loop on all the filter options
  for(size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    std::string prop = opt->getPropertyName();
    std::string typ = opt->getValueType();

    if (opt->getCastableValueType().empty() == false)
    {
      std::string cType = opt->getCastableValueType();
      fprintf(f, " private:\n");
      fprintf(f, "    %s m_%s;\n", cType.c_str(), prop.c_str());
      fprintf(f, " public:\n");
      fprintf(f, "    Q_PROPERTY(%s %s READ get%s WRITE set%s)\n", cType.c_str(), prop.c_str(), prop.c_str(), prop.c_str());
      fprintf(f, " public slots:\n");
      fprintf(f, "    void set%s(%s v);\n", prop.c_str(), cType.c_str());
      fprintf(f, " public:\n");
      fprintf(f, "    %s  get%s();\n", cType.c_str(), prop.c_str());
    }
    else if (opt->getValueType().compare("string") == 0)
    {
      std::string cType = "QString";
      fprintf(f, " private:\n");
      fprintf(f, "    QString m_%s;\n", prop.c_str());
      fprintf(f, " public:\n");
      fprintf(f, "    Q_PROPERTY(%s %s READ get%s WRITE set%s)\n", cType.c_str(), prop.c_str(), prop.c_str(), prop.c_str());
      fprintf(f, " public slots:\n");
      fprintf(f, "    void set%s(const %s &v);\n", prop.c_str(), cType.c_str());
      fprintf(f, " public:\n");
      fprintf(f, "    %s  get%s();\n\n", cType.c_str(), prop.c_str());
    }
    else
    {
      fprintf(f, "    Q_PROPERTY(%s %s READ get%s WRITE set%s)\n", typ.c_str(), prop.c_str(), prop.c_str(), prop.c_str());
      fprintf(f, "    QFILTERWIDGET_INSTANCE_PROPERTY(%s, %s)\n\n", typ.c_str(), prop.c_str());
    }
  }

  // This template function will generate all the necessary code to set the name of each
  // required and created array.
  appendArrayNameCodeToHeader<T>(t, f);

  fprintf(f, "  private:\n");
  fprintf(f, "    QString m_FilterGroup;\n\n");
  fprintf(f, "    Q%sWidget(const Q%sWidget&);\n", filter.c_str(), filter.c_str());
  fprintf(f, "    void operator=(const Q%sWidget&);\n", filter.c_str());
  fprintf(f, "};\n");
  fprintf(f, "#endif /* Q%s_H_ */\n", filter.c_str());

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
    if (itemsRead != 1)
    {

    }
    fclose(c);

    FILE* t = fopen(tempPath.c_str(), "rb");
    unsigned char* tempContents = reinterpret_cast<unsigned char*>(malloc(tempFileSize));
    itemsRead = fread(tempContents, tempFileSize, 1, t);
    if (itemsRead != 1)
    {

    }
    fclose(t);

    int result = ::memcmp(currentContents, tempContents, tempFileSize);
    if (result != 0)
    {
      std::cout << "1-Creating Header File: " <<completePath << std::endl;
      copyFile(tempPath, completePath);
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void createHTMLFragment( const std::string &group, const std::string &filter)
{

#if (GENERATE_HTML_FRAGMENT == 0)
  if (true) return;
#endif
  std::string s = FILTER_WIDGETS_TEMP_DIR();
  s.append("_html_fragment.html");
  FILE* f = fopen(s.c_str(), "ab+");

  typename T::Pointer t = T::New();


  VoxelDataContainer::Pointer m = VoxelDataContainer::New();
  t->setVoxelDataContainer(m.get());
  t->preflight();

  fprintf(f, "<a href=\"../%s/%s.html#wp2\">Description</a> ", group.c_str(), filter.c_str());
  fprintf(f, "| <a href=\"../%s/%s.html#wp3\">Options</a> ", group.c_str(), filter.c_str());
  fprintf(f, "| <a href=\"../%s/%s.html#wp4\">Required Arrays</a> ", group.c_str(), filter.c_str());
  fprintf(f, "| <a href=\"../%s/%s.html#wp5\">Created Arrays</a> ", group.c_str(), filter.c_str());
  fprintf(f, "| <a href=\"../%s/%s.html#wp1\">Authors</a> </p>\n\n\n", group.c_str(), filter.c_str());
  fclose(f);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void createOptionsWriterCode( const std::string &group, const std::string &filter)
{

#if (GENERATE_OPTIONS_WRITER_CODE == 0)
  if (true) return;
#endif
  typename T::Pointer t = T::New();


  std::stringstream ss;
  ss << FILTER_WIDGETS_SOURCE_DIR() << "/" << group << "Widgets/Q" << filter << "Widget.h";
  std::string completePath = MXADir::toNativeSeparators(ss.str());
//  if (MXADir::exists(completePath) == true)
//  {
//   std::cout << filter << ": FilterParameters file already exists in source directory. NOT generating FilterParametersWriter." << std::endl;
//   return;
//  }
  std::string origHeaderFile = completePath;
  ss.str("");
  ss << FILTER_WIDGETS_BINARY_DIR() << "/NameMapping/" << group << "/";

  MXADir::mkdir(ss.str(), true);

  ss << filter << "_NameMapping.h";

  completePath = MXADir::toNativeSeparators(ss.str());

  FILE* f = fopen(completePath.c_str(), "wb");
  if (NULL == f)
  {
    return;
  }


  VoxelDataContainer::Pointer m = VoxelDataContainer::New();
  t->setVoxelDataContainer(m.get());
  t->preflight();

  {
    std::set<std::string> list = t->getRequiredCellData();
    if(list.size() > 0)
    {
      fprintf(f, "//------ Required Cell Data\n");
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "DREAM3D_INSTANCE_STRING_PROPERTY(%sArrayName)\n", (*iter).c_str() );
      }
    }
  }
  {
    std::set<std::string> list = t->getCreatedCellData();
    if(list.size() > 0)
    {
      fprintf(f, "//------ Created Cell Data\n");
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "DREAM3D_INSTANCE_STRING_PROPERTY(%sArrayName)\n", (*iter).c_str() );
      }
    }
  }
  {
    std::set<std::string> list = t->getRequiredFieldData();
    if(list.size() > 0)
    {
      fprintf(f, "//------ Required Field Data\n");
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "DREAM3D_INSTANCE_STRING_PROPERTY(%sArrayName)\n", (*iter).c_str() );
      }
    }
  }
  {
    std::set<std::string> list = t->getCreatedFieldData();
    if(list.size() > 0)
    {
      fprintf(f, "//------ Created Field Data\n");
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "DREAM3D_INSTANCE_STRING_PROPERTY(%sArrayName)\n", (*iter).c_str() );
      }
    }
  }

  {
    std::set<std::string> list = t->getRequiredEnsembleData();
    if(list.size() > 0)
    {
      fprintf(f, "//------ Required Ensemble Data\n");
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "DREAM3D_INSTANCE_STRING_PROPERTY(%sArrayName)\n", (*iter).c_str() );
      }
    }
  }
  {
    std::set<std::string> list = t->getCreatedEnsembleData();
    if(list.size() > 0)
    {
      fprintf(f, "//------ Created Ensemble Data\n");
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "DREAM3D_INSTANCE_STRING_PROPERTY(%sArrayName)\n", (*iter).c_str() );
      }
    }
  }

  // -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  // BUild up the initializer List
  fprintf(f, "\n\n// These go in the constructors initializer list\n" );
  fprintf(f, "// They should be placed just under the call to the superclass\n" );
  fprintf(f, "// which in most cases is 'AbstractFilter()'\n");

  {
    std::set<std::string> list = t->getRequiredCellData();
    if(list.size() > 0)
    {
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "m_%sArrayName(DREAM3D::CellData::%s),\n", (*iter).c_str(), (*iter).c_str() );
      }
    }
  }
  {
    std::set<std::string> list = t->getCreatedCellData();
    if(list.size() > 0)
    {
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "m_%sArrayName(DREAM3D::CellData::%s),\n", (*iter).c_str(), (*iter).c_str() );
      }
    }
  }
  {
    std::set<std::string> list = t->getRequiredFieldData();
    if(list.size() > 0)
    {
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "m_%sArrayName(DREAM3D::FieldData::%s),\n", (*iter).c_str(), (*iter).c_str() );
      }
    }
  }
  {
    std::set<std::string> list = t->getCreatedFieldData();
    if(list.size() > 0)
    {
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "m_%sArrayName(DREAM3D::FieldData::%s),\n", (*iter).c_str(), (*iter).c_str() );
      }
    }
  }

  {
    std::set<std::string> list = t->getRequiredEnsembleData();
    if(list.size() > 0)
    {
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "m_%sArrayName(DREAM3D::EnsembleData::%s),\n", (*iter).c_str(), (*iter).c_str() );
      }
    }
  }
  {
    std::set<std::string> list = t->getCreatedEnsembleData();
    if(list.size() > 0)
    {
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "m_%sArrayName(DREAM3D::EnsembleData::%s),\n", (*iter).c_str(), (*iter).c_str() );
      }
    }
  }

  fclose(f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void createSourceFile( const std::string &group, const std::string &filter)
{

  createOptionsWriterCode<T>(group, filter);
  typename T::Pointer t = T::New();
  std::vector<FilterParameter::Pointer> options = t->getFilterParameters();

  std::stringstream ss;
  ss << FILTER_WIDGETS_SOURCE_DIR() << "/" << group << "Widgets/Q" << filter << "Widget.h";
  std::string completePath = MXADir::toNativeSeparators(ss.str());
  if(MXADir::exists(completePath) == true)
  {
    std::cout << filter << ": Header file already exists in source directory. NOT generating generic widget." << std::endl;
    return;
  }
  std::string origHeaderFile = completePath;
  ss.str("");
  ss << FILTER_WIDGETS_BINARY_DIR() << "/" << group << "Widgets/Q" << filter << "Widget.cpp";

  completePath = MXADir::toNativeSeparators(ss.str());

  ss.str("");
  ss << FILTER_WIDGETS_TEMP_DIR() << "/TEMP_WIDGET.cpp";
  std::string tempPath = ss.str();

  ss.str("");
  ss << "Q" << filter << "Widget.h";

  std::string headerFile = ss.str();

  FILE* f = fopen(tempPath.c_str(), "wb");

  fprintf(f, "/*\n");
  fprintf(f, "  This file was auto-generated from the program FilterWidgetCodeGen.cpp which is\n  itself generated during cmake time\n");
  fprintf(f, "  If you need to make changes to the code that is generated you will need to make\n  them in the original file. \n");
  fprintf(f, "  The code generated is based off values from the filter located at\n");
  if(FILTER_INCLUDE_PREFIX().empty() == true)
  {
    fprintf(f, "  %s/%s.h\n*/\n", group.c_str(), filter.c_str());
  }
  else
  {
    fprintf(f, "  %s/%s/%s.h\n*/\n", FILTER_INCLUDE_PREFIX().c_str(), group.c_str(), filter.c_str());
  }
  fprintf(f, "#include \"%s\"\n", headerFile.c_str());
  fprintf(f, "#include <QLineEdit>\n");
  fprintf(f, "#include <QCheckBox>\n");
  fprintf(f, "#include <QComboBox>\n");
  fprintf(f, "\n\n\n");
  fprintf(f, "// -----------------------------------------------------------------------------\n");
  fprintf(f, "// This file is generated by Qt's moc program which sets up the signals/slots\n");
  fprintf(f, "// for this class.\n");
  fprintf(f, "#include \"moc_Q%sWidget.cxx\"\n", filter.c_str());

  fprintf(f, "// -----------------------------------------------------------------------------\n");
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
    else
    {
      fprintf(f, "     set%s( filter->get%s() );\n", prop.c_str(), prop.c_str());
    }
  }

  // Generate code to get all the array names and set the local variables that hold those names
  appendArrayNameConstructorCode<T>(t, f);

  // Finish Writing the remainder of the constructor code
  fprintf(f, "     m_FilterGroup = QString::fromStdString(filter->getGroupName());\n");
  fprintf(f, "     setupGui();\n");
  fprintf(f, "     setTitle(QString::fromStdString(filter->getHumanLabel()));\n");
  fprintf(f, "}\n\n");

  fprintf(f, "// -----------------------------------------------------------------------------\n");
  fprintf(f, "Q%sWidget::~Q%sWidget(){}\n\n", filter.c_str(), filter.c_str());

  fprintf(f, "// -----------------------------------------------------------------------------\n");
  fprintf(f, "QString Q%sWidget::getFilterGroup() {\n  return m_FilterGroup;\n}\n\n", filter.c_str() );

  fprintf(f, "// -----------------------------------------------------------------------------\n");
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
    else
    {
      fprintf(f, "  filter->set%s( get%s() );\n", prop.c_str(), prop.c_str());
    }
  }
  // Generate all the source code to set the various array names into the filter
  appendArrayNameGetFilterCode<T>(t, f);

  fprintf(f, "  return filter;\n");
  fprintf(f, "}\n");

  fprintf(f, "// -----------------------------------------------------------------------------\n");
  fprintf(f, "QFilterWidget* Q%sWidget::createDeepCopy() \n{\n", filter.c_str());
  fprintf(f, "  Q%sWidget* w = new Q%sWidget(NULL);\n", filter.c_str(), filter.c_str());
  for (size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    std::string prop = opt->getPropertyName();
    std::string typ = opt->getValueType();
    if(opt->getValueType().compare("string") == 0)
    {
      fprintf(f, "  w->set%s( get%s() );\n", prop.c_str(), prop.c_str());
    }
    else
    {
      fprintf(f, "  w->set%s( get%s() );\n", prop.c_str(), prop.c_str());
    }
  }
  // Generate the code that will push the names of the arrays to the deep copy
  appendArrayNameDeepCopyCode<T>(t, f);

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

      fprintf(f, "// -----------------------------------------------------------------------------\n");
      fprintf(f, "void Q%sWidget::set%s(%s v)\n{\n  m_%s = static_cast<%s>(v);\n", filter.c_str(), prop.c_str(), cType.c_str(), prop.c_str(), typ.c_str());
      fprintf(f, "  emit parametersChanged();\n}\n");
      fprintf(f, "// -----------------------------------------------------------------------------\n");
      fprintf(f, "%s  Q%sWidget::get%s() { \n  return static_cast<%s>(m_%s); \n}\n", cType.c_str(), filter.c_str(), prop.c_str(), typ.c_str(), prop.c_str());
    }
    else if(opt->getValueType().compare("string") == 0)
    {
      std::string cType = "QString";

      fprintf(f, "// -----------------------------------------------------------------------------\n");
      fprintf(f, "void Q%sWidget::set%s(const %s &v)\n{\n  m_%s = v;\n", filter.c_str(), prop.c_str(), cType.c_str(), prop.c_str());
      fprintf(f, "  emit parametersChanged();\n}\n");

      fprintf(f, "// -----------------------------------------------------------------------------\n");
      fprintf(f, "%s  Q%sWidget::get%s()\n{ \n  return m_%s; \n}\n", cType.c_str(), filter.c_str(), prop.c_str(), prop.c_str());
    }
    else
    {
      // If we fall in here then the methods should have been generated in the header file
      //    fprintf(f, "Q_PROPERTY(%s %s READ get%s WRITE set%s)\n", typ.c_str(), prop.c_str(), prop.c_str(), prop.c_str());
      //     fprintf(f, "FILTER_PROPERTY_WRAPPER(%s, %s, m_Filter);\n", typ.c_str(), prop.c_str());
    }
  }

  fprintf(f, "// -----------------------------------------------------------------------------\n");
  fprintf(f, "void Q%sWidget::writeOptions(QSettings &prefs)\n{\n", filter.c_str());
  fprintf(f, "  prefs.setValue(\"Filter_Name\", \"%s\" );\n", filter.c_str());
  for (size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    std::string prop = opt->getPropertyName();
    std::string typ = opt->getValueType();

    fprintf(f, "  prefs.setValue(\"%s\", get%s() );\n", prop.c_str(), prop.c_str());
  }
  fprintf(f, "}\n");

  fprintf(f, "// -----------------------------------------------------------------------------\n");
  fprintf(f, "void Q%sWidget::readOptions(QSettings &prefs)\n{\n", filter.c_str());
  // fprintf(f, "  std::cout << \"Reading Prefs for Filter  %s \" << std::endl;\n", filter.c_str());

  for (size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    std::string prop = opt->getPropertyName();
    std::string typ = opt->getValueType();
    std::string hl = opt->getHumanLabel();
    fprintf(f, "  {\n   QVariant p_%s = prefs.value(\"%s\");\n", prop.c_str(), prop.c_str());

    if(opt->getWidgetType() == FilterParameter::StringWidget)
    {
      fprintf(f, "   QLineEdit* le = findChild<QLineEdit*>(\"%s\");\n", prop.c_str());
      fprintf(f, "   if (le) { le->setText(p_%s.toString()); }\n", prop.c_str());
    }
    else if(opt->getWidgetType() == FilterParameter::IntWidget)
    {
      fprintf(f, "   QLineEdit* le = findChild<QLineEdit*>(\"%s\");\n", prop.c_str());
      fprintf(f, "   if (le) { le->setText(p_%s.toString()); }\n", prop.c_str());
    }
    else if(opt->getWidgetType() == FilterParameter::DoubleWidget)
    {
      fprintf(f, "   QLineEdit* le = findChild<QLineEdit*>(\"%s\");\n", prop.c_str());
      fprintf(f, "   if (le) { le->setText(p_%s.toString());}\n", prop.c_str());
    }
    else if(opt->getWidgetType() == FilterParameter::InputFileWidget
        || opt->getWidgetType() == FilterParameter::InputPathWidget
        || opt->getWidgetType() == FilterParameter::OutputFileWidget
        || opt->getWidgetType() == FilterParameter::OutputPathWidget)
    {
      fprintf(f, "   QLineEdit* lb = qFindChild<QLineEdit*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "   if (lb) { lb->setText(p_%s.toString()); }\n", prop.c_str());
      fprintf(f, "   set%s(p_%s.toString());\n", prop.c_str(), prop.c_str());
    }
    else if(opt->getWidgetType() == FilterParameter::BooleanWidget)
    {
      fprintf(f, "   QCheckBox* le = findChild<QCheckBox*>(\"%s\");\n", prop.c_str());
      fprintf(f, "   if (le) { le->setChecked(p_%s.toBool()); }\n", prop.c_str());
    }
    #if 0
    else if(opt->getWidgetType() == FilterParameter::IntConstrainedWidget)
    {
      fprintf(f, "assert(false);\n");
    }
    else if(opt->getWidgetType() == FilterParameter::DoubleConstrainedWidget)
    {
      fprintf(f, "assert(false);\n");
    }
    #endif
    else if(opt->getWidgetType() == FilterParameter::ChoiceWidget)
    {
      fprintf(f, "   QComboBox* cb = findChild<QComboBox*>(\"%s\");\n", prop.c_str());
      fprintf(f, "   if (cb) {\n");
      fprintf(f, "     bool ok = false;\n");
      fprintf(f, "     if (p_%s.toInt(&ok) < cb->count()) {\n", prop.c_str());
      fprintf(f, "       cb->setCurrentIndex(p_%s.toInt());\n", prop.c_str());
      fprintf(f, "     }\n");
      fprintf(f, "   }\n");
    }
    else
    {
      fprintf(f, " #error: Class %s  Property %s  NOTHING WAS GENERATED TO READ/WRITE PROPERTY\n", filter.c_str(), prop.c_str());
    }
    fprintf(f, "  }\n");
  }
  fprintf(f, "\n}\n\n\n");

  // This template function will generate all the necessary code to set the name of each
  // required and created array.
  appendArrayNameCodeToSource<T>(t, f);



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

    FILE* t = fopen(tempPath.c_str(), "rb");
    unsigned char* tempContents = reinterpret_cast<unsigned char*>(malloc(tempFileSize));
    itemsRead = fread(tempContents, tempFileSize, 1, t);
    if(itemsRead != 1)
    {

    }
    fclose(t);

    int result = ::memcmp(currentContents, tempContents, tempFileSize);
    if(result != 0)
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
void createListFile( const std::string &group, const std::string &filter)
{
  std::string s = FILTER_WIDGETS_TEMP_DIR();
  s.append("/Mike_list.txt");
  FILE* f = fopen(s.c_str(), "ab+");
  fprintf(f, "FILTERNAME: %s\n", filter.c_str());

  typename T::Pointer t = T::New();
   std::vector<FilterParameter::Pointer> options = t->getFilterParameters();

  fprintf(f, "** OPTIONS **\n");

    for (size_t i = 0; i < options.size(); ++i)
    {

      FilterParameter::Pointer opt = options[i];
      std::string prop = opt->getPropertyName();
      std::string typ = opt->getValueType();
      std::string hl = opt->getHumanLabel();
      fprintf(f, "  NAME: %s\n", hl.c_str());

      if(opt->getWidgetType() == FilterParameter::IntWidget)
      {
        fprintf(f, "  TYPE: Integer");
      }
      else if(opt->getWidgetType() == FilterParameter::DoubleWidget)
      {
        fprintf(f, "  TYPE: Double");
      }
      else if(opt->getWidgetType() == FilterParameter::InputFileWidget)
      {
        fprintf(f, "  TYPE: Input File");
      }
      else if(opt->getWidgetType() == FilterParameter::OutputFileWidget)
      {
        fprintf(f, "  TYPE: Output File");
      }
      else if(opt->getWidgetType() == FilterParameter::BooleanWidget)
      {
        fprintf(f, "  TYPE: Boolean (On or Off)");
      }
      #if 0
      else if(opt->getWidgetType() == FilterParameter::IntConstrainedWidget)
      {
        fprintf(f, "  TYPE: Bounded Integer");
      }
      else if(opt->getWidgetType() == FilterParameter::DoubleConstrainedWidget)
      {
        fprintf(f, "  TYPE: Bounded Double");
      }
      #endif
      else if(opt->getWidgetType() == FilterParameter::ChoiceWidget)
      {
        fprintf(f, "  TYPE: Choices");
      }
      else
      {
        fprintf(f, "  TYPE: Unknown Type");
      }
      fprintf(f, "\n");
      fprintf(f, "  UNITS: \n");
      fprintf(f, "  DESCRIPTION: \n\n");
    }

    VoxelDataContainer::Pointer m = VoxelDataContainer::New();
    t->setVoxelDataContainer(m.get());
    t->preflight();


    fprintf(f, "** REQUIRED ARRAYS **\n");
    {
      std::set<std::string> list = t->getRequiredCellData();
      if(list.size() > 0)
      {
        for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
        {
          fprintf(f, "  NAME:%s\n  TYPE: Cell\n  HOW_USED:\n  DEFINITION:\n\n",(*iter).c_str());
        }
      }
    }
    {
      std::set<std::string> list = t->getRequiredFieldData();
      if(list.size() > 0)
      {
        for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
        {
          fprintf(f, "  NAME:%s\n  TYPE: Field\n  HOW_USED:\n  DEFINITION:\n\n",(*iter).c_str());
        }
      }
    }
    {
      std::set<std::string> list = t->getRequiredEnsembleData();
      if(list.size() > 0)
      {
        for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
        {
          fprintf(f, "  NAME:%s\n  TYPE: Ensemble\n  HOW_USED:\n  DEFINITION:\n\n",(*iter).c_str());
        }
      }
    }

    fprintf(f, "** CREATED ARRAYS **\n");
    {
      std::set<std::string> list = t->getCreatedCellData();
      if(list.size() > 0)
      {
        for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
        {
          fprintf(f, "  NAME:%s\n  TYPE: Cell\n  HOW_USED:\n  DEFINITION:\n\n",(*iter).c_str());
        }
      }
    }

    {
      std::set<std::string> list = t->getCreatedFieldData();
      if(list.size() > 0)
      {
        for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
        {
          fprintf(f, "  NAME:%s\n  TYPE: Field\n  HOW_USED:\n  DEFINITION:\n\n",(*iter).c_str());
        }
      }
    }


    {
      std::set<std::string> list = t->getCreatedEnsembleData();
      if(list.size() > 0)
      {
        for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
        {
          fprintf(f, "  NAME:%s\n  TYPE: Ensemble\n  HOW_USED:\n  DEFINITION:\n\n",(*iter).c_str());
        }
      }
    }
    fprintf(f, "===================================================================================\n");
    fclose(f);
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



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void createHTMLFile( const std::string &group, const std::string &filter)
{
#if (GENERATE_FILTER_TEXT_LIST == 1)
  createListFile<T>(group, filter);
#endif

#if (GENERATE_HTML_FRAGMENT == 1)
  createHTMLFragment<T>(group, filter);
#endif

#if (GENERATE_PREFLIGHT_TEST_CODE_FRAGMENT == 1)
  createPreflightTestCode<T>(group, filter);
#endif

#if (GENERATE_HTML_FILE == 0)
  return;
#endif
  typename T::Pointer t = T::New();
  std::vector<FilterParameter::Pointer> options = t->getFilterParameters();

  std::stringstream ss;
  ss << FILTER_WIDGETS_DOCS_DIR() << "/" << t->getGroupName() << "Filters/" << filter << ".html";

  std::string completePath = MXADir::toNativeSeparators(ss.str());
  if(MXADir::exists(completePath) == true)
  {
    // std::cout << "EXISTS HTML File:" << ss.str() << std::endl;
    return;
  }
  else
  {
    std::cout << "MISSING HTML File: " << ss.str() << std::endl;
  }

  ss.str("");
#if (OVERWRITE_HTML_DOCS == 1)
  ss << FILTER_WIDGETS_DOCS_DIR() << "/" << t->getGroupName() << "Filters/";
#else
  ss << FILTER_WIDGETS_BINARY_DIR() << "/Documentation/Filters/" << group;
#endif
  MXADir::mkdir(ss.str(), true);
  ss << "/" << filter << ".html";

  completePath = MXADir::toNativeSeparators(ss.str());

  ss.str("");
  std::cout << "CREATING HTML File: ";
#if (OVERWRITE_HTML_DOCS == 1)
      ss << FILTER_WIDGETS_DOCS_DIR() << "/" << t->getGroupName() << "Filters/" << filter << ".html";
#else
      << FILTER_WIDGETS_BINARY_DIR()
      << "/Documentation/Filters/" << t->getGroupName() << "/" << filter << ".html" << std::endl;
#endif
  const char* groupName = t->getGroupName().c_str();

  FILE* f = fopen(completePath.c_str(), "wb");

  fprintf(f, "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n");
  fprintf(f, "<html>\n");
  fprintf(f, "<head>\n");
  fprintf(f, "<meta name=\"qrichtext\" content=\"1\" />\n");
  fprintf(f, "<style type=\"text/css\">\n");
  fprintf(f, "h1.pHeading1 { color: #003366; font-family: Arial, Verdana, Helvetica, sans-serif; font-size: x-large; font-weight: bold; text-align: left }\n");
  fprintf(f, "h2.pHeading2 { color: #003366; font-family: Arial, Verdana, Helvetica, sans-serif; font-size: large; font-weight: bold; text-align: left }\n");
  fprintf(f, "p.pBody { font-family: Arial, Verdana, Helvetica, sans-serif; font-size: medium; text-align: left }\n");
  fprintf(f, "p.pCellBody { font-family: Arial, Verdana, Helvetica, sans-serif; font-size: medium; text-align: left }\n");
  fprintf(f, "#footer\n{\n\
   font-family: Arial, Verdana, Helvetica, sans-serif;\n\
   font-color:Blue;\n\
   font-size:small;\n\
   background-color:#CCCCCC;\n\
   padding:0pt;\n\
   position:fixed;\n\
   bottom:1%%;\n\
   left:1%%;\n\
   width:98%%;\n}\n");
  fprintf(f, "</style>\n");
  fprintf(f, "<title>%s</title>\n", t->getHumanLabel().c_str());
  fprintf(f, "</head>\n");
  fprintf(f, "<body>\n");
  fprintf(f, "<h1 class=\"pHeading1\">%s Filter</h1>\n", t->getHumanLabel().c_str());
  fprintf(f, "<p class=\"pCellBody\">\n");
  fprintf(f, "<a href=\"../%sFilters/%s.html#wp2\">Description</a>\n", groupName, filter.c_str());
  fprintf(f, "| <a href=\"../%sFilters/%s.html#wp3\">Options</a>\n", groupName, filter.c_str());
  fprintf(f, "| <a href=\"../%sFilters/%s.html#wp4\">Required Arrays</a>\n", groupName, filter.c_str());
  fprintf(f, "| <a href=\"../%sFilters/%s.html#wp5\">Created Arrays</a>\n", groupName, filter.c_str());
  fprintf(f, "| <a href=\"../%sFilters/%s.html#wp1\">Authors</a> </p>\n\n", groupName, filter.c_str());
  fprintf(f, "<a name=\"wp7\"></a>\n");
  fprintf(f, "<h2 class=\"pHeading2\">Group</h2>\n");
  fprintf(f, "<p class=\"pBody\">%s</p>\n\n", groupName);

  fprintf(f, "<a name=\"wp2\"> </a>");
  fprintf(f, "<h2 class=\"pHeading2\">Description</h2>\n");
  fprintf(f, "<!-- Write all your documentation here -->\n\n<p class=\"pBody\">\n");
//  fprintf(f, "This filter does ....\n");
  fprintf(f, "</p>\n\n<!-- Do NOT write documentation below this line -->\n");
  fprintf(f, "<!-- DREAM3D AUTO-GENERATED DOCUMENTATION START -->\n");
  fprintf(f, "\n<!-- A TABLE OF Parameters FOR YOUR FILTER -->\n");
  fprintf(f, "<a name=\"wp3\"> </a><h2 class=\"pHeading2\">Options</h2>\n");
  if (options.size() > 0) {
    fprintf(f, "<table border=\"0\" cellpadding=\"4\" cellspacing=\"1\">\n");
    fprintf(f, "<tr bgcolor=\"#A2A2A2\"><th>Name</th><th>Type</th></tr>\n");
  }
  for (size_t i = 0; i < options.size(); ++i)
  {
    fprintf(f, "<tr bgcolor=\"#E2E2E2\">\n");
    FilterParameter::Pointer opt = options[i];
    std::string prop = opt->getPropertyName();
    std::string typ = opt->getValueType();
    std::string hl = opt->getHumanLabel();
    fprintf(f, "<td>%s</td>", hl.c_str());

    if(opt->getWidgetType() == FilterParameter::IntWidget)
    {
      fprintf(f, "<td>Integer</td>");
    }
    else if(opt->getWidgetType() == FilterParameter::DoubleWidget)
    {
      fprintf(f, "<td>Double</td>");
    }
    else if(opt->getWidgetType() == FilterParameter::InputFileWidget)
    {
      fprintf(f, "<td>Input File</td>");
    }
    else if(opt->getWidgetType() == FilterParameter::OutputFileWidget)
    {
      fprintf(f, "<td>Output File</td>");
    }
    else if(opt->getWidgetType() == FilterParameter::BooleanWidget)
    {
      fprintf(f, "<td>Boolean (On or Off)</td>");
    }
    #if 0
    else if(opt->getWidgetType() == FilterParameter::IntConstrainedWidget)
    {
      fprintf(f, "<td>Bounded Integer</td>");
    }
    else if(opt->getWidgetType() == FilterParameter::DoubleConstrainedWidget)
    {
      fprintf(f, "<td>Bounded Double</td>");
    }
    #endif
    else if(opt->getWidgetType() == FilterParameter::ChoiceWidget)
    {
      fprintf(f, "<td>Choices</td>");
    }
    else
    {
      fprintf(f, "<td>Unknown Type</td>");
    }
    fprintf(f, "</tr>\n");

  }
  if (options.size() > 0) {
  fprintf(f, "</table>\n");
  }
  VoxelDataContainer::Pointer m = VoxelDataContainer::New();
  t->setVoxelDataContainer(m.get());
  t->preflight();

  fprintf(f, "\n<!-- A table of Required Data for your filter -->\n");
  fprintf(f, "<a name=\"wp4\"> </a><h2 class=\"pHeading2\">Required Arrays</h2>\n");
  fprintf(f, "<table border=\"0\" cellpadding=\"4\" cellspacing=\"1\">\n");
  fprintf(f, "<tr bgcolor=\"#A2A2A2\"><th>Type</th><th>Name</th><th>Comment</th></tr>\n");
  {
    std::set<std::string> list = t->getRequiredCellData();
    if(list.size() > 0)
    {
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "<tr bgcolor=\"#E2E2E2\"><td>Cell</td><td>%s</td><td></td></tr>\n",(*iter).c_str());
      }
    }
  }
  {
    std::set<std::string> list = t->getRequiredFieldData();
    if(list.size() > 0)
    {
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "<tr bgcolor=\"#E2E2E2\"><td>Field</td><td>%s</td><td></td></tr>\n",(*iter).c_str());
      }
    }
  }
  {
    std::set<std::string> list = t->getRequiredEnsembleData();
    if(list.size() > 0)
    {
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "<tr bgcolor=\"#E2E2E2\"><td>Ensemble</td><td>%s</td><td></td></tr>\n",(*iter).c_str());
      }
    }
  }
  fprintf(f, "</table>\n");

  fprintf(f, "\n<!-- A table of Created Data for your filter -->\n");
  fprintf(f, "<a name=\"wp5\"> </a><h2 class=\"pHeading2\">Created Arrays</h2>\n");
  fprintf(f, "<table border=\"0\" cellpadding=\"4\" cellspacing=\"1\">\n");
  fprintf(f, "<tr bgcolor=\"#A2A2A2\"><th>Type</th><th>Name</th><th>Comment</th></tr>\n");
  {
    std::set<std::string> list = t->getCreatedCellData();
    if(list.size() > 0)
    {
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "<tr bgcolor=\"#E2E2E2\"><td>Cell</td><td>%s</td><td></td></tr>\n",(*iter).c_str());
      }
    }
  }

  {
    std::set<std::string> list = t->getCreatedFieldData();
    if(list.size() > 0)
    {
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "<tr bgcolor=\"#E2E2E2\"><td>Field</td><td>%s</td><td></td></tr>\n",(*iter).c_str());
      }
    }
  }


  {
    std::set<std::string> list = t->getCreatedEnsembleData();
    if(list.size() > 0)
    {
      for (std::set<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter)
      {
        fprintf(f, "<tr bgcolor=\"#E2E2E2\"><td>Ensemble</td><td>%s</td><td></td></tr>\n",(*iter).c_str());
      }
    }
  }
  fprintf(f, "</table>\n");

  t->setVoxelDataContainer(NULL);

  fprintf(f, "\n<a name=\"wp1\"> </a><h2 class=\"pHeading2\">Authors</h2>\n<p class=\"pBody\">\n");

  fprintf(f, "Copyright [INSERT YOUR NAME HERE]<br />\n");

  fprintf(f, "Contact Info:[INSERT EMAIL ADDRESS HERE]<br />\n");
  fprintf(f, "Version: 1.0.0<br />\n");
  fprintf(f, "License: See the License.txt file that came with DREAM3D.<br />\n");
  fprintf(f, "</p>\n");
  fprintf(f, "<!-- DREAM3D AUTO-GENERATED DOCUMENTATION END -->\n");
  fprintf(f, "<div>\
  <table width=\"98%%\" border=\"0\" bgcolor=\"#CCCCCC\"><tr><td><a href=\"../index.html\">Index</a></td></tr></table>\
  </div>\n");
  fprintf(f, "</body>\n");
  fprintf(f, "</html>\n");

  fclose(f);
}



#include "FilterWidgetCodeGen.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{

#if (GENERATE_FILTER_TEXT_LIST == 1)
  std::string s = FILTER_WIDGETS_TEMP_DIR();
  s.append("html_fragment.html");
  FILE* f = fopen(s.c_str(), "wb"); // Clear out this file
  fclose(f);
#endif


#if (GENERATE_HTML_FRAGMENT == 1)
  std::string s = FILTER_WIDGETS_TEMP_DIR();
  s.append("Mike_list.txt");
  FILE* f = fopen(s.c_str(), "wb"); // Clear out this file
  fclose(f);
#endif

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

  return 0;
}


