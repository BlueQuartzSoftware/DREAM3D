Find Distances to Characteristic Grain Boundaries {#finddiststocharactgbs}
=============

## Group (Subgroup) ##
Statistics (Crystallographic)

## Description ##
This **Filter** computes 'approximate distances' from a given boundary to the nearest boundaries of characteristic geometries, i.e., tilt, twist, symmetric, and 180&deg;-tilt boundaries.

Five-tuples of macroscopic boundary parameters constitute the five-dimensional boundary space. Crystal and grain-exchange symmetries imply that different 5-tuples may represent the same physical boundary. All these equivalent representations need to be considered. Differences in geometries of boundaries can be quantified by means of a metric defined in the space. With a properly defined metric the distance between two boundaries (minimized over all representations) is small (large) if they are similar (distinct). Boundaries of special geometric features serve as reference points in the boundary space, and general boundaries may be characterized by their distances to the nearest reference boundaries. Since calculation of the distances to the nearest characteristic boundaries is numerically expensive,  the accurate distances are replaced by parameters defined  in 
[A. Morawiec, K. Glowinski, On "macroscopic" characterization of mixed grain boundaries, Acta Mater. 61, 5756-5767 (2013)](http://www.sciencedirect.com/science/article/pii/S1359645413004515) and
[K. Glowinski, On identification of symmetric and improperly quasi-symmetric grain boundaries, J. Appl. Cryst. 47, 726-731 (2014)](http://scripts.iucr.org/cgi-bin/paper?S160057671400435X). These parameters are not only fast to calculate, but it was also proved that they are strongly correlated with the true distances.

The parameters approximating the distances to the nearest characteristic boundaries are defined as follows:
+ To the nearest tilt boundary: &alpha;<sub>L</sub> = min { &alpha; }
+ To the nearest twist boundary: &alpha;<sub>N</sub> = 90&deg; - max { &alpha; }
+ To the nearest symmetric boundary: &alpha;<sub>S</sub> = min { [ &alpha;<sup>2</sup> + (180&deg; - &omega;)<sup>2</sup> ]<sup>1/2</sup> }
+ To the nearest 180&deg;-tilt boundary: &alpha;<sub>I</sub> = min { [ (90&deg; - &alpha;)<sup>2</sup> + (180&deg; - &omega;)<sup>2</sup> ]<sup>1/2</sup> }

In the above formulas, &alpha; = | **u** * **n**<sub>1</sub> | with **u** and **n**<sub>1</sub> being
unit vectors representing the misorientation axis and boundary normal given in *first* of the crystallites, respectively;
&omega; stands for the misorientation angle, and minimization is over symmetrically equivalent boundary representations.

![GB network reconstructed from the pure Ni 3D data set presented by J. Li, S.J. Dillon and G.S. Rohrer in 'Relative Grain Boundary Area and Energy Distributions in Nickel', Acta Mater. 57, 4304-4311 (2009). GBs are colored according to their distances to the nearest symmetric(-tilt) boundaries; Boundaries with that distance smaller than 8 deg. are marked with blue color.](Images/FindDistsToCharactGBs_example.png)

### Why are some distances equal to 999.0? ###
If phases of **Features** separated by a **Face** are the same, i.e., if the **Face** is a grain boundary, then the distances are
computed as defined above; the number of equivalent boundary representations processed is determined by crystal symmetry of the given phase.

However, if the phases of **Features** separated by a **Face** are different, then that **Face** is not a grain boundary, but
an interphase boundary, and all distances are therefore set to 999.0&deg; (a value which is out of the ranges for the distances).
999.0 is also ascribed to triangles lying at the outer surface of the volume.

## Parameters ##
None

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

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|--------|--------------|-------------|-------|-----|
| **Face Attribute Array** | DistanceToTilt | float | (1) | &alpha;<sub>L</sub>, given in degrees |
| **Face Attribute Array** | DistanceToTwist | float | (1) | &alpha;<sub>N</sub>, given in degrees |
| **Face Attribute Array** | DistanceToSymmetric | float | (1) | &alpha;<sub>S</sub>, given in degrees |
| **Face Attribute Array** | DistanceTo180tilt | float | (1) | &alpha;<sub>I</sub>, given in degrees |

## Feedback ##
In the case of any questions, suggestions, bugs, etc., please feel free to email the author of this filter at kglowinski *at* ymail.com

## References ##
[1] A. Morawiec, K. Glowinski, On "macroscopic" characterization of mixed grain boundaries, Acta Mater. 61, 5756-5767 (2013)

[2] K. Glowinski, On identification of symmetric and improperly quasi-symmetric grain boundaries, J. Appl. Cryst. 47, 726-731 (2014)

[3] J. Li, S.J. Dillon and G.S. Rohrer in 'Relative Grain Boundary Area and Energy Distributions in Nickel', Acta Mater. 57, 4304-4311 (2009)

## License & Copyright ##
Please see the description file distributed with this **Plugin**.

## DREAM.3D Mailing Lists ##
If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)