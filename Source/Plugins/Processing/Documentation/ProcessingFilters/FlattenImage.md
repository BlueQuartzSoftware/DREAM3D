Flatten Image {#flattenimage}
======

## Group (Subgroup) ##
Processing Filters (Image)

## Description ##
This filter allows the user to selected a "flattening" method for turning an array of RGB or RGBa values to grayscale values.  The options available in this filter are "Average" and "Luminosity".  If "Average" is selected, then the R, G and B values are averaged to get a single grayscale value.  If "Luminosity" is selected, then the grayscale value equals (R*0.21)+(G*0.72)+(B*0.07).

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Flattening Method | Selection | Tells filter which method to use when flattening the RGB array.

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Element | ImageData | Float | (1,3) or (1,4) | RGB or RGBa values |

## Created Arrays ##
| Type | Default Name | Description | Comment |
|------|--------------|---------|-------|
| Element | FlatImageData | Float | (1,1) | Grayscale values |


## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.


See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

