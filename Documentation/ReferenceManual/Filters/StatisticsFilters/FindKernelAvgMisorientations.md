Find Local Misorientation Gradients {#}
======
<h1 class="pHeading1">Find Local Misorientation Gradients Filter</h1>
<p class="pCellBody">
<a href="../StatisticsFilters/FindLocalMisorientationGradients.html#wp2">Description</a> | <a href="../StatisticsFilters/FindLocalMisorientationGradients.html#wp3">Options</a> | <a href="../StatisticsFilters/FindLocalMisorientationGradients.html#wp4">Required Arrays</a> | <a href="../StatisticsFilters/FindLocalMisorientationGradients.html#wp5">Created Arrays</a> | <a href="../StatisticsFilters/FindLocalMisorientationGradients.html#wp1">Authors</a> 

Statistics Filters


This Filter utilizes the __Cell CellPhases, GrainIds__, and __Quats__ to generate __GrainMisorientations, KernelAvgMisorientations__, and __MisorientationGradients__. For the __Field__, it uses the __Field AvgQuats__ data to generate __Field GrainAvgMisorientations__. The user must enter a _Kernel Size_ as an integer.


## Parameters ##

| Name | Type |
|------|------|
| Kernel Size | Integer |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation | Filter will calculate the quaternion for each **Cell** if it is not already calculated. |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | KernelAverageMisorientations |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



