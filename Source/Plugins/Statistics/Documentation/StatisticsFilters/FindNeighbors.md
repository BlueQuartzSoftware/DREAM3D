Find Feature Neighbors {#findneighbors}
=============

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter determines, for each **Feature**, the number of other **Features** that are in contact with it.  The algorithm for determining the number of "contiguous" neighbors of each **Feature** is as follows:

1. Identify the **Feature** to which a **Cell** belongs
2. Identify the **Features** to which each of the current **Cell**'s six (6) face-face neighboring **Cells** (front, back, left, right, up, down) belong
3. If a neighboring **Cell** belongs to a different **Feature** than the current **Cell**, then that **Feature** (owner of the neighboring **Cell**) is added to the list of contiguous neighbors of the **Feature** that owns the current **Cell**.
4. Repeat 1-3 for all **Cells**

While performing the above steps, the number of neighboring **Cells** with a different **Feature** owner than a given **Cell** is stored, which identifies whether a **Cell** lies on the surface/edge/corner of a **Feature** (i.e. the **Feature** boundary).
Additionally, the surface area shared between each set of contiguous **Features** is calculated by tracking the number of times two neighboring **Cells** correspond to a contiguous **Feature** pair.0
The Filter also notes which **Features** touch the outer surface of the sample (this is obtained for "free" while performing the above algorithm). The Filter gives the user the option whether or not they want to store this additional information.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Store Boundary Cells Array | Boolean | Specifies whether to store the boundary **Cells** array |
| Store Surface Features Array | Boolean | Specifies whether to store the surface **Features** array |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |
| Feature | Phases | Int | (1) | Specifies the phase of the **Feature** - Values will begin at 1 as there is no phase 0, which is used temporarily in some filters for bad data|

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | SurfaceCells | Int(8) | (1) | The number of neighboring **Cells** of a given **Cell** that belong to a different **Feature** than itself. Values will range from *0* to *6* (only created if *Store Boundary Cells Array* is set to *true*) |
| Feature | NumNeighbors | Int | (1) | Number of contiguous neighboring **Features** for a given **Feature** |
| Feature | NeighborLists | List of Ints | (1) | List of the contiguous neighboring **Features** for a given **Feature** |
| Feature | SharedSurfaceAreaLists | List of Floats | (1) | List of the shared surface area for each of the contiguous neighboring **Features** for a given **Feature** |
| Feature | SurfaceFeatures | Boolean | (1) | Flag equal to 1 if the **Feature** touches an outer surface of the sample and equal to 0 if it does not. (only created if *Store Surface Features Array* is set to *true*) |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


