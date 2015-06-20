Write STL Files From Triangle Geometry {#surfacemeshtostl}
=============

## Group (Subgroup) ##
IO (Output)

## Description ##
This Filter will write a binary STL File for each unique **Feature** Id in the associated **Triangle** geometry. The STL files will be named with the [Feature_Id].stl. The user can designate an optional prefix for the files.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Output STL Directory | File Path | The output directory path where all of the individual STL files will be saved |
| STL File Prefix | String | Optional Prefix to use when creating the STL file names |
| Group Files By Ensemble | Boolean | Whether to group the individual STL files by their associated **Ensemble** |

## Required Geometry ##
Triangle

## Required Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Face | FaceLabels | Int | (2)  | Specifies to which two **Features** each triangle belongs |
| Vertex | NodeType | Int | (2) | Specifies the node type (e.g., triple line, quadruple point, etc.) for each vertex |

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


