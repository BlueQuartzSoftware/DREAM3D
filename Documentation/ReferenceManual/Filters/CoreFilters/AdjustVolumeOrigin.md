Adjust Origin  {#adjustvolumeorigin}
==============================

## Group (Subgroup) ##
Generic Filters (Spatial)

## Description ##
This filter changes the origin of the volume. For example if the current origin is
at (0,0,0) and the user would like the origin to be (10, 4, 8) then the user should enter the following input values:

+ X Origin: 10
+ Y Origin: 4
+ Z Origin: 8

## Parameters ##

| Name    | Type      |  Notes |
|---------|-----------|--------|
|  Origin | 3 x Float |        |
| Apply to Voxel Volume | Bool | Should the new origin be applied to the Voxel Volume |
| Apply to SurfaceMesh Volume | Bool | Should the new origin be applied to the Surface Mesh |

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

