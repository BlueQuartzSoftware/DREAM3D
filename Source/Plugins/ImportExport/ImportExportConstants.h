/*
 * Your License should go here
 */
#ifndef _importexportconstants_h_
#define _importexportconstants_h_

#include <QtCore/QString>

/**
 * @brief This namespace is used to define some Constants for the plugin itself.
 */
namespace ImportExportConstants
{
const QString ImportExportPluginFile("ImportExportPlugin");
const QString ImportExportPluginDisplayName("ImportExport");
const QString ImportExportBaseName("ImportExport");

namespace BlueQuartz
{
const QString VendorName("BlueQuartz Software, LLC");
const QString URL("http://www.bluequartz.net");
const QString Copyright("(C) 2018 BlueQuartz Software, LLC");
} // namespace BlueQuartz

namespace FilterGroups
{
const QString ImportExportFilters("ImportExport");
}
} // namespace ImportExportConstants

/**
 * @brief Use this namespace to define any custom GUI widgets that collect FilterParameters
 * for a filter. Do NOT define general reusable widgets here.
 */
namespace FilterParameterWidgetType
{
const QString ProcessMonitoringWidget("ProcessMonitoringWidget");
}
#endif
