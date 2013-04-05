Find Average Orientations {#}
======
<h1 class="pHeading1">Find Average Orientations Filter</h1>
<p class="pCellBody">
<a href="../StatisticsFilters/FindAvgOrientations.html#wp2">Description</a> | <a href="../StatisticsFilters/FindAvgOrientations.html#wp3">Options</a> | <a href="../StatisticsFilters/FindAvgOrientations.html#wp4">Required Arrays</a> | <a href="../StatisticsFilters/FindAvgOrientations.html#wp5">Created Arrays</a> | <a href="../StatisticsFilters/FindAvgOrientations.html#wp1">Authors</a> 


Statistics Filters


This Filter searches the __Cell GrainIds__ to identify their orientations based 
on the associated __Quats__ and __Cell CellPhases__, from which the average of 
the __Cell Quats__ is generated as __Field AvgQuats__. The _Euler_ angles 
of the __Field__s are used in this Filter.


## Parameters ##

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation | Filter will calculate the quaternion for each **Cell** if it is not already calculated. |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Field | AvgQuats | Five (5) values (floats) defining the average orientation of the **Field** in quaternion representation |
| Field | FieldEulerAngles | Three (3) angles (floats) defining the orientation of each **Field** in Bunge convention (Z-X-Z) | The first value is a dummy value, so each **Field** has quat = {dummy, q1, q2, q3, q4} - where q1, q2, and q3 contain the axis information and q4 contains the angle information of the quaternion |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



