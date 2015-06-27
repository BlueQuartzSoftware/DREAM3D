Move Data {#movedata}
=============

## Group (Subgroup) ##
Core (Memory/Management)


## Description ##
This **Filter** allows the user to move an **Attribute Array** from one **Attribute Matrix** to another compatible **Attribute Matrix** or to move an **Attribute Matrix** from one **Data Container** to another **Data Container**. **Attribute Matrices** are compatible if the _number of tuples_ are equal, **not** the actual _tuple dimensions_. 

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Object to Move | Enumeration | Whether to move an **Attribute Matrix** or an **Attribute Array** |

## Required Geometry ##
Not Applicable

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Matrix**  | None         | Any | N/A | **Attribute Matrix** to move |
| **Data Container**  | None         | N/A | N/A | **Data Container** in which to move the selected **Attribute Matrix** |
| Any **Attribute Array**  | None         | Any | Any | **Attribute Array** to move |
| **Attribute Matrix**  | None         | Any | N/A | **Attribute Matrix** in which to move the selected **Attribute Array** |


## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


