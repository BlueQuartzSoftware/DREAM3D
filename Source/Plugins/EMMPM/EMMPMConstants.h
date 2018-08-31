/*
 * Your License should go here
 */
#pragma once

#include <QtCore/QString>

/**
* @brief This namespace is used to define some Constants for the plugin itself.
*/
namespace EMMPMConstants
{
  const QString EMMPMPluginFile("EMMPMPlugin");
  const QString EMMPMPluginDisplayName("EMMPM");
  const QString EMMPMBaseName("EMMPM");

  namespace BlueQuartz
  {
  const QString VendorName("BlueQuartz Software, LLC");
  const QString URL("http://www.bluequartz.net");
  const QString Copyright("(C) 2018 BlueQuartz Software, LLC");
  } // namespace BlueQuartz
}

/**
* @brief Use this namespace to define any custom GUI widgets that collect FilterParameters
* for a filter. Do NOT define general reusable widgets here.
*/
namespace FilterParameterWidgetType
{

  /*  const QString SomeCustomWidget("SomeCustomWidget"); */

}


