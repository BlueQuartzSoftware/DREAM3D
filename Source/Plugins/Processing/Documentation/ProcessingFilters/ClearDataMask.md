Clear Data (Mask) {#cleardatamask}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This filter allows the user to define a subvolume of the dataset in which the filter will erase any "true's" in the "mask" array and replace them with "false's".

## Parameters ##
| Name | Type | Description |
|------|------|------|
| X Min | Integer | Minimum X bound in voxels |
| Y Min | Integer | Minimum Y bound in voxels |
| Z Min | Integer | Minimum Z bound in voxels |
| X Max | Integer | Maximum X bound in voxels |
| Y Max | Integer | Maximum Y bound in voxels |
| Z Max | Integer | Maximum Z bound in voxels |

## Required Geometry ##
Image/Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Cell | GoodVoxels | Boolean | (1,1) | Used to define whether the **Cells** are part of the mask  |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


