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
void createHeaderFile(const std::string &group, const std::string &filter, std::vector<FilterParameter::Pointer> options, const std::string &outSubPath)
{
  std::stringstream ss;
  std::string completePath;
  ss << FILTER_WIDGETS_BINARY_DIR() << "/" << outSubPath;

  completePath = MXADir::toNativeSeparators(ss.str());
  // Make sure the output path exists
  std::string parentPath = MXADir::parentPath(completePath);
  if (MXADir::exists(parentPath) == false)
  {
    MXADir::mkdir(ss.str(), true);
  }

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

  fprintf(f, "#ifndef _Q%sWidget_H_\n", filter.c_str());
  fprintf(f, "#define _Q%sWidget_H_\n\n", filter.c_str());

  fprintf(f, "#include <QtCore/QObject>\n");
  fprintf(f, "#include <QtCore/QSettings>\n\n");

  fprintf(f, "#include \"PipelineBuilder/QFilterWidget.h\"\n");
  fprintf(f, "#include \"DREAM3DLib/Common/DREAM3DSetGetMacros.h\"\n");
  fprintf(f, "#include \"DREAM3DLib/Common/FilterParameter.h\"\n\n");
  if (FILTER_INCLUDE_PREFIX().empty() == true) {
    fprintf(f, "#include \"%s/%s.h\"\n", group.c_str(), filter.c_str());
  }
  else
  {
    fprintf(f, "#include \"%s/%s/%s.h\"\n", FILTER_INCLUDE_PREFIX().c_str(), group.c_str(), filter.c_str());
  }
  fprintf(f, "class Q%sWidget : public QFilterWidget \n{\n", filter.c_str());
  fprintf(f, "   Q_OBJECT\n");
  fprintf(f, "  public:\n");
  fprintf(f, "    Q%sWidget(QWidget* parent = NULL);\n", filter.c_str());
  fprintf(f, "    virtual ~Q%sWidget();\n", filter.c_str());
  fprintf(f, "    virtual AbstractFilter::Pointer getFilter();\n");
  fprintf(f, "    void writeOptions(QSettings &prefs);\n");
  fprintf(f, "    void readOptions(QSettings &prefs);\n\n");
  fprintf(f, "    QFilterWidget* createDeepCopy();\n\n");
  fprintf(f, "    QString getFilterGroup();\n\n");
  fprintf(f, "    QString getFilterSubGroup();\n\n");

  bool implementArrayNameComboBoxUpdated = false;

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
    else if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget)
    {
      fprintf(f, "  public:\n");
      fprintf(f, "    virtual void preflightAboutToExecute(VoxelDataContainer::Pointer vdc, SurfaceMeshDataContainer::Pointer smdc, SolidMeshDataContainer::Pointer sdc);\n");
      fprintf(f, "\n\n");
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
    else if (opt->getWidgetType() == FilterParameter::ComparisonSelectionWidget)
    {
      fprintf(f, "  public:\n");
      fprintf(f, "    virtual void preflightAboutToExecute(VoxelDataContainer::Pointer vdc, SurfaceMeshDataContainer::Pointer smdc, SolidMeshDataContainer::Pointer sdc);\n");
      fprintf(f, "\n\n");
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


  // This template function will generate all the necessary code to set the name of each
  // required and created array.
  //appendArrayNameCodeToHeader<T>(t, f);

  fprintf(f, "  private:\n");
  fprintf(f, "    QString m_FilterGroup;\n\n");
  fprintf(f, "    QString m_FilterSubGroup;\n\n");
  fprintf(f, "    Q%sWidget(const Q%sWidget&);\n", filter.c_str(), filter.c_str());
  fprintf(f, "    void operator=(const Q%sWidget&);\n", filter.c_str());
  fprintf(f, "};\n");
  fprintf(f, "#endif /* Q%sWidget_H_ */\n", filter.c_str());

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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void createSourceFile( const std::string &group,
                       const std::string &filter,
                       std::vector<FilterParameter::Pointer> options,
                       const std::string &outSubPath)
{
  std::stringstream ss;
  std::string completePath;
  ss << FILTER_WIDGETS_BINARY_DIR() << "/" << outSubPath;

  completePath = MXADir::toNativeSeparators(ss.str());
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
  fprintf(f, "#include <QtGui/QLineEdit>\n");
  fprintf(f, "#include <QtGui/QCheckBox>\n");
  fprintf(f, "#include <QtGui/QComboBox>\n\n");
  for (size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget)
    {
      fprintf(f, "#include \"ArraySelectionWidget.h\"\n");
    }
    if (opt->getWidgetType() == FilterParameter::ComparisonSelectionWidget)
    {
      fprintf(f, "#include \"ComparisonSelectionWidget.h\"\n");
    }
  }
  fprintf(f, "\n\n\n");
  fprintf(f, "// -----------------------------------------------------------------------------\n");
  fprintf(f, "// This file is generated by Qt's moc program which sets up the signals/slots\n");
  fprintf(f, "// for this class.\n");
  fprintf(f, "#include \"moc_Q%sWidget.cxx\"\n\n", filter.c_str());

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
    else if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget)
    {
      fprintf(f, "    //Do we need to preset something from the filter maybe?\n");
      implementArrayNameSelectionWidget = true;
    }
    else if (opt->getWidgetType() == FilterParameter::ComparisonSelectionWidget)
    {
      fprintf(f, "    //Do we need to preset something from the filter maybe?\n");
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

  fprintf(f, "// -----------------------------------------------------------------------------\n");
  fprintf(f, "Q%sWidget::~Q%sWidget(){}\n\n", filter.c_str(), filter.c_str());

  fprintf(f, "// -----------------------------------------------------------------------------\n");
  fprintf(f, "QString Q%sWidget::getFilterGroup() {\n  return m_FilterGroup;\n}\n\n", filter.c_str() );

  fprintf(f, "// -----------------------------------------------------------------------------\n");
  fprintf(f, "QString Q%sWidget::getFilterSubGroup() {\n  return m_FilterSubGroup;\n}\n\n", filter.c_str() );

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
    else if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget)
    {
      fprintf(f, "  ArraySelectionWidget* w = qFindChild<ArraySelectionWidget*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "  if (NULL != w) {\n");
      fprintf(f, "    w->getArraySelections(filter.get());\n  }\n");
    }
    else if (opt->getWidgetType() == FilterParameter::ComparisonSelectionWidget)
    {
      fprintf(f, "  ComparisonSelectionWidget* w = qFindChild<ComparisonSelectionWidget*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "  if (NULL != w) {\n");
      fprintf(f, "    w->setComparisonsIntoFilter<%s>(filter.get());\n  }\n", filter.c_str());
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

  fprintf(f, "// -----------------------------------------------------------------------------\n");
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

      fprintf(f, "// -----------------------------------------------------------------------------\n");
      fprintf(f, "void Q%sWidget::set%s(%s v, bool emitChanged)\n{\n  m_%s = static_cast<%s>(v);\n", filter.c_str(), prop.c_str(), cType.c_str(), prop.c_str(), typ.c_str());
      fprintf(f, "  if (true == emitChanged) { emit parametersChanged();}\n}\n");
      fprintf(f, "// -----------------------------------------------------------------------------\n");
      fprintf(f, "%s  Q%sWidget::get%s() { \n  return static_cast<%s>(m_%s); \n}\n", cType.c_str(), filter.c_str(), prop.c_str(), typ.c_str(), prop.c_str());
    }
    else if(opt->getValueType().compare("string") == 0)
    {
      std::string cType = "QString";

      fprintf(f, "// -----------------------------------------------------------------------------\n");
      fprintf(f, "void Q%sWidget::set%s(const %s &v, bool emitChanged)\n{\n  m_%s = v;\n", filter.c_str(), prop.c_str(), cType.c_str(), prop.c_str());
      if (opt->getWidgetType() == FilterParameter::OutputFileWidget ||
          opt->getWidgetType() == FilterParameter::OutputPathWidget ||
          opt->getWidgetType() == FilterParameter::InputFileWidget ||
          opt->getWidgetType() == FilterParameter::InputPathWidget)
      {
        fprintf(f, "  m_%s = QDir::toNativeSeparators(m_%s);\n", prop.c_str(), prop.c_str());
      }
      fprintf(f, "  if (true == emitChanged) { emit parametersChanged();}\n}\n");

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
      fprintf(f, "  ArraySelectionWidget* w = qFindChild<ArraySelectionWidget*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "  if (NULL != w) {\n");
      fprintf(f, "    w->writeOptions(prefs, QString::fromUtf8(\"%s\"));\n", prop.c_str());
      fprintf(f, "  }\n");
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
      fprintf(f, "  prefs.setValue(\"x\", (double)(v_%s.x));\n", prop.c_str());
      fprintf(f, "  prefs.setArrayIndex(1);\n");
      fprintf(f, "  prefs.setValue(\"y\", (double)(v_%s.y));\n", prop.c_str());
      fprintf(f, "  prefs.setArrayIndex(2);\n");
      fprintf(f, "  prefs.setValue(\"z\", (double)(v_%s.z));\n", prop.c_str());
      fprintf(f, "  prefs.endArray();\n");
    }
    else if (opt->getWidgetType() == FilterParameter::ComparisonSelectionWidget)
    {
      fprintf(f, "  // ------------- %s ----------------------------------\n", prop.c_str());
      fprintf(f, "  ComparisonSelectionWidget* w = qFindChild<ComparisonSelectionWidget*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "  if (NULL != w) {\n");
      fprintf(f, "    w->writeOptions(prefs, QString::fromUtf8(\"%s\"));\n", prop.c_str());
      fprintf(f, "  }\n");
    }
    else
    {
      fprintf(f, "  prefs.setValue(\"%s\", get%s() );\n", prop.c_str(), prop.c_str());
    }

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
    fprintf(f, "  // ------------- %s ----------------------------------\n", prop.c_str());
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
      fprintf(f, "   QString path = QDir::toNativeSeparators(p_%s.toString());\n", prop.c_str());
      fprintf(f, "   QLineEdit* lb = qFindChild<QLineEdit*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "   if (lb) { lb->setText(path); }\n");
      fprintf(f, "   set%s(path);\n", prop.c_str());
    }
    else if(opt->getWidgetType() == FilterParameter::BooleanWidget)
    {
      fprintf(f, "   QCheckBox* le = findChild<QCheckBox*>(\"%s\");\n", prop.c_str());
      fprintf(f, "   if (le) { le->setChecked(p_%s.toBool()); }\n", prop.c_str());
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
    }
    else if (opt->getWidgetType() == FilterParameter::ComparisonSelectionWidget)
    {
      fprintf(f, "    ComparisonSelectionWidget* w = qFindChild<ComparisonSelectionWidget*>(this, \"%s\");\n", prop.c_str());
      fprintf(f, "    if (NULL != w) {\n");
      fprintf(f, "      w->readOptions(prefs, QString::fromUtf8(\"%s\"));\n", prop.c_str());
      fprintf(f, "    }\n");
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
    fprintf(f, "// -----------------------------------------------------------------------------\n");
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

  if (implementArrayNameSelectionWidget == true)
  {

    fprintf(f, "// -----------------------------------------------------------------------------\n");
    fprintf(f, "void Q%sWidget::preflightAboutToExecute(VoxelDataContainer::Pointer vdc, SurfaceMeshDataContainer::Pointer smdc, SolidMeshDataContainer::Pointer sdc)\n{\n", filter.c_str());
    for (size_t i = 0; i < options.size(); ++i)
    {
      FilterParameter::Pointer opt = options[i];
      std::string prop = opt->getPropertyName();
      std::string typ = opt->getValueType();
      std::string hl = opt->getHumanLabel();
      if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget ) {
        fprintf(f, "  ArraySelectionWidget* w = qFindChild<ArraySelectionWidget*>(this, \"%s\");\n", prop.c_str()); // Make sure we have a non null QWidget to deal with

        fprintf(f, "  if (NULL != w) {\n    w->populateArrayNames(vdc, smdc, sdc);\n  }\n");
      }
    }
    fprintf(f, "}\n");
  }

  if (implementComparisonSelectionWidget == true)
  {
    fprintf(f, "// -----------------------------------------------------------------------------\n");
    fprintf(f, "void Q%sWidget::preflightAboutToExecute(VoxelDataContainer::Pointer vdc, SurfaceMeshDataContainer::Pointer smdc, SolidMeshDataContainer::Pointer sdc)\n{\n", filter.c_str());
    for (size_t i = 0; i < options.size(); ++i)
    {
      FilterParameter::Pointer opt = options[i];
      std::string prop = opt->getPropertyName();
      std::string typ = opt->getValueType();
      std::string hl = opt->getHumanLabel();
      if (opt->getWidgetType() == FilterParameter::ComparisonSelectionWidget ) {
        fprintf(f, "  ComparisonSelectionWidget* w = qFindChild<ComparisonSelectionWidget*>(this, \"%s\");\n", prop.c_str()); // Make sure we have a non null QWidget to deal with

        fprintf(f, "  if (NULL != w) {\n    w->populateArrayNames(vdc, smdc, sdc);\n  }\n");
      }
    }
    fprintf(f, "}\n");
  }

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

  return 0;
}


