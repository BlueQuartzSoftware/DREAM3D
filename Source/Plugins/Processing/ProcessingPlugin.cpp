/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ProcessingPlugin.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "moc_ProcessingPlugin.cpp"

Q_EXPORT_PLUGIN2(ProcessingPlugin, ProcessingPlugin)

namespace Detail
{
  const QString ProcessingPluginFile("ProcessingPlugin");
  const QString ProcessingPluginDisplayName("Processing");
  const QString ProcessingPluginBaseName("ProcessingPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessingPlugin::ProcessingPlugin() :
m_Version(DREAM3DLib::Version::Package()),
m_CompatibilityVersion(DREAM3DLib::Version::Package()),
m_Vendor(DREAM3D::BlueQuartz::VendorName),
m_URL(DREAM3D::BlueQuartz::URL),
m_Location(""),
m_Copyright(DREAM3D::BlueQuartz::Copyright),
m_Filters(QList<QString>()),
  m_DidLoad(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessingPlugin::~ProcessingPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ProcessingPlugin::getPluginName()
{
  return (Detail::ProcessingPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ProcessingPlugin::getVersion()
{
  return m_Version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ProcessingPlugin::getCompatibilityVersion()
{
  return m_CompatibilityVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ProcessingPlugin::getVendor()
{
  return m_Vendor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ProcessingPlugin::getURL()
{
  return m_URL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ProcessingPlugin::getLocation()
{
  return m_Location;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ProcessingPlugin::getDescription()
{
  QFile licenseFile(":/Processing/ProcessingDescription.txt");
  QFileInfo licenseFileInfo(licenseFile);
  QString text = "<<--Description was not read-->>";

  if ( licenseFileInfo.exists() )
  {
    if ( licenseFile.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
      QTextStream in(&licenseFile);
      text = in.readAll();
    }
  }
  return text;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ProcessingPlugin::getCopyright()
{
  return m_Copyright;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ProcessingPlugin::getLicense()
{
  QFile licenseFile(":/DREAM3D/DREAM3DLicense.txt");
  QFileInfo licenseFileInfo(licenseFile);
  QString text = "<<--License was not read-->>";

  if ( licenseFileInfo.exists() )
  {
    if ( licenseFile.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
      QTextStream in(&licenseFile);
      text = in.readAll();
    }
  }
  return text;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> ProcessingPlugin::getFilters()
{
  return m_Filters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, QString> ProcessingPlugin::getThirdPartyLicenses()
{
  QMap<QString, QString> licenseMap;
  QList<QString> fileStrList;
  fileStrList.push_back(":/ThirdParty/HDF5.txt");
  fileStrList.push_back(":/ThirdParty/Boost.txt");
  fileStrList.push_back(":/ThirdParty/Qt.txt");
  fileStrList.push_back(":/ThirdParty/Qwt.txt");

  for (QList<QString>::iterator iter = fileStrList.begin(); iter != fileStrList.end(); iter++)
  {
    QFile file(*iter);
    QFileInfo licenseFileInfo(file);

    if ( licenseFileInfo.exists() )
    {
      if ( file.open(QIODevice::ReadOnly | QIODevice::Text) )
      {
        QTextStream in(&file);
        licenseMap.insert(licenseFileInfo.baseName(), in.readAll());
      }
    }
  }

  return licenseMap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ProcessingPlugin::getDidLoad()
{
  return m_DidLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessingPlugin::setDidLoad(bool didLoad)
{
  m_DidLoad = didLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessingPlugin::setLocation(QString filePath)
{
  m_Location = filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessingPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessingPlugin::readSettings(QSettings& prefs)
{

}

#include "ProcessingFilters/RegisterKnownFilters.cpp"
#include "Processing/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"

