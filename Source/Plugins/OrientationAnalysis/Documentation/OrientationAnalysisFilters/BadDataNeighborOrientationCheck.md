Bad Data Orientation Correlation {#baddataneighbororientationcheck}
=============

## Group (Subgroup) ##
Orientation Analysis (Cleanup)

## Description ##
This Filter compares the orientations of *bad* **Cells** with their neighbor **Cells**.  If the misorientation is below a user defined tolerance for a user defined number of neighbor **Cells** , then the *bad* **Cell** will be changed to a *good* **Cell**.

*Note:* Only the boolean value defining the **Cell** as *good* or *bad* is changed, not the data at **Cell**.

*Note:* The Filter will iteratively reduce the required number of neighbors from 6 until it reaches the user defined number. So, if the user selects a required number of neighbors of 4, then the filter will run with a required number of neighbors of 6, then 5, then 4 before finishing.  

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Misorientation Tolerance (Degrees) | Float | Angular tolerance used to compare with neighboring **Cells** |
| Required Number of Neighbors | Integer | Minimum number of neighbor **Cells** that must have orientations within above tolerace to allow **Cell** to be changed |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | Quats | Float | (4) | Specifies the orientation of the **Cell** in quaternion representation |
| Cell | GoodVoxels | Boolean | (1) | Used to define **Cells** as *good* or *bad*  |
| Cell | Phases | Int | (1) | Specifies the **Ensemble** of the **Cell** |
| Ensemble | CrystalStructures | Int | (1) | Enumeration representing the crystal structure for each phase |

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


