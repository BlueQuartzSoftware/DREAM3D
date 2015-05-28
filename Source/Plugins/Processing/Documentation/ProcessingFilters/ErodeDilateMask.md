Erode/Dilate Mask {#erodedilatemask}
=============

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
If the mask is _dilated_, the Filter grows the *true* regions by one **Cell** in an iterative sequence for a user defined number of iterations.  During the *dilate* process the classification of any **Cell** neighboring a *false* **Cell** will be changed to *true*.  
If the mask is _eroded_, the filter shrinks the *true* regions by one **Cell** in an iterative sequence for a user defined number of iterations.  During the *erode* process the classification of the *false* **Cells** is changed to *true* if one of its neighbors is *true*.
The Filter also offers the option(s) to turn on/off the erosion or dilation in specific directions (x, y or z).

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Operation | Selection | Tells Filter whether to dilate or erode |
| Number of Iterations | Int | The number of iterations to use for the Filter |
| XDirOn | Boolean | Tells the Filter whether to erode/dilate in the X direction or not |
| YDirOn | Boolean | Tells the Filter whether to erode/dilate in the Y direction or not |
| ZDirOn | Boolean | Tells the Filter whether to erode/dilate in the Z direction or not |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | GoodVoxels | Boolean | (1) | Used to define **Cells** as *good* or *bad*  |

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


