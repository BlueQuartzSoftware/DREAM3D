Generate Vector Colors {#generatevectorcolors}
========

## Group (Subgroup) ##
Generic Filters (Misc)

## Description ##
This filter generates a color for each **Element** based on the vector assigned to that **Element** in the input vector data array.  The color scheme assigns a unique color to all points on the unit hemisphere using a HSV-like scheme. 

## Input Options ##
| Option | Type |
|-------|-------|
| UseGoodVoxels | bool |

## Required Geometry ##
Not Applicable

## Required Arrays ##

| Type | Default Name | Description  | Filters Known to Create Data |
|------|--------------|------------|-----|
| Element | VectorData | The array that holds the vectors the colors will represent  |
| Element | GoodVoxels | The array that holds the boolean value stating if the **Element** is a 'good' **Element**  |


## Created Arrays ##
| Type | Name | Description |
|------|------|-------------|
| Element | Colors | The RGB colors are encoded as an unsigned char triplet  |


### Authors ###


**Copyright:** 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:** See the License.txt file that came with DREAM3D.

See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

