

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

  int index = cpp.indexOf("m->createNonPrereqArray<");
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
    index = cpp.indexOf("m->createNonPrereqArray<", index + 1);
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

      QString dc("DREAM3D_INSTANCE_STRING_PROPERTY(DataContainerName)\n");
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
        replaceText(header, source);
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
