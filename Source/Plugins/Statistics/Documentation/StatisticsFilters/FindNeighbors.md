Find Feature Neighbors {#findneighbors}
=============

## Group (Subgroup) ##
Statistics (Morphological)

## Description ##
This **Filter** determines, for each **Feature**, the number of other **Features** that are in contact with it.  The algorithm for determining the number of "contiguous" neighbors of each **Feature** is as follows:

1. Identify the **Feature** to which a **Cell** belongs
2. Identify the **Features** to which each of the current **Cell**'s six (6) face-face neighboring **Cells** (front, back, left, right, up, down) belong
3. If a neighboring **Cell** belongs to a different **Feature** than the current **Cell**, then that **Feature** (owner of the neighboring **Cell**) is added to the list of contiguous neighbors of the **Feature** that owns the current **Cell**
4. Repeat 1-3 for all **Cells**

While performing the above steps, the number of neighboring **Cells** with a different **Feature** owner than a given **Cell** is stored, which identifies whether a **Cell** lies on the surface/edge/corner of a **Feature** (i.e. the **Feature** boundary). Additionally, the surface area shared between each set of contiguous **Features** is calculated by tracking the number of times two neighboring **Cells** correspond to a contiguous **Feature** pair. The **Filter** also notes which **Features** touch the outer surface of the sample (this is obtained for "free" while performing the above algorithm). The **Filter** gives the user the option whether or not they want to store this additional information.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Store Boundary Cells Array | bool | Whether to store the boundary **Cells** array |
| Store Surface Features Array | bool | Whether to store the surface **Features** array |

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |
| **Feature Attribute Matrix** | CellFeatureData | Cell Feature | N/A | **Feature Attribute Matrix** of the selected _Feature Ids_ |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | BoundaryCells | int8_t | (1) | The number of neighboring **Cells** of a given **Cell** that belong to a different **Feature** than itself. Values will range from *0* to *6*. Only created if *Store Boundary Cells Array* is checked|
| **Feature Attribute Array** | NumNeighbors | int32_t | (1) | Number of contiguous neighboring **Features** for a given **Feature** |
| **Feature Attribute Array**  | NeighborList | List of int32_t | (1) | List of the contiguous neighboring **Features** for a given **Feature** |
| **Feature Attribute Array**  | SharedSurfaceAreaLists | List of float | (1) | List of the shared surface area for each of the contiguous neighboring **Features** for a given **Feature** |
| **Feature Attribute Array**  | SurfaceFeatures | bool | (1) | Flag equal to 1 if the **Feature** touches an outer surface of the sample and equal to 0 if it does not. Only created if *Store Surface Features Array* is checked |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


