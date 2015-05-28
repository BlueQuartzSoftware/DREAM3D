Find Feature Shapes {#findshapes}
=============

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter calculates the second-order moments of each **Feature** in order to determine the *principal axis lengths, pricipal axis directions, aspect ratios and moment invariant Omega3s*.  The *principal axis lengths* are those of a "best-fit" ellipsoid.  The algorithm for determining the moments and these values is as follows:

1. For each **Cell**, determine the x, y and z distance to the centroid of the **Feature** that owns the **Cell**.
2. For each **Cell**, calculate Ixx, Iyy, Izz, Ixy, Ixz and Iyz using the x, y and z distances determined in step 1.
3. Sum the individual Ixx, Iyy, Izz, Ixy, Ixz and Iyz values for all **Cells** belonging to the same **Feature**.
4. Find the *eigenvalues* and *eigenvectors* of the *3x3* symmetric matrix defined by the *6* values calculated in step 3 for each **Feature**.
5. Use the relationship of *principal moments* to the *principal axis lengths* for an ellipsoid, which can be found in [4], to determine the *Axis Lengths*
6. Calculate the *Aspect Ratios* from the *Axis Lengths* found in step 5.
7. Determine the Euler angles required to represent the *principal axis directions* in the *sample reference frame* and store them as the **Feature**'s *Axis Euler Angles*.
8. Calculate the moment variant Omega3 as definied in [2] and is discussed further in [1] and [3].

------------
## References ## 

1. Representation and Reconstruction of Three-dimensional
Microstructures in Ni-based Superalloys, AFOSR FA9550-07-1-0179 Final Report, 20 Dec 2010.
2. On the use of moment invariants for the automated classifcation of 3-D particle shapes, J.
MacSleyne, J.P. Simmons and M. De Graef, Modeling and Simulations in Materials Science
and Engineering, 16, 045008 (2008). 
3.   For a full mathematical description of moment invariants and application to shapes of n-dimensional objects, see:
n-Dimensional Moment Invariants and
Conceptual Mathematical Theory of
Recognition n-Dimensional Solids,
Alexander G. Mamistvalov, IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE INTELLIGENCE, VOL. 20, NO. 8, AUGUST 1998, p. 819-831.
4. M. Groeber, M. Uchic, D. Dimiduk, and S. Ghosh. “A Framework for Automated Analysis and Simulation of 3D Polycrystalline Microstructures, Part 1: Statistical Characterization” Acta Materialia, 56 (2008), 1257-1273. 
_


## Parameters ##
None

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |
| Feature | Centroids | Float | (3) | X, Y, Z coordinates of **Feature** center of mass |
| Feature | Volumes |  Float | (1) | Volume (in units^3) of the **Feature**. This value may be "redundant" from the NumCells value if all **Cells** are the same size and is calculable from the EquivalentDiameters value. | 

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | AspectRatios | Float | (2) | Ratio of axis lengths (b/a and c/a) for best-fit ellipsoid to **Feature** |
| Feature | AxisEulerAngles | Float | (3) | Euler angles (in radians) necessary to rotate the sample reference frame to the reference frame of the **Feature**, where the prinicpal axes of the best-fit ellipsoid are (x,y,z). |
| Feature | AxisLengths | Float | (3) | Axis lengths (a, b, c) for best-fit ellipsoid to **Feature** |
| Feature | Omega3s | Float | (1) | 3rd invariant of the second-order moment matrix for the **Feature**, does not assume a shape type (ie ellipsoid) |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


