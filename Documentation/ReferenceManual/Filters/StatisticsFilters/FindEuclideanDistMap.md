Find Euclidean Distance Map {#}
======
<h1 class="pHeading1">Find Euclidean Distance Map Filter</h1>
<p class="pCellBody">
<a href="../StatisticsFilters/FindEuclideanDistMap.html#wp2">Description</a> | <a href="../StatisticsFilters/FindEuclideanDistMap.html#wp3">Options</a> | <a href="../StatisticsFilters/FindEuclideanDistMap.html#wp4">Required Arrays</a> | <a href="../StatisticsFilters/FindEuclideanDistMap.html#wp5">Created Arrays</a> | <a href="../StatisticsFilters/FindEuclideanDistMap.html#wp1">Authors</a> 

Statistics Filters


This Filter utilizes the __Cell GrainIds__ to find __Cell NearestNeighborDistances__ and the __Cell NearestNeighbors__.


## Parameters ## 

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which feature/grain each voxel/datapoint belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | NearestNeighborDistances |  |
| Cell | NearestNeighbors |  |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



