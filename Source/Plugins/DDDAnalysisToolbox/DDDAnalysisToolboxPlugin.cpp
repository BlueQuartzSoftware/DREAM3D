/*
 * Your License or Copyright Information can go here
 */


#include "DDDAnalysisToolboxPlugin.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"


#include "DDDAnalysisToolbox/moc_DDDAnalysisToolboxPlugin.cpp"

Q_EXPORT_PLUGIN2(DDDAnalysisToolboxPlugin, DDDAnalysisToolboxPlugin)

namespace Detail
{
  const QString DDDAnalysisToolboxPluginFile("DDDAnalysisToolboxPlugin");
  const QString DDDAnalysisToolboxPluginDisplayName("DDDAnalysisToolbox");
  const QString DDDAnalysisToolboxPluginBaseName("DDDAnalysisToolboxPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DDDAnalysisToolboxPlugin::DDDAnalysisToolboxPlugin() :
m_Version(DREAM3DLib::Version::Package()),
m_CompatibilityVersion(DREAM3DLib::Version::Package()),
m_Vendor(DREAM3D::BlueQuartz::VendorName),
m_Group(""),
m_URL(DREAM3D::BlueQuartz::URL),
m_Location(""),
m_Platforms(QList<QString>()),
m_Copyright(""),
m_Dependencies(QList<QString>())
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DDDAnalysisToolboxPlugin::~DDDAnalysisToolboxPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getPluginName()
{
  return (Detail::DDDAnalysisToolboxPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getVersion()
{
  return m_Version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getCompatibilityVersion()
{
  return m_CompatibilityVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getVendor()
{
  return m_Vendor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getURL()
{
  return m_URL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getLocation()
{
  return m_Location;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> DDDAnalysisToolboxPlugin::getPlatforms()
{
  return m_Platforms;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getDescription()
{
  QFile licenseFile(":/DDDAnalysisToolbox/DDDAnalysisToolboxDescription.txt");
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
QString DDDAnalysisToolboxPlugin::getCopyright()
{
  return m_Copyright;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DDDAnalysisToolboxPlugin::getLicense()
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
QList<QString> DDDAnalysisToolboxPlugin::getDependencies()
{
  return m_Dependencies;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, QString> DDDAnalysisToolboxPlugin::getThirdPartyLicenses()
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
void DDDAnalysisToolboxPlugin::writeSettings(QSettings& prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DDDAnalysisToolboxPlugin::readSettings(QSettings& prefs)
{

}

#include "DDDAnalysisToolboxFilters/RegisterKnownFilters.cpp"

#include "DDDAnalysisToolbox/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"

