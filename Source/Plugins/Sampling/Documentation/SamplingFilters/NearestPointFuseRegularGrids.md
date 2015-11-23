Fuse Regular Grids (Nearest Points) {#nearestpointfuseregulargrids}
=============

## Group (Subgroup) ##
Sampling (Resolution)

## Description ##
This **Filter** fuses two **Image Geometry** data sets together. The grid of **Cells** in the *Reference* **Data Container** is overlaid on the grid of **Cells** in the *Sampling* **Data Container**.  Each **Cell** in the *Reference* **Data Container** is associated with the nearest point in the *Sampling* **Data Container** (i.e., no *interpolation* is performed).  All the attributes of the **Cell** in the *Sampling* **Data Container** are then assigned to the **Cell** in the *Reference* **Data Container**.  Additional to the **Cell** attributes being copied, all **Feature and Ensemble Attribute Matrices** from the *Sampling* **Data Container** are copied to the *Reference* **Data Container**.

*Note:* The *Sampling* **Data Container** remains identical after this **Filter**, but the *Reference* **Data Container**, while "geometrically identical", gains all the attribute arrays from the *Sampling* **Data Container**.

## Parameters ##
None

## Required Geometry ##
Image

## Required Objects ##
None

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Matrix** | CellData | Cell | N/A | *Reference* **Cell** data to use for fusion |
| **Attribute Matrix** | CellData | Cell | N/A | *Sampling* **Cell** data to use for fusion |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


