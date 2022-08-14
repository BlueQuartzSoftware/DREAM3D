# Partition Geometry #


## Group (Subgroup) ##

Reconstruction (Reconstruction)

## Description ##

This **Filter** splits a given geometry into partitions using the partition's X, Y, and Z bounds, grid dimensions, origin, and spacing.

If the **Filter** determines that any voxel of a partition is out-of-bounds compared to the original geometry, the **Out-Of-Bounds Default Value** will be used as the partition ID in the output partition IDs array.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Partition X Bounds | FloatVec2 | The min/max values in the X dimension for each partition in the partitioning grid |
| Partition Y Bounds | FloatVec2 | The min/max values in the Y dimension for each partition in the partitioning grid |
| Partition Z Bounds | FloatVec2 | The min/max values in the Z dimension for each partition in the partitioning grid |
| Partition Grid Dimensions | IntVec3 | The dimensions of the partitioning grid |
| Out-Of-Bounds Default Value | int | The default value used when any voxel in a partition is out-of-bounds compared to the original geometry |
| Origin | FloatVec3 | The origin of the partitioning grid |
| Spacing | FloatVec3 | The spacing for each partition in the partitioning grid |

## Required Geometry ##

Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Matrix** | Cell Attribute Matrix | Cell | N/A | The cell attribute matrix whose data container contains the geometry to be partitioned.  This attribute matrix is used to store the output partition IDs array. |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Element/Feature/Ensemble/etc. Attribute Array** | Partition Ids | int32_t | (1) | The array containing the partition id at each voxel or node in the original geometry |


## Example Pipelines ##

None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users