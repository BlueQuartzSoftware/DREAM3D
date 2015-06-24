Quick Surface Mesh {#quicksurfacemesh}
============

## Group (Subgroup) ##
Surface Meshing (Generation)

## Description ##
This **Filter** generates a **Triangle Geometry** from an **Image Geometry** that represents a surface mesh of the present **Features**. The algorithm proceeds by creating a pair of **Triangles** for each face of the **Cell** where the neighboring **Cell** have a different **Feature** Id value. The meshing operation is extremely quick but can result in a surface mesh that is very "stair stepped". The user is encouraged to use a [smoothing operation](@ref laplaciansmoothing) to reduce this "blockiness".

---------------

![Example Quick Mesh Output](QuickSurfaceMeshOutput.png)
@image latex QuickSurfaceMeshOutput.png "Example Quick Mesh Output" width=6in

---------------

## Parameters ##
| Name | Type | Description |
|------|------|-------------|
| Transfer Phase Id | Bool | Whether to transfer the **Ensemble** Ids from the **Image Geometry** into the new **Triangle Geometry** |

## Required Geometry ##
Image

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs |
| Cell | Phases | Int | (1) | Specifies to which **Ensemble** each **Cell** belongs. Only needed if _Transfer Phase Id_ is checked |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Face | FaceLabels | Int | (2) | Specifies which **Features** are on either side of each **Face** |
| Vertex| NodeTypes | Int | (1) | Specifies the type of node in the **Geometry** |
| Face | Phases | Int | (1) | Specifies to which **Ensemble** each **Face** belongs. Only needed if _Transfer Phase Id_ is checked |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


