Find Field Average C-Axes {#findavgcaxes}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This Filter determines the average c-axis location of each **Field** by the following algorithm:

1. Gather all **Cells** that belong to the **Field**
2. Determine the location of the c-axis in the sample *reference frame* for the rotated quaternions for all **Cells**.
3.  average the locations and store as the average for the **Field**.

Note: This filter will only work properly for *Hexagonal* materials.  The filter does not apply any symmetry operators because there is only one c-axis (<001>) in *Hexagonal* materials and thus all symmetry operators will leave the c-axis in the same position in the sample *reference frame*.  However, in *Cubic* materials, for example, the {100} family of directions are all equivalent and the <001> direction will change location in the sample *reference frame* when symmetry operators are applied.  
 

## Parameters ##
None

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | GrainIds |  |
| Cell | Quats |  |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|---------|--------|
| Field | AvgCAxes | This array is the direction <u,v,w> of the c-axis in the sample reference frame |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



