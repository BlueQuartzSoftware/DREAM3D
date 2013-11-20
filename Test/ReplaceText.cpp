

#include <QtCore/QFile>

#include <QtCore/QString>





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

  int index = cpp.indexOf("m->getPrereqArray<");
  while (index > 0)
  {

    index = cpp.indexOf("m->getPrereqArray<", index + 1);

  }

}










int main(int argc, char *argv[])
{




  return 0;
}
