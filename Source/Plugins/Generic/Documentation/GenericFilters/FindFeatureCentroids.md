Find Feature Centroids {#findfeaturecentroids}
=======

## Group (Subgroup) ##
Generic Filters (Misc)

## Description ##
This filter calculates the _centroid_ of each **Feature** by determining the average x, y, and z position of all the **Cells** belonging to the **Feature**.
Note that **Features** that intersect the outer surfaces of the sample will still have _centroids_ calculated, but they will be _centroids_ of the truncated part of the **Feature** that lies inside the sample.


## Parameters ##

None

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | Centroids | X, Y, Z coordinates (floats) of **Feature** center of mass |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

