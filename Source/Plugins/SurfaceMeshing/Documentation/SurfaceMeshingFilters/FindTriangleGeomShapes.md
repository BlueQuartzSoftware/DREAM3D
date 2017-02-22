Find Feature Shapes from Triangle Geometry {#findtrianglegeomshapes}
=============

## Group (Subgroup) ##
Statistics (Morphological)

## Description ##
This **Filter** calculates the second-order moments of each enclosed **Feature** in a **Triangle Geometry**.  The second-order moments allow for the determination of the *principal axis lengths, pricipal axis directions, aspect ratios and moment invariant Omega3s*.  The *principal axis lengths* are those of a "best-fit" ellipsoid.  The algorithm for determining the moments and these values is as follows:

1. For each **Triangle** on the bounding surface of a **Feature**, construct a tetrahedron whose fourth vertex is the centroid of the **Feature**, ensuring normals are consistent (this **Filter** uses the convetion where normals point inwards; note that the actual winding of the **Triangle Geometry** is not modified)
2. Subdivide each constructed tetrahedron into 8 smaller tetrahedra
3. For each subdivided tetrahedron, compute the distance from that tetrahedron's centroid to the centroid of the parent **Feature**
4. For each subdivided tetrahedron, calculate Ixx, Iyy, Izz, Ixy, Ixz and Iyz using the x, y and z distances determined in step 1
5. Use the relationship of *principal moments* to the *principal axis lengths* for an ellipsoid, which can be found in [4], to determine the *Axis Lengths*
6. Calculate the *Aspect Ratios* from the *Axis Lengths* found in step 5.
7. Determine the Euler angles required to represent the *principal axis directions* in the *sample reference frame* and store them as the **Feature**'s *Axis Euler Angles*.
8. Calculate the moment invariant Omega3 as definied in [2] and is discussed further in [1] and [3] 

*Note:* Due to the method used to subdivide the tetrahedra, some sharp corners of shapes may not be properly represented, resulting in inaccurate Omega3 values.  This problem is especially apparent for perfect rectangular prisms, but any shape with clear sharp corners may be affected.  

## Parameters ##
None

## Required Geometry ##
Triangle

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Face Attribute Array** | FaceLabels | int32_t | (2) | Specifies which **Features** are on either side of each **Face** |
| **Feature Attribute Array**  | Centroids | float | (3) | Coordinates of the center of mass for a given enclosed **Feature** |
| **Feature Attribute Array** | Volumes |  float | (1) | Volume of the enclosed **Feature** |
| **Attribute Matrix** | FaceFeatureData | Face Feature | N/A | **Feature Attribute Matrix** of the selected _Face Labels |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | AspectRatios | float | (2) | Ratio of axis lengths (b/a and c/a) for best-fit ellipsoid to enclosed **Feature** |
| **Feature Attribute Array** | AxisEulerAngles | float | (3) | Euler angles (in radians) necessary to rotate the sample reference frame to the reference frame of the enclosed **Feature**, where the prinicpal axes of the best-fit ellipsoid are (X, Y, Z) |
| **Feature Attribute Array** | AxisLengths | float | (3) | Axis lengths (a, b, c) for best-fit ellipsoid to enclosed **Feature** |
| **Feature Attribute Array** | Omega3s | float | (1) | 3rd invariant of the second-order moment matrix for the enclosed **Feature**, does not assume a shape type (i.e., ellipsoid) |

## References ## 

[1] Representation and Reconstruction of Three-dimensional Microstructures in Ni-based Superalloys, AFOSR FA9550-07-1-0179 Final Report, 20 Dec 2010.

[2] On the use of moment invariants for the automated classifcation of 3-D particle shapes, J. MacSleyne, J.P. Simmons and M. De Graef, Modeling and Simulations in Materials Science and Engineering, 16, 045008 (2008).

[3] n-Dimensional Moment Invariants and Conceptual Mathematical Theory of Recognition n-Dimensional Solids, Alexander G. Mamistvalov, IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE INTELLIGENCE, VOL. 20, NO. 8, AUGUST 1998, p. 819-831.

[4] M. Groeber, M. Uchic, D. Dimiduk, and S. Ghosh. “A Framework for Automated Analysis and Simulation of 3D Polycrystalline Microstructures, Part 1: Statistical Characterization” Acta Materialia, 56 (2008), 1257-1273. 

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users