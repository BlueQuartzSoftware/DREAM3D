Export INL File {#inlwriter}
=============

## Group (Subgroup) ##
IO (Output)

## Description ##
This **Filter** writes out **Cell** data from an **Image Geometry** to a file format used by the Idaho National Laboratory (INL).  The format is columnar and space delimited, with header lines denoted by the "#" character. The columns are the following:

	- phi1 
	- Phi 
	- phi2
	- x Position (Microns)
	- y Position (Microns)
	- z Position (Microns)
	- Feature Id
	- Phase Id
	- Symmetry

Some information about the phase is included in the header section of the file in addition to values for the origin, step size, dimensions and number of **Features** in the file.

## Example Output ##

	# File written from DREAM3DLib Version 5.2.1789.6419a8d
	# DateTime: Fri Jun 19 10:13:25 2015
	# X_STEP: 0.250000
	# Y_STEP: 0.250000
	# Z_STEP: 0.250000
	#
	# X_MIN: -36.750004
	# Y_MIN: 10.250000
	# Z_MIN: -29.000000
	#
	# X_MAX: -11.750004
	# Y_MAX: 35.250000
	# Z_MAX: -4.000000
	#
	# X_DIM: 100
	# Y_DIM: 100
	# Z_DIM: 100
	#
	# Phase_1: Nickel 
	# Symmetry_1: 43
	# Features_1: 796
	#
	# Num_Features: 796 
	#
	# phi1 PHI phi2 x y z FeatureId PhaseId Symmetry
	0.266984 0.696910 4.434039 -36.750004 10.250000 -29.000000 639 1 43
	0.266984 0.696910 4.434039 -36.500004 10.250000 -29.000000 639 1 43
	0.266984 0.696910 4.434039 -36.250004 10.250000 -29.000000 639 1 43
	0.266984 0.696910 4.434039 -36.000004 10.250000 -29.000000 639 1 43
	0.267274 0.697210 4.432979 -35.750004 10.250000 -29.000000 639 1 43
	0.266984 0.696910 4.434039 -35.500004 10.250000 -29.000000 639 1 43
	0.266984 0.696910 4.434039 -35.250004 10.250000 -29.000000 639 1 43
	0.266984 0.696910 4.434039 -35.000004 10.250000 -29.000000 639 1 43
	0.266234 0.697020 4.434729 -34.750004 10.250000 -29.000000 639 1 43
	0.266234 0.697020 4.434729 -34.500004 10.250000 -29.000000 639 1 43

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Output File | File Path | The output .txt file path |

## Required Geometry ##
Image 

## Required Objects ##

| Kind | Default Name |Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |
| **Cell Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Cell** belongs |
| **Cell Attribute Array** | EulerAngles | float | (3) | Three angles defining the orientation of the **Cell** in Bunge convention (Z-X-Z) |
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each **Ensemble** |
| **Ensemble Attribute Array** | Material Names | String | (1)  | Name of each **Ensemble** |
| **Ensemble Attribute Array** | Number of Features | uint32_t | (1) | The number of **Features** per **Ensemble** |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


