#pragma once

#include <QtCore/QString>

namespace SyntheticBuildingConstants
{
  const QString SyntheticBuildingPluginFile("SyntheticBuildingPlugin");
  const QString SyntheticBuildingPluginDisplayName("SyntheticBuilding");
  const QString SyntheticBuildingBaseName("SyntheticBuilding");

  namespace BlueQuartz
  {
  const QString VendorName("BlueQuartz Software, LLC");
  const QString URL("http://www.bluequartz.net");
  const QString Copyright("(C) 2018 BlueQuartz Software, LLC");
  } // namespace BlueQuartz

  namespace FilterGroups
  {
  const QString StatsGeneratorFilters("StatsGenerator");
  }

  static const float k_Mu = 1.0f;
  static const float k_Sigma = 0.1f;
}

/**
 * @namespace FilterParameterWidgetType
 * @brief If you create custom Filter Parameter Widgets for your classes then those need to be defined here
 */
namespace FilterParameterWidgetType
{
  const QString InitializeSyntheticVolumeWidget("InitializeSyntheticVolumeWidget");
}


#if defined(Q_OS_WIN)

#define SG_FONT_SIZE 10

#elif defined(Q_OS_LINUX)

#define SG_FONT_SIZE 10

#elif defined(Q_OS_OSX)

#define SG_FONT_SIZE 14

#else

#define SG_FONT_SIZE 12

#endif

