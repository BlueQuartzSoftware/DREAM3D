Find Field Average C-Axes {#findavgcaxes}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This Filter determines the average orientation of each **Field** by the following algorithm:

1. Gather all **Cells** that belong to the **Field**
2. Using the symmetry operators of the phase of the **Field**, rotate the quaternion of the **Field**'s first **Cell** into the *Fundamental Zone* nearest to the origin. 
3. Rotate each subsequent **Cell**'s quaternion (with same symmetry operators) looking for the quaternion closest to the quaternion selected in Step 2.
4. Average the rotated quaternions for all **Cells** and store as the average for the **Field**

Note: The process of finding the nearest quaternion in Step 3 is to account for the periodicity of orientation space, which would cause problems in the averaging if all quaternions were forced to be rotated into the same *Fundamental Zone* 

Note: The quaternions can be averaged with a simple average because the quaternion space is not distorted like Euler space.  

## Parameters ##
None

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | CellPhases |  |
| Cell | GrainIds |  |
| Cell | Quats |  |
| Ensemble | CrystalStructures |  |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|---------|--------|
| Field | AvgCAxes | This array is the direction <u,v,w> of the c-axis in the sample reference frame |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



