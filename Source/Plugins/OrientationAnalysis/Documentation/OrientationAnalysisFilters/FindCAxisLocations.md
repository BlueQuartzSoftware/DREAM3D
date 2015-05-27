Find C-Axis Locations {#findcaxislocations}
======

## Group (Subgroup) ##
Orientation Analysis Filters (Statistics)

## Description ##
This Filter determines the direction <u,v,w> of the c-axis for each **Element** by applying the quaternion of the **Element** to the <001> direction, which is the c-axis for *Hexagonal* materials.  This will tell where the c-axis of the **Element** sits in the *sample reference frame*.

Note: This filter will only work properly for *Hexagonal* materials.  The filter does not apply any symmetry operators because there is only one c-axis (<001>) in *Hexagonal* materials and thus all symmetry operators will leave the c-axis in the same position in the sample *reference frame*.  However, in *Cubic* materials, for example, the {100} family of directions are all equivalent and the <001> direction will change location in the *sample reference frame* when symmetry operators are applied. 

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | Quats | Float | (4) | Specifies the orientation of the **Element** in quaternion representation |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | CAxisLocation | Float | (3) | Direction <u,v,w> of the c-axis for each **Element** in the sample reference frame |

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


