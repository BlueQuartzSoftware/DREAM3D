Find Feature Average C-Axes {#findavgcaxes}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This Filter determines the average c-axis location of each **Feature** by the following algorithm:

1. Gather all **Elements** that belong to the **Feature**
2. Determine the location of the c-axis in the sample *reference frame* for the rotated quaternions for all **Elements**.

3.  Average the locations and store as the average for the **Feature**.

Note: This filter will only work properly for *Hexagonal* materials.  The filter does not apply any symmetry operators because there is only one c-axis (<001>) in *Hexagonal* materials and thus all symmetry operators will leave the c-axis in the same position in the sample *reference frame*.  However, in *Cubic* materials, for example, the {100} family of directions are all equivalent and the <001> direction will change location in the sample *reference frame* when symmetry operators are applied. 

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |
| Element | Quats | Float | (4) | Specifies the orientation of the **Element** in quaternion representation |

## Created Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | AvgCAxes | Float | (3) | The direction <u,v,w> of the **Feature's** c-axis in the sample reference frame |

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

