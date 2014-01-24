Flatten Image {#flattenimage}
======

## Group (Subgroup) ##
Processing Filters (Image)

## Description ##
This filter allows the user to selected a "flattening" method for turning an array of RGB or RGBa values to grayscale values.

## Parameters ##

| Name | Type |
|------|------|
| Flattening Method | Choices |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | ImageData | This is either a 3 or 4 component array of RGB or RGBa values |  | Import Images (3D Stack) (ImageImport) |


## Created Arrays ##
| Type | Default Name | Description | Comment |
|------|--------------|---------|-------|
| Cell | FlatImageData | This is a 1 component array of grayscale values | |


## Authors ##


**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.


See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

