Update Cell Quaternions {#updatecellquats}
=======

## Group (Subgroup) ##
Generic Filters (Crystallography)

## Description ##

Versions of DREAM3D prior to 4.2 used a 5 component Quaternion for code compatibility with other developments codes. Updates to internal structures has removed this need and DREAM3D versions 4.2 and forward use a standard 4 component quaternion (x, y, z, w). This filter updates *quaternion* arrays for each **Cell** that have 5 components to the newer 4 component structure.

#### This filter will replace the current 5 component quaternion with a 4 component representation. The actual data is not changed in this filter. ####

## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description  | Filters Known to Create Data |
|--------|--------------------|-----------------|-----------------------------------------|
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation.  The first value is a dummy value, so each **Cell** has quat = {dummy, x, y, z, w} - where x, y, and z contain the axis information and w contains the angle information of the quaternion | Find Cell Quaternions (Generic) |


## Created Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Cell | Quats | Four (4) values (floats) that specify the orientation of the **Cell** in quaternion representation | A standard quaternion definition of (X, Y, Z, W) |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation Correction)
