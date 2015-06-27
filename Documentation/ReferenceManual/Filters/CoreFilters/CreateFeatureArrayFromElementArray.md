Create Feature Array From Cell Array {#createfeaturearrayfromelementarray}
=============

## Group (Subgroup) ##

Core (Memory/Management)

## Description ##

This **Filter** copies all the associated **Element** data of a selected **Element Attribute Array** to the **Feature** to which the **Elements** belong. The value stored for each **Feature** will be the value of the _last element copied_. 

## Parameters ##
None

## Required Geometry ##
Not Applicable


## Required Arrays ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element **Attribute Array** | None | Any | Any | **Element** data to copy to **Feature** data |
| Element **Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Element** belongs |


## Created Arrays ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Matrix** | None | **Feature**  | N/A | **Feature Attribute Matrix** in which to place the copied data |
| Feature **Attribute Array** | None | Any  | Any | Copied **Attribute Array** name |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


