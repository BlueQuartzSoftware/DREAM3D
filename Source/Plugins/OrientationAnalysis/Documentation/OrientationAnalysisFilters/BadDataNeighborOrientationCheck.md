Neighbor Orientation Comparison (Bad Data) {#baddataneighbororientationcheck}
=============

## Group (Subgroup) ##
Orientation Analysis (Cleanup)

## Description ##
This **Filter** compares the orientations of *bad* **Cells** with their neighbor **Cells**.  If the misorientation is below a user defined tolerance for a user defined number of neighbor **Cells** , then the *bad* **Cell** will be changed to a *good* **Cell**.

*Note:* Only the boolean value defining the **Cell** as *good* or *bad* is changed, not the data at **Cell**.

*Note:* The **Filter** will iteratively reduce the required number of neighbors from 6 until it reaches the user defined number. So, if the user selects a required number of neighbors of 4, then the **Filter** will run with a required number of neighbors of 6, then 5, then 4 before finishing.  

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Misorientation Tolerance (Degrees) | float | Angular tolerance used to compare with neighboring **Cells** |
| Required Number of Neighbors | int32_t | Minimum number of neighbor **Cells** that must have orientations within above tolerace to allow **Cell** to be changed |

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | Quats | flaot | (4) | Specifies the orientation of the **Cell** in quaternion representation |
| **Cell Attribute Array** | GoodVoxels | bool | (1) | Used to define **Cells** as *good* or *bad*  |
| **Cell Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Cell** belongs |
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each phase |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


