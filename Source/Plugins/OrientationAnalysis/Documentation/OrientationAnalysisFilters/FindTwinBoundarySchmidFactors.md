Find Twin Boundary Schmid Factors {#findtwinboundaryschmidfactors}
=============

## Group (Subgroup) ##
Statistics (Crystallographic)

## Description ##
This **Filter** calculates the *Schmid factors* for the 3 slip systems in the plane of each twin boundary.  The **Filter** only calculates the values for **Faces** already identified as twins. The **Filter** can also write an information file that contains details about each **Triangle** in the **Geometry**. The information written is ASCII text with the following columns of data for each **Triangle**:

	Feature1	Feature2	IsTwin	Plane	Schmid1	Schmid2	Schmid3

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Loading Direction | float (3x) | The loading axis for the sample |
| Write Twin Boundary Info File | bool | Whether to write an ASCII text file |
| Twin Boundary Info File | File Path | The output twin boundary info file path. Only needed if _Write Twin Boundary Info File_ is checked |


## Required Geometry ##
Image + Triangle

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | AvgQuats | float | (4) | Specifies the average orienation of each **Feature** in quaternion representation |
| **Feature Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Feature** belongs |
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each **Ensemble** |
| **Face Attribute Array** | FaceLabels | int32_t | (2) | Specifies which **Features** are on either side of each triangle |
| **Face Attribute Array** | FaceNormals | double | (3) | Specifies the normal of each triangle |
| **Face Attribute Array** | TwinBoundary | bool | (1) | Specifies whether a given **Face** belongs to a twin boundary |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Face Attribute Array** | TwinBoundarySchmidFactors | float | (3) | The three Schmid factors for each **Triangle** corresponding to the three slip systems. This array will have a _0_ value if the **Triangle** is not a twin boundary |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


