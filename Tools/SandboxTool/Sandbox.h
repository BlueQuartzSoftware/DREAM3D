#pragma once

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#define OVERWRITE_SOURCE_FILE 1

class Sandbox
{

public:
  Sandbox() = default;

  QString quote(const QString& str)
  {
    return QString("\"%1\"").arg(str);
  }

  /**
   * @brief writeOutput
   * @param didReplace
   * @param outLines
   * @param filename
   */
  void writeOutput(bool didReplace, const QStringList& outLines, const QString& filename)
  {
    QString finalOutPath = filename;
    QFileInfo fi(filename);
    if(didReplace)
    {
      QFileInfo fi(filename);
#if OVERWRITE_SOURCE_FILE
      QString filePath = filename;
#else
      QString filePath = "/tmp/" + fi.fileName();
      fi = QFileInfo(filePath);
#endif
      QFile hOut(filePath);
      hOut.open(QFile::WriteOnly);
      QTextStream stream(&hOut);
      stream << outLines.join("\n");
      hOut.close();
      qDebug() << "Saved File " << fi.absoluteFilePath();
    }
  }

  /**
   * @brief writeOutput
   * @param didReplace
   * @param outLines
   * @param filename
   */
  void writeOutput(bool didReplace, QVector<QString>& outLines, const QString& filename)
  {
    QString finalOutPath = filename;
    QFileInfo fi(filename);
    if(didReplace)
    {
      QFileInfo fi(filename);
#if OVERWRITE_SOURCE_FILE
      QString filePath = filename;
#else
      QString filePath = "/tmp/" + fi.fileName();
      fi = QFileInfo(filePath);
#endif
      QFile hOut(filePath);
      hOut.open(QFile::WriteOnly);
      QTextStream stream(&hOut);
      for(qint32 i = 0; i < outLines.size() - 1; i++)
      {
        stream << outLines[i] << "\n";
      }
      hOut.close();

      qDebug() << "Saved File " << fi.absoluteFilePath();
    }
  }
};
