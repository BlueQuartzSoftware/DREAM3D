Scale Volume {#scalevolume}
=============

## Group (Subgroup) ##
Core (Spatial)

## Description ##
This **Filter** scales a volume by the factors entered by the user. For example, if a volume has a range of 10 to 30 in the X direction and the user enters a _Scale Factor_ for X of 3, then the new range will be 30 to 90. Note that the origin will **note** change with this **Filter**. 

## Parameters ##
| Name    | Type      |  Description |
|---------|-----------|--------|
| Scaling Factor | float (3x) | Applied to (dx, dy, dz) for an **Image Geometry** and (node 1, node 2, node 3) for a surface **Geometry** |
| Apply to Image Geometry | bool | Whether the new scaling should be applied to an **Image Geoemtry** |
| Apply to Surface Geometry | bool | Whether the new scaling should be applied to a surface **Geometry |

## Required Geometry ##
Image and/or 2D surface (Triangle/Quad)

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | None | N/A | N/A | **Data Container** with an **Image Geometry** to scale. Only required if _Apply to Image Geometry_ is checked |
| **Data Container** | None | N/A | N/A | **Data Container** with a surface **Geometry** to scale. Only required if _Apply to Surface Geometry_ is checked |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


