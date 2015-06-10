Write Abaqus Surface Mesh {#abaqussurfacemeshwriter}
=====

# EXPERIMENTAL FILTER #


## Group (Subgroup) ##
IOFilters (Output)


## Description ##
This filter writes an Abaqus file that is a surface mesh using S3 Elements.
There are 3 Sections to this INP File: Nodes, Elements and Sets of Elements for each grain.
This file represents a triangular based mesh. The element type selected is TRI_ELEMENT_TYPE "SFM3D3" for the triangles.
This file is an experimental output from DREAM3D. The user is responsible for verifying all elements in Abaqus.
We have selected to use a 'shell' element type currently. No boundary elements are written.

## Parameters ##
| Name             | Type | Description |
|------------------|------|--------------------|
| Output File | File Path | |

## Required Geometry ##
Triangle

## Required Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Triangle  | SurfaceMeshFaceLabels | int32 | (2)   | The list of all triangles (faces) in the mesh. |

## Created Arrays ##

None


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


