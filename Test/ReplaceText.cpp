

#include <QtCore/QFile>

#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QtDebug>
#include <QtCore/QStringList>



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void replaceText(QString hFile, QString cppFile)
{
  QFile h(hFile);
  h.open(QFile::ReadOnly);
  QString header = h.readAll();

  QFile source(cppFile);
  source.open(QFile::ReadOnly);
  QString cpp = source.readAll();

  QStringList varNames;
  QStringList typeNames;

  int index = cpp.indexOf("m->getPrereqArray<");
  int endIdx = 0;
  while (index > 0)
  {
    endIdx = cpp.indexOf(";", index);
    QString sub = cpp.mid(index, endIdx - index);
    sub = sub.section(',', 3, 3);   // str == "surname"
    sub = sub.trimmed();
    QString _type = cpp.mid(index, endIdx - index);
    endIdx = sub.indexOf("ArrayName");
    sub = sub.mid(2, endIdx-2);
    varNames << sub;

    _type = _type.section('<', 1, 1);
    _type = _type.section(',', 0, 0);
    typeNames << _type;
    qDebug() << sub;

    index = cpp.indexOf("m->getPrereqArray<", index + 1);
  }


  // Now open the header file and read that.
  for (int i = 0; i < varNames.size(); ++i)
  {
    QString s = typeNames.at(i) + "* m_" + varNames.at(i);
    index = header.indexOf(s, 0);
    QString replace = "DEFINE_PTR_WEAKPTR_DATAARRAY(" + typeNames.at(i) + ", " + varNames.at(i) + ")";
    header.replace(s, replace);
  }


  index = cpp.indexOf("m->createNonPrereqArray<");
  endIdx = 0;
  while (index > 0)
  {
    endIdx = cpp.indexOf(";", index);
    QString sub = cpp.mid(index, endIdx - index);
    sub = sub.section(',', 3, 3);   // str == "surname"
    sub = sub.trimmed();
    QString _type = cpp.mid(index, endIdx - index);
    endIdx = sub.indexOf("ArrayName");
    sub = sub.mid(2, endIdx-2);
    varNames << sub;

    _type = _type.section('<', 1, 1);
    _type = _type.section(',', 0, 0);
    typeNames << _type;
    qDebug() << sub;

    index = cpp.indexOf("m->createNonPrereqArray<", index + 1);
  }


  // Now open the header file and read that.
  for (int i = 0; i < varNames.size(); ++i)
  {
    QString s = typeNames.at(i) + "* m_" + varNames.at(i);
    index = header.indexOf(s, 0);
    QString replace = "DEFINE_PTR_WEAKPTR_DATAARRAY(" + typeNames.at(i) + ", " + varNames.at(i) + ")\n";
    header.replace(s, replace);

    //DREAM3D_INSTANCE_STRING_PROPERTY(FeatureIdsArrayName)
    s = "DREAM3D_INSTANCE_STRING_PROPERTY(" + varNames.at(i) + "ArrayName)";
    header.replace(s, "");
  }


  //// WRITE THE HEADER BACK OUT TO A FILE
  h.close();
  QFileInfo fi(hFile);
  QFile hOut("/tmp/" + fi.fileName() );
  hOut.open(QFile::WriteOnly);
  QTextStream stream( &hOut );
    stream << header;
    hOut.close();


}










int main(int argc, char *argv[])
{

  QString header = argv[1];
  QString source = argv[2];
  replaceText(header, source);

  return 0;
}
