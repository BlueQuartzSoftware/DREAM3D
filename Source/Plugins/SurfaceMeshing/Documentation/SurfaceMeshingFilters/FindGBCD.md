Find GBCD {#findgbcd}
=============

## Group (Subgroup) ##
Statistics (Crystallographic)

## Description ##
This Filter computes the 5D grain boundary character distribution (GBCD) for a **Triangle Geometry**, which is the relative area of grain boundary for a given misorientation and normal. The GBCD can be visualized by using either the [Write GBCD Pole Figure (GMT)](@ref visualizegbcdgmt) or the [Write GBCD Pole Figure (VTK)](@ref visualizegbcdpolefigure) **Filters**.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| GBCD Resolution (Degrees) | Int | The resolution in degrees for the GBCD calculation |

## Required Geometry ##
Image + Triangle

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Face | FaceLabels | Int | (2) | Specifies which **Features** are on either side of each **Face** |
| Face | FaceNormals | Double | (3) | Specifies the normal of each **Face** |
| Face | FaceAreas | Double | (1) | Specifies the area of each **Face** |
| Feature | EulerAngles | Float | (3) | Three angles defining the orientation of the **Feature** in Bunge convention (Z-X-Z) |
| Feature | Phases | Int | (1) | Specifies to which phase each **Feature** belongs |
| Ensemble | CrystalStructures | Int | (1) | Enumeration representing the crystal structure for each phase |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Ensemble | GBCD | Double | (6) | 5 parameter GBCD data. The 6<sup>th</sup> component is used internally to track the northern vs. southern hemisphere of the Lambert sphere |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


