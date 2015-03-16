Generate Vector Colors {#generatevectorcolors}
========

## Group (Subgroup) ##

Generic Filters (Misc)

## Description ##

This filter generates a color for each **Element** based on the vector assigned to that **Element** in the input vector data array.  The color scheme assigns a unique color to all points on the unit hemisphere using a HSV-like scheme. 

## Parameters ##

| Name             | Type | Description |
|------------------|------|---------|
| UseGoodVoxels | Boolean | Whether or not to assign colors to "bad" voxels or leave them black |

## Required Geometry ##

Not Applicable

## Required Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | VectorData | Float | (3) | Vectors the colors will represent  |
| Element | GoodVoxels | Boolean | (1) | States if the **Element** is a 'good' **Element**  |


## Created Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | Colors | Unsigned Char | (3) | The RGB colors |


### Authors ###


**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:** See the License.txt file that came with DREAM3D.

See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

