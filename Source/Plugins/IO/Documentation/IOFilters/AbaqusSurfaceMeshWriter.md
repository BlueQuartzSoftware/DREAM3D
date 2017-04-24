Export Abaqus Surface Mesh {#abaqussurfacemeshwriter}
=============

## Group (Subgroup) ##
IO (Output)

## Description ##
This **Filter** writes an Abaqus file that is a surface mesh using S3 Elements. There are 3 sections to this INP File: Nodes, Elements and Sets of Elements for each **Feature**. This file represents a triangular based mesh with 'shell' elements. No boundary elements are written. The element type selected is TRI_ELEMENT_TYPE "SFM3D3" for the triangles.

**This file is an experimental output from DREAM.3D. The user is responsible for verifying all elements in Abaqus.**

### Example Output ###     

	*HEADING
    ** File Created with DREAM3D Version 5.2.1591.1744ded
    ** There are 3 Sections to this INP File: Nodes, Elements and Sets of Elements for each grain
    ** This file represents a trianguglar based mesh. The element type selected is SFM3D3 for the triangles
    ** This file is an experimental output from DREAM3D. The user is responsible for verifying all elements in Abaqus
    ** We have selected to use a 'shell' element type currently. No boundary elements are written
    **Number of Nodes: 346727     Number of Triangles: 757562   Number of Grains: 794
    *PREPRINT,ECHO=NO,HISTORY=NO,MODEL=NO
    *Node,NSET=NALL
    1, -36.750000, 10.250000, -29.000004
    2, -36.750000, 10.500000, -29.000004
    3, -36.750000, 10.250000, -28.750004
    4, -36.750000, 10.500000, -28.750004
    5, -36.500000, 10.250000, -29.000004
    6, -36.500000, 10.250000, -28.750004
    7, -36.500000, 10.500000, -29.000004
    …..

## Parameters ##
| Name             | Type | Description |
|------------------|------|--------------------|
| Output File | File Path | The output path for the .inp file |

## Required Geometry ##
Triangle

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Face Attribute Array** | FaceLabels | int32_t | (2) | Specifies which **Features** are on either side of each **Face** |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


