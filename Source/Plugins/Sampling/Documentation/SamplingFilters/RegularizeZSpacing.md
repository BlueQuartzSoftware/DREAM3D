Regularize Z Spacing {#regularizezspacing}
======

## Group (Subgroup) ##
Sampling Filters (Resolution)

## Description ##
This filter "resamples" the 3D array in the *Z*-direction (which is generally taken as the sectioning direction).  The *Image/Rectilinear Grid* geometry assumes all **Cells** are of the same size(dX x dY x dZ).  If the *Z* locations (i.e. slice positions) are not uniform, then the user can enter a list of the actual *Z* locations and a desired uniform "resolution".  The filter will then create a new volume with uniform *Z* locations and determine which of the old "slices" should be copied to the new "slices". Below is an example:

If a dataset had 10 sections with the following locations:

|Old Slice # |Old Slice Location|
|---------|---------|
|0|0|
|1|3|
|2|7|
|3|12|
|4|14|
|5|19|
|6|24|
|7|25|
|8|30|
|9|34|

and the user wanted a uniform resolution of *3*, then the following mapping would occur 

|New Slice # | New Slice Location |Old Slice # Used|
|---------|---------|---------|
|0|0|0|
|1|3|1|
|2|6|2|
|3|9|2|
|4|12|3|
|5|15|4|
|6|18|5|
|7|21|5|
|8|24|6|
|9|27|7|
|10|30|8|
|11|33|9|

Note: there is no interpolation of the data and the nearest "old slice" is copied to a given "new slice".

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Current Z Positions File | File Path | Location of the Z positions file |
| Z Res | Double | Uniform resolution desired in the Z direction of the new grid |

## Required Geometry ##
Image/Rectilinear Grid

## Required Arrays ##
None

## Created Arrays ##
None

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

