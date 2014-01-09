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


#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QMap>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QCryptographicHash>


#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/FilterParameters/FilterParameter.h"
#include "DREAM3DLib/Common/CreatedArrayHelpIndexEntry.h"




// Enabling this will create a pair of files that can be used to update
// the "PreflightTest.cpp" unit test
#define GENERATE_PREFLIGHT_TEST_CODE_FRAGMENT 0
#define GENERATE_FILTER_PARAMTERS_READER_CODE 0



typedef QMap<QString, QSet<QString> >  FilterMapType;
typedef QSet<QString>  StringSetType;

// These will be defined in an include header file below.
QString FILTER_WIDGETS_BINARY_DIR();
QString FILTER_WIDGETS_SOURCE_DIR();
QString FILTER_WIDGETS_TEMP_DIR();
QString FILTER_WIDGETS_DOCS_DIR();
QString DREAM3D_SOURCE_DIR();
QString FILTER_INCLUDE_PREFIX();
QString DREAM3D_SOURCE_DIR();
QString DREAM3D_BINARY_DIR();
QString DREAM3DLIB_SOURCE_DIR();

typedef QMap<QString, CreatedArrayHelpIndexEntry::VectorType> IndexMap_t;

QMap<QString, CreatedArrayHelpIndexEntry::VectorType>  helpIndex;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void copyFile(const QString& src, const QString& dest)
{
  if (src.isEmpty() == true || dest.isEmpty() == true)
  {
    BOOST_ASSERT(false);
  }

  QFile srcFile(src);

  QFile dstFile(dest);
  if(dstFile.exists() == true)
  {
    dstFile.remove();
  }

  srcFile.copy(dest);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Md5OfFile(QString filePath)
{
  if (filePath.isEmpty() == true)
  {
    BOOST_ASSERT(false);
  }
  QFile file(filePath);
  if(!file.open(QFile::ReadOnly | QFile::Text))
  {
    qDebug() << "Could not open file '" << filePath << "' to compute MD5 Checksum";
    return QString("");
  }

  QByteArray contents = file.readAll();
  QByteArray hash = QCryptographicHash::hash(contents, QCryptographicHash::Md5).toHex();

  return QString(hash);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void extractHelpIndexEntries(AbstractFilter* filter)
{
  DataContainerArray::Pointer dca = DataContainerArray::New();

  VolumeDataContainer::Pointer vol = VolumeDataContainer::New();
  SurfaceDataContainer::Pointer surf = SurfaceDataContainer::New();
  EdgeDataContainer::Pointer edg = EdgeDataContainer::New();
  VertexDataContainer::Pointer vert = VertexDataContainer::New();

  dca->pushBack(vol);
  dca->pushBack(surf);
  dca->pushBack(edg);
  dca->pushBack(vert);

  filter->setDataContainerArray(dca);
  filter->preflight();
  CreatedArrayHelpIndexEntry::VectorType entries = filter->getCreatedArrayHelpIndexEntries();

  for(CreatedArrayHelpIndexEntry::VectorType::iterator entry = entries.begin(); entry != entries.end(); ++entry)
  {
    QString entryName = (*entry)->getArrayDefaultName();
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

  QString path = DREAM3D_BINARY_DIR();
  path = path + "/" + "createdarrayindex.md";

  FILE* f = fopen(path.toLatin1().data(), "wb");

  fprintf(f, "Created Array Index {#createdarrayindex}\n======\n");

  for(IndexMap_t::iterator entry = helpIndex.begin(); entry != helpIndex.end(); ++entry)
  {
    QString name = entry.key();
    fprintf (f, "## %s ##\n\n", entry.key().toLatin1().data());

    CreatedArrayHelpIndexEntry::VectorType& filters = entry.value();


    for(CreatedArrayHelpIndexEntry::VectorType::iterator indexEntry = filters.begin(); indexEntry != filters.end(); ++indexEntry)
    {
      QString lower = (*indexEntry)->getFilterName();
      lower = lower.toLower();
      fprintf(f, "+ [%s](%s.html) (%s->%s)\n", (*indexEntry)->getFilterHumanLabel().toLatin1().data(), lower.toLatin1().data(), (*indexEntry)->getFilterGroup().toLatin1().data(), (*indexEntry)->getFilterSubGroup().toLatin1().data());
    }

    fprintf(f, "\n");
  }
  fclose(f);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void createHeaderFile(const QString& group, const QString& filterName, AbstractFilter* filterPtr, const QString& outputPath)
{
  extractHelpIndexEntries(filterPtr);


  QString completePath = QDir::toNativeSeparators(outputPath);
  // Make sure the output path exists
  QDir parentPath(completePath);
  //QString parentPath = MXADir::parentPath(completePath);
  if (parentPath.exists() == false)
  {
    parentPath.mkpath(".");
  }

  QVector<FilterParameter::Pointer> options = filterPtr->getFilterParameters();

  QString tempPath = FILTER_WIDGETS_TEMP_DIR() + "/" + group + "_" + filterName + ".h";

  FILE* f = fopen(tempPath.toLatin1().data(), "wb");
  if (NULL == f)
  {
    qDebug() << "Could not open file '" << tempPath << "' for writing." << "\n";
    return;
  }

  fprintf(f, "/*\n");
  fprintf(f, "  This file was auto-generated from the program FilterWidgetCodeGen.cpp which is\n  itself generated during cmake time\n");
  fprintf(f, "  If you need to make changes to the code that is generated you will need to make\n  them in the original file. \n");
  fprintf(f, "  The code generated is based off values from the filter located at\n");
  if (FILTER_INCLUDE_PREFIX().isEmpty() == true)
  {
    fprintf(f, "  %s/%s.h\n*/\n", group.toLatin1().data(), filterName.toLatin1().data());
  }
  else
  {
    fprintf(f, "  %s/%s/%s.h\n*/\n", FILTER_INCLUDE_PREFIX().toLatin1().data(), group.toLatin1().data(), filterName.toLatin1().data());
  }

  fprintf(f, "#ifndef _Q%sWidget_H_\n", filterName.toLatin1().data());
  fprintf(f, "#define _Q%sWidget_H_\n\n", filterName.toLatin1().data());

  fprintf(f, "#include <QtCore/QObject>\n");
  fprintf(f, "#include <QtCore/QSettings>\n");
  fprintf(f, "#include <QtCore/QUrl>\n\n");

  fprintf(f, "#include \"PipelineBuilder/QFilterWidget.h\"\n");
  fprintf(f, "#include \"DREAM3DLib/Common/DREAM3DSetGetMacros.h\"\n");
  fprintf(f, "#include \"DREAM3DLib/FilterParameters/FilterParameter.h\"\n\n");
  if (FILTER_INCLUDE_PREFIX().isEmpty() == true)
  {
    fprintf(f, "#include \"%s/%s.h\"\n", group.toLatin1().data(), filterName.toLatin1().data());
  }
  else
  {
    fprintf(f, "#include \"%s/%s/%s.h\"\n", FILTER_INCLUDE_PREFIX().toLatin1().data(), group.toLatin1().data(), filterName.toLatin1().data());
  }
  fprintf(f, "\n\nclass Q%sWidget : public QFilterWidget \n{\n", filterName.toLatin1().data());
  fprintf(f, "   Q_OBJECT\n");
  fprintf(f, "  public:\n");
  fprintf(f, "    Q%sWidget(QWidget* parent = NULL);\n", filterName.toLatin1().data());
  fprintf(f, "    virtual ~Q%sWidget();\n", filterName.toLatin1().data());
  fprintf(f, "    virtual AbstractFilter::Pointer getFilter(bool defaultValues);\n");
  fprintf(f, "    QFilterWidget* createDeepCopy();\n\n");
  fprintf(f, "    QString getFilterGroup();\n\n");
  fprintf(f, "    QString getFilterSubGroup();\n\n");
  fprintf(f, "    virtual void openHtmlHelpFile();\n\n");
  fprintf(f, "    virtual void getGuiParametersFromFilter(AbstractFilter* filt);\n\n");

  bool implementArrayNameComboBoxUpdated = false;
  bool implementPreflightAboutToExecute = true;
  int axisAngleWidgetCount = 0;
  // Loop on all the filter options
  for(size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    QString prop = opt->getPropertyName();
    QString typ = opt->getValueType();

    if (opt->getCastableValueType().isEmpty() == false)
    {
      QString cType = opt->getCastableValueType();
      fprintf(f, "  private:\n");
      fprintf(f, "    %s m_%s;\n", cType.toLatin1().data(), prop.toLatin1().data());
      fprintf(f, "  public:\n");
      fprintf(f, "    Q_PROPERTY(%s %s READ get%s WRITE set%s)\n", cType.toLatin1().data(), prop.toLatin1().data(), prop.toLatin1().data(), prop.toLatin1().data());
      fprintf(f, "  public slots:\n");
      fprintf(f, "    void set%s(%s v, bool emitChanged = true);\n", prop.toLatin1().data(), cType.toLatin1().data());
      fprintf(f, "  public:\n");
      fprintf(f, "    %s  get%s();\n", cType.toLatin1().data(), prop.toLatin1().data());
    }
    else if (opt->getValueType().compare("string") == 0)
    {
      QString cType = "QString";
      fprintf(f, "  private:\n");
      fprintf(f, "    QString m_%s;\n", prop.toLatin1().data());
      fprintf(f, "  public:\n");
      fprintf(f, "    Q_PROPERTY(%s %s READ get%s WRITE set%s)\n", cType.toLatin1().data(), prop.toLatin1().data(), prop.toLatin1().data(), prop.toLatin1().data());
      fprintf(f, "  public slots:\n");
      fprintf(f, "    void set%s(const %s &v, bool emitChanged = true);\n", prop.toLatin1().data(), cType.toLatin1().data());
      fprintf(f, "  public:\n");
      fprintf(f, "    %s  get%s();\n\n\n", cType.toLatin1().data(), prop.toLatin1().data());
    }
    else if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget && implementPreflightAboutToExecute == true)
    {
      fprintf(f, "  public:\n");
      fprintf(f, "    virtual void preflightAboutToExecute(DataContainerArray::Pointer dca);\n");
      fprintf(f, "\n\n");
      implementPreflightAboutToExecute = false;
    }
    else if (opt->getWidgetType() == FilterParameter::IntVec3Widget)
    {
      fprintf(f, "    Q_PROPERTY(IntVec3Widget_t %s READ get%s WRITE set%s)\n", prop.toLatin1().data(), prop.toLatin1().data(), prop.toLatin1().data());
      fprintf(f, "    QFILTERWIDGET_INSTANCE_PROPERTY(IntVec3Widget_t, %s)\n\n", prop.toLatin1().data());
    }
    else if (opt->getWidgetType() == FilterParameter::FloatVec3Widget)
    {
      fprintf(f, "    Q_PROPERTY(FloatVec3Widget_t %s READ get%s WRITE set%s)\n", prop.toLatin1().data(), prop.toLatin1().data(), prop.toLatin1().data());
      fprintf(f, "    QFILTERWIDGET_INSTANCE_PROPERTY(FloatVec3Widget_t, %s)\n\n", prop.toLatin1().data());
    }
    else if (opt->getWidgetType() == FilterParameter::AxisAngleWidget)
    {
      fprintf(f, "DREAM3D_INSTANCE_PROPERTY(QVector<AxisAngleInput_t>, %s)\n\n", prop.toLatin1().data());
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
      fprintf(f, "\n  DREAM3D_INSTANCE_PROPERTY(QVector<ComparisonInput_t>, %s)\n\n", prop.toLatin1().data());
      fprintf(f, "  public:\n");
      fprintf(f, "    virtual void preflightAboutToExecute(DataContainerArray::Pointer dca);\n");
      fprintf(f, "\n\n");
      implementPreflightAboutToExecute = false;
    }
    else
    {
      fprintf(f, "    Q_PROPERTY(%s %s READ get%s WRITE set%s)\n", typ.toLatin1().data(), prop.toLatin1().data(), prop.toLatin1().data(), prop.toLatin1().data());
      fprintf(f, "    QFILTERWIDGET_INSTANCE_PROPERTY(%s, %s)\n\n", typ.toLatin1().data(), prop.toLatin1().data());
    }

    if (opt->getWidgetType() >= FilterParameter::VolumeCellArrayNameSelectionWidget
        && opt->getWidgetType() <= FilterParameter::VertexEnsembleArrayNameSelectionWidget )
    { implementArrayNameComboBoxUpdated = true; }
  }

  if (true == implementArrayNameComboBoxUpdated)
  {
    fprintf(f, "  public:\n    virtual void arrayNameComboBoxUpdated(QComboBox* cb);\n\n");
  }


  fprintf(f, "  private:\n");
  fprintf(f, "    QString m_FilterGroup;\n\n");
  fprintf(f, "    QString m_FilterSubGroup;\n\n");
  fprintf(f, "    Q%sWidget(const Q%sWidget&);\n", filterName.toLatin1().data(), filterName.toLatin1().data());
  fprintf(f, "    void operator=(const Q%sWidget&);\n", filterName.toLatin1().data());
  fprintf(f, "};\n");
  fprintf(f, "#endif /* Q%sWidget_H_ */\n", filterName.toLatin1().data());

  fclose(f);

  qint64 currentFileSize = 0;
  // Now compare the file just generated with any possible existing file
  {
    QFileInfo fi(completePath);
    currentFileSize = fi.size();
  }

  qint64 tempFileSize = 0;
  {
    QFileInfo fi(tempPath);
    tempFileSize = fi.size();
  }
  // If the file sizes are different then copy the file
  if (currentFileSize != tempFileSize)
  {
    qDebug() << "0-Creating Header File: " << completePath;
    copyFile(tempPath, completePath);
  }
  else // Just because the files are the same size does not mean they are the same.
  {

    QString currentHash = Md5OfFile(completePath);
    QString tempHash = Md5OfFile(tempPath);

    // Use MD5 Checksums to figure out if the files are different
    if (tempHash != currentHash)
    {
      qDebug() << "0-Copying Header File: " << completePath;
      qDebug() << "    Hex Digest:    " << QString(currentHash);
      qDebug() << "    tempHexDigest: " << QString(tempHash);
      copyFile(tempPath, completePath);
    }

    QFile tempFile(tempPath);
    if(tempFile.remove() == false)
    {
      qDebug() << "Could not remove temp header file " << tempPath;
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


int readLine(std::istream& in, char* buf, int bufSize)
{

  bool readAnotherLine = true;
  size_t gcount = 0;
  while ( readAnotherLine == true && in.gcount() != 0 && in)
  {
    // Zero out the buffer
    ::memset(buf, 0, bufSize);
    // Read a line up to a '\n' which will catch windows and unix line endings but
    // will leave a trailing '\r' at the end of the string
    in.getline(buf, bufSize, '\n');
    gcount = in.gcount();
    if (gcount > 1 && buf[in.gcount() - 2] == '\r')
    {
      buf[in.gcount() - 2] = 0;
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
void parseSourceFileForMarker(const QString filename, const QString marker, const QString& replace)
{

  QString tempfile = filename + "_tmp";
  {
    std::ofstream out(tempfile.toLatin1().data(), std::ios_base::binary);

    qDebug() << filename << "\n";
    std::ifstream instream;
    instream.open(filename.toLatin1().data(), std::ios_base::binary);
    if (!instream.is_open())
    {
      qDebug() << " file could not be opened: " << filename << "\n";
      return;
    }

    char buf[kBufferSize];
    ::memset(buf, 0, kBufferSize);
    size_t gcount = 0;
    while( instream.getline(buf, kBufferSize, '\n') )
    {
      gcount = instream.gcount();
      if (gcount > 1 && buf[instream.gcount() - 2] == '\r')
      {
        buf[instream.gcount() - 2] = 0;
      }
      if (marker.compare(buf) == 0)
      {
        out << replace;
      }
      else
      {
        out << QString(buf) << "\n";
      }
    }
  }
  copyFile(tempfile, filename);
  if ( !QFile::remove(tempfile) )
  {
    qDebug() << "FILE NOT REMOVED: " << tempfile << "\n";
  }

}


#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void createSourceFile( const QString& group,
                       const QString& filter,
                       QVector<FilterParameter::Pointer> options,
                       const QString& outputPath)
{

  QString completePath = QDir::toNativeSeparators(outputPath);
  QFileInfo fi(completePath);
  // Make sure the output path exists
  QDir parentPath = fi.path();
  if (parentPath.exists() == false)
  {
    parentPath.mkpath(".");
  }

  QString tempPath = FILTER_WIDGETS_TEMP_DIR() + "/" + group + "_" + filter + ".cpp";

  FILE* f = fopen(tempPath.toLatin1().data(), "wb");

  bool implementArrayNameComboBoxUpdated = false;
  bool implementArrayNameSelectionWidget = false;
  bool implementComparisonSelectionWidget = false;

  fprintf(f, "/*\n");
  fprintf(f, "* This file was auto-generated from the program FilterWidgetCodeGen.cpp which is\n  itself generated during cmake time\n");
  fprintf(f, "* If you need to make changes to the code that is generated you will need to make\n  them in the original file. \n");
  fprintf(f, "* The code generated is based off values from the filter located at\n");
  if(FILTER_INCLUDE_PREFIX().isEmpty() == true)
  {
    fprintf(f, "* %s/%s.h\n*/\n", group.toLatin1().data(), filter.toLatin1().data());
  }
  else
  {
    fprintf(f, "* %s/%s/%s.h\n*/\n", FILTER_INCLUDE_PREFIX().toLatin1().data(), group.toLatin1().data(), filter.toLatin1().data());
  }
  fprintf(f, "#include \"%s/%sWidgets/Q%sWidget.h\"\n", FILTER_INCLUDE_PREFIX().toLatin1().data(), group.toLatin1().data(), filter.toLatin1().data());
  fprintf(f, "#include <QtCore/QDir>\n");
  fprintf(f, "#include <QtGui/QApplication>\n");
  fprintf(f, "#include <QtGui/QLineEdit>\n");
  fprintf(f, "#include <QtGui/QCheckBox>\n");
  fprintf(f, "#include <QtGui/QComboBox>\n\n");
  fprintf(f, "#include \"QtSupport/DREAM3DHelpUrlGenerator.h\"\n");
  fprintf(f, "#include \"QtSupport/QFSDropLineEdit.h\"\n\n");
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
  fprintf(f, "#include \"moc_Q%sWidget.cxx\"\n\n", filter.toLatin1().data());

  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "Q%sWidget::Q%sWidget(QWidget* parent):\nQFilterWidget(parent)\n", filter.toLatin1().data(), filter.toLatin1().data());
  fprintf(f, "{\n");
  fprintf(f, "     %s::Pointer filter = %s::New();\n", filter.toLatin1().data(), filter.toLatin1().data());
  fprintf(f, "     m_FilterGroup = filter->getGroupName();\n");
  fprintf(f, "     m_FilterSubGroup = filter->getSubGroupName();\n");
  fprintf(f, "     setupGui();\n");
  fprintf(f, "     getGuiParametersFromFilter( filter.get() );\n");
  fprintf(f, "     setTitle(filter->getHumanLabel());\n");
  fprintf(f, "}\n\n");

  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "Q%sWidget::~Q%sWidget(){}\n\n", filter.toLatin1().data(), filter.toLatin1().data());

  // Write getGuiParametersFromFilter(filter) function
  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "void Q%sWidget::getGuiParametersFromFilter(AbstractFilter* filt)\n{\n", filter.toLatin1().data());
  if (options.size() > 0)
  {
    fprintf(f, "     %s* filter = %s::SafeObjectDownCast<AbstractFilter*, %s*>(filt);\n", filter.toLatin1().data(), filter.toLatin1().data(), filter.toLatin1().data());
    fprintf(f, "     blockSignals(true);\n");
  }
  // Loop on all the options getting the defaults from a fresh instance of the filter class
  for (size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    QString prop = opt->getPropertyName();
    QString typ = opt->getValueType();

    if(opt->getWidgetType() == FilterParameter::StringWidget || opt->getWidgetType() == FilterParameter::InputFileWidget
        || opt->getWidgetType() == FilterParameter::InputPathWidget || opt->getWidgetType() == FilterParameter::OutputFileWidget
        || opt->getWidgetType() == FilterParameter::OutputPathWidget)
    {
      fprintf(f, "     { \n");
      fprintf(f, "        QLineEdit* w = qFindChild<QLineEdit*>(this, \"%s\");\n", prop.toLatin1().data());
      fprintf(f, "        if (w) {\n");
      fprintf(f, "           w->setText( filter->get%s() );\n", prop.toLatin1().data());
      fprintf(f, "        }\n");
      fprintf(f, "     }\n");
    }
    else if (opt->getWidgetType() == FilterParameter::IntWidget || opt->getWidgetType() == FilterParameter::DoubleWidget)
    {
      fprintf(f, "     {\n");
      fprintf(f, "        QLineEdit* w = qFindChild<QLineEdit*>(this, \"%s\");\n", prop.toLatin1().data());
      fprintf(f, "        if (w) {\n");
      //  fprintf(f, "           \n");
      //  fprintf(f, "           ss << filter->get%s();\n", prop.toLatin1().data());
      fprintf(f, "           w->setText( QString::number(filter->get%s()) );\n", prop.toLatin1().data());
      fprintf(f, "        }\n");
      fprintf(f, "     }\n");
    }
    else if (opt->getWidgetType() == FilterParameter::BooleanWidget)
    {
      fprintf(f, "     {\n");
      fprintf(f, "        QCheckBox* w = qFindChild<QCheckBox*>(this, \"%s\");\n", prop.toLatin1().data());
      fprintf(f, "        if (w) {\n");
      fprintf(f, "           w->setChecked( filter->get%s() );\n", prop.toLatin1().data());
      fprintf(f, "        }\n");
      fprintf(f, "     }\n");
    }
    else if (opt->getWidgetType() == FilterParameter::ChoiceWidget)
    {
      ChoiceFilterParameter* ptr = ChoiceFilterParameter::SafePointerDownCast( opt.get() );

      fprintf(f, "     {\n");
      fprintf(f, "        QComboBox* w = qFindChild<QComboBox*>(this, \"%s\");\n", prop.toLatin1().data());
      fprintf(f, "        if (w) {\n");
      if (opt->getValueType().compare("string") == 0)
      {
        fprintf(f, "           int index = w->findText( filter->get%s() );\n", prop.toLatin1().data());
        fprintf(f, "           if (index >= 0)\n");
        fprintf(f, "           {\n");
        fprintf(f, "              w->setCurrentIndex(index);\n");
        fprintf(f, "           }\n");
        fprintf(f, "           else if (%d) {\n", (int)(ptr->getEditable()) );
        fprintf(f, "             w->setEditable(true);\n");
        fprintf(f, "             w->addItem( filter->get%s() );\n", prop.toLatin1().data());
        fprintf(f, "             w->setCurrentIndex( w->findText(filter->get%s()) );\n", prop.toLatin1().data());
        fprintf(f, "           }\n");
      }
      else
      {
        fprintf(f, "           w->setCurrentIndex( filter->get%s() );\n", prop.toLatin1().data());
      }
      fprintf(f, "        }\n");
      fprintf(f, "     }\n");
    }
    else if (opt->getWidgetType() == FilterParameter::IntVec3Widget || opt->getWidgetType() == FilterParameter::FloatVec3Widget)
    {
      QString wType = "IntVec3Widget_t";
      if(opt->getWidgetType() == FilterParameter::FloatVec3Widget)
      {
        wType = "FloatVec3Widget_t";
      }
      fprintf(f, "     {\n");
      fprintf(f, "        %s value = filter->get%s();\n", wType.toLatin1().data(), prop.toLatin1().data());
      fprintf(f, "        QLineEdit* w1 = qFindChild<QLineEdit*>(this, \"0_%s\");\n", prop.toLatin1().data());
      fprintf(f, "        QLineEdit* w2 = qFindChild<QLineEdit*>(this, \"1_%s\");\n", prop.toLatin1().data());
      fprintf(f, "        QLineEdit* w3 = qFindChild<QLineEdit*>(this, \"2_%s\");\n", prop.toLatin1().data());
      fprintf(f, "        if (w1 && w2 && w3) {\n");
      fprintf(f, "           w1->setText(QString::number(value.x));\n");
      fprintf(f, "           w2->setText(QString::number(value.y));\n");
      fprintf(f, "           w3->setText(QString::number(value.z));\n");
      fprintf(f, "        }\n");
      fprintf(f, "     }\n");
    }
    else if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget)
    {
      fprintf(f, "     {\n");
      fprintf(f, "        ArraySelectionWidget* w = qFindChild<ArraySelectionWidget*>(this, \"%s\");\n", prop.toLatin1().data());
      fprintf(f, "        if (NULL != w) {\n");
      fprintf(f, "           w->setArraySelections(filter);\n");
      fprintf(f, "        }\n");
      fprintf(f, "     }\n");

      implementArrayNameSelectionWidget = true;
    }
    else if (opt->getWidgetType() == FilterParameter::AxisAngleWidget)
    {
      fprintf(f, "     {\n");
      fprintf(f, "        AxisAngleWidget* w = qFindChild<AxisAngleWidget*>(this, \"%s\");\n", prop.toLatin1().data());
      fprintf(f, "        if (NULL != w) {\n");
      fprintf(f, "           QVector<AxisAngleInput_t> v = filter->get%s();\n", prop.toLatin1().data());
      fprintf(f, "           w->getTableModel()->removeRows(0, w->getTableModel()->rowCount());\n");
      fprintf(f, "           for (int i=0; i<v.size(); i++)\n");
      fprintf(f, "           {\n");
      fprintf(f, "              QString buf;\n");
      fprintf(f, "              QTextStream ss(&buf);\n");
      fprintf(f, "              ss << \"<\" << v[i].h << \", \" << v[i].k << \", \" << v[i].l << \">\";\n");
      fprintf(f, "              w->getTableModel()->insertRow(w->getTableModel()->rowCount());\n");
      fprintf(f, "              w->getTableModel()->setRowData( i, v[i].angle, buf );\n");
      fprintf(f, "           }\n");
      fprintf(f, "        }\n");
      fprintf(f, "     }\n");
    }
    else if (opt->getWidgetType() >= FilterParameter::VolumeCellArrayNameSelectionWidget
             && opt->getWidgetType() <= FilterParameter::VertexEnsembleArrayNameSelectionWidget)
    {
      fprintf(f, "     {\n");
      fprintf(f, "        QComboBox* w = qFindChild<QComboBox*>(this, \"%s\");\n", prop.toLatin1().data());
      fprintf(f, "        if (w) {\n");
      fprintf(f, "           int index = w->findText(filter->get%s() );\n", prop.toLatin1().data());
      fprintf(f, "           if (index >= 0)\n");
      fprintf(f, "           {\n");
      fprintf(f, "              w->setCurrentIndex(index);\n");
      fprintf(f, "           }\n");
      fprintf(f, "        }\n");
      fprintf(f, "     }\n");
    }
    else if (opt->getWidgetType() >= FilterParameter::CellArrayComparisonSelectionWidget
             && opt->getWidgetType() <= FilterParameter::EdgeArrayComparisonSelectionWidget)
    {
      // fprintf(f, "     {\n");
      fprintf(f, "     ComparisonSelectionWidget* w_%s = qFindChild<ComparisonSelectionWidget*>(this, \"%s\");\n", prop.toLatin1().data(), prop.toLatin1().data());
      fprintf(f, "     if (w_%s) {\n", prop.toLatin1().data());
      fprintf(f, "       w_%s->setComparisons(filter->get%s());\n", prop.toLatin1().data(), prop.toLatin1().data());
      fprintf(f, "     }\n");
      // fprintf(f, "     }\n");
      implementComparisonSelectionWidget = true;
    }

#if 0
    else
    {
      fprintf(f, "     set%s( filter->get%s() );\n", prop.toLatin1().data(), prop.toLatin1().data());
    }
#endif
  }
  if (options.size() > 0)
  {
    fprintf(f, "     blockSignals(false);\n");
    fprintf(f, "     emit parametersChanged();\n");
  }
  fprintf(f, "}\n");

  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "QString Q%sWidget::getFilterGroup() {\n  return m_FilterGroup;\n}\n\n", filter.toLatin1().data() );

  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "QString Q%sWidget::getFilterSubGroup() {\n  return m_FilterSubGroup;\n}\n\n", filter.toLatin1().data() );

  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "AbstractFilter::Pointer Q%sWidget::getFilter(bool defaultValues) \n{\n", filter.toLatin1().data());
  fprintf(f, "  %s::Pointer filter = %s::New();\n", filter.toLatin1().data(), filter.toLatin1().data());
  fprintf(f, "  if (defaultValues == true) { return filter; }\n\n");
  for (size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    QString prop = opt->getPropertyName();
    QString typ = opt->getValueType();
    if(opt->getValueType().compare("string") == 0)
    {
      fprintf(f, "  filter->set%s( get%s() );\n", prop.toLatin1().data(), prop.toLatin1().data());
    }
    else if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget)
    {
      fprintf(f, "  {\n    ArraySelectionWidget* w = qFindChild<ArraySelectionWidget*>(this, \"%s\");\n", prop.toLatin1().data());
      fprintf(f, "    if (NULL != w) {\n");
      fprintf(f, "      w->getArraySelections(filter.get());\n    }\n  }\n");
    }
    else if (opt->getWidgetType() == FilterParameter::AxisAngleWidget)
    {
      fprintf(f, "  {\n    AxisAngleWidget* w = qFindChild<AxisAngleWidget*>(this, \"%s\");\n", prop.toLatin1().data());
      fprintf(f, "    if (NULL != w) {\n");
      //  fprintf(f, "//      w->setAxisAnglesIntoFilter<%s>(filter.get());\n    }\n  }\n", filter.toLatin1().data());
      fprintf(f, "      filter->set%s(w->getAxisAngleRotations());\n    }\n  }\n", prop.toLatin1().data());
    }
    else if (opt->getWidgetType() >= FilterParameter::CellArrayComparisonSelectionWidget
             && opt->getWidgetType() <= FilterParameter::EdgeArrayComparisonSelectionWidget)
    {
      fprintf(f, "  {\n    ComparisonSelectionWidget* w = qFindChild<ComparisonSelectionWidget*>(this, \"%s\");\n", prop.toLatin1().data());
      fprintf(f, "    if (NULL != w) {\n");
      // fprintf(f, "//      w->setComparisonsIntoFilter<%s>(filter.get());\n    }\n  }\n", filter.toLatin1().data());
      fprintf(f, "        filter->set%s(w->getComparisonInputs());\n    }\n  }\n", prop.toLatin1().data());
    }
    else
    {
      fprintf(f, "  filter->set%s( get%s() );\n", prop.toLatin1().data(), prop.toLatin1().data());
    }
  }
  // Generate all the source code to set the various array names into the filter
  //  appendArrayNameGetFilterCode<T>(t, f);

  fprintf(f, "  return filter;\n");
  fprintf(f, "}\n");

  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "QFilterWidget* Q%sWidget::createDeepCopy() \n{\n", filter.toLatin1().data());
  fprintf(f, "  Q%sWidget* w = new Q%sWidget(NULL);\n", filter.toLatin1().data(), filter.toLatin1().data());
  fprintf(f, "  return w;\n");
  fprintf(f, "}\n");


  // Loop on all the filter options
  for (size_t i = 0; i < options.size(); ++i)
  {
    FilterParameter::Pointer opt = options[i];
    QString prop = opt->getPropertyName();
    QString typ = opt->getValueType();

    if(opt->getCastableValueType().isEmpty() == false)
    {
      QString cType = opt->getCastableValueType();

      fprintf(f, "\n// -----------------------------------------------------------------------------\n");
      fprintf(f, "void Q%sWidget::set%s(%s v, bool emitChanged)\n{\n", filter.toLatin1().data(), prop.toLatin1().data(), cType.toLatin1().data() );
      fprintf(f, "  m_%s = v;\n", prop.toLatin1().data());
      fprintf(f, "  if (true == emitChanged) { emit parametersChanged();}\n}\n");
      fprintf(f, "\n// -----------------------------------------------------------------------------\n");
      fprintf(f, "%s  Q%sWidget::get%s() { \n  return m_%s; \n}\n", cType.toLatin1().data(), filter.toLatin1().data(), prop.toLatin1().data(),  prop.toLatin1().data());
    }
    else if(opt->getValueType().compare("string") == 0)
    {
      QString cType = "QString";

      fprintf(f, "\n// -----------------------------------------------------------------------------\n");
      fprintf(f, "void Q%sWidget::set%s(const %s &v, bool emitChanged)\n{\n  m_%s = v;\n", filter.toLatin1().data(), prop.toLatin1().data(), cType.toLatin1().data(), prop.toLatin1().data());
      if (opt->getWidgetType() == FilterParameter::OutputFileWidget ||
          opt->getWidgetType() == FilterParameter::OutputPathWidget ||
          opt->getWidgetType() == FilterParameter::InputFileWidget ||
          opt->getWidgetType() == FilterParameter::InputPathWidget)
      {
        fprintf(f, "  m_%s = QDir::toNativeSeparators(m_%s);\n", prop.toLatin1().data(), prop.toLatin1().data());
      }
      fprintf(f, "  if (true == emitChanged) { emit parametersChanged();}\n}\n");

      fprintf(f, "\n// -----------------------------------------------------------------------------\n");
      fprintf(f, "%s  Q%sWidget::get%s()\n{ \n  return m_%s; \n}\n", cType.toLatin1().data(), filter.toLatin1().data(), prop.toLatin1().data(), prop.toLatin1().data());
    }
    else
    {
      // If we fall in here then the methods should have been generated in the header file
      //    fprintf(f, "Q_PROPERTY(%s %s READ get%s WRITE set%s)\n", typ.toLatin1().data(), prop.toLatin1().data(), prop.toLatin1().data(), prop.toLatin1().data());
      //    fprintf(f, "FILTER_PROPERTY_WRAPPER(%s, %s, m_Filter);\n", typ.toLatin1().data(), prop.toLatin1().data());
    }

    if (opt->getWidgetType() >= FilterParameter::VolumeCellArrayNameSelectionWidget
        && opt->getWidgetType() <= FilterParameter::VertexEnsembleArrayNameSelectionWidget )
    { implementArrayNameComboBoxUpdated = true; }
  }


#if 0
  ss.str("");
  replaceStream << "/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/" << "\n";
  ss << DREAM3DLIB_SOURCE_DIR() << "/" << group << "/" << filter << ".cpp";
  parseSourceFileForMarker(ss.str(), "////!!##", replaceStream.str());
#endif

  if (true == implementArrayNameComboBoxUpdated)
  {
    fprintf(f, "\n// -----------------------------------------------------------------------------\n");
    fprintf(f, "void Q%sWidget::arrayNameComboBoxUpdated(QComboBox* cb)\n{\n", filter.toLatin1().data());
    fprintf(f, "  if (NULL == cb) { return; }\n"); // Make sure we have a non null QWidget to deal with
    for (size_t i = 0; i < options.size(); ++i)
    {
      FilterParameter::Pointer opt = options[i];
      QString prop = opt->getPropertyName();
      QString typ = opt->getValueType();
      QString hl = opt->getHumanLabel();
      if (opt->getWidgetType() >= FilterParameter::VolumeCellArrayNameSelectionWidget
          && opt->getWidgetType() <= FilterParameter::VertexEnsembleArrayNameSelectionWidget )
      {
        fprintf(f, "  if(cb->objectName().compare(\"%s\") == 0) {\n", prop.toLatin1().data());
        fprintf(f, "    m_%s = cb->currentText();\n  }\n", prop.toLatin1().data());
      }
    }
    fprintf(f, "}\n");
  }

  if (implementArrayNameSelectionWidget == true || implementComparisonSelectionWidget == true)
  {

    fprintf(f, "\n// -----------------------------------------------------------------------------\n");
    fprintf(f, "void Q%sWidget::preflightAboutToExecute(DataContainerArray::Pointer dca)\n{\n", filter.toLatin1().data());
    for (size_t i = 0; i < options.size(); ++i)
    {
      FilterParameter::Pointer opt = options[i];
      QString prop = opt->getPropertyName();
      QString typ = opt->getValueType();
      QString hl = opt->getHumanLabel();
      if (opt->getWidgetType() == FilterParameter::ArraySelectionWidget )
      {
        fprintf(f, "  {\n    ArraySelectionWidget* w = qFindChild<ArraySelectionWidget*>(this, \"%s\");\n", prop.toLatin1().data()); // Make sure we have a non null QWidget to deal with

        fprintf(f, "    if (NULL != w) {\n      w->populateArrayNames(dca);\n    }\n  }\n");
      }
      if (opt->getWidgetType() >= FilterParameter::CellArrayComparisonSelectionWidget
          && opt->getWidgetType() <= FilterParameter::EdgeArrayComparisonSelectionWidget)
      {
        fprintf(f, "  {\n    ComparisonSelectionWidget* w = qFindChild<ComparisonSelectionWidget*>(this, \"%s\");\n", prop.toLatin1().data()); // Make sure we have a non null QWidget to deal with

        fprintf(f, "    if (NULL != w) {\n      w->populateArrayNames(dca);\n    }\n  }\n");
      }
    }
    fprintf(f, "}\n");
  }



  /* Implement the htmlHelpIndexFile() method */
  QString lower = filter.toLower();

  fprintf(f, "\n// -----------------------------------------------------------------------------\n");
  fprintf(f, "void Q%sWidget::openHtmlHelpFile()\n{\n", filter.toLatin1().data());
  fprintf(f, "\tDREAM3DHelpUrlGenerator::generateAndOpenHTMLUrl(\"%s\", this);\n", lower.toLatin1().data());
  fprintf(f, "}\n");
  fprintf(f, "\n\n");


  // Close the file we are currently writing
  fclose(f);

  // Now compare the file just generated with any possible existing file
  QFileInfo curFi(completePath);
  QFileInfo tmpFi(tempPath);

  qint64 currentFileSize = curFi.size();
  qint64 tempFileSize = tmpFi.size();

  // If the file sizes are different then copy the file
  if(currentFileSize != tempFileSize)
  {
    qDebug() << "0-Creating Source File: " << completePath;
    copyFile(tempPath, completePath);
  }
  else // Just because the files are the same size does not mean they are the same.
  {

    QString currentHash = Md5OfFile(completePath);
    QString tempHash = Md5OfFile(tempPath);

    // Use MD5 Checksums to figure out if the files are different
    if (tempHash != currentHash)
    {
      qDebug() << "0-Copying Source File: " << completePath;
      qDebug() << "    Hex Digest:    " << QString(currentHash);
      qDebug() << "    tempHexDigest: " << QString(tempHash);
      copyFile(tempPath, completePath);
    }

    QFile tempFile(tempPath);
    if(tempFile.remove() == false)
    {
      qDebug() << "Could not remove temp Source file " << tempPath;
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void createPreflightTestCode( const QString& group, const QString& filter)
{
  QString s = FILTER_WIDGETS_TEMP_DIR();
  s.append("Preflight_Test_Code_Fragment_1.h");
  FILE* f = fopen(s.toLatin1().data(), "ab+"); // Clear out this file
  fprintf(f, "MAKE_FILTER_TEST(  %s, FAIL_IS_PASS)\n", filter.toLatin1().data());
  fclose(f);


  s = FILTER_WIDGETS_TEMP_DIR();
  s.append("Preflight_Test_Code_Fragment_2.h");
  f = fopen(s.toLatin1().data(), "ab+"); // Clear out this file
  fprintf(f, "DREAM3D_REGISTER_TEST( %s_PreFlightTest() )\n", filter.toLatin1().data());
  fclose(f);

}




#include "FilterWidgetCodeGen.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // if (true) return 0;

#if (GENERATE_PREFLIGHT_TEST_CODE_FRAGMENT == 1)
  QString s = FILTER_WIDGETS_TEMP_DIR();
  s.append("Preflight_Test_Code_Fragment_1.h");
  FILE* f = fopen(s.toLatin1().data(), "wb"); // Clear out this file
  fclose(f);

  s = FILTER_WIDGETS_TEMP_DIR();
  s.append("Preflight_Test_Code_Fragment_2.h");
  f = fopen(s.toLatin1().data(), "wb"); // Clear out this file
  fclose(f);
#endif

  setFilters();

  createMarkdownCreatedArrayIndex();

  return 0;
}


