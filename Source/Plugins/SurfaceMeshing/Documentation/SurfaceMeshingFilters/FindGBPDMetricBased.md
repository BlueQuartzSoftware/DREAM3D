Find GBPD (Metric-based Approach) {#findgbpdmetricbased}
=============

## Group (Subgroup) ##
Surface Meshing

## Description ##
This filter computes the grain boundary plane distribution (GBPD) like that shown in Fig. 1.
It should be noted that most GBPDs presented so far in literature were
obtained using a method based on partition of the grain boundary space
into bins, similar to that implemented in *Find GBCD* filter.
This filter
calculates the GBPD using 
an alternative approach adapted from the one included in
*Find GBCD (Metric-based Approach)* filter
and described by
K. Glowinski and A. Morawiec
in 
[Analysis of experimental grain boundary distributions based on boundary-space metrics, Metall. Mater. Trans. A 45, 3189-3194 (2014)](http://link.springer.com/article/10.1007%2Fs11661-014-2325-y).
Briefly, the GBPD is probed at evenly distributed sampling directions
(similarly to *Find GBCD (Metric-based Approach)* filter) and areas of mesh segments with their
normal vectors deviated by less than a limiting angle &rho;<sub>p</sub> 
from a given direction are summed. If *n*<sub>S</sub> is the number of crystal symmetry transformations,
each boundary plane segment is represented by up to 4 &times; *n*<sub>S</sub> equivalent vectors, 
and all of them are processed. It is enough to sample the distribution at directions corresponding
to the standard stereographic triangle (or, in general, to a fundamental region corresponding to a considered crystallographic point group);
values at remaining points are obtained based on crystal symmetries.
After summing the boundary areas, the distribution is normalized. 
First, the values at sampling vectors are divided by the total area of all segments. 
Then, in order to express the distribution in the conventional units, i.e., 
multiples of random distribution (MRDs), the obtained fractional values are divided by
the volume *v* = (*A* n<sub>S</sub>) / (4&pi;), where *A* is the area of a spherical cap determined by &rho;<sub>p</sub>. 

![Fig. 1: GBPD obtained for Small IN100 with the limiting distance set to 7&deg; and with triangles adjacent to triple lines removed. Units are MRDs.](FindGBPDMetricBased_example.png)

This filter also calculates statistical errors of the distributions using the formula
&epsilon; = ( *f* *n* *v* )<sup>1/2</sup>, where &epsilon;
is the relative error of the
distribution function at a given point, *f* is the value of the function at that point,
and *n* stands for the number
of grain boundaries (**not** the number of mesh triangles)
in the considered network.
The errors can be calculated either as their absolute values, 
i.e., &epsilon; &times; *f* or as relative errors, i.e., 100% &times; &epsilon;.
The latter are computed in a way that if the relative error exceeds 100%, it is rounded down to 100%.
See also the documentation for *Find GBCD (Metric-based Approach)* filter.



## Parameters ##
| Name | Type | Description |
|------|------|-------------|
| Phase of Interest | int32_t | Index of the **Ensemble** for which to compute GBPD; boundaries having grains of this phase on both its sides will only be taken into account |
| Limiting Distance | float | &rho;<sub>p</sub> as defined above |
| Number of Sampling Points | int32_t | The **approximate** number of sampling directions |
| Exclude Triangles Directly Neighboring Triple Lines | bool | Only interiors of **Faces** are included in GBPD |
| Save GBPD to | File Path | The output file path (extension .dat, GMT format) |
| Save GBPD Errors to | File Path |  The output file path (extension .dat, GMT format) |
| Save Relative Errors... | bool | See above |

## Required Geometry ##
Image + Triangle

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|-------|--------------|-------------|---------|-----|
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each **Ensemble** |
| **Feature Attribute Array** | AvgEulerAngles | float | (3) | Three angles defining the orientation of the **Feature** in Bunge convention (Z-X-Z) |
| **Feature Attribute Array** | Phases | int32_t | (1) | Specifies to which phase each **Feature** belongs |
| **Face Attribute Array** | FaceLabels | int32_t | (2) | Specifies which **Features** are on either side of each **Face** |
| **Face Attribute Array**  | FaceNormals | double | (3) | Specifies the normal of each **Face** |
| **Face Attribute Array**  | FaceAreas | double | (1) | Specifies the area of each **Face** |
| **Feature Face Attribute Array**  | FaceLabels | int32_t | (2) | Specifies to which phase each **Face Feature** belongs |
| **Vertex Attribute Array**  | NodeTypes | int8_t | (1) | Specifies the type of node in the Geometry


## Format of Output Files ##
Output files are formatted to be readable by GMT plotting program.
The first line is always "0.0 0.0 0.0 0.0".
Each of the remaining lines contains three numbers. 
The first two columns are angles (in degrees) describing a given sampling direction; 
let us denote them  *col*<sub>1</sub> and *col*<sub>2</sub>, respectively.
The third column is either the value of the GBCD (in MRD) for that direction or its error (in MRD or %, depending on user's selection).
If you use other software, you can retrive spherical angles &theta;
and &phi; of the sampling directions in the following way:

&theta; = 90&deg; - *col*<sub>1</sub>

&phi; = *col*<sub>2</sub>

Then, the directions are given as [ sin &theta; &times; cos &phi; , sin &theta; &times; sin &phi; , cos &theta; ].


## Feedback ##
In the case of any questions, suggestions, bugs, etc., 
please feel free to email the author of this filter at kglowinski *at* ymail.com



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users