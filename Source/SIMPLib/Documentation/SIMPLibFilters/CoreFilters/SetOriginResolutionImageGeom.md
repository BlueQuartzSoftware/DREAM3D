Set Origin & Resolution (Image)  {#setoriginresolutionimagegeom}
=============

## Group (Subgroup) ##
Core (Spatial)

## Description ##
This **Filter** changes the origin and/or the resolution of an **Image Geometry**. For example, if the current origin is at (0, 0, 0) and the user would like the origin to be (10, 4, 8), then the user should enter the following input values:

+ X Origin: 10
+ Y Origin: 4
+ Z Origin: 8

## Parameters ##

| Name    | Type      |  Description |
|---------|-----------|--------|
| Origin | float (3x) | New origin for the **Image Geometry** |
| Resolution | float (3x) | New resolution for the **Image Geometry** |
| Change Origin | bool | Whether a new origin should be applied to the **Image Geometry** |
| Change Resolution | bool | Whether a new resolution should be applied to the **Image Geometry** |

## Required Geometry ##
Image

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | None | N/A | N/A | **Data Container** with an **Image Geometry** to modify |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


