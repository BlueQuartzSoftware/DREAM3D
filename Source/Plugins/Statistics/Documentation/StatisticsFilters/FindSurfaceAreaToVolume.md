Find Surface Area To Volume {#findsurfaceareatovolume}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This filter calculates the ratio of surface area to volume for each feature for a voxel-based structure. First all the boundary cells are found for each feature. Next the surface area for each face that is in contact with a different feature is totaled. This is divided by the volume of each feature, calculated by taking the number of cells of each feature and multiplying by the resolution. 


## Parameters ##
| Name             | Type | Comments |
|------------------|------|-----|
| SurfaceAreaVolumeRatio | String | This is the name of the data array calculated by this filter |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

| Feature | NumCells | Number of cells (int) of each **Feature**. |  | Find Feature Sizes (Statistics) | 

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | SurfaceAReaVolumeRatio | This is the ratio of surface area to volume for each feature. The units are in inverse length units (float) |  |


## Authors ##

**Copyright:** 

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

