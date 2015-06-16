/*
 * Your License should go here
 */
#ifndef _HEDMAnalysisConstants_H_
#define _HEDMAnalysisConstants_H_

#include <QtCore/QString>

/**
* @brief This namespace is used to define some Constants for the plugin itself.
*/
namespace HEDMAnalysisConstants
{
  const QString HEDMAnalysisPluginFile("HEDMAnalysisPlugin");
  const QString HEDMAnalysisPluginDisplayName("HEDMAnalysisPlugin");
  const QString HEDMAnalysisBaseName("HEDMAnalysisPlugin");

  namespace FilterGroups
  {
    const QString HEDMAnalysisFilters("HEDM Analysis");
  }
}

/**
* @brief Use this namespace to define any custom GUI widgets that collect FilterParameters
* for a filter. Do NOT define general reusable widgets here.
*/
namespace FilterParameterWidgetType
{

/*  const QString SomeCustomWidget("SomeCustomWidget"); */

}
#endif
