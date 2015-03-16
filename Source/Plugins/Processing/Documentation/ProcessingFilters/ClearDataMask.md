Clear Data (Mask) {#cleardatamask}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This filter clears all attribute arrays for each **Element** that have a value of "false" in the "mask" array.  All **Elements** that have the value of "true" in the "mask" array will retain all their attributes.

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Cell | GoodVoxels | Boolean | (1,1) | Used to define whether the **Cells** are part of the mask  |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


