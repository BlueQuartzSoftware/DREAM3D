Create Attribute Matrix {#createattributematrix}
=============

## Group (Subgroup) ##
Core (Generation)


## Description ##

This **Filter** creates a new **Attribute Matrix** inside of an existing **Data Container**.

## Parameters ##
| Name | Type | Description |
|------|------|-------------|
| Attribute Matrix Type | Enumeration | The type of **Attribute Matrix** to create. See table below for possible options |
| Tuple Dimensions | Any | An array that contains the size of each tuple dimension |

### Attribute Matrix Types ###

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


### Example Usage ###

If you wanted to create an **Attribute Matrix** to represent a 3D volume where the dimensions of the 3 orthogonal axesare X=3, y=4 and Z=5, then the _Tuple Dimensions_ would have a value of (3, 4, 5) and the _Attribute Matrix Type_ would be "3" (**Cell**).


## Required Geometry ##

Not Applicable

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container**  | None | N/A | N/A | **Data Container** for the created **Attribute Matrix**  |

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Matrix**  | None | Any | N/A | Created **Attribute Matrix** name  |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)
