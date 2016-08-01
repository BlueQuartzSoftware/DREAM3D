Read Dx File (Feature Ids) {#dxreader}
=============

## Group (Subgroup) ##

IO (Input)

## Description ##

This **Filter** reads from a Dx file. This is a file in a format generated using the _OpenDX Source Software_ based on IBM's _Visualization Data Explorer_. **Note that the data is read according to the OpenDX specifications where the Z dimension is moving the fastest, then Y, then X.**

## Example Input##

	object 1 class gridpositions counts 6 5 4
	origin 0 0 0
	delta  1 0 0
	delta  0 1 0
	delta  0 0 1

	object 2 class gridconnections counts 6 5 4

	object 3 class array type int rank 0 items 60 data follows
	100 101 102 
	103 104 105 
	106 107 108 
	109 110 111 
	112 113 114 
	115 116 117 
	118 119 120 
	121 122 123 
	124 125 126 
	127 128 129 
	130 131 132 
	133 134 135 
	136 137 138 
	139 140 141 
	142 143 144 
	145 146 147 
	148 149 150 
	151 152 153 
	154 155 156 
	157 158 159 

	attribute "dep" string "connections"

	object "DREAM3D Generated" class feature
	component  "positions"    value 1
	component  "connections"  value 2
	component  "data"         value 3

	end

_See: [the opendx site](http://www.opendx.org)_ for more information.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Input File | File Path | The input .dx file path |
| Origin | Float (x3) | The location in space of the (0, 0, 0) coordinate |
| Resolution | Float (x3) | The resolution values (dx, dy, dz) |

## Required Geometry ##
Image 

## Required Objects ##
None

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | ImageDataContainer | N/A | N/A | Created **Data Container** name with an **Image Geometry** |
| **Attribute Matrix** | CellData | Cell | N/A | Created **Cell Attribute Matrix** name |
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


