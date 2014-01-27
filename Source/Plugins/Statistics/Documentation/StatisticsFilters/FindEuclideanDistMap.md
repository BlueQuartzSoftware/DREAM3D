Find Euclidean Distance Map {#findeuclideandistmap}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter calculates the distance of each **Cell** from the nearest **Feature** boundary, triple line and quadruple point.  The following algorithm explains the process:

1. Find the **Feature** that owns each **Cell**
2. Find the number of different **Features** that own the six face-face neighbors of each **Cell**
3. For all **Cells** that have *at least 2* different neighbors, set their *GBEuclideanDistance* to *0*.  For all **Cells** that have *at least 3* different neighbors, set their *TJEuclideanDistance* to *0*.  For all **Cells** that have *at least 4* different neighbors, set their *QPEuclideanDistance* to *0*.
4. For each of the three *EuclideanDistace* maps, iteratively "grow" out from the **Cells** identified to have a distance of *0* by the following sub-steps:

  - Determine the **Cells** that neighbor a **Cell** of distance *0* in the current map.
  - Assign a distance of *1* to those **Cells**.
  - Repeat previous two sub-steps, increasing the distances by *1* each iteration, until no **Cells** remain without a distance assigned.



Note : the distances calculated are "city-block" distances and not "shortest distance" distances.



## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | NearestNeighbors |  |  |
| Cell | GBEuclideanDistances |  |  |
| Cell | TJEuclideanDistances |  |  |
| Cell | QPEuclideanDistances |  |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

