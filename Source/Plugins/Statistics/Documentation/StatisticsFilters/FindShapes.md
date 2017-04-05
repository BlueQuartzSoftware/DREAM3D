Find Feature Shapes {#findshapes}
=============

## Group (Subgroup) ##
Statistics (Morphological)

## Description ##
This **Filter** calculates the second-order moments of each **Feature** in order to determine the *principal axis lengths, principal axis directions, aspect ratios and moment invariant Omega3s*.  The *principal axis lengths* are those of a "best-fit" ellipsoid.  The algorithm for determining the moments and these values is as follows:

1. For each **Cell**, determine the x, y and z distance to the centroid of the **Feature** that owns the **Cell**
2. For each **Cell**, calculate Ixx, Iyy, Izz, Ixy, Ixz and Iyz using the x, y and z distances determined in step 1.
3. Sum the individual Ixx, Iyy, Izz, Ixy, Ixz and Iyz values for all **Cells** belonging to the same **Feature**
4. Find the *eigenvalues* and *eigenvectors* of the *3x3* symmetric matrix defined by the *6* values calculated in step 3 for each **Feature**
5. Use the relationship of *principal moments* to the *principal axis lengths* for an ellipsoid, which can be found in [4], to determine the *Axis Lengths*
6. Calculate the *Aspect Ratios* from the *Axis Lengths* found in step 5.
7. Determine the Euler angles required to represent the *principal axis directions* in the *sample reference frame* and store them as the **Feature**'s *Axis Euler Angles*.
8. Calculate the moment variant Omega3 as definied in [2] and is discussed further in [1] and [3]

## Parameters ##
None

## Required Geometry ##
Image 

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |
| **Feature Attribute Array** | Centroids | float | (3) | X, Y, Z coordinates of **Feature** center of mass |
| **Attribute Matrix** | CellFeatureData | Cell Feature | N/A | **Feature Attribute Matrix** of the selected _Feature Ids_ |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | AspectRatios | float | (2) | Ratio of axis lengths (b/a and c/a) for best-fit ellipsoid to **Feature** |
| **Feature Attribute Array** | AxisEulerAngles | float | (3) | Euler angles (in radians) necessary to rotate the sample reference frame to the reference frame of the **Feature**, where the prinicpal axes of the best-fit ellipsoid are (X, Y, Z) |
| **Feature Attribute Array** | AxisLengths | float | (3) | Axis lengths (a, b, c) for best-fit ellipsoid to **Feature** |
| **Feature Attribute Array** | Omega3s | float | (1) | 3rd invariant of the second-order moment matrix for the **Feature**, does not assume a shape type (i.e., ellipsoid) |
| **Feature Attribute Array** | Volumes | float | (1) | The volume of each **Feature** |

## References ## 

[1] Representation and Reconstruction of Three-dimensional Microstructures in Ni-based Superalloys, AFOSR FA9550-07-1-0179 Final Report, 20 Dec 2010.

[2] On the use of moment invariants for the automated classifcation of 3-D particle shapes, J. MacSleyne, J.P. Simmons and M. De Graef, Modeling and Simulations in Materials Science and Engineering, 16, 045008 (2008).

[3] n-Dimensional Moment Invariants and Conceptual Mathematical Theory of Recognition n-Dimensional Solids, Alexander G. Mamistvalov, IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE INTELLIGENCE, VOL. 20, NO. 8, AUGUST 1998, p. 819-831.

[4] M. Groeber, M. Uchic, D. Dimiduk, and S. Ghosh. “A Framework for Automated Analysis and Simulation of 3D Polycrystalline Microstructures, Part 1: Statistical Characterization” Acta Materialia, 56 (2008), 1257-1273. 

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


