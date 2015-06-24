Find Twin Boundaries {#findtwinboundaries}
=============

## Group (Subgroup) ##
Statistics (Crystallographic)

## Description ##
This Filter identifies all **Triangles** between neighboring **Features** that have a &sigma; = 3 twin relationship.  The Filter uses the average orientation of the **Features** on either side of the **Triangle** to determine the *misorientation* between the **Features**.  If the *axis-angle* that describes the *misorientation* is within both the axis and angle user-defined tolerance, then the **Triangle** is flagged as being a twin.  After the **Triangle** is flagged as a twin, the crystal direction parallel to the **Face** normal is determined and compared with the *misorientation axis* if *Compute Coherence* is selected.  The misalignment of these two crystal directions is stored as the incoherence value for the **Triangle** (in degrees). Note this Filter will only extract twin boundaries if the twin **Feature** is the same phase as the parent **Feature**. 

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Axis Tolerance (Degrees) | Float | Degree of tolerance for angular distance from the [111] axis  |
| Angle Tolerance (Degrees) | Float | Degree of tolerance for angular deviation from 60<sup>o</sup> |
| Compute Coherence | Bool | Whether to compute the coherence between the **Face** normal and the misorientation axis |

## Required Geometry ##
Image + Triangle

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | AvgQuats | Float | (4) | Specifies the average orientation of each **Feature** in quaternion representation |
| Feature | Phases | Int | (1) | Specifies to which phase each **Feature** belongs |
| Ensemble | CrystalStructures | Int | (1) | Enumeration representing the crystal structure for each phase |
| Face | FaceLabels | Int | (2) | Specifies which **Features** are on either side of each triangle |
| Face | FaceNormals | Double | (3) | Specifies the normal of each triangle. Only required if _Compute Coherence_ is checked |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Face | TwinBoundary | Bool | (1) | Whether a give **Face** belongs to a twin boundary |
| Face | TwinBoundaryIncoherence |  Float | (1) |Angle in degrees between crystal direction parallel to **Face** normal and misorientation axis | 


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


