

#include <iostream>

#include <QtCore/QFile>

#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QtDebug>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>

#include "TestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void buildInitializerList(QString hFile, QString cppFile)
{

  // Read the header file
  QFile h(hFile);
  h.open(QFile::ReadOnly);
  QString header = h.readAll();
  h.close();

  QStringList list;
  QStringList initializerList;

  list = header.split(QRegExp("\\n"));

  QStringListIterator lines(list);
  while (lines.hasNext())
  {

    QString line = lines.next();
    // std::cout << line.toStdString() << std::endl;
    if(line.contains(QString("SIMPL_INSTANCE_STRING_PROPERTY")) )
    {
      QStringList chunks = line.split(QRegExp("\\("));
      chunks = chunks.at(1).split(QRegExp("\\)"));
      QString s = QString("m_") + chunks.at(0);
      s = s.trimmed();
      initializerList << s;
    }

    if(line.contains(QString("SIMPL_INSTANCE_PROPERTY")) )
    {
      QStringList chunks = line.split(QRegExp(", "));
      chunks = chunks.at(1).split(QRegExp("\\)"));
      QString s = QString("m_") + chunks.at(0);
      s = s.trimmed();
      initializerList << s;
    }

    if(line.contains(QString("DEFINE_DATAARRAY_VARIABLE")) )
    {
      QStringList chunks = line.split(QRegExp(", "));
      chunks = chunks.at(1).split(QRegExp("\\)"));
      QString s = QString("m_") + chunks.at(0) + QString("ArrayName");
      initializerList << s;
      s = QString("m_") + chunks.at(0) + "(NULL)";
      s = s.trimmed();
      initializerList << s;
    }

  }
  //  lines = QStringListIterator(initializerList);
  //  while (lines.hasNext())
  //  {
  //    std::cout << lines.next().toStdString() << std::endl;
  //  }

  // now read the source file
  QFile source(cppFile);
  source.open(QFile::ReadOnly);
  QString cpp = source.readAll();
  source.close();

  QFileInfo fi(hFile);
  QFile outSource(cppFile);
  outSource.open(QFile::WriteOnly);
  QTextStream stream(&outSource);




  QString constructor = fi.baseName() + "::" + fi.baseName();
  QStringList existinInitializerList;

  list = cpp.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    stream << line << "\n";
    if(line.contains(constructor) == true)
    {
      // We are in the constructor
      line = sourceLines.next(); // Should be the super class
      stream << line << "\n";
      line = sourceLines.next(); // Should be the first instance variable of class
      bool stop = false;
      while(stop == false)
      {
        QStringList chunks = line.split(',');
        existinInitializerList << chunks.at(0).trimmed();
        if(chunks.size() == 1) { stop = true; }
        else { line = sourceLines.next(); }
      }
      break;
    }
  }

  //  lines = QStringListIterator(existinInitializerList);
  //  while (lines.hasNext())
  //  {
  //    std::cout << lines.next().toStdString() << std::endl;
  //  }

  QStringList newList;

  lines = QStringListIterator(initializerList);
  while (lines.hasNext())
  {
    QString line = lines.next();
    QStringList result;
    result = existinInitializerList.filter(line + "(");

    if(result.size() == 1)
    {
      newList << result.at(0);
    }
    else
    {

      if(line.contains("NULL") == false)
      {
        newList << line + QString("");
      }
      else
      {
        newList << QString(line);
      }
    }
  }
  //  std::cout << "----------------" << std::endl;

  QString outS;
  QTextStream ss(&outS);

  lines = QStringListIterator(newList);
  while (lines.hasNext())
  {
    ss << "  " << lines.next();
    if(lines.hasNext() == true) { ss << ",\n"; }
    else { ss << "\n"; }
  }

  stream << outS;

  // Finish writing the source file back out
  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    stream << line << "\n";
  }

  outSource.close();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void replaceText1(QString hFile, QString cppFile)
{
  QFile h(hFile);
  h.open(QFile::ReadOnly);
  QString header = h.readAll();
  h.close();

  QFile source(cppFile);
  source.open(QFile::ReadOnly);
  QString cpp = source.readAll();

  QStringList varNames;
  QStringList typeNames;

  bool doReplace = false;

  QString searchStr("size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();");
  int index = cpp.indexOf(searchStr);
  int hIndex = -1;
  if(index > 0)
  {
    hIndex = header.indexOf("SIMPL_INSTANCE_STRING_PROPERTY(CellFeatureAttributeMatrixName)");
    if(hIndex < 0) // This class does not have a Feature Attribute Matrix Name
    {
      cpp.replace(searchStr, "size_t totalFeatures = 0;");
      doReplace = true;
    }
  }


  searchStr = QString("size_t totalEnsembles = m->getAttributeMatrix(getCellEnsembleAttributeMatrixName())->getNumTuples();");
  index = cpp.indexOf(searchStr);
  hIndex = -1;
  if(index > 0)
  {
    hIndex = header.indexOf("SIMPL_INSTANCE_STRING_PROPERTY(CellEnsembleAttributeMatrixName)");
    if(hIndex < 0) // This class does not have a Ensemble Attribute Matrix Name
    {
      cpp.replace(searchStr, "size_t totalEnsembles = 0;");
      doReplace = true;
    }
  }

  //// WRITE THE HEADER BACK OUT TO A FILE
  if(doReplace == true)
  {
    QFileInfo fi(cppFile);
#if 0
    QFile hOut("/tmp/junk/" + fi.fileName());
#else
    QFile hOut(cppFile);
#endif
    hOut.open(QFile::WriteOnly);
    QTextStream stream( &hOut );
    stream << cpp;
    hOut.close();

    qDebug() << "Saved File " << cppFile;
  }

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void replaceText(QString hFile, QString cppFile)
{
  QFile h(hFile);
  h.open(QFile::ReadOnly);
  QString header = h.readAll();
  h.close();

  QFile source(cppFile);
  source.open(QFile::ReadOnly);
  QString cpp = source.readAll();

  QStringList varNames;
  QStringList typeNames;

  int index = cpp.indexOf("cellAttrMat->getPrereqArray<");
  int endIdx = 0;
  bool doReplace = false;
  while (index > 0)
  {
    endIdx = cpp.indexOf(";", index);
    QString sub = cpp.mid(index, endIdx - index);
    sub = sub.section(',', 2, 2);
    sub = sub.trimmed();
    QString _type = cpp.mid(index, endIdx - index);
    endIdx = sub.indexOf("AttributeMatrixName");
    sub = sub.mid(2, endIdx - 2);
    varNames << sub;
    doReplace = true;
    index = cpp.indexOf("cellAttrMat->getPrereqArray<", index + 1);
    //break;
  }

  if (doReplace == false) { return; }
  doReplace = false;
  // Now open the header file and read that.
  for (int i = 0; i < varNames.size(); ++i)
  {
    QString s("SIMPL_INSTANCE_STRING_PROPERTY(" + varNames.at(i) + "AttributeMatrixName)");
    index = header.indexOf(s, 0);
    if (index < 0)   // Not found
    {

      QString dc("SIMPL_INSTANCE_STRING_PROPERTY(SurfaceDataContainerName)\n");
      int idx = header.indexOf(dc, 0);
      if (idx > 0)
      {
        header.replace(dc,  dc + "    " + s + "\n");
        doReplace = true;
      }
    }
  }


  //// WRITE THE HEADER BACK OUT TO A FILE
  if(doReplace == true)
  {
    QFileInfo fi(hFile);
#if 0
    QFile hOut("/tmp/junk/" + fi.fileName());
#else
    QFile hOut(hFile);
#endif
    hOut.open(QFile::WriteOnly);
    QTextStream stream( &hOut );
    stream << header;
    hOut.close();

    qDebug() << "Saved File " << hFile;
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void fixDataCheck(QString hFile, QString cppFile)
{
  // Read the Source File
  QFileInfo fi(cppFile);
  QFile source(cppFile);
  source.open(QFile::ReadOnly);
  QString cpp = source.readAll();
  source.close();

  QStringList list;
  QStringList initializerList;

  list = cpp.split(QRegExp("\\n"));

  QString dataCheck = QString("void ") + fi.baseName() + QString("::dataCheck(");
  QString preflight = QString("void ") + fi.baseName() + QString("::preflight()");

  bool needCellAttrGet = false;
  bool needCellAttrCreate = false;
  bool hasCellGet = false;
  bool hasCellCreate = false;
  bool alreadyHasDecl = false;


  QStringListIterator sourceLines(list);
  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    if(line.contains(dataCheck) )
    {
      // We are in the constructor
      bool stop = false;
      while(stop == false)
      {
        line = sourceLines.next();
        if(line.contains("cellEnsembleAttrMat->get"))
        {
          hasCellGet = true;
        }
        if(line.contains("cellEnsembleAttrMat->create"))
        {
          hasCellCreate = true;
        }
        if(line.contains("AttributeMatrix* cellEnsembleAttrMat ="))
        {
          alreadyHasDecl = true;
        }


        if(line.contains(preflight)) { stop = true; }
        if(sourceLines.hasNext() == false) { stop = true; }
      }
      break;
    }
  }
  if(hasCellGet == true && alreadyHasDecl == false)
  {
    needCellAttrGet = true;
  }
  else if(hasCellCreate == true && alreadyHasDecl == false)
  {
    needCellAttrCreate = true;
  }



  if (!needCellAttrGet && !needCellAttrCreate) { return; }

  qDebug("Updating %s", qPrintable(cppFile));

#if 0
  QFile fout("/tmp/out.cpp");
#else
  QFile fout(cppFile);
#endif
  fout.open(QFile::WriteOnly);
  QTextStream out(&fout);


  sourceLines = QStringListIterator(list);
  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    if(line.contains(dataCheck) )
    {
      out << line << "\n";
      // We are in the constructor
      bool stop = false;
      while(stop == false)
      {
        line = sourceLines.next();
        if(line.contains("VolumeDataContainer* m = getDataContainerArray()"))
        {
          out << line << "\n";
          line = sourceLines.next();
          out << line << "\n";
          if(needCellAttrGet)
          {
            out << "  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), -301);\n";
            out << "  if(getErrorCondition() < 0) { return; }\n";
          }
          if(needCellAttrCreate)
          {
            out << "  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), DREAM3D::AttributeMatrixType::CellEnsemble);\n";
            out << "  if(getErrorCondition() < 0) { return; }\n";
          }

          stop = true;
        }
        else
        {
          out << line << "\n";
        }
        if(line.contains(preflight)) { out << line << "\n"; stop = true; }

      }
    }
    else
    {
      if (sourceLines.hasNext()) { out << line << "\n"; }
    }

  }

  fout.close();
}


void scanDirIter(QDir dir)
{
  QDirIterator iterator(dir.absolutePath(), QDirIterator::Subdirectories);
  while (iterator.hasNext())
  {
    iterator.next();
    if (!iterator.fileInfo().isDir())
    {
      QString filename = iterator.fileName();
      if (filename.endsWith(".cpp") )
      {
        //qDebug("Found %s matching pattern.", qPrintable(filename));
        QFileInfo fi(iterator.filePath());
        QString header = fi.path() + "/" + fi.baseName() + ".h";
        QString source = iterator.filePath();
        fixDataCheck(header, source);
      }
    }
  }

}



int main(int argc, char* argv[])
{
  Q_ASSERT(false); // We don't want anyone to run this program.

//#if 0
//  QString header = argv[1];
//  QString source = argv[2];
//  buildInitializerList(header, source);
//#else
//  scanDirIter(UnitTest::DREAM3DProjDir + QString("/Source/DREAM3DLib"));
//  scanDirIter(UnitTest::DREAM3DProjDir + QString("/Source/Plugins"));
//#endif

  return 0;
}
