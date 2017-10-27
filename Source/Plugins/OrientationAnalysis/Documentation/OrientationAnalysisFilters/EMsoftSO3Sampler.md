 EMsoftSO3Sampler {#emsoftso3sampler}
=============

## Group (Subgroup) ##
EMsoftToolbox (EMsoftToolbox)

## Description ##
The EMsoftSO3Sampler filter produces several different types of uniform samples of SO(3): 

| Mode | Description |
|------|-------------|
| 0 | a uniform sampling of a Rodrigues fundamental zone (FZ) |
| 1 | a uniform sampling of orientations at a constant misorientation from a given orientation |
| 2 | a uniform sampling of orientations at less than a given misorientation from a given orientation. |

All three sampling methods are based on the cubochoric rotation representation, which starts with a cubical grid inside the cubochoric cube.  This cube represents an equal-volume mapping of the quaternion Northern hemisphere (i.e., all 3D rotations with positive scalar quaternion component).  For sampling mode 0, the filter creates a uniform grid of cubochoric vectors, transforms each vector to the Rodrigues representation and determines whether or not the point lies inside the FZ for the point group symmetry set by the user.  The filter then returns an array of Euler angle triplets (Bunge convention) for use in subsequent filters.  The sampling grid can be offset from the center of the cube, in which case the identity orientation will not be part of the sample.

For sampling mode 1, the filter samples the surface of a centered cube inside the cubochoric cube and converts those points to a quadratic surface (prolate spheroid, spheroidal paraboloid, or double-sheet hyperboloid, depending on the parameter choices) in Rodrigues Space; all generated points will have the same misorientation with respect to a user defined reference point.

Sampling mode 2 does the same as mode 2, but now the inside of the starting cube is also filled with sampling points, leading to a uniform sampling of orientations surrounding a user defined orientation with up to a maximum misorientation with respect to that orientation.

Detailed information on the cubochoric rotation representation can be found in the following paper: D. Rosca, A. Morawiec, and M. De Graef. **"A new method of constructing a grid in the space of 3D rotations and its applications to texture analysis,"** _Modeling and Simulations in Materials Science and Engineering **22**, 075013 (2014)._

Details on the misorientation sampling approach can be found in the following paper: S. Singh and M. De Graef, **"Orientation sampling for dictionary-based diffraction pattern indexing methods"** submitted to _MSMSE (2016)_.

## Point group identifiers ##
Crystallographic point groups are identified by an integer from 1 to 32 according to the International Tables for Crystallography (Volume A). The valid numbers, along with the corresponding Hermann-Mauguin point group symbols, are listed here, along with the point group symbol between double quotation marks, the corresponding rotation group and its order M (in bold face):

| ID -> Symbol | ID -> Symbol | ID -> Symbol | ID -> Symbol |
|------|------|------|------|
| 1 -> "1" -> **1(1)** | 2 -> "-1" -> **1(1)**| 3 -> "2" -> **2(2)**| 4 -> "m" -> **2(2)**|
|5 -> "2/m" -> **2(2)**| 6 -> "222"  -> **222(4)**| 7 -> "mm2"  -> **222(4)**| 8 -> "mmm"  -> **222(4)**|
| 9 -> "4"  -> **4(4)** | 10 -> "-4" -> **4(4)**  | 11 -> "4/m" -> **4(4)** | 12 -> "422" -> **422(8)** |
|13 -> "4mm" -> **422(8)** | 14 -> "-42m" -> **422(8)** | 15 -> "4/mmm" -> **422(8)** | 16 -> "3" -> **3(3)** |
|17 -> "-3" -> **3(3)** | 18 -> "32" -> **32(6)** | 19 ->  "3m"  -> **32(6)** | 20 -> "-3m" -> **32(6)** |
|21 -> "6" -> **6(6)** | 22 -> "-6"  -> **6(6)** | 23 -> "6/m" -> **6(6)** | 24 -> "622" -> **622(12)** |
| 25 -> "6mm" -> **622(12)** | 26 -> "-6m2" -> **622(12)**  | 27 -> "6/mmm" -> **622(12)**  | 28 -> "23" -> **23(12)** |
| 29 -> "m3" -> **23(12)** | 30 -> "432" -> **432(24)** | 31 -> "-43m" -> **432(24)** | 32 -> "m-3m" -> **432(24)** |

## Number of grid points ##
The cubochoric space is a cube with edge length pi^(2/3) and origin (the identity rotation) at the center of the cube.  The number of sampling points entered by the user represents the number of grid points **along a semi-edge of the cube**.  In other words, if the user requests N=50 sampling points, and the origin is part of the grid (see next item), then there will be 2N+1 actual sampling points along each cube edge.  The total number of sampling points in the grid will then be (2N+1)^3.  For point groups #1 and #2, in the absence of any rotational symmetry, the Rodrigues FZ will correspond to the full cubochoric grid, with (2N+1)^3 grid points.  For any other point group, the Rodrigues FZ will correspond to a portion of the cubochoric grid, and the number of points will be approximately given by (2N+1)^3 /M, where M is the order of the rotation group corresponding to the point group.  In the table above, the rotation group and its order are indicated in bold face.

## Grid offset switch##
For sampling mode 1, the user has the option to offset the cubochoric grid from the origin by half a grid unit.  In that case, the grid will have a maximum of 8N^3 grid points, and the identity rotation will **not** be part of the sample.  The total number of points inside the Rodrigues FZ will then be approximately 8N^3 /M.

## Misorientation sampling ##
For sampling modes 2 and 3, the user must provide a reference orientation in the form of an Euler angle triplet (Bunge convention); this orientation will be used as the reference orientation around which the misorientation sampling will be computed.  The output of all three sampling modes will be in Euler angles.

## Filter progress bar ##
The filter progress bar at the bottom of the main DREAM.3D window will update the total number of points analyzed out of the total possible (either (2N+1)^3 or 8N^3) along with the number of grid points found to lie inside the Rodrigues FZ.


## Parameters ##
| Name | Type | Default Value | Description |
|------|------|------|------|
| SamplingMode | int | 1 | Sampling mode selector |
| Numpg| int | 32 | Point group identifier (mode 1 only)|
| Numsp| int | 50 | Number of grid points along sampling cube semi-edge |
| Numpg| bool | false | Grid offset switch (mode 1 only)|
| Misor | float | 3.0 | Misorientation angle (degrees, modes 2 and 3 only) |
| Refor | float | (0.0, 0.0, 0.0) | Euler angles for reference orientation (modes 2 and 3 only) |

## Required Geometry ##

Not Applicable

## Required Objects ##

Not Applicable

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|----------------|
| **Data Container** | EMsoftDataContainer | N/A | N/A | Created **Data Container** name **EMsoftDataContainer**|
| **Attribute Matrix** | EMsoftAttributeMatrix | Generic | N/A | Created **Cell Attribute Matrix** name |
| **Generic Attribute Array** | (euler; user defined) | float | (1 tuple; 3D array) | Created **Generic Attribute Matrix** name |

## License & Copyright ##

Please see the description file distributed with this plugin.

## Funding Acknowledgment ##

This filter was developed with financial support from contract AFRL FA8650-10-D-5210, Task Order 0034.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users

