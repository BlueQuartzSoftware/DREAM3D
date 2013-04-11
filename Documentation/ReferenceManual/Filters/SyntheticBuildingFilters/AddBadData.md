
Add Bad Data {#addbaddata}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Misc)

## Description ##
This filter adds "bad" data to a volume.  This filter is intended to add "realism" to synthetic structures that don't have any "bad" **Cells**.  The user can choose to add Poisson (i.e. random) "noise" and "noise" along **Field** boundaries.  If they choose to add either kind of "noise", the user must then set the volume fraction of **Cells** to set as "bad".

## Parameters ##
| Name | Type |
|------|------|
| Add Poisson Noise | Boolean |
| Volume Fraction Poisson Noise | float |
| Add Boundary Noise | Boolean |
| Volume Fraction Boundary Noise | float |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | GBEuclideanDistances |  |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



