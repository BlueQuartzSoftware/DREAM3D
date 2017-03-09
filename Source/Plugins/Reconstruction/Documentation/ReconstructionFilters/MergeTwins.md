Merge Twins {#mergetwins}
======

## Group (Subgroup) ##
Reconstruction (Grouping)

## Description ##
This **Filter** groups neighboring **Features** that are in a twin relationship with each other (currently only FCC &sigma; = 3 twins).  The algorithm for grouping the **Features** is analogous to the algorithm for segmenting the **Features** - only the average orientation of the **Features** are used instead of the orientations of the individual **Elements**.  The user can specify a tolerance on both the *axis* and the *angle* that defines the twin relationship (i.e., a tolerance of 1 degree for both tolerances would allow the neighboring **Features** to be grouped if their misorientation was between 59-61 degrees about an axis within 1 degree of <111>, since the Sigma 3 twin relationship is 60 degrees about <111>).


## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Axis Tolerance (Degrees) | float | Tolerance allowed when comparing the axis part of the axis-angle representation of the misorientation |
| Angle Tolerance (Degrees) | float | Tolerance allowed when comparing the angle part of the axis-angle representation of the misorientation |

## Required Geometry ##
Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | NonContiguousNeighbors | List of int32_t | (1) | List of non-contiguous neighbors for each **Feature**. Only needed if _Use Non-Contiguous Neighbors_ is checked |
| **Feature Attribute Array** | NeighborList | List of int32_t | (1) | List of neighbors for each **Feature** |
| **Element Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Element** belongs |
| **Feature Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Feature** belongs |
| **Feature Attribute Array** | AvgQuats | float| (4) | Specifies the average orientation of the **Feature** in quaternion representation |
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each **Ensemble** |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Element Attribute Array** | ParentIds | int32_t | (1) | Specifies to which _parent_ each **Element** belongs |
| **Attribute Matrix** | NewFeatureData | Feature | N/A | Created **Feature Attribute Matrix** name |
| **Feature Attribute Array** | ParentIds | int32_t | (1) | Specifies to which _parent_ each **Feature** belongs |
| **Feature Attribute Array** | Active | bool | (1) | Specifies if the **Feature** is still in the sample (*true* if the **Feature** is in the sample and *false* if it is not). At the end of the **Filter**, all **Features** will be _Active_ |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


