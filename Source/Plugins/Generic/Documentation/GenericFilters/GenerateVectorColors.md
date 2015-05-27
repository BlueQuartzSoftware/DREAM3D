Generate Vector Colors {#generatevectorcolors}
=============

## Group (Subgroup) ##
Generic Filters (Misc)

## Description ##
This Filter generates a color for each **Element** based on the vector assigned to that **Element** in the input vector data array.  The color scheme assigns a unique color to all points on the unit hemisphere using a HSV-like scheme. 

## Parameters ##
| Name             | Type | Description |
|------------------|------|---------|
| Apply to Good Voxels Only (Bad Voxels Will Be Black) | Boolean | Whether or not to assign colors to *bad* voxels or leave them black |

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | VectorData | Float | (3) | Vectors the colors will represent  |
| Element | GoodVoxels | Boolean | (1) | Used to define **Elements** as *good* or *bad*  |


## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | Colors | Unsigned Char | (3) | The RGB colors |


### Authors ###
**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:** See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


