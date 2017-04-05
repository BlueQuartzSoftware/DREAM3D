Export STL Files From Triangle Geometry {#writestlfile}
=============

## Group (Subgroup) ##
IO (Output)

## Description ##
This **Filter** will write a binary STL File for each unique **Feature** Id in the associated **Triangle** geometry. The STL files will be named with the [Feature_Id].stl. The user can designate an optional prefix for the files.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Output STL Directory | File Path | The output directory path where all of the individual STL files will be saved |
| STL File Prefix | String | Optional Prefix to use when creating the STL file names |

## Required Geometry ##
Triangle

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Face Attribute Array** | FaceLabels | int32_t | (2)  | Specifies which **Features** are on either side of each **Face** |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


