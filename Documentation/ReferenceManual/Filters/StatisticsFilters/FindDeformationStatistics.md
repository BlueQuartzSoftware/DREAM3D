Find Deformation Statistics {#}
======
<h1 class="pHeading1">Find Deformation Statistics Filter</h1>
<p class="pCellBody">
<a href="../StatisticsFilters/FindDeformationStatistics.html#wp2">Description</a> | <a href="../StatisticsFilters/FindDeformationStatistics.html#wp3">Options</a> | <a href="../StatisticsFilters/FindDeformationStatistics.html#wp4">Required Arrays</a> | <a href="../StatisticsFilters/FindDeformationStatistics.html#wp5">Created Arrays</a> | <a href="../StatisticsFilters/FindDeformationStatistics.html#wp1">Authors</a> 

Statistics Filters


This Filter finds the _deformation statistics_ by using data from the __Cells__ and the __Fields__. 
The Outputs are a Deformation Statistics file and a VTK file.

## Parameters ## 

| Name | Type |
|------|------|
| Deformation Statistics File | Output File |
| VTK Output File | Output File |

## Required Arrays ##



| Cell | GrainIds |  |
| Cell | GrainMisorientations |  |
| Cell | KernelAverageMisorientations |  |
| Cell | MisorientationGradients |  |
| Cell | NearestNeighborDistances |  |
| Cell | NearestNeighbors |  |
| Field | AvgQuats |  |
| Field | FieldPhases |  |
| Field | GrainAvgMisorientations |  |
| Field | Poles |  |
| Field | Schmids |  |
| Field | SlipSystems |  |

## Created Arrays ##
None

## Authors ##


**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



