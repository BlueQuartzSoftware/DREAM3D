Read STL File {#readstlfile}
======

## Group (Subgroup) ##
IO (Input)

## Description ##

This Filter will read a binary STL File and create a **Triangle Geometry** object in memory. The STL reader is very strict to the STL specification. An explanation of the STL file format can be found on [Wikipedia](https://en.wikipedia.org/wiki/STL_(file_format)). The structure of the file is as follows:

	UINT8[80] – Header
	UINT32 – Number of triangles

	foreach triangle
	REAL32[3] – Normal vector
	REAL32[3] – Vertex 1
	REAL32[3] – Vertex 2
	REAL32[3] – Vertex 3
	UINT16 – Attribute byte count
	end

**It is very important that the "Attribute byte Count" is correct as DREAM.3D follows the specification strictly.** If you are writing an STL file be sure that the value for the "Attribute byte count" is ZERO (0). If you chose to encode additional data into a section after each triangle then be sure that the "Attribute byte count" is set correctly. DREAM.3D will obey the value located in the "Attribute byte count".

## Parameters ##

| Name | Type | Description |
|------|------|------|
| STL File | File Path  | The file path to the STL file on disk |
| Data Container | String | Created **Data Container** name |
| Face Attribute Matrix | String | Created **Cell Attribute Matrix** name |

## Required Geometry ##
Not Applicable

## Required Arrays ##
None

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Cell | FaceNormals  | double | (3) | The Normal of the triangle |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


