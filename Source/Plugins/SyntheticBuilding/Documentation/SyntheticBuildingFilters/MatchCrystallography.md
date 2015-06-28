Match Crystallography {#matchcrystallography}
=============

## Group (Subgroup) ##
Synthetic Building (Crystallography)

## Description ##
This **Filter** attempts to match a defined _orientation distribution function_ (ODF) to a set of **Features**. The matching algorithm involves two processes that are performed iteratively:

1. _Swappping_ out the orientation of a randomly selected **Feature** for a new orientation sampled from the goal ODF
2. _Switching_ the orientations of two **Features** selected at random 

The _switch_ or _swap_ is accepted if it lowers the error of the current ODF and misorientation distribution function (MDF) from the goal. This process continues for a user defined number of iterations, or until the texture functions are matched to within precision.

For more information on synthetic building, visit the [tutorial](@ref tutorialsyntheticsingle).  

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Maximum Number of Iterations (Swaps) | int32_t | Maximum number of swaps to perform for the matching process |

## Required Geometry ##
Image

## Required Objects ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |
| **Feature Attribute Array** | Phases | int32_t | (1) |  Specifies to which **Ensemble** each **Cell** belongs |
| **Feature Attribute Array** | SurfaceFeatures | bool | (1) | Flag equal to 1 if the **Feature** touches an outer surface of the sample and equal to 0 if it does not |
| **Feature Attribute Array** | NeighborList | List of int32_t | (1) | List of the contiguous neighboring **Features** for a given **Feature** |
| **Feature Attribute Array** | SharedSurfaceAreaLists | List of float | (1) | List of the shared surface area for each of the contiguous neighboring **Features** for a given **Feature** |
| **Ensemble Attribute Array** | Statistics | Statistics Object | (1) | Statistics objects (depending on *Phase Type*) that store fits to descriptors such as size distribution, shape distribution, neighbor distribution, ODF, MDF, etc. |
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each **Ensemble** |
| **Ensemble Attribute Array** | PhaseTypes | uint32_t | (1) | Enumeration specifying the phase type of each **Ensemble** |
| **Ensemble Attribute Array** | NumFeatures | int32_t | (1) | Specifies the number of **Features** in each **Ensemble** |

## Created Objects ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | EulerAngles | float | (3) | Three angles defining the orientation of the **Cell** in Bunge convention (Z-X-Z) |
| **Feature Attribute Array** | Volumes | float | (1) | Specifies the volume of each **Feature** |
| **Feature Attribute Array** | AvgEulerAngles | float | (3) | Three angles defining the orientation of the **Feature** in Bunge convention (Z-X-Z) |
| **Feature Attribute Array** | AvgQuats | float | (4) | The average orientation of each **Feature** in quaternion representation |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


