# Find Local Average C-Axis Misalignments  #


## Group (Subgroup) ##

Statistics (Crystallographic)

## Description ##

This filter finds parent average feature kernel average c-axis misalignment.  Alternatively, if the "Calculate Unbiased Local Average C-Axis Misalignment" parameter is TRUE, the parent average sub-boundary misalignment is calculated.


## Parameters ##

| Name | Type |
|------|------|
| Calculate Unbiased Local Average C-Axis Misalignments | Boolean |
| Calculate Local Average C-Axis Misalignments | Boolean |


## Required DataContainers ##

Volume

## Required Objects ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | ParentIds | List of grouped "parents" of features region **Cells**.  |  |
| Feature | ParentIds | List of grouped "parents" of features region **Features**. |  |
| Feature | CAxisMisalignmentList | NeighborList of c-axis misalignments. | Only needed if "Calculate Local Average C-Axis Misalignments" IS checked. | Find Field Neighbor C-Axis Misalignments (Statistics) |  
| Feature | NeighborList | List of grain nearest neighbors. | Only needed if "Calculate Unbiased Local Average C-Axis Misalignments" IS checked. | Find Neighbors (Statistics) |

| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Objects ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | NumFeaturesPerParent | List of number of features associated with each ParentId. |  |
| Feature | LocalCAxisMisalignments| Float of the parent region average "feature"-kernel average c-axis misalignment. | This is only calculated if  "Calculate Local Average C-Axis Misalignments" IS NOT checked|  
| Feature | UnbiasedLocalCAxisMisalignments| Float of the parent region average sub-boundary c-axis misalignment. | This is only calculated if  "Calculate Unbiased Local Average C-Axis Misalignments" IS checked|  



## Example Pipelines ##



## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


