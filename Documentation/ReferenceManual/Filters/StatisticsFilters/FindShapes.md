Find Field Shapes {#findshapes}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
  The 3-D moment invariants of second order are three numbers, represented by the symbols 0≤Ωi≤1 (i = 1, 2, 3), which are computed starting
from the shape of an object. The moment invariants are invariant with respect to either similarity
or affine coordinate transformations, which renders them particularly useful as quantitative shape
descriptors that are independent of the reference frame used to represent the object(s).
Omega1 and Omega2 are related to aspect ratios, while Omega3 is useful for shape characterization. Details of the Omega3 shape descriptors are given in the Sources that include Tables of all three Omega expressions for basic geometrical shapes.

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
_


## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Field | Centroids | X, Y, Z coordinates (floats) of **Field** center of mass | Filter will calculate **Field** centroids if not previously calculated |
| Field | Volumes | Volume (float) in um^3 of the **Field**. | Filter will calculate volume of each **Field** if not already calculated |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | AspectRatios |  |  |
| Field | AxisEulerAngles |  |  |
| Field | AxisLengths |  |  |
| Field | Omega3s |  |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



