

#include <QtCore/QFile>

#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QtDebug>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>


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
     hIndex = header.indexOf("DREAM3D_INSTANCE_STRING_PROPERTY(CellFeatureAttributeMatrixName)");
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
     hIndex = header.indexOf("DREAM3D_INSTANCE_STRING_PROPERTY(CellEnsembleAttributeMatrixName)");
     if(hIndex < 0) // This class does not have a Ensemble Attribute Matrix Name
     {
      cpp.replace(searchStr, "size_t totalEnsembles = 0;");
      doReplace = true;
     }
  }

  //// WRITE THE HEADER BACK OUT TO A FILE
  if(doReplace== true)
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

  int index = cpp.indexOf("m->getPrereqArray<");
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
    sub = sub.mid(2, endIdx-2);
    varNames << sub;
    doReplace = true;
    index = cpp.indexOf("m->getPrereqArray<", index + 1);
    //break;
  }

  if (doReplace == false) { return; }
  doReplace = false;
  // Now open the header file and read that.
  for (int i = 0; i < varNames.size(); ++i)
  {
    QString s("DREAM3D_INSTANCE_STRING_PROPERTY(" + varNames.at(i) + "AttributeMatrixName)");
    index = header.indexOf(s, 0);
    if (index < 0) { // Not found

      QString dc("DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceDataContainerName)\n");
      int idx = header.indexOf(dc, 0);
      if (idx > 0)
      {
        header.replace(dc,  dc + "    " + s + "\n");
        doReplace = true;
      }
    }
  }


  //// WRITE THE HEADER BACK OUT TO A FILE
  if(doReplace== true)
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
        // qDebug("Found %s matching pattern.", qPrintable(filename));
        QFileInfo fi(iterator.filePath());
        QString header = fi.path() + "/" + fi.baseName() + ".h";
        QString source = iterator.filePath();
        replaceText1(header, source);
      }
    }
  }

}



int main(int argc, char *argv[])
{

  QString header = argv[1];
  QString source = argv[2];

  scanDirIter(QString("/Users/mjackson/Workspace/DREAM3D_Rewrite/Source/DREAM3DLib"));
  scanDirIter(QString("/Users/mjackson/Workspace/DREAM3D_Rewrite/Source/Plugins"));

  return 0;
}
