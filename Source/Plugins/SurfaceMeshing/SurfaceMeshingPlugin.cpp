/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SurfaceMeshingPlugin.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include <QtCore/QTextStream>

#include "SIMPLib/Filtering/FilterFactory.hpp"

#include "SIMPLib/Filtering/FilterManager.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshingPlugin::SurfaceMeshingPlugin()
: m_Version(SurfaceMeshing::Version::Package())
, m_CompatibilityVersion(SurfaceMeshing::Version::Package())
, m_Vendor(SurfaceMeshingConstants::BlueQuartz::VendorName)
, m_URL(SurfaceMeshingConstants::BlueQuartz::URL)
, m_Location("")
, m_Copyright(SurfaceMeshingConstants::BlueQuartz::Copyright)
, m_Filters(QList<QString>())
, m_DidLoad(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshingPlugin::~SurfaceMeshingPlugin() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SurfaceMeshingPlugin::getPluginFileName()
{
  return SurfaceMeshingConstants::SurfaceMeshingPluginFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SurfaceMeshingPlugin::getPluginDisplayName()
{
  return SurfaceMeshingConstants::SurfaceMeshingPluginDisplayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SurfaceMeshingPlugin::getPluginBaseName()
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SurfaceMeshingPlugin::getVersion()
{
  return m_Version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SurfaceMeshingPlugin::getCompatibilityVersion()
{
  return m_CompatibilityVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SurfaceMeshingPlugin::getVendor()
{
  return m_Vendor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SurfaceMeshingPlugin::getURL()
{
  return m_URL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SurfaceMeshingPlugin::getLocation()
{
  return m_Location;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SurfaceMeshingPlugin::getDescription()
{
  QFile licenseFile(":/SurfaceMeshing/SurfaceMeshingDescription.txt");
  QFileInfo licenseFileInfo(licenseFile);
  QString text = "<<--Description was not read-->>";

  if(licenseFileInfo.exists())
  {
    if(licenseFile.open(QIODevice::ReadOnly | QIODevice::Text))
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
QString SurfaceMeshingPlugin::getCopyright()
{
  return m_Copyright;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SurfaceMeshingPlugin::getLicense()
{
  QFile licenseFile(":/DREAM3D/DREAM3DLicense.txt");
  QFileInfo licenseFileInfo(licenseFile);
  QString text = "<<--License was not read-->>";

  if(licenseFileInfo.exists())
  {
    if(licenseFile.open(QIODevice::ReadOnly | QIODevice::Text))
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
QMap<QString, QString> SurfaceMeshingPlugin::getThirdPartyLicenses()
{
  QMap<QString, QString> licenseMap;
  QList<QString> fileStrList;
  fileStrList.push_back(":/ThirdParty/HDF5.txt");
  
  fileStrList.push_back(":/ThirdParty/Qt.txt");
  fileStrList.push_back(":/ThirdParty/Qwt.txt");

  for(QList<QString>::iterator iter = fileStrList.begin(); iter != fileStrList.end(); iter++)
  {
    QFile file(*iter);
    QFileInfo licenseFileInfo(file);

    if(licenseFileInfo.exists())
    {
      if(file.open(QIODevice::ReadOnly | QIODevice::Text))
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
bool SurfaceMeshingPlugin::getDidLoad()
{
  return m_DidLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingPlugin::setDidLoad(bool didLoad)
{
  m_DidLoad = didLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingPlugin::setLocation(QString filePath)
{
  m_Location = filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingPlugin::writeSettings(QSettings& prefs)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingPlugin::readSettings(QSettings& prefs)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshingPlugin::registerFilterWidgets(FilterWidgetManager* fwm)
{
}

#include "SurfaceMeshingFilters/RegisterKnownFilters.cpp"
