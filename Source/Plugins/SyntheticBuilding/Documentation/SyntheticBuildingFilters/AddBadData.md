
Add Bad Data {#addbaddata}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Misc)

## Description ##
This filter adds "bad" data to a volume.  This filter is intended to add "realism" (i.e. more representative of an experimental dataset) to synthetic structures that don't have any "bad" **Cells**.  The user can choose to add "random noise" and/or "noise" along **Feature** boundaries.  If they choose to add either kind of "noise", the user must then set the volume fraction of **Cells** to set as "bad".  The volume fractions entered by the user apply to only the set of **Cells** that the "noise" would affect.  For example, if the user chose *0.2* for the volume fraction of boundary "noise", then each boundary **Cell** would have a *20%* chance of being changed to a "bad" **Cell** and all other **Cells** would have a *0%* chance of being changed.


## Parameters ##

| Name | Type |
|------|------|
| Add Random Noise | Boolean |
| Volume Fraction Random Noise | float |
| Add Boundary Noise | Boolean |
| Volume Fraction Boundary Noise | float |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GBEuclideanDistances |  |  | Find Euclidean Distance Map (Statistics) |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

