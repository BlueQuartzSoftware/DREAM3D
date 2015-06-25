Jumble Orientations {#jumbleorientations}
=============

## Group (Subgroup) ##
Synthetic Building (Crystallography)

## Description ##
This Filter "jumbles" the orientations of a set of **Features** amongst each other. The algorithm for this procedure is as follows:

1. For each **Feauture**, select another random **Feature** belonging to the same **Ensemble**
2. Swap the orientations at both the **Feature** and **Element** level of the current **Feature** and the randomly selected **Feature**
3. After iterating through all **Features**, recompute the average orientations for each **Feature**

By cycling through all the **Features**, it is guaranteed that each **Feature** will be swapped at least once, but it may be swapped multiple times if a later **Feature** randomly selects it to swap with.

For more information on synthetic building, visit the [tutorial](@ref tutorialsyntheticsingle).

## Parameters ##
None

## Required Geometry ##
Not Applicable


## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Element | FeaturesIds | Int | (1) | Specifies to which **Feature** each **Element** belongs |
| Feature | EulerAngles | Float | (3) | Three angles defining the orientation of the **Feature** in Bunge convention (Z-X-Z) |
| Feature | Phases | Int | (1) |  Specifies to which **Ensemble** each **Cell** belongs |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Element | EulerAngles | Float | (3) | Three angles defining the orientation of the **Element** in Bunge convention (Z-X-Z) |
| Feature | AvgQuats | Float | (4) | The average orientation of each **Feature** in quaternion representation |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


