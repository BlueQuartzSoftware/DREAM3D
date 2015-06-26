Create Attribute Matrix {#createattributematrix}
=============

## Group (Subgroup) ##

Core Filters (Generation)


## Description ##

This Filter creates a new **Attribute Matrix** inside of an existing **Data Container**.

## Parameters ##
| Name | Type | Description |
|------|------|-------------|
| Selected Data Container | String | |
| Created Attribute Matrix Name | String |  |
| Attribute Matrix Type | Int | The type of Attribute Matrix to create. See table below. |
| Tuple Dimensions | (n) | An array that contains the size of each dimension. |

### Accepted Values for Attribute Matrix Type ###

| Name | Value |
|------|-------|
| Vertex | 0 |
| Edge | 1 |
| Face | 2 |
| Cell | 3 |
| VertexFeature | 4 |
| EdgeFeature | 5 |
| FaceFeature | 6 |
| CellFeature | 7 |
| VertexEnsemble | 8 |
| EdgeEnsemble | 9 |
| FaceEnsemble | 10 |
| CellEnsemble | 11 |
| MetaData | 12 |
| Generic | 13 |
| Unknown | 999 |


## Example Usage ##

If you wanted to create an Attribute Matrix to represent a 3D volume where the dimensions of the 3 orthogonal axis
are X=3, y=4 and z=5, then the _Tuple Dimensions_ would have a value of (3, 4, 5) and the _Attribute Matrix Type_
would be "3".


## Required Geometry ##

Not Applicable

## Required Arrays ##

Not Applicable

## Created Arrays ##

Not Applicable

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users
