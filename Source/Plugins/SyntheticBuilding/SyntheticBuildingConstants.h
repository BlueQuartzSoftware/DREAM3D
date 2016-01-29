#ifndef _SyntheticBuildingConstants_H_
#define _SyntheticBuildingConstants_H_

#include <QtCore/QString>

namespace SyntheticBuildingConstants
{
  const QString SyntheticBuildingPluginFile("SyntheticBuildingPlugin");
  const QString SyntheticBuildingPluginDisplayName("SyntheticBuilding");
  const QString SyntheticBuildingBaseName("SyntheticBuilding");
}

/**
 * @namespace FilterParameterWidgetType
 * @brief If you create custom Filter Parameter Widgets for your classes then those need to be defined here
 */
namespace FilterParameterWidgetType
{
  const QString InitializeSyntheticVolumeWidget("InitializeSyntheticVolumeWidget");
}

namespace BlueQuartz
{
  const QString VendorName("BlueQuartz Software, LLC");
  const QString URL("http://www.bluequartz.net");
  const QString Copyright("(C) 2016 BlueQuartz Software, LLC");
}

#endif
