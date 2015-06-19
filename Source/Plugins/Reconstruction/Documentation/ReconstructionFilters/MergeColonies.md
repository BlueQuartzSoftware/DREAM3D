Merge Colonies {#mergecolonies}
======

## Group (Subgroup) ##
Reconstruction Filters (Grouping)

## Description ##
This Filter groups neighboring **Features** that have a *"special"* misorientation that is associated with *alpha* variants that transformed from the same *beta* grain in titanium.  The algorithm for grouping the **Features** is analogous to the algorithm for segmenting the **Features** - only the average orientation of the **Features** are used instead of the orientations of the individual **Cells** and the criterion for grouping is specific to the *alpha-beta transformation*.  The user can specify a tolerance on both the *axis* and the *angle* that defines the misorientation relationship (i.e. a tolerance of 1 degree for both tolerances would allow the neighboring **Features** to be grouped if their misorientation was between 59-61 degrees about an axis within 1 degree of a2, as given by the 3rd *"special"* misorientation below).

The list of *"special"* misorientations can be found in the paper by Germain et al [1] and are listed here: 

| Angle | Axis |
|------|------|
| 0 | Identity |
| 10.529 | c = <0001> |
| 60 | a2 = <-12-10> |
| 60.832 | d1 at 80.97 degrees from c in the plane of (d3,c) |
| 63.262 | d2 at 72.73 degrees from c in the plane of (a2,c) |
| 90 | d3 at 5.26 degrees from a2 in the basal plane |

__[1] L. Germain, N. Gey and M. Humbert, Reliability of reconstructed Beta orientation maps in titanium alloys, Ultramicrscopy, 2007, 1129-1135.__


## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Axis Tolerance | Float | Tolerance allowed when comparing the axis part of the axis-angle representation of the misorientation to the "special" misorientations listed above |
| Angle Tolerance | Float | Tolerance allowed when comparing the angle part of the axis-angle representation of the misorientation to the "special" misorientations listed above |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |
| Feature | Phases | Int | (1) | Specifies the phase of the **Feature** - Values will begin at 1 as there is no phase 0, which is used temporarily in some filters for bad data|
| Feature | AvgQuats | Float | (4) | Defines the average orientation of the **Feature** in quaternion representation  (<x,y,z>, w) |
| Ensemble | CrystalStructures | Int | (1) | Specifies the crystal structure of each Ensemble using an enumeration defined by DREAM3D (Hexagonal_High=0, Cubic_High=1, Hexagonal_Low=2, Cubic_Low=3, Triclinic=4, Monoclinic=5, Orthorhombic=6, Tetragonal_Low=7, Tetragonal_High=8, Trigonal_Low=9, Trigonal_High=10, Unknown=999) |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | Active | Boolean | (1) | Value specifying if the **Feature** is still in the sample (1 if the **Feature** is in the sample and 0 if it is not). At the end of the filter, all **Features** will be "Active" as the "Inactive" **Features** will have been removed.  |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


