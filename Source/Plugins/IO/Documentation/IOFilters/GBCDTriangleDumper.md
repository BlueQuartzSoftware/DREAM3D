Write GBCD Triangles File {#gbcdtriangledumper}
=============

## Group (Subgroup) ##
IO (Output)

## Description ##
This **Filter** writes relevant information about the Grain Boundary Character Distribution (GBCD) on an existing set of triangles.  The information written includes the inward and outward Euler angles, normals, and areas for each triangle.  The file format was originally defined by Prof. Greg Rohrer (CMU).

## Example Output ## 

	# Triangles Produced from DREAM3D version 5.2
	# Column 1-3:    right hand average orientation (phi1, PHI, phi2 in RADIANS)
	# Column 4-6:    left hand average orientation (phi1, PHI, phi2 in RADIANS)
	# Column 7-9:    triangle normal
	# Column 8:      surface area
	0.2662 0.6970 4.4347 0.7993 0.6738 3.5200 0.0000 0.8829 -0.4696 0.0240
	0.2662 0.6970 4.4347 0.7993 0.6738 3.5200 0.4532 0.3203 -0.8319 0.0211
	0.2662 0.6970 4.4347 0.7993 0.6738 3.5200 1.0000 0.0000 0.0000 0.0312
	0.2662 0.6970 4.4347 0.7993 0.6738 3.5200 0.9939 0.0780 0.0780 0.0315
	0.2662 0.6970 4.4347 0.7993 0.6738 3.5200 0.0000 0.9985 0.0551 0.0332
	0.2662 0.6970 4.4347 0.7993 0.6738 3.5200 0.5074 0.7638 -0.3989 0.0260
	0.7993 0.6738 3.5200 0.2662 0.6970 4.4347 0.0000 -0.7792 0.6268 0.0182
	0.7993 0.6738 3.5200 0.2662 0.6970 4.4347 -0.5737 -0.0995 0.8130 0.0221
	3.4109 0.6178 1.0586 0.2662 0.6970 4.4347 1.0000 0.0000 0.0000 0.0312
	3.4109 0.6178 1.0586 0.2662 0.6970 4.4347 0.9822 0.1328 0.1328 0.0256
	…..

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Output File | File Path | The output GBCD triangle file path |

## Required Geometry ##
Image + Triangle

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Face Attribute Array** | FaceLabels | int32_t  | (2) | Specifies which **Features** are on either side of each **Face** |
| **Face Attribute Array** | FaceNormals | double | (3) | Specifies the normal of each **Face** |
| **Face Attribute Array**  | FaceAreas | double | (1) | Specifies the area of each **Face** |
| **Feature Attribute Array**  | AvgEulerAngles | float | (3) | Three angles defining the orientation of the **Feature** in Bunge convention (Z-X-Z). |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)

