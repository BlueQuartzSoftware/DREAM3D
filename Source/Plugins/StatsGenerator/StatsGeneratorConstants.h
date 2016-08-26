/*
 * Your License should go here
 */
#ifndef _StatsGeneratorConstants_H_
#define _StatsGeneratorConstants_H_

#include <QtCore/QString>

/**
* @brief This namespace is used to define some Constants for the plugin itself.
*/
namespace StatsGeneratorConstants
{
  const QString StatsGeneratorPluginFile("StatsGeneratorPlugin");
  const QString StatsGeneratorPluginDisplayName("StatsGeneratorPlugin");
  const QString StatsGeneratorBaseName("StatsGeneratorPlugin");

  namespace FilterGroups
  {
    const QString StatsGeneratorFilters("StatsGenerator");
  }

  static const float k_Mu = 1.0f;
  static const float k_Sigma = 0.1f;
}

/**
* @brief Use this namespace to define any custom GUI widgets that collect FilterParameters
* for a filter. Do NOT define general reusable widgets here.
*/
namespace FilterParameterWidgetType
{
/* const QString SomeCustomWidget("SomeCustomWidget"); */
}

namespace BlueQuartz
{
  const QString VendorName("BlueQuartz Software, LLC");
  const QString URL("http://www.bluequartz.net");
  const QString Copyright("(C) 2016 BlueQuartz Software, LLC");
}

#if defined(Q_OS_WIN)

#define SG_FONT_SIZE 10

#elif defined (Q_OS_LINUX)

#define SG_FONT_SIZE 10

#elif defined (Q_OS_OSX)

#define SG_FONT_SIZE 14

#else

#define SG_FONT_SIZE 12

#endif


#endif
