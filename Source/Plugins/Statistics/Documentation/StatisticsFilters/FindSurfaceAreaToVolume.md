Find Surface Area To Volume {#findsurfaceareatovolume}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This filter calculates the ratio of surface area to volume for each feature. 




## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | Centroids | X, Y, Z coordinates (floats) of **Feature** center of mass | Filter will calculate **Feature** centroids if not previously calculated | Find Feature Centroids (Generic) |
| Feature | Volumes | Volume (float) in um^3 of the **Feature**. | Filter will calculate volume of each **Feature** if not already calculated | Find Feature Sizes (Statistics) | 
| Feature | AxisEulerAngles |  |  |  |
| Feature | AxisLengths |  |  |  |
## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|


## Authors ##

**Copyright:** 

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

