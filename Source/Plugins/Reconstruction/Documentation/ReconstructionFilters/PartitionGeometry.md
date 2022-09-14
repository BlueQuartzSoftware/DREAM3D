# Partition Geometry #

## Group (Subgroup) ##

Reconstruction (Reconstruction)

## Description ##

This **Filter** splits a given geometry into partitions using the partition's X, Y, and Z bounds, grid dimensions, origin, and spacing.

If the **Filter** determines that any voxel or node of a partition is out-of-bounds compared to the original geometry, the **Out-Of-Bounds Partition ID** will be used as the partition ID in the output partition IDs array.

There are three available schemes to choose from:
+ **Basic** - Just provide the starting ID and the number of partitions per axis, and the filter will automatically create a partitioning scheme that fits your geometry data.
+ **Advanced** - Provide the starting ID, out-of-bounds ID, number of partitions per axis, partitioning scheme origin, and length per partition, and the filter will create a partitioning scheme using these inputs.  It IS possible, using this scheme, to create a partitioning scheme that does not completely fit the input geometry.
+ **Bounding Box** - Provide the starting ID, number of partitions per axis, lower left coordinate (**ll**), and upper right coordinate (**ur**), and the filter will create a partitioning scheme that has bounds [**ll**,**ur**).  It IS possible, using this scheme, to create a partitioning scheme that does not completely fit the input geometry.

**Note**: Voxels/nodes that are exactly on the edge between two partitions will be labeled with the upper partition's ID, or if they are on the upper edge of the partitioning scheme, they will be labeled with the **Out-Of-Bounds Partition ID**.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Partitioning Scheme | int | The scheme used to partition the geometry.  The choices are Basic, Advanced, and Bounding Box. |
| Starting Partition ID | int | The starting ID to use when creating the partitioning scheme. |
| Out-Of-Bounds Partition ID | int | The default partition ID used when the voxel or node in a partition is out-of-bounds compared to the original geometry. Only available in Advanced and Bounding Box schemes. |
| Number of Partitions Per Axis (X,Y,Z) | IntVec3 | The number of partitions that will be created in each axis. |
| Lower Left Coordinate (X,Y,Z) | FloatVec3 | The lower left coordinate for the bounding box of the partitioning scheme. Only available in Bounding Box scheme. |
| Upper Right Coordinate (X,Y,Z) | FloatVec3 | The upper right coordinate for the bounding box of the partitioning scheme. Only available in Bounding Box scheme. |
| Partitioning Scheme Origin (X,Y,Z) | FloatVec3 | The origin of the partitioning scheme. Only available in Advanced scheme. |
| Length Per Partition (X,Y,Z) | FloatVec3 | The length in each axis for each partition. Only available in Advanced scheme. |
| Save Partitioning Scheme As Image Geometry | bool | Determines whether or not to save the partitioning scheme as an image geometry |

## Required Geometry ##

Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Matrix** | Attribute Matrix | Cell/Vertex/Edge/Face | N/A | The attribute matrix whose data container contains the geometry to be partitioned.  This attribute matrix is used to store the output partition IDs array. |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Element/Feature/Ensemble/etc. Attribute Array** | PartitioningSchemeIds | int32_t | (1) | The array containing the partition id at each voxel or node in the original geometry |


## Example Pipelines ##

None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users