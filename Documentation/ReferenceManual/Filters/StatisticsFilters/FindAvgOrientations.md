Find Field Average Orientations {#findavgorientations}
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

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Comment | |
|------|--------------|---------|--|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation | Filter will calculate the quaternion for each **Cell** if it is not already calculated. |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##

| Type | Default Name | Comment |  |
|------|--------------|---------|--|
| Field | AvgQuats | Five (5) values (floats) defining the average orientation of the **Field** in quaternion representation | |
| Field | FieldEulerAngles | Three (3) angles (floats) defining the orientation of each **Field** in Bunge convention (Z-X-Z) | The first value is a dummy value, so each **Field** has quat = {dummy, q1, q2, q3, q4} - where q1, q2, and q3 contain the axis information and q4 contains the angle information of the quaternion |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



