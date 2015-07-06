Find Ellipsoid Error {#findellipsoiderror}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
Between the *Find Shapes* and *Find Centroids* filters, an ideal ellipsoid for each **Feature** can be described by the centroids, axis lengths and angles of rotation into the sample reference frame. This filter calculates the fraction of cells of each feature that lie outside the ideal ellipse described by those parameters. 

**Currently, this filter only works on 2D Data. **


## Parameters ##
| Name             | Type | Comments |
|------------------|------|-----|
| Write Ideal Ellipse Feature Ids | Bool | This writes out a cell array containing the ideal feature IDs. This is only for visualization, and can be a lengthy calculation |

## Required Objects ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | Centroids | X, Y, Z coordinates (floats) of **Feature** center of mass |  | Find Feature Centroids (Generic) |
| Feature | NumCells | Number of cells (int) of each **Feature**. |  | Find Feature Sizes (Statistics) | 
| Feature | AxisEulers | The euler angles in radians (3 component - float) required to rotate the ellipsoid into the sample reference frame  **Feature**. |  | Find Feature Shapes (Statistics) | 
| Feature | AxisLengths | Length in microns (3 component - float) of each axis of the ellipsoid. They are ordered from largest to smallest **Feature**. |  | Find Feature Shapes (Statistics) | 

## Created Objects ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | EllipsoidError | This is the fraction of cells of a feature that lie outside the ideal ellipse calculated for that feature |  |
| Cell | IdealFeatureIds | This assigns each cell the feature number that owns it if each feature was a perfect ellipse. | This is primarily for visualization purposes. Often two ideal ellipses may overlap, and only one feature can own a cell, so it may seem as though some ellipses are cut off. For the actual calculation of the error, the ideal ellipses are not cut off.  |



## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


