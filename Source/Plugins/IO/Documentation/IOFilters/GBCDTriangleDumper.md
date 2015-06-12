Name of Filter {#gbcdtriangledumper}
=====


## EXPERIMENTAL FILTER WARNING ##

__This filter is highly experimental and under heavy development. Future versions of DREAM3D may not have this filter or output completely different files. Please do not depend on this filter for long term research use.__


## Group (Subgroup) ##
I/O Filters (Misc)


## Description ##
This filter writes Grain Boundary Character Distribution (GBCD) triangles.

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Output File | File Path | GBCD Triangles File |


## Required Geometry ##
Triangle

## Required Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Triangle | SurfaceMeshFaceLabels | Int  | (2)   | The list of all triangles (faces) in the mesh |
| Face | SurfaceMeshFaceNormals | Double | (2) | |
| Face  | SurfaceMeshFaceAreas | Double | (2) | |
| Feature  | FeatureEulerAngles | Float | (3) | |


## Created Arrays ##

None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users

