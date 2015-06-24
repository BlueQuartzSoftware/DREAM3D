Find Twin Boundary Schmid Factors {#findtwinboundaryschmidfactors}
=============

## Group (Subgroup) ##
Statistics (Crystallographic)

## Description ##
This Filter calculates the *Schmid factors* for the 3 slip systems in the plane of each twin boundary.  The Filter only calculates the values for **Faces** already identified as twins. The **Filter** can also write an information file that contains details about each **Triangle** in the **Geometry**. The information written is ASCII text with the following columns of data for each **Triangle**:

	Feature1	Feature2	IsTwin	Plane	Schmid1	Schmid2	Schmid3

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Loading Direction | Float (3x) | The loading axis for the sample |
| Write Twin Boundary Info File | Bool | Whether to write an ASCII text file |
| Twin Boundary Info File | File Path | The output twin boundary info file path. Only needed if _Write Twin Boundary Info File_ is checked |


## Required Geometry ##
Image + Triangle

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | AvgQuats | Float | (4) | Specifies the average orientation of each **Feature** in quaternion representation |
| Feature | Phases | Int | (1) | Specifies to which phase each **Feature** belongs |
| Ensemble | CrystalStructures | Int | (1) | Enumeration representing the crystal structure for each phase |
| Face | FaceLabels | Int | (2) | Specifies which **Features** are on either side of each triangle |
| Face | FaceNormals | Double | (3) | Specifies the normal of each triangle |
| Face | TwinBoundary | Bool | (1) | Whether a give **Face** belongs to a twin boundary |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Face | TwinBoundarySchmidFactors | Float | (3) | The three Schmid factors for each **Triangle** corresponding to the three slip systems. This array will have a _0_ value if the **Triangle** is not a twin boundary |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


