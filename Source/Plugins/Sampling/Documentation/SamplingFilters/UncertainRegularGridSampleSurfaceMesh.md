Sample Triangle Geometry on Uncertain Regular Grid {#uncertainregulargridsamplesurfacemesh}
=============

## Group (Subgroup) ##
Sampling (Resolution)

## Description ##
This **Filter** "samples" a triangulated surface mesh on a rectilinear grid, but with "uncertainty" in the absolute position of the **Cells**.  The "uncertainty" is meant to simulate the possible positioning error in a sampling probe.  The user can specify the number of **Cells** along the X, Y, and Z directions in addition to the resolution in each direction and origin to define a rectilinear grid.  The sampling, with "uncertainty", is then performed by the following steps:

1. Determine the bounding box and **Triangle** list of each **Feature** by scanning all **Triangles** and noting the **Features** on either side of the **Triangle**
2. For each **Cell** in the rectilinear grid, perturb the location of the **Cell** by generating a three random numbers between [-1, 1] and multiplying them by the three uncertainty values (one for each direction)
3. For each perturbed **Cell** in the rectilinear grid, determine which bounding box(es) they fall in (*Note:* the bounding box of multiple **Features** can overlap)
4. For each bounding box a **Cell** falls in, check against that **Feature's** **Triangle** list to determine if the **Cell** falls within that n-sided polyhedra. (*Note:* if the surface mesh is conformal, then each **Cell** will only belong to one **Feature**, but if not, the last **Feature** the **Cell** is found to fall inside of will *own* the **Cell**)
5. Assign the **Feature** number that the **Cell** falls within to the *Feature Ids* array in the new rectilinear grid geometry

**Note that the unperturbed grid is where the _Feature Ids_ actually live, but the perturbed locations are where the Cells are sampled from.  Essentially, the _Feature Ids_ are stored where the user _thinks_ the sampling took place, not where it actually took place!**

## Parameters ##
| Name | Type | Description |
|------|------|------|
| X Points (Column) | int32_t | Number of **Cells** along X axis |
| Y Points (Row) | int32_t | Number of **Cells** along Y axis |
| Z Points (Plane) | int32_t | Number of **Cells** along Z axis |
| Resolution | float (3x) | The resolution values (dx, dy, dz) |
| Origin | float (3x) | The origin of the sampling volume |
| Uncertainty | float (3x) | Vector of uncertainty values associated with X, Y and Z positions of **Cells** |

## Required Geometry ##
Triangle

## Required Objects ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Face Attribute Array** | FaceLabels | Int | (2) | Specifies which **Features** are on either side of each **Face**. |

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | ImageDataContainer | N/A | N/A | Created **Data Container** name with an **Image Geometry** |
| **Attribute Matrix** | CellData | Cell | N/A | Created **Cell Attribute Matrix** name |
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)

