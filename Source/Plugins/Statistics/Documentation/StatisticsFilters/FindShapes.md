Find Feature Shapes {#findshapes}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter calculates the second-order moments of each **Feature** in order to determine the *principal axis lengths, pricipal axis directions, aspect ratios and moment invariant Omega3*.  The *principal axis lengths* are those of a "best-fit" ellipsoid.  The algorithm for determining the moments and these values is as follows:

1. Obtain the centroid for each **Feature**.
2. For each **Cell**, determine the x, y and z distance to the centroid of the **Feature** that owns the **Cell**.
3. For each **Cell**, calculate Ixx, Iyy, Izz, Ixy, Ixz and Iyz using the x, y and z distances determined in step 2.
4. Sum the individual Ixx, Iyy, Izz, Ixy, Ixz and Iyz values for all **Cells** belonging to the same **Feature**.
5. Find the *eigenvalues* and *eigenvectors* of the *3x3* symmetric matrix defined by the *6* values calculated in step 4 for each **Feature**.
6. Use the relationship of *principal moments* to the *principal axis lengths* for an ellipsoid, which can be found in [4], to determine the *Axis Lengths*
7. Calculate the *Aspect Ratios* from the *Axis Lengths* found in step 6.
8. Determine the Euler angles required to represent the *principal axis directions* in the *sample reference frame* and store them as the **Feature**'s *Axis Euler Angles*.
9. Calculate the moment variant Omega3 as definied in [2] and is discussed further in [1] and [3].


------------
Sources: 

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

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | Centroids | X, Y, Z coordinates (floats) of **Feature** center of mass | Filter will calculate **Feature** centroids if not previously calculated | Find Feature Centroids (Generic) |
| Feature | Volumes | Volume (float) in um^3 of the **Feature**. | Filter will calculate volume of each **Feature** if not already calculated | Find Feature Sizes (Statistics) | 

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | AspectRatios |  |  |
| Feature | AxisEulerAngles |  |  |
| Feature | AxisLengths |  |  |
| Feature | Omega3s |  |  |

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

