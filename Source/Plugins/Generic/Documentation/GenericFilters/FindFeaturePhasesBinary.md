Find Feature Phases Binary {#findfeaturephasesbinary}
==========

## Group (Subgroup) ##

Generic Filters (Misc)

## Description ##

This filter assigns a phase number to binary data. The 'true' **Cells** will be phase 1, and 'false' **Cells** will be phase 0. It is generally for use when the cell phases weren't known ahead of time. For example, if an image is segmented into precipitates and non-precipitates, this filter will assign the precipitates to phase 1, and the non-precipitates to phase 0.

Additionally, this filter creates a Ensemble Attribute Matrix to hold ensemble data. 

## Parameters ##
None

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | GoodVoxels | Boolean | (1) | Specifies if the **Cell** is to be counted as a phase or not |
| Cell | FeatureIds | Int | (1) | Ids that specify to which **Feature** each **Cell** belongs. |


## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | FeaturePhases | Int | (1) | Id specifying the phase of the **Feature** |


## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

