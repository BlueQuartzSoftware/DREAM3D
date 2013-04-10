Find Euclidean Distance Map {#findeuclideandistmap}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter utilizes the __Cell GrainIds__ to find __Cell NearestNeighborDistances__ and the __Cell NearestNeighbors__.


## Parameters ##

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | NearestNeighborDistances |  |  |
| Cell | NearestNeighbors |  |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



