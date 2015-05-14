/* ============================================================================
* Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
* Copyright (c) 2012 Joseph B. Kleingers (Student Research Assistant)
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
* Neither the name of Michael A. Groeber, Michael A. Jackson, Joseph B. Kleingers,
* the US Air Force, BlueQuartz Software nor the names of its contributors may be
* used to endorse or promote products derived from this software without specific
* prior written permission.
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

#include "DREAM3DSettings.h"

#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>
#include <QtCore/QJsonDocument>
#include <QtCore/QTextCodec>

#include <QtWidgets/QTreeWidgetItemIterator>

#include <iostream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DSettings::DREAM3DSettings(QObject *parent)
{
  setParent(parent);

#if defined (Q_OS_MAC)
  QSettings::Format format = QSettings::NativeFormat;
#else
  QSettings::Format format = QSettings::IniFormat;
#endif

  QSettings prefs(format, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
  QString filePath = prefs.fileName();

  // Now turn the .ini path into .json path
  QFileInfo fi(filePath);
  QString ext = fi.completeSuffix();
  filePath.chop(ext.size());
  filePath.append("json");

  m_FilePath = filePath;

  // Open the file and read the contents, if it exists
  openFile();

  m_CurrentGroup = m_Root;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DSettings::DREAM3DSettings(const QString &filePath, QObject *parent)
{
  setParent(parent);

  m_FilePath = filePath;

  // Open the file and read the contents, if it exists
  openFile();

  m_CurrentGroup = m_Root;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DSettings::~DREAM3DSettings()
{
  QFile outputFile(m_FilePath);
  QFileInfo info(outputFile);
  QString parentPath = info.absolutePath();
  QDir parentDir(parentPath);

  if (parentDir.exists() == false)
  {
    parentDir.mkpath(parentPath);
  }

  QJsonDocument doc(m_Root);

  if (outputFile.exists() == true)
  {
    outputFile.remove();
  }
  if (outputFile.open(QIODevice::WriteOnly))
  {
    outputFile.write(doc.toJson());
    outputFile.close();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DREAM3DSettings::fileName()
{
  return m_FilePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DREAM3DSettings::contains(const QString &key)
{
  return m_CurrentGroup.contains(key);
}

// -----------------------------------------------------------------------------
// Opens a group.  Groups CAN NOT currently be nested.
// -----------------------------------------------------------------------------
bool DREAM3DSettings::beginGroup(const QString &prefix)
{
  if (m_CurrentGroupName.isEmpty() == false)
  {
    // A group is already open, so return.
    return false;
  }

  // Set the new group name
  m_CurrentGroupName = prefix;

  if (m_Root.contains(prefix) == true)
  {
    m_CurrentGroup = m_Root[prefix].toObject();
  }
  else
  {
    m_CurrentGroup = QJsonObject();
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DSettings::endGroup()
{
  m_Root[m_CurrentGroupName] = m_CurrentGroup;
  m_CurrentGroup = m_Root;
  m_CurrentGroupName = "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant DREAM3DSettings::value(const QString &key, const QVariant &defaultValue)
{
  if (m_CurrentGroup.contains(key) == false)
  {
    return defaultValue;
  }

  if (m_CurrentGroupName == "WindowSettings")
  {
    QByteArray byteArray8Bit = m_CurrentGroup.value(key).toString().toLocal8Bit();
    QByteArray byteArray = QByteArray::fromBase64(byteArray8Bit);
    return byteArray;
  }
  else
  {
    return m_CurrentGroup.value(key).toVariant();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DSettings::setValue(const QString &key, const QVariant &value)
{
  if (QString::fromStdString(std::string(value.typeName())) == "QByteArray")
  {
    QByteArray byteArray = value.toByteArray().toBase64();
    QString str = QString::fromLocal8Bit(byteArray);
    m_CurrentGroup.insert(key, str);
  }
  else
  {
    QJsonValue val = QJsonValue::fromVariant(value);
    QJsonObject obj = val.toObject();
    m_CurrentGroup.insert(key, obj);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DSettings::openFile()
{
  if (m_Root.isEmpty() == false || m_CurrentGroup.isEmpty() == false)
  {
    closeFile();
  }

  QFile inputFile(m_FilePath);
  if (inputFile.open(QIODevice::ReadOnly))
  {
    QByteArray byteArray = inputFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(byteArray);
    m_Root = doc.object();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DSettings::closeFile()
{
  m_Root = QJsonObject();
  m_CurrentGroup = QJsonObject();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject DREAM3DSettings::value(const QString &key, const QJsonObject &defaultObject)
{
  if (m_Root[key].isObject())
  {
    return m_Root[key].toObject();
  }
  return QJsonObject();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DSettings::setValue(const QString &key, const QJsonObject &object)
{
  m_Root[key] = object;
}





