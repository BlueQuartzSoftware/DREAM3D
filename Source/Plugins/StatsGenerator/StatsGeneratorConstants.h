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
}

/**
* @brief Use this namespace to define any custom GUI widgets that collect FilterParameters
* for a filter. Do NOT define general reusable widgets here.
*/
namespace FilterParameterWidgetType
{
/* const QString SomeCustomWidget("SomeCustomWidget"); */
}
#endif
