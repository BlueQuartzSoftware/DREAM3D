Find Average Value of Scalars For Feature {#findavgscalarvalueforfeatures}
======

## Group (Subgroup) ##
Statistics Filters (Misc)

## Description ##
This Filter determines the average value of a *scalar array* for each **Feature** by the following algorithm:

1. Gather all **Elements** that belong to the **Feature**
2. Average the values of the *scalar array* for all **Elements** and store as the average for the **Feature**

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |
| Element | None | Variable | (1) | Scalar array to average for each **Feature** |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | None | Float | (1) | Average value of scalar array for each **Feature** |


## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

