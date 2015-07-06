Link Feature Attribute Matrix to Element Attribute Array {#linkfeaturemaptoelementarray}
=============

## Group (Subgroup) ##
Core (Memory/Management)

## Description ##
An **Element** array is used as the index into the **Feature** map for each **Element**.  This **Filter** allows the user to specify which **Element** array is that index.  The **Filter** clears the current **Feature Attribute Matrix**, creates a new **Feature** map and resizes it the largest value found in the user selected **Element** array.  The **Filter** also creates an _Active_ array in the new **Feature Attribute Matrix**, which identifies whether each **Feature** *exists* by checking if there is *at least* one **Element** that belongs to each **Feature**. 

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Element Attribute Array**  | None | int32_t | (1) | **Element Attribute Array** to use as the **Feature** index map |

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Matrix**  | None | **Feature** | N/A | **Feature Attribute Matrix** to reset |
| **Feature Attribute Array**  | Active | bool | (1) | Active **Attribute Array** name |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


