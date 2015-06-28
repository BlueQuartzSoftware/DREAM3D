Neighbor Orientation Correlation {#neighbororientationcorrelation}
======

## Group (Subgroup) ##
Processing (Cleanup)

## Description ##
This **Filter** first identifies all **Cells** that have a *confidence index* below the minimum set by the user.  Then, for each of those **Cells**, their neighboring **Cells** are checked to determine the number of neighbor **Cells** with orientations different than the reference **Cell** by more than the user defined *misorientation tolerance*.  In addition the neighboring **Cells** are compared with each other to determine the number of neighboring **Cells** that have the same orientation (again within the user defined tolerance). The *Cleanup Level* set by the user is then used to determine which **Cells** are replaced.  If a level of 5 is set, then at least 5 of the neighboring **Cells** must be different than the reference **Cell** and at least 5 of the neighboring **Cells** must be the same as each other (and so on for other *Cleanup Level*). If a **Cell** meets the replacement criteria, then all of its attributes are replaced with the attribtues of one of the neighboring **Cells** that are the same as each other.

*Note:* The **Filter** will iteratively reduce the *Cleanup Level* from 6 until it reaches the user defined number. So, if the user selects a level of 4, then the **Filter** will run with a level of 6, then 5, then 4 before finishing.  

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Minimum Confidence Index | float | Sets the minimum value of 'confidence' a **Cell** must have |
| Misorientation Tolerance (Degrees) | Float | Angular tolerance used to compare with neighboring **Cells** |
| Cleanup Level | int32_t | Minimum number of neighbor **Cells** that must have orientations within above tolerace to allow **Cell** to be changed | 

## Required Geometry ##
Image

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Cell Attribute Array** | Confidence Index | float | (1) | Specifies the confidence in the orientation of the **Cell** (TSL data) |
| **Cell Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Cell** belongs |
| **Cell Attribute Array** | Quats | float | (4) | Specifies the orientation of the **Cell** in quaternion representation |
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each **Ensemble** |

## Created Objects ##
None


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


