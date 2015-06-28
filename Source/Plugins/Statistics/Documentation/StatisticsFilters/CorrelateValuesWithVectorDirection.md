Correlate Values with Vector Direction {#correlatevalueswithvectordirection}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This Filter determines the average c-axis location of each **Feature** by the following algorithm:

1. Gather all **Cells** that belong to the **Feature**
2. Determine the location of the c-axis in the sample *reference frame* for the rotated quaternions for all **Cells**.

3.  average the locations and store as the average for the **Feature**.

Note: This filter will only work properly for *Hexagonal* materials.  The filter does not apply any symmetry operators because there is only one c-axis (<001>) in *Hexagonal* materials and thus all symmetry operators will leave the c-axis in the same position in the sample *reference frame*.  However, in *Cubic* materials, for example, the {100} family of directions are all equivalent and the <001> direction will change location in the sample *reference frame* when symmetry operators are applied. 

 

## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Objects ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation | Filter will calculate the quaternion for each **Cell** if it is not already calculated. | Find Cell Quaternions (Generic) |

## Created Objects ##

| Type | Default Name | Description | Comment |
|------|--------------|---------|--------|
| Feature | AvgCAxes | This array is the direction <u,v,w> of the c-axis in the sample reference frame |  |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


