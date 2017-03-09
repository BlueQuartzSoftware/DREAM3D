Merge Colonies {#mergecolonies}
======

## Group (Subgroup) ##
Reconstruction (Grouping)

## Description ##
This **Filter** groups neighboring **Features** that have a *special* misorientation that is associated with *alpha* variants that transformed from the same *beta* grain in titanium.  The algorithm for grouping the **Features** is analogous to the algorithm for segmenting the **Features**, except the average orientation of the **Features** are used instead of the orientations of the individual **Elements** and the criterion for grouping is specific to the *alpha-beta transformation*.  The user can specify a tolerance on both the *axis* and the *angle* that defines the misorientation relationship (i.e., a tolerance of 1 degree for both tolerances would allow the neighboring **Features** to be grouped if their misorientation was between 59-61 degrees about an axis within 1 degree of a2, as given by the 3rd *special* misorientation below).

The list of *special* misorientations can be found in the paper by Germain et al.<sup>1</sup> and are listed here: 

| Angle | Axis |
|------|------|
| 0 | Identity |
| 10.529 | c = <0001> |
| 60 | a2 = <-12-10> |
| 60.832 | d1 at 80.97 degrees from c in the plane of (d3,c) |
| 63.262 | d2 at 72.73 degrees from c in the plane of (a2,c) |
| 90 | d3 at 5.26 degrees from a2 in the basal plane |


## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Axis Tolerance (Degrees) | float | Tolerance allowed when comparing the axis part of the axis-angle representation of the misorientation to the _special_ misorientations listed above |
| Angle Tolerance (Degrees) | float | Tolerance allowed when comparing the angle part of the axis-angle representation of the misorientation to the _special_ misorientations listed above |
| Use Non-Contiguous Neighbors | bool | Whether to use a non-contiguous neighbor list during the merging process |
| Identify Glob Alpha | bool | Whether to identify glob alpha regions during the merging process |

## Required Geometry ##
Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | NonContiguousNeighbors | List of int32_t | (1) | List of non-contiguous neighbors for each **Feature**. Only needed if _Use Non-Contiguous Neighbors_ is checked |
| **Feature Attribute Array** | NeighborList | List of int32_t | (1) | List of neighbors for each **Feature** |
| **Element Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Element** belongs |
| **Element Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Element** belongs |
| **Feature Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Feature** belongs |
| **Feature Attribute Array** | AvgQuats | float| (4) | Specifies the average orientation of the **Feature** in quaternion representation |
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each **Ensemble** |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Element Attribute Array** | ParentIds | int32_t | (1) | Specifies to which _parent_ each **Element** belongs |
| **Element Attribute Array** | GlobAlpha | int32_t | (1) | Specifies to which _glob alpha_ each **Element** belongs. Only created if _Identify Glob Alpha_ is checked |
| **Attribute Matrix** | NewFeatureData | Feature | N/A | Created **Feature Attribute Matrix** name |
| **Feature Attribute Array** | ParentIds | int32_t | (1) | Specifies to which _parent_ each **Feature** belongs |
| **Feature Attribute Array** | Active | bool | (1) | Specifies if the **Feature** is still in the sample (*true* if the **Feature** is in the sample and *false* if it is not). At the end of the **Filter**, all **Features** will be _Active_ |

## References ##

[1] L. Germain, N. Gey and M. Humbert, Reliability of reconstructed Beta orientation maps in titanium alloys, Ultramicrscopy, 2007, 1129-1135.


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


