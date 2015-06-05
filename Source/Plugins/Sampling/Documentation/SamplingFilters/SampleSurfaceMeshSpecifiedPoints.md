SampleSurfaceMeshSpecifiedPoints {#samplesurfacemeshspecifiedpoints}
=====

## Group (Subgroup) ##
Sampling (Resolution)

## Description ##
This Filter 'samples' a triangulated surface mesh with a specified list of **Vertices** (or points) read from a file.  The sampling is performed by the following steps:

1. Determine the bounding box and **Triangle** list of each **Feature** by scanning all **Triangles** and noting the **Features** on either side of the **Triangle**.
2. For each **Vertex** read from the file, determine which bounding box(es) they fall in (*Note:* the bounding box of multiple **Features** can overlap).
3. For each bounding box a **Vertex** falls in, check against that **Feature's** **Triangle** list to determine if the **Vertex** falls within that n-sided polyhedra (*Note:* if the surface mesh is conformal, then each **Vertex** will only belong to one **Feature**, but if not, the last **Feature** the **Vertex** is found to fall inside of will *own* the **Vertex**).
4. Assign the **Feature** number that the **Vertex** falls within to the *Feature Ids* array in the new **Vertex** geometry.

The Filter will write out a file with the list of *FeatureIds* for the **Vertices**.  The Filter also creates a new *DataContainer* to hold the **Vertex** geometry and the *FeatureIds* that live on them.

The format of the *Specified Points File* is as follows:

~~~~~~~~~~~~~~~
Number of Points
X, Y, Z
X, Y, Z
X, Y, Z
.
.
.
X, Y, Z
~~~~~~~~~~~~~~~


## Parameters ##
| Name | Type | Description |
|------|------|------|
| Specified Points File Path | File Path | Location of file listing points to sample at |
| Sampled Values File Path | File Path | Location where output information will be written |

## Required Geometry ##
Triangle

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Triangle | SurfaceMeshFaceLabels | Int | (2) | Specifies which **Features** are on either side of each **Triangle**. |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


