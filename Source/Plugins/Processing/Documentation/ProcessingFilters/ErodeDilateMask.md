Erode/Dilate Mask {#erodedilatemask}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
If the mask is _dilated_, the filter grows the "true" regions by one **Cell** in an iterative sequence for a user defined number of iterations.  During the *dilate* process the classification of any **Cell** neighboring a "false" Cell** will be changed to "true".  
If the mask is _eroded_, the filter shrinks the "true" regions by one **Cell** in an iterative sequence for a user defined number of iterations.  During the *erode* process the classification of the "false" **Cells** is changed to "true" if one of its neighbors is "true".
The filter also offers the option(s) to turn on/off the erosion or dilation in specific directions (x, y or z).

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Operation | Selection | Tells filter whether to dilate or erode |
| Number of Iterations | Int | The number of iterations to use for the filter |
| XDirOn | Boolean | Tells the filter whether to erode/dilate in the X direction or not |
| YDirOn | Boolean | Tells the filter whether to erode/dilate in the Y direction or not |
| ZDirOn | Boolean | Tells the filter whether to erode/dilate in the Z direction or not |

## Required Geometry ##
Image/Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | GoodVoxels | Boolean | (1) | Used to define **Cells** as "good" or "bad"  |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

