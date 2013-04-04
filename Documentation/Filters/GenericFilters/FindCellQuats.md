Find Cell Quaternions {#findcellquats}
=======

## Group ##
Generic Filters

## Description ##
This filter determines the _quaternions_ for each voxel/datapoint, given the _Euler_ angles for the voxel/datapoint. 
The _quaternions_ calculated are rotated such that they all lie within the _Fundamental Zone (FZ) _ nearest to the origin of orientation space for the crystal structure of the voxel/datapoint's phase.
The FZ is the region of orientation space that contains general positions on which the symmetry operations of the structure cannot operate to generate other positions within the region that are symmetrically equivalent. 
 


## Parameters ## 

## Required Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Cell | CellEulerAngles | Three (3) angles (floats) defining the orientation of the voxel/datapoint in Bunge convention (Z-X-Z) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | CellPhases | Phase Id (int) specifying the phase of the voxel/datapoint | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Cell | Quats | Five (5) values (floats) that specify the orientation of the voxel/datapoint in quaternion representation | The first value is a dummy value, so each voxel/datapoint has quat = {dummy, q1, q2, q3, q4} - where q1, q2, and q3 contain the axis information and q4 contains the angle information of the quaternion |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



