Find Projected Image Statistics {#findprojectedimagestatistics}
======

## Group (Subgroup) ##
Processing Filters (Image)

## Description ##
This filter calculates statistics (minimum, maximum, average, standard deviation and variance) for each "stack" of **Cells** perpendicular to a user selected plane of interest (XY, XZ, YZ).  Each **Cell** in the "stack" is then assigned the statistic(s) for the whole stack.  
For example, if there was a 100x200x300 **Cell** volume and the plane of interest was selected as "XY", then a 100x200 temp "image" would be created.  Each **Cell** in the temp "image" would represent a 300 **Cell** "stack"in the z-direction.  The statistics listed previously would be calculated for each 300 **Cell** "stack" and stored at the corresponding **Cell** in the 100x200 temp "image".  Finally, the values for each of the statistics stored in the temp "image" are assigned to all 300 **Cells** in the "stack" that they describe, which is equivalent to "extruding" the 100x200 temp "image" 300 *Cells** in the z-direction. 

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Plane of Interest | Selection | Tells the filter which plane (XY, XZ, YZ) to project perpendicular to |

## Required Geometry ##
Image/Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Cell | None | Any, except Boolean | (1,1) | Any scalar array the user would like to calculate the projected statistics of |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Cell | ProjectedImageMin | Float | (1,1) | The minimum value of the scalar array along the stack of voxels perpendicular to the plane of interest |
| Cell | ProjectedImageMax | Float | (1,1) | The maximum value of the scalar array along the stack of voxels perpendicular to the plane of interest |
| Cell | ProjectedImageAvg | Float | (1,1) | The average value of the scalar array along the stack of voxels perpendicular to the plane of interest |
| Cell | ProjectedImageStd | Float | (1,1) | The standard deviation of the scalar array along the stack of voxels perpendicular to the plane of interest |
| Cell | ProjectedImageVar | Float | (1,1) | The variance of the scalar array along the stack of voxels perpendicular to the plane of interest |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

