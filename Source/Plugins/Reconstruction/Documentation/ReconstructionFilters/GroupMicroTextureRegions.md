Group MicroTexture Regions (C-Axis Misorientation) {#groupmicrotextureregions}
======

## Group (Subgroup) ##
Reconstruction Filters (Grouping)

## Description ##
This Filter groups neighboring **Features** that have c-axes aligned within a user defined tolerance.  The algorithm for grouping the **Features** is analogous to the algorithm for segmenting the **Features** - only the average orientation of the **Features** are used instead of the orientations of the individual **Cells** and the criterion for grouping only considers the alignment of the c-axes.  The user can specify a tolerance for how closely aligned the c-axes must be for neighbor **Features** to be grouped.


NOTE: This filter is intended for use with *Hexagonal* materials.  While the c-axis is actually just referring to the <001> direction and thus will operate on any symmetry, the utility of grouping by <001> alignment is likely only important/useful in materials with anisotropy in that direction (like materials with *Hexagonal* symmetry).


## Parameters ##

| Name | Type |
|------|------|
| C-Axis Alignment Tolerance | Float |
| Use Running Average | Boolean |

## Required DataContainers ##
Volume

## Required Objects ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | Volumes | | | |
| Feature | AvgQuats | Four (4) values (floats) defining the average orientation of the **Feature** in quaternion representation | Filter will calculate average quaternions for **Features** if not already calculated. | Find Feature Average Orientations (Statistics) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Objects ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | Active | Boolean value specifying if the **Feature** is still in the sample (1 if the **Feature** is in the sample and 0 if it is not). | At the end of the filter, all **Features** will be "Active" as the "Inactive" **Features** will have been removed.  |
| Feature | ParentIds | List of grouped microtexture region **Features**. |  |
| Cell | ParentIds | List of grouped microtexture region **Cells**.  |  |
| Attribute Matrix | NewCellFeatureAttributeMatrixName | | |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


