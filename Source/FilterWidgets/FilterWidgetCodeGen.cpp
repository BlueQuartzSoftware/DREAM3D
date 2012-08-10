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


#define GENERATE_FILTER_TEXT_LIST 0
#define GENERATE_HTML_FRAGMENT 0
#define GENERATE_HTML_FILE 0
#define OVERWRITE_SOURCE_DOCS 0
#define GENERATE_OPTIONS_WRITER_CODE 0

typedef std::map<std::string, std::set<std::string> >  FilterMapType;
typedef std::set<std::string>  StringSetType;

// These will be defined in an include header file below.
std::string FILTER_WIDGETS_BINARY_DIR();
std::string FILTER_WIDGETS_SOURCE_DIR();
std::string FILTER_WIDGETS_TEMP_DIR();
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
      fprintf(f, "    %s  get%s();\n", cType.c_str(), prop.c_str());
    }
    else
    {
      fprintf(f, "    Q_PROPERTY(%s %s READ get%s WRITE set%s)\n", typ.c_str(), prop.c_str(), prop.c_str(), prop.c_str());
      fprintf(f, "    QFILTERWIDGET_INSTANCE_PROPERTY(%s, %s)\n", typ.c_str(), prop.c_str());
    }
  }

  fprintf(f, "  private:\n");
 // fprintf(f, "    %s::Pointer m_Filter;\n\n", filter.c_str());
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


  DataContainer::Pointer m = DataContainer::New();
  t->setDataContainer(m.get());
  t->preflight();

  fprintf(f, "<a href=\"../%s/%s.html#wp2\">Description</a> ", group.c_str(), filter.c_str());
  fprintf(f, "| <a href=\"../%s/%s.html#wp3\">Options</a> ", group.c_str(), filter.c_str());
  fprintf(f, "| <a href=\"../%s/%s.html#wp4\">Required Arrays</a> ", group.c_str(), filter.c_str());
  fprintf(f, "| <a href=\"../%s/%s.html#wp5\">Created Arrays</a> ", group.c_str(), filter.c_str());
  fprintf(f, "| <a href=\"../%s/%s.html#wp1\">Authors</a> </p>\n\n\n", group.c_str(), filter.c_str());
  fclose(f);
}



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


  DataContainer::Pointer m = DataContainer::New();
  t->setDataContainer(m.get());
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
  fprintf(f, "#include \"%s\"\n\n\n", headerFile.c_str());

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

  fprintf(f, "     setupGui();\n");
  fprintf(f, "     setTitle(QString::fromStdString(filter->getHumanLabel()));\n");
  fprintf(f, "}\n\n");

  fprintf(f, "// -----------------------------------------------------------------------------\n");
  fprintf(f, "Q%sWidget::~Q%sWidget(){}\n\n", filter.c_str(), filter.c_str());

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
    else if(opt->getWidgetType() == FilterParameter::IntConstrainedWidget)
    {
      fprintf(f, "assert(false);\n");
    }
    else if(opt->getWidgetType() == FilterParameter::DoubleConstrainedWidget)
    {
      fprintf(f, "assert(false);\n");
    }
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
      else if(opt->getWidgetType() == FilterParameter::IntConstrainedWidget)
      {
        fprintf(f, "  TYPE: Bounded Integer");
      }
      else if(opt->getWidgetType() == FilterParameter::DoubleConstrainedWidget)
      {
        fprintf(f, "  TYPE: Bounded Double");
      }
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

    DataContainer::Pointer m = DataContainer::New();
    t->setDataContainer(m.get());
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
void createHTMLFile( const std::string &group, const std::string &filter)
{
#if (GENERATE_FILTER_TEXT_LIST == 1)
  createListFile<T>(group, filter);
#endif

#if (GENERATE_HTML_FRAGMENT == 1)
  createHTMLFragment<T>(group, filter);
#endif

#if (GENERATE_HTML_FILE == 0)
  return;
#endif
  typename T::Pointer t = T::New();
  std::vector<FilterParameter::Pointer> options = t->getFilterParameters();

  std::stringstream ss;
  ss << FILTER_WIDGETS_SOURCE_DIR() << "/" << group << "/" << filter << ".html";

  std::string completePath = MXADir::toNativeSeparators(ss.str());
  if(MXADir::exists(completePath) == true)
  {
    std::cout << filter << ": HTML already exists NOT generating a generic file." << std::endl;
    return;
  }

  ss.str("");
#if (OVERWRITE_SOURCE_DOCS == 1)
  ss << DREAM3D_SOURCE_DIR() << "/Documentation/Filters/" << group;
#else
  ss << FILTER_WIDGETS_BINARY_DIR() << "/Documentation/Filters/" << group;
#endif
  MXADir::mkdir(ss.str(), true);
  ss << "/" << filter << ".html";

  completePath = MXADir::toNativeSeparators(ss.str());

  ss.str("");

 // std::cout << "Creating HTML File: " << completePath << std::endl;
  std::cout << "Creating HTML File: "
#if (OVERWRITE_SOURCE_DOCS == 1)
      << DREAM3D_SOURCE_DIR()
#else
      << FILTER_WIDGETS_BINARY_DIR()
#endif
      << "/Documentation/Filters/" << group << "/" << filter << ".html" << std::endl;

  FILE* f = fopen(completePath.c_str(), "wb");

  fprintf(f, "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">");
  fprintf(f, "<html>\n");
  fprintf(f, "<head>\n");
  fprintf(f, "<meta name=\"qrichtext\" content=\"1\" />\n");
  fprintf(f, "<style type=\"text/css\">\n");
  fprintf(f, "h1.pHeading1 { color: #003366; font-family: Arial, Verdana, Helvetica, sans-serif; font-size: x-large; font-weight: bold; text-align: left }\n");
  fprintf(f, "h2.pHeading2 { color: #003366; font-family: Arial, Verdana, Helvetica, sans-serif; font-size: large; font-weight: bold; text-align: left }\n");
  fprintf(f, "p.pBody { font-family: Arial, Verdana, Helvetica, sans-serif; font-size: medium; text-align: left }\n");
  fprintf(f, "p.pCellBody { font-family: Arial, Verdana, Helvetica, sans-serif; font-size: medium; text-align: left }\n");
  fprintf(f, "</style>\n");
  fprintf(f, "<title>%s</title>\n", t->getHumanLabel().c_str());
  fprintf(f, "</head>\n");
  fprintf(f, "<body>\n");
  fprintf(f, "<h1 class=\"pHeading1\">%s Filter</h1>\n<p class=\"pCellBody\">\n", t->getHumanLabel().c_str());
  fprintf(f, "<a href=\"MFESurfaceSmoothingFilter.html#wp2\">Description</a> ");
  fprintf(f, "| <a href=\"MFESurfaceSmoothingFilter.html#wp3\">Options</a> ");
  fprintf(f, "| <a href=\"MFESurfaceSmoothingFilter.html#wp4\">Required Arrays</a> ");
  fprintf(f, "| <a href=\"MFESurfaceSmoothingFilter.html#wp5\">Created Arrays</a>");
  fprintf(f, "| <a href=\"MFESurfaceSmoothingFilter.html#wp1\">Authors</a> </p>\n\n");
  fprintf(f, "<a name=\"wp7\"></a>\n");
  fprintf(f, "<h2 class=\"pHeading2\">Group</h2>\n");
  fprintf(f, "<p class=\"pBody\">%s</p>\n\n", group.c_str());

  fprintf(f, "<a name=\"wp2\"> </a>");
  fprintf(f, "<h2 class=\"pHeading2\">Description</h2>\n<p class=\"pBody\">\n");
  fprintf(f, "<!-- Write all your documentation here -->\n\n");
//  fprintf(f, "This filter does ....\n");
  fprintf(f, "<!-- Do NOT write documentation below this line -->\n</p>\n\n");
  fprintf(f, "<!-- DREAM3D AUTO-GENERATED DOCUMENTATION START -->\n");
  fprintf(f, "<!-- A TABLE OF Options FOR YOUR FILTER -->\n");
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
    else if(opt->getWidgetType() == FilterParameter::IntConstrainedWidget)
    {
      fprintf(f, "<td>Bounded Integer</td>");
    }
    else if(opt->getWidgetType() == FilterParameter::DoubleConstrainedWidget)
    {
      fprintf(f, "<td>Bounded Double</td>");
    }
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
  DataContainer::Pointer m = DataContainer::New();
  t->setDataContainer(m.get());
  t->preflight();

  fprintf(f, "<!-- A table of Required Data for your filter -->\n");
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

  fprintf(f, "<!-- A table of Created Data for your filter -->\n");
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

  t->setDataContainer(NULL);

  fprintf(f, "<a name=\"wp1\"> </a><h2 class=\"pHeading2\">Authors</h2>\n<p class=\"pBody\">\n");

  fprintf(f, "Copyright 2012 Michael A. Groeber (AFRL), ");
  fprintf(f, "Michael A. Jackson (BlueQuartz Software)<br />\n");
  fprintf(f, "Contact Info: dream3d@bluequartz.net<br />\n");
  fprintf(f, "Version: 1.0.0\n");
  fprintf(f, "License: See the License.txt file that came with DREAM3D.<br />\n");
  fprintf(f, "</p>\n");

  fprintf(f, "<!-- DREAM3D AUTO-GENERATED DOCUMENTATION END -->\n");

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

  setFilters();

  return 0;
}


