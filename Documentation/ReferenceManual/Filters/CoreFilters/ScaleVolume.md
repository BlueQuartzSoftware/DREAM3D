Scale Volume {#scalevolume}
==============================

## Group (Subgroup) ##
Generic Filters (Spatial)

## Description ##
This filter scales the Volume by the factors entered by the user. For example if a volume has a range
of 10 to 30 in the X direction and the user enters a **ScaleFactor** for X of 3, then the new range will be 10 to 90. Note
that the origin will NOT change with this filter.

## Parameters ##

| Name    | Type      |  Notes |
|---------|-----------|--------|
| Scaling | 3 x Float | Scale Factor     |
| Apply to Voxel Volume | Bool | Should the new scaling be applied to the Voxel Volume |
| Apply to SurfaceMesh Volume | Bool | Should the new scaling be applied to the Surface Mesh |

## Required DataContainers ##

Voxel: If being applied to the Voxel Volume
SurfaceMesh: If being applied to the SurfaceMesh

## Required Arrays ##

None

## Created Arrays ##

None

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

