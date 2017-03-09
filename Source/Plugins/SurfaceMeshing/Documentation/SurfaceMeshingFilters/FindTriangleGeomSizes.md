Find Feature Volumes from Triangle Geometry {#findtrianglegeomsizes}
=============

## Group (Subgroup) ##
Statistics (Morphological)

## Description ##
This **Filter** computes the enclosed volume of each **Feature** in a **Triangle Geometry**.  The result is the volume of each surface meshed **Feature**, or alternatively the volume of each unique polyhedron defined by the given _Face Labels_ array.  The volume of any generic polyhedron can be computed using the following algorithm:

1. Triangulate each face of the polyhedron (in this case, each face is already a triangle within the **Triangle Geometry**)
2. For each triangular face, ensure the normals are all consistent (this **Filter** uses the convetion where normals point inwards; note that the actual winding of the **Triangle Geometry** is not modified)
3. For each triangular face, create a tetrahedron where the fourth vertex is the origin
4. Compute the signed volume of each tetrahedron
5. Sum the signed tetrahedra volumes to obtain the volume of the enclosing polyhedron

This computation is _not_ the same as the [Find Feature Sizes](@ref findsizes) for **Triangle Geometries**, which computes the sum of the unit element sizes for a set of **Features** (thus, the [Find Feature Sizes](@ref findsizes) would compute the _area_ of **Features** in a **Triangle Geometry**, whereas this **Filter** is specialized to compute the enclosed volumes of **Features** in a surface mesh).

## Parameters ##
None

## Required Geometry ##
Triangle

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Face Attribute Array** | FaceLabels | int32_t | (2) | Specifies which **Features** are on either side of each **Face** |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Attribute Matrix** | FaceFeatureData | Face Feature | N/A | Created **Feature Attribute Matrix** name |
| **Feature Attribute Array** | Volumes |  float | (1) | Volume of the enclosed **Feature** |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users