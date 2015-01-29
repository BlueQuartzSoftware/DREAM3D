/*
 * Your License or Copyright Information can go here
 */


#include "Version4CompatibilityPlugin.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"


#include "Version4Compatibility/moc_Version4CompatibilityPlugin.cpp"

Q_EXPORT_PLUGIN2(Version4CompatibilityPlugin, Version4CompatibilityPlugin)

namespace Detail
{
  const QString Version4CompatibilityPluginFile("Version4CompatibilityPlugin");
  const QString Version4CompatibilityPluginDisplayName("Version4Compatibility");
  const QString Version4CompatibilityPluginBaseName("Version4CompatibilityPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Version4CompatibilityPlugin::Version4CompatibilityPlugin() :
m_Version(DREAM3DLib::Version::Package()),
m_CompatibilityVersion(DREAM3DLib::Version::Package()),
m_Vendor(DREAM3D::BlueQuartz::VendorName),
m_URL(DREAM3D::BlueQuartz::URL),
m_Location(""),
m_Platforms(QList<QString>()),
m_Copyright(DREAM3D::BlueQuartz::Copyright),
m_Dependencies(QList<QString>()),
  m_DidLoad(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Version4CompatibilityPlugin::~Version4CompatibilityPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Version4CompatibilityPlugin::getPluginName()
{
  return (Detail::Version4CompatibilityPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Version4CompatibilityPlugin::getVersion()
{
  return m_Version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Version4CompatibilityPlugin::getCompatibilityVersion()
{
  return m_CompatibilityVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Version4CompatibilityPlugin::getVendor()
{
  return m_Vendor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Version4CompatibilityPlugin::getURL()
{
  return m_URL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Version4CompatibilityPlugin::getLocation()
{
  return m_Location;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> Version4CompatibilityPlugin::getPlatforms()
{
  return m_Platforms;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Version4CompatibilityPlugin::getDescription()
{
  QFile licenseFile(":/Version4Compatibility/Version4CompatibilityDescription.txt");
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
QString Version4CompatibilityPlugin::getCopyright()
{
  return m_Copyright;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString Version4CompatibilityPlugin::getLicense()
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
QList<QString> Version4CompatibilityPlugin::getDependencies()
{
  return m_Dependencies;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, QString> Version4CompatibilityPlugin::getThirdPartyLicenses()
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
bool Version4CompatibilityPlugin::getDidLoad()
{
  return m_DidLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Version4CompatibilityPlugin::setDidLoad(bool didLoad)
{
  m_DidLoad = didLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Version4CompatibilityPlugin::setLocation(QString filePath)
{
  m_Location = filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Version4CompatibilityPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Version4CompatibilityPlugin::readSettings(QSettings& prefs)
{

}

#include "Version4CompatibilityFilters/RegisterKnownFilters.cpp"

#include "Version4Compatibility/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"

