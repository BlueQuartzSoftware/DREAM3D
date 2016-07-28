Write Dx File (Feature Ids) {#dxwriter}
=============

## Group (Subgroup) ##

IO (Output)

## Description ##

This **Filter** writes to a Dx file. The file format is that used by the _OpenDX Source Software_ based on IBM's _Visualization Data Explorer_. **Note that the data is written according to the OpenDX specifications where the Z dimension is moving the fastest, then Y, then X.**

### Example Output ###
The .dx file:
```
	object 1 class gridpositions counts 118 202 190
	origin 0 0 0
	delta  1 0 0
	delta  0 1 0
	delta  0 0 1

	object 2 class gridconnections counts 118 202 190

	object 3 class array type int rank 0 items 4444713 data follows
	1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
	…..
```
The .dx file with a surface layer:
```
	object 1 class gridpositions counts 120 204 192
	origin 0 0 0
	delta  1 0 0
	delta  0 1 0
	delta  0 0 1

	object 2 class gridconnections counts 120 204 192

	object 3 class array type int rank 0 items 4613987 data follows  
	-3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3  
	…..
```
_See: [the opendx site](http://www.opendx.org)_ for more information.

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Output File | File Path | The output .dx file path |
| Add Surface Layer | Boolean | Adds an extra layer of **Cells** around the outside of the structure and assigns them a *FeatureId* of *-3* |

## Required Geometry ##
Image 

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


