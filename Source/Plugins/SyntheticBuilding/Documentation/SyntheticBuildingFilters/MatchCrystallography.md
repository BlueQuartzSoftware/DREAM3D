Match Crystallography {#matchcrystallography}
=============

## Group (Subgroup) ##
Synthetic Building (Crystallography)

## Description ##
This Filter attempts to match a defined _orientation distribution function_ (ODF) to a set of **Features**. The matching algorithm involves two processes that are performed iteratively:

1. _Swappping_ out the orientation of a randomly selected **Feature** for a new orientation sampled from the goal ODF
2. _Switching_ the orientations of two **Features** selected at random 

The _switch_ or _swap_ is accepted if it lowers the error of the current ODF and misorientation distribution function (MDF) from the goal. This process continues for a user defined number of iterations, or until the texture functions are matched to within precision.

For more information on synthetic building, visit the [tutorial](@ref tutorialsyntheticsingle).  

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Maximum Number of Iterations (Swaps) | Int | Maximum number of swaps to perform for the matching process |

## Required Geometry ##
Image

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Ensemble | Statistics | Statistics Object | (1) | Statistics objects (depending on *Phase Type*) that store fits to descriptors such as size distribution, shape distribution, neighbor distribution, ODF, MDF, etc. |
| Ensemble | CrystalStructures | Int | (1) | Enumeration representing the crystal structure for each phase |
| Ensemble | PhaseTypes | Int | (1) | Enumeration specifying the type of phase of each **Ensemble**  |
| Cell | FeaturesIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs |
| Feature | Phases | Int | (1) |  Specifies to which **Ensemble** each **Cell** belongs |
| Feature | SurfaceFeatures | Boolean | (1) | Flag equal to 1 if the **Feature** touches an outer surface of the sample and equal to 0 if it does not |
| Feature | NeighborList | List of Ints | (1) | List of the contiguous neighboring **Features** for a given **Feature** |
| Feature | SharedSurfaceAreaLists | List of Floats | (1) | List of the shared surface area for each of the contiguous neighboring **Features** for a given **Feature** |
| Ensemble | NumFeatures | Int | (1) | Specified the number of **Features** in each **Ensemble** |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Cell | EulerAngles | Float | (3) | Three angles defining the orientation of the **Cell** in Bunge convention (Z-X-Z) |
| Feature | Volumes | Float | (1) | Specifies the volume of each **Feature** |
| Feature | EulerAngles | Float | (3) | Three angles defining the orientation of the **Feature** in Bunge convention (Z-X-Z) |
| Feature | AvgQuats | Float | (4) | The average orientation of each **Feature** in quaternion representation |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


