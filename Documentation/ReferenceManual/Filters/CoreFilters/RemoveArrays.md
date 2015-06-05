Delete Arrays {#removearrays}
=============

## Group (Subgroup) ##
Core Filters (Memory/Management)

## Description ##
This filter allows the user to remove specified arrays from the pipeline by deleting them from memory. This can be helpful if the user has operations that need as much memory as possible and there are extra arrays that are not needed residing in memory. For example when creating a surface mesh one typically only needs an array that defines the structure of the volume and then afterwards can be safely deleted.

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Any  | None         | Any | Any | Array to be removed |

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


